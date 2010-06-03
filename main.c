#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include "terminal.h"
#include "view.h"

/**
 * app_close
 */
static gboolean
app_close (gpointer loop)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
	printf ("quitting app_close\n");
	g_main_loop_quit (loop);
	return FALSE;
}

/**
 * term_work
 */
static gboolean
term_work (gpointer data)
{
	//printf ("Entering: %s\n", __FUNCTION__);
	static int count = 0;

	//RarTerminal *term = RAR_TERMINAL (data);

	count++;
	printf ("\e[32m%s %d\e[0m\n", __FUNCTION__, count);

	return TRUE;
}

/**
 * main
 */
int
main (int argc, char *argv[])
{
	//printf ("Entering: %s\n", __FUNCTION__);e
	GObject *obj = NULL;
	RarTerminal *term = NULL;
	RarView *view1 = NULL;
	RarView *view2 = NULL;
	const char *env[] = { "ONE=apple", "TWO=banana", NULL };
#if 0
	int h1, h2;
	int t1, t2;
	char *str1, *str2;
#endif
	GMainContext *context = NULL;
	GMainLoop *loop = NULL;

	g_type_init();

	obj = rar_terminal_new();
	term = RAR_TERMINAL(obj);

	rar_terminal_set_shell      (term, "/bin/bash");
	rar_terminal_set_size       (term, 80, 24);
	rar_terminal_set_scrollback (term, 100);
	rar_terminal_set_env        (term, env);
	rar_terminal_run_shell      (term);

	view1 = rar_terminal_new_view (term);
	view2 = rar_terminal_new_view (term);

#if 0
	rar_view_set_follow_cursor (view1, TRUE);
	rar_view_set_follow_cursor (view2, FALSE);

	rar_view_send_text (view1, "ls\n");
	rar_view_send_text (view2, "seq 50\n");

	h1 = rar_view_get_height (view1);
	h2 = rar_view_get_height (view2);

	t1 = rar_view_get_top_row (view1);
	t2 = rar_view_get_top_row (view2);

	str1 = rar_view_get_text_row (view1, t1);
	str2 = rar_view_get_text_row (view2, t2);

	free (str1);
	free (str2);
#endif

	loop = g_main_loop_new(context, TRUE);
	g_timeout_add_seconds (5, app_close, (gpointer) loop);
	g_timeout_add (100,  term_work, (gpointer) term);
	g_main_loop_run(loop);

	g_object_unref (view1);
	g_object_unref (view2);
	g_object_unref (term);

	return 0;
}
