#include <glib-object.h>

#include "vterowdata.h"
#include "ring.h"
#include "terminal.h"
#include "debug.h"
#include "vtetc.h"
#include "screen.h"

#ifdef RARXXX // copied from vteseq.c
/* Typedef the handle type */
typedef void (*VteTerminalSequenceHandler) (RarOuter *outer, GValueArray *params);
#endif

#ifdef RARXXX // copied from vte-private.h
typedef enum {
	MOUSE_TRACKING_NONE,
	MOUSE_TRACKING_SEND_XY_ON_CLICK,
	MOUSE_TRACKING_SEND_XY_ON_BUTTON,
	MOUSE_TRACKING_HILITE_TRACKING,
	MOUSE_TRACKING_CELL_MOTION_TRACKING,
	MOUSE_TRACKING_ALL_MOTION_TRACKING
} MouseTrackingMode;

typedef enum _VteKeymode {
	VTE_KEYMODE_NORMAL,
	VTE_KEYMODE_APPLICATION
} VteKeymode;

#define VTE_COLOR_BRIGHT_OFFSET		8
#define VTE_TAB_MAX			999
#endif // RARXXX

/* Missing functions */
/**
 * _vte_invalidate_all
 */
/*static*/ void
_vte_invalidate_all(RarOuter *outer)
{
	printf ("\e[31mNOT IMPL\e[0m: %s\n", __FUNCTION__);
}

/**
 * _vte_invalidate_cells
 */
/*static*/ void
_vte_invalidate_cells(RarOuter *outer, glong column_start, gint column_count, glong row_start, gint row_count)
{
	printf ("\e[31mNOT IMPL\e[0m: %s\n", __FUNCTION__);
}

/**
 * _vte_terminal_adjust_adjustments
 */
/*static*/ void
_vte_terminal_adjust_adjustments(RarOuter *outer)
{
	printf ("\e[31mNOT IMPL\e[0m: %s\n", __FUNCTION__);
}

/**
 * _vte_terminal_cleanup_tab_fragments_at_cursor 
 */
/*static*/ void
_vte_terminal_cleanup_tab_fragments_at_cursor (RarOuter *outer)
{
	printf ("\e[31mNOT IMPL\e[0m: %s\n", __FUNCTION__);
}

/**
 * _vte_terminal_cursor_down 
 */
/*static*/ void
_vte_terminal_cursor_down (RarOuter *outer)
{
	//printf ("\e[31mNOT IMPL\e[0m: %s\n", __FUNCTION__);
	printf ("\n");
}

/**
 * _vte_terminal_emit_text_deleted
 */
/*static*/ void
_vte_terminal_emit_text_deleted(RarOuter *outer)
{
	printf ("\e[31mNOT IMPL\e[0m: %s\n", __FUNCTION__);
}

/**
 * _vte_terminal_emit_text_inserted
 */
/*static*/ void
_vte_terminal_emit_text_inserted(RarOuter *outer)
{
	printf ("\e[31mNOT IMPL\e[0m: %s\n", __FUNCTION__);
}

/**
 * _vte_terminal_ensure_row 
 */
/*static*/ VteRowData *
_vte_terminal_ensure_row (RarOuter *outer)
{
	printf ("\e[31mNOT IMPL\e[0m: %s\n", __FUNCTION__);
	return NULL;
}

/**
 * _vte_terminal_get_tabstop
 */
/*static*/ gboolean
_vte_terminal_get_tabstop(RarOuter *outer, int column)
{
	printf ("\e[31mNOT IMPL\e[0m: %s\n", __FUNCTION__);
	return FALSE;
}

/**
 * _vte_terminal_insert_char
 */
/*static*/ gboolean
_vte_terminal_insert_char(RarOuter *outer, gunichar c, gboolean insert, gboolean invalidate_now)
{
	//printf ("\e[31mNOT IMPL\e[0m: %s\n", __FUNCTION__);
	printf ("\e[33m%c\e[0m", c);
	return FALSE;
}

/**
 * _vte_terminal_queue_contents_changed
 */
/*static*/ void
_vte_terminal_queue_contents_changed(RarOuter *outer)
{
	printf ("\e[31mNOT IMPL\e[0m: %s\n", __FUNCTION__);
}

/**
 * vte_terminal_reset
 */
/*static*/ void
vte_terminal_reset(RarOuter *outer, gboolean clear_tabstops, gboolean clear_history)
{
	printf ("\e[31mNOT IMPL\e[0m: %s\n", __FUNCTION__);
}

/**
 * _vte_terminal_ring_append 
 */
/*static*/ VteRowData *
_vte_terminal_ring_append (RarOuter *outer, gboolean fill)
{
	printf ("\e[31mNOT IMPL\e[0m: %s\n", __FUNCTION__);
	return NULL;
}

/**
 * _vte_terminal_ring_insert 
 */
/*static*/ VteRowData *
_vte_terminal_ring_insert (RarOuter *outer, glong position, gboolean fill)
{
	printf ("\e[31mNOT IMPL\e[0m: %s\n", __FUNCTION__);
	return NULL;
}

/**
 * _vte_terminal_ring_remove 
 */
/*static*/ void
_vte_terminal_ring_remove (RarOuter *outer, glong position)
{
	printf ("\e[31mNOT IMPL\e[0m: %s\n", __FUNCTION__);
}

/**
 * _vte_terminal_scroll_region 
 */
/*static*/ void
_vte_terminal_scroll_region (RarOuter *outer, long row, glong count, glong delta)
{
	printf ("\e[31mNOT IMPL\e[0m: %s\n", __FUNCTION__);
}

/**
 * _vte_terminal_set_default_attributes
 */
/*static*/ void
_vte_terminal_set_default_attributes(RarOuter *outer)
{
	printf ("\e[31mNOT IMPL\e[0m: %s\n", __FUNCTION__);
}

/**
 * _vte_terminal_set_pointer_visible
 */
/*static*/ void
_vte_terminal_set_pointer_visible(RarOuter *outer, gboolean visible)
{
	printf ("\e[31mNOT IMPL\e[0m: %s\n", __FUNCTION__);
}

/**
 * vte_terminal_set_scrollback_lines
 */
/*static*/ void
vte_terminal_set_scrollback_lines(RarOuter *outer, glong lines)
{
	printf ("\e[31mNOT IMPL\e[0m: %s\n", __FUNCTION__);
}


/**
 * vte_sequence_handler_multiple
 *
 * Call another function a given number of times, or once.
 */
/*static*/ void
vte_sequence_handler_multiple(RarOuter *outer,
			      GValueArray *params,
			      VteTerminalSequenceHandler handler)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
	long val = 1;
	int i;
	GValue *value;

	if ((params != NULL) && (params->n_values > 0)) {
		value = g_value_array_get_nth(params, 0);
		if (G_VALUE_HOLDS_LONG(value)) {
			val = g_value_get_long(value);
			val = MAX(val, 1);	/* FIXME: vttest. */
		}
	}
	for (i = 0; i < val; i++)
		handler (outer, NULL);
}

/**
 * vte_sequence_handler_offset
 *
 * Call another handler, offsetting any long arguments by the given
 * increment value.
 */
/*static*/ void
vte_sequence_handler_offset(RarOuter *outer,
			    GValueArray *params,
			    int increment,
			    VteTerminalSequenceHandler handler)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
	guint i;
	long val;
	GValue *value;
	/* Decrement the parameters and let the _cs handler deal with it. */
	for (i = 0; (params != NULL) && (i < params->n_values); i++) {
		value = g_value_array_get_nth(params, i);
		if (G_VALUE_HOLDS_LONG(value)) {
			val = g_value_get_long(value);
			val += increment;
			g_value_set_long(value, val);
		}
	}
	handler (outer, params);
}

/**
 * _vte_terminal_clear_current_line
 *
 * Clear the current line.
 */
/*static*/ void
_vte_terminal_clear_current_line (RarOuter *outer)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
#ifndef RARXXX
	VteRowData *rowdata;

	/* If the cursor is actually on the screen, clear data in the row
	 * which corresponds to the cursor. */
	if (_vte_ring_next(outer->screen->row_data) > outer->screen->cursor_current.row) {
		/* Get the data for the row which the cursor points to. */
		rowdata = _vte_ring_index_writable (outer->screen->row_data, outer->screen->cursor_current.row);
		g_assert(rowdata != NULL);
		/* Remove it. */
		_vte_row_data_shrink (rowdata, 0);
		/* Add enough cells to the end of the line to fill out the row. */
		_vte_row_data_fill (rowdata, &outer->screen->fill_defaults, outer->column_count);
		rowdata->attr.soft_wrapped = 0;
		/* Repaint this row. */
		_vte_invalidate_cells(outer,
				      0, outer->column_count,
				      outer->screen->cursor_current.row, 1);
	}

	/* We've modified the display.  Make a note of it. */
	outer->text_deleted_flag = TRUE;
#endif
}

/**
 * _vte_terminal_clear_screen
 *
 * Clear the entire screen.
 */
/*static*/ void
_vte_terminal_clear_screen (RarOuter *outer)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
#ifndef RARXXX
	long i, initial, row;
	initial = outer->screen->insert_delta;
	row = outer->screen->cursor_current.row - outer->screen->insert_delta;
	initial = _vte_ring_next(outer->screen->row_data);
	/* Add a new screen's worth of rows. */
	for (i = 0; i < outer->row_count; i++)
		_vte_terminal_ring_append (outer, TRUE);
	/* Move the cursor and insertion delta to the first line in the
	 * newly-cleared area and scroll if need be. */
	outer->screen->insert_delta = initial;
	outer->screen->cursor_current.row = row + outer->screen->insert_delta;
	_vte_terminal_adjust_adjustments(outer);
	/* Redraw everything. */
	_vte_invalidate_all(outer);
	/* We've modified the display.  Make a note of it. */
	outer->text_deleted_flag = TRUE;
#endif
}

/**
 * vte_terminal_find_charcell
 *
 * Find the character an the given position in the backscroll buffer.
 */
/*static*/ VteCell *
vte_terminal_find_charcell (RarOuter *outer, glong col, glong row)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
	VteCell *ret = NULL;
#ifndef RARXXX
	VteRowData *rowdata;
	g_assert(RAR_IS_TERMINAL(outer));
	if (_vte_ring_contains (outer->screen->row_data, row)) {
		rowdata = _vte_ring_index_writable (outer->screen->row_data, row);
		ret = _vte_row_data_get_writable (rowdata, col);
	}
#endif
	return ret;
}

/**
 * _vte_terminal_home_cursor
 */
/*static*/ void
_vte_terminal_home_cursor (RarOuter *outer)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
#ifndef RARXXX
	outer->screen->cursor_current.row = outer->screen->insert_delta;
	outer->screen->cursor_current.col = 0;
#endif
}

/**
 * _vte_terminal_scroll_text
 *
 * Scroll the text, but don't move the cursor.  Negative = up, positive = down.
 */
/*static*/ void
_vte_terminal_scroll_text (RarOuter *outer, int scroll_amount)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
#ifndef RARXXX
	long start, end, i;

	if (outer->screen->scrolling_restricted) {
		start = outer->screen->insert_delta + outer->screen->scrolling_region.start;
		end = outer->screen->insert_delta + outer->screen->scrolling_region.end;
	} else {
		start = outer->screen->insert_delta;
		end = start + outer->row_count - 1;
	}

	while (_vte_ring_next(outer->screen->row_data) <= end)
		_vte_terminal_ring_append (outer, FALSE);

	if (scroll_amount > 0) {
		for (i = 0; i < scroll_amount; i++) {
			_vte_terminal_ring_remove (outer, end);
			_vte_terminal_ring_insert (outer, start, TRUE);
		}
	} else {
		for (i = 0; i < -scroll_amount; i++) {
			_vte_terminal_ring_remove (outer, start);
			_vte_terminal_ring_insert (outer, end, TRUE);
		}
	}

	/* Update the display. */
	_vte_terminal_scroll_region(outer, start, end - start + 1,
				   scroll_amount);

	/* Adjust the scrollbars if necessary. */
	_vte_terminal_adjust_adjustments(outer);

	/* We've modified the display.  Make a note of it. */
	outer->text_inserted_flag = TRUE;
	outer->text_deleted_flag = TRUE;
#endif
}

/**
 * vte_terminal_termcap_string_same_as_for
 */
/*static*/ gboolean
vte_terminal_termcap_string_same_as_for (RarOuter *outer,
					 const char  *cap_str,
					 const char  *cap_other)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
#ifndef RARXXX
	char *other_str;
	gboolean ret;

	other_str = _vte_termcap_find_string(outer->termcap,
					     outer->emulation,
					     cap_other);

	ret = other_str && (g_ascii_strcasecmp(cap_str, other_str) == 0);

	g_free (other_str);

	return ret;
#endif
	return FALSE;
}

/**
 * _vte_terminal_clear_above_current
 *
 * Clear above the current line.
 */
/*static*/ void
_vte_terminal_clear_above_current (RarOuter *outer)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
#ifndef RARXXX
	VteRowData *rowdata;
	long i;
	/* If the cursor is actually on the screen, clear data in the row
	 * which corresponds to the cursor. */
	for (i = outer->screen->insert_delta; i < outer->screen->cursor_current.row; i++) {
		if (_vte_ring_next(outer->screen->row_data) > i) {
			/* Get the data for the row we're erasing. */
			rowdata = _vte_ring_index_writable (outer->screen->row_data, i);
			g_assert(rowdata != NULL);
			/* Remove it. */
			_vte_row_data_shrink (rowdata, 0);
			/* Add new cells until we fill the row. */
			_vte_row_data_fill (rowdata, &outer->screen->fill_defaults, outer->column_count);
			rowdata->attr.soft_wrapped = 0;
			/* Repaint the row. */
			_vte_invalidate_cells(outer,
					0, outer->column_count, i, 1);
		}
	}
	/* We've modified the display.  Make a note of it. */
	outer->text_deleted_flag = TRUE;
#endif
}


/* Prototype all handlers... */
#define VTE_SEQUENCE_HANDLER(name) static void name (RarOuter *outer, GValueArray *params);
#include "rseq-list.h"
#undef VTE_SEQUENCE_HANDLER

/**
 * vte_sequence_handler_al - Add line
 *
 * Add a line at the current cursor position.
 */
static void
vte_sequence_handler_al (RarOuter *outer, GValueArray *params)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
	long start, end, param, i;
	GValue *value;

	/* Find out which part of the screen we're messing with. */
	start = outer->screen->cursor_current.row;
	if (outer->screen->scrolling_restricted) {
		end = outer->screen->insert_delta + outer->screen->scrolling_region.end;
	} else {
		end = outer->screen->insert_delta + outer->row_count - 1;
	}

	/* Extract any parameters. */
	param = 1;
	if ((params != NULL) && (params->n_values > 0)) {
		value = g_value_array_get_nth(params, 0);
		if (G_VALUE_HOLDS_LONG(value)) {
			param = g_value_get_long(value);
		}
	}

	/* Insert the right number of lines. */
	for (i = 0; i < param; i++) {
		/* Clear a line off the end of the region and add one to the
		 * top of the region. */
		_vte_terminal_ring_remove (outer, end);
		_vte_terminal_ring_insert (outer, start, TRUE);
		/* Adjust the scrollbars if necessary. */
		_vte_terminal_adjust_adjustments(outer);
	}

	/* Update the display. */
	_vte_terminal_scroll_region(outer, start, end - start + 1, param);

	/* We've modified the display.  Make a note of it. */
	outer->text_deleted_flag = TRUE;
}

/**
 * vte_sequence_handler_AL - Add lines
 *
 * Add N lines at the current cursor position.
 */
static void
vte_sequence_handler_AL (RarOuter *outer, GValueArray *params)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
	vte_sequence_handler_al (outer, params);
}

/**
 * vte_sequence_handler_bt - Backtab
 */
static void
vte_sequence_handler_bt (RarOuter *outer, GValueArray *params)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
	long newcol;

	/* Calculate which column is the previous tab stop. */
	newcol = outer->screen->cursor_current.col;

	if (outer->tabstops != NULL) {
		/* Find the next tabstop. */
		while (newcol >= 0) {
			if (_vte_terminal_get_tabstop(outer,
						     newcol % outer->column_count)) {
				break;
			}
			newcol--;
		}
	}

	/* If we have no tab stops, stop at the first column. */
	if (newcol <= 0) {
		newcol = 0;
	}

	/* Warp the cursor. */
	_vte_debug_print(VTE_DEBUG_PARSE,
			"Moving cursor to column %ld.\n", (long)newcol);
	outer->screen->cursor_current.col = newcol;
}

/**
 * vte_sequence_handler_cb - Clear to beginning
 *
 * Clear from the cursor position to the beginning of the line.
 */
static void
vte_sequence_handler_cb (RarOuter *outer, GValueArray *params)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
	VteRowData *rowdata;
	long i;
	VteCell *pcell;

	/* Get the data for the row which the cursor points to. */
	rowdata = _vte_terminal_ensure_row(outer);
	/* Clear the data up to the current column with the default
	 * attributes.  If there is no such character cell, we need
	 * to add one. */
	for (i = 0; i <= outer->screen->cursor_current.col; i++) {
		if (i < (glong) _vte_row_data_length (rowdata)) {
			/* Muck with the cell in this location. */
			pcell = _vte_row_data_get_writable (rowdata, i);
			*pcell = outer->screen->color_defaults;
		} else {
			/* Add new cells until we have one here. */
			_vte_row_data_append (rowdata, &outer->screen->color_defaults);
		}
	}
	/* Repaint this row. */
	_vte_invalidate_cells(outer,
			      0, outer->screen->cursor_current.col+1,
			      outer->screen->cursor_current.row, 1);

	/* We've modified the display.  Make a note of it. */
	outer->text_deleted_flag = TRUE;
}

/**
 * vte_sequence_handler_cd - Clear down
 *
 * Clear to the right of the cursor and below the current line.
 */
static void
vte_sequence_handler_cd (RarOuter *outer, GValueArray *params)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
	VteRowData *rowdata;
	glong i;

	/* If the cursor is actually on the screen, clear the rest of the
	 * row the cursor is on and all of the rows below the cursor. */
	i = outer->screen->cursor_current.row;
	if (i < _vte_ring_next(outer->screen->row_data)) {
		/* Get the data for the row we're clipping. */
		rowdata = _vte_ring_index_writable (outer->screen->row_data, i);
		/* Clear everything to the right of the cursor. */
		if (rowdata)
			_vte_row_data_shrink (rowdata, outer->screen->cursor_current.col);
	}
	/* Now for the rest of the lines. */
	for (i = outer->screen->cursor_current.row + 1;
	     i < _vte_ring_next(outer->screen->row_data);
	     i++) {
		/* Get the data for the row we're removing. */
		rowdata = _vte_ring_index_writable (outer->screen->row_data, i);
		/* Remove it. */
		if (rowdata)
			_vte_row_data_shrink (rowdata, 0);
	}
	/* Now fill the cleared areas. */
	for (i = outer->screen->cursor_current.row;
	     i < outer->screen->insert_delta + outer->row_count;
	     i++) {
		/* Retrieve the row's data, creating it if necessary. */
		if (_vte_ring_contains (outer->screen->row_data, i)) {
			rowdata = _vte_ring_index_writable (outer->screen->row_data, i);
			g_assert(rowdata != NULL);
		} else {
			rowdata = _vte_terminal_ring_append (outer, FALSE);
		}
		/* Pad out the row. */
		_vte_row_data_fill (rowdata, &outer->screen->fill_defaults, outer->column_count);
		rowdata->attr.soft_wrapped = 0;
		/* Repaint this row. */
		_vte_invalidate_cells(outer,
				      0, outer->column_count,
				      i, 1);
	}

	/* We've modified the display.  Make a note of it. */
	outer->text_deleted_flag = TRUE;
}

/**
 * vte_sequence_handler_ce
 *
 * Clear from the cursor position to the end of the line.
 */
static void
vte_sequence_handler_ce (RarOuter *outer, GValueArray *params)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
	VteRowData *rowdata;

	/* Get the data for the row which the cursor points to. */
	rowdata = _vte_terminal_ensure_row(outer);
	g_assert(rowdata != NULL);
	/* Remove the data at the end of the array until the current column
	 * is the end of the array. */
	if ((glong) _vte_row_data_length (rowdata) > outer->screen->cursor_current.col) {
		_vte_row_data_shrink (rowdata, outer->screen->cursor_current.col);
		/* We've modified the display.  Make a note of it. */
		outer->text_deleted_flag = TRUE;
	}
	if (outer->screen->fill_defaults.attr.back != VTE_DEF_BG) {
		/* Add enough cells to fill out the row. */
		_vte_row_data_fill (rowdata, &outer->screen->fill_defaults, outer->column_count);
	}
	rowdata->attr.soft_wrapped = 0;
	/* Repaint this row. */
	_vte_invalidate_cells(outer,
			      outer->screen->cursor_current.col,
			      outer->column_count -
			      outer->screen->cursor_current.col,
			      outer->screen->cursor_current.row, 1);
}

/**
 * vte_sequence_handler_ch
 *
 * Move the cursor to the given column (horizontal position).
 */
static void
vte_sequence_handler_ch (RarOuter *outer, GValueArray *params)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
	GValue *value;
	long val;

	/* We only care if there's a parameter in there. */
	if ((params != NULL) && (params->n_values > 0)) {
		value = g_value_array_get_nth(params, 0);
		if (G_VALUE_HOLDS_LONG(value)) {
			val = CLAMP(g_value_get_long(value),
				    0,
				    outer->column_count - 1);
			/* Move the cursor. */
			outer->screen->cursor_current.col = val;
			_vte_terminal_cleanup_tab_fragments_at_cursor (outer);
		}
	}
}

/**
 * vte_sequence_handler_cl
 *
 * Clear the screen and home the cursor.
 */
static void
vte_sequence_handler_cl (RarOuter *outer, GValueArray *params)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
	_vte_terminal_clear_screen (outer);
	_vte_terminal_home_cursor (outer);

	/* We've modified the display.  Make a note of it. */
	outer->text_deleted_flag = TRUE;
}

/**
 * vte_sequence_handler_cm
 *
 * Move the cursor to the given position.
 */
static void
vte_sequence_handler_cm (RarOuter *outer, GValueArray *params)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
	GValue *row, *col;
	long rowval, colval, origin;

	/* We need at least two parameters. */
	rowval = colval = 0;
	if (params != NULL && params->n_values >= 1) {
		/* The first is the row, the second is the column. */
		row = g_value_array_get_nth(params, 0);
		if (G_VALUE_HOLDS_LONG(row)) {
			if (outer->screen->origin_mode &&
			    outer->screen->scrolling_restricted) {
				origin = outer->screen->scrolling_region.start;
			} else {
				origin = 0;
			}
			rowval = g_value_get_long(row) + origin;
			rowval = CLAMP(rowval, 0, outer->row_count - 1);
		}
		if (params->n_values >= 2) {
			col = g_value_array_get_nth(params, 1);
			if (G_VALUE_HOLDS_LONG(col)) {
				colval = g_value_get_long(col);
				colval = CLAMP(colval, 0, outer->column_count - 1);
			}
		}
	}
	outer->screen->cursor_current.row = rowval + outer->screen->insert_delta;
	outer->screen->cursor_current.col = colval;
	_vte_terminal_cleanup_tab_fragments_at_cursor (outer);
}

/**
 * vte_sequence_handler_cr - Carriage return
 */
static void
vte_sequence_handler_cr (RarOuter *outer, GValueArray *params)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
	outer->screen->cursor_current.col = 0;
}

/**
 * vte_sequence_handler_cs
 *
 * Restrict scrolling and updates to a subset of the visible lines.
 */
static void
vte_sequence_handler_cs (RarOuter *outer, GValueArray *params)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
	long start=-1, end=-1, rows;
	GValue *value;

	_vte_terminal_home_cursor (outer);

	/* We require two parameters.  Anything less is a reset. */
	if ((params == NULL) || (params->n_values < 2)) {
		outer->screen->scrolling_restricted = FALSE;
		return;
	}
	/* Extract the two values. */
	value = g_value_array_get_nth(params, 0);
	if (G_VALUE_HOLDS_LONG(value)) {
		start = g_value_get_long(value);
	}
	value = g_value_array_get_nth(params, 1);
	if (G_VALUE_HOLDS_LONG(value)) {
		end = g_value_get_long(value);
	}
	/* Catch garbage. */
	rows = outer->row_count;
	if (start <= 0 || start >= rows) {
		start = 0;
	}
	if (end <= 0 || end >= rows) {
		end = rows - 1;
	}
	/* Set the right values. */
	outer->screen->scrolling_region.start = start;
	outer->screen->scrolling_region.end = end;
	outer->screen->scrolling_restricted = TRUE;
	/* Special case -- run wild, run free. */
	if (outer->screen->scrolling_region.start == 0 &&
	    outer->screen->scrolling_region.end == rows - 1) {
		outer->screen->scrolling_restricted = FALSE;
	}
}

/**
 * vte_sequence_handler_cS
 *
 * Restrict scrolling and updates to a subset of the visible lines, because
 * GNU Emacs is special.
 */
static void
vte_sequence_handler_cS (RarOuter *outer, GValueArray *params)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
	long start=0, end=outer->row_count-1, rows;
	GValue *value;

	/* We require four parameters. */
	if ((params == NULL) || (params->n_values < 2)) {
		outer->screen->scrolling_restricted = FALSE;
		return;
	}
	/* Extract the two parameters we care about, encoded as the number
	 * of lines above and below the scrolling region, respectively. */
	value = g_value_array_get_nth(params, 1);
	if (G_VALUE_HOLDS_LONG(value)) {
		start = g_value_get_long(value);
	}
	value = g_value_array_get_nth(params, 2);
	if (G_VALUE_HOLDS_LONG(value)) {
		end -= g_value_get_long(value);
	}
	/* Set the right values. */
	outer->screen->scrolling_region.start = start;
	outer->screen->scrolling_region.end = end;
	outer->screen->scrolling_restricted = TRUE;
	/* Special case -- run wild, run free. */
	rows = outer->row_count;
	if ((outer->screen->scrolling_region.start == 0) &&
	    (outer->screen->scrolling_region.end == rows - 1)) {
		outer->screen->scrolling_restricted = FALSE;
	}
	/* Clamp the cursor to the scrolling region. */
	outer->screen->cursor_current.row = CLAMP(outer->screen->cursor_current.row,
					   outer->screen->insert_delta + start,
					   outer->screen->insert_delta + end);
}

/**
 * vte_sequence_handler_cv
 *
 * Move the cursor to the given row (vertical position).
 */
static void
vte_sequence_handler_cv (RarOuter *outer, GValueArray *params)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
	GValue *value;
	long val, origin;
	/* We only care if there's a parameter in there. */
	if ((params != NULL) && (params->n_values > 0)) {
		value = g_value_array_get_nth(params, 0);
		if (G_VALUE_HOLDS_LONG(value)) {
			/* Move the cursor. */
			if (outer->screen->origin_mode &&
			    outer->screen->scrolling_restricted) {
				origin = outer->screen->scrolling_region.start;
			} else {
				origin = 0;
			}
			val = g_value_get_long(value) + origin;
			val = CLAMP(val, 0, outer->row_count - 1);
			outer->screen->cursor_current.row = outer->screen->insert_delta + val;
		}
	}
}

/**
 * vte_sequence_handler_dc
 *
 * Delete a character at the current cursor position.
 */
static void
vte_sequence_handler_dc (RarOuter *outer, GValueArray *params)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
	VteRowData *rowdata;
	long col;

	if (_vte_ring_next(outer->screen->row_data) > outer->screen->cursor_current.row) {
		long len;
		/* Get the data for the row which the cursor points to. */
		rowdata = _vte_ring_index_writable (outer->screen->row_data, outer->screen->cursor_current.row);
		g_assert(rowdata != NULL);
		col = outer->screen->cursor_current.col;
		len = _vte_row_data_length (rowdata);
		/* Remove the column. */
		if (col < len) {
			_vte_row_data_remove (rowdata, col);
			if (outer->screen->fill_defaults.attr.back != VTE_DEF_BG) {
				_vte_row_data_fill (rowdata, &outer->screen->fill_defaults, outer->column_count);
				len = outer->column_count;
			}
			/* Repaint this row. */
			_vte_invalidate_cells(outer,
					col, len - col,
					outer->screen->cursor_current.row, 1);
		}
	}

	/* We've modified the display.  Make a note of it. */
	outer->text_deleted_flag = TRUE;
}

/**
 * vte_sequence_handler_DC
 *
 * Delete N characters at the current cursor position.
 */
static void
vte_sequence_handler_DC (RarOuter *outer, GValueArray *params)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
	vte_sequence_handler_multiple(outer, params, vte_sequence_handler_dc);
}

/**
 * vte_sequence_handler_dl
 *
 * Delete a line at the current cursor position.
 */
static void
vte_sequence_handler_dl (RarOuter *outer, GValueArray *params)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
	long start, end, param, i;
	GValue *value;

	/* Find out which part of the screen we're messing with. */
	start = outer->screen->cursor_current.row;
	if (outer->screen->scrolling_restricted) {
		end = outer->screen->insert_delta + outer->screen->scrolling_region.end;
	} else {
		end = outer->screen->insert_delta + outer->row_count - 1;
	}

	/* Extract any parameters. */
	param = 1;
	if ((params != NULL) && (params->n_values > 0)) {
		value = g_value_array_get_nth(params, 0);
		if (G_VALUE_HOLDS_LONG(value)) {
			param = g_value_get_long(value);
		}
	}

	/* Delete the right number of lines. */
	for (i = 0; i < param; i++) {
		/* Clear a line off the end of the region and add one to the
		 * top of the region. */
		_vte_terminal_ring_remove (outer, start);
		_vte_terminal_ring_insert (outer, end, TRUE);
		/* Adjust the scrollbars if necessary. */
		_vte_terminal_adjust_adjustments(outer);
	}

	/* Update the display. */
	_vte_terminal_scroll_region(outer, start, end - start + 1, -param);

	/* We've modified the display.  Make a note of it. */
	outer->text_deleted_flag = TRUE;
}

/**
 * vte_sequence_handler_DL
 *
 * Delete N lines at the current cursor position.
 */
static void
vte_sequence_handler_DL (RarOuter *outer, GValueArray *params)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
	vte_sequence_handler_dl (outer, params);
}

/**
 * vte_sequence_handler_do - Cursor down
 *
 * Cursor down, no scrolling.
 */
static void
vte_sequence_handler_do (RarOuter *outer, GValueArray *params)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
	long start, end;

	if (outer->screen->scrolling_restricted) {
		start = outer->screen->insert_delta + outer->screen->scrolling_region.start;
		end = outer->screen->insert_delta + outer->screen->scrolling_region.end;
	} else {
		start = outer->screen->insert_delta;
		end = start + outer->row_count - 1;
	}

	/* Move the cursor down. */
	outer->screen->cursor_current.row = MIN(outer->screen->cursor_current.row + 1, end);
}

/**
 * vte_sequence_handler_DO
 *
 * Cursor down, no scrolling.
 */
static void
vte_sequence_handler_DO (RarOuter *outer, GValueArray *params)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
	vte_sequence_handler_multiple(outer, params, vte_sequence_handler_do);
}

/**
 * vte_sequence_handler_ec
 *
 * Erase characters starting at the cursor position (overwriting N with
 * spaces, but not moving the cursor).
 */
static void
vte_sequence_handler_ec (RarOuter *outer, GValueArray *params)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
	VteRowData *rowdata;
	GValue *value;
	VteCell *cell;
	long col, i, count;

	/* If we got a parameter, use it. */
	count = 1;
	if ((params != NULL) && (params->n_values > 0)) {
		value = g_value_array_get_nth(params, 0);
		if (G_VALUE_HOLDS_LONG(value)) {
			count = g_value_get_long(value);
		}
	}

	/* Clear out the given number of characters. */
	rowdata = _vte_terminal_ensure_row(outer);
	if (_vte_ring_next(outer->screen->row_data) > outer->screen->cursor_current.row) {
		g_assert(rowdata != NULL);
		/* Write over the characters.  (If there aren't enough, we'll
		 * need to create them.) */
		for (i = 0; i < count; i++) {
			col = outer->screen->cursor_current.col + i;
			if (col >= 0) {
				if (col < (glong) _vte_row_data_length (rowdata)) {
					/* Replace this cell with the current
					 * defaults. */
					cell = _vte_row_data_get_writable (rowdata, col);
					*cell = outer->screen->color_defaults;
				} else {
					/* Add new cells until we have one here. */
					_vte_row_data_fill (rowdata, &outer->screen->color_defaults, col);
				}
			}
		}
		/* Repaint this row. */
		_vte_invalidate_cells(outer,
				      outer->screen->cursor_current.col, count,
				      outer->screen->cursor_current.row, 1);
	}

	/* We've modified the display.  Make a note of it. */
	outer->text_deleted_flag = TRUE;
}

/**
 * vte_sequence_handler_fs
 *
 * Move from status line.
 */
static void
vte_sequence_handler_fs (RarOuter *outer, GValueArray *params)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
	outer->screen->status_line = FALSE;
}

/**
 * vte_sequence_handler_ho
 *
 * Move the cursor to the home position.
 */
static void
vte_sequence_handler_ho (RarOuter *outer, GValueArray *params)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
	_vte_terminal_home_cursor (outer);
}

/**
 * vte_sequence_handler_ic
 *
 * Insert a character.
 */
static void
vte_sequence_handler_ic (RarOuter *outer, GValueArray *params)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
	VteVisualPosition save;

	save = outer->screen->cursor_current;

	_vte_terminal_insert_char(outer, ' ', TRUE, TRUE);

	outer->screen->cursor_current = save;
}

/**
 * vte_sequence_handler_IC
 *
 * Insert N characters.
 */
static void
vte_sequence_handler_IC (RarOuter *outer, GValueArray *params)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
	vte_sequence_handler_multiple(outer, params, vte_sequence_handler_ic);
}

/**
 * vte_sequence_handler_kb
 *
 * Send me a backspace key sym, will you?  Guess that the application meant
 * to send the cursor back one position.
 */
static void
vte_sequence_handler_kb (RarOuter *outer, GValueArray *params)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
	/* Move the cursor left. */
	vte_sequence_handler_le (outer, params);
}

/**
 * vte_sequence_handler_ke
 *
 * Keypad mode end.
 */
static void
vte_sequence_handler_ke (RarOuter *outer, GValueArray *params)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
	outer->keypad_mode = VTE_KEYMODE_NORMAL;
}

/**
 * vte_sequence_handler_ks
 *
 * Keypad mode start.
 */
static void
vte_sequence_handler_ks (RarOuter *outer, GValueArray *params)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
	outer->keypad_mode = VTE_KEYMODE_APPLICATION;
}

/**
 * vte_sequence_handler_le - Cursor left
 */
static void
vte_sequence_handler_le (RarOuter *outer, GValueArray *params)
{
	//printf ("Entering: %s\n", __FUNCTION__);e

	if (outer->screen->cursor_current.col > 0) {
		/* There's room to move left, so do so. */
		outer->screen->cursor_current.col--;
		_vte_terminal_cleanup_tab_fragments_at_cursor (outer);
	} else {
		if (outer->flags.bw) {
			/* Wrap to the previous line. */
			outer->screen->cursor_current.col = outer->column_count - 1;
			if (outer->screen->scrolling_restricted) {
				vte_sequence_handler_sr (outer, params);
			} else {
				outer->screen->cursor_current.row = MAX(outer->screen->cursor_current.row - 1,
								 outer->screen->insert_delta);
			}
		} else {
			/* Stick to the first column. */
			outer->screen->cursor_current.col = 0;
		}
	}
}

/**
 * vte_sequence_handler_LE
 *
 * Move the cursor left N columns.
 */
static void
vte_sequence_handler_LE (RarOuter *outer, GValueArray *params)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
	vte_sequence_handler_multiple(outer, params, vte_sequence_handler_le);
}

/**
 * vte_sequence_handler_ll
 *
 * Move the cursor to the lower left corner of the display.
 */
static void
vte_sequence_handler_ll (RarOuter *outer, GValueArray *params)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
	outer->screen->cursor_current.row = MAX(outer->screen->insert_delta,
					 outer->screen->insert_delta +
					 outer->row_count - 1);
	outer->screen->cursor_current.col = 0;
}

/**
 * vte_sequence_handler_mb
 *
 * Blink on.
 */
static void
vte_sequence_handler_mb (RarOuter *outer, GValueArray *params)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
	outer->screen->defaults.attr.blink = 1;
}

/**
 * vte_sequence_handler_md
 *
 * Bold on.
 */
static void
vte_sequence_handler_md (RarOuter *outer, GValueArray *params)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
	outer->screen->defaults.attr.bold = 1;
	outer->screen->defaults.attr.half = 0;
}

/**
 * vte_sequence_handler_me
 *
 * End modes.
 */
static void
vte_sequence_handler_me (RarOuter *outer, GValueArray *params)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
	_vte_terminal_set_default_attributes(outer);
}

/**
 * vte_sequence_handler_mh
 *
 * Half-bright on.
 */
static void
vte_sequence_handler_mh (RarOuter *outer, GValueArray *params)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
	outer->screen->defaults.attr.half = 1;
	outer->screen->defaults.attr.bold = 0;
}

/**
 * vte_sequence_handler_mk
 *
 * Invisible on.
 */
static void
vte_sequence_handler_mk (RarOuter *outer, GValueArray *params)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
	outer->screen->defaults.attr.invisible = 1;
}

/**
 * vte_sequence_handler_mr
 *
 * Reverse on.
 */
static void
vte_sequence_handler_mr (RarOuter *outer, GValueArray *params)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
	outer->screen->defaults.attr.reverse = 1;
}

/**
 * vte_sequence_handler_nd
 *
 * Cursor right.
 */
static void
vte_sequence_handler_nd (RarOuter *outer, GValueArray *params)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
	if ((outer->screen->cursor_current.col + 1) < outer->column_count) {
		/* There's room to move right. */
		outer->screen->cursor_current.col++;
	}
}

/**
 * vte_sequence_handler_nw
 *
 * Carriage return command(?).
 */
static void
vte_sequence_handler_nw (RarOuter *outer, GValueArray *params)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
	vte_sequence_handler_cr (outer, params);
}

/**
 * vte_sequence_handler_rc
 *
 * Restore cursor (position).
 */
static void
vte_sequence_handler_rc (RarOuter *outer, GValueArray *params)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
	outer->screen->cursor_current.col = outer->screen->cursor_saved.col;
	outer->screen->cursor_current.row = CLAMP(outer->screen->cursor_saved.row +
					   outer->screen->insert_delta,
					   outer->screen->insert_delta,
					   outer->screen->insert_delta +
					   outer->row_count - 1);
}

/**
 * vte_sequence_handler_RI
 *
 * Cursor right N characters.
 */
static void
vte_sequence_handler_RI (RarOuter *outer, GValueArray *params)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
	vte_sequence_handler_multiple(outer, params, vte_sequence_handler_nd);
}

/**
 * vte_sequence_handler_sc
 *
 * Save cursor (position).
 */
static void
vte_sequence_handler_sc (RarOuter *outer, GValueArray *params)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
	outer->screen->cursor_saved.col = outer->screen->cursor_current.col;
	outer->screen->cursor_saved.row = CLAMP(outer->screen->cursor_current.row -
					 outer->screen->insert_delta,
					 0, outer->row_count - 1);
}

/**
 * vte_sequence_handler_se
 *
 * Standout end.
 */
static void
vte_sequence_handler_se (RarOuter *outer, GValueArray *params)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
	char *standout;

	/* Standout may be mapped to another attribute, so attempt to do
	 * the Right Thing here.
	 *
	 * If the standout sequence is the same as another sequence, do what
	 * we'd do for that other sequence instead. */

	standout = _vte_termcap_find_string(outer->termcap,
					    outer->emulation,
					    "so");
	g_assert(standout != NULL);

	if (vte_terminal_termcap_string_same_as_for (outer, standout, "mb") /* blink */   ||
	    vte_terminal_termcap_string_same_as_for (outer, standout, "md") /* bold */    ||
	    vte_terminal_termcap_string_same_as_for (outer, standout, "mh") /* half */    ||
	    vte_terminal_termcap_string_same_as_for (outer, standout, "mr") /* reverse */ ||
	    vte_terminal_termcap_string_same_as_for (outer, standout, "us") /* underline */)
	{
		vte_sequence_handler_me (outer, params);
	} else {
		/* Otherwise just set standout mode. */
		outer->screen->defaults.attr.standout = 0;
	}

	g_free(standout);
}

/**
 * vte_sequence_handler_sf
 *
 * Cursor down, with scrolling.
 */
static void
vte_sequence_handler_sf (RarOuter *outer, GValueArray *params)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
	_vte_terminal_cursor_down (outer);
}

/**
 * vte_sequence_handler_SF
 *
 * Cursor down, with scrolling.
 */
static void
vte_sequence_handler_SF (RarOuter *outer, GValueArray *params)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
	/* XXX implement this directly in _vte_terminal_cursor_down */
	vte_sequence_handler_multiple(outer, params, vte_sequence_handler_sf);
}

/**
 * vte_sequence_handler_so
 *
 * Standout start.
 */
static void
vte_sequence_handler_so (RarOuter *outer, GValueArray *params)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
	char *standout;

	/* Standout may be mapped to another attribute, so attempt to do
	 * the Right Thing here.
	 *
	 * If the standout sequence is the same as another sequence, do what
	 * we'd do for that other sequence instead. */

	standout = _vte_termcap_find_string(outer->termcap,
					    outer->emulation,
					    "so");
	g_assert(standout != NULL);

	if (vte_terminal_termcap_string_same_as_for (outer, standout, "mb") /* blink */)
		vte_sequence_handler_mb (outer, params);
	else if (vte_terminal_termcap_string_same_as_for (outer, standout, "md") /* bold */)
		vte_sequence_handler_md (outer, params);
	else if (vte_terminal_termcap_string_same_as_for (outer, standout, "mh") /* half */)
		vte_sequence_handler_mh (outer, params);
	else if (vte_terminal_termcap_string_same_as_for (outer, standout, "mr") /* reverse */)
		vte_sequence_handler_mr (outer, params);
	else if (vte_terminal_termcap_string_same_as_for (outer, standout, "us") /* underline */)
		vte_sequence_handler_us (outer, params);
	else {
		/* Otherwise just set standout mode. */
		outer->screen->defaults.attr.standout = 1;
	}

	g_free(standout);
}

/**
 * vte_sequence_handler_sr
 *
 * Cursor up, scrolling if need be.
 */
static void
vte_sequence_handler_sr (RarOuter *outer, GValueArray *params)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
	long start, end;

	if (outer->screen->scrolling_restricted) {
		start = outer->screen->scrolling_region.start + outer->screen->insert_delta;
		end = outer->screen->scrolling_region.end + outer->screen->insert_delta;
	} else {
		start = outer->screen->insert_delta;
		end = start + outer->row_count - 1;
	}

	if (outer->screen->cursor_current.row == start) {
		/* If we're at the top of the scrolling region, add a
		 * line at the top to scroll the bottom off. */
		_vte_terminal_ring_remove (outer, end);
		_vte_terminal_ring_insert (outer, start, TRUE);
		/* Update the display. */
		_vte_terminal_scroll_region(outer, start, end - start + 1, 1);
		_vte_invalidate_cells(outer,
				      0, outer->column_count,
				      start, 2);
	} else {
		/* Otherwise, just move the cursor up. */
		outer->screen->cursor_current.row--;
	}
	/* Adjust the scrollbars if necessary. */
	_vte_terminal_adjust_adjustments(outer);
	/* We modified the display, so make a note of it. */
	outer->text_modified_flag = TRUE;
}

/**
 * vte_sequence_handler_SR
 *
 * Cursor up, with scrolling.
 */
static void
vte_sequence_handler_SR (RarOuter *outer, GValueArray *params)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
	vte_sequence_handler_multiple(outer, params, vte_sequence_handler_sr);
}

/**
 * vte_sequence_handler_ta
 *
 * Tab.
 */
static void
vte_sequence_handler_ta (RarOuter *outer, GValueArray *params)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
	long old_len, newcol, col;

	/* Calculate which column is the next tab stop. */
	newcol = col = outer->screen->cursor_current.col;

	g_assert (col >= 0);

	if (outer->tabstops != NULL) {
		/* Find the next tabstop. */
		for (newcol++; newcol < VTE_TAB_MAX; newcol++) {
			if (_vte_terminal_get_tabstop(outer, newcol)) {
				break;
			}
		}
	}

	/* If we have no tab stops or went past the end of the line, stop
	 * at the right-most column. */
	if (newcol >= outer->column_count) {
		newcol = outer->column_count - 1;
	}

	/* but make sure we don't move cursor back (bug #340631) */
	if (col < newcol) {
		VteRowData *rowdata = _vte_terminal_ensure_row (outer);

		/* Smart tab handling: bug 353610
		 *
		 * If we currently don't have any cells in the space this
		 * tab creates, we try to make the tab character copyable,
		 * by appending a single tab char with lots of fragment
		 * cells following it.
		 *
		 * Otherwise, just append empty cells that will show up
		 * as a space each.
		 */

		old_len = _vte_row_data_length (rowdata);
		_vte_row_data_fill (rowdata, &outer->screen->fill_defaults, newcol);

		/* Insert smart tab if there's nothing in the line after
		 * us.  Though, there may be empty cells (with non-default
		 * background color for example.
		 *
		 * Notable bugs here: 545924 and 597242 */
		{
			glong i;
			gboolean found = FALSE;
			for (i = old_len; i > col; i--) {
				const VteCell *cell = _vte_row_data_get (rowdata, i - 1);
				if (cell->attr.fragment || cell->c != 0) {
					found = TRUE;
					break;
				}
			}
			/* Nothing found on the line after us, turn this into
			 * a smart tab */
			if (!found) {
				VteCell *cell = _vte_row_data_get_writable (rowdata, col);
				VteCell tab = *cell;
				tab.attr.columns = newcol - col;
				tab.c = '\t';
				/* Check if it fits in columns */
				if (tab.attr.columns == newcol - col) {
					/* Save tab char */
					*cell = tab;
					/* And adjust the fragments */
					for (i = col + 1; i < newcol; i++) {
						cell = _vte_row_data_get_writable (rowdata, i);
						cell->c = '\t';
						cell->attr.columns = 1;
						cell->attr.fragment = 1;
					}
				}
			}
		}

		_vte_invalidate_cells (outer,
				outer->screen->cursor_current.col,
				newcol - outer->screen->cursor_current.col,
				outer->screen->cursor_current.row, 1);
		outer->screen->cursor_current.col = newcol;
	}
}

/**
 * vte_sequence_handler_ts
 *
 * Move to status line.
 */
static void
vte_sequence_handler_ts (RarOuter *outer, GValueArray *params)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
	outer->screen->status_line = TRUE;
	outer->screen->status_line_changed = TRUE;
	g_string_truncate(outer->screen->status_line_contents, 0);
}

/**
 * vte_sequence_handler_uc
 *
 * Underline this character and move right.
 */
static void
vte_sequence_handler_uc (RarOuter *outer, GValueArray *params)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
	VteCell *cell;
	int column;

	column = outer->screen->cursor_current.col;
	cell = vte_terminal_find_charcell(outer, column, outer->screen->cursor_current.row);
	while ((cell != NULL) && (cell->attr.fragment) && (column > 0)) {
		column--;
		cell = vte_terminal_find_charcell(outer, column, outer->screen->cursor_current.row);
	}
	if (cell != NULL) {
		/* Set this character to be underlined. */
		cell->attr.underline = 1;
		/* Cause the character to be repainted. */
		_vte_invalidate_cells(outer,
				      column, cell->attr.columns,
				      outer->screen->cursor_current.row, 1);
		/* Move the cursor right. */
		vte_sequence_handler_nd (outer, params);
	}

	/* We've modified the display without changing the text.  Make a note
	 * of it. */
	outer->text_modified_flag = TRUE;
}

/**
 * vte_sequence_handler_ue
 *
 * Underline end.
 */
static void
vte_sequence_handler_ue (RarOuter *outer, GValueArray *params)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
	outer->screen->defaults.attr.underline = 0;
}

/**
 * vte_sequence_handler_up
 *
 * Cursor up, no scrolling.
 */
static void
vte_sequence_handler_up (RarOuter *outer, GValueArray *params)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
	long start, end;

	if (outer->screen->scrolling_restricted) {
		start = outer->screen->insert_delta + outer->screen->scrolling_region.start;
		end = outer->screen->insert_delta + outer->screen->scrolling_region.end;
	} else {
		start = outer->screen->insert_delta;
		end = start + outer->row_count - 1;
	}

	outer->screen->cursor_current.row = MAX(outer->screen->cursor_current.row - 1, start);
}

/**
 * vte_sequence_handler_UP
 *
 * Cursor up N lines, no scrolling.
 */
static void
vte_sequence_handler_UP (RarOuter *outer, GValueArray *params)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
	vte_sequence_handler_multiple(outer, params, vte_sequence_handler_up);
}

/**
 * vte_sequence_handler_us
 *
 * Underline start.
 */
static void
vte_sequence_handler_us (RarOuter *outer, GValueArray *params)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
	outer->screen->defaults.attr.underline = 1;
}


/**
 * vte_sequence_handler_character_attributes
 *
 * Handle ANSI color setting and related stuffs (SGR).
 */
static void
vte_sequence_handler_character_attributes (RarOuter *outer, GValueArray *params)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
	unsigned int i;
	GValue *value;
	long param;
	/* The default parameter is zero. */
	param = 0;
	/* Step through each numeric parameter. */
	for (i = 0; (params != NULL) && (i < params->n_values); i++) {
		/* If this parameter isn't a number, skip it. */
		value = g_value_array_get_nth(params, i);
		if (!G_VALUE_HOLDS_LONG(value)) {
			continue;
		}
		param = g_value_get_long(value);
		switch (param) {
		case 0:
			_vte_terminal_set_default_attributes(outer);
			break;
		case 1:
			outer->screen->defaults.attr.bold = 1;
			outer->screen->defaults.attr.half = 0;
			break;
		case 2:
			outer->screen->defaults.attr.half = 1;
			outer->screen->defaults.attr.bold = 0;
			break;
		case 4:
			outer->screen->defaults.attr.underline = 1;
			break;
		case 5:
			outer->screen->defaults.attr.blink = 1;
			break;
		case 7:
			outer->screen->defaults.attr.reverse = 1;
			break;
		case 8:
			outer->screen->defaults.attr.invisible = 1;
			break;
		case 9:
			outer->screen->defaults.attr.strikethrough = 1;
			break;
		case 21: /* Error in old versions of linux console. */
		case 22: /* ECMA 48. */
			outer->screen->defaults.attr.bold = 0;
			outer->screen->defaults.attr.half = 0;
			break;
		case 24:
			outer->screen->defaults.attr.underline = 0;
			break;
		case 25:
			outer->screen->defaults.attr.blink = 0;
			break;
		case 27:
			outer->screen->defaults.attr.reverse = 0;
			break;
		case 28:
			outer->screen->defaults.attr.invisible = 0;
			break;
		case 29:
			outer->screen->defaults.attr.strikethrough = 0;
			break;
		case 30:
		case 31:
		case 32:
		case 33:
		case 34:
		case 35:
		case 36:
		case 37:
			outer->screen->defaults.attr.fore = param - 30;
			break;
		case 38:
		{
			/* The format looks like: ^[[38;5;COLORNUMBERm,
			   so look for COLORNUMBER here. */
			if ((i + 2) < params->n_values){
				GValue *value1, *value2;
				long param1, param2;
				value1 = g_value_array_get_nth(params, i + 1);
				value2 = g_value_array_get_nth(params, i + 2);
				if (G_UNLIKELY (!(G_VALUE_HOLDS_LONG(value1) && G_VALUE_HOLDS_LONG(value2))))
					break;
				param1 = g_value_get_long(value1);
				param2 = g_value_get_long(value2);
				if (G_LIKELY (param1 == 5 && param2 >= 0 && param2 < 256))
					outer->screen->defaults.attr.fore = param2;
				i += 2;
			}
			break;
		}
		case 39:
			/* default foreground */
			outer->screen->defaults.attr.fore = VTE_DEF_FG;
			break;
		case 40:
		case 41:
		case 42:
		case 43:
		case 44:
		case 45:
		case 46:
		case 47:
			outer->screen->defaults.attr.back = param - 40;
			break;
		case 48:
		{
			/* The format looks like: ^[[48;5;COLORNUMBERm,
			   so look for COLORNUMBER here. */
			if ((i + 2) < params->n_values){
				GValue *value1, *value2;
				long param1, param2;
				value1 = g_value_array_get_nth(params, i + 1);
				value2 = g_value_array_get_nth(params, i + 2);
				if (G_UNLIKELY (!(G_VALUE_HOLDS_LONG(value1) && G_VALUE_HOLDS_LONG(value2))))
					break;
				param1 = g_value_get_long(value1);
				param2 = g_value_get_long(value2);
				if (G_LIKELY (param1 == 5 && param2 >= 0 && param2 < 256))
					outer->screen->defaults.attr.back = param2;
				i += 2;
			}
			break;
		}
		case 49:
			/* default background */
			outer->screen->defaults.attr.back = VTE_DEF_BG;
			break;
		case 90:
		case 91:
		case 92:
		case 93:
		case 94:
		case 95:
		case 96:
		case 97:
			outer->screen->defaults.attr.fore = param - 90 + VTE_COLOR_BRIGHT_OFFSET;
			break;
		case 100:
		case 101:
		case 102:
		case 103:
		case 104:
		case 105:
		case 106:
		case 107:
			outer->screen->defaults.attr.back = param - 100 + VTE_COLOR_BRIGHT_OFFSET;
			break;
		}
	}
	/* If we had no parameters, default to the defaults. */
	if (i == 0) {
		_vte_terminal_set_default_attributes(outer);
	}
	/* Save the new colors. */
	outer->screen->color_defaults.attr.fore = outer->screen->defaults.attr.fore;
	outer->screen->color_defaults.attr.back = outer->screen->defaults.attr.back;
	outer->screen->fill_defaults.attr.fore = outer->screen->defaults.attr.fore;
	outer->screen->fill_defaults.attr.back = outer->screen->defaults.attr.back;
}

/**
 * vte_sequence_handler_character_position_absolute
 *
 * Move the cursor.
 */
static void
vte_sequence_handler_character_position_absolute (RarOuter *outer, GValueArray *params)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
	vte_sequence_handler_offset(outer, params, -1, vte_sequence_handler_ch);
}

/**
 * vte_sequence_handler_cursor_character_absolute
 *
 * Move the cursor to the given column, 1-based.
 */
static void
vte_sequence_handler_cursor_character_absolute (RarOuter *outer, GValueArray *params)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
	GValue *value;
	long val;

        val = 0;
	if ((params != NULL) && (params->n_values > 0)) {
		value = g_value_array_get_nth(params, 0);
		if (G_VALUE_HOLDS_LONG(value)) {
			val = CLAMP(g_value_get_long(value),
				    1, outer->column_count) - 1;
		}
	}

        outer->screen->cursor_current.col = val;
	_vte_terminal_cleanup_tab_fragments_at_cursor (outer);
}

/**
 * vte_sequence_handler_cursor_lower_left
 *
 * Move the cursor to the lower left-hand corner.
 */
static void
vte_sequence_handler_cursor_lower_left (RarOuter *outer, GValueArray *params)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
	long row;
	row = MAX(0, outer->row_count - 1);
	outer->screen->cursor_current.row = outer->screen->insert_delta + row;
	outer->screen->cursor_current.col = 0;
}

/**
 * vte_sequence_handler_cursor_next_line
 *
 * Move the cursor to the beginning of the next line, scrolling if necessary.
 */
static void
vte_sequence_handler_cursor_next_line (RarOuter *outer, GValueArray *params)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
	outer->screen->cursor_current.col = 0;
	vte_sequence_handler_DO (outer, params);
}

/**
 * vte_sequence_handler_cursor_position
 *
 * Move the cursor to the given position, 1-based.
 */
static void
vte_sequence_handler_cursor_position (RarOuter *outer, GValueArray *params)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
	vte_sequence_handler_offset(outer, params, -1, vte_sequence_handler_cm);
}

/**
 * vte_sequence_handler_cursor_preceding_line
 *
 * Move the cursor to the beginning of the next line, scrolling if necessary.
 */
static void
vte_sequence_handler_cursor_preceding_line (RarOuter *outer, GValueArray *params)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
	outer->screen->cursor_current.col = 0;
	vte_sequence_handler_UP (outer, params);
}

/**
 * vte_sequence_handler_decset_internal
 *
 * Manipulate certain terminal attributes.
 */
static void
vte_sequence_handler_decset_internal (RarOuter *outer,
				     int setting,
				     gboolean restore,
				     gboolean save,
				     gboolean set)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
	gboolean recognized = FALSE;
	gpointer p;
	guint i;
	struct {
		int setting;
		gboolean *bvalue;	// bool value
		gint *ivalue;		// int value
		gpointer *pvalue;	// pointer value
		gpointer fvalue;	// From
		gpointer tvalue;	// To
		VteTerminalSequenceHandler reset, set;
	} settings[] = {
		/* 1: Application/normal cursor keys. */
		{1, NULL, &outer->cursor_mode, NULL,
		 GINT_TO_POINTER(VTE_KEYMODE_NORMAL),
		 GINT_TO_POINTER(VTE_KEYMODE_APPLICATION),
		 NULL, NULL,},
		/* 2: disallowed, we don't do VT52. */
		{2, NULL, NULL, NULL, NULL, NULL, NULL, NULL,},
		/* 3: disallowed, window size is set by user. */
		{3, NULL, NULL, NULL, NULL, NULL, NULL, NULL,},
		/* 4: Smooth scroll. */
		{4, &outer->smooth_scroll, NULL, NULL,
		 GINT_TO_POINTER(FALSE),
		 GINT_TO_POINTER(TRUE),
		 NULL, NULL,},
		/* 5: Reverse video. */
		{5, &outer->screen->reverse_mode, NULL, NULL,
		 GINT_TO_POINTER(FALSE),
		 GINT_TO_POINTER(TRUE),
		 NULL, NULL,},
		/* 6: Origin mode: when enabled, cursor positioning is
		 * relative to the scrolling region. */
		{6, &outer->screen->origin_mode, NULL, NULL,
		 GINT_TO_POINTER(FALSE),
		 GINT_TO_POINTER(TRUE),
		 NULL, NULL,},
		/* 7: Wraparound mode. */
		{7, &outer->flags.am, NULL, NULL,
		 GINT_TO_POINTER(FALSE),
		 GINT_TO_POINTER(TRUE),
		 NULL, NULL,},
		/* 8: disallowed, keyboard repeat is set by user. */
		{8, NULL, NULL, NULL, NULL, NULL, NULL, NULL,},
		/* 9: Send-coords-on-click. */
		{9, NULL, &outer->mouse_tracking_mode, NULL,
		 GINT_TO_POINTER(0),
		 GINT_TO_POINTER(MOUSE_TRACKING_SEND_XY_ON_CLICK),
		 NULL, NULL,},
		/* 12: disallowed, cursor blinks is set by user. */
		{12, NULL, NULL, NULL, NULL, NULL, NULL, NULL,},
		/* 18: print form feed. */
		/* 19: set print extent to full screen. */
		/* 25: Cursor visible. */
		{25, &outer->cursor_visible, NULL, NULL,
		 GINT_TO_POINTER(FALSE),
		 GINT_TO_POINTER(TRUE),
		 NULL, NULL,},
		/* 30/rxvt: disallowed, scrollbar visibility is set by user. */
		{30, NULL, NULL, NULL, NULL, NULL, NULL, NULL,},
		/* 35/rxvt: disallowed, fonts set by user. */
		{35, NULL, NULL, NULL, NULL, NULL, NULL, NULL,},
		/* 38: enter Tektronix mode. */
		/* 40: disallowed, the user sizes dynamically. */
		{40, NULL, NULL, NULL, NULL, NULL, NULL, NULL,},
		/* 41: more(1) fix. */
		/* 42: Enable NLS replacements. */
		{42, &outer->nrc_mode, NULL, NULL,
		 GINT_TO_POINTER(FALSE),
		 GINT_TO_POINTER(TRUE),
		 NULL, NULL,},
		/* 44: Margin bell. */
		{44, &outer->margin_bell, NULL, NULL,
		 GINT_TO_POINTER(FALSE),
		 GINT_TO_POINTER(TRUE),
		 NULL, NULL,},
		/* 47: Alternate screen. */
		{47, NULL, NULL, (gpointer) &outer,
		 &outer->normal_screen,
		 &outer->alternate_screen,
		 NULL, NULL,},
		/* 66: Keypad mode. */
		{66, &outer->keypad_mode, NULL, NULL,
		 GINT_TO_POINTER(VTE_KEYMODE_NORMAL),
		 GINT_TO_POINTER(VTE_KEYMODE_APPLICATION),
		 NULL, NULL,},
		/* 67: disallowed, backspace key policy is set by user. */
		{67, NULL, NULL, NULL, NULL, NULL, NULL, NULL,},
		/* 1000: Send-coords-on-button. */
		{1000, NULL, &outer->mouse_tracking_mode, NULL,
		 GINT_TO_POINTER(0),
		 GINT_TO_POINTER(MOUSE_TRACKING_SEND_XY_ON_BUTTON),
		 NULL, NULL,},
		/* 1001: Hilite tracking. */
		{1001, NULL, &outer->mouse_tracking_mode, NULL,
		 GINT_TO_POINTER(0),
		 GINT_TO_POINTER(MOUSE_TRACKING_HILITE_TRACKING),
		 NULL, NULL,},
		/* 1002: Cell motion tracking. */
		{1002, NULL, &outer->mouse_tracking_mode, NULL,
		 GINT_TO_POINTER(0),
		 GINT_TO_POINTER(MOUSE_TRACKING_CELL_MOTION_TRACKING),
		 NULL, NULL,},
		/* 1003: All motion tracking. */
		{1003, NULL, &outer->mouse_tracking_mode, NULL,
		 GINT_TO_POINTER(0),
		 GINT_TO_POINTER(MOUSE_TRACKING_ALL_MOTION_TRACKING),
		 NULL, NULL,},
		/* 1010/rxvt: disallowed, scroll-on-output is set by user. */
		{1010, NULL, NULL, NULL, NULL, NULL, NULL, NULL,},
		/* 1011/rxvt: disallowed, scroll-on-keypress is set by user. */
		{1011, NULL, NULL, NULL, NULL, NULL, NULL, NULL,},
		/* 1035: disallowed, don't know what to do with it. */
		{1035, NULL, NULL, NULL, NULL, NULL, NULL, NULL,},
		/* 1036: Meta-sends-escape. */
		{1036, &outer->meta_sends_escape, NULL, NULL,
		 GINT_TO_POINTER(FALSE),
		 GINT_TO_POINTER(TRUE),
		 NULL, NULL,},
		/* 1037: disallowed, delete key policy is set by user. */
		{1037, NULL, NULL, NULL, NULL, NULL, NULL, NULL,},
		/* 1047: Use alternate screen buffer. */
		{1047, NULL, NULL, (gpointer) &outer,
		 &outer->normal_screen,
		 &outer->alternate_screen,
		 NULL, NULL,},
		/* 1048: Save/restore cursor position. */
		{1048, NULL, NULL, NULL,
		 NULL,
		 NULL,
		 vte_sequence_handler_rc,
		 vte_sequence_handler_sc,},
		/* 1049: Use alternate screen buffer, saving the cursor
		 * position. */
		{1049, NULL, NULL, (gpointer) &outer,
		 &outer->normal_screen,
		 &outer->alternate_screen,
		 vte_sequence_handler_rc,
		 vte_sequence_handler_sc,},
		/* 1051: Sun function key mode. */
		{1051, NULL, NULL, (gpointer) &outer->sun_fkey_mode,
		 GINT_TO_POINTER(FALSE),
		 GINT_TO_POINTER(TRUE),
		 NULL, NULL},
		/* 1052: HP function key mode. */
		{1052, NULL, NULL, (gpointer) &outer->hp_fkey_mode,
		 GINT_TO_POINTER(FALSE),
		 GINT_TO_POINTER(TRUE),
		 NULL, NULL},
		/* 1060: Legacy function key mode. */
		{1060, NULL, NULL, (gpointer) &outer->legacy_fkey_mode,
		 GINT_TO_POINTER(FALSE),
		 GINT_TO_POINTER(TRUE),
		 NULL, NULL},
		/* 1061: VT220 function key mode. */
		{1061, NULL, NULL, (gpointer) &outer->vt220_fkey_mode,
		 GINT_TO_POINTER(FALSE),
		 GINT_TO_POINTER(TRUE),
		 NULL, NULL},
		/* 2004: Bracketed paste mode. */
		{2004, &outer->screen->bracketed_paste_mode, NULL, NULL,
		 GINT_TO_POINTER(FALSE),
		 GINT_TO_POINTER(TRUE),
		 NULL, NULL,},
	};

	/* Handle the setting. */
	for (i = 0; i < G_N_ELEMENTS(settings); i++)
	if (settings[i].setting == setting) {
		recognized = TRUE;
		/* Handle settings we want to ignore. */
		if ((settings[i].fvalue == settings[i].tvalue) &&
		    (settings[i].set == NULL) &&
		    (settings[i].reset == NULL)) {
			continue;
		}

		/* Read the old setting. */
		if (restore) {
			p = g_hash_table_lookup(outer->dec_saved,
						GINT_TO_POINTER(setting));
			set = (p != NULL);
			_vte_debug_print(VTE_DEBUG_PARSE,
					"Setting %d was %s.\n",
					setting, set ? "set" : "unset");
		}
		/* Save the current setting. */
		if (save) {
			if (settings[i].bvalue) {
				set = *(settings[i].bvalue) != FALSE;
			} else
			if (settings[i].ivalue) {
				set = *(settings[i].ivalue) ==
				      GPOINTER_TO_INT(settings[i].tvalue);
			} else
			if (settings[i].pvalue) {
				set = *(settings[i].pvalue) ==
				      settings[i].tvalue;
			}
			_vte_debug_print(VTE_DEBUG_PARSE,
					"Setting %d is %s, saving.\n",
					setting, set ? "set" : "unset");
			g_hash_table_insert(outer->dec_saved,
					    GINT_TO_POINTER(setting),
					    GINT_TO_POINTER(set));
		}
		/* Change the current setting to match the new/saved value. */
		if (!save) {
			_vte_debug_print(VTE_DEBUG_PARSE,
					"Setting %d to %s.\n",
					setting, set ? "set" : "unset");
			if (settings[i].set && set) {
				settings[i].set (outer, NULL);
			}
			if (settings[i].bvalue) {
				*(settings[i].bvalue) = set;
			} else
			if (settings[i].ivalue) {
				*(settings[i].ivalue) = set ?
					GPOINTER_TO_INT(settings[i].tvalue) :
					GPOINTER_TO_INT(settings[i].fvalue);
			} else
			if (settings[i].pvalue) {
				*(settings[i].pvalue) = set ?
					settings[i].tvalue :
					settings[i].fvalue;
			}
			if (settings[i].reset && !set) {
				settings[i].reset (outer, NULL);
			}
		}
	}

	/* Do whatever's necessary when the setting changes. */
	switch (setting) {
	case 1:
		_vte_debug_print(VTE_DEBUG_KEYBOARD, set ?
				"Entering application cursor mode.\n" :
				"Leaving application cursor mode.\n");
		break;
#if 0		/* 3: disallowed, window size is set by user. */
	case 3:
		vte_terminal_emit_resize_window(outer,
						(set ? 132 : 80) *
						outer->screen->char_width +
						outer->screen->inner_border.left +
                                                outer->screen->inner_border.right,
						outer->row_count *
						outer->screen->char_height +
						outer->screen->inner_border.top +
                                                outer->screen->inner_border.bottom);
		/* Request a resize and redraw. */
		_vte_invalidate_all(outer);
		break;
#endif
	case 5:
		/* Repaint everything in reverse mode. */
		_vte_invalidate_all(outer);
		break;
	case 6:
		/* Reposition the cursor in its new home position. */
		outer->screen->cursor_current.col = 0;
		outer->screen->cursor_current.row =
			outer->screen->insert_delta;
		break;
	case 47:
	case 1047:
	case 1049:
		/* Clear the alternate screen if we're switching
		 * to it, and home the cursor. */
		if (set) {
			_vte_terminal_clear_screen (outer);
			_vte_terminal_home_cursor (outer);
		}
		/* Reset scrollbars and repaint everything. */
#ifndef RARXXX
		outer->screen->adjustment->value =
			outer->screen->scroll_delta;
		vte_terminal_set_scrollback_lines(outer,
				outer->screen->scrollback_lines);
#endif
		_vte_terminal_queue_contents_changed(outer);
		_vte_invalidate_all (outer);
		break;
	case 9:
	case 1000:
	case 1001:
	case 1002:
	case 1003:
		/* Make the pointer visible. */
		_vte_terminal_set_pointer_visible(outer, TRUE);
		break;
	case 66:
		_vte_debug_print(VTE_DEBUG_KEYBOARD, set ?
				"Entering application keypad mode.\n" :
				"Leaving application keypad mode.\n");
		break;
	case 1051:
		_vte_debug_print(VTE_DEBUG_KEYBOARD, set ?
				"Entering Sun fkey mode.\n" :
				"Leaving Sun fkey mode.\n");
		break;
	case 1052:
		_vte_debug_print(VTE_DEBUG_KEYBOARD, set ?
				"Entering HP fkey mode.\n" :
				"Leaving HP fkey mode.\n");
		break;
	case 1060:
		_vte_debug_print(VTE_DEBUG_KEYBOARD, set ?
				"Entering Legacy fkey mode.\n" :
				"Leaving Legacy fkey mode.\n");
		break;
	case 1061:
		_vte_debug_print(VTE_DEBUG_KEYBOARD, set ?
				"Entering VT220 fkey mode.\n" :
				"Leaving VT220 fkey mode.\n");
		break;
	default:
		break;
	}

	if (!recognized) {
		_vte_debug_print (VTE_DEBUG_MISC,
				  "DECSET/DECRESET mode %d not recognized, ignoring.\n",
				  setting);
	}
}

/**
 * vte_sequence_handler_decset
 *
 * Set certain terminal attributes.
 */
static void
vte_sequence_handler_decset (RarOuter *outer, GValueArray *params)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
	GValue *value;
	long setting;
	guint i;
	if ((params == NULL) || (params->n_values == 0)) {
		return;
	}
	for (i = 0; i < params->n_values; i++) {
		value = g_value_array_get_nth(params, i);
		if (!G_VALUE_HOLDS_LONG(value)) {
			continue;
		}
		setting = g_value_get_long(value);
		vte_sequence_handler_decset_internal(outer, setting, FALSE, FALSE, TRUE);
	}
}

/**
 * vte_sequence_handler_decreset
 *
 * Unset certain terminal attributes.
 */
static void
vte_sequence_handler_decreset (RarOuter *outer, GValueArray *params)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
	GValue *value;
	long setting;
	guint i;
	if ((params == NULL) || (params->n_values == 0)) {
		return;
	}
	for (i = 0; i < params->n_values; i++) {
		value = g_value_array_get_nth(params, i);
		if (!G_VALUE_HOLDS_LONG(value)) {
			continue;
		}
		setting = g_value_get_long(value);
		vte_sequence_handler_decset_internal(outer, setting, FALSE, FALSE, FALSE);
	}
}

/**
 * vte_sequence_handler_delete_lines
 *
 * Delete certain lines from the scrolling region.
 */
static void
vte_sequence_handler_delete_lines (RarOuter *outer, GValueArray *params)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
	GValue *value;
	long param, end, row;
	int i;

	/* The default is one. */
	param = 1;
	/* Extract any parameters. */
	if ((params != NULL) && (params->n_values > 0)) {
		value = g_value_array_get_nth(params, 0);
		if (G_VALUE_HOLDS_LONG(value)) {
			param = g_value_get_long(value);
		}
	}
	/* Find the region we're messing with. */
	row = outer->screen->cursor_current.row;
	if (outer->screen->scrolling_restricted) {
		end = outer->screen->insert_delta + outer->screen->scrolling_region.end;
	} else {
		end = outer->screen->insert_delta + outer->row_count - 1;
	}
	/* Clear them from below the current cursor. */
	for (i = 0; i < param; i++) {
		/* Insert a line at the end of the region and remove one from
		 * the top of the region. */
		_vte_terminal_ring_remove (outer, row);
		_vte_terminal_ring_insert (outer, end, TRUE);
	}
	/* Update the display. */
	_vte_terminal_scroll_region(outer, row, end - row + 1, -param);
	/* Adjust the scrollbars if necessary. */
	_vte_terminal_adjust_adjustments(outer);
	/* We've modified the display.  Make a note of it. */
	outer->text_deleted_flag = TRUE;
}

/**
 * vte_sequence_handler_erase_characters
 *
 * Erase a specified number of characters.
 */
static void
vte_sequence_handler_erase_characters (RarOuter *outer, GValueArray *params)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
	vte_sequence_handler_ec (outer, params);
}

/**
 * vte_sequence_handler_erase_in_display
 *
 * Erase certain lines in the display.
 */
static void
vte_sequence_handler_erase_in_display (RarOuter *outer, GValueArray *params)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
	GValue *value;
	long param;
	guint i;
	/* The default parameter is 0. */
	param = 0;
	/* Pull out a parameter. */
	for (i = 0; (params != NULL) && (i < params->n_values); i++) {
		value = g_value_array_get_nth(params, i);
		if (!G_VALUE_HOLDS_LONG(value)) {
			continue;
		}
		param = g_value_get_long(value);
	}
	/* Clear the right area. */
	switch (param) {
	case 0:
		/* Clear below the current line. */
		vte_sequence_handler_cd (outer, NULL);
		break;
	case 1:
		/* Clear above the current line. */
		_vte_terminal_clear_above_current (outer);
		/* Clear everything to the left of the cursor, too. */
		/* FIXME: vttest. */
		vte_sequence_handler_cb (outer, NULL);
		break;
	case 2:
		/* Clear the entire screen. */
		_vte_terminal_clear_screen (outer);
		break;
	default:
		break;
	}
	/* We've modified the display.  Make a note of it. */
	outer->text_deleted_flag = TRUE;
}

/**
 * vte_sequence_handler_erase_in_line
 *
 * Erase certain parts of the current line in the display.
 */
static void
vte_sequence_handler_erase_in_line (RarOuter *outer, GValueArray *params)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
	GValue *value;
	long param;
	guint i;
	/* The default parameter is 0. */
	param = 0;
	/* Pull out a parameter. */
	for (i = 0; (params != NULL) && (i < params->n_values); i++) {
		value = g_value_array_get_nth(params, i);
		if (!G_VALUE_HOLDS_LONG(value)) {
			continue;
		}
		param = g_value_get_long(value);
	}
	/* Clear the right area. */
	switch (param) {
	case 0:
		/* Clear to end of the line. */
		vte_sequence_handler_ce (outer, NULL);
		break;
	case 1:
		/* Clear to start of the line. */
		vte_sequence_handler_cb (outer, NULL);
		break;
	case 2:
		/* Clear the entire line. */
		_vte_terminal_clear_current_line (outer);
		break;
	default:
		break;
	}
	/* We've modified the display.  Make a note of it. */
	outer->text_deleted_flag = TRUE;
}

/**
 * vte_sequence_handler_form_feed
 *
 * Form-feed / next-page.
 */
static void
vte_sequence_handler_form_feed (RarOuter *outer, GValueArray *params)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
	vte_sequence_handler_index (outer, params);
}

/**
 * vte_sequence_handler_full_reset
 *
 * Perform a full-bore reset.
 */
static void
vte_sequence_handler_full_reset (RarOuter *outer, GValueArray *params)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
	vte_terminal_reset(outer, TRUE, TRUE);
}

/**
 * vte_sequence_handler_horizontal_and_vertical_position
 *
 * Move the cursor to a specified position.
 */
static void
vte_sequence_handler_horizontal_and_vertical_position (RarOuter *outer, GValueArray *params)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
	vte_sequence_handler_offset(outer, params, -1, vte_sequence_handler_cm);
}

/**
 * vte_sequence_handler_index
 *
 * Cursor down, with scrolling.
 */
static void
vte_sequence_handler_index (RarOuter *outer, GValueArray *params)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
	vte_sequence_handler_sf (outer, params);
}

/**
 * vte_sequence_handler_insert_blank_characters
 *
 * Insert a specified number of blank characters.
 */
static void
vte_sequence_handler_insert_blank_characters (RarOuter *outer, GValueArray *params)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
	vte_sequence_handler_IC (outer, params);
}

/**
 * vte_sequence_handler_insert_lines
 *
 * Insert a certain number of lines below the current cursor.
 */
static void
vte_sequence_handler_insert_lines (RarOuter *outer, GValueArray *params)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
	GValue *value;
	long param, end, row;
	int i;
	/* The default is one. */
	param = 1;
	/* Extract any parameters. */
	if ((params != NULL) && (params->n_values > 0)) {
		value = g_value_array_get_nth(params, 0);
		if (G_VALUE_HOLDS_LONG(value)) {
			param = g_value_get_long(value);
		}
	}
	/* Find the region we're messing with. */
	row = outer->screen->cursor_current.row;
	if (outer->screen->scrolling_restricted) {
		end = outer->screen->insert_delta + outer->screen->scrolling_region.end;
	} else {
		end = outer->screen->insert_delta + outer->row_count - 1;
	}
	/* Insert the new lines at the cursor. */
	for (i = 0; i < param; i++) {
		/* Clear a line off the end of the region and add one to the
		 * top of the region. */
		_vte_terminal_ring_remove (outer, end);
		_vte_terminal_ring_insert (outer, row, TRUE);
	}
	/* Update the display. */
	_vte_terminal_scroll_region(outer, row, end - row + 1, param);
	/* Adjust the scrollbars if necessary. */
	_vte_terminal_adjust_adjustments(outer);
	/* We've modified the display.  Make a note of it. */
	outer->text_inserted_flag = TRUE;
}

/**
 * vte_sequence_handler_line_position_absolute
 */
static void
vte_sequence_handler_line_position_absolute (RarOuter *outer, GValueArray *params)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
	vte_sequence_handler_offset(outer, params, -1, vte_sequence_handler_cv);
}

/**
 * vte_sequence_handler_next_line
 *
 * Move the cursor to the beginning of the next line, scrolling if necessary.
 */
static void
vte_sequence_handler_next_line (RarOuter *outer, GValueArray *params)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
	outer->screen->cursor_current.col = 0;
	vte_sequence_handler_DO (outer, params);
}

/**
 * vte_sequence_handler_restore_mode
 *
 * Restore a certain terminal attribute.
 */
static void
vte_sequence_handler_restore_mode (RarOuter *outer, GValueArray *params)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
	GValue *value;
	long setting;
	guint i;
	if ((params == NULL) || (params->n_values == 0)) {
		return;
	}
	for (i = 0; i < params->n_values; i++) {
		value = g_value_array_get_nth(params, i);
		if (!G_VALUE_HOLDS_LONG(value)) {
			continue;
		}
		setting = g_value_get_long(value);
		vte_sequence_handler_decset_internal(outer, setting, TRUE, FALSE, FALSE);
	}
}

/**
 * vte_sequence_handler_reverse_index
 *
 * Cursor down, with scrolling.
 */
static void
vte_sequence_handler_reverse_index (RarOuter *outer, GValueArray *params)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
	vte_sequence_handler_sr (outer, params);
}

/**
 * vte_sequence_handler_save_mode
 *
 * Save a certain terminal attribute.
 */
static void
vte_sequence_handler_save_mode (RarOuter *outer, GValueArray *params)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
	GValue *value;
	long setting;
	guint i;
	if ((params == NULL) || (params->n_values == 0)) {
		return;
	}
	for (i = 0; i < params->n_values; i++) {
		value = g_value_array_get_nth(params, i);
		if (!G_VALUE_HOLDS_LONG(value)) {
			continue;
		}
		setting = g_value_get_long(value);
		vte_sequence_handler_decset_internal(outer, setting, FALSE, TRUE, FALSE);
	}
}

/**
 * vte_sequence_handler_screen_alignment_test
 * Perform a screen alignment test -- fill all visible cells with the
 * letter "E".
 */
static void
vte_sequence_handler_screen_alignment_test (RarOuter *outer, GValueArray *params)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
	long row;
	VteRowData *rowdata;
	VteCell cell;

	for (row = outer->screen->insert_delta;
	     row < outer->screen->insert_delta + outer->row_count;
	     row++) {
		/* Find this row. */
		while (_vte_ring_next(outer->screen->row_data) <= row)
			_vte_terminal_ring_append (outer, FALSE);
		_vte_terminal_adjust_adjustments(outer);
		rowdata = _vte_ring_index_writable (outer->screen->row_data, row);
		g_assert(rowdata != NULL);
		/* Clear this row. */
		_vte_row_data_shrink (rowdata, 0);

		_vte_terminal_emit_text_deleted(outer);
		/* Fill this row. */
		cell.c = 'E';
		cell.attr = basic_cell.cell.attr;
		cell.attr.columns = 1;
		_vte_row_data_fill (rowdata, &cell, outer->column_count);
		_vte_terminal_emit_text_inserted(outer);
	}
	_vte_invalidate_all(outer);

	/* We modified the display, so make a note of it for completeness. */
	outer->text_modified_flag = TRUE;
}

/**
 * vte_sequence_handler_scroll_down
 *
 * Scroll the text down, but don't move the cursor.
 */
static void
vte_sequence_handler_scroll_down (RarOuter *outer, GValueArray *params)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
	long val = 1;
	GValue *value;

	if ((params != NULL) && (params->n_values > 0)) {
		value = g_value_array_get_nth(params, 0);
		if (G_VALUE_HOLDS_LONG(value)) {
			val = g_value_get_long(value);
			val = MAX(val, 1);
		}
	}

	_vte_terminal_scroll_text (outer, val);
}

/**
 * vte_sequence_handler_scroll_up
 *
 * Scroll the text up, but don't move the cursor.
 */
static void
vte_sequence_handler_scroll_up (RarOuter *outer, GValueArray *params)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
	long val = 1;
	GValue *value;

	if ((params != NULL) && (params->n_values > 0)) {
		value = g_value_array_get_nth(params, 0);
		if (G_VALUE_HOLDS_LONG(value)) {
			val = g_value_get_long(value);
			val = MAX(val, 1);
		}
	}

	_vte_terminal_scroll_text (outer, -val);
}

/**
 * vte_sequence_handler_set_scrolling_region
 *
 * Restrict the scrolling region.
 */
static void
vte_sequence_handler_set_scrolling_region (RarOuter *outer, GValueArray *params)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
	vte_sequence_handler_offset(outer, params, -1, vte_sequence_handler_cs);
}

/**
 * vte_sequence_handler_set_scrolling_region_from_start
 */
static void
vte_sequence_handler_set_scrolling_region_from_start (RarOuter *outer, GValueArray *params)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
	GValue value = {0};

	g_value_init (&value, G_TYPE_LONG);
	g_value_set_long (&value, 0); /* Out of range means start/end */

	g_value_array_insert (params, 0, &value);

	vte_sequence_handler_offset(outer, params, -1, vte_sequence_handler_cs);
}

/**
 * vte_sequence_handler_set_scrolling_region_to_end
 */
static void
vte_sequence_handler_set_scrolling_region_to_end (RarOuter *outer, GValueArray *params)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
	GValue value = {0};

	g_value_init (&value, G_TYPE_LONG);
	g_value_set_long (&value, 0); /* Out of range means start/end */

	g_value_array_insert (params, 1, &value);

	vte_sequence_handler_offset(outer, params, -1, vte_sequence_handler_cs);
}

/**
 * vte_sequence_handler_soft_reset
 *
 * Perform a soft reset.
 */
static void
vte_sequence_handler_soft_reset (RarOuter *outer, GValueArray *params)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
	vte_terminal_reset(outer, FALSE, FALSE);
}

/**
 * vte_sequence_handler_vertical_tab
 *
 * Vertical tab.
 */
static void
vte_sequence_handler_vertical_tab (RarOuter *outer, GValueArray *params)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
	vte_sequence_handler_index (outer, params);
}


#define VTE_SEQUENCE_HANDLER(name) name
#include "rperf-vte.c"
#undef VTE_SEQUENCE_HANDLER

/**
 * _vte_sequence_get_handler
 */
static VteTerminalSequenceHandler
_vte_sequence_get_handler (const char *name)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
	int len = strlen (name);

	const struct vteseq_n_struct *seqhandler;
	seqhandler = vteseq_n_lookup (name, len);
	return seqhandler ? seqhandler->handler : NULL;
}

/**
 * _vte_terminal_handle_sequence
 * Handle a terminal control sequence and its parameters.
 */
void
_vte_terminal_handle_sequence (RarOuter *outer,
			      const char *match_s,
			      GQuark match G_GNUC_UNUSED,
			      GValueArray *params)
{
	//printf ("Entering: %s\n", __FUNCTION__);e
	VteTerminalSequenceHandler handler;

#if 0
	_VTE_DEBUG_IF(VTE_DEBUG_PARSE)
		display_control_sequence(match_s, params);
#endif

	/* Find the handler for this control sequence. */
	handler = _vte_sequence_get_handler (match_s);

	if (handler != NULL) {
		/* Let the handler handle it. */
		handler (outer, params);
	} else {
		_vte_debug_print (VTE_DEBUG_MISC,
				  "No handler for control sequence `%s' defined.\n",
				  match_s);
	}
}


