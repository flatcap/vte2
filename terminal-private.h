#ifndef __TERMINAL_PRIVATE_H__
#define __TERMINAL_PRIVATE_H__

#include "vtepty.h"
#include "reaper.h"
#include "buffer.h"

#ifdef RARXXX // Copied from vte-private.h
#define VTE_INPUT_CHUNK_SIZE		0x2000
#endif

/**
 * RarTerminalPrivate
 */
struct _RarTerminalPrivate {
	/* Properties */

	/* PTY handling data. */
	VtePty *pty;
	GIOChannel *pty_channel;	/* master channel */
	guint pty_input_source;
	guint pty_output_source;
	gboolean pty_input_active;
	GPid pty_pid;			/* pid of child using pty slave */
	VteReaper *pty_reaper;
        int child_exit_status;

	struct _vte_incoming_chunk{
		struct _vte_incoming_chunk *next;
		guint len;
		guchar data[VTE_INPUT_CHUNK_SIZE
			- 2 * sizeof(void *)];
	} *incoming;			/* pending bytestream */

	GArray *pending;		/* pending characters */
	glong input_bytes;
	struct _vte_iso2022_state *iso2022;
	struct _vte_matcher *matcher;	/* control sequence matcher */
	const char *encoding;		/* the pty's encoding */
	GList *active;                  /* is the terminal processing data */
	glong max_input_bytes;
	GSList *update_regions;

	/* Output data queue. */
	VteBuffer *outgoing;	/* pending input characters */

	RarScreen *screen;
};


#endif // __TERMINAL_PRIVATE_H__

