#ifdef ISO2022_MAIN
#include <stdio.h>
int
main(int argc, char **argv)
{
	VteBuffer *buffer;
	struct _vte_iso2022_state *state;
	GString *string;
	GArray *gunichars;
	struct {
		const char *s;
		gboolean process;
	} strings[] = {
		{"abcd\033$(Cefgh\ri\nj\033)0k\017lmn\033Nopqrst\033%G", TRUE},
		{"ABCD\033$(C\033)", TRUE},
		{"0", TRUE},
		{"\014\033[33;41m", TRUE},
		{"\015", TRUE},
		{"\014{|}\015\r\n", TRUE},
		{"\033(B\033)0\033*B\033+B", TRUE},
		{"\033$B$+$J4A;z\033(J~", TRUE},
		{"\033(B\033)0\033*B\033+B", TRUE},
		{"\033$)C\0161hD!\017", TRUE},
		{"\033$*C\033N1hD!", TRUE},
		{"\033$(G\043\071", TRUE},
		{"\033(B\033)0\033*B\033+B", TRUE},
		{"\r\n", TRUE},
	};
	guint i;
	FILE *fp;
	guchar b;

	state = _vte_iso2022_state_new(NULL, NULL, NULL);
	buffer = _vte_buffer_new();
	gunichars = g_array_new(FALSE, FALSE, sizeof(gunichar));
	if (argc > 1) {
		string = g_string_new(NULL);
		for (i = 1; i < (guint) argc; i++) {
			if (strcmp(argv[i], "-") == 0) {
				fp = stdin;
			} else {
				fp = fopen(argv[i], "r");
			}
			while (fread(&b, sizeof(guchar), 1, fp) == sizeof(b)) {
				g_string_append_c(string, b);
			}
			if (fp != stdin) {
				fclose(fp);
			}
		}
		_vte_buffer_append(buffer, string->str, string->len);
		_vte_iso2022_process(state, buffer->data, _vte_buffer_length (buffer), gunichars);
		g_string_free(string, TRUE);
	} else {
		for (i = 0; i < G_N_ELEMENTS(strings); i++) {
			string = g_string_new(strings[i].s);
			_vte_buffer_append(buffer, string->str, string->len);
			g_string_free(string, TRUE);
			if (strings[i].process) {
				_vte_iso2022_process(state, buffer->data, _vte_buffer_length (buffer), gunichars);
			}
		}
	}
	_vte_buffer_free(buffer);
	_vte_iso2022_state_free(state);

	string = g_string_new(NULL);
	for (i = 0; i < gunichars->len; i++) {
		g_string_append_unichar(string,
					g_array_index(gunichars, gunichar, i));
	}
	(void) write(STDOUT_FILENO, string->str, string->len);
	g_string_free(string, TRUE);

	return 0;
}
#endif
