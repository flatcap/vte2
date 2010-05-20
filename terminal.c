#include "config.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <glib-object.h>
#include <glib/gi18n-lib.h>
#include <gobject/gmarshal.h>

#include "terminal.h"
#include "terminal-private.h"

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

/* Enumerations */
/**
 * Object properties
 */
enum {
        PROP_0,
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
rar_terminal_new(void)
{
	return g_object_new(RAR_TYPE_TERMINAL, NULL);
}

/**
 * rar_terminal_init
 */
void
rar_terminal_init(RarTerminal *terminal)
{
	RarTerminalPrivate *pvt;

	/* Initialize private data. */
	pvt = terminal->pvt = GET_PRIVATE (terminal);

}

/**
 * rar_terminal_dispose
 */
static void
rar_terminal_dispose (GObject *gobject)
{
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
rar_terminal_finalize(GObject *object)
{
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
        //RarTerminal *terminal = RAR_TERMINAL (object);
        //RarTerminalPrivate *pvt = terminal->pvt;

	switch (prop_id)
	{
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
        //RarTerminal *terminal = RAR_TERMINAL (object);

	switch (prop_id)
	{
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
			return;
	}
}

/**
 * rar_terminal_class_init
 */
void
rar_terminal_class_init(RarTerminalClass *klass)
{
	GObjectClass *gobject_class;

	g_type_class_add_private(klass, sizeof (RarTerminalPrivate));

	gobject_class = G_OBJECT_CLASS(klass);

	/* Override some of the default handlers. */
	gobject_class->dispose      = rar_terminal_dispose;
	gobject_class->finalize     = rar_terminal_finalize;
        gobject_class->get_property = rar_terminal_get_property;
        gobject_class->set_property = rar_terminal_set_property;

	/* Register some signals of our own. */
}


/* Terminal Functions */
/**
 * rar_terminal_set_shell
 */
void
rar_terminal_set_shell (RarTerminal *term, const char *shell)
{
	printf ("%s: shell = %s\n", __FUNCTION__, shell);
}

/**
 * rar_terminal_set_size
 */
void
rar_terminal_set_size (RarTerminal *term, int width, int height)
{
	printf ("%s: size = %d,%d\n", __FUNCTION__, width, height);
}

/**
 * rar_terminal_set_scrollback
 */
void
rar_terminal_set_scrollback (RarTerminal *term, int lines)
{
	printf ("%s: scrollback = %d\n", __FUNCTION__, lines);
}

/**
 * rar_terminal_set_env
 */
void
rar_terminal_set_env (RarTerminal *term, const char **env)
{
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
	printf ("%s: running shell\n", __FUNCTION__);
}

/**
 * rar_terminal_new_view
 */
RarView *
rar_terminal_new_view (RarTerminal *term)
{
	GObject *obj = NULL;
	RarView *view = NULL;

	obj = rar_view_new();
	view = RAR_VIEW (obj);

	printf ("%s: new view %p\n", __FUNCTION__, view);
	return view;
}

