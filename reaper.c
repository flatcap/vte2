/*
 * Copyright (C) 2002 Red Hat, Inc.
 *
 * This is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Library General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/**
 * SECTION: vte-reaper
 * @short_description: A singleton object which catches %SIGCHLD signals and
 * converts them into GObject-style &quot;child-exited&quot; signals
 *
 * Because an application may need to be notified when child processes
 * exit, and because there is only one %SIGCHLD handler, the #VteTerminal
 * widget relies on a #VteReaper to watch for %SIGCHLD notifications and
 * retrieve the exit status of child processes which have exited.  When
 * glib provides child_watch functionality, the #VteReaper merely acts as
 * a proxy for glib's own functionality.
 *
 * Since 0.11.11
 */

#include <config.h>

#include "debug.h"
#include "marshal.h"
#include "reaper.h"

static VteReaper *singleton_reaper = NULL;

G_DEFINE_TYPE(VteReaper, vte_reaper, G_TYPE_OBJECT)

static void
vte_reaper_child_watch_cb(GPid pid, gint status, gpointer data)
{
	printf ("Entering: %s\n", __FUNCTION__);
	_vte_debug_print(VTE_DEBUG_SIGNALS,
			"Reaper emitting child-exited signal.\n");
	g_signal_emit_by_name(data, "child-exited", pid, status);
	g_spawn_close_pid (pid);
}

/**
 * vte_reaper_add_child:
 * @pid: the ID of a child process which will be monitored
 *
 * Ensures that child-exited signals will be emitted when @pid exits.  This is
 * necessary for correct operation when running with glib versions >= 2.4.
 *
 * Returns: the new source ID
 *
 * Since 0.11.11
 */
int
vte_reaper_add_child(GPid pid)
{
	printf ("Entering: %s\n", __FUNCTION__);
	return g_child_watch_add_full(G_PRIORITY_LOW,
				      pid,
				      vte_reaper_child_watch_cb,
				      vte_reaper_get(),
				      (GDestroyNotify)g_object_unref);
}

static void
vte_reaper_init(VteReaper *reaper)
{
	printf ("Entering: %s\n", __FUNCTION__);
}

static GObject*
vte_reaper_constructor (GType                  type,
                        guint                  n_construct_properties,
                        GObjectConstructParam *construct_properties)
{
	printf ("Entering: %s\n", __FUNCTION__);
  if (singleton_reaper) {
	  return g_object_ref (singleton_reaper);
  } else {
	  GObject *obj;
	  obj = G_OBJECT_CLASS (vte_reaper_parent_class)->constructor (type, n_construct_properties, construct_properties);
	  singleton_reaper = VTE_REAPER (obj);
	  return obj;
  }
}


static void
vte_reaper_finalize(GObject *reaper)
{
	printf ("Entering: %s\n", __FUNCTION__);
	G_OBJECT_CLASS(vte_reaper_parent_class)->finalize(reaper);
	singleton_reaper = NULL;
}

static void
vte_reaper_class_init(VteReaperClass *klass)
{
	printf ("Entering: %s\n", __FUNCTION__);
	GObjectClass *gobject_class;

        /**
         * VteReaper::child-exited:
         * @vtereaper: the object which received the signal
         * @arg1: the process ID of the exited child
         * @arg2: the status of the exited child, as returned by waitpid()
         * 
         * Emitted when the #VteReaper object detects that a child of the
         * current process has exited.
         *
         * Since: 0.11.11
         */
	klass->child_exited_signal = g_signal_new(g_intern_static_string("child-exited"),
						  G_OBJECT_CLASS_TYPE(klass),
						  G_SIGNAL_RUN_LAST,
						  0,
						  NULL,
						  NULL,
						  _vte_marshal_VOID__INT_INT,
						  G_TYPE_NONE,
						  2, G_TYPE_INT, G_TYPE_INT);

	gobject_class = G_OBJECT_CLASS(klass);
	gobject_class->constructor = vte_reaper_constructor;
	gobject_class->finalize = vte_reaper_finalize;
}

/**
 * vte_reaper_get:
 *
 * Finds the address of the global #VteReaper object, creating the object if
 * necessary.
 *
 * Returns: the global #VteReaper object, which should not be unreffed.
 */
VteReaper *
vte_reaper_get(void)
{
	printf ("Entering: %s\n", __FUNCTION__);
	return g_object_new(VTE_TYPE_REAPER, NULL);
}

