
#if 0
/**
 * vte_capability_init:
 *
 * Initializes the vte_terminal_capability_strings and
 * vte_xterm_capability_strings structures used by the terminal.  Can
 * be called multiple times without ill effect.
 *
 * Returns: void
 */

void
_vte_capability_init(void)
{
	unsigned int i;
	for (i = 0; _vte_terminal_capability_strings[i].capability[0]; i++) {
		const char *tmp;
		GQuark quark;
		tmp = _vte_terminal_capability_strings[i].capability;
		quark = g_quark_from_static_string(tmp);
		_vte_terminal_capability_strings[i].quark = quark;
	}
	for (i = 0; i < G_N_ELEMENTS(_vte_xterm_capability_strings); i++) {
		const char *tmp;
		GQuark quark;
		tmp = _vte_xterm_capability_strings[i].value;
		if (tmp != NULL) {
			quark = g_quark_from_static_string(tmp);
			_vte_xterm_capability_strings[i].quark = quark;
		}
	}
}
#endif
