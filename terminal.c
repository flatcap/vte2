#include "config.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <wchar.h>
#include <errno.h>

#ifdef RARXXX // Not sure these are needed /here/
#include <unistd.h>
#include <fcntl.h>
#endif

#include <glib.h>
#include <glib/gstdio.h>
#include <glib-object.h>
#include <glib/gi18n-lib.h>
#include <gobject/gmarshal.h>

#include "debug.h"
#include "terminal.h"
#include "terminal-private.h"
#include "vtepty.h"
#include "reaper.h"
#include "iso2022.h"
#include "rseq-vte.h"

#ifdef RARXXX
#include "vtepty-private.h"
#define VTE_CHILD_INPUT_PRIORITY	G_PRIORITY_DEFAULT_IDLE
#define VTE_MAX_PROCESS_TIME		100
#define VTE_ROWS			24
#define VTE_COLUMNS			80
#define VTE_SCROLLBACK_INIT		100
typedef struct _GdkPoint
{
	gint x;
	gint y;
} GdkPoint;

typedef struct _GdkRectangle
{
	gint x;
	gint y;
	gint width;
	gint height;
} GdkRectangle;
#endif

#ifdef RARXXX // copied from vte-private.h
#define VTE_DISPLAY_TIMEOUT		10
#define VTE_MAX_INPUT_READ		0x1000
#endif

#ifdef RARXXX // copied from vteseq.c
/* Typedef the handle type */
typedef void (*VteTerminalSequenceHandler) (RarTerminal *terminal, GValueArray *params);
#endif

G_DEFINE_TYPE(RarTerminal, rar_terminal, G_TYPE_OBJECT)

/* I can only find these in vte.[ch] */
#define I_(string) (g_intern_static_string(string))
#define STATIC_PARAMS (G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK | G_PARAM_STATIC_BLURB)

#ifdef ENABLE_NLS
#define P_(String) g_dgettext(GETTEXT_PACKAGE "-properties",String)
#else 
#define P_(String) (String)
#endif

#define GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), RAR_TYPE_TERMINAL, RarTerminalPrivate))

/* Forward Declarations */
gboolean vte_terminal_fork_command_full(RarTerminal *terminal, VtePtyFlags pty_flags, const char *working_directory, char **argv, char **envv, GSpawnFlags spawn_flags, GSpawnChildSetupFunc child_setup, gpointer child_setup_data, GPid *child_pid, GError **error);
void vte_terminal_set_pty(RarTerminal *terminal, int pty_master);
int vte_terminal_get_pty(RarTerminal *terminal);
VtePty * vte_terminal_get_pty_object(RarTerminal *terminal);
static gboolean vte_terminal_io_read(GIOChannel *channel, GIOCondition condition, RarTerminal *terminal);
static inline void _vte_terminal_enable_input_source (RarTerminal *terminal);
static inline gboolean need_processing (RarTerminal *terminal);
static void time_process_incoming (RarTerminal *terminal);
void vte_terminal_set_pty_object (RarTerminal *terminal, VtePty *pty);
static void vte_terminal_eof(GIOChannel *channel, RarTerminal *terminal);
static void vte_terminal_queue_eof(RarTerminal *terminal);
static void _vte_terminal_codeset_changed_cb(struct _vte_iso2022_state *state, gpointer p);
void vte_terminal_set_encoding(RarTerminal *terminal, const char *codeset);
void vte_terminal_set_emulation(RarTerminal *terminal, const char *emulation);
void vte_terminal_set_size(RarTerminal *terminal, glong columns, glong rows);
const char * vte_terminal_get_default_emulation(RarTerminal *terminal);
static void vte_terminal_set_termcap(RarTerminal *terminal, const char *path, gboolean reset);
static void vte_terminal_emit_emulation_changed(RarTerminal *terminal);
void _vte_terminal_inline_error_message(RarTerminal *terminal, const char *format, ...);
static void vte_terminal_emit_text_modified(RarTerminal *terminal);
static void vte_terminal_refresh_size(RarTerminal *terminal);

/* these static variables are guarded by the GDK mutex */
static guint process_timeout_tag = 0;
static gboolean in_process_timeout;
static guint update_timeout_tag = 0;
//static gboolean in_update_timeout;
static GList *active_terminals;
static GTimer *process_timer;

/* Enumerations */
/**
 * Object properties
 */
enum {
        PROP_0,
        PROP_PTY,
        PROP_PTY_OBJECT,
};

/**
 * Signals
 */
enum
{
	LAST_SIGNAL
};

//static guint signals[LAST_SIGNAL] = { 0 };

/* Property Helper Functions */

/* Default Signal Handlers */

/* Class Management Functions */
/**
 * rar_terminal_new
 */
GObject *
rar_terminal_new (void)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
	return g_object_new(RAR_TYPE_TERMINAL, NULL);
}

/**
 * rar_terminal_init
 */
void
rar_terminal_init (RarTerminal *terminal)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
	RarTerminalPrivate *pvt;

	/* Initialize private data. */
	pvt = terminal->pvt = GET_PRIVATE (terminal);

#ifdef RARXXX // temp object
	terminal->pvt->outer = g_malloc0 (sizeof (RarOuter));
#endif

	/* Initialize the screens and histories. */
	_vte_ring_init (pvt->outer->alternate_screen.row_data, terminal->row_count);
	pvt->outer->alternate_screen.sendrecv_mode = TRUE;
	pvt->outer->alternate_screen.status_line_contents = g_string_new(NULL);
	pvt->outer->screen = &terminal->pvt->outer->alternate_screen;
	//RARXXX _vte_terminal_set_default_attributes(terminal);

	_vte_ring_init (pvt->outer->normal_screen.row_data,  VTE_SCROLLBACK_INIT);
	pvt->outer->normal_screen.sendrecv_mode = TRUE;
	pvt->outer->normal_screen.status_line_contents = g_string_new(NULL);
	pvt->outer->screen = &terminal->pvt->outer->normal_screen;
	//RARXXX _vte_terminal_set_default_attributes(terminal);

	/* Set up I/O encodings. */
	pvt->iso2022 = _vte_iso2022_state_new(pvt->encoding,
					      &_vte_terminal_codeset_changed_cb,
					      terminal);
	pvt->incoming = NULL;
	pvt->pending = g_array_new(FALSE, TRUE, sizeof(gunichar));
	pvt->max_input_bytes = VTE_MAX_INPUT_READ;
	pvt->outgoing = _vte_buffer_new();
	pvt->outgoing_conv = VTE_INVALID_CONV;
	pvt->conv_buffer = _vte_buffer_new();
	vte_terminal_set_encoding(terminal, NULL);
	g_assert(terminal->pvt->encoding != NULL);

	/* Load the termcap data and set up the emulation. */
	//RARXXX pvt->keypad_mode = VTE_KEYMODE_NORMAL;
	//RARXXX pvt->cursor_mode = VTE_KEYMODE_NORMAL;
	//RARXXX pvt->dec_saved = g_hash_table_new(NULL, NULL);
	pvt->default_column_count = VTE_COLUMNS;
	pvt->default_row_count = VTE_ROWS;

	/* Setting the terminal type and size requires the PTY master to
	 * be set up properly first. */
        pvt->pty = NULL;
	vte_terminal_set_emulation(terminal, NULL);
	vte_terminal_set_size(terminal,
			      pvt->default_column_count,
			      pvt->default_row_count);
	pvt->pty_input_source = 0;
	pvt->pty_output_source = 0;
	pvt->pty_pid = -1;
        pvt->child_exit_status = 0;

}

/**
 * rar_terminal_dispose
 */
static void
rar_terminal_dispose (GObject *gobject)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
#if 0
	/* 
	 * In dispose, you are supposed to free all types referenced from this
	 * object which might themselves hold a reference to self. Generally,
	 * the most simple solution is to unref all members on which you own a 
	 * reference.
	 */

	/* dispose might be called multiple times, so we must guard against
	 * calling g_object_unref() on an invalid GObject.
	 */
	if (self->priv->an_object)
	{
		g_object_unref (self->priv->an_object);
		self->priv->an_object = NULL;
	}

	/* Chain up to the parent class */
	G_OBJECT_CLASS (maman_bar_parent_class)->dispose (gobject);
#endif
	//printf ("%s\n", __FUNCTION__);
}

/**
 * rar_terminal_finalize
 */
static void
rar_terminal_finalize (GObject *object)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
	printf ("%s: %p dying\n", __FUNCTION__, object);

	/*
	RarTerminal *terminal = RAR_TERMINAL (object);
	printf ("term = %p, obj = %p\n", terminal, object);
        int matches = g_signal_handlers_disconnect_matched (object, G_SIGNAL_MATCH_DATA, 0, 0, NULL, NULL, terminal);
	printf ("disconnected %d signals\n", matches);
	*/

	/* Call the inherited finalize() method. */
	G_OBJECT_CLASS(rar_terminal_parent_class)->finalize(object);
}

/**
 * rar_terminal_get_property
 */
static void
rar_terminal_get_property (GObject *object,
                           guint prop_id,
                           GValue *value,
                           GParamSpec *pspec)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
        RarTerminal *terminal = RAR_TERMINAL (object);
        RarTerminalPrivate *pvt = terminal->pvt;

	switch (prop_id)
	{
                case PROP_PTY:
                        g_value_set_int (value, pvt->pty != NULL ? vte_pty_get_fd(pvt->pty) : -1);
                        break;
                case PROP_PTY_OBJECT:
                        g_value_set_object (value, vte_terminal_get_pty_object(terminal));
                        break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
			return;
        }
}

/**
 * rar_terminal_set_property
 */
static void
rar_terminal_set_property (GObject *object,
                           guint prop_id,
                           const GValue *value,
                           GParamSpec *pspec)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
        RarTerminal *terminal = RAR_TERMINAL (object);

	switch (prop_id)
	{
                case PROP_PTY:
                        vte_terminal_set_pty (terminal, g_value_get_int (value));
                        break;
                case PROP_PTY_OBJECT:
                        vte_terminal_set_pty_object (terminal, g_value_get_object (value));
                        break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
			return;
	}
}

/**
 * rar_terminal_class_init
 */
void
rar_terminal_class_init (RarTerminalClass *klass)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
	GObjectClass *gobject_class;

	g_type_class_add_private(klass, sizeof (RarTerminalPrivate));

	gobject_class = G_OBJECT_CLASS(klass);

	/* Override some of the default handlers. */
	gobject_class->dispose      = rar_terminal_dispose;
	gobject_class->finalize     = rar_terminal_finalize;
        gobject_class->get_property = rar_terminal_get_property;
        gobject_class->set_property = rar_terminal_set_property;

	/* Register some signals of our own. */

        /**
         * RarTerminal:pty:
         *
         * The file descriptor of the master end of the terminal's PTY.
         * 
         * Since: 0.20
         *
         * Deprecated: 0.26: Use the #RarTerminal:pty-object property instead
         */
        g_object_class_install_property
                (gobject_class,
                 PROP_PTY,
                 g_param_spec_int ("pty", NULL, NULL,
                                   -1, G_MAXINT,
                                   -1,
                                   G_PARAM_READWRITE | STATIC_PARAMS));

        /**
         * RarTerminal:pty-object:
         *
         * The PTY object for the terminal.
         *
         * Since: 0.26
         */
        g_object_class_install_property
                (gobject_class,
                 PROP_PTY_OBJECT,
                 g_param_spec_object ("pty-object", NULL, NULL,
                                      VTE_TYPE_PTY,
                                      G_PARAM_READWRITE |
                                      G_PARAM_STATIC_STRINGS));

	process_timer = g_timer_new ();
}


/* Terminal Functions */
/**
 * rar_terminal_set_shell
 */
void
rar_terminal_set_shell (RarTerminal *term, const char *shell)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
	printf ("%s: shell = %s\n", __FUNCTION__, shell);
}

/**
 * rar_terminal_set_size
 */
void
rar_terminal_set_size (RarTerminal *term, int width, int height)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
	printf ("%s: size = %d,%d\n", __FUNCTION__, width, height);
}

/**
 * rar_terminal_set_scrollback
 */
void
rar_terminal_set_scrollback (RarTerminal *term, int lines)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
	printf ("%s: scrollback = %d\n", __FUNCTION__, lines);
}

/**
 * rar_terminal_set_env
 */
void
rar_terminal_set_env (RarTerminal *term, const char **env)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
	printf ("%s: env =", __FUNCTION__);
	for (; *env; env++) {
		printf (" %s", *env);
	}
	printf ("\n");
}

/**
 * rar_terminal_run_shell
 */
void
rar_terminal_run_shell (RarTerminal *term)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
	const char *command_argv[] = { "/bin/bash", NULL };
	const char *env_add[] = { "DEBUG_APP=TRUE", NULL };
	GPid child_pid = 0;

	gboolean result;

	result = vte_terminal_fork_command_full(term, VTE_PTY_DEFAULT, NULL, (char**) command_argv, (char**) env_add, G_SPAWN_SEARCH_PATH, NULL, NULL, &child_pid, NULL);
	printf ("%s: running shell: %d\n", __FUNCTION__, child_pid);
}

/**
 * rar_terminal_new_view
 */
RarView *
rar_terminal_new_view (RarTerminal *term)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
	GObject *obj = NULL;
	RarView *view = NULL;

	obj = rar_view_new();
	view = RAR_VIEW (obj);

	printf ("%s: new view %p\n", __FUNCTION__, view);
	return view;
}


// VTE
/* process incoming data without copying */
static struct _vte_incoming_chunk *free_chunks;

/**
 * vte_terminal_set_pty:
 * @terminal: a #RarTerminal
 * @pty_master: a file descriptor of the master end of a PTY, or %-1
 *
 * Attach an existing PTY master side to the terminal widget.  Use
 * instead of vte_terminal_fork_command() or vte_terminal_forkpty().
 *
 * Since: 0.12.1
 *
 * Deprecated: 0.26: Use vte_pty_new_foreign() and vte_terminal_set_pty_object()
 */
void
vte_terminal_set_pty(RarTerminal *terminal, int pty_master)
{
        VtePty *pty;

        if (pty_master == -1) {
                vte_terminal_set_pty_object(terminal, NULL);
                return;
        }

        pty = vte_pty_new_foreign(pty_master, NULL);
        if (pty == NULL)
                return;

        vte_terminal_set_pty_object(terminal, pty);
        g_object_unref(pty);
}

/**
 * vte_terminal_get_pty:
 * @terminal: a #RarTerminal
 *
 * Returns the file descriptor of the master end of @terminal's PTY.
 *
 * Return value: the file descriptor, or -1 if the terminal has no PTY.
 *
 * Since: 0.20
 *
 * Deprecated: 0.26: Use vte_terminal_get_pty_object() and vte_pty_get_fd()
 */
int
vte_terminal_get_pty(RarTerminal *terminal)
{
        RarTerminalPrivate *pvt;

        g_return_val_if_fail (RAR_IS_TERMINAL (terminal), -1);

        pvt = terminal->pvt;
        if (pvt->pty != NULL)
                return vte_pty_get_fd(pvt->pty);

        return -1;
}

/**
 * vte_terminal_get_pty_object:
 * @terminal: a #RarTerminal
 *
 * Returns the #VtePty of @terminal.
 *
 * Returns: (transfer none): a #VtePty, or %NULL
 *
 * Since: 0.26
 */
VtePty *
vte_terminal_get_pty_object(RarTerminal *terminal)
{
        g_return_val_if_fail (RAR_IS_TERMINAL (terminal), NULL);

        return terminal->pvt->pty;
}


/**
 * get_chunk
 */
/*static*/ struct _vte_incoming_chunk *
get_chunk (void)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
	struct _vte_incoming_chunk *chunk = NULL;
	if (free_chunks) {
		chunk = free_chunks;
		free_chunks = free_chunks->next;
	}
	if (chunk == NULL) {
		chunk = g_new (struct _vte_incoming_chunk, 1);
	}
	chunk->next = NULL;
	chunk->len = 0;
	return chunk;
}

/**
 * release_chunk
 */
static void
release_chunk (struct _vte_incoming_chunk *chunk)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
	chunk->next = free_chunks;
	chunk->len = free_chunks ? free_chunks->len + 1 : 0;
	free_chunks = chunk;
}

/**
 * prune_chunks
 */
/*static*/ void
prune_chunks (guint len)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
	struct _vte_incoming_chunk *chunk = NULL;
	if (len && free_chunks != NULL) {
	    if (free_chunks->len > len) {
		struct _vte_incoming_chunk *last;
		chunk = free_chunks;
		while (free_chunks->len > len) {
		    last = free_chunks;
		    free_chunks = free_chunks->next;
		}
		last->next = NULL;
	    }
	} else {
	    chunk = free_chunks;
	    free_chunks = NULL;
	}
	while (chunk != NULL) {
		struct _vte_incoming_chunk *next = chunk->next;
		g_free (chunk);
		chunk = next;
	}
}

/**
 * _vte_incoming_chunks_release
 */
static void
_vte_incoming_chunks_release (struct _vte_incoming_chunk *chunk)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
	while (chunk) {
		struct _vte_incoming_chunk *next = chunk->next;
		release_chunk (chunk);
		chunk = next;
	}
}

/**
 * _vte_incoming_chunks_length
 */
static gsize
_vte_incoming_chunks_length (struct _vte_incoming_chunk *chunk)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
	gsize len = 0;
	while (chunk) {
		len += chunk->len;
		chunk = chunk->next;
	}
	return len;
}

/**
 * _vte_incoming_chunks_count
 */
static gsize
_vte_incoming_chunks_count (struct _vte_incoming_chunk *chunk)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
	gsize cnt = 0;
	while (chunk) {
		cnt ++;
		chunk = chunk->next;
	}
	return cnt;
}

/**
 * _vte_incoming_chunk
 */
/*static*/ struct _vte_incoming_chunk *
_vte_incoming_chunks_reverse (struct _vte_incoming_chunk *chunk)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
	struct _vte_incoming_chunk *prev = NULL;
	while (chunk) {
		struct _vte_incoming_chunk *next = chunk->next;
		chunk->next = prev;
		prev = chunk;
		chunk = next;
	}
	return prev;
}


/**
 * vte_terminal_process_incoming
 * Process incoming data, first converting it to unicode characters, and then
 * processing control sequences.
 */
static void
vte_terminal_process_incoming (RarTerminal *terminal)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
	RarOuter *outer;
	//RARXXX VteVisualPosition cursor;
	//RARXXX gboolean cursor_visible;
	GdkPoint bbox_topleft, bbox_bottomright;
	gunichar *wbuf, c;
	//RARXXX long wcount, start, delta;
	long wcount, start;
	//RARXXX gboolean leftovers, modified, bottom, again;
	gboolean leftovers, modified, again;
	gboolean invalidated_text;
	GArray *unichars;
	struct _vte_incoming_chunk *chunk, *next_chunk, *achunk = NULL;

	_vte_debug_print(VTE_DEBUG_IO,
			"Handler processing %"G_GSIZE_FORMAT" bytes over %"G_GSIZE_FORMAT" chunks + %d bytes pending.\n",
			_vte_incoming_chunks_length(terminal->pvt->incoming),
			_vte_incoming_chunks_count(terminal->pvt->incoming),
			terminal->pvt->pending->len);
	_vte_debug_print (VTE_DEBUG_WORK, "(");

	outer = terminal->pvt->outer;

#ifndef RARXXX
	delta = outer->screen->scroll_delta;
	bottom = outer->screen->insert_delta == delta;

	/* Save the current cursor position. */
	cursor = outer->screen->cursor_current;
	cursor_visible = terminal->pvt->cursor_visible;
#endif

	/* We should only be called when there's data to process. */
	g_assert(terminal->pvt->incoming ||
		 (terminal->pvt->pending->len > 0));

	/* Convert the data into unicode characters. */
	unichars = terminal->pvt->pending;
	for (chunk = _vte_incoming_chunks_reverse (terminal->pvt->incoming);
			chunk != NULL;
			chunk = next_chunk) {
		gsize processed;
		next_chunk = chunk->next;
		if (chunk->len == 0) {
			goto skip_chunk;
		}

		processed = _vte_iso2022_process(terminal->pvt->iso2022,
				chunk->data, chunk->len,
				unichars);
		if (G_UNLIKELY (processed != chunk->len)) {
			/* shuffle the data about */
			g_memmove (chunk->data, chunk->data + processed,
					chunk->len - processed);
			chunk->len = chunk->len - processed;
			processed = sizeof (chunk->data) - chunk->len;
			if (processed != 0 && next_chunk !=  NULL) {
				if (next_chunk->len <= processed) {
					/* consume it entirely */
					memcpy (chunk->data + chunk->len,
							next_chunk->data,
							next_chunk->len);
					chunk->len += next_chunk->len;
					chunk->next = next_chunk->next;
					release_chunk (next_chunk);
				} else {
					/* next few bytes */
					memcpy (chunk->data + chunk->len,
							next_chunk->data,
							processed);
					chunk->len += processed;
					g_memmove (next_chunk->data,
							next_chunk->data + processed,
							next_chunk->len - processed);
					next_chunk->len -= processed;
				}
				next_chunk = chunk; /* repeat */
			} else {
				break;
			}
		} else {
skip_chunk:
			/* cache the last chunk */
			if (achunk) {
				release_chunk (achunk);
			}
			achunk = chunk;
		}
	}
	if (achunk) {
		if (chunk != NULL) {
			release_chunk (achunk);
		} else {
			chunk = achunk;
			chunk->next = NULL;
			chunk->len = 0;
		}
	}
	terminal->pvt->incoming = chunk;

	/* Compute the number of unicode characters we got. */
	wbuf = &g_array_index(unichars, gunichar, 0);
	wcount = unichars->len;

	/* Try initial substrings. */
	start = 0;
	modified = leftovers = again = FALSE;
	invalidated_text = FALSE;

	bbox_bottomright.x = bbox_bottomright.y = -G_MAXINT;
	bbox_topleft.x = bbox_topleft.y = G_MAXINT;

	while (start < wcount && !leftovers) {
		const char *match = NULL;
		GQuark quark;
		const gunichar *next;
		GValueArray *params = NULL;

		/* Try to match any control sequences. */
		_vte_matcher_match(terminal->pvt->matcher,
				   &wbuf[start],
				   wcount - start,
				   &match,
				   &next,
				   &quark,
				   &params);
		/* We're in one of three possible situations now.
		 * First, the match string is a non-empty string and next
		 * points to the first character which isn't part of this
		 * sequence. */
		if ((match != NULL) && (match[0] != '\0')) {
			/* Call the right sequence handler for the requested
			 * behavior. */
			_vte_terminal_handle_sequence(terminal,
						      match,
						      quark,
						      params);
			/* Skip over the proper number of unicode chars. */
			start = (next - wbuf);
			modified = TRUE;

#ifndef RARXXX
			/* if we have moved during the sequence handler, restart the bbox */
			if (invalidated_text &&
					(outer->screen->cursor_current.col > bbox_bottomright.x + VTE_CELL_BBOX_SLACK ||
					 outer->screen->cursor_current.col < bbox_topleft.x - VTE_CELL_BBOX_SLACK     ||
					 outer->screen->cursor_current.row > bbox_bottomright.y + VTE_CELL_BBOX_SLACK ||
					 outer->screen->cursor_current.row < bbox_topleft.y - VTE_CELL_BBOX_SLACK)) {
				/* Clip off any part of the box which isn't already on-screen. */
				bbox_topleft.x = MAX(bbox_topleft.x, 0);
				bbox_topleft.y = MAX(bbox_topleft.y, delta);
				bbox_bottomright.x = MIN(bbox_bottomright.x,
						terminal->column_count);
				/* lazily apply the +1 to the cursor_row */
				bbox_bottomright.y = MIN(bbox_bottomright.y + 1,
						delta + terminal->row_count);

				_vte_invalidate_cells(terminal,
						bbox_topleft.x,
						bbox_bottomright.x - bbox_topleft.x,
						bbox_topleft.y,
						bbox_bottomright.y - bbox_topleft.y);

				invalidated_text = FALSE;
				bbox_bottomright.x = bbox_bottomright.y = -G_MAXINT;
				bbox_topleft.x = bbox_topleft.y = G_MAXINT;
			}
#endif
		} else
		/* Second, we have a NULL match, and next points to the very
		 * next character in the buffer.  Insert the character which
		 * we're currently examining into the screen. */
		if (match == NULL) {
			c = wbuf[start];
			/* If it's a control character, permute the order, per
			 * vttest. */
			if ((c != *next) &&
			    ((*next & 0x1f) == *next) &&
			    (start + 1 < next - wbuf)) {
				const gunichar *tnext = NULL;
				const char *tmatch = NULL;
				GQuark tquark = 0;
				gunichar ctrl;
				int i;
				/* We don't want to permute it if it's another
				 * control sequence, so check if it is. */
				_vte_matcher_match(terminal->pvt->matcher,
						   next,
						   wcount - (next - wbuf),
						   &tmatch,
						   &tnext,
						   &tquark,
						   NULL);
				/* We only do this for non-control-sequence
				 * characters and random garbage. */
				if (tnext == next + 1) {
					/* Save the control character. */
					ctrl = *next;
					/* Move everything before it up a
					 * slot.  */
					for (i = next - wbuf; i > start; i--) {
						wbuf[i] = wbuf[i - 1];
					}
					/* Move the control character to the
					 * front. */
					wbuf[i] = ctrl;
					goto next_match;
				}
			}
			_VTE_DEBUG_IF(VTE_DEBUG_PARSE) {
				gunichar cc = c & ~VTE_ISO2022_ENCODED_WIDTH_MASK;
				if (cc > 255) {
					g_printerr("U+%04lx\n", (long) cc);
				} else {
					if (cc > 127) {
						g_printerr("%ld = ",
								(long) cc);
					}
					if (cc < 32) {
						g_printerr("^%lc\n",
								(wint_t)cc + 64);
					} else {
						g_printerr("`%lc'\n",
								(wint_t)cc);
					}
				}
			}

#ifndef RARXXX
			bbox_topleft.x = MIN(bbox_topleft.x,
					outer->screen->cursor_current.col);
			bbox_topleft.y = MIN(bbox_topleft.y,
					outer->screen->cursor_current.row);
#endif

			/* Insert the character. */
			if (G_UNLIKELY (_vte_terminal_insert_char(terminal, c,
						 FALSE, FALSE))) {
#ifndef RARXXX
				/* line wrapped, correct bbox */
				if (invalidated_text &&
						(outer->screen->cursor_current.col > bbox_bottomright.x + VTE_CELL_BBOX_SLACK	||
						 outer->screen->cursor_current.col < bbox_topleft.x - VTE_CELL_BBOX_SLACK	||
						 outer->screen->cursor_current.row > bbox_bottomright.y + VTE_CELL_BBOX_SLACK	||
						 outer->screen->cursor_current.row < bbox_topleft.y - VTE_CELL_BBOX_SLACK)) {
					/* Clip off any part of the box which isn't already on-screen. */
					bbox_topleft.x = MAX(bbox_topleft.x, 0);
					bbox_topleft.y = MAX(bbox_topleft.y, delta);
					bbox_bottomright.x = MIN(bbox_bottomright.x,
							terminal->column_count);
					/* lazily apply the +1 to the cursor_row */
					bbox_bottomright.y = MIN(bbox_bottomright.y + 1,
							delta + terminal->row_count);

					_vte_invalidate_cells(terminal,
							bbox_topleft.x,
							bbox_bottomright.x - bbox_topleft.x,
							bbox_topleft.y,
							bbox_bottomright.y - bbox_topleft.y);
					bbox_bottomright.x = bbox_bottomright.y = -G_MAXINT;
					bbox_topleft.x = bbox_topleft.y = G_MAXINT;

				}
				bbox_topleft.x = MIN(bbox_topleft.x, 0);
				bbox_topleft.y = MIN(bbox_topleft.y,
						outer->screen->cursor_current.row);
#endif
			}
#ifndef RARXXX
			/* Add the cells over which we have moved to the region
			 * which we need to refresh for the user. */
			bbox_bottomright.x = MAX(bbox_bottomright.x,
					outer->screen->cursor_current.col);
			/* cursor_current.row + 1 (defer until inv.) */
			bbox_bottomright.y = MAX(bbox_bottomright.y,
					outer->screen->cursor_current.row);
#endif
			invalidated_text = TRUE;

			/* We *don't* emit flush pending signals here. */
			modified = TRUE;
			start++;
		} else {
			/* Case three: the read broke in the middle of a
			 * control sequence, so we're undecided with no more
			 * data to consult. If we have data following the
			 * middle of the sequence, then it's just garbage data,
			 * and for compatibility, we should discard it. */
			if (wbuf + wcount > next) {
				_vte_debug_print(VTE_DEBUG_PARSE,
						"Invalid control "
						"sequence, discarding %ld "
						"characters.\n",
						(long)(next - (wbuf + start)));
				/* Discard. */
				start = next - wbuf + 1;
			} else {
				/* Pause processing here and wait for more
				 * data before continuing. */
				leftovers = TRUE;
			}
		}

#ifdef VTE_DEBUG
		/* Some safety checks: ensure the visible parts of the buffer
		 * are all in the buffer. */
		g_assert(outer->screen->insert_delta >=
			 _vte_ring_delta(outer->screen->row_data));
		/* The cursor shouldn't be above or below the addressable
		 * part of the display buffer. */
		g_assert(outer->screen->cursor_current.row >= outer->screen->insert_delta);
#endif

next_match:
		if (G_LIKELY(params != NULL)) {
			/* Free any parameters we don't care about any more. */
			_vte_matcher_free_params_array(terminal->pvt->matcher,
					params);
		}
	}

	/* Remove most of the processed characters. */
	if (start < wcount) {
		g_array_remove_range(terminal->pvt->pending, 0, start);
	} else {
		g_array_set_size(terminal->pvt->pending, 0);
		/* If we're out of data, we needn't pause to let the
		 * controlling application respond to incoming data, because
		 * the main loop is already going to do that. */
	}

	if (modified) {
		/* Keep the cursor on-screen if we scroll on output, or if
		 * we're currently at the bottom of the buffer. */
#ifndef RARXXX
		_vte_terminal_update_insert_delta(terminal);
		if (terminal->pvt->scroll_on_output || bottom) {
			vte_terminal_maybe_scroll_to_bottom(terminal);
		}
		/* Deselect the current selection if its contents are changed
		 * by this insertion. */
		if (terminal->pvt->has_selection) {
			char *selection;
			selection =
			vte_terminal_get_text_range(terminal,
						    terminal->pvt->selection_start.row,
						    0,
						    terminal->pvt->selection_end.row,
						    terminal->column_count,
						    vte_cell_is_selected,
						    NULL,
						    NULL);
			if ((selection == NULL) || (terminal->pvt->selection == NULL) ||
			    (strcmp(selection, terminal->pvt->selection) != 0)) {
				vte_terminal_deselect_all(terminal);
			}
			g_free(selection);
		}
#endif
	}

#ifndef RARXXX
	if (modified || (screen != terminal->pvt->screen)) {
		/* Signal that the visible contents changed. */
		_vte_terminal_queue_contents_changed(terminal);
	}
#endif

	//RARXXX vte_terminal_emit_pending_signals (terminal);

#ifndef RARXXX
	if (invalidated_text) {
		/* Clip off any part of the box which isn't already on-screen. */
		bbox_topleft.x = MAX(bbox_topleft.x, 0);
		bbox_topleft.y = MAX(bbox_topleft.y, delta);
		bbox_bottomright.x = MIN(bbox_bottomright.x,
				terminal->column_count);
		/* lazily apply the +1 to the cursor_row */
		bbox_bottomright.y = MIN(bbox_bottomright.y + 1,
				delta + terminal->row_count);

		_vte_invalidate_cells(terminal,
				bbox_topleft.x,
				bbox_bottomright.x - bbox_topleft.x,
				bbox_topleft.y,
				bbox_bottomright.y - bbox_topleft.y);
	}

	if ((cursor.col != terminal->pvt->outer->screen->cursor_current.col) ||
	    (cursor.row != terminal->pvt->outer->screen->cursor_current.row)) {
		/* invalidate the old and new cursor positions */
		if (cursor_visible)
			_vte_invalidate_cell(terminal, cursor.col, cursor.row);
		_vte_invalidate_cursor_once(terminal, FALSE);
		/* Signal that the cursor moved. */
		vte_terminal_queue_cursor_moved(terminal);
	} else if (cursor_visible != terminal->pvt->cursor_visible) {
		_vte_invalidate_cell(terminal, cursor.col, cursor.row);
	}

	/* Tell the input method where the cursor is. */
	if (GTK_WIDGET_REALIZED(terminal)) {
		GdkRectangle rect;
		rect.x = terminal->pvt->outer->screen->cursor_current.col *
			 terminal->char_width + terminal->pvt->inner_border.left;
		rect.width = terminal->char_width;
		rect.y = (terminal->pvt->outer->screen->cursor_current.row - delta) *
			 terminal->char_height + terminal->pvt->inner_border.top;
		rect.height = terminal->char_height;
		gtk_im_context_set_cursor_location(terminal->pvt->im_context,
						   &rect);
	}
#endif

	_vte_debug_print (VTE_DEBUG_WORK, ")");
	_vte_debug_print (VTE_DEBUG_IO,
			"%ld chars and %ld bytes in %"G_GSIZE_FORMAT" chunks left to process.\n",
			(long) unichars->len,
			(long) _vte_incoming_chunks_length(terminal->pvt->incoming),
			_vte_incoming_chunks_count(terminal->pvt->incoming));
}

/**
 * _vte_terminal_disconnect_pty_read
 */
static void
_vte_terminal_disconnect_pty_read (RarTerminal *terminal)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
	if (terminal->pvt->pty_input_source != 0) {
		_vte_debug_print (VTE_DEBUG_IO, "disconnecting poll of vte_terminal_io_read\n");
		g_source_remove(terminal->pvt->pty_input_source);
		terminal->pvt->pty_input_source = 0;
	}
}

/**
 * _vte_terminal_disconnect_pty_write
 */
static void
_vte_terminal_disconnect_pty_write (RarTerminal *terminal)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
	if (terminal->pvt->pty_output_source != 0) {
		_vte_debug_print (VTE_DEBUG_IO, "disconnecting poll of vte_terminal_io_write\n");

		g_source_remove(terminal->pvt->pty_output_source);
		terminal->pvt->pty_output_source = 0;
	}
}

/**
 * remove_from_active_list
 */
static void
remove_from_active_list (RarTerminal *terminal)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
#if 0
	if (terminal->pvt->active != NULL
			&& terminal->pvt->update_regions == NULL) {
		_vte_debug_print(VTE_DEBUG_TIMEOUT,
			"Removing terminal from active list\n");
		active_terminals = g_list_delete_link (active_terminals,
				terminal->pvt->active);
		terminal->pvt->active = NULL;

		if (active_terminals == NULL) {
			if (in_process_timeout == FALSE &&
					process_timeout_tag != 0) {
				_vte_debug_print(VTE_DEBUG_TIMEOUT,
						"Removing process timeout\n");
				g_source_remove (process_timeout_tag);
				process_timeout_tag = 0;
			}
			if (in_update_timeout == FALSE &&
					update_timeout_tag != 0) {
				_vte_debug_print(VTE_DEBUG_TIMEOUT,
						"Removing update timeout\n");
				g_source_remove (update_timeout_tag);
				update_timeout_tag = 0;
			}
		}
	}
#endif
}

/**
 * vte_terminal_stop_processing
 */
static void
vte_terminal_stop_processing (RarTerminal *terminal)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
	remove_from_active_list (terminal);
}

/**
 * _vte_terminal_setup_utf8
 */
static void
_vte_terminal_setup_utf8 (RarTerminal *terminal)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
#ifndef RARXXX
        RarTerminalPrivate *pvt = terminal->pvt;
        GError *error = NULL;

        if (!vte_pty_set_utf8(pvt->pty,
                              strcmp(terminal->pvt->encoding, "UTF-8") == 0,
                              &error)) {
                g_warning ("Failed to set UTF8 mode: %s\n", error->message);
                g_error_free (error);
        }
#endif
}

/**
 * _vte_terminal_feed_chunks
 */
/*static*/ void
_vte_terminal_feed_chunks (RarTerminal *terminal, struct _vte_incoming_chunk *chunks)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
	struct _vte_incoming_chunk *last;

	_vte_debug_print(VTE_DEBUG_IO, "Feed %"G_GSIZE_FORMAT" bytes, in %"G_GSIZE_FORMAT" chunks.\n",
			_vte_incoming_chunks_length(chunks),
			_vte_incoming_chunks_count(chunks));

	for (last = chunks; last->next != NULL; last = last->next) ;
	last->next = terminal->pvt->incoming;
	terminal->pvt->incoming = chunks;
}

/**
 * vte_terminal_is_processing
 */
static inline gboolean
vte_terminal_is_processing (RarTerminal *terminal)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
	return terminal->pvt->active != NULL;
}

/**
 * process_timeout
 * This function is called after DISPLAY_TIMEOUT ms.
 * It makes sure initial output is never delayed by more than DISPLAY_TIMEOUT
 */
static gboolean
process_timeout (gpointer data)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
	GList *l, *next;
	gboolean again;

	//RARXXX GDK_THREADS_ENTER();

	in_process_timeout = TRUE;

	_vte_debug_print (VTE_DEBUG_WORK, "<");
	_vte_debug_print (VTE_DEBUG_TIMEOUT,
			"Process timeout:  %d active\n",
			g_list_length (active_terminals));

	for (l = active_terminals; l != NULL; l = next) {
		RarTerminal *terminal = l->data;
		gboolean active = FALSE;

		next = g_list_next (l);

		if (l != active_terminals) {
			_vte_debug_print (VTE_DEBUG_WORK, "T");
		}
		if (terminal->pvt->pty_channel != NULL) {
			if (terminal->pvt->pty_input_active ||
					terminal->pvt->pty_input_source == 0) {
				terminal->pvt->pty_input_active = FALSE;
				vte_terminal_io_read (terminal->pvt->pty_channel,
						G_IO_IN, terminal);
			}
			_vte_terminal_enable_input_source (terminal);
		}
		if (need_processing (terminal)) {
			active = TRUE;
			if (VTE_MAX_PROCESS_TIME) {
				time_process_incoming (terminal);
			} else {
				vte_terminal_process_incoming(terminal);
			}
			terminal->pvt->input_bytes = 0;
		} else {
			//RARXXX vte_terminal_emit_pending_signals (terminal);
		}
		if (!active && terminal->pvt->update_regions == NULL) {
			if (terminal->pvt->active != NULL) {
				_vte_debug_print(VTE_DEBUG_TIMEOUT,
						"Removing terminal from active list [process]\n");
				active_terminals = g_list_delete_link (
						active_terminals,
						terminal->pvt->active);
				terminal->pvt->active = NULL;
			}
		}
	}

	_vte_debug_print (VTE_DEBUG_WORK, ">");

	if (active_terminals && update_timeout_tag == 0) {
		again = TRUE;
	} else {
		_vte_debug_print(VTE_DEBUG_TIMEOUT,
				"Stoping process timeout\n");
		process_timeout_tag = 0;
		again = FALSE;
	}

	in_process_timeout = FALSE;

	//RARXXX GDK_THREADS_LEAVE();

	if (again) {
		/* Force us to relinquish the CPU as the child is running
		 * at full tilt and making us run to keep up...
		 */
		g_usleep (0);
	} else if (update_timeout_tag == 0) {
		/* otherwise free up memory used to capture incoming data */
		prune_chunks (10);
	}

	return again;
}

/**
 * vte_terminal_add_process_timeout
 */
static void
vte_terminal_add_process_timeout (RarTerminal *terminal)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
	_vte_debug_print(VTE_DEBUG_TIMEOUT,
			"Adding terminal to active list\n");
	terminal->pvt->active = active_terminals =
		g_list_prepend (active_terminals, terminal);
	if (update_timeout_tag == 0 &&
			process_timeout_tag == 0) {
		_vte_debug_print(VTE_DEBUG_TIMEOUT,
				"Starting process timeout\n");
		process_timeout_tag =
			g_timeout_add (VTE_DISPLAY_TIMEOUT,
					process_timeout, NULL);
	}
}

/**
 * vte_terminal_eof
 * Handle an EOF from the client.
 */
static void
vte_terminal_eof(GIOChannel *channel, RarTerminal *terminal)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
        GObject *object = G_OBJECT(terminal);

        g_object_freeze_notify(object);

        vte_terminal_set_pty_object(terminal, NULL);

	/* Emit a signal that we read an EOF. */
	vte_terminal_queue_eof(terminal);

        g_object_thaw_notify(object);
}

/**
 * vte_terminal_emit_eof
 */
static gboolean
vte_terminal_emit_eof(RarTerminal *terminal)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
	_vte_debug_print(VTE_DEBUG_SIGNALS,
			"Emitting `eof'.\n");
	//RARXXX GDK_THREADS_ENTER ();
	g_signal_emit_by_name(terminal, "eof");
	//RARXXX GDK_THREADS_LEAVE ();

	return FALSE;
}

/**
 * vte_terminal_queue_eof
 * Emit a "eof" signal.
 */
static void
vte_terminal_queue_eof(RarTerminal *terminal)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
	_vte_debug_print(VTE_DEBUG_SIGNALS,
			"Queueing `eof'.\n");
	g_idle_add_full (G_PRIORITY_HIGH,
		(GSourceFunc) vte_terminal_emit_eof,
		g_object_ref (terminal),
		g_object_unref);
}

/**
 * vte_terminal_io_read:
 * Read and handle data from the child.
 */
static gboolean
vte_terminal_io_read (GIOChannel *channel,
		     GIOCondition condition,
		     RarTerminal *terminal)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
	int err = 0;
	gboolean eof, again = TRUE;

	_vte_debug_print (VTE_DEBUG_WORK, ".");

	/* Check for end-of-file. */
	eof = condition & G_IO_HUP;

	/* Read some data in from this channel. */
	if (condition & G_IO_IN) {
		struct _vte_incoming_chunk *chunk, *chunks = NULL;
		const int fd = g_io_channel_unix_get_fd (channel);
		guchar *bp;
		int rem, len;
		guint bytes, max_bytes;

		/* Limit the amount read between updates, so as to
		 * 1. maintain fairness between multiple terminals;
		 * 2. prevent reading the entire output of a command in one
		 *    pass, i.e. we always try to refresh the terminal ~40Hz.
		 *    See time_process_incoming() where we estimate the
		 *    maximum number of bytes we can read/process in between
		 *    updates.
		 */
		max_bytes = terminal->pvt->active ?
		            g_list_length (active_terminals) - 1 : 0;
		if (max_bytes) {
			max_bytes = terminal->pvt->max_input_bytes / max_bytes;
		} else {
			max_bytes = VTE_MAX_INPUT_READ;
		}
		bytes = terminal->pvt->input_bytes;

		chunk = terminal->pvt->incoming;
		do {
			if (!chunk || chunk->len >= 3*sizeof (chunk->data)/4) {
				chunk = get_chunk ();
				chunk->next = chunks;
				chunks = chunk;
			}
			rem = sizeof (chunk->data) - chunk->len;
			bp = chunk->data + chunk->len;
			len = 0;
			do {
				int ret = read (fd, bp, rem);
				switch (ret){
					case -1:
						err = errno;
						goto out;
					case 0:
						eof = TRUE;
						goto out;
					default:
						bp += ret;
						rem -= ret;
						len += ret;
						break;
				}
			} while (rem);
out:
			chunk->len += len;
			bytes += len;
		} while (bytes < max_bytes &&
		         chunk->len == sizeof (chunk->data));
		if (chunk->len == 0 && chunk == chunks) {
			chunks = chunks->next;
			release_chunk (chunk);
		}

		if (chunks != NULL) {
			_vte_terminal_feed_chunks (terminal, chunks);
		}
		if (!vte_terminal_is_processing (terminal)) {
			//RARXXX GDK_THREADS_ENTER ();
			vte_terminal_add_process_timeout (terminal);
			//RARXXX GDK_THREADS_LEAVE ();
		}
		terminal->pvt->pty_input_active = len != 0;
		terminal->pvt->input_bytes = bytes;
		again = bytes < max_bytes;

		_vte_debug_print (VTE_DEBUG_IO, "read %d/%d bytes, again? %s, active? %s\n",
				bytes, max_bytes,
				again ? "yes" : "no",
				terminal->pvt->pty_input_active ? "yes" : "no");
	}

	/* Error? */
	switch (err) {
		case 0: /* no error */
			break;
		case EIO: /* Fake an EOF. */
			eof = TRUE;
			break;
		case EAGAIN:
		case EBUSY: /* do nothing */
			break;
		default:
			/* Translators: %s is replaced with error message returned by strerror(). */
			g_warning (_("Error reading from child: " "%s."),
					g_strerror (err));
			break;
	}

	/* If we detected an eof condition, signal one. */
	if (eof) {
		/* potential deadlock ... */
		if (!vte_terminal_is_processing (terminal)) {
			//RARXXX GDK_THREADS_ENTER ();
			vte_terminal_eof (channel, terminal);
			//RARXXX GDK_THREADS_LEAVE ();
		} else {
			vte_terminal_eof (channel, terminal);
		}

		again = FALSE;
	}

	return again;
}

/**
 * mark_input_source_invalid
 */
static void
mark_input_source_invalid (RarTerminal *terminal)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
	_vte_debug_print (VTE_DEBUG_IO, "removed poll of vte_terminal_io_read\n");
	terminal->pvt->pty_input_source = 0;
}

/**
 * _vte_terminal_connect_pty_read
 */
static void
_vte_terminal_connect_pty_read (RarTerminal *terminal)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
	if (terminal->pvt->pty_channel == NULL) {
		return;
	}

	if (terminal->pvt->pty_input_source == 0) {
		_vte_debug_print (VTE_DEBUG_IO, "polling vte_terminal_io_read\n");
		terminal->pvt->pty_input_source =
			g_io_add_watch_full(terminal->pvt->pty_channel,
					    VTE_CHILD_INPUT_PRIORITY,
					    G_IO_IN | G_IO_HUP,
					    (GIOFunc) vte_terminal_io_read,
					    terminal,
					    (GDestroyNotify) mark_input_source_invalid);
	}
}

/**
 * vte_terminal_set_pty_object:
 * @terminal: a #RarTerminal
 * @pty: (allow-none): a #VtePty, or %NULL
 *
 * Sets @pty as the PTY to use in @terminal.
 * Use %NULL to unset the PTY.
 *
 * Since: 0.26.
 */
void
vte_terminal_set_pty_object (RarTerminal *terminal,
                            VtePty *pty)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
        RarTerminalPrivate *pvt;
        GObject *object;
        long flags;
        int pty_master;

        g_return_if_fail(RAR_IS_TERMINAL(terminal));
        g_return_if_fail(pty == NULL || VTE_IS_PTY(pty));

        pvt = terminal->pvt;
        if (pvt->pty == pty)
                return;

        object = G_OBJECT(terminal);

        g_object_freeze_notify(object);

        if (pvt->pty != NULL) {
                _vte_terminal_disconnect_pty_read(terminal);
                _vte_terminal_disconnect_pty_write(terminal);

                if (terminal->pvt->pty_channel != NULL) {
                        g_io_channel_unref (terminal->pvt->pty_channel);
                        pvt->pty_channel = NULL;
                }

		/* Take one last shot at processing whatever data is pending,
		 * then flush the buffers in case we're about to run a new
		 * command, disconnecting the timeout. */
		if (terminal->pvt->incoming != NULL) {
			vte_terminal_process_incoming(terminal);
			_vte_incoming_chunks_release (terminal->pvt->incoming);
			terminal->pvt->incoming = NULL;
			terminal->pvt->input_bytes = 0;
		}
		g_array_set_size(terminal->pvt->pending, 0);
		vte_terminal_stop_processing (terminal);

		/* Clear the outgoing buffer as well. */
		_vte_buffer_clear(terminal->pvt->outgoing);

                vte_pty_close(pvt->pty);
                g_object_unref(pvt->pty);
                pvt->pty = NULL;
        }

        if (pty == NULL) {
                pvt->pty = NULL;
                g_object_notify(object, "pty");
                g_object_notify(object, "pty-object");
                g_object_thaw_notify(object);
                return;
        }

        pvt->pty = g_object_ref(pty);
        pty_master = vte_pty_get_fd(pvt->pty);

        pvt->pty_channel = g_io_channel_unix_new (pty_master);
        g_io_channel_set_close_on_unref (pvt->pty_channel, FALSE);

        /* FIXMEchpe: vte_pty_open_unix98 does the inverse ... */
        /* Set the pty to be non-blocking. */
        flags = fcntl(pty_master, F_GETFL);
        if ((flags & O_NONBLOCK) == 0) {
                fcntl(pty_master, F_SETFL, flags | O_NONBLOCK);
        }

#ifndef RARXXX
        vte_terminal_set_size(terminal,
                              terminal->column_count,
                              terminal->row_count);
#endif

        _vte_terminal_setup_utf8 (terminal);

        /* Open channels to listen for input on. */
        _vte_terminal_connect_pty_read (terminal);

        g_object_notify(object, "pty");
        g_object_notify(object, "pty-object");

        g_object_thaw_notify(object);
}

/**
 * vte_terminal_emit_child_exited
 * Emit a "child-exited" signal.
 */
static void
vte_terminal_emit_child_exited (RarTerminal *terminal)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
	_vte_debug_print(VTE_DEBUG_SIGNALS,
			"Emitting `child-exited'.\n");
	g_signal_emit_by_name(terminal, "child-exited");
}

/**
 * vte_terminal_catch_child_exited
 * Catch a VteReaper child-exited signal, and if it matches the one we're
 * looking for, emit one of our own.
 */
static void
vte_terminal_catch_child_exited (VteReaper *reaper, int pid, int status,
				RarTerminal *terminal)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
	if (pid == terminal->pvt->pty_pid) {
                GObject *object = G_OBJECT(terminal);

                g_object_ref(object);
                g_object_freeze_notify(object);

		_VTE_DEBUG_IF (VTE_DEBUG_LIFECYCLE) {
			g_printerr ("Child[%d] exited with status %d\n",
					pid, status);
#ifdef HAVE_SYS_WAIT_H
			if (WIFEXITED (status)) {
				g_printerr ("Child[%d] exit code %d.\n",
						pid, WEXITSTATUS (status));
			}else if (WIFSIGNALED (status)) {
				g_printerr ("Child[%d] dies with signal %d.\n",
						pid, WTERMSIG (status));
			}
#endif
		}
		/* Disconnect from the reaper. */
		if (terminal->pvt->pty_reaper != NULL) {
			g_signal_handlers_disconnect_by_func(terminal->pvt->pty_reaper,
							     vte_terminal_catch_child_exited,
							     terminal);
			g_object_unref(terminal->pvt->pty_reaper);
			terminal->pvt->pty_reaper = NULL;
		}
		terminal->pvt->pty_pid = -1;

		/* Close out the PTY. */
                vte_terminal_set_pty_object(terminal, NULL);

		/* Tell observers what's happened. */
                terminal->pvt->child_exit_status = status;
		vte_terminal_emit_child_exited(terminal);

                g_object_thaw_notify(object);
                g_object_unref(object);

                /* Note: terminal may be destroyed at this point */
	}
}

/**
 * vte_terminal_watch_child:
 * @terminal: a #RarTerminal
 * @child_pid: a #GPid
 *
 * Watches @child_pid. When the process exists, the #VteReaper::child-exited
 * signal will be called. Use vte_terminal_get_child_exit_status() to
 * retrieve the child's exit status.
 *
 * Prior to calling this function, a #VtePty must have been set in @terminal
 * using vte_terminal_set_pty_object().
 * When the child exits, the terminal's #VtePty will be set to %NULL.
 *
 * Note: g_child_watch_add() or g_child_watch_add_full() must not have
 * been called for @child_pid, nor a #GSource for it been created with
 * g_child_watch_source_new().
 *
 * Note: when using the g_spawn_async() family of functions,
 * the %G_SPAWN_DO_NOT_REAP_CHILD flag MUST have been passed.
 *
 * Since: 0.26
 */
void
vte_terminal_watch_child (RarTerminal *terminal,
                          GPid child_pid)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
        RarTerminalPrivate *pvt;
        GObject *object;
        VteReaper *reaper;

        g_return_if_fail(RAR_IS_TERMINAL(terminal));
        g_return_if_fail(child_pid != -1);

        pvt = terminal->pvt;
        g_return_if_fail(pvt->pty != NULL);

        // FIXMEchpe: support passing child_pid = -1 to remove the wathch

        object = G_OBJECT(terminal);

        g_object_freeze_notify(object);

        /* Set this as the child's pid. */
        pvt->pty_pid = child_pid;
        pvt->child_exit_status = 0;

        /* Catch a child-exited signal from the child pid. */
        reaper = vte_reaper_get();
        vte_reaper_add_child(child_pid);
        if (reaper != pvt->pty_reaper) {
                if (terminal->pvt->pty_reaper != NULL) {
                        g_signal_handlers_disconnect_by_func(pvt->pty_reaper,
                                        vte_terminal_catch_child_exited,
                                        terminal);
                        g_object_unref(pvt->pty_reaper);
                }
                g_signal_connect(reaper, "child-exited",
                                G_CALLBACK(vte_terminal_catch_child_exited),
                                terminal);
                pvt->pty_reaper = reaper;
        } else {
                g_object_unref(reaper);
	}

        /* FIXMEchpe: call vte_terminal_set_size here? */

        g_object_thaw_notify(object);
}

/**
 * vte_terminal_fork_command_full:
 * @terminal: a #RarTerminal
 * @pty_flags: flags from #VtePtyFlags
 * @argv: (array zero-terminated=1) (element-type filename): child's argument vector
 * @envv: (allow-none) (array zero-terminated=1) (element-type filename): a list of environment
 *   variables to be added to the environment before starting the process, or %NULL
 * @working_directory: (allow-none) (type filename): the name of a directory the command should start
 *   in, or %NULL to use the current working directory
 * @spawn_flags: flags from #GSpawnFlags
 * @child_setup: (allow-none): function to run in the child just before exec(), or %NULL
 * @child_setup_data: user data for @child_setup
 * @child_pid: (out) (allow-none) (transfer full): a location to store the child PID, or %NULL
 * @error: (allow-none): return location for a #GError, or %NULL
 *
 * Starts the specified command under a newly-allocated controlling
 * pseudo-terminal.  The @argv and @envv lists should be %NULL-terminated.
 * The "TERM" environment variable is automatically set to reflect the
 * terminal widget's emulation setting.
 * @pty_flags controls logging the session to the specified system log files.
 *
 * Note that %G_SPAWN_DO_NOT_REAP_CHILD will always be added to @spawn_flags.
 *
 * Note that unless @spawn_flags contains %G_SPAWN_LEAVE_DESCRIPTORS_OPEN, all file
 * descriptors except stdin/stdout/stderr will be closed before calling exec()
 * in the child.
 *
 * See vte_pty_new(), g_spawn_async() and vte_terminal_watch_child() for more information.
 *
 * Returns: %TRUE on success, or %FALSE on error with @error filled in
 *
 * Since: 0.26
 */
gboolean
vte_terminal_fork_command_full (RarTerminal *terminal,
                               VtePtyFlags pty_flags,
                               const char *working_directory,
                               char **argv,
                               char **envv,
                               GSpawnFlags spawn_flags,
                               GSpawnChildSetupFunc child_setup,
                               gpointer child_setup_data,
                               GPid *child_pid /* out */,
                               GError **error)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
        VtePty *pty;
        GPid pid;

        g_return_val_if_fail(RAR_IS_TERMINAL(terminal), FALSE);
        g_return_val_if_fail(argv != NULL, FALSE);
        g_return_val_if_fail(child_setup_data == NULL || child_setup, FALSE);
        g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

        pty = vte_pty_new(pty_flags, error);
        if (pty == NULL)
                return FALSE;

        /* FIXMEchpe: is this flag needed */
        spawn_flags |= G_SPAWN_CHILD_INHERITS_STDIN;

        if (!__vte_pty_spawn(pty,
                             working_directory,
                             argv,
                             envv,
                             spawn_flags,
                             child_setup, child_setup_data,
                             &pid,
                             error)) {
                g_object_unref(pty);
                return FALSE;
        }

        vte_terminal_set_pty_object(terminal, pty);
        vte_terminal_watch_child(terminal, pid);

        if (child_pid)
                *child_pid = pid;

#if 0
	int fd = vte_pty_get_fd (pty);
	printf ("bash fd = %d\n", fd);
	int r;
	int w;
	char buffer[4096];

	r = read (fd, buffer, sizeof (buffer));
	//printf ("r = %d\n", r);
	printf ("buffer=%s\n", buffer);

	snprintf (buffer, sizeof (buffer), "ls -al --color ~/bin\n");
	//snprintf (buffer, sizeof (buffer), "set\n");
	w = write (fd, buffer, strlen (buffer));
	//printf ("w = %d\n", w);

	r = read (fd, buffer, sizeof (buffer));
	//printf ("r = %d\n", r);
	printf ("buffer=%s\n", buffer);

	r = read (fd, buffer, sizeof (buffer));
	//printf ("r = %d\n", r);
	printf ("buffer=%s\n", buffer);
#endif
        return TRUE;
}


/**
 * _vte_terminal_enable_input_source
 */
static inline void
_vte_terminal_enable_input_source (RarTerminal *terminal)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
	if (terminal->pvt->pty_channel == NULL) {
		return;
	}

	if (terminal->pvt->pty_input_source == 0) {
		_vte_debug_print (VTE_DEBUG_IO, "polling vte_terminal_io_read\n");
		terminal->pvt->pty_input_source =
			g_io_add_watch_full(terminal->pvt->pty_channel,
					    VTE_CHILD_INPUT_PRIORITY,
					    G_IO_IN | G_IO_HUP,
					    (GIOFunc) vte_terminal_io_read,
					    terminal,
					    (GDestroyNotify) mark_input_source_invalid);
	}
}

/**
 * need_processing
 */
static inline gboolean
need_processing (RarTerminal *terminal)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
	return _vte_incoming_chunks_length (terminal->pvt->incoming) != 0;
}

/**
 * time_process_incoming
 */
static void
time_process_incoming (RarTerminal *terminal)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
	gdouble elapsed;
	glong target;
	g_timer_reset (process_timer);
	vte_terminal_process_incoming (terminal);
	elapsed = g_timer_elapsed (process_timer, NULL) * 1000;
	target = VTE_MAX_PROCESS_TIME / elapsed * terminal->pvt->input_bytes;
	terminal->pvt->max_input_bytes =
		(terminal->pvt->max_input_bytes + target) / 2;
	printf ("Leaving: %s\n", __FUNCTION__);
}

/**
 * vte_terminal_start_processing
 */
static inline void
vte_terminal_start_processing (RarTerminal *terminal)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
	if (!vte_terminal_is_processing (terminal)) {
		vte_terminal_add_process_timeout (terminal);
	}
}


/**
 * vte_terminal_feed:
 * @terminal: a #RarTerminal
 * @data: (array length=length zero-terminated=0) (element-type uint8): a string in the terminal's current encoding
 * @length: the length of the string
 *
 * Interprets @data as if it were data received from a child process.  This
 * can either be used to drive the terminal without a child process, or just
 * to mess with your users.
 */
void
vte_terminal_feed(RarTerminal *terminal, const char *data, glong length)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
	/* If length == -1, use the length of the data string. */
	if (length == ((gssize)-1)) {
		length = strlen(data);
	}

	/* If we have data, modify the incoming buffer. */
	if (length > 0) {
		struct _vte_incoming_chunk *chunk;
		if (terminal->pvt->incoming &&
				(gsize)length < sizeof (terminal->pvt->incoming->data) - terminal->pvt->incoming->len) {
			chunk = terminal->pvt->incoming;
		} else {
			chunk = get_chunk ();
			_vte_terminal_feed_chunks (terminal, chunk);
		}
		do { /* break the incoming data into chunks */
			gsize rem = sizeof (chunk->data) - chunk->len;
			gsize len = (gsize) length < rem ? (gsize) length : rem;
			memcpy (chunk->data + chunk->len, data, len);
			chunk->len += len;
			length -= len;
			if (length == 0) {
				break;
			}
			data += len;

			chunk = get_chunk ();
			_vte_terminal_feed_chunks (terminal, chunk);
		} while (1);
		vte_terminal_start_processing (terminal);
	}
}

/**
 * _vte_terminal_codeset_changed_cb
 */
static void
_vte_terminal_codeset_changed_cb(struct _vte_iso2022_state *state, gpointer p)
{
	vte_terminal_set_encoding(p, _vte_iso2022_state_get_codeset(state));
}

/**
 * vte_terminal_set_encoding:
 * @terminal: a #RarTerminal
 * @codeset: (allow-none): a valid #GIConv target, or %NULL to use the default encoding
 *
 * Changes the encoding the terminal will expect data from the child to
 * be encoded with.  For certain terminal types, applications executing in the
 * terminal can change the encoding.  The default encoding is defined by the
 * application's locale settings.
 */
void
vte_terminal_set_encoding(RarTerminal *terminal, const char *codeset)
{
        RarTerminalPrivate *pvt;
        GObject *object;
	const char *old_codeset;
	VteConv conv;
	char *obuf1, *obuf2;
	gsize bytes_written;

	g_return_if_fail(RAR_IS_TERMINAL(terminal));

        object = G_OBJECT(terminal);
        pvt = terminal->pvt;

	old_codeset = pvt->encoding;
	if (codeset == NULL) {
		g_get_charset(&codeset);
	}
	if ((old_codeset != NULL) && (strcmp(codeset, old_codeset) == 0)) {
		/* Nothing to do! */
		return;
	}

        g_object_freeze_notify(object);

	/* Open new conversions. */
	conv = _vte_conv_open(codeset, "UTF-8");
	if (conv == VTE_INVALID_CONV) {
		g_warning(_("Unable to convert characters from %s to %s."),
			  "UTF-8", codeset);
		/* fallback to no conversion */
		conv = _vte_conv_open(codeset = "UTF-8", "UTF-8");
	}
	if (terminal->pvt->outgoing_conv != VTE_INVALID_CONV) {
		_vte_conv_close(terminal->pvt->outgoing_conv);
	}
	terminal->pvt->outgoing_conv = conv;

	/* Set the terminal's encoding to the new value. */
	terminal->pvt->encoding = g_intern_string(codeset);

	/* Convert any buffered output bytes. */
	if ((_vte_buffer_length(terminal->pvt->outgoing) > 0) &&
	    (old_codeset != NULL)) {
		/* Convert back to UTF-8. */
		obuf1 = g_convert((gchar *)terminal->pvt->outgoing->data,
				  _vte_buffer_length(terminal->pvt->outgoing),
				  "UTF-8",
				  old_codeset,
				  NULL,
				  &bytes_written,
				  NULL);
		if (obuf1 != NULL) {
			/* Convert to the new encoding. */
			obuf2 = g_convert(obuf1,
					  bytes_written,
					  codeset,
					  "UTF-8",
					  NULL,
					  &bytes_written,
					  NULL);
			if (obuf2 != NULL) {
				_vte_buffer_clear(terminal->pvt->outgoing);
				_vte_buffer_append(terminal->pvt->outgoing,
						   obuf2, bytes_written);
				g_free(obuf2);
			}
			g_free(obuf1);
		}
	}

	/* Set the encoding for incoming text. */
	_vte_iso2022_state_set_codeset(terminal->pvt->iso2022,
				       terminal->pvt->encoding);

	_vte_debug_print(VTE_DEBUG_IO,
			"Set terminal encoding to `%s'.\n",
			terminal->pvt->encoding);
	//RARXXX vte_terminal_emit_encoding_changed(terminal);

        g_object_thaw_notify(object);
}

/**
 * vte_terminal_set_emulation:
 * @terminal: a #RarTerminal
 * @emulation: (allow-none): the name of a terminal description, or %NULL to use the default
 *
 * Sets what type of terminal the widget attempts to emulate by scanning for
 * control sequences defined in the system's termcap file.  Unless you
 * are interested in this feature, always use "xterm".
 */
void
vte_terminal_set_emulation(RarTerminal *terminal, const char *emulation)
{
        RarTerminalPrivate *pvt;
        GObject *object;
	int columns, rows;

	g_return_if_fail(RAR_IS_TERMINAL(terminal));

        object = G_OBJECT(terminal);
        pvt = terminal->pvt;

        g_object_freeze_notify(object);

	/* Set the emulation type, for reference. */
	if (emulation == NULL) {
		emulation = vte_terminal_get_default_emulation(terminal);
	}
	terminal->pvt->outer->emulation = g_intern_string(emulation);
	_vte_debug_print(VTE_DEBUG_MISC,
			"Setting emulation to `%s'...\n", emulation);
	/* Find and read the right termcap file. */
	vte_terminal_set_termcap(terminal, NULL, FALSE);

	/* Create a table to hold the control sequences. */
	if (terminal->pvt->matcher != NULL) {
		_vte_matcher_free(terminal->pvt->matcher);
	}
	terminal->pvt->matcher = _vte_matcher_new(emulation, terminal->pvt->outer->termcap);

	if (terminal->pvt->outer->termcap != NULL) {
		/* Read emulation flags. */
		terminal->pvt->outer->flags.am = _vte_termcap_find_boolean(terminal->pvt->outer->termcap,
								    terminal->pvt->outer->emulation,
								    "am");
		terminal->pvt->outer->flags.bw = _vte_termcap_find_boolean(terminal->pvt->outer->termcap,
								    terminal->pvt->outer->emulation,
								    "bw");
		terminal->pvt->outer->flags.LP = _vte_termcap_find_boolean(terminal->pvt->outer->termcap,
								    terminal->pvt->outer->emulation,
								    "LP");
		terminal->pvt->outer->flags.ul = _vte_termcap_find_boolean(terminal->pvt->outer->termcap,
								    terminal->pvt->outer->emulation,
								    "ul");
		terminal->pvt->outer->flags.xn = _vte_termcap_find_boolean(terminal->pvt->outer->termcap,
								    terminal->pvt->outer->emulation,
								    "xn");

		/* Resize to the given default. */
		columns = _vte_termcap_find_numeric(terminal->pvt->outer->termcap,
						    terminal->pvt->outer->emulation,
						    "co");
		if (columns <= 0) {
			columns = VTE_COLUMNS;
		}
		terminal->pvt->default_column_count = columns;

		rows = _vte_termcap_find_numeric(terminal->pvt->outer->termcap,
						 terminal->pvt->outer->emulation,
						 "li");
		if (rows <= 0 ) {
			rows = VTE_ROWS;
		}
		terminal->pvt->default_row_count = rows;
	}

	/* Notify observers that we changed our emulation. */
	vte_terminal_emit_emulation_changed(terminal);

        g_object_thaw_notify(object);
}

/**
 * vte_terminal_set_size:
 * @terminal: a #RarTerminal
 * @columns: the desired number of columns
 * @rows: the desired number of rows
 *
 * Attempts to change the terminal's size in terms of rows and columns.  If
 * the attempt succeeds, the widget will resize itself to the proper size.
 */
void
vte_terminal_set_size(RarTerminal *terminal, glong columns, glong rows)
{
	glong old_columns, old_rows;

	g_return_if_fail(RAR_IS_TERMINAL(terminal));

	_vte_debug_print(VTE_DEBUG_MISC,
			"Setting PTY size to %ldx%ld.\n",
			columns, rows);

	old_rows = terminal->row_count;
	old_columns = terminal->column_count;

	if (terminal->pvt->pty != NULL) {
                GError *error = NULL;

		/* Try to set the terminal size, and read it back,
		 * in case something went awry.
                 */
		if (!vte_pty_set_size(terminal->pvt->pty, rows, columns, &error)) {
			g_warning("%s\n", error->message);
                        g_error_free(error);
		}
		vte_terminal_refresh_size(terminal);
	} else {
		terminal->row_count = rows;
		terminal->column_count = columns;
	}
	if (old_rows != terminal->row_count || old_columns != terminal->column_count) {
		RarOuter *outer = terminal->pvt->outer;
		glong visible_rows = MIN (old_rows, _vte_ring_length (outer->screen->row_data));
		if (terminal->row_count < visible_rows) {
			glong delta = visible_rows - terminal->row_count;
			outer->screen->insert_delta += delta;
#ifndef RARXXX
			vte_terminal_queue_adjustment_value_changed (
					terminal,
					outer->screen->scroll_delta + delta);
#endif
		}
		//RARXXX gtk_widget_queue_resize_no_redraw (&terminal->widget);
		/* Our visible text changed. */
		vte_terminal_emit_text_modified(terminal);
	}
}

/**
 * vte_terminal_get_default_emulation:
 * @terminal: a #RarTerminal
 *
 * Queries the terminal for its default emulation, which is attempted if the
 * terminal type passed to vte_terminal_set_emulation() is %NULL.
 *
 * Returns: (transfer none) (type utf8): an interned string containing the name of the default terminal
 *   type the widget attempts to emulate
 *
 * Since: 0.11.11
 */
const char *
vte_terminal_get_default_emulation(RarTerminal *terminal)
{
	return g_intern_static_string(VTE_DEFAULT_EMULATION);
}

/**
 * vte_terminal_set_termcap
 * Set the path to the termcap file we read, and read it in.
 */
static void
vte_terminal_set_termcap(RarTerminal *terminal, const char *path,
			 gboolean reset)
{
        GObject *object = G_OBJECT(terminal);
	struct stat st;
	char *wpath;

	if (path == NULL) {
		wpath = g_strdup_printf(DATADIR "/" PACKAGE "/termcap/%s",
					terminal->pvt->outer->emulation ?
					terminal->pvt->outer->emulation :
					vte_terminal_get_default_emulation(terminal));
		if (g_stat(wpath, &st) != 0) {
			g_free(wpath);
			wpath = g_strdup("/etc/termcap");
		}
		path = g_intern_string (wpath);
		g_free(wpath);
	} else {
		path = g_intern_string (path);
	}
	if (path == terminal->pvt->termcap_path) {
		return;
	}

        g_object_freeze_notify(object);

	terminal->pvt->termcap_path = path;

	_vte_debug_print(VTE_DEBUG_MISC, "Loading termcap `%s'...",
			terminal->pvt->termcap_path);
	if (terminal->pvt->outer->termcap != NULL) {
		_vte_termcap_free(terminal->pvt->outer->termcap);
	}
	terminal->pvt->outer->termcap = _vte_termcap_new(terminal->pvt->termcap_path);
	_vte_debug_print(VTE_DEBUG_MISC, "\n");
	if (terminal->pvt->outer->termcap == NULL) {
		_vte_terminal_inline_error_message(terminal,
				"Failed to load terminal capabilities from '%s'",
				terminal->pvt->termcap_path);
	}
	if (reset) {
		vte_terminal_set_emulation(terminal, terminal->pvt->outer->emulation);
	}

        g_object_thaw_notify(object);
}

/**
 * vte_terminal_emit_emulation_changed
 * Emit an "emulation-changed" signal.
 */
static void
vte_terminal_emit_emulation_changed(RarTerminal *terminal)
{
#ifndef RAR
	_vte_debug_print(VTE_DEBUG_SIGNALS,
			"Emitting `emulation-changed'.\n");
	g_signal_emit_by_name(terminal, "emulation-changed");
        g_object_notify(G_OBJECT(terminal), "emulation");
#endif
}

/**
 * _vte_terminal_inline_error_message
 */
void
_vte_terminal_inline_error_message(RarTerminal *terminal, const char *format, ...)
{
	va_list ap;
	char *str;

	va_start (ap, format);
	str = g_strdup_vprintf (format, ap);
	va_end (ap);

	vte_terminal_feed (terminal, "*** VTE ***: ", 13);
	vte_terminal_feed (terminal, str, -1);
	vte_terminal_feed (terminal, "\r\n", 2);
	g_free (str);
}

/**
 * vte_terminal_emit_text_modified
 * Emit a "text-modified" signal.
 */
static void
vte_terminal_emit_text_modified(RarTerminal *terminal)
{
#ifndef RARXXX
	if (!terminal->pvt->accessible_emit) {
		return;
	}
	_vte_debug_print(VTE_DEBUG_SIGNALS,
			"Emitting `text-modified'.\n");
	g_signal_emit_by_name(terminal, "text-modified");
#endif
}

/**
 * vte_terminal_refresh_size
 * Read and refresh our perception of the size of the PTY.
 */
static void
vte_terminal_refresh_size(RarTerminal *terminal)
{
        RarTerminalPrivate *pvt = terminal->pvt;
	int rows, columns;
        GError *error = NULL;

        if (pvt->pty == NULL)
                return;

        if (vte_pty_get_size(pvt->pty, &rows, &columns, &error)) {
                terminal->row_count = rows;
                terminal->column_count = columns;
        } else {
                g_warning(_("Error reading PTY size, using defaults: %s\n"), error->message);
                g_error_free(error);
	}
}

