#ifndef __VIEW_H__
#define __VIEW_H__

#include <glib-object.h>

#define RAR_TYPE_VIEW            (rar_view_get_type())
#define RAR_VIEW(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), RAR_TYPE_VIEW, RarView))
#define RAR_VIEW_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass),  RAR_TYPE_VIEW, RarViewClass))
#define RAR_IS_VIEW(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), RAR_TYPE_VIEW))
#define RAR_IS_VIEW_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),  RAR_TYPE_VIEW))
#define RAR_VIEW_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),  RAR_TYPE_VIEW, RarViewClass))

typedef struct _RarView         RarView;
typedef struct _RarViewPrivate  RarViewPrivate;
typedef struct _RarViewClass    RarViewClass;

/**
 * RarView
 */
struct _RarView {
	GObject parent;

	RarViewPrivate *pvt;
};

/**
 * RarViewClass
 */
struct _RarViewClass {
	/*< public >  */
	/* Inherited parent class. */
	GObjectClass parent_class;

	/*< protected >*/
	/* Virtual methods */

	/*< protected >*/
	/* Default signal handlers. */
};


GType rar_view_get_type(void);

GObject *rar_view_new(void);

void   rar_view_set_follow_cursor (RarView *view, gboolean follow);
void   rar_view_send_text         (RarView *view, const char *text);
int    rar_view_get_height        (RarView *view);
int    rar_view_get_top_row       (RarView *view);
char * rar_view_get_text_row      (RarView *view, int row);

#endif // __VIEW_H__

