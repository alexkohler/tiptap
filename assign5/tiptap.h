#ifndef TIPTAP_H
#define TIPTAP_H

#include <stdint.h>

#define TIPTAP_VERSION "1.4"

#define TIPTAP_IP "127.0.0.1"
#define TIPTAP_PORT 9988

// Flags used in TipTap instructions
#define TTFLAG_EXTRUDE 1
#define TTFLAG_PROBE 2
#define TTFLAG_RESERVED_1 4
#define TTFLAG_RESERVED_2 8

// Opcodes used in TipTap instructions
enum tiptap_op {
	TT_INVALID = 0,
	TT_POWERON = 1,
	TT_MOVEX = 2,
	TT_MOVEY = 3,
	TT_MOVEZ = 4,
	TT_POWEROFF = 31,
};

// Public interface of the TipTap 3-D printer
int tapctl(uint32_t insn, void *buf);

void tt_error(int num);
int tt_log_layer(uint16_t z);

#endif
