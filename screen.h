#ifndef __SCREEN_H__
#define __SCREEN_H__

#include "ring.h"

typedef enum {
  VTE_REGEX_CURSOR_GDKCURSOR,
  VTE_REGEX_CURSOR_GDKCURSORTYPE,
  VTE_REGEX_CURSOR_NAME
} VteRegexCursorMode;

typedef struct _RarShared {
	glong row_count;
	glong column_count;

	gboolean text_modified_flag;
	gboolean text_inserted_flag;
	gboolean text_deleted_flag;
	struct _vte_termcap *termcap;	/* termcap storage */
	GHashTable *tabstops;
	int keypad_mode;
	const char *emulation;		/* terminal type to emulate */
	struct vte_terminal_flags {	/* boolean termcap flags */
		gboolean am;
		gboolean bw;
		gboolean LP;
		gboolean ul;
		gboolean xn;
	} flags;

	gboolean sun_fkey_mode;
	gboolean hp_fkey_mode;
	gboolean legacy_fkey_mode;
	gboolean vt220_fkey_mode;

	int mouse_tracking_mode; /* this is of type MouseTrackingMode,
				    but we need to guarantee its type. */
	gboolean smooth_scroll;
	gboolean cursor_visible;
	int cursor_mode;
	GHashTable *dec_saved;
	gboolean nrc_mode;
	gboolean meta_sends_escape;
	gboolean margin_bell;

	//normal_screen
	//alternate_screen
} RarShared;

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

	RarShared *shared;
};

#endif // __SCREEN_H__
