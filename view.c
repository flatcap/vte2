#include "config.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <glib-object.h>
#include <glib/gi18n-lib.h>
#include <gobject/gmarshal.h>

#include "view.h"
#include "view-private.h"

G_DEFINE_TYPE(RarView, rar_view, G_TYPE_OBJECT)

/* I can only find these in vte.[ch] */
#define I_(string) (g_intern_static_string(string))
#define STATIC_PARAMS (G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK | G_PARAM_STATIC_BLURB)

#ifdef ENABLE_NLS
#define P_(String) g_dgettext(GETTEXT_PACKAGE "-properties",String)
#else 
#define P_(String) (String)
#endif

#define GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), RAR_TYPE_VIEW, RarViewPrivate))

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
 * rar_view_new
 */
GObject *
rar_view_new(void)
{
	printf ("Entering: %s\n", __FUNCTION__);
	return g_object_new(RAR_TYPE_VIEW, NULL);
}

/**
 * rar_view_init
 */
void
rar_view_init(RarView *view)
{
	printf ("Entering: %s\n", __FUNCTION__);
	RarViewPrivate *pvt;

	/* Initialize private data. */
	pvt = view->pvt = GET_PRIVATE (view);

}

/**
 * rar_view_dispose
 */
static void
rar_view_dispose (GObject *gobject)
{
	printf ("Entering: %s\n", __FUNCTION__);
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
 * rar_view_finalize
 */
static void
rar_view_finalize(GObject *object)
{
	printf ("Entering: %s\n", __FUNCTION__);
	printf ("%s: %p dying\n", __FUNCTION__, object);

	/*
	RarView *view = RAR_VIEW (object);
	printf ("view = %p, obj = %p\n", view, object);
        int matches = g_signal_handlers_disconnect_matched (object, G_SIGNAL_MATCH_DATA, 0, 0, NULL, NULL, view);
	printf ("disconnected %d signals\n", matches);
	*/

	/* Call the inherited finalize() method. */
	G_OBJECT_CLASS(rar_view_parent_class)->finalize(object);
}

/**
 * rar_view_get_property
 */
static void
rar_view_get_property (GObject *object,
                           guint prop_id,
                           GValue *value,
                           GParamSpec *pspec)
{
	printf ("Entering: %s\n", __FUNCTION__);
        //RarView *view = RAR_VIEW (object);
        //RarViewPrivate *pvt = view->pvt;

	switch (prop_id)
	{
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
			return;
        }
}

/**
 * rar_view_set_property
 */
static void
rar_view_set_property (GObject *object,
                           guint prop_id,
                           const GValue *value,
                           GParamSpec *pspec)
{
	printf ("Entering: %s\n", __FUNCTION__);
        //RarView *view = RAR_VIEW (object);

	switch (prop_id)
	{
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
			return;
	}
}

/**
 * rar_view_class_init
 */
void
rar_view_class_init(RarViewClass *klass)
{
	printf ("Entering: %s\n", __FUNCTION__);
	GObjectClass *gobject_class;

	g_type_class_add_private(klass, sizeof (RarViewPrivate));

	gobject_class = G_OBJECT_CLASS(klass);

	/* Override some of the default handlers. */
	gobject_class->dispose      = rar_view_dispose;
	gobject_class->finalize     = rar_view_finalize;
        gobject_class->get_property = rar_view_get_property;
        gobject_class->set_property = rar_view_set_property;

	/* Register some signals of our own. */
}


/* View Functions */
/**
 * rar_view_set_follow_cursor
 */
void
rar_view_set_follow_cursor (RarView *view, gboolean follow)
{
	printf ("Entering: %s\n", __FUNCTION__);
	printf ("%s: %s\n", __FUNCTION__, follow ? "TRUE" : "FALSE");
}

/**
 * rar_view_send_text
 */
void
rar_view_send_text (RarView *view, const char *text)
{
	printf ("Entering: %s\n", __FUNCTION__);
	printf ("%s: %s", __FUNCTION__, text);
}

/**
 * rar_view_get_height
 */
int
rar_view_get_height (RarView *view)
{
	printf ("Entering: %s\n", __FUNCTION__);
	int height = 24;

	printf ("%s: height %d\n", __FUNCTION__, height);
	return height;
}

/**
 * rar_view_get_top_row
 */
int
rar_view_get_top_row (RarView *view)
{
	printf ("Entering: %s\n", __FUNCTION__);
	int top_row = 42;

	printf ("%s: top row %d\n", __FUNCTION__, top_row);
	return top_row;
}

/**
 * rar_view_get_text_row
 */
char *
rar_view_get_text_row (RarView *view, int row)
{
	printf ("Entering: %s\n", __FUNCTION__);
	const char *text = "-rw-r-----. 1   434 2010-05-20 17:35 Makefile";

	printf ("%s: row %d: %s\n", __FUNCTION__, row, text);
	return strdup (text);
}


