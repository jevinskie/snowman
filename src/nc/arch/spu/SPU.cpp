/* The file is part of Snowman decompiler. */
/* See doc/licenses.asciidoc for the licensing information. */

#include <stdio.h>

#include "SPU.h"

namespace nc {
namespace arch {
namespace spu {

#define s32(n) \
	((int)(((((int)n) & 0xff000000) >> 24) | \
	((((int)n) & 0x00ff0000) >> 8 ) | \
	((((int)n) & 0x0000ff00) << 8 ) | \
	((((int)n) & 0x000000ff) << 24)))

static char buffer[1024];

extern const struct spu_opcode spu_opcodes[];
extern const int spu_num_opcodes;

static const struct spu_opcode *spu_disassemble_table[(1<<11)];

static void
init_spu_disassemble (void)
{
  int i;

  /* If two instructions have the same opcode then we prefer the first
   * one.  In most cases it is just an alternate mnemonic. */
  for (i = 0; i < spu_num_opcodes; i++)
    {
      int o = spu_opcodes[i].opcode;
      if (spu_disassemble_table[o] == 0)
	spu_disassemble_table[o] = &spu_opcodes[i];
    }
}

/* Determine the instruction from the 10 least significant bits. */
static const struct spu_opcode *
get_index_for_opcode (unsigned int insn)
{
  const struct spu_opcode *index;
  unsigned int opcode = insn >> (32-11);

  /* Init the table.  This assumes that element 0/opcode 0 (currently
   * NOP) is always used */
  if (spu_disassemble_table[0] == 0)
    init_spu_disassemble ();

  if ((index = spu_disassemble_table[opcode & 0x780]) != 0
      && index->insn_type == RRR)
    return index;

  if ((index = spu_disassemble_table[opcode & 0x7f0]) != 0
      && (index->insn_type == RI18 || index->insn_type == LBT))
    return index;

  if ((index = spu_disassemble_table[opcode & 0x7f8]) != 0
      && index->insn_type == RI10)
    return index;

  if ((index = spu_disassemble_table[opcode & 0x7fc]) != 0
      && (index->insn_type == RI16))
    return index;

  if ((index = spu_disassemble_table[opcode & 0x7fe]) != 0
      && (index->insn_type == RI8))
    return index;

  if ((index = spu_disassemble_table[opcode & 0x7ff]) != 0)
    return index;

  return 0;
}

/* Decode a Spu instruction */

void
spu_decode_insn (unsigned int memaddr, unsigned int opcode, spu_instruction *insn)
{
    int i, o;
    int value;
    const struct spu_opcode *index;
    
    insn->mnemonic = spu_disassemble_insn(memaddr, opcode);

    opcode = s32(opcode);
    index = get_index_for_opcode(opcode);
    insn->opcode = index->insn;
    insn->op_count = 0;
    insn->addr = memaddr;

    for (i = 0; i < 5; i++)
    {
        insn->operands[i].type = OP_INVALID;
        insn->operands[i].mem.base = 0;
        insn->operands[i].mem.disp = 0;
    }

    for (i = 1, o = 0; i <= index->arg[0]; i++)
    {
        int arg = index->arg[i];
        switch (arg)
        {
        case A_T:
            insn->operands[o].reg = DECODE_INSN_RT (opcode);
            insn->operands[o].type = OP_REG;
            break;
        case A_A:
            if (index->arg[i-1] == A_P)
            {
                value = insn->operands[--o].imm;
                insn->operands[o].mem.base = DECODE_INSN_RA (opcode);
                insn->operands[o].mem.disp = value;
                insn->operands[o].type = OP_MEM;
            }
            else 
            {
                insn->operands[o].reg = DECODE_INSN_RA (opcode);
                insn->operands[o].type = OP_REG;
            }
            break;
        case A_S:
        case A_H:
            insn->operands[o].reg = DECODE_INSN_RA (opcode);
            insn->operands[o].type = OP_REG;
            break;
        case A_B:
            insn->operands[o].reg = DECODE_INSN_RB (opcode);
            insn->operands[o].type = OP_REG;
            break;
        case A_C:
            insn->operands[o].reg = DECODE_INSN_RC (opcode);
            insn->operands[o].type = OP_REG;
            break;
        case A_P:
            continue;
        case A_U7A:
            insn->operands[o].imm = (173 - DECODE_INSN_U8 (opcode));
            insn->operands[i].type = OP_IMM;
            break;
        case A_U7B:
            insn->operands[o].imm = (155 - DECODE_INSN_U8 (opcode));
            insn->operands[o].type = OP_IMM;
            break;
        case A_S3:
        case A_S6:
        case A_S7:
        case A_S7N:
        case A_U3:
        case A_U5:
        case A_U6:
        case A_U7:
            insn->operands[o].imm = DECODE_INSN_I7 (opcode);
            insn->operands[o].type = OP_IMM;
            break;
        case A_S11:
            insn->operands[o].imm = (memaddr + DECODE_INSN_I9a (opcode) * 4);
            insn->operands[o].type = OP_IMM;
            break;
        case A_S11I:
            insn->operands[o].imm = (memaddr + DECODE_INSN_I9b (opcode) * 4);
            insn->operands[o].type = OP_IMM;
            break;
        case A_S10:
        case A_S10B:
            insn->operands[o].imm = DECODE_INSN_I10 (opcode);
            insn->operands[o].type = OP_IMM;
            break;
        case A_S14:
            insn->operands[o].imm = (DECODE_INSN_I10 (opcode) * 16);
            insn->operands[o].type = OP_IMM;
            break;
        case A_S16:
        case A_X16:
            insn->operands[o].imm = DECODE_INSN_I16 (opcode);
            insn->operands[o].type = OP_IMM;
            break;
        case A_R18:
            value = (DECODE_INSN_I16 (opcode) * 4);
            if (value == 0)
                insn->operands[o].imm = value;
            else
                insn->operands[o].imm = (memaddr + value) & 0x3ffff;
            insn->operands[o].type = OP_IMM;
            break;
        case A_S18:
            insn->operands[o].imm = (DECODE_INSN_U16 (opcode) * 4);
            insn->operands[o].type = OP_IMM;
            break;
        case A_U18:
            insn->operands[o].imm = DECODE_INSN_U18 (opcode);
            insn->operands[o].type = OP_IMM;
            break;
        case A_U14:
            insn->operands[o].imm = DECODE_INSN_U14 (opcode);
            insn->operands[o].type = OP_IMM;
            break;
        }
        ++o;
    }

    insn->op_count = o;
}

/* Print a Spu instruction.  */

char *
spu_disassemble_insn (unsigned int memaddr, unsigned int insn)
{
  int value;
  int hex_value;
  int status;
  const struct spu_opcode *index;
  enum spu_insns tag;

  insn = s32(insn);
  index = get_index_for_opcode (insn);

  if (index == 0)
    {
      sprintf(buffer, "");
      return NULL;
    }
  else
    {
      int i;
      int paren = 0;
      int len = 0;
      tag = (enum spu_insns)(index - spu_opcodes);
      len += sprintf(buffer, "%s", index->mnemonic);
      if (tag == M_BI || tag == M_BISL || tag == M_IRET || tag == M_BISLED
	  || tag == M_BIHNZ || tag == M_BIHZ || tag == M_BINZ || tag == M_BIZ
          || tag == M_SYNC || tag == M_HBR)
	{
	  int fb = (insn >> (32-18)) & 0x7f;
	  if (fb & 0x40)
	    len += sprintf(&buffer[len], tag == M_SYNC ? "c" : "p");
	  if (fb & 0x20)
	    len += sprintf(&buffer[len], "d");
	  if (fb & 0x10)
	    len += sprintf(&buffer[len], "e");
	}
      if (index->arg[0] != 0)
	len += sprintf(&buffer[len], "\t");
      hex_value = 0;
      for (i = 1;  i <= index->arg[0]; i++)
	{
	  int arg = index->arg[i];
	  if (arg != A_P && !paren && i > 1)
	    len += sprintf(&buffer[len], ",");

	  switch (arg)
	    {
	    case A_T:
	      len += sprintf (&buffer[len], "$%d",
				     DECODE_INSN_RT (insn));
	      break;
	    case A_A:
	      len += sprintf (&buffer[len], "$%d",
				     DECODE_INSN_RA (insn));
	      break;
	    case A_B:
	      len += sprintf (&buffer[len], "$%d",
				     DECODE_INSN_RB (insn));
	      break;
	    case A_C:
	      len += sprintf (&buffer[len], "$%d",
				     DECODE_INSN_RC (insn));
	      break;
	    case A_S:
	      len += sprintf (&buffer[len], "$sp%d",
				     DECODE_INSN_RA (insn));
	      break;
	    case A_H:
	      len += sprintf (&buffer[len], "$ch%d",
				     DECODE_INSN_RA (insn));
	      break;
	    case A_P:
	      paren++;
	      len += sprintf (&buffer[len], "(");
	      break;
	    case A_U7A:
	      len += sprintf (&buffer[len], "%d",
				     173 - DECODE_INSN_U8 (insn));
	      break;
	    case A_U7B:
	      len += sprintf (&buffer[len], "%d",
				     155 - DECODE_INSN_U8 (insn));
	      break;
	    case A_S3:
	    case A_S6:
	    case A_S7:
	    case A_S7N:
	    case A_U3:
	    case A_U5:
	    case A_U6:
	    case A_U7:
	      hex_value = DECODE_INSN_I7 (insn);
	      len += sprintf (&buffer[len], "%d", hex_value);
	      break;
	    case A_S11:
	      len += sprintf (&buffer[len], "0x%x", memaddr + DECODE_INSN_I9a (insn) * 4);
	      break;
	    case A_S11I:
	      len += sprintf (&buffer[len], "0x%x", memaddr + DECODE_INSN_I9b (insn) * 4);
	      break;
	    case A_S10:
	    case A_S10B:
	      hex_value = DECODE_INSN_I10 (insn);
	      len += sprintf (&buffer[len], "%d", hex_value);
	      break;
	    case A_S14:
	      hex_value = DECODE_INSN_I10 (insn) * 16;
	      len += sprintf (&buffer[len], "%d", hex_value);
	      break;
	    case A_S16:
	      hex_value = DECODE_INSN_I16 (insn);
	      len += sprintf (&buffer[len], "%d", hex_value);
	      break;
	    case A_X16:
	      hex_value = DECODE_INSN_U16 (insn);
	      len += sprintf (&buffer[len], "%u", hex_value);
	      break;
	    case A_R18:
	      value = DECODE_INSN_I16 (insn) * 4;
	      if (value == 0)
		len += sprintf (&buffer[len], "%d", value);
	      else
		{
		  hex_value = memaddr + value;
		  len += sprintf (&buffer[len], "0x%x", hex_value & 0x3ffff);
		}
	      break;
	    case A_S18:
	      value = DECODE_INSN_U16 (insn) * 4;
	      if (value == 0)
		len += sprintf (&buffer[len], "%d", value);
	      else
		len += sprintf (&buffer[len], "0x%x", value);
	      break;
	    case A_U18:
	      value = DECODE_INSN_U18 (insn);
	      if (value == 0 /*|| !(*info->symbol_at_address_func)(0, info)*/)
		{
		  hex_value = value;
		  len += sprintf (&buffer[len], "%u", value);
		}
	      else
		len += sprintf (&buffer[len], "0x%x", value);
	      break;
	    case A_U14:
	      hex_value = DECODE_INSN_U14 (insn);
	      len += sprintf (&buffer[len], "%u", hex_value);
	      break;
	    }
	  if (arg != A_P && paren)
	    {
	      len += sprintf (&buffer[len], ")");
	      paren--;
	    }
	}
//      if (hex_value > 16)
//	len += sprintf (&buffer[len], "\t# %x", hex_value);
    }
  return buffer;
}

} /* namespace spu */
} /* namespace arch */
} /* namespace nc */