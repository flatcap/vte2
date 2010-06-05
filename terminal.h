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

void vte_terminal_feed_child(RarTerminal *terminal, const char *text, glong length);

void rar_terminal_dump_screens (RarTerminal *term);
void rar_terminal_dump_termcap (RarTerminal *term);
void rar_terminal_dump_matcher (RarTerminal *term);

#endif // __TERMINAL_H__

