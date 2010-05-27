#include <glib-object.h>

#include "vterowdata.h"

//RARXXX dummy
typedef struct _VteTerminal {
	int x;
} VteTerminal;

//RARXXX dummy
typedef struct _VteScreen {
	int x;
} VteScreen;

#ifdef RARXXX // copied from vteseq.c
/* Typedef the handle type */
typedef void (*VteTerminalSequenceHandler) (VteTerminal *terminal, GValueArray *params);
#endif

/**
 * vte_sequence_handler_multiple
 *
 * Call another function a given number of times, or once.
 */
static void
vte_sequence_handler_multiple(VteTerminal *terminal,
			      GValueArray *params,
			      VteTerminalSequenceHandler handler)
{
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
		handler (terminal, NULL);
}

/**
 * vte_sequence_handler_offset
 *
 * Call another handler, offsetting any long arguments by the given
 * increment value.
 */
static void
vte_sequence_handler_offset(VteTerminal *terminal,
			    GValueArray *params,
			    int increment,
			    VteTerminalSequenceHandler handler)
{
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
	handler (terminal, params);
}

/**
 * vte_terminal_clear_current_line
 *
 * Clear the current line.
 */
static void
vte_terminal_clear_current_line (VteTerminal *terminal)
{
	VteRowData *rowdata;
	VteScreen *screen;

	screen = terminal->pvt->screen;

	/* If the cursor is actually on the screen, clear data in the row
	 * which corresponds to the cursor. */
	if (_vte_ring_next(screen->row_data) > screen->cursor_current.row) {
		/* Get the data for the row which the cursor points to. */
		rowdata = _vte_ring_index_writable (screen->row_data, screen->cursor_current.row);
		g_assert(rowdata != NULL);
		/* Remove it. */
		_vte_row_data_shrink (rowdata, 0);
		/* Add enough cells to the end of the line to fill out the row. */
		_vte_row_data_fill (rowdata, &screen->fill_defaults, terminal->column_count);
		rowdata->attr.soft_wrapped = 0;
		/* Repaint this row. */
		_vte_invalidate_cells(terminal,
				      0, terminal->column_count,
				      screen->cursor_current.row, 1);
	}

	/* We've modified the display.  Make a note of it. */
	terminal->pvt->text_deleted_flag = TRUE;
}

/**
 * vte_terminal_clear_screen
 *
 * Clear the entire screen.
 */
static void
vte_terminal_clear_screen (VteTerminal *terminal)
{
	long i, initial, row;
	VteScreen *screen;
	screen = terminal->pvt->screen;
	initial = screen->insert_delta;
	row = screen->cursor_current.row - screen->insert_delta;
	initial = _vte_ring_next(screen->row_data);
	/* Add a new screen's worth of rows. */
	for (i = 0; i < terminal->row_count; i++)
		_vte_terminal_ring_append (terminal, TRUE);
	/* Move the cursor and insertion delta to the first line in the
	 * newly-cleared area and scroll if need be. */
	screen->insert_delta = initial;
	screen->cursor_current.row = row + screen->insert_delta;
	_vte_terminal_adjust_adjustments(terminal);
	/* Redraw everything. */
	_vte_invalidate_all(terminal);
	/* We've modified the display.  Make a note of it. */
	terminal->pvt->text_deleted_flag = TRUE;
}

/**
 * vte_terminal_find_charcell
 *
 * Find the character an the given position in the backscroll buffer.
 */
static VteCell *
vte_terminal_find_charcell (VteTerminal *terminal, glong col, glong row)
{
	VteRowData *rowdata;
	VteCell *ret = NULL;
	VteScreen *screen;
	g_assert(VTE_IS_TERMINAL(terminal));
	screen = terminal->pvt->screen;
	if (_vte_ring_contains (screen->row_data, row)) {
		rowdata = _vte_ring_index_writable (screen->row_data, row);
		ret = _vte_row_data_get_writable (rowdata, col);
	}
	return ret;
}

/**
 * vte_terminal_home_cursor
 */
static void
vte_terminal_home_cursor (VteTerminal *terminal)
{
	VteScreen *screen;
	screen = terminal->pvt->screen;
	screen->cursor_current.row = screen->insert_delta;
	screen->cursor_current.col = 0;
}

/**
 * vte_terminal_scroll_text
 *
 * Scroll the text, but don't move the cursor.  Negative = up, positive = down.
 */
static void
vte_terminal_scroll_text (VteTerminal *terminal, int scroll_amount)
{
	long start, end, i;
	VteScreen *screen;

	screen = terminal->pvt->screen;

	if (screen->scrolling_restricted) {
		start = screen->insert_delta + screen->scrolling_region.start;
		end = screen->insert_delta + screen->scrolling_region.end;
	} else {
		start = screen->insert_delta;
		end = start + terminal->row_count - 1;
	}

	while (_vte_ring_next(screen->row_data) <= end)
		_vte_terminal_ring_append (terminal, FALSE);

	if (scroll_amount > 0) {
		for (i = 0; i < scroll_amount; i++) {
			_vte_terminal_ring_remove (terminal, end);
			_vte_terminal_ring_insert (terminal, start, TRUE);
		}
	} else {
		for (i = 0; i < -scroll_amount; i++) {
			_vte_terminal_ring_remove (terminal, start);
			_vte_terminal_ring_insert (terminal, end, TRUE);
		}
	}

	/* Update the display. */
	_vte_terminal_scroll_region(terminal, start, end - start + 1,
				   scroll_amount);

	/* Adjust the scrollbars if necessary. */
	_vte_terminal_adjust_adjustments(terminal);

	/* We've modified the display.  Make a note of it. */
	terminal->pvt->text_inserted_flag = TRUE;
	terminal->pvt->text_deleted_flag = TRUE;
}

/**
 * vte_terminal_termcap_string_same_as_for
 */
static gboolean
vte_terminal_termcap_string_same_as_for (VteTerminal *terminal,
					 const char  *cap_str,
					 const char  *cap_other)
{
	char *other_str;
	gboolean ret;

	other_str = _vte_termcap_find_string(terminal->pvt->termcap,
					     terminal->pvt->emulation,
					     cap_other);

	ret = other_str && (g_ascii_strcasecmp(cap_str, other_str) == 0);

	g_free (other_str);

	return ret;
}

/**
 * vte_terminal_clear_above_current
 *
 * Clear above the current line.
 */
static void
vte_terminal_clear_above_current (VteTerminal *terminal)
{
	VteRowData *rowdata;
	long i;
	VteScreen *screen;
	screen = terminal->pvt->screen;
	/* If the cursor is actually on the screen, clear data in the row
	 * which corresponds to the cursor. */
	for (i = screen->insert_delta; i < screen->cursor_current.row; i++) {
		if (_vte_ring_next(screen->row_data) > i) {
			/* Get the data for the row we're erasing. */
			rowdata = _vte_ring_index_writable (screen->row_data, i);
			g_assert(rowdata != NULL);
			/* Remove it. */
			_vte_row_data_shrink (rowdata, 0);
			/* Add new cells until we fill the row. */
			_vte_row_data_fill (rowdata, &screen->fill_defaults, terminal->column_count);
			rowdata->attr.soft_wrapped = 0;
			/* Repaint the row. */
			_vte_invalidate_cells(terminal,
					0, terminal->column_count, i, 1);
		}
	}
	/* We've modified the display.  Make a note of it. */
	terminal->pvt->text_deleted_flag = TRUE;
}


/**
 * vte_sequence_handler_al
 *
 * Add a line at the current cursor position.
 */
static void
vte_sequence_handler_al (VteTerminal *terminal, GValueArray *params)
{
	VteScreen *screen;
	long start, end, param, i;
	GValue *value;

	/* Find out which part of the screen we're messing with. */
	screen = terminal->pvt->screen;
	start = screen->cursor_current.row;
	if (screen->scrolling_restricted) {
		end = screen->insert_delta + screen->scrolling_region.end;
	} else {
		end = screen->insert_delta + terminal->row_count - 1;
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
		_vte_terminal_ring_remove (terminal, end);
		_vte_terminal_ring_insert (terminal, start, TRUE);
		/* Adjust the scrollbars if necessary. */
		_vte_terminal_adjust_adjustments(terminal);
	}

	/* Update the display. */
	_vte_terminal_scroll_region(terminal, start, end - start + 1, param);

	/* We've modified the display.  Make a note of it. */
	terminal->pvt->text_deleted_flag = TRUE;
}

/**
 * vte_sequence_handler_AL
 *
 * Add N lines at the current cursor position.
 */
static void
vte_sequence_handler_AL (VteTerminal *terminal, GValueArray *params)
{
	vte_sequence_handler_al (terminal, params);
}

/**
 * vte_sequence_handler_bt
 *
 * Backtab.
 */
static void
vte_sequence_handler_bt (VteTerminal *terminal, GValueArray *params)
{
	long newcol;

	/* Calculate which column is the previous tab stop. */
	newcol = terminal->pvt->screen->cursor_current.col;

	if (terminal->pvt->tabstops != NULL) {
		/* Find the next tabstop. */
		while (newcol >= 0) {
			if (_vte_terminal_get_tabstop(terminal,
						     newcol % terminal->column_count)) {
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
	terminal->pvt->screen->cursor_current.col = newcol;
}

/**
 * vte_sequence_handler_cb
 *
 * Clear from the cursor position to the beginning of the line.
 */
static void
vte_sequence_handler_cb (VteTerminal *terminal, GValueArray *params)
{
	VteRowData *rowdata;
	long i;
	VteScreen *screen;
	VteCell *pcell;
	screen = terminal->pvt->screen;

	/* Get the data for the row which the cursor points to. */
	rowdata = _vte_terminal_ensure_row(terminal);
	/* Clear the data up to the current column with the default
	 * attributes.  If there is no such character cell, we need
	 * to add one. */
	for (i = 0; i <= screen->cursor_current.col; i++) {
		if (i < (glong) _vte_row_data_length (rowdata)) {
			/* Muck with the cell in this location. */
			pcell = _vte_row_data_get_writable (rowdata, i);
			*pcell = screen->color_defaults;
		} else {
			/* Add new cells until we have one here. */
			_vte_row_data_append (rowdata, &screen->color_defaults);
		}
	}
	/* Repaint this row. */
	_vte_invalidate_cells(terminal,
			      0, screen->cursor_current.col+1,
			      screen->cursor_current.row, 1);

	/* We've modified the display.  Make a note of it. */
	terminal->pvt->text_deleted_flag = TRUE;
}

/**
 * vte_sequence_handler_cd
 *
 * Clear to the right of the cursor and below the current line.
 */
static void
vte_sequence_handler_cd (VteTerminal *terminal, GValueArray *params)
{
	VteRowData *rowdata;
	glong i;
	VteScreen *screen;

	screen = terminal->pvt->screen;
	/* If the cursor is actually on the screen, clear the rest of the
	 * row the cursor is on and all of the rows below the cursor. */
	i = screen->cursor_current.row;
	if (i < _vte_ring_next(screen->row_data)) {
		/* Get the data for the row we're clipping. */
		rowdata = _vte_ring_index_writable (screen->row_data, i);
		/* Clear everything to the right of the cursor. */
		if (rowdata)
			_vte_row_data_shrink (rowdata, screen->cursor_current.col);
	}
	/* Now for the rest of the lines. */
	for (i = screen->cursor_current.row + 1;
	     i < _vte_ring_next(screen->row_data);
	     i++) {
		/* Get the data for the row we're removing. */
		rowdata = _vte_ring_index_writable (screen->row_data, i);
		/* Remove it. */
		if (rowdata)
			_vte_row_data_shrink (rowdata, 0);
	}
	/* Now fill the cleared areas. */
	for (i = screen->cursor_current.row;
	     i < screen->insert_delta + terminal->row_count;
	     i++) {
		/* Retrieve the row's data, creating it if necessary. */
		if (_vte_ring_contains (screen->row_data, i)) {
			rowdata = _vte_ring_index_writable (screen->row_data, i);
			g_assert(rowdata != NULL);
		} else {
			rowdata = _vte_terminal_ring_append (terminal, FALSE);
		}
		/* Pad out the row. */
		_vte_row_data_fill (rowdata, &screen->fill_defaults, terminal->column_count);
		rowdata->attr.soft_wrapped = 0;
		/* Repaint this row. */
		_vte_invalidate_cells(terminal,
				      0, terminal->column_count,
				      i, 1);
	}

	/* We've modified the display.  Make a note of it. */
	terminal->pvt->text_deleted_flag = TRUE;
}

/**
 * vte_sequence_handler_ce
 *
 * Clear from the cursor position to the end of the line.
 */
static void
vte_sequence_handler_ce (VteTerminal *terminal, GValueArray *params)
{
	VteRowData *rowdata;
	VteScreen *screen;

	screen = terminal->pvt->screen;
	/* Get the data for the row which the cursor points to. */
	rowdata = _vte_terminal_ensure_row(terminal);
	g_assert(rowdata != NULL);
	/* Remove the data at the end of the array until the current column
	 * is the end of the array. */
	if ((glong) _vte_row_data_length (rowdata) > screen->cursor_current.col) {
		_vte_row_data_shrink (rowdata, screen->cursor_current.col);
		/* We've modified the display.  Make a note of it. */
		terminal->pvt->text_deleted_flag = TRUE;
	}
	if (screen->fill_defaults.attr.back != VTE_DEF_BG) {
		/* Add enough cells to fill out the row. */
		_vte_row_data_fill (rowdata, &screen->fill_defaults, terminal->column_count);
	}
	rowdata->attr.soft_wrapped = 0;
	/* Repaint this row. */
	_vte_invalidate_cells(terminal,
			      screen->cursor_current.col,
			      terminal->column_count -
			      screen->cursor_current.col,
			      screen->cursor_current.row, 1);
}

/**
 * vte_sequence_handler_ch
 *
 * Move the cursor to the given column (horizontal position).
 */
static void
vte_sequence_handler_ch (VteTerminal *terminal, GValueArray *params)
{
	VteScreen *screen;
	GValue *value;
	long val;

	screen = terminal->pvt->screen;
	/* We only care if there's a parameter in there. */
	if ((params != NULL) && (params->n_values > 0)) {
		value = g_value_array_get_nth(params, 0);
		if (G_VALUE_HOLDS_LONG(value)) {
			val = CLAMP(g_value_get_long(value),
				    0,
				    terminal->column_count - 1);
			/* Move the cursor. */
			screen->cursor_current.col = val;
			_vte_terminal_cleanup_tab_fragments_at_cursor (terminal);
		}
	}
}

/**
 * vte_sequence_handler_cl
 *
 * Clear the screen and home the cursor.
 */
static void
vte_sequence_handler_cl (VteTerminal *terminal, GValueArray *params)
{
	vte_terminal_clear_screen (terminal);
	vte_terminal_home_cursor (terminal);

	/* We've modified the display.  Make a note of it. */
	terminal->pvt->text_deleted_flag = TRUE;
}

/**
 * vte_sequence_handler_cm
 *
 * Move the cursor to the given position.
 */
static void
vte_sequence_handler_cm (VteTerminal *terminal, GValueArray *params)
{
	GValue *row, *col;
	long rowval, colval, origin;
	VteScreen *screen;

	screen = terminal->pvt->screen;

	/* We need at least two parameters. */
	rowval = colval = 0;
	if (params != NULL && params->n_values >= 1) {
		/* The first is the row, the second is the column. */
		row = g_value_array_get_nth(params, 0);
		if (G_VALUE_HOLDS_LONG(row)) {
			if (screen->origin_mode &&
			    screen->scrolling_restricted) {
				origin = screen->scrolling_region.start;
			} else {
				origin = 0;
			}
			rowval = g_value_get_long(row) + origin;
			rowval = CLAMP(rowval, 0, terminal->row_count - 1);
		}
		if (params->n_values >= 2) {
			col = g_value_array_get_nth(params, 1);
			if (G_VALUE_HOLDS_LONG(col)) {
				colval = g_value_get_long(col);
				colval = CLAMP(colval, 0, terminal->column_count - 1);
			}
		}
	}
	screen->cursor_current.row = rowval + screen->insert_delta;
	screen->cursor_current.col = colval;
	_vte_terminal_cleanup_tab_fragments_at_cursor (terminal);
}

/**
 * vte_sequence_handler_cr
 *
 * Carriage return.
 */
static void
vte_sequence_handler_cr (VteTerminal *terminal, GValueArray *params)
{
	terminal->pvt->screen->cursor_current.col = 0;
}

/**
 * vte_sequence_handler_cs
 *
 * Restrict scrolling and updates to a subset of the visible lines.
 */
static void
vte_sequence_handler_cs (VteTerminal *terminal, GValueArray *params)
{
	long start=-1, end=-1, rows;
	GValue *value;
	VteScreen *screen;

	vte_terminal_home_cursor (terminal);

	/* We require two parameters.  Anything less is a reset. */
	screen = terminal->pvt->screen;
	if ((params == NULL) || (params->n_values < 2)) {
		screen->scrolling_restricted = FALSE;
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
	rows = terminal->row_count;
	if (start <= 0 || start >= rows) {
		start = 0;
	}
	if (end <= 0 || end >= rows) {
		end = rows - 1;
	}
	/* Set the right values. */
	screen->scrolling_region.start = start;
	screen->scrolling_region.end = end;
	screen->scrolling_restricted = TRUE;
	/* Special case -- run wild, run free. */
	if (screen->scrolling_region.start == 0 &&
	    screen->scrolling_region.end == rows - 1) {
		screen->scrolling_restricted = FALSE;
	}
}

/**
 * vte_sequence_handler_cS
 *
 * Restrict scrolling and updates to a subset of the visible lines, because
 * GNU Emacs is special.
 */
static void
vte_sequence_handler_cS (VteTerminal *terminal, GValueArray *params)
{
	long start=0, end=terminal->row_count-1, rows;
	GValue *value;
	VteScreen *screen;

	/* We require four parameters. */
	screen = terminal->pvt->screen;
	if ((params == NULL) || (params->n_values < 2)) {
		screen->scrolling_restricted = FALSE;
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
	screen->scrolling_region.start = start;
	screen->scrolling_region.end = end;
	screen->scrolling_restricted = TRUE;
	/* Special case -- run wild, run free. */
	rows = terminal->row_count;
	if ((screen->scrolling_region.start == 0) &&
	    (screen->scrolling_region.end == rows - 1)) {
		screen->scrolling_restricted = FALSE;
	}
	/* Clamp the cursor to the scrolling region. */
	screen->cursor_current.row = CLAMP(screen->cursor_current.row,
					   screen->insert_delta + start,
					   screen->insert_delta + end);
}

/**
 * vte_sequence_handler_cv
 *
 * Move the cursor to the given row (vertical position).
 */
static void
vte_sequence_handler_cv (VteTerminal *terminal, GValueArray *params)
{
	VteScreen *screen;
	GValue *value;
	long val, origin;
	screen = terminal->pvt->screen;
	/* We only care if there's a parameter in there. */
	if ((params != NULL) && (params->n_values > 0)) {
		value = g_value_array_get_nth(params, 0);
		if (G_VALUE_HOLDS_LONG(value)) {
			/* Move the cursor. */
			if (screen->origin_mode &&
			    screen->scrolling_restricted) {
				origin = screen->scrolling_region.start;
			} else {
				origin = 0;
			}
			val = g_value_get_long(value) + origin;
			val = CLAMP(val, 0, terminal->row_count - 1);
			screen->cursor_current.row = screen->insert_delta + val;
		}
	}
}

/**
 * vte_sequence_handler_dc
 *
 * Delete a character at the current cursor position.
 */
static void
vte_sequence_handler_dc (VteTerminal *terminal, GValueArray *params)
{
	VteScreen *screen;
	VteRowData *rowdata;
	long col;

	screen = terminal->pvt->screen;

	if (_vte_ring_next(screen->row_data) > screen->cursor_current.row) {
		long len;
		/* Get the data for the row which the cursor points to. */
		rowdata = _vte_ring_index_writable (screen->row_data, screen->cursor_current.row);
		g_assert(rowdata != NULL);
		col = screen->cursor_current.col;
		len = _vte_row_data_length (rowdata);
		/* Remove the column. */
		if (col < len) {
			_vte_row_data_remove (rowdata, col);
			if (screen->fill_defaults.attr.back != VTE_DEF_BG) {
				_vte_row_data_fill (rowdata, &screen->fill_defaults, terminal->column_count);
				len = terminal->column_count;
			}
			/* Repaint this row. */
			_vte_invalidate_cells(terminal,
					col, len - col,
					screen->cursor_current.row, 1);
		}
	}

	/* We've modified the display.  Make a note of it. */
	terminal->pvt->text_deleted_flag = TRUE;
}

/**
 * vte_sequence_handler_DC
 *
 * Delete N characters at the current cursor position.
 */
static void
vte_sequence_handler_DC (VteTerminal *terminal, GValueArray *params)
{
	vte_sequence_handler_multiple(terminal, params, vte_sequence_handler_dc);
}

/**
 * vte_sequence_handler_dl
 *
 * Delete a line at the current cursor position.
 */
static void
vte_sequence_handler_dl (VteTerminal *terminal, GValueArray *params)
{
	VteScreen *screen;
	long start, end, param, i;
	GValue *value;

	/* Find out which part of the screen we're messing with. */
	screen = terminal->pvt->screen;
	start = screen->cursor_current.row;
	if (screen->scrolling_restricted) {
		end = screen->insert_delta + screen->scrolling_region.end;
	} else {
		end = screen->insert_delta + terminal->row_count - 1;
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
		_vte_terminal_ring_remove (terminal, start);
		_vte_terminal_ring_insert (terminal, end, TRUE);
		/* Adjust the scrollbars if necessary. */
		_vte_terminal_adjust_adjustments(terminal);
	}

	/* Update the display. */
	_vte_terminal_scroll_region(terminal, start, end - start + 1, -param);

	/* We've modified the display.  Make a note of it. */
	terminal->pvt->text_deleted_flag = TRUE;
}

/**
 * vte_sequence_handler_DL
 *
 * Delete N lines at the current cursor position.
 */
static void
vte_sequence_handler_DL (VteTerminal *terminal, GValueArray *params)
{
	vte_sequence_handler_dl (terminal, params);
}

/**
 * vte_sequence_handler_do
 *
 * Cursor down, no scrolling.
 */
static void
vte_sequence_handler_do (VteTerminal *terminal, GValueArray *params)
{
	long start, end;
	VteScreen *screen;

	screen = terminal->pvt->screen;

	if (screen->scrolling_restricted) {
		start = screen->insert_delta + screen->scrolling_region.start;
		end = screen->insert_delta + screen->scrolling_region.end;
	} else {
		start = screen->insert_delta;
		end = start + terminal->row_count - 1;
	}

	/* Move the cursor down. */
	screen->cursor_current.row = MIN(screen->cursor_current.row + 1, end);
}

/**
 * vte_sequence_handler_DO
 *
 * Cursor down, no scrolling.
 */
static void
vte_sequence_handler_DO (VteTerminal *terminal, GValueArray *params)
{
	vte_sequence_handler_multiple(terminal, params, vte_sequence_handler_do);
}

/**
 * vte_sequence_handler_ec
 *
 * Erase characters starting at the cursor position (overwriting N with
 * spaces, but not moving the cursor).
 */
static void
vte_sequence_handler_ec (VteTerminal *terminal, GValueArray *params)
{
	VteScreen *screen;
	VteRowData *rowdata;
	GValue *value;
	VteCell *cell;
	long col, i, count;

	screen = terminal->pvt->screen;

	/* If we got a parameter, use it. */
	count = 1;
	if ((params != NULL) && (params->n_values > 0)) {
		value = g_value_array_get_nth(params, 0);
		if (G_VALUE_HOLDS_LONG(value)) {
			count = g_value_get_long(value);
		}
	}

	/* Clear out the given number of characters. */
	rowdata = _vte_terminal_ensure_row(terminal);
	if (_vte_ring_next(screen->row_data) > screen->cursor_current.row) {
		g_assert(rowdata != NULL);
		/* Write over the characters.  (If there aren't enough, we'll
		 * need to create them.) */
		for (i = 0; i < count; i++) {
			col = screen->cursor_current.col + i;
			if (col >= 0) {
				if (col < (glong) _vte_row_data_length (rowdata)) {
					/* Replace this cell with the current
					 * defaults. */
					cell = _vte_row_data_get_writable (rowdata, col);
					*cell = screen->color_defaults;
				} else {
					/* Add new cells until we have one here. */
					_vte_row_data_fill (rowdata, &screen->color_defaults, col);
				}
			}
		}
		/* Repaint this row. */
		_vte_invalidate_cells(terminal,
				      screen->cursor_current.col, count,
				      screen->cursor_current.row, 1);
	}

	/* We've modified the display.  Make a note of it. */
	terminal->pvt->text_deleted_flag = TRUE;
}

/**
 * vte_sequence_handler_fs
 *
 * Move from status line.
 */
static void
vte_sequence_handler_fs (VteTerminal *terminal, GValueArray *params)
{
	terminal->pvt->screen->status_line = FALSE;
}

/**
 * vte_sequence_handler_ho
 *
 * Move the cursor to the home position.
 */
static void
vte_sequence_handler_ho (VteTerminal *terminal, GValueArray *params)
{
	vte_terminal_home_cursor (terminal);
}

/**
 * vte_sequence_handler_ic
 *
 * Insert a character.
 */
static void
vte_sequence_handler_ic (VteTerminal *terminal, GValueArray *params)
{
	VteVisualPosition save;
	VteScreen *screen;

	screen = terminal->pvt->screen;

	save = screen->cursor_current;

	_vte_terminal_insert_char(terminal, ' ', TRUE, TRUE);

	screen->cursor_current = save;
}

/**
 * vte_sequence_handler_IC
 *
 * Insert N characters.
 */
static void
vte_sequence_handler_IC (VteTerminal *terminal, GValueArray *params)
{
	vte_sequence_handler_multiple(terminal, params, vte_sequence_handler_ic);
}

/**
 * vte_sequence_handler_kb
 *
 * Send me a backspace key sym, will you?  Guess that the application meant
 * to send the cursor back one position.
 */
static void
vte_sequence_handler_kb (VteTerminal *terminal, GValueArray *params)
{
	/* Move the cursor left. */
	vte_sequence_handler_le (terminal, params);
}

/**
 * vte_sequence_handler_ke
 *
 * Keypad mode end.
 */
static void
vte_sequence_handler_ke (VteTerminal *terminal, GValueArray *params)
{
	terminal->pvt->keypad_mode = VTE_KEYMODE_NORMAL;
}

/**
 * vte_sequence_handler_ks
 *
 * Keypad mode start.
 */
static void
vte_sequence_handler_ks (VteTerminal *terminal, GValueArray *params)
{
	terminal->pvt->keypad_mode = VTE_KEYMODE_APPLICATION;
}

/**
 * vte_sequence_handler_le
 *
 * Cursor left.
 */
static void
vte_sequence_handler_le (VteTerminal *terminal, GValueArray *params)
{
	VteScreen *screen;

	screen = terminal->pvt->screen;
	if (screen->cursor_current.col > 0) {
		/* There's room to move left, so do so. */
		screen->cursor_current.col--;
		_vte_terminal_cleanup_tab_fragments_at_cursor (terminal);
	} else {
		if (terminal->pvt->flags.bw) {
			/* Wrap to the previous line. */
			screen->cursor_current.col = terminal->column_count - 1;
			if (screen->scrolling_restricted) {
				vte_sequence_handler_sr (terminal, params);
			} else {
				screen->cursor_current.row = MAX(screen->cursor_current.row - 1,
								 screen->insert_delta);
			}
		} else {
			/* Stick to the first column. */
			screen->cursor_current.col = 0;
		}
	}
}

/**
 * vte_sequence_handler_LE
 *
 * Move the cursor left N columns.
 */
static void
vte_sequence_handler_LE (VteTerminal *terminal, GValueArray *params)
{
	vte_sequence_handler_multiple(terminal, params, vte_sequence_handler_le);
}

/**
 * vte_sequence_handler_ll
 *
 * Move the cursor to the lower left corner of the display.
 */
static void
vte_sequence_handler_ll (VteTerminal *terminal, GValueArray *params)
{
	VteScreen *screen;
	screen = terminal->pvt->screen;
	screen->cursor_current.row = MAX(screen->insert_delta,
					 screen->insert_delta +
					 terminal->row_count - 1);
	screen->cursor_current.col = 0;
}

/**
 * vte_sequence_handler_mb
 *
 * Blink on.
 */
static void
vte_sequence_handler_mb (VteTerminal *terminal, GValueArray *params)
{
	terminal->pvt->screen->defaults.attr.blink = 1;
}

/**
 * vte_sequence_handler_md
 *
 * Bold on.
 */
static void
vte_sequence_handler_md (VteTerminal *terminal, GValueArray *params)
{
	terminal->pvt->screen->defaults.attr.bold = 1;
	terminal->pvt->screen->defaults.attr.half = 0;
}

/**
 * vte_sequence_handler_me
 *
 * End modes.
 */
static void
vte_sequence_handler_me (VteTerminal *terminal, GValueArray *params)
{
	_vte_terminal_set_default_attributes(terminal);
}

/**
 * vte_sequence_handler_mh
 *
 * Half-bright on.
 */
static void
vte_sequence_handler_mh (VteTerminal *terminal, GValueArray *params)
{
	terminal->pvt->screen->defaults.attr.half = 1;
	terminal->pvt->screen->defaults.attr.bold = 0;
}

/**
 * vte_sequence_handler_mk
 *
 * Invisible on.
 */
static void
vte_sequence_handler_mk (VteTerminal *terminal, GValueArray *params)
{
	terminal->pvt->screen->defaults.attr.invisible = 1;
}

/**
 * vte_sequence_handler_mr
 *
 * Reverse on.
 */
static void
vte_sequence_handler_mr (VteTerminal *terminal, GValueArray *params)
{
	terminal->pvt->screen->defaults.attr.reverse = 1;
}

/**
 * vte_sequence_handler_nd
 *
 * Cursor right.
 */
static void
vte_sequence_handler_nd (VteTerminal *terminal, GValueArray *params)
{
	VteScreen *screen;
	screen = terminal->pvt->screen;
	if ((screen->cursor_current.col + 1) < terminal->column_count) {
		/* There's room to move right. */
		screen->cursor_current.col++;
	}
}

/**
 * vte_sequence_handler_nw
 *
 * Carriage return command(?).
 */
static void
vte_sequence_handler_nw (VteTerminal *terminal, GValueArray *params)
{
	vte_sequence_handler_cr (terminal, params);
}

/**
 * vte_sequence_handler_rc
 *
 * Restore cursor (position).
 */
static void
vte_sequence_handler_rc (VteTerminal *terminal, GValueArray *params)
{
	VteScreen *screen;
	screen = terminal->pvt->screen;
	screen->cursor_current.col = screen->cursor_saved.col;
	screen->cursor_current.row = CLAMP(screen->cursor_saved.row +
					   screen->insert_delta,
					   screen->insert_delta,
					   screen->insert_delta +
					   terminal->row_count - 1);
}

/**
 * vte_sequence_handler_RI
 *
 * Cursor right N characters.
 */
static void
vte_sequence_handler_RI (VteTerminal *terminal, GValueArray *params)
{
	vte_sequence_handler_multiple(terminal, params, vte_sequence_handler_nd);
}

/**
 * vte_sequence_handler_sc
 *
 * Save cursor (position).
 */
static void
vte_sequence_handler_sc (VteTerminal *terminal, GValueArray *params)
{
	VteScreen *screen;
	screen = terminal->pvt->screen;
	screen->cursor_saved.col = screen->cursor_current.col;
	screen->cursor_saved.row = CLAMP(screen->cursor_current.row -
					 screen->insert_delta,
					 0, terminal->row_count - 1);
}

/**
 * vte_sequence_handler_se
 *
 * Standout end.
 */
static void
vte_sequence_handler_se (VteTerminal *terminal, GValueArray *params)
{
	char *standout;

	/* Standout may be mapped to another attribute, so attempt to do
	 * the Right Thing here.
	 *
	 * If the standout sequence is the same as another sequence, do what
	 * we'd do for that other sequence instead. */

	standout = _vte_termcap_find_string(terminal->pvt->termcap,
					    terminal->pvt->emulation,
					    "so");
	g_assert(standout != NULL);

	if (vte_terminal_termcap_string_same_as_for (terminal, standout, "mb") /* blink */   ||
	    vte_terminal_termcap_string_same_as_for (terminal, standout, "md") /* bold */    ||
	    vte_terminal_termcap_string_same_as_for (terminal, standout, "mh") /* half */    ||
	    vte_terminal_termcap_string_same_as_for (terminal, standout, "mr") /* reverse */ ||
	    vte_terminal_termcap_string_same_as_for (terminal, standout, "us") /* underline */)
	{
		vte_sequence_handler_me (terminal, params);
	} else {
		/* Otherwise just set standout mode. */
		terminal->pvt->screen->defaults.attr.standout = 0;
	}

	g_free(standout);
}

/**
 * vte_sequence_handler_sf
 *
 * Cursor down, with scrolling.
 */
static void
vte_sequence_handler_sf (VteTerminal *terminal, GValueArray *params)
{
	_vte_terminal_cursor_down (terminal);
}

/**
 * vte_sequence_handler_SF
 *
 * Cursor down, with scrolling.
 */
static void
vte_sequence_handler_SF (VteTerminal *terminal, GValueArray *params)
{
	/* XXX implement this directly in _vte_terminal_cursor_down */
	vte_sequence_handler_multiple(terminal, params, vte_sequence_handler_sf);
}

/**
 * vte_sequence_handler_so
 *
 * Standout start.
 */
static void
vte_sequence_handler_so (VteTerminal *terminal, GValueArray *params)
{
	char *standout;

	/* Standout may be mapped to another attribute, so attempt to do
	 * the Right Thing here.
	 *
	 * If the standout sequence is the same as another sequence, do what
	 * we'd do for that other sequence instead. */

	standout = _vte_termcap_find_string(terminal->pvt->termcap,
					    terminal->pvt->emulation,
					    "so");
	g_assert(standout != NULL);

	if (vte_terminal_termcap_string_same_as_for (terminal, standout, "mb") /* blink */)
		vte_sequence_handler_mb (terminal, params);
	else if (vte_terminal_termcap_string_same_as_for (terminal, standout, "md") /* bold */)
		vte_sequence_handler_md (terminal, params);
	else if (vte_terminal_termcap_string_same_as_for (terminal, standout, "mh") /* half */)
		vte_sequence_handler_mh (terminal, params);
	else if (vte_terminal_termcap_string_same_as_for (terminal, standout, "mr") /* reverse */)
		vte_sequence_handler_mr (terminal, params);
	else if (vte_terminal_termcap_string_same_as_for (terminal, standout, "us") /* underline */)
		vte_sequence_handler_us (terminal, params);
	else {
		/* Otherwise just set standout mode. */
		terminal->pvt->screen->defaults.attr.standout = 1;
	}

	g_free(standout);
}

/**
 * vte_sequence_handler_sr
 *
 * Cursor up, scrolling if need be.
 */
static void
vte_sequence_handler_sr (VteTerminal *terminal, GValueArray *params)
{
	long start, end;
	VteScreen *screen;

	screen = terminal->pvt->screen;

	if (screen->scrolling_restricted) {
		start = screen->scrolling_region.start + screen->insert_delta;
		end = screen->scrolling_region.end + screen->insert_delta;
	} else {
		start = terminal->pvt->screen->insert_delta;
		end = start + terminal->row_count - 1;
	}

	if (screen->cursor_current.row == start) {
		/* If we're at the top of the scrolling region, add a
		 * line at the top to scroll the bottom off. */
		_vte_terminal_ring_remove (terminal, end);
		_vte_terminal_ring_insert (terminal, start, TRUE);
		/* Update the display. */
		_vte_terminal_scroll_region(terminal, start, end - start + 1, 1);
		_vte_invalidate_cells(terminal,
				      0, terminal->column_count,
				      start, 2);
	} else {
		/* Otherwise, just move the cursor up. */
		screen->cursor_current.row--;
	}
	/* Adjust the scrollbars if necessary. */
	_vte_terminal_adjust_adjustments(terminal);
	/* We modified the display, so make a note of it. */
	terminal->pvt->text_modified_flag = TRUE;
}

/**
 * vte_sequence_handler_SR
 *
 * Cursor up, with scrolling.
 */
static void
vte_sequence_handler_SR (VteTerminal *terminal, GValueArray *params)
{
	vte_sequence_handler_multiple(terminal, params, vte_sequence_handler_sr);
}

/**
 * vte_sequence_handler_ta
 *
 * Tab.
 */
static void
vte_sequence_handler_ta (VteTerminal *terminal, GValueArray *params)
{
	VteScreen *screen;
	long old_len, newcol, col;

	/* Calculate which column is the next tab stop. */
	screen = terminal->pvt->screen;
	newcol = col = screen->cursor_current.col;

	g_assert (col >= 0);

	if (terminal->pvt->tabstops != NULL) {
		/* Find the next tabstop. */
		for (newcol++; newcol < VTE_TAB_MAX; newcol++) {
			if (_vte_terminal_get_tabstop(terminal, newcol)) {
				break;
			}
		}
	}

	/* If we have no tab stops or went past the end of the line, stop
	 * at the right-most column. */
	if (newcol >= terminal->column_count) {
		newcol = terminal->column_count - 1;
	}

	/* but make sure we don't move cursor back (bug #340631) */
	if (col < newcol) {
		VteRowData *rowdata = _vte_terminal_ensure_row (terminal);

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
		_vte_row_data_fill (rowdata, &screen->fill_defaults, newcol);

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

		_vte_invalidate_cells (terminal,
				screen->cursor_current.col,
				newcol - screen->cursor_current.col,
				screen->cursor_current.row, 1);
		screen->cursor_current.col = newcol;
	}
}

/**
 * vte_sequence_handler_ts
 *
 * Move to status line.
 */
static void
vte_sequence_handler_ts (VteTerminal *terminal, GValueArray *params)
{
	terminal->pvt->screen->status_line = TRUE;
	terminal->pvt->screen->status_line_changed = TRUE;
	g_string_truncate(terminal->pvt->screen->status_line_contents, 0);
}

/**
 * vte_sequence_handler_uc
 *
 * Underline this character and move right.
 */
static void
vte_sequence_handler_uc (VteTerminal *terminal, GValueArray *params)
{
	VteCell *cell;
	int column;
	VteScreen *screen;

	screen = terminal->pvt->screen;
	column = screen->cursor_current.col;
	cell = vte_terminal_find_charcell(terminal, column, screen->cursor_current.row);
	while ((cell != NULL) && (cell->attr.fragment) && (column > 0)) {
		column--;
		cell = vte_terminal_find_charcell(terminal, column, screen->cursor_current.row);
	}
	if (cell != NULL) {
		/* Set this character to be underlined. */
		cell->attr.underline = 1;
		/* Cause the character to be repainted. */
		_vte_invalidate_cells(terminal,
				      column, cell->attr.columns,
				      screen->cursor_current.row, 1);
		/* Move the cursor right. */
		vte_sequence_handler_nd (terminal, params);
	}

	/* We've modified the display without changing the text.  Make a note
	 * of it. */
	terminal->pvt->text_modified_flag = TRUE;
}

/**
 * vte_sequence_handler_ue
 *
 * Underline end.
 */
static void
vte_sequence_handler_ue (VteTerminal *terminal, GValueArray *params)
{
	terminal->pvt->screen->defaults.attr.underline = 0;
}

/**
 * vte_sequence_handler_up
 *
 * Cursor up, no scrolling.
 */
static void
vte_sequence_handler_up (VteTerminal *terminal, GValueArray *params)
{
	VteScreen *screen;
	long start, end;

	screen = terminal->pvt->screen;

	if (screen->scrolling_restricted) {
		start = screen->insert_delta + screen->scrolling_region.start;
		end = screen->insert_delta + screen->scrolling_region.end;
	} else {
		start = screen->insert_delta;
		end = start + terminal->row_count - 1;
	}

	screen->cursor_current.row = MAX(screen->cursor_current.row - 1, start);
}

/**
 * vte_sequence_handler_UP
 *
 * Cursor up N lines, no scrolling.
 */
static void
vte_sequence_handler_UP (VteTerminal *terminal, GValueArray *params)
{
	vte_sequence_handler_multiple(terminal, params, vte_sequence_handler_up);
}

/**
 * vte_sequence_handler_us
 *
 * Underline start.
 */
static void
vte_sequence_handler_us (VteTerminal *terminal, GValueArray *params)
{
	terminal->pvt->screen->defaults.attr.underline = 1;
}


/**
 * vte_sequence_handler_character_attributes
 *
 * Handle ANSI color setting and related stuffs (SGR).
 */
static void
vte_sequence_handler_character_attributes (VteTerminal *terminal, GValueArray *params)
{
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
			_vte_terminal_set_default_attributes(terminal);
			break;
		case 1:
			terminal->pvt->screen->defaults.attr.bold = 1;
			terminal->pvt->screen->defaults.attr.half = 0;
			break;
		case 2:
			terminal->pvt->screen->defaults.attr.half = 1;
			terminal->pvt->screen->defaults.attr.bold = 0;
			break;
		case 4:
			terminal->pvt->screen->defaults.attr.underline = 1;
			break;
		case 5:
			terminal->pvt->screen->defaults.attr.blink = 1;
			break;
		case 7:
			terminal->pvt->screen->defaults.attr.reverse = 1;
			break;
		case 8:
			terminal->pvt->screen->defaults.attr.invisible = 1;
			break;
		case 9:
			terminal->pvt->screen->defaults.attr.strikethrough = 1;
			break;
		case 21: /* Error in old versions of linux console. */
		case 22: /* ECMA 48. */
			terminal->pvt->screen->defaults.attr.bold = 0;
			terminal->pvt->screen->defaults.attr.half = 0;
			break;
		case 24:
			terminal->pvt->screen->defaults.attr.underline = 0;
			break;
		case 25:
			terminal->pvt->screen->defaults.attr.blink = 0;
			break;
		case 27:
			terminal->pvt->screen->defaults.attr.reverse = 0;
			break;
		case 28:
			terminal->pvt->screen->defaults.attr.invisible = 0;
			break;
		case 29:
			terminal->pvt->screen->defaults.attr.strikethrough = 0;
			break;
		case 30:
		case 31:
		case 32:
		case 33:
		case 34:
		case 35:
		case 36:
		case 37:
			terminal->pvt->screen->defaults.attr.fore = param - 30;
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
					terminal->pvt->screen->defaults.attr.fore = param2;
				i += 2;
			}
			break;
		}
		case 39:
			/* default foreground */
			terminal->pvt->screen->defaults.attr.fore = VTE_DEF_FG;
			break;
		case 40:
		case 41:
		case 42:
		case 43:
		case 44:
		case 45:
		case 46:
		case 47:
			terminal->pvt->screen->defaults.attr.back = param - 40;
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
					terminal->pvt->screen->defaults.attr.back = param2;
				i += 2;
			}
			break;
		}
		case 49:
			/* default background */
			terminal->pvt->screen->defaults.attr.back = VTE_DEF_BG;
			break;
		case 90:
		case 91:
		case 92:
		case 93:
		case 94:
		case 95:
		case 96:
		case 97:
			terminal->pvt->screen->defaults.attr.fore = param - 90 + VTE_COLOR_BRIGHT_OFFSET;
			break;
		case 100:
		case 101:
		case 102:
		case 103:
		case 104:
		case 105:
		case 106:
		case 107:
			terminal->pvt->screen->defaults.attr.back = param - 100 + VTE_COLOR_BRIGHT_OFFSET;
			break;
		}
	}
	/* If we had no parameters, default to the defaults. */
	if (i == 0) {
		_vte_terminal_set_default_attributes(terminal);
	}
	/* Save the new colors. */
	terminal->pvt->screen->color_defaults.attr.fore =
		terminal->pvt->screen->defaults.attr.fore;
	terminal->pvt->screen->color_defaults.attr.back =
		terminal->pvt->screen->defaults.attr.back;
	terminal->pvt->screen->fill_defaults.attr.fore =
		terminal->pvt->screen->defaults.attr.fore;
	terminal->pvt->screen->fill_defaults.attr.back =
		terminal->pvt->screen->defaults.attr.back;
}

/**
 * vte_sequence_handler_character_position_absolute
 *
 * Move the cursor.
 */
static void
vte_sequence_handler_character_position_absolute (VteTerminal *terminal, GValueArray *params)
{
	vte_sequence_handler_offset(terminal, params, -1, vte_sequence_handler_ch);
}

/**
 * vte_sequence_handler_cursor_character_absolute
 *
 * Move the cursor to the given column, 1-based.
 */
static void
vte_sequence_handler_cursor_character_absolute (VteTerminal *terminal, GValueArray *params)
{
	VteScreen *screen;
	GValue *value;
	long val;

	screen = terminal->pvt->screen;

        val = 0;
	if ((params != NULL) && (params->n_values > 0)) {
		value = g_value_array_get_nth(params, 0);
		if (G_VALUE_HOLDS_LONG(value)) {
			val = CLAMP(g_value_get_long(value),
				    1, terminal->column_count) - 1;
		}
	}

        screen->cursor_current.col = val;
	_vte_terminal_cleanup_tab_fragments_at_cursor (terminal);
}

/**
 * vte_sequence_handler_cursor_lower_left
 *
 * Move the cursor to the lower left-hand corner.
 */
static void
vte_sequence_handler_cursor_lower_left (VteTerminal *terminal, GValueArray *params)
{
	VteScreen *screen;
	long row;
	screen = terminal->pvt->screen;
	row = MAX(0, terminal->row_count - 1);
	screen->cursor_current.row = screen->insert_delta + row;
	screen->cursor_current.col = 0;
}

/**
 * vte_sequence_handler_cursor_next_line
 *
 * Move the cursor to the beginning of the next line, scrolling if necessary.
 */
static void
vte_sequence_handler_cursor_next_line (VteTerminal *terminal, GValueArray *params)
{
	terminal->pvt->screen->cursor_current.col = 0;
	vte_sequence_handler_DO (terminal, params);
}

/**
 * vte_sequence_handler_cursor_position
 *
 * Move the cursor to the given position, 1-based.
 */
static void
vte_sequence_handler_cursor_position (VteTerminal *terminal, GValueArray *params)
{
	vte_sequence_handler_offset(terminal, params, -1, vte_sequence_handler_cm);
}

/**
 * vte_sequence_handler_cursor_preceding_line
 *
 * Move the cursor to the beginning of the next line, scrolling if necessary.
 */
static void
vte_sequence_handler_cursor_preceding_line (VteTerminal *terminal, GValueArray *params)
{
	terminal->pvt->screen->cursor_current.col = 0;
	vte_sequence_handler_UP (terminal, params);
}

/**
 * vte_sequence_handler_decset_internal
 *
 * Manipulate certain terminal attributes.
 */
static void
vte_sequence_handler_decset_internal(VteTerminal *terminal,
				     int setting,
				     gboolean restore,
				     gboolean save,
				     gboolean set)
{
	gboolean recognized = FALSE;
	gpointer p;
	guint i;
	struct {
		int setting;
		gboolean *bvalue;
		gint *ivalue;
		gpointer *pvalue;
		gpointer fvalue;
		gpointer tvalue;
		VteTerminalSequenceHandler reset, set;
	} settings[] = {
		/* 1: Application/normal cursor keys. */
		{1, NULL, &terminal->pvt->cursor_mode, NULL,
		 GINT_TO_POINTER(VTE_KEYMODE_NORMAL),
		 GINT_TO_POINTER(VTE_KEYMODE_APPLICATION),
		 NULL, NULL,},
		/* 2: disallowed, we don't do VT52. */
		{2, NULL, NULL, NULL, NULL, NULL, NULL, NULL,},
		/* 3: disallowed, window size is set by user. */
		{3, NULL, NULL, NULL, NULL, NULL, NULL, NULL,},
		/* 4: Smooth scroll. */
		{4, &terminal->pvt->smooth_scroll, NULL, NULL,
		 GINT_TO_POINTER(FALSE),
		 GINT_TO_POINTER(TRUE),
		 NULL, NULL,},
		/* 5: Reverse video. */
		{5, &terminal->pvt->screen->reverse_mode, NULL, NULL,
		 GINT_TO_POINTER(FALSE),
		 GINT_TO_POINTER(TRUE),
		 NULL, NULL,},
		/* 6: Origin mode: when enabled, cursor positioning is
		 * relative to the scrolling region. */
		{6, &terminal->pvt->screen->origin_mode, NULL, NULL,
		 GINT_TO_POINTER(FALSE),
		 GINT_TO_POINTER(TRUE),
		 NULL, NULL,},
		/* 7: Wraparound mode. */
		{7, &terminal->pvt->flags.am, NULL, NULL,
		 GINT_TO_POINTER(FALSE),
		 GINT_TO_POINTER(TRUE),
		 NULL, NULL,},
		/* 8: disallowed, keyboard repeat is set by user. */
		{8, NULL, NULL, NULL, NULL, NULL, NULL, NULL,},
		/* 9: Send-coords-on-click. */
		{9, NULL, &terminal->pvt->mouse_tracking_mode, NULL,
		 GINT_TO_POINTER(0),
		 GINT_TO_POINTER(MOUSE_TRACKING_SEND_XY_ON_CLICK),
		 NULL, NULL,},
		/* 12: disallowed, cursor blinks is set by user. */
		{12, NULL, NULL, NULL, NULL, NULL, NULL, NULL,},
		/* 18: print form feed. */
		/* 19: set print extent to full screen. */
		/* 25: Cursor visible. */
		{25, &terminal->pvt->cursor_visible, NULL, NULL,
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
		{42, &terminal->pvt->nrc_mode, NULL, NULL,
		 GINT_TO_POINTER(FALSE),
		 GINT_TO_POINTER(TRUE),
		 NULL, NULL,},
		/* 44: Margin bell. */
		{44, &terminal->pvt->margin_bell, NULL, NULL,
		 GINT_TO_POINTER(FALSE),
		 GINT_TO_POINTER(TRUE),
		 NULL, NULL,},
		/* 47: Alternate screen. */
		{47, NULL, NULL, (gpointer) &terminal->pvt->screen,
		 &terminal->pvt->normal_screen,
		 &terminal->pvt->alternate_screen,
		 NULL, NULL,},
		/* 66: Keypad mode. */
		{66, &terminal->pvt->keypad_mode, NULL, NULL,
		 GINT_TO_POINTER(VTE_KEYMODE_NORMAL),
		 GINT_TO_POINTER(VTE_KEYMODE_APPLICATION),
		 NULL, NULL,},
		/* 67: disallowed, backspace key policy is set by user. */
		{67, NULL, NULL, NULL, NULL, NULL, NULL, NULL,},
		/* 1000: Send-coords-on-button. */
		{1000, NULL, &terminal->pvt->mouse_tracking_mode, NULL,
		 GINT_TO_POINTER(0),
		 GINT_TO_POINTER(MOUSE_TRACKING_SEND_XY_ON_BUTTON),
		 NULL, NULL,},
		/* 1001: Hilite tracking. */
		{1001, NULL, &terminal->pvt->mouse_tracking_mode, NULL,
		 GINT_TO_POINTER(0),
		 GINT_TO_POINTER(MOUSE_TRACKING_HILITE_TRACKING),
		 NULL, NULL,},
		/* 1002: Cell motion tracking. */
		{1002, NULL, &terminal->pvt->mouse_tracking_mode, NULL,
		 GINT_TO_POINTER(0),
		 GINT_TO_POINTER(MOUSE_TRACKING_CELL_MOTION_TRACKING),
		 NULL, NULL,},
		/* 1003: All motion tracking. */
		{1003, NULL, &terminal->pvt->mouse_tracking_mode, NULL,
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
		{1036, &terminal->pvt->meta_sends_escape, NULL, NULL,
		 GINT_TO_POINTER(FALSE),
		 GINT_TO_POINTER(TRUE),
		 NULL, NULL,},
		/* 1037: disallowed, delete key policy is set by user. */
		{1037, NULL, NULL, NULL, NULL, NULL, NULL, NULL,},
		/* 1047: Use alternate screen buffer. */
		{1047, NULL, NULL, (gpointer) &terminal->pvt->screen,
		 &terminal->pvt->normal_screen,
		 &terminal->pvt->alternate_screen,
		 NULL, NULL,},
		/* 1048: Save/restore cursor position. */
		{1048, NULL, NULL, NULL,
		 NULL,
		 NULL,
		 vte_sequence_handler_rc,
		 vte_sequence_handler_sc,},
		/* 1049: Use alternate screen buffer, saving the cursor
		 * position. */
		{1049, NULL, NULL, (gpointer) &terminal->pvt->screen,
		 &terminal->pvt->normal_screen,
		 &terminal->pvt->alternate_screen,
		 vte_sequence_handler_rc,
		 vte_sequence_handler_sc,},
		/* 1051: Sun function key mode. */
		{1051, NULL, NULL, (gpointer) &terminal->pvt->sun_fkey_mode,
		 GINT_TO_POINTER(FALSE),
		 GINT_TO_POINTER(TRUE),
		 NULL, NULL},
		/* 1052: HP function key mode. */
		{1052, NULL, NULL, (gpointer) &terminal->pvt->hp_fkey_mode,
		 GINT_TO_POINTER(FALSE),
		 GINT_TO_POINTER(TRUE),
		 NULL, NULL},
		/* 1060: Legacy function key mode. */
		{1060, NULL, NULL, (gpointer) &terminal->pvt->legacy_fkey_mode,
		 GINT_TO_POINTER(FALSE),
		 GINT_TO_POINTER(TRUE),
		 NULL, NULL},
		/* 1061: VT220 function key mode. */
		{1061, NULL, NULL, (gpointer) &terminal->pvt->vt220_fkey_mode,
		 GINT_TO_POINTER(FALSE),
		 GINT_TO_POINTER(TRUE),
		 NULL, NULL},
		/* 2004: Bracketed paste mode. */
		{2004, &terminal->pvt->screen->bracketed_paste_mode, NULL, NULL,
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
			p = g_hash_table_lookup(terminal->pvt->dec_saved,
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
			g_hash_table_insert(terminal->pvt->dec_saved,
					    GINT_TO_POINTER(setting),
					    GINT_TO_POINTER(set));
		}
		/* Change the current setting to match the new/saved value. */
		if (!save) {
			_vte_debug_print(VTE_DEBUG_PARSE,
					"Setting %d to %s.\n",
					setting, set ? "set" : "unset");
			if (settings[i].set && set) {
				settings[i].set (terminal, NULL);
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
				settings[i].reset (terminal, NULL);
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
		vte_terminal_emit_resize_window(terminal,
						(set ? 132 : 80) *
						terminal->char_width +
						terminal->pvt->inner_border.left +
                                                terminal->pvt->inner_border.right,
						terminal->row_count *
						terminal->char_height +
						terminal->pvt->inner_border.top +
                                                terminal->pvt->inner_border.bottom);
		/* Request a resize and redraw. */
		_vte_invalidate_all(terminal);
		break;
#endif
	case 5:
		/* Repaint everything in reverse mode. */
		_vte_invalidate_all(terminal);
		break;
	case 6:
		/* Reposition the cursor in its new home position. */
		terminal->pvt->screen->cursor_current.col = 0;
		terminal->pvt->screen->cursor_current.row =
			terminal->pvt->screen->insert_delta;
		break;
	case 47:
	case 1047:
	case 1049:
		/* Clear the alternate screen if we're switching
		 * to it, and home the cursor. */
		if (set) {
			vte_terminal_clear_screen (terminal);
			vte_terminal_home_cursor (terminal);
		}
		/* Reset scrollbars and repaint everything. */
		terminal->adjustment->value =
			terminal->pvt->screen->scroll_delta;
		vte_terminal_set_scrollback_lines(terminal,
				terminal->pvt->scrollback_lines);
		_vte_terminal_queue_contents_changed(terminal);
		_vte_invalidate_all (terminal);
		break;
	case 9:
	case 1000:
	case 1001:
	case 1002:
	case 1003:
		/* Make the pointer visible. */
		_vte_terminal_set_pointer_visible(terminal, TRUE);
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
vte_sequence_handler_decset (VteTerminal *terminal, GValueArray *params)
{
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
		vte_sequence_handler_decset_internal(terminal, setting, FALSE, FALSE, TRUE);
	}
}

/**
 * vte_sequence_handler_decreset
 *
 * Unset certain terminal attributes.
 */
static void
vte_sequence_handler_decreset (VteTerminal *terminal, GValueArray *params)
{
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
		vte_sequence_handler_decset_internal(terminal, setting, FALSE, FALSE, FALSE);
	}
}

/**
 * vte_sequence_handler_delete_lines
 *
 * Delete certain lines from the scrolling region.
 */
static void
vte_sequence_handler_delete_lines (VteTerminal *terminal, GValueArray *params)
{
	GValue *value;
	VteScreen *screen;
	long param, end, row;
	int i;

	screen = terminal->pvt->screen;
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
	row = screen->cursor_current.row;
	if (screen->scrolling_restricted) {
		end = screen->insert_delta + screen->scrolling_region.end;
	} else {
		end = screen->insert_delta + terminal->row_count - 1;
	}
	/* Clear them from below the current cursor. */
	for (i = 0; i < param; i++) {
		/* Insert a line at the end of the region and remove one from
		 * the top of the region. */
		_vte_terminal_ring_remove (terminal, row);
		_vte_terminal_ring_insert (terminal, end, TRUE);
	}
	/* Update the display. */
	_vte_terminal_scroll_region(terminal, row, end - row + 1, -param);
	/* Adjust the scrollbars if necessary. */
	_vte_terminal_adjust_adjustments(terminal);
	/* We've modified the display.  Make a note of it. */
	terminal->pvt->text_deleted_flag = TRUE;
}

/**
 * vte_sequence_handler_erase_characters
 *
 * Erase a specified number of characters.
 */
static void
vte_sequence_handler_erase_characters (VteTerminal *terminal, GValueArray *params)
{
	vte_sequence_handler_ec (terminal, params);
}

/**
 * vte_sequence_handler_erase_in_display
 *
 * Erase certain lines in the display.
 */
static void
vte_sequence_handler_erase_in_display (VteTerminal *terminal, GValueArray *params)
{
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
		vte_sequence_handler_cd (terminal, NULL);
		break;
	case 1:
		/* Clear above the current line. */
		vte_terminal_clear_above_current (terminal);
		/* Clear everything to the left of the cursor, too. */
		/* FIXME: vttest. */
		vte_sequence_handler_cb (terminal, NULL);
		break;
	case 2:
		/* Clear the entire screen. */
		vte_terminal_clear_screen (terminal);
		break;
	default:
		break;
	}
	/* We've modified the display.  Make a note of it. */
	terminal->pvt->text_deleted_flag = TRUE;
}

/**
 * vte_sequence_handler_erase_in_line
 *
 * Erase certain parts of the current line in the display.
 */
static void
vte_sequence_handler_erase_in_line (VteTerminal *terminal, GValueArray *params)
{
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
		vte_sequence_handler_ce (terminal, NULL);
		break;
	case 1:
		/* Clear to start of the line. */
		vte_sequence_handler_cb (terminal, NULL);
		break;
	case 2:
		/* Clear the entire line. */
		vte_terminal_clear_current_line (terminal);
		break;
	default:
		break;
	}
	/* We've modified the display.  Make a note of it. */
	terminal->pvt->text_deleted_flag = TRUE;
}

/**
 * vte_sequence_handler_form_feed
 *
 * Form-feed / next-page.
 */
static void
vte_sequence_handler_form_feed (VteTerminal *terminal, GValueArray *params)
{
	vte_sequence_handler_index (terminal, params);
}

/**
 * vte_sequence_handler_full_reset
 *
 * Perform a full-bore reset.
 */
static void
vte_sequence_handler_full_reset (VteTerminal *terminal, GValueArray *params)
{
	vte_terminal_reset(terminal, TRUE, TRUE);
}

/**
 * vte_sequence_handler_horizontal_and_vertical_position
 *
 * Move the cursor to a specified position.
 */
static void
vte_sequence_handler_horizontal_and_vertical_position (VteTerminal *terminal, GValueArray *params)
{
	vte_sequence_handler_offset(terminal, params, -1, vte_sequence_handler_cm);
}

/**
 * vte_sequence_handler_index
 *
 * Cursor down, with scrolling.
 */
static void
vte_sequence_handler_index (VteTerminal *terminal, GValueArray *params)
{
	vte_sequence_handler_sf (terminal, params);
}

/**
 * vte_sequence_handler_insert_blank_characters
 *
 * Insert a specified number of blank characters.
 */
static void
vte_sequence_handler_insert_blank_characters (VteTerminal *terminal, GValueArray *params)
{
	vte_sequence_handler_IC (terminal, params);
}

/**
 * vte_sequence_handler_insert_lines
 *
 * Insert a certain number of lines below the current cursor.
 */
static void
vte_sequence_handler_insert_lines (VteTerminal *terminal, GValueArray *params)
{
	GValue *value;
	VteScreen *screen;
	long param, end, row;
	int i;
	screen = terminal->pvt->screen;
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
	row = screen->cursor_current.row;
	if (screen->scrolling_restricted) {
		end = screen->insert_delta + screen->scrolling_region.end;
	} else {
		end = screen->insert_delta + terminal->row_count - 1;
	}
	/* Insert the new lines at the cursor. */
	for (i = 0; i < param; i++) {
		/* Clear a line off the end of the region and add one to the
		 * top of the region. */
		_vte_terminal_ring_remove (terminal, end);
		_vte_terminal_ring_insert (terminal, row, TRUE);
	}
	/* Update the display. */
	_vte_terminal_scroll_region(terminal, row, end - row + 1, param);
	/* Adjust the scrollbars if necessary. */
	_vte_terminal_adjust_adjustments(terminal);
	/* We've modified the display.  Make a note of it. */
	terminal->pvt->text_inserted_flag = TRUE;
}

/**
 * vte_sequence_handler_line_position_absolute
 */
static void
vte_sequence_handler_line_position_absolute (VteTerminal *terminal, GValueArray *params)
{
	vte_sequence_handler_offset(terminal, params, -1, vte_sequence_handler_cv);
}

/**
 * vte_sequence_handler_next_line
 *
 * Move the cursor to the beginning of the next line, scrolling if necessary.
 */
static void
vte_sequence_handler_next_line (VteTerminal *terminal, GValueArray *params)
{
	terminal->pvt->screen->cursor_current.col = 0;
	vte_sequence_handler_DO (terminal, params);
}

/**
 * vte_sequence_handler_restore_mode
 *
 * Restore a certain terminal attribute.
 */
static void
vte_sequence_handler_restore_mode (VteTerminal *terminal, GValueArray *params)
{
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
		vte_sequence_handler_decset_internal(terminal, setting, TRUE, FALSE, FALSE);
	}
}

/**
 * vte_sequence_handler_reverse_index
 *
 * Cursor down, with scrolling.
 */
static void
vte_sequence_handler_reverse_index (VteTerminal *terminal, GValueArray *params)
{
	vte_sequence_handler_sr (terminal, params);
}

/**
 * vte_sequence_handler_save_mode
 *
 * Save a certain terminal attribute.
 */
static void
vte_sequence_handler_save_mode (VteTerminal *terminal, GValueArray *params)
{
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
		vte_sequence_handler_decset_internal(terminal, setting, FALSE, TRUE, FALSE);
	}
}

/**
 * vte_sequence_handler_screen_alignment_test
 * Perform a screen alignment test -- fill all visible cells with the
 * letter "E".
 */
static void
vte_sequence_handler_screen_alignment_test (VteTerminal *terminal, GValueArray *params)
{
	long row;
	VteRowData *rowdata;
	VteScreen *screen;
	VteCell cell;

	screen = terminal->pvt->screen;

	for (row = terminal->pvt->screen->insert_delta;
	     row < terminal->pvt->screen->insert_delta + terminal->row_count;
	     row++) {
		/* Find this row. */
		while (_vte_ring_next(screen->row_data) <= row)
			_vte_terminal_ring_append (terminal, FALSE);
		_vte_terminal_adjust_adjustments(terminal);
		rowdata = _vte_ring_index_writable (screen->row_data, row);
		g_assert(rowdata != NULL);
		/* Clear this row. */
		_vte_row_data_shrink (rowdata, 0);

		_vte_terminal_emit_text_deleted(terminal);
		/* Fill this row. */
		cell.c = 'E';
		cell.attr = basic_cell.cell.attr;
		cell.attr.columns = 1;
		_vte_row_data_fill (rowdata, &cell, terminal->column_count);
		_vte_terminal_emit_text_inserted(terminal);
	}
	_vte_invalidate_all(terminal);

	/* We modified the display, so make a note of it for completeness. */
	terminal->pvt->text_modified_flag = TRUE;
}

/**
 * vte_sequence_handler_scroll_down
 *
 * Scroll the text down, but don't move the cursor.
 */
static void
vte_sequence_handler_scroll_down (VteTerminal *terminal, GValueArray *params)
{
	long val = 1;
	GValue *value;

	if ((params != NULL) && (params->n_values > 0)) {
		value = g_value_array_get_nth(params, 0);
		if (G_VALUE_HOLDS_LONG(value)) {
			val = g_value_get_long(value);
			val = MAX(val, 1);
		}
	}

	vte_terminal_scroll_text (terminal, val);
}

/**
 * vte_sequence_handler_scroll_up
 *
 * Scroll the text up, but don't move the cursor.
 */
static void
vte_sequence_handler_scroll_up (VteTerminal *terminal, GValueArray *params)
{
	long val = 1;
	GValue *value;

	if ((params != NULL) && (params->n_values > 0)) {
		value = g_value_array_get_nth(params, 0);
		if (G_VALUE_HOLDS_LONG(value)) {
			val = g_value_get_long(value);
			val = MAX(val, 1);
		}
	}

	vte_terminal_scroll_text (terminal, -val);
}

/**
 * vte_sequence_handler_set_scrolling_region
 *
 * Restrict the scrolling region.
 */
static void
vte_sequence_handler_set_scrolling_region (VteTerminal *terminal, GValueArray *params)
{
	vte_sequence_handler_offset(terminal, params, -1, vte_sequence_handler_cs);
}

/**
 * vte_sequence_handler_set_scrolling_region_from_start
 */
static void
vte_sequence_handler_set_scrolling_region_from_start (VteTerminal *terminal, GValueArray *params)
{
	GValue value = {0};

	g_value_init (&value, G_TYPE_LONG);
	g_value_set_long (&value, 0); /* Out of range means start/end */

	g_value_array_insert (params, 0, &value);

	vte_sequence_handler_offset(terminal, params, -1, vte_sequence_handler_cs);
}

/**
 * vte_sequence_handler_set_scrolling_region_to_end
 */
static void
vte_sequence_handler_set_scrolling_region_to_end (VteTerminal *terminal, GValueArray *params)
{
	GValue value = {0};

	g_value_init (&value, G_TYPE_LONG);
	g_value_set_long (&value, 0); /* Out of range means start/end */

	g_value_array_insert (params, 1, &value);

	vte_sequence_handler_offset(terminal, params, -1, vte_sequence_handler_cs);
}

/**
 * vte_sequence_handler_soft_reset
 *
 * Perform a soft reset.
 */
static void
vte_sequence_handler_soft_reset (VteTerminal *terminal, GValueArray *params)
{
	vte_terminal_reset(terminal, FALSE, FALSE);
}

/**
 * vte_sequence_handler_vertical_tab
 *
 * Vertical tab.
 */
static void
vte_sequence_handler_vertical_tab (VteTerminal *terminal, GValueArray *params)
{
	vte_sequence_handler_index (terminal, params);
}

