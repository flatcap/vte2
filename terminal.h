#ifndef __TERMINAL_H__
#define __TERMINAL_H__

#include <glib-object.h>

#include "view.h"
#include "screen.h"

#define RAR_TYPE_TERMINAL            (rar_terminal_get_type())
#define RAR_TERMINAL(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), RAR_TYPE_TERMINAL, RarTerminal))
#define RAR_TERMINAL_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass),  RAR_TYPE_TERMINAL, RarTerminalClass))
#define RAR_IS_TERMINAL(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), RAR_TYPE_TERMINAL))
#define RAR_IS_TERMINAL_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),  RAR_TYPE_TERMINAL))
#define RAR_TERMINAL_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),  RAR_TYPE_TERMINAL, RarTerminalClass))

typedef struct _RarTerminal         RarTerminal;
typedef struct _RarTerminalPrivate  RarTerminalPrivate;
typedef struct _RarTerminalClass    RarTerminalClass;

typedef struct _dummy {
	glong value;
} Adj;

/**
 * RarTerminal
 */
struct _RarTerminal {
	GObject parent;

	RarTerminalPrivate *pvt;

	/* These were in VteTerminal */
	Adj *adjustment;	// GtkAdjustment
	glong column_count;
	glong row_count;

	/* There were in VtePrivate */
	RarScreen *alternate_screen;
	RarScreen *normal_screen;
	int cursor_mode;
	gboolean cursor_visible;
	GHashTable *dec_saved;
	const char *emulation;
	struct vte_terminal_flags {
		gboolean am;
		gboolean bw;
		gboolean LP;
		gboolean ul;
		gboolean xn;
	} flags;
	gboolean hp_fkey_mode;
	int keypad_mode;
	gboolean legacy_fkey_mode;
	gboolean margin_bell;
	gboolean meta_sends_escape;
	int mouse_tracking_mode;
	gboolean nrc_mode;
	RarScreen *screen;
	long scrollback_lines;
	gboolean smooth_scroll;
	gboolean sun_fkey_mode;
	GHashTable *tabstops;
	struct _vte_termcap *termcap;
	gboolean text_deleted_flag;
	gboolean text_modified_flag;
	gboolean text_inserted_flag;
	gboolean vt220_fkey_mode;
};

/**
 * RarTerminalClass
 */
struct _RarTerminalClass {
	/*< public >  */
	/* Inherited parent class. */
	GObjectClass parent_class;

	/*< protected >*/
	/* Virtual methods */

	/*< protected >*/
	/* Default signal handlers. */
};


GType rar_terminal_get_type(void);

GObject *rar_terminal_new(void);

void rar_terminal_set_shell      (RarTerminal *term, const char *shell);
void rar_terminal_set_size       (RarTerminal *term, int width, int height);
void rar_terminal_set_scrollback (RarTerminal *term, int lines);
void rar_terminal_set_env        (RarTerminal *term, const char **env);
void rar_terminal_run_shell      (RarTerminal *term);

RarView * rar_terminal_new_view  (RarTerminal *term);

#endif // __TERMINAL_H__

