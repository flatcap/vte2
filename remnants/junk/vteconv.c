#ifdef VTECONV_MAIN
static void
clear(gunichar wide[5], gchar narrow[5])
{
	wide[0] = 'T';
	wide[1] = 'E';
	wide[2] = 'S';
	wide[3] = 'T';
	wide[4] = '\0';
	strcpy(narrow, "test");
}

static int
mixed_strcmp(gunichar *wide, gchar *narrow)
{
	while (*wide && *narrow) {
		if (*wide != *narrow) {
			return -1;
		}
		wide++;
		narrow++;
	}
	return 0;
}

int
main(int argc, char **argv)
{
	gunichar wide_test[5];
	gchar narrow_test[5], buf[10];
	VteConv conv;
	gchar *inbuf, *outbuf;
	gsize inbytes, outbytes;
	char mbyte_test[] = {0xe2, 0x94, 0x80};
	char mbyte_test_break[] = {0xe2, 0xe2, 0xe2};
	int i;

	i = _vte_conv_utf8_strlen("\0\0\0\0", 4);
	g_assert(i == 4);
	i = _vte_conv_utf8_strlen("\0A\0\0", 4);
	g_assert(i == 4);
	i = _vte_conv_utf8_strlen("\0A\0B", 4);
	g_assert(i == 4);
	i = _vte_conv_utf8_strlen("A\0B\0", 4);
	g_assert(i == 4);
	i = _vte_conv_utf8_strlen("ABCDE", 4);
	g_assert(i == 4);

	/* Test g_iconv, no gunichar stuff. */
	clear(wide_test, narrow_test);
	memset(buf, 0, sizeof(buf));
	inbuf = narrow_test;
	inbytes = strlen(narrow_test);
	outbuf = buf;
	outbytes = sizeof(buf);
	conv = _vte_conv_open("UTF-8", "ISO-8859-1");
	i = _vte_conv(conv, &inbuf, &inbytes, &outbuf, &outbytes);
	g_assert(inbytes == 0);
	if (strcmp(narrow_test, buf) != 0) {
		g_error("Conversion 1 failed.\n");
	}
	_vte_conv_close(conv);

	/* Test g_iconv, no gunichar stuff. */
	clear(wide_test, narrow_test);
	memset(buf, 0, sizeof(buf));
	inbuf = narrow_test;
	inbytes = strlen(narrow_test);
	outbuf = buf;
	outbytes = sizeof(buf);
	conv = _vte_conv_open("ISO-8859-1", "UTF-8");
	i = _vte_conv(conv, &inbuf, &inbytes, &outbuf, &outbytes);
	g_assert(inbytes == 0);
	if (strcmp(narrow_test, buf) != 0) {
		g_error("Conversion 2 failed.\n");
	}
	_vte_conv_close(conv);

	/* Test g_iconv + gunichar stuff. */
	clear(wide_test, narrow_test);
	memset(buf, 0, sizeof(buf));
	inbuf = narrow_test;
	inbytes = strlen(narrow_test);
	outbuf = (gchar*) wide_test;
	outbytes = sizeof(wide_test);
	conv = _vte_conv_open(VTE_CONV_GUNICHAR_TYPE, "ISO-8859-1");
	i = _vte_conv(conv, &inbuf, &inbytes, &outbuf, &outbytes);
	g_assert(inbytes == 0);
	if (mixed_strcmp(wide_test, narrow_test) != 0) {
		g_error("Conversion 3 failed.\n");
	}
	_vte_conv_close(conv);

	/* Test g_iconv + gunichar stuff. */
	clear(wide_test, narrow_test);
	memset(buf, 0, sizeof(buf));
	inbuf = (gchar*) wide_test;
	inbytes = 4 * sizeof(gunichar);
	outbuf = buf;
	outbytes = sizeof(buf);
	conv = _vte_conv_open("ISO-8859-1", VTE_CONV_GUNICHAR_TYPE);
	i = _vte_conv(conv, &inbuf, &inbytes, &outbuf, &outbytes);
	g_assert(inbytes == 0);
	if (mixed_strcmp(wide_test, buf) != 0) {
		g_error("Conversion 4 failed.\n");
	}
	_vte_conv_close(conv);

	/* Test UTF-8 to UTF-8 "conversion". */
	clear(wide_test, narrow_test);
	memset(buf, 0, sizeof(buf));
	inbuf = (gchar*) narrow_test;
	inbytes = strlen(narrow_test);
	outbuf = buf;
	outbytes = sizeof(buf);
	conv = _vte_conv_open("UTF-8", "UTF-8");
	i = _vte_conv(conv, &inbuf, &inbytes, &outbuf, &outbytes);
	g_assert(inbytes == 0);
	if (strcmp(narrow_test, buf) != 0) {
		g_error("Conversion 5 failed.\n");
	}
	_vte_conv_close(conv);

	/* Test zero-byte pass-through. */
	clear(wide_test, narrow_test);
	memset(wide_test, 0, sizeof(wide_test));
	inbuf = (gchar*) wide_test;
	inbytes = 3 * sizeof(gunichar);
	outbuf = narrow_test;
	outbytes = sizeof(narrow_test);
	conv = _vte_conv_open("UTF-8", VTE_CONV_GUNICHAR_TYPE);
	i = _vte_conv(conv, &inbuf, &inbytes, &outbuf, &outbytes);
	g_assert(inbytes == 0);
	if ((narrow_test[0] != 0) ||
	    (narrow_test[1] != 0) ||
	    (narrow_test[2] != 0)) {
		g_error("Conversion 6 failed.\n");
	}
	_vte_conv_close(conv);

	/* Test zero-byte pass-through. */
	clear(wide_test, narrow_test);
	memset(wide_test, 'A', sizeof(wide_test));
	memset(narrow_test, 0, sizeof(narrow_test));
	inbuf = narrow_test;
	inbytes = 3;
	outbuf = (char*)wide_test;
	outbytes = sizeof(wide_test);
	conv = _vte_conv_open(VTE_CONV_GUNICHAR_TYPE, "UTF-8");
	i = _vte_conv(conv, &inbuf, &inbytes, &outbuf, &outbytes);
	g_assert(inbytes == 0);
	if ((wide_test[0] != 0) ||
	    (wide_test[1] != 0) ||
	    (wide_test[2] != 0)) {
		g_error("Conversion 7 failed.\n");
	}
	_vte_conv_close(conv);

	/* Test zero-byte pass-through. */
	clear(wide_test, narrow_test);
	memset(wide_test, 'A', sizeof(wide_test));
	memset(narrow_test, 0, sizeof(narrow_test));
	inbuf = narrow_test;
	inbytes = 3;
	outbuf = (char*)wide_test;
	outbytes = sizeof(wide_test);
	conv = _vte_conv_open(VTE_CONV_GUNICHAR_TYPE, "ISO-8859-1");
	i = _vte_conv(conv, &inbuf, &inbytes, &outbuf, &outbytes);
	g_assert(inbytes == 0);
	if ((wide_test[0] != 0) ||
	    (wide_test[1] != 0) ||
	    (wide_test[2] != 0)) {
		g_error("Conversion 8 failed.\n");
	}
	_vte_conv_close(conv);

	/* Test UTF-8 to UTF-8 error reporting, valid multibyte. */
	for (i = 0; i < sizeof(mbyte_test); i++) {
		int ret;
		inbuf = mbyte_test;
		inbytes = i + 1;
		outbuf = buf;
		outbytes = sizeof(buf);
		conv = _vte_conv_open("UTF-8", "UTF-8");
		ret = _vte_conv(conv, &inbuf, &inbytes, &outbuf, &outbytes);
		switch (i) {
		case 0:
			g_assert((ret == -1) && (errno == EINVAL));
			break;
		case 1:
			g_assert((ret == -1) && (errno == EINVAL));
			break;
		case 2:
			g_assert(ret != -1);
			break;
		default:
			g_assert_not_reached();
			break;
		}
		_vte_conv_close(conv);
	}

	/* Test UTF-8 to UTF-8 error reporting, invalid multibyte. */
	for (i = 0; i < sizeof(mbyte_test_break); i++) {
		int ret;
		inbuf = mbyte_test_break;
		inbytes = i + 1;
		outbuf = buf;
		outbytes = sizeof(buf);
		conv = _vte_conv_open("UTF-8", "UTF-8");
		ret = _vte_conv(conv, &inbuf, &inbytes, &outbuf, &outbytes);
		_vte_conv_close(conv);
		switch (i) {
		case 0:
			g_assert((ret == -1) && (errno == EINVAL));
			break;
		case 1:
			g_assert((ret == -1) && (errno == EINVAL));
			break;
		case 2:
			g_assert((ret == -1) && (errno == EILSEQ));
			break;
		default:
			g_assert_not_reached();
			break;
		}
	}

	return 0;
}
#endif
