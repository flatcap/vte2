#ifndef __RSEQ_VTE_H__
#define __RSEQ_VTE_H__

void
_vte_terminal_handle_sequence (RarTerminal *terminal,
			      const char *match_s,
			      GQuark match G_GNUC_UNUSED,
			      GValueArray *params);

gboolean

_vte_terminal_insert_char(RarTerminal *terminal, gunichar c, gboolean insert, gboolean invalidate_now);

void _vte_terminal_queue_contents_changed(RarTerminal *terminal);

#endif // __RSEQ_VTE_H__

