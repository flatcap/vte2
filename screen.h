#ifndef __SCREEN_H__
#define __SCREEN_H__

#include "ring.h"

#ifdef RARXXX // Copied from vte-private.h
typedef struct _VteVisualPosition {
	long row, col;
} VteVisualPosition;
#endif

typedef struct _RarScreen RarScreen;

/* Screen data.  We support the normal screen, and an alternate
 * screen, which seems to be a DEC-specific feature. */
struct _RarScreen {
	VteRing row_data[1];	/* buffer contents */
	VteVisualPosition cursor_current, cursor_saved;
	/* the current and saved positions of the [insertion] cursor -- current is
	   absolute, saved is relative to the insertion delta */
	gboolean reverse_mode;	/* reverse mode */
	gboolean origin_mode;	/* origin mode */
	gboolean sendrecv_mode;	/* sendrecv mode */
	gboolean insert_mode;	/* insert mode */
	gboolean linefeed_mode;	/* linefeed mode */
	gboolean bracketed_paste_mode;
	struct vte_scrolling_region {
		int start, end;
	} scrolling_region;	/* the region we scroll in */
	gboolean scrolling_restricted;
	long scroll_delta;	/* scroll offset */
	long insert_delta;	/* insertion offset */
	VteCell defaults;	/* default characteristics for insertion of any new
				   characters */
	VteCell color_defaults;	/* original defaults plus the current fore/back */
	VteCell fill_defaults;	/* original defaults plus the current fore/back with no
				   character data */
	gboolean alternate_charset;
	gboolean status_line;
	GString *status_line_contents;
	gboolean status_line_changed;
};

#endif // __SCREEN_H__
