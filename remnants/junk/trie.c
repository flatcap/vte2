#ifdef TRIE_MAIN
static void
dump_array(GValueArray *array)
{
	unsigned int i;
	if (array != NULL) {
		printf("args = {");
		for (i = 0; i < array->n_values; i++) {
			GValue *value;
			value = g_value_array_get_nth(array, i);
			if (i > 0) {
				printf(", ");
			}
			if (G_VALUE_HOLDS_LONG(value)) {
				printf("%ld", g_value_get_long(value));
			}
			if (G_VALUE_HOLDS_STRING(value)) {
				printf("`%s'", g_value_get_string(value));
			}
			if (G_VALUE_HOLDS_POINTER(value)) {
				printf("`%ls'",
				       (wchar_t*) g_value_get_pointer(value));
			}
		}
		printf("}\n");
	}
}

static void
convert_mbstowcs(const char *i, gsize ilen,
		 gunichar *o, gsize *olen, gsize max_olen)
{
	VteConv conv;
	gsize outlen;
	conv = _vte_conv_open(VTE_CONV_GUNICHAR_TYPE, "UTF-8");
	g_assert(conv != VTE_INVALID_CONV);

	memset(o, 0, max_olen);
	outlen = max_olen;
	_vte_conv_cu(conv, (char**)&i, &ilen, &o, &outlen);
	_vte_conv_close(conv);

	*olen = (max_olen - outlen) / sizeof(gunichar);
}

int
main(int argc, char **argv)
{
	struct _vte_trie *trie;
	GValueArray *array = NULL;
	GQuark quark;
	gunichar buf[LINE_MAX];
	const gunichar *consumed;
	gsize buflen;

	_vte_debug_init();

	g_type_init();
	trie = _vte_trie_new();

	_vte_trie_add(trie, "abcdef", 6, "abcdef",
		      g_quark_from_static_string("abcdef"));
	_vte_trie_add(trie, "abcde", 5, "abcde",
		      g_quark_from_static_string("abcde"));
	_vte_trie_add(trie, "abcdeg", 6, "abcdeg",
		      g_quark_from_static_string("abcdeg"));
	_vte_trie_add(trie, "abc%+Aeg", 8, "abc%+Aeg",
		      g_quark_from_static_string("abc%+Aeg"));
	_vte_trie_add(trie, "abc%deg", 7, "abc%deg",
		      g_quark_from_static_string("abc%deg"));
	_vte_trie_add(trie, "abc%%eg", 7, "abc%%eg",
		      g_quark_from_static_string("abc%%eg"));
	_vte_trie_add(trie, "abc%%%i%deg", 11, "abc%%%i%deg",
		      g_quark_from_static_string("abc%%%i%deg"));
	_vte_trie_add(trie, "<esc>[%i%d;%dH", 14, "vtmatch",
		      g_quark_from_static_string("vtmatch"));
	_vte_trie_add(trie, "<esc>[%i%mL", 11, "multimatch",
		      g_quark_from_static_string("multimatch"));
	_vte_trie_add(trie, "<esc>[%mL<esc>[%mL", 18, "greedy",
		      g_quark_from_static_string("greedy"));
	_vte_trie_add(trie, "<esc>]2;%sh", 11, "decset-title",
		      g_quark_from_static_string("decset-title"));

	printf("Wide encoding is `%s'.\n", VTE_CONV_GUNICHAR_TYPE);

	_vte_trie_print(trie);
	printf("\n");

	quark = 0;
	convert_mbstowcs("abc", 3, buf, &buflen, sizeof(buf));
	printf("`%s' = `%s'\n", "abc",
	       _vte_trie_match(trie, buf, buflen,
			       NULL, &consumed, &quark, &array));
	printf("=> `%s' (%d)\n", g_quark_to_string(quark), (int)(consumed - buf));
	if (array != NULL) {
		dump_array(array);
		_vte_matcher_free_params_array(NULL, array);
		array = NULL;
	}

	quark = 0;
	convert_mbstowcs("abcdef", 6, buf, &buflen, sizeof(buf));
	printf("`%s' = `%s'\n", "abcdef",
	       _vte_trie_match(trie, buf, buflen,
			       NULL, &consumed, &quark, &array));
	printf("=> `%s' (%d)\n", g_quark_to_string(quark), (int)(consumed - buf));
	if (array != NULL) {
		dump_array(array);
		_vte_matcher_free_params_array(NULL, array);
		array = NULL;
	}

	quark = 0;
	convert_mbstowcs("abcde", 5, buf, &buflen, sizeof(buf));
	printf("`%s' = `%s'\n", "abcde",
	       _vte_trie_match(trie, buf, buflen,
			       NULL, &consumed, &quark, &array));
	printf("=> `%s' (%d)\n", g_quark_to_string(quark), (int)(consumed - buf));
	if (array != NULL) {
		dump_array(array);
		_vte_matcher_free_params_array(NULL, array);
		array = NULL;
	}

	quark = 0;
	convert_mbstowcs("abcdeg", 6, buf, &buflen, sizeof(buf));
	printf("`%s' = `%s'\n", "abcdeg",
	       _vte_trie_match(trie, buf, buflen,
			       NULL, &consumed, &quark, &array));
	printf("=> `%s' (%d)\n", g_quark_to_string(quark), (int)(consumed - buf));
	if (array != NULL) {
		dump_array(array);
		_vte_matcher_free_params_array(NULL, array);
		array = NULL;
	}

	quark = 0;
	convert_mbstowcs("abc%deg", 7, buf, &buflen, sizeof(buf));
	printf("`%s' = `%s'\n", "abc%deg",
	       _vte_trie_match(trie, buf, buflen,
			       NULL, &consumed, &quark, &array));
	printf("=> `%s' (%d)\n", g_quark_to_string(quark), (int)(consumed - buf));
	if (array != NULL) {
		dump_array(array);
		_vte_matcher_free_params_array(NULL, array);
		array = NULL;
	}

	quark = 0;
	convert_mbstowcs("abc10eg", 7, buf, &buflen, sizeof(buf));
	printf("`%s' = `%s'\n", "abc10eg",
	       _vte_trie_match(trie, buf, buflen,
			       NULL, &consumed, &quark, &array));
	printf("=> `%s' (%d)\n", g_quark_to_string(quark), (int)(consumed - buf));
	if (array != NULL) {
		dump_array(array);
		_vte_matcher_free_params_array(NULL, array);
		array = NULL;
	}

	quark = 0;
	convert_mbstowcs("abc%eg", 6, buf, &buflen, sizeof(buf));
	printf("`%s' = `%s'\n", "abc%eg",
	       _vte_trie_match(trie, buf, buflen,
			       NULL, &consumed, &quark, &array));
	printf("=> `%s' (%d)\n", g_quark_to_string(quark), (int)(consumed - buf));
	if (array != NULL) {
		dump_array(array);
		_vte_matcher_free_params_array(NULL, array);
		array = NULL;
	}

	quark = 0;
	convert_mbstowcs("abc%10eg", 8, buf, &buflen, sizeof(buf));
	printf("`%s' = `%s'\n", "abc%10eg",
	       _vte_trie_match(trie, buf, buflen,
			       NULL, &consumed, &quark, &array));
	printf("=> `%s' (%d)\n", g_quark_to_string(quark), (int)(consumed - buf));
	if (array != NULL) {
		dump_array(array);
		_vte_matcher_free_params_array(NULL, array);
		array = NULL;
	}

	quark = 0;
	convert_mbstowcs("abcBeg", 6, buf, &buflen, sizeof(buf));
	printf("`%s' = `%s'\n", "abcBeg",
	       _vte_trie_match(trie, buf, buflen,
			       NULL, &consumed, &quark, &array));
	printf("=> `%s' (%d)\n", g_quark_to_string(quark), (int)(consumed - buf));
	if (array != NULL) {
		dump_array(array);
		_vte_matcher_free_params_array(NULL, array);
		array = NULL;
	}

	quark = 0;
	convert_mbstowcs("<esc>[25;26H", 12, buf, &buflen, sizeof(buf));
	printf("`%s' = `%s'\n", "<esc>[25;26H",
	       _vte_trie_match(trie, buf, buflen,
			       NULL, &consumed, &quark, &array));
	printf("=> `%s' (%d)\n", g_quark_to_string(quark), (int)(consumed - buf));
	if (array != NULL) {
		dump_array(array);
		_vte_matcher_free_params_array(NULL, array);
		array = NULL;
	}

	quark = 0;
	convert_mbstowcs("<esc>[25;2", 10, buf, &buflen, sizeof(buf));
	printf("`%s' = `%s'\n", "<esc>[25;2",
	       _vte_trie_match(trie, buf, buflen,
			       NULL, &consumed, &quark, &array));
	printf("=> `%s' (%d)\n", g_quark_to_string(quark), (int)(consumed - buf));
	if (array != NULL) {
		dump_array(array);
		_vte_matcher_free_params_array(NULL, array);
	}

	quark = 0;
	convert_mbstowcs("<esc>[25L", 9, buf, &buflen, sizeof(buf));
	printf("`%s' = `%s'\n", "<esc>[25L",
	       _vte_trie_match(trie, buf, buflen,
			       NULL, &consumed, &quark, &array));
	printf("=> `%s' (%d)\n", g_quark_to_string(quark), (int)(consumed - buf));
	if (array != NULL) {
		dump_array(array);
		_vte_matcher_free_params_array(NULL, array);
	}

	quark = 0;
	convert_mbstowcs("<esc>[25L<esc>[24L", 18, buf, &buflen, sizeof(buf));
	printf("`%s' = `%s'\n", "<esc>[25L<esc>[24L",
	       _vte_trie_match(trie, buf, buflen,
			       NULL, &consumed, &quark, &array));
	printf("=> `%s' (%d)\n", g_quark_to_string(quark), (int)(consumed - buf));
	if (array != NULL) {
		dump_array(array);
		_vte_matcher_free_params_array(NULL, array);
	}

	quark = 0;
	convert_mbstowcs("<esc>[25;26L", 12, buf, &buflen, sizeof(buf));
	printf("`%s' = `%s'\n", "<esc>[25;26L",
	       _vte_trie_match(trie, buf, buflen,
			       NULL, &consumed, &quark, &array));
	printf("=> `%s' (%d)\n", g_quark_to_string(quark), (int)(consumed - buf));
	if (array != NULL) {
		dump_array(array);
		_vte_matcher_free_params_array(NULL, array);
	}

	quark = 0;
	convert_mbstowcs("<esc>]2;WoofWoofh", 17, buf, &buflen, sizeof(buf));
	printf("`%s' = `%s'\n", "<esc>]2;WoofWoofh",
	       _vte_trie_match(trie, buf, buflen,
			       NULL, &consumed, &quark, &array));
	printf("=> `%s' (%d)\n", g_quark_to_string(quark), (int)(consumed - buf));
	if (array != NULL) {
		dump_array(array);
		_vte_matcher_free_params_array(NULL, array);
		array = NULL;
	}

	quark = 0;
	convert_mbstowcs("<esc>]2;WoofWoofh<esc>]2;WoofWoofh", 34,
			 buf, &buflen, sizeof(buf));
	printf("`%s' = `%s'\n", "<esc>]2;WoofWoofh<esc>]2;WoofWoofh",
	       _vte_trie_match(trie, buf, buflen,
			       NULL, &consumed, &quark, &array));
	printf("=> `%s' (%d)\n", g_quark_to_string(quark), (int)(consumed - buf));
	if (array != NULL) {
		dump_array(array);
		_vte_matcher_free_params_array(NULL, array);
		array = NULL;
	}

	quark = 0;
	convert_mbstowcs("<esc>]2;WoofWoofhfoo", 20, buf, &buflen, sizeof(buf));
	printf("`%s' = `%s'\n", "<esc>]2;WoofWoofhfoo",
	       _vte_trie_match(trie, buf, buflen,
			       NULL, &consumed, &quark, &array));
	printf("=> `%s' (%d)\n", g_quark_to_string(quark), (int)(consumed - buf));
	if (array != NULL) {
		dump_array(array);
		_vte_matcher_free_params_array(NULL, array);
		array = NULL;
	}

	_vte_trie_free(trie);

	return 0;
}
#endif
