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

#include "config.h"

#ifndef RARXXX
// Really?
#include <sys/types.h>
#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <glib-object.h>
#endif

#include <string.h>
#include <glib.h>
#include "debug.h"
#include "iso2022.h"
#include "matcher.h"
#include "table.h"

/* Table info. */
#define VTE_TABLE_MAX_LITERAL (128 + 32)
#define _vte_table_map_literal(__c) \
	(((__c) < (VTE_TABLE_MAX_LITERAL)) ? (__c) : 0)
#define _vte_table_is_numeric(__c) \
	(((__c) >= '0') && ((__c) <= '9'))
#define _vte_table_is_numeric_list(__c) \
	((((__c) >= '0') && ((__c) <= '9')) || (__c) == ';')

/**
 * struct _vte_table
 */
struct _vte_table {
	struct _vte_matcher_impl impl;
	GQuark resultq;
	const char *result;
	unsigned char *original;
	gssize original_length;
	int increment;
	struct _vte_table *table_string;
	struct _vte_table *table_number;
	struct _vte_table *table_number_list;
	struct _vte_table **table;
};

/**
 * enum _vte_table_argtype
 * Argument info.
 */
enum _vte_table_argtype {
	_vte_table_arg_number=0,
	_vte_table_arg_string,
	_vte_table_arg_char
};

/**
 * struct _vte_table_arginfo
 */
struct _vte_table_arginfo {
	const gunichar *start;
	struct _vte_table_arginfo *next;
	guint type:2;
	guint length:30;
};

#define MAX_STACK 16
/**
 * struct _vte_table_arginfo_head
 */
struct _vte_table_arginfo_head {
	guint stack_allocated;
	struct _vte_table_arginfo *list;
	struct _vte_table_arginfo stack[MAX_STACK];
};

/**
 * _vte_table_arginfo_head_init
 */
static void
_vte_table_arginfo_head_init(struct _vte_table_arginfo_head *head)
{
	//printf ("Entering: %s\n", __FUNCTION__);
	head->list = NULL;
	head->stack_allocated = 0;
}

/**
 * _vte_table_arginfo_alloc
 */
static inline struct _vte_table_arginfo*
_vte_table_arginfo_alloc(struct _vte_table_arginfo_head *head)
{
	//printf ("Entering: %s\n", __FUNCTION__);
	struct _vte_table_arginfo *info;
	if (G_LIKELY (head->stack_allocated < G_N_ELEMENTS(head->stack))) {
		info = &head->stack[head->stack_allocated++];
	} else {
		info = g_slice_new (struct _vte_table_arginfo);
	}
	info->next = head->list;
	head->list = info;
	return info;
}

/**
 * _vte_table_arginfo_head_revert
 */
static void
_vte_table_arginfo_head_revert(struct _vte_table_arginfo_head *head, struct _vte_table_arginfo *last)
{
	//printf ("Entering: %s\n", __FUNCTION__);
	struct _vte_table_arginfo *info;
	info = head->list;
	head->list = last->next;
	if (last >= &head->stack[0] &&
			last < &head->stack[G_N_ELEMENTS(head->stack)]){
		head->stack_allocated = last - &head->stack[0];
	}
	do {
		struct _vte_table_arginfo *next = info->next;
		if (info >= &head->stack[0] &&
				info < &head->stack[G_N_ELEMENTS(head->stack)]){
			break;
		}
		g_slice_free(struct _vte_table_arginfo, info);
		if (info == last) {
			break;
		}
		info = next;
	}while (TRUE);
}

/**
 * _vte_table_arginfo_head_reverse
 */
static struct _vte_table_arginfo *
_vte_table_arginfo_head_reverse(struct _vte_table_arginfo_head *head)
{
	//printf ("Entering: %s\n", __FUNCTION__);
	struct _vte_table_arginfo *prev = NULL;
	while (head->list) {
		struct _vte_table_arginfo *next = head->list->next;

		head->list->next = prev;

		prev = head->list;
		head->list = next;
	}
	return prev;
}

/**
 * _vte_table_arginfo_head_finalize
 */
static void
_vte_table_arginfo_head_finalize(struct _vte_table_arginfo_head *head)
{
	//printf ("Entering: %s\n", __FUNCTION__);
	struct _vte_table_arginfo *info, *next;
	for (info = head->list; info != NULL; info = next) {
		next = info->next;
		if (info >= &head->stack[0] &&
				info < &head->stack[G_N_ELEMENTS(head->stack)]){
			continue;
		}
		g_slice_free(struct _vte_table_arginfo, info);
	}
}

/**
 * _vte_table_new
 * Create an empty, one-level table.
 */
struct _vte_table *
_vte_table_new(void)
{
	//printf ("Entering: %s\n", __FUNCTION__);
	struct _vte_table * ret;
	ret = g_slice_new0(struct _vte_table);
	ret->impl.klass = &_vte_matcher_table;
	return ret;
}

/**
 * _vte_table_literal_new
 */
static struct _vte_table **
_vte_table_literal_new(void)
{
	//printf ("Entering: %s\n", __FUNCTION__);
	return g_new0(struct _vte_table *, VTE_TABLE_MAX_LITERAL);
}

/**
 * _vte_table_free
 * Free a table.
 */
void
_vte_table_free(struct _vte_table *table)
{
	//printf ("Entering: %s\n", __FUNCTION__);
	unsigned int i;
	if (table->table != NULL) {
		for (i = 0; i < VTE_TABLE_MAX_LITERAL; i++) {
			if (table->table[i] != NULL) {
				_vte_table_free(table->table[i]);
			}
		}
		g_free(table->table);
	}
	if (table->table_string != NULL) {
		_vte_table_free(table->table_string);
	}
	if (table->table_number != NULL) {
		_vte_table_free(table->table_number);
	}
	if (table->table_number_list != NULL) {
		_vte_table_free(table->table_number_list);
	}
	if (table->original_length == 0) {
		g_assert(table->original == NULL);
	} else {
		g_assert(table->original != NULL);
	}
	if (table->original != NULL) {
		g_free(table->original);
	}
	g_slice_free(struct _vte_table, table);
}

/**
 * _vte_table_addi
 * Add a string to the tree with the given increment value.
 */
static void
_vte_table_addi(struct _vte_table *table,
		const unsigned char *original, gssize original_length,
		const char *pattern, gssize length,
		const char *result, GQuark quark, int inc)
{
	//printf ("Entering: %s\n", __FUNCTION__);
	int i;
	guint8 check;
	struct _vte_table *subtable;

	if (original_length == -1) {
		original_length = strlen((char *) original);
	}
	if (length == -1) {
		length = strlen(pattern);
	}

	/* If this is the terminal node, set the result. */
	if (length == 0) {
		if (table->result != NULL)
			_vte_debug_print (VTE_DEBUG_PARSE, 
					  "`%s' and `%s' are indistinguishable.\n",
					  table->result, result);

		table->resultq = g_quark_from_string(result);
		table->result = g_quark_to_string(table->resultq);
		if (table->original != NULL) {
			g_free(table->original);
		}
		table->original = g_memdup(original, original_length);
		table->original_length = original_length;
		table->increment = inc;
		return;
	}

	/* All of the interesting arguments begin with '%'. */
	if (pattern[0] == '%') {
		/* Handle an increment. */
		if (pattern[1] == 'i') {
			_vte_table_addi(table, original, original_length,
					pattern + 2, length - 2,
					result, quark, inc + 1);
			return;
		}

		/* Handle numeric parameters. */
		if ((pattern[1] == 'd') ||
		    (pattern[1] == '2') ||
		    (pattern[1] == '3')) {
			/* Create a new subtable. */
			if (table->table_number == NULL) {
				subtable = _vte_table_new();
				table->table_number = subtable;
			} else {
				subtable = table->table_number;
			}
			/* Add the rest of the string to the subtable. */
			_vte_table_addi(subtable, original, original_length,
					pattern + 2, length - 2,
					result, quark, inc);
			return;
		}

		/* Handle variable-length parameters. */
		if ((pattern[1] == 'm') ||
		    (pattern[1] == 'M')) {
			/* Build the "new" original using the initial portion
			 * of the original string and what's left after this
			 * specifier. */
			if (pattern[1] == 'm') {
				int initial;
				GByteArray *b;

				initial = original_length - length;
				/* 0 args; we use 'M' to signal that zero is
				 * not allowed.  */
				b = g_byte_array_new();
				g_byte_array_set_size(b, 0);
				g_byte_array_append(b, original, initial);
				g_byte_array_append(b, (const guint8*)pattern + 2, length - 2);
				_vte_table_addi(table, b->data, b->len,
						(const char *)b->data + initial,
						b->len - initial,
						result, quark, inc);
				g_byte_array_free(b, TRUE);
			}
			/* Create a new subtable. */
			if (table->table_number_list == NULL) {
				subtable = _vte_table_new();
				table->table_number_list = subtable;
			} else {
				subtable = table->table_number_list;
			}
			/* Add the rest of the string to the subtable. */
			_vte_table_addi(subtable, original, original_length,
					pattern + 2, length - 2,
					result, quark, inc);
			return;
		}

		/* Handle string parameters. */
		if (pattern[1] == 's') {
			/* It must have a terminator. */
			g_assert(length >= 3);
			/* Create a new subtable. */
			if (table->table_string == NULL) {
				subtable = _vte_table_new();
				table->table_string = subtable;
			} else {
				subtable = table->table_string;
			}
			/* Add the rest of the string to the subtable. */
			_vte_table_addi(subtable, original, original_length,
					pattern + 2, length - 2,
					result, quark, inc);
			return;
		}

		/* Handle an escaped '%'. */
		if (pattern[1] == '%') {
			/* Create a new subtable. */
			if (table->table == NULL) {
				table->table = _vte_table_literal_new();
				subtable = _vte_table_new();
				table->table['%'] = subtable;
			} else
			if (table->table['%'] == NULL) {
				subtable = _vte_table_new();
				table->table['%'] = subtable;
			} else {
				subtable = table->table['%'];
			}
			/* Add the rest of the string to the subtable. */
			_vte_table_addi(subtable, original, original_length,
					pattern + 2, length - 2,
					result, quark, inc);
			return;
		}

		/* Handle a parameter character. */
		if (pattern[1] == '+') {
			/* It must have an addend. */
			g_assert(length >= 3);
			/* Fill in all of the table entries above the given
			 * character value. */
			for (i = pattern[2]; i < VTE_TABLE_MAX_LITERAL; i++) {
				/* Create a new subtable. */
				if (table->table == NULL) {
					table->table = _vte_table_literal_new();
					subtable = _vte_table_new();
					table->table[i] = subtable;
				} else
				if (table->table[i] == NULL) {
					subtable = _vte_table_new();
					table->table[i] = subtable;
				} else {
					subtable = table->table[i];
				}
				/* Add the rest of the string to the subtable. */
				_vte_table_addi(subtable,
						original, original_length,
						pattern + 3, length - 3,
						result, quark, inc);
			}
			/* Also add a subtable for higher characters. */
			if (table->table == NULL) {
				table->table = _vte_table_literal_new();
				subtable = _vte_table_new();
				table->table[0] = subtable;
			} else
			if (table->table[0] == NULL) {
				subtable = _vte_table_new();
				table->table[0] = subtable;
			} else {
				subtable = table->table[0];
			}
			/* Add the rest of the string to the subtable. */
			_vte_table_addi(subtable, original, original_length,
					pattern + 3, length - 3,
					result, quark, inc);
			return;
		}
	}

	/* A literal (or an unescaped '%', which is also a literal). */
	check = (guint8) pattern[0];
	g_assert(check < VTE_TABLE_MAX_LITERAL);
	if (table->table == NULL) {
		table->table = _vte_table_literal_new();
		subtable = _vte_table_new();
		table->table[check] = subtable;
	} else
	if (table->table[check] == NULL) {
		subtable = _vte_table_new();
		table->table[check] = subtable;
	} else {
		subtable = table->table[check];
	}

	/* Add the rest of the string to the subtable. */
	_vte_table_addi(subtable, original, original_length,
			pattern + 1, length - 1,
			result, quark, inc);
}

/**
 * _vte_table_add
 * Add a string to the matching tree.
 */
void
_vte_table_add(struct _vte_table *table,
	       const char *pattern, gssize length,
	       const char *result, GQuark quark)
{
	//printf ("Entering: %s\n", __FUNCTION__);
	_vte_table_addi(table,
			(const unsigned char *) pattern, length,
			pattern, length,
			result, quark, 0);
}

/**
 * _vte_table_matchi
 * Match a string in a subtree.
 */
static const char *
_vte_table_matchi(struct _vte_table *table,
		  const gunichar *candidate, gssize length,
		  const char **res, const gunichar **consumed, GQuark *quark,
		  unsigned char **original, gssize *original_length,
		  struct _vte_table_arginfo_head *params)
{
	//printf ("Entering: %s\n", __FUNCTION__);
	int i = 0;
	struct _vte_table *subtable = NULL;
	struct _vte_table_arginfo *arginfo;

	/* Check if this is a result node. */
	if (table->result != NULL) {
		*consumed = candidate;
		*original = table->original;
		*original_length = table->original_length;
		*res = table->result;
		*quark = table->resultq;
		return table->result;
	}

	/* If we're out of data, but we still have children, return the empty
	 * string. */
	if (G_UNLIKELY (length == 0)) {
		*consumed = candidate;
		return "";
	}

	/* Check if this node has a string disposition. */
	if (table->table_string != NULL) {
		/* Iterate over all non-terminator values. */
		subtable = table->table_string;
		for (i = 0; i < length; i++) {
			if ((subtable->table != NULL) &&
			    (subtable->table[_vte_table_map_literal(candidate[i])] != NULL)) {
				break;
			}
		}
		/* Save the parameter info. */
		arginfo = _vte_table_arginfo_alloc(params);
		arginfo->type = _vte_table_arg_string;
		arginfo->start = candidate;
		arginfo->length = i;
		/* Continue. */
		return _vte_table_matchi(subtable, candidate + i, length - i,
					 res, consumed, quark,
					 original, original_length, params);
	}

	/* Check if this could be a list. */
	if ((_vte_table_is_numeric_list(candidate[0])) &&
	    (table->table_number_list != NULL)) {
		const char *local_result;

		subtable = table->table_number_list;
		/* Iterate over all numeric characters and ';'. */
		for (i = 1; i < length; i++) {
			if (!_vte_table_is_numeric_list(candidate[i])) {
				break;
			}
		}
		/* Save the parameter info. */
		arginfo = _vte_table_arginfo_alloc(params);
		arginfo->type = _vte_table_arg_number;
		arginfo->start = candidate;
		arginfo->length = i;

		/* Try and continue. */
		local_result = _vte_table_matchi(subtable,
					 candidate + i, length - i,
					 res, consumed, quark,
					 original, original_length,
					 params);
		if (local_result != NULL) {
			return local_result;
		}
		_vte_table_arginfo_head_revert (params, arginfo);

		/* try again */
	}

	/* Check if this could be a number. */
	if ((_vte_table_is_numeric(candidate[0])) &&
	    (table->table_number != NULL)) {
		subtable = table->table_number;
		/* Iterate over all numeric characters. */
		for (i = 1; i < length; i++) {
			if (!_vte_table_is_numeric(candidate[i])) {
				break;
			}
		}
		/* Save the parameter info. */
		arginfo = _vte_table_arginfo_alloc(params);
		arginfo->type = _vte_table_arg_number;
		arginfo->start = candidate;
		arginfo->length = i;
		/* Continue. */
		return _vte_table_matchi(subtable, candidate + i, length - i,
					 res, consumed, quark,
					 original, original_length, params);
	}

	/* Check for an exact match. */
	if ((table->table != NULL) &&
	    (table->table[_vte_table_map_literal(candidate[0])] != NULL)) {
		subtable = table->table[_vte_table_map_literal(candidate[0])];
		/* Save the parameter info. */
		arginfo = _vte_table_arginfo_alloc(params);
		arginfo->type = _vte_table_arg_char;
		arginfo->start = candidate;
		arginfo->length = 1;
		/* Continue. */
		return _vte_table_matchi(subtable, candidate + 1, length - 1,
					 res, consumed, quark,
					 original, original_length, params);
	}

	/* If there's nothing else to do, then we can't go on.  Keep track of
	 * where we are. */
	*consumed = candidate;
	return NULL;
}

/**
 * _vte_table_extract_numbers
 */
static void
_vte_table_extract_numbers(GValueArray **array,
			   struct _vte_table_arginfo *arginfo, long increment)
{
	//printf ("Entering: %s\n", __FUNCTION__);
	GValue value = {0,};
	gssize i;

	g_value_init(&value, G_TYPE_LONG);
	i = 0;
	do {
		long total = 0;
		for (; i < arginfo->length && arginfo->start[i] != ';'; i++) {
			gint v = g_unichar_digit_value (arginfo->start[i]);
			total *= 10;
			total += v == -1 ?  0 : v;
		}
		if (G_UNLIKELY (*array == NULL)) {
			*array = g_value_array_new(1);
		}
		g_value_set_long(&value, total);
		g_value_array_append(*array, &value);
	} while (i++ < arginfo->length);
	g_value_unset(&value);
}

/**
 * _vte_table_extract_string
 */
static void
_vte_table_extract_string(GValueArray **array,
			  struct _vte_table_arginfo *arginfo)
{
	//printf ("Entering: %s\n", __FUNCTION__);
	GValue value = {0,};
	gunichar *ptr;
	guint i;

	ptr = g_new(gunichar, arginfo->length + 1);
	for (i = 0; i < arginfo->length; i++) {
		ptr[i] = arginfo->start[i] & ~VTE_ISO2022_ENCODED_WIDTH_MASK;
	}
	ptr[i] = '\0';
	g_value_init(&value, G_TYPE_POINTER);
	g_value_set_pointer(&value, ptr);

	if (G_UNLIKELY (*array == NULL)) {
		*array = g_value_array_new(1);
	}
	g_value_array_append(*array, &value);
	g_value_unset(&value);
}

/**
 * _vte_table_extract_char
 */
static void
_vte_table_extract_char(GValueArray **array,
			struct _vte_table_arginfo *arginfo, long increment)
{
	//printf ("Entering: %s\n", __FUNCTION__);
	GValue value = {0,};

	g_value_init(&value, G_TYPE_LONG);
	g_value_set_long(&value, *(arginfo->start) - increment);

	if (G_UNLIKELY (*array == NULL)) {
		*array = g_value_array_new(1);
	}
	g_value_array_append(*array, &value);
	g_value_unset(&value);
}

/**
 * _vte_table_match
 * Check if a string matches something in the tree.
 */
const char *
_vte_table_match(struct _vte_table *table,
		 const gunichar *candidate, gssize length,
		 const char **res, const gunichar **consumed,
		 GQuark *quark, GValueArray **array)
{
	//printf ("Entering: %s\n", __FUNCTION__);
	struct _vte_table *head;
	const gunichar *dummy_consumed;
	const char *dummy_res;
	GQuark dummy_quark;
	GValueArray *dummy_array;
	const char *ret;
	unsigned char *original, *p;
	gssize original_length;
	long increment = 0;
	int i;
	struct _vte_table_arginfo_head params;
	struct _vte_table_arginfo *arginfo;

	/* Clean up extracted parameters. */
	if (G_UNLIKELY (res == NULL)) {
		res = &dummy_res;
	}
	*res = NULL;
	if (G_UNLIKELY (consumed == NULL)) {
		consumed = &dummy_consumed;
	}
	*consumed = candidate;
	if (G_UNLIKELY (quark == NULL)) {
		quark = &dummy_quark;
	}
	*quark = 0;
	if (G_UNLIKELY (array == NULL)) {
		dummy_array = NULL;
		array = &dummy_array;
	}

	/* Provide a fast path for the usual "not a sequence" cases. */
	if (G_LIKELY (length == 0 || candidate == NULL)) {
		return NULL;
	}

	/* If there's no literal path, and no generic path, and the numeric
	 * path isn't available, then it's not a sequence, either. */
	if (table->table == NULL ||
	    table->table[_vte_table_map_literal(candidate[0])] == NULL) {
		if (table->table_string == NULL) {
			if (table->table_number == NULL ||
					!_vte_table_is_numeric(candidate[0])){
				if (table->table_number_list == NULL ||
					!_vte_table_is_numeric_list(candidate[0])){
					/* No match. */
					return NULL;
				}
			}
		}
	}

	/* Check for a literal match. */
	for (i = 0, head = table; i < length && head != NULL; i++) {
		if (head->table == NULL) {
			head = NULL;
		} else {
			head = head->table[_vte_table_map_literal(candidate[i])];
		}
	}
	if (head != NULL && head->result != NULL) {
		/* Got a literal match. */
		*consumed = candidate + i;
		*res = head->result;
		*quark = head->resultq;
		return *res;
	}

	_vte_table_arginfo_head_init (&params);

	/* Check for a pattern match. */
	ret = _vte_table_matchi(table, candidate, length,
				res, consumed, quark,
				&original, &original_length,
				&params);
	*res = ret;

	/* If we got a match, extract the parameters. */
	if (ret != NULL && ret[0] != '\0' && array != &dummy_array) {
		g_assert(original != NULL);
		p = original;
		arginfo = _vte_table_arginfo_head_reverse (&params);
		do {
			/* All of the interesting arguments begin with '%'. */
			if (p[0] == '%') {
				/* Handle an increment. */
				if (p[1] == 'i') {
					increment++;
					p += 2;
					continue;
				}
				/* Handle an escaped '%'. */
				else if (p[1] == '%') {
					p++;
				}
				/* Handle numeric parameters. */
				else if ((p[1] == 'd') ||
				    (p[1] == '2') ||
				    (p[1] == '3') ||
				    (p[1] == 'm') ||
				    (p[1] == 'M')) {
					_vte_table_extract_numbers(array,
								   arginfo,
								   increment);
					p++;
				}
				/* Handle string parameters. */
				else if (p[1] == 's') {
					_vte_table_extract_string(array,
								  arginfo);
					p++;
				}
				/* Handle a parameter character. */
				else if (p[1] == '+') {
					_vte_table_extract_char(array,
								arginfo,
								p[2]);
					p += 2;
				} else {
					_vte_debug_print (VTE_DEBUG_PARSE,
							  "Invalid termcap sequence %s\n",
							  original);
				}
			} /* else Literal. */
			arginfo = arginfo->next;
		} while (++p < original + original_length && arginfo);
	}

	/* Clean up extracted parameters. */
	_vte_table_arginfo_head_finalize (&params);

	return ret;
}

/**
 * _vte_table_printi
 */
static void
_vte_table_printi(struct _vte_table *table, const char *lead, int *count)
{
	//printf ("Entering: %s\n", __FUNCTION__);
	unsigned int i;
	char *newlead = NULL;

	//printf ("Lead = %s\n", lead);

	(*count)++;

	/* Result? */
	if (table->result != NULL) {
		g_printerr("%s = `%s'(%d)\n", lead,
			table->result, table->increment);
	}

	/* Literal? */
	for (i = 1; i < VTE_TABLE_MAX_LITERAL; i++) {
		if ((table->table != NULL) && (table->table[i] != NULL)) {
			if (i < 32) {
				newlead = g_strdup_printf("%s^%c", lead,
							  i + 64);
			} else {
				newlead = g_strdup_printf("%s%c", lead, i);
			}
			_vte_table_printi(table->table[i], newlead, count);
			g_free(newlead);
		}
	}

	/* String? */
	if (table->table_string != NULL) {
		newlead = g_strdup_printf("%s{string}", lead);
		_vte_table_printi(table->table_string,
				  newlead, count);
		g_free(newlead);
	}

	/* Number(+)? */
	if (table->table_number != NULL) {
		newlead = g_strdup_printf("%s{number}", lead);
		_vte_table_printi(table->table_number,
				  newlead, count);
		g_free(newlead);
	}
}

/**
 * _vte_table_print
 * Dump out the contents of a tree.
 */
void
_vte_table_print(struct _vte_table *table)
{
	//printf ("Entering: %s\n", __FUNCTION__);
	int count = 0;
	_vte_table_printi(table, "", &count);
	g_printerr("%d nodes = %ld bytes.\n",
		count, (long) count * sizeof(struct _vte_table));
}

/**
 * struct _vte_matcher_class _vte_matcher_table
 */
const struct _vte_matcher_class _vte_matcher_table = {
	(_vte_matcher_create_func)_vte_table_new,
	(_vte_matcher_add_func)_vte_table_add,
	(_vte_matcher_print_func)_vte_table_print,
	(_vte_matcher_match_func)_vte_table_match,
	(_vte_matcher_destroy_func)_vte_table_free
};
