/* The file is part of Snowman decompiler. */
/* See doc/licenses.asciidoc for the licensing information. */

#pragma once

namespace nc {
namespace arch {
namespace spu {

typedef enum {
  RRR,
  RI18,
  RI16,
  RI10,
  RI8,
  RI7,
  RR,
  LBT,
  LBTI,
  IDATA,
  UNKNOWN_IFORMAT
} spu_iformat;

enum spu_insn
{
    M_BR,
    M_BRSL,
    M_BRA,
    M_BRASL,
    M_FSMBI,
    M_LQA,
    M_LQR,
    M_STOP,
    M_STOP2,
    M_STOPD,
    M_LNOP,
    M_SYNC,
    M_DSYNC,
    M_MFSPR,
    M_RDCH,
    M_RCHCNT,
    M_HBRA,
    M_HBRR,
    M_BRZ,
    M_BRNZ,
    M_BRHZ,
    M_BRHNZ,
    M_STQA,
    M_STQR,
    M_MTSPR,
    M_WRCH,
    M_LQD,
    M_BI,
    M_BISL,
    M_IRET,
    M_IRET2,
    M_BISLED,
    M_HBR,
    M_FREST,
    M_FRSQEST,
    M_FSM,
    M_FSMH,
    M_FSMB,
    M_GB,
    M_GBH,
    M_GBB,
    M_CBD,
    M_CHD,
    M_CWD,
    M_CDD,
    M_ROTQBII,
    M_ROTQBYI,
    M_ROTQMBII,
    M_ROTQMBYI,
    M_SHLQBII,
    M_SHLQBYI,
    M_STQD,
    M_BIHNZ,
    M_BIHZ,
    M_BINZ,
    M_BIZ,
    M_CBX,
    M_CHX,
    M_CWX,
    M_CDX,
    M_LQX,
    M_ROTQBI,
    M_ROTQMBI,
    M_SHLQBI,
    M_ROTQBY,
    M_ROTQMBY,
    M_SHLQBY,
    M_ROTQBYBI,
    M_ROTQMBYBI,
    M_SHLQBYBI,
    M_STQX,
    M_SHUFB,
    M_IL,
    M_ILH,
    M_ILHU,
    M_ILA,
    M_NOP,
    M_NOP2,
    M_IOHL,
    M_ANDBI,
    M_ANDHI,
    M_ANDI,
    M_ORBI,
    M_ORHI,
    M_ORI,
    M_ORX,
    M_XORBI,
    M_XORHI,
    M_XORI,
    M_AHI,
    M_AI,
    M_SFHI,
    M_SFI,
    M_CGTBI,
    M_CGTHI,
    M_CGTI,
    M_CLGTBI,
    M_CLGTHI,
    M_CLGTI,
    M_CEQBI,
    M_CEQHI,
    M_CEQI,
    M_HGTI,
    M_HGTI2,
    M_HLGTI,
    M_HLGTI2,
    M_HEQI,
    M_HEQI2,
    M_MPYI,
    M_MPYUI,
    M_CFLTS,
    M_CFLTU,
    M_CSFLT,
    M_CUFLT,
    M_FESD,
    M_FRDS,
    M_FSCRRD,
    M_FSCRWR,
    M_FSCRWR2,
    M_CLZ,
    M_CNTB,
    M_XSBH,
    M_XSHW,
    M_XSWD,
    M_ROTI,
    M_ROTMI,
    M_ROTMAI,
    M_SHLI,
    M_ROTHI,
    M_ROTHMI,
    M_ROTMAHI,
    M_SHLHI,
    M_A,
    M_AH,
    M_SF,
    M_SFH,
    M_CGT,
    M_CGTB,
    M_CGTH,
    M_CLGT,
    M_CLGTB,
    M_CLGTH,
    M_CEQ,
    M_CEQB,
    M_CEQH,
    M_HGT,
    M_HGT2,
    M_HLGT,
    M_HLGT2,
    M_HEQ,
    M_HEQ2,
    M_FCEQ,
    M_FCMEQ,
    M_FCGT,
    M_FCMGT,
    M_AND,
    M_NAND,
    M_OR,
    M_NOR,
    M_XOR,
    M_EQV,
    M_ANDC,
    M_ORC,
    M_ABSDB,
    M_AVGB,
    M_SUMB,
    M_DFA,
    M_DFM,
    M_DFS,
    M_FA,
    M_FM,
    M_FS,
    M_MPY,
    M_MPYH,
    M_MPYHH,
    M_MPYHHU,
    M_MPYS,
    M_MPYU,
    M_FI,
    M_ROT,
    M_ROTM,
    M_ROTMA,
    M_SHL,
    M_ROTH,
    M_ROTHM,
    M_ROTMAH,
    M_SHLH,
    M_MPYHHA,
    M_MPYHHAU,
    M_DFMA,
    M_DFMS,
    M_DFNMS,
    M_DFNMA,
    M_FMA,
    M_FMS,
    M_FNMS,
    M_MPYA,
    M_SELB,
    M_SYSCALL,
    M_ADDX,
    M_CG,
    M_CGX,
    M_SFX,
    M_BG,
    M_BGX,
    M_BID,
    M_BIE,
    M_BISLD,
    M_BISLE,
    M_IRETD,
    M_IRETD2,
    M_IRETE,
    M_IRETE2,
    M_BISLEDD,
    M_BISLEDE,
    M_BIHNZD,
    M_BIHNZE,
    M_BIHZD,
    M_BIHZE,
    M_BINZD,
    M_BINZE,
    M_BIZD,
    M_BIZE,
    M_SYNCC,
    M_HBRP,
    M_LR,
    M_BIHT,
    M_BIHF,
    M_BIT,
    M_BIF,
    M_BIHTD,
    M_BIHTE,
    M_BIHFD,
    M_BIHFE,
    M_BITD,
    M_BITE,
    M_BIFD,
    M_BIFE
};

typedef enum {
  A_T,  /* register at pos 0 */
  A_A,  /* register at pos 7 */
  A_B,  /* register at pos 14 */
  A_C,  /* register at pos 21 */
  A_S,  /* special purpose register at pos 7 */
  A_H,  /* channel register at pos 7 */
  A_P,  /* parenthesis, this has to separate regs from immediates */
  A_S3,
  A_S6,
  A_S7N,
  A_S7,
  A_U7A,
  A_U7B,
  A_S10B,
  A_S10,
  A_S11,
  A_S11I,
  A_S14,
  A_S16,
  A_S18,
  A_R18,
  A_U3,
  A_U5,
  A_U6,
  A_U7,
  A_U14,
  A_X16,
  A_U18,
  A_MAX
} spu_aformat;

#define _A0() {0}
#define _A1(a) {1,a}
#define _A2(a,b) {2,a,b}
#define _A3(a,b,c) {3,a,b,c}
#define _A4(a,b,c,d) {4,a,b,c,d}

struct spu_opcode
{
   spu_insn insn;
   spu_iformat insn_type;
   unsigned int opcode;
   char *mnemonic;
   int arg[5];
};

const struct spu_opcode spu_opcodes[] = {
#define APUOP(TAG,MACFORMAT,OPCODE,MNEMONIC,ASMFORMAT,DEP,PIP) \
    { TAG, MACFORMAT, OPCODE, MNEMONIC, ASMFORMAT },
#define APUOPFB(TAG,MACFORMAT,OPCODE,FB,MNEMONIC,ASMFORMAT,DEP,PIPE) \
	{ TAG, MACFORMAT, OPCODE, MNEMONIC, ASMFORMAT },

APUOP(M_BR,		RI16,	0x190,	"br",		_A1(A_R18),	00000,	BR)	/* BRel          IP<-IP+I16 */
APUOP(M_BRSL,		RI16,	0x198,	"brsl",		_A2(A_T,A_R18),	00002,	BR)	/* BRelSetLink   RT,IP<-IP,IP+I16 */
APUOP(M_BRA,		RI16,	0x180,	"bra",		_A1(A_S18),	00000,	BR)	/* BRAbs         IP<-I16 */
APUOP(M_BRASL,		RI16,	0x188,	"brasl",	_A2(A_T,A_S18),	00002,	BR)	/* BRAbsSetLink  RT,IP<-IP,I16 */
APUOP(M_FSMBI,		RI16,	0x194,	"fsmbi",	_A2(A_T,A_X16),	00002,	SHUF)	/* FormSelMask%I RT<-fsm(I16) */
APUOP(M_LQA,		RI16,	0x184,	"lqa",		_A2(A_T,A_S18),	00002,	LS)	/* LoadQAbs      RT<-M[I16] */
APUOP(M_LQR,		RI16,	0x19C,	"lqr",		_A2(A_T,A_R18),	00002,	LS)	/* LoadQRel      RT<-M[IP+I16] */
APUOP(M_STOP,		RR,	0x000,	"stop",		_A0(),		00000,	BR)	/* STOP          stop */
APUOP(M_STOP2,		RR,	0x000,	"stop",		_A1(A_U14),	00000,	BR)	/* STOP          stop */
APUOP(M_STOPD,		RR,	0x140,	"stopd",	_A3(A_T,A_A,A_B),         00111,	BR)	/* STOPD         stop (with register dependencies) */
APUOP(M_LNOP,		RR,	0x001,	"lnop",		_A0(),		00000,	LNOP)	/* LNOP          no_operation */
APUOP(M_SYNC,		RR,	0x002,	"sync",		_A0(),		00000,	BR)	/* SYNC          flush_pipe */
APUOP(M_DSYNC,		RR,	0x003,	"dsync",	_A0(),		00000,	BR)	/* DSYNC         flush_store_queue */
APUOP(M_MFSPR,		RR,	0x00c,	"mfspr",	_A2(A_T,A_S),	00002,	SPR)	/* MFSPR         RT<-SA */
APUOP(M_RDCH,		RR,	0x00d,	"rdch",		_A2(A_T,A_H),	00002,	SPR)	/* ReaDCHannel   RT<-CA:data */
APUOP(M_RCHCNT,		RR,	0x00f,	"rchcnt",	_A2(A_T,A_H),	00002,	SPR)	/* ReaDCHanCouNT RT<-CA:count */
APUOP(M_HBRA,		LBT,	0x080,	"hbra",		_A2(A_S11,A_S18),	00000,	LS)	/* HBRA          BTB[B9]<-M[I16] */
APUOP(M_HBRR,		LBT,	0x090,	"hbrr",		_A2(A_S11,A_R18),	00000,	LS)	/* HBRR          BTB[B9]<-M[IP+I16] */
APUOP(M_BRZ,		RI16,	0x100,	"brz",		_A2(A_T,A_R18),	00001,	BR)	/* BRZ           IP<-IP+I16_if(RT) */
APUOP(M_BRNZ,		RI16,	0x108,	"brnz",		_A2(A_T,A_R18),	00001,	BR)	/* BRNZ          IP<-IP+I16_if(RT) */
APUOP(M_BRHZ,		RI16,	0x110,	"brhz",		_A2(A_T,A_R18),	00001,	BR)	/* BRHZ          IP<-IP+I16_if(RT) */
APUOP(M_BRHNZ,		RI16,	0x118,	"brhnz",	_A2(A_T,A_R18),	00001,	BR)	/* BRHNZ         IP<-IP+I16_if(RT) */
APUOP(M_STQA,		RI16,	0x104,	"stqa",		_A2(A_T,A_S18),	00001,	LS)	/* SToreQAbs     M[I16]<-RT */
APUOP(M_STQR,		RI16,	0x11C,	"stqr",		_A2(A_T,A_R18),	00001,	LS)	/* SToreQRel     M[IP+I16]<-RT */
APUOP(M_MTSPR,		RR,	0x10c,	"mtspr",	_A2(A_S,A_T),	00001,	SPR)	/* MTSPR         SA<-RT */
APUOP(M_WRCH,		RR,	0x10d,	"wrch",		_A2(A_H,A_T),	00001,	SPR)	/* ChanWRite     CA<-RT */
APUOP(M_LQD,		RI10,	0x1a0,	"lqd",		_A4(A_T,A_S14,A_P,A_A),	00012,	LS)	/* LoadQDisp     RT<-M[Ra+I10] */
APUOP(M_BI,		RR,	0x1a8,	"bi",		_A1(A_A),		00010,	BR)	/* BI            IP<-RA */
APUOP(M_BISL,		RR,	0x1a9,	"bisl",		_A2(A_T,A_A),	00012,	BR)	/* BISL          RT,IP<-IP,RA */
APUOP(M_IRET,  		RR,	0x1aa,	"iret",	        _A1(A_A), 	00010,	BR)	/* IRET          IP<-SRR0 */
APUOP(M_IRET2, 		RR,	0x1aa,	"iret",	        _A0(),	 	00010,	BR)	/* IRET          IP<-SRR0 */
APUOP(M_BISLED,		RR,	0x1ab,	"bisled",	_A2(A_T,A_A),	00012,	BR)	/* BISLED        RT,IP<-IP,RA_if(ext) */
APUOP(M_HBR,		LBTI,	0x1ac,	"hbr",		_A2(A_S11I,A_A),	00010,	LS)	/* HBR           BTB[B9]<-M[Ra] */
APUOP(M_FREST,		RR,	0x1b8,	"frest",	_A2(A_T,A_A),	00012,	SHUF)	/* FREST         RT<-recip(RA) */
APUOP(M_FRSQEST,	RR,	0x1b9,	"frsqest",	_A2(A_T,A_A),	00012,	SHUF)	/* FRSQEST       RT<-rsqrt(RA) */
APUOP(M_FSM,		RR,	0x1b4,	"fsm",		_A2(A_T,A_A),	00012,	SHUF)	/* FormSelMask%  RT<-expand(Ra) */
APUOP(M_FSMH,		RR,	0x1b5,	"fsmh",		_A2(A_T,A_A),	00012,	SHUF)	/* FormSelMask%  RT<-expand(Ra) */
APUOP(M_FSMB,		RR,	0x1b6,	"fsmb",		_A2(A_T,A_A),	00012,	SHUF)	/* FormSelMask%  RT<-expand(Ra) */
APUOP(M_GB,		RR,	0x1b0,	"gb",		_A2(A_T,A_A),	00012,	SHUF)	/* GatherBits%   RT<-gather(RA) */
APUOP(M_GBH,		RR,	0x1b1,	"gbh",		_A2(A_T,A_A),	00012,	SHUF)	/* GatherBits%   RT<-gather(RA) */
APUOP(M_GBB,		RR,	0x1b2,	"gbb",		_A2(A_T,A_A),	00012,	SHUF)	/* GatherBits%   RT<-gather(RA) */
APUOP(M_CBD,		RI7,	0x1f4,	"cbd",		_A4(A_T,A_U7,A_P,A_A),	00012,	SHUF)	/* genCtl%%insD  RT<-sta(Ra+I4,siz) */
APUOP(M_CHD,		RI7,	0x1f5,	"chd",		_A4(A_T,A_U7,A_P,A_A),	00012,	SHUF)	/* genCtl%%insD  RT<-sta(Ra+I4,siz) */
APUOP(M_CWD,		RI7,	0x1f6,	"cwd",		_A4(A_T,A_U7,A_P,A_A),	00012,	SHUF)	/* genCtl%%insD  RT<-sta(Ra+I4,siz) */
APUOP(M_CDD,		RI7,	0x1f7,	"cdd",		_A4(A_T,A_U7,A_P,A_A),	00012,	SHUF)	/* genCtl%%insD  RT<-sta(Ra+I4,siz) */
APUOP(M_ROTQBII,	RI7,	0x1f8,	"rotqbii",	_A3(A_T,A_A,A_U3),	00012,	SHUF)	/* ROTQBII       RT<-RA<<<I7 */
APUOP(M_ROTQBYI,	RI7,	0x1fc,	"rotqbyi",	_A3(A_T,A_A,A_S7N),	00012,	SHUF)	/* ROTQBYI       RT<-RA<<<(I7*8) */
APUOP(M_ROTQMBII,	RI7,	0x1f9,	"rotqmbii",	_A3(A_T,A_A,A_S3),	00012,	SHUF)	/* ROTQMBII      RT<-RA<<I7 */
APUOP(M_ROTQMBYI,	RI7,	0x1fd,	"rotqmbyi",	_A3(A_T,A_A,A_S6),	00012,	SHUF)	/* ROTQMBYI      RT<-RA<<I7 */
APUOP(M_SHLQBII,	RI7,	0x1fb,	"shlqbii",	_A3(A_T,A_A,A_U3),	00012,	SHUF)	/* SHLQBII       RT<-RA<<I7 */
APUOP(M_SHLQBYI,	RI7,	0x1ff,	"shlqbyi",	_A3(A_T,A_A,A_U5),	00012,	SHUF)	/* SHLQBYI       RT<-RA<<I7 */
APUOP(M_STQD,		RI10,	0x120,	"stqd",		_A4(A_T,A_S14,A_P,A_A),	00011,	LS)	/* SToreQDisp    M[Ra+I10]<-RT */
APUOP(M_BIHNZ,		RR,	0x12b,	"bihnz",	_A2(A_T,A_A),	00011,	BR)	/* BIHNZ         IP<-RA_if(RT) */
APUOP(M_BIHZ,		RR,	0x12a,	"bihz",		_A2(A_T,A_A),	00011,	BR)	/* BIHZ          IP<-RA_if(RT) */
APUOP(M_BINZ,		RR,	0x129,	"binz",		_A2(A_T,A_A),	00011,	BR)	/* BINZ          IP<-RA_if(RT) */
APUOP(M_BIZ,		RR,	0x128,	"biz",		_A2(A_T,A_A),	00011,	BR)	/* BIZ           IP<-RA_if(RT) */
APUOP(M_CBX,		RR,	0x1d4,	"cbx",		_A3(A_T,A_A,A_B),		00112,	SHUF)	/* genCtl%%insX  RT<-sta(Ra+Rb,siz) */
APUOP(M_CHX,		RR,	0x1d5,	"chx",		_A3(A_T,A_A,A_B),		00112,	SHUF)	/* genCtl%%insX  RT<-sta(Ra+Rb,siz) */
APUOP(M_CWX,		RR,	0x1d6,	"cwx",		_A3(A_T,A_A,A_B),		00112,	SHUF)	/* genCtl%%insX  RT<-sta(Ra+Rb,siz) */
APUOP(M_CDX,		RR,	0x1d7,	"cdx",		_A3(A_T,A_A,A_B),		00112,	SHUF)	/* genCtl%%insX  RT<-sta(Ra+Rb,siz) */
APUOP(M_LQX,		RR,	0x1c4,	"lqx",		_A3(A_T,A_A,A_B),		00112,	LS)	/* LoadQindeX    RT<-M[Ra+Rb] */
APUOP(M_ROTQBI,		RR,	0x1d8,	"rotqbi",	_A3(A_T,A_A,A_B),		00112,	SHUF)	/* ROTQBI        RT<-RA<<<Rb */
APUOP(M_ROTQMBI,	RR,	0x1d9,	"rotqmbi",	_A3(A_T,A_A,A_B),		00112,	SHUF)	/* ROTQMBI       RT<-RA<<Rb */
APUOP(M_SHLQBI,		RR,	0x1db,	"shlqbi",	_A3(A_T,A_A,A_B),		00112,	SHUF)	/* SHLQBI        RT<-RA<<Rb */
APUOP(M_ROTQBY,		RR,	0x1dc,	"rotqby",	_A3(A_T,A_A,A_B),		00112,		SHUF)	/* ROTQBY        RT<-RA<<<(Rb*8) */
APUOP(M_ROTQMBY,	RR,	0x1dd,	"rotqmby",	_A3(A_T,A_A,A_B),		00112,		SHUF)	/* ROTQMBY       RT<-RA<<Rb */
APUOP(M_SHLQBY,		RR,	0x1df,	"shlqby",	_A3(A_T,A_A,A_B),		00112,	SHUF)	/* SHLQBY        RT<-RA<<Rb */
APUOP(M_ROTQBYBI,	RR,	0x1cc,	"rotqbybi",	_A3(A_T,A_A,A_B),		00112,		SHUF)	/* ROTQBYBI      RT<-RA<<Rb */
APUOP(M_ROTQMBYBI,	RR,	0x1cd,	"rotqmbybi",	_A3(A_T,A_A,A_B),		00112,		SHUF)	/* ROTQMBYBI     RT<-RA<<Rb */
APUOP(M_SHLQBYBI,	RR,	0x1cf,	"shlqbybi",	_A3(A_T,A_A,A_B),		00112,	SHUF)	/* SHLQBYBI      RT<-RA<<Rb */
APUOP(M_STQX,		RR,	0x144,	"stqx",		_A3(A_T,A_A,A_B),		00111,	LS)	/* SToreQindeX   M[Ra+Rb]<-RT */
APUOP(M_SHUFB,		RRR,	0x580,	"shufb",	_A4(A_C,A_A,A_B,A_T),	02111,	SHUF)	/* SHUFfleBytes  RC<-f(RA,RB,RT) */
APUOP(M_IL,		RI16,	0x204,	"il",		_A2(A_T,A_S16),	00002,	FX2)	/* ImmLoad       RT<-sxt(I16) */
APUOP(M_ILH,		RI16,	0x20c,	"ilh",		_A2(A_T,A_X16),	00002,	FX2)	/* ImmLoadH      RT<-I16 */
APUOP(M_ILHU,		RI16,	0x208,	"ilhu",		_A2(A_T,A_X16),	00002,	FX2)	/* ImmLoadHUpper RT<-I16<<16 */
APUOP(M_ILA,		RI18,	0x210,	"ila",		_A2(A_T,A_U18),	00002,	FX2)	/* ImmLoadAddr   RT<-zxt(I18) */
APUOP(M_NOP,		RR,	0x201,	"nop",		_A1(A_T),		00000,	NOP)	/* XNOP          no_operation */
APUOP(M_NOP2,		RR,	0x201,	"nop",		_A0(),		00000,	NOP)	/* XNOP          no_operation */
APUOP(M_IOHL,		RI16,	0x304,	"iohl",		_A2(A_T,A_X16),	00003,	FX2)	/* AddImmeXt     RT<-RT+sxt(I16) */
APUOP(M_ANDBI,		RI10,	0x0b0,	"andbi",	_A3(A_T,A_A,A_S10B),	00012,	FX2)	/* AND%I         RT<-RA&I10 */
APUOP(M_ANDHI,		RI10,	0x0a8,	"andhi",	_A3(A_T,A_A,A_S10),	00012,	FX2)	/* AND%I         RT<-RA&I10 */
APUOP(M_ANDI,		RI10,	0x0a0,	"andi",		_A3(A_T,A_A,A_S10),	00012,	FX2)	/* AND%I         RT<-RA&I10 */
APUOP(M_ORBI,		RI10,	0x030,	"orbi",		_A3(A_T,A_A,A_S10B),	00012,	FX2)	/* OR%I          RT<-RA|I10 */
APUOP(M_ORHI,		RI10,	0x028,	"orhi",		_A3(A_T,A_A,A_S10),	00012,	FX2)	/* OR%I          RT<-RA|I10 */
APUOP(M_ORI,		RI10,	0x020,	"ori",		_A3(A_T,A_A,A_S10),	00012,	FX2)	/* OR%I          RT<-RA|I10 */
APUOP(M_ORX,		RR,	0x1f0,	"orx",		_A2(A_T,A_A),		00012,	BR)	/* ORX           RT<-RA.w0|RA.w1|RA.w2|RA.w3 */
APUOP(M_XORBI,		RI10,	0x230,	"xorbi",	_A3(A_T,A_A,A_S10B),	00012,	FX2)	/* XOR%I         RT<-RA^I10 */
APUOP(M_XORHI,		RI10,	0x228,	"xorhi",	_A3(A_T,A_A,A_S10),	00012,	FX2)	/* XOR%I         RT<-RA^I10 */
APUOP(M_XORI,		RI10,	0x220,	"xori",		_A3(A_T,A_A,A_S10),	00012,	FX2)	/* XOR%I         RT<-RA^I10 */
APUOP(M_AHI,		RI10,	0x0e8,	"ahi",		_A3(A_T,A_A,A_S10),	00012,	FX2)	/* Add%Immed     RT<-RA+I10 */
APUOP(M_AI,		RI10,	0x0e0,	"ai",		_A3(A_T,A_A,A_S10),	00012,	FX2)	/* Add%Immed     RT<-RA+I10 */
APUOP(M_SFHI,		RI10,	0x068,	"sfhi",		_A3(A_T,A_A,A_S10),	00012,	FX2)	/* SubFrom%Imm   RT<-I10-RA */
APUOP(M_SFI,		RI10,	0x060,	"sfi",		_A3(A_T,A_A,A_S10),	00012,	FX2)	/* SubFrom%Imm   RT<-I10-RA */
APUOP(M_CGTBI,		RI10,	0x270,	"cgtbi",	_A3(A_T,A_A,A_S10B),	00012,	FX2)	/* CGT%I         RT<-(RA>I10) */
APUOP(M_CGTHI,		RI10,	0x268,	"cgthi",	_A3(A_T,A_A,A_S10),	00012,	FX2)	/* CGT%I         RT<-(RA>I10) */
APUOP(M_CGTI,		RI10,	0x260,	"cgti",		_A3(A_T,A_A,A_S10),	00012,	FX2)	/* CGT%I         RT<-(RA>I10) */
APUOP(M_CLGTBI,		RI10,	0x2f0,	"clgtbi",	_A3(A_T,A_A,A_S10B),	00012,	FX2)	/* CLGT%I        RT<-(RA>I10) */
APUOP(M_CLGTHI,		RI10,	0x2e8,	"clgthi",	_A3(A_T,A_A,A_S10),	00012,	FX2)	/* CLGT%I        RT<-(RA>I10) */
APUOP(M_CLGTI,		RI10,	0x2e0,	"clgti",	_A3(A_T,A_A,A_S10),	00012,	FX2)	/* CLGT%I        RT<-(RA>I10) */
APUOP(M_CEQBI,		RI10,	0x3f0,	"ceqbi",	_A3(A_T,A_A,A_S10B),	00012,	FX2)	/* CEQ%I         RT<-(RA=I10) */
APUOP(M_CEQHI,		RI10,	0x3e8,	"ceqhi",	_A3(A_T,A_A,A_S10),	00012,	FX2)	/* CEQ%I         RT<-(RA=I10) */
APUOP(M_CEQI,		RI10,	0x3e0,	"ceqi",		_A3(A_T,A_A,A_S10),	00012,	FX2)	/* CEQ%I         RT<-(RA=I10) */
APUOP(M_HGTI,		RI10,	0x278,	"hgti",		_A3(A_T,A_A,A_S10),	00010,	FX2)	/* HaltGTI       halt_if(RA>I10) */
APUOP(M_HGTI2,		RI10,	0x278,	"hgti",		_A2(A_A,A_S10),	00010,	FX2)	/* HaltGTI       halt_if(RA>I10) */
APUOP(M_HLGTI,		RI10,	0x2f8,	"hlgti",	_A3(A_T,A_A,A_S10),	00010,	FX2)	/* HaltLGTI      halt_if(RA>I10) */
APUOP(M_HLGTI2,		RI10,	0x2f8,	"hlgti",	_A2(A_A,A_S10),	00010,	FX2)	/* HaltLGTI      halt_if(RA>I10) */
APUOP(M_HEQI,		RI10,	0x3f8,	"heqi",		_A3(A_T,A_A,A_S10),	00010,	FX2)	/* HaltEQImm     halt_if(RA=I10) */
APUOP(M_HEQI2,		RI10,	0x3f8,	"heqi",		_A2(A_A,A_S10),	00010,	FX2)	/* HaltEQImm     halt_if(RA=I10) */
APUOP(M_MPYI,		RI10,	0x3a0,	"mpyi",		_A3(A_T,A_A,A_S10),	00012,	FP7)	/* MPYI          RT<-RA*I10 */
APUOP(M_MPYUI,		RI10,	0x3a8,	"mpyui",	_A3(A_T,A_A,A_S10),	00012,	FP7)	/* MPYUI         RT<-RA*I10 */
APUOP(M_CFLTS,		RI8,	0x3b0,	"cflts",	_A3(A_T,A_A,A_U7A),	00012,	FP7)	/* CFLTS         RT<-int(RA,I8) */
APUOP(M_CFLTU,		RI8,	0x3b2,	"cfltu",	_A3(A_T,A_A,A_U7A),	00012,	FP7)	/* CFLTU         RT<-int(RA,I8) */
APUOP(M_CSFLT,		RI8,	0x3b4,	"csflt",	_A3(A_T,A_A,A_U7B),	00012,	FP7)	/* CSFLT         RT<-flt(RA,I8) */
APUOP(M_CUFLT,		RI8,	0x3b6,	"cuflt",	_A3(A_T,A_A,A_U7B),	00012,	FP7)	/* CUFLT         RT<-flt(RA,I8) */
APUOP(M_FESD,		RR,	0x3b8,	"fesd",		_A2(A_T,A_A),	00012,	FPD)	/* FESD          RT<-double(RA) */
APUOP(M_FRDS,		RR,	0x3b9,	"frds",		_A2(A_T,A_A),	00012,	FPD)	/* FRDS          RT<-single(RA) */
APUOP(M_FSCRRD,		RR,	0x398,	"fscrrd",	_A1(A_T),		00002,	FPD)	/* FSCRRD        RT<-FP_status */
APUOP(M_FSCRWR,		RR,	0x3ba,	"fscrwr",	_A2(A_T,A_A),	00010,	FP7)	/* FSCRWR        FP_status<-RA */
APUOP(M_FSCRWR2,	RR,	0x3ba,	"fscrwr",	_A1(A_A),		00010,	FP7)	/* FSCRWR        FP_status<-RA */
APUOP(M_CLZ,		RR,	0x2a5,	"clz",		_A2(A_T,A_A),	00012,	FX2)	/* CLZ           RT<-clz(RA) */
APUOP(M_CNTB,		RR,	0x2b4,	"cntb",		_A2(A_T,A_A),	00012,	FXB)	/* CNT           RT<-pop(RA) */
APUOP(M_XSBH,		RR,	0x2b6,	"xsbh",		_A2(A_T,A_A),	00012,	FX2)	/* eXtSignBtoH   RT<-sign_ext(RA) */
APUOP(M_XSHW,		RR,	0x2ae,	"xshw",		_A2(A_T,A_A),	00012,	FX2)	/* eXtSignHtoW   RT<-sign_ext(RA) */
APUOP(M_XSWD,		RR,	0x2a6,	"xswd",		_A2(A_T,A_A),	00012,	FX2)	/* eXtSignWtoD   RT<-sign_ext(RA) */
APUOP(M_ROTI,		RI7,	0x078,	"roti",		_A3(A_T,A_A,A_S7N),	00012,	FX3)	/* ROT%I         RT<-RA<<<I7 */
APUOP(M_ROTMI,		RI7,	0x079,	"rotmi",	_A3(A_T,A_A,A_S7),	00012,	FX3)	/* ROT%MI        RT<-RA<<I7 */
APUOP(M_ROTMAI,		RI7,	0x07a,	"rotmai",	_A3(A_T,A_A,A_S7),	00012,	FX3)	/* ROTMA%I       RT<-RA<<I7 */
APUOP(M_SHLI,		RI7,	0x07b,	"shli",		_A3(A_T,A_A,A_U6),	00012,	FX3)	/* SHL%I         RT<-RA<<I7 */
APUOP(M_ROTHI,		RI7,	0x07c,	"rothi",	_A3(A_T,A_A,A_S7N),	00012,	FX3)	/* ROT%I         RT<-RA<<<I7 */
APUOP(M_ROTHMI,		RI7,	0x07d,	"rothmi",	_A3(A_T,A_A,A_S6),	00012,	FX3)	/* ROT%MI        RT<-RA<<I7 */
APUOP(M_ROTMAHI,	RI7,	0x07e,	"rotmahi",	_A3(A_T,A_A,A_S6),	00012,	FX3)	/* ROTMA%I       RT<-RA<<I7 */
APUOP(M_SHLHI,		RI7,	0x07f,	"shlhi",	_A3(A_T,A_A,A_U5),	00012,	FX3)	/* SHL%I         RT<-RA<<I7 */
APUOP(M_A,		RR,	0x0c0,	"a",		_A3(A_T,A_A,A_B),		00112,	FX2)	/* Add%          RT<-RA+RB */
APUOP(M_AH,		RR,	0x0c8,	"ah",		_A3(A_T,A_A,A_B),		00112,	FX2)	/* Add%          RT<-RA+RB */
APUOP(M_SF,		RR,	0x040,	"sf",		_A3(A_T,A_A,A_B),		00112,	FX2)	/* SubFrom%      RT<-RB-RA */
APUOP(M_SFH,		RR,	0x048,	"sfh",		_A3(A_T,A_A,A_B),		00112,	FX2)	/* SubFrom%      RT<-RB-RA */
APUOP(M_CGT,		RR,	0x240,	"cgt",		_A3(A_T,A_A,A_B),		00112,	FX2)	/* CGT%          RT<-(RA>RB) */
APUOP(M_CGTB,		RR,	0x250,	"cgtb",		_A3(A_T,A_A,A_B),		00112,	FX2)	/* CGT%          RT<-(RA>RB) */
APUOP(M_CGTH,		RR,	0x248,	"cgth",		_A3(A_T,A_A,A_B),		00112,	FX2)	/* CGT%          RT<-(RA>RB) */
APUOP(M_CLGT,		RR,	0x2c0,	"clgt",		_A3(A_T,A_A,A_B),		00112,	FX2)	/* CLGT%         RT<-(RA>RB) */
APUOP(M_CLGTB,		RR,	0x2d0,	"clgtb",	_A3(A_T,A_A,A_B),		00112,	FX2)	/* CLGT%         RT<-(RA>RB) */
APUOP(M_CLGTH,		RR,	0x2c8,	"clgth",	_A3(A_T,A_A,A_B),		00112,	FX2)	/* CLGT%         RT<-(RA>RB) */
APUOP(M_CEQ,		RR,	0x3c0,	"ceq",		_A3(A_T,A_A,A_B),		00112,	FX2)	/* CEQ%          RT<-(RA=RB) */
APUOP(M_CEQB,		RR,	0x3d0,	"ceqb",		_A3(A_T,A_A,A_B),		00112,	FX2)	/* CEQ%          RT<-(RA=RB) */
APUOP(M_CEQH,		RR,	0x3c8,	"ceqh",		_A3(A_T,A_A,A_B),		00112,	FX2)	/* CEQ%          RT<-(RA=RB) */
APUOP(M_HGT,		RR,	0x258,	"hgt",		_A3(A_T,A_A,A_B),		00110,	FX2)	/* HaltGT        halt_if(RA>RB) */
APUOP(M_HGT2,		RR,	0x258,	"hgt",		_A2(A_A,A_B),	00110,	FX2)	/* HaltGT        halt_if(RA>RB) */
APUOP(M_HLGT,		RR,	0x2d8,	"hlgt",		_A3(A_T,A_A,A_B),		00110,	FX2)	/* HaltLGT       halt_if(RA>RB) */
APUOP(M_HLGT2,		RR,	0x2d8,	"hlgt",		_A2(A_A,A_B),	00110,	FX2)	/* HaltLGT       halt_if(RA>RB) */
APUOP(M_HEQ,		RR,	0x3d8,	"heq",		_A3(A_T,A_A,A_B),		00110,	FX2)	/* HaltEQ        halt_if(RA=RB) */
APUOP(M_HEQ2,		RR,	0x3d8,	"heq",		_A2(A_A,A_B),	00110,	FX2)	/* HaltEQ        halt_if(RA=RB) */
APUOP(M_FCEQ,		RR,	0x3c2,	"fceq",		_A3(A_T,A_A,A_B),		00112,	FX2)	/* FCEQ          RT<-(RA=RB) */
APUOP(M_FCMEQ,		RR,	0x3ca,	"fcmeq",	_A3(A_T,A_A,A_B),		00112,	FX2)	/* FCMEQ         RT<-(|RA|=|RB|) */
APUOP(M_FCGT,		RR,	0x2c2,	"fcgt",		_A3(A_T,A_A,A_B),		00112,	FX2)	/* FCGT          RT<-(RA<RB) */
APUOP(M_FCMGT,		RR,	0x2ca,	"fcmgt",	_A3(A_T,A_A,A_B),		00112,	FX2)	/* FCMGT         RT<-(|RA|<|RB|) */
APUOP(M_AND,		RR,	0x0c1,	"and",		_A3(A_T,A_A,A_B),		00112,	FX2)	/* AND           RT<-RA&RB */
APUOP(M_NAND,		RR,	0x0c9,	"nand",		_A3(A_T,A_A,A_B),		00112,	FX2)	/* NAND          RT<-!(RA&RB) */
APUOP(M_OR,		RR,	0x041,	"or",		_A3(A_T,A_A,A_B),		00112,	FX2)	/* OR            RT<-RA|RB */
APUOP(M_NOR,		RR,	0x049,	"nor",		_A3(A_T,A_A,A_B),		00112,	FX2)	/* NOR           RT<-!(RA&RB) */
APUOP(M_XOR,		RR,	0x241,	"xor",		_A3(A_T,A_A,A_B),		00112,	FX2)	/* XOR           RT<-RA^RB */
APUOP(M_EQV,		RR,	0x249,	"eqv",		_A3(A_T,A_A,A_B),		00112,	FX2)	/* EQuiValent    RT<-!(RA^RB) */
APUOP(M_ANDC,		RR,	0x2c1,	"andc",		_A3(A_T,A_A,A_B),		00112,	FX2)	/* ANDComplement RT<-RA&!RB */
APUOP(M_ORC,		RR,	0x2c9,	"orc",		_A3(A_T,A_A,A_B),		00112,	FX2)	/* ORComplement  RT<-RA|!RB */
APUOP(M_ABSDB,		RR,	0x053,	"absdb",	_A3(A_T,A_A,A_B),		00112,	FXB)	/* ABSoluteDiff  RT<-|RA-RB| */
APUOP(M_AVGB,		RR,	0x0d3,	"avgb",		_A3(A_T,A_A,A_B),		00112,	FXB)	/* AVG%          RT<-(RA+RB+1)/2 */
APUOP(M_SUMB,		RR,	0x253,	"sumb",		_A3(A_T,A_A,A_B),		00112,	FXB)	/* SUM%          RT<-f(RA,RB) */
APUOP(M_DFA,		RR,	0x2cc,	"dfa",		_A3(A_T,A_A,A_B),		00112,	FPD)	/* DFAdd         RT<-RA+RB */
APUOP(M_DFM,		RR,	0x2ce,	"dfm",		_A3(A_T,A_A,A_B),		00112,	FPD)	/* DFMul         RT<-RA*RB */
APUOP(M_DFS,		RR,	0x2cd,	"dfs",		_A3(A_T,A_A,A_B),		00112,	FPD)	/* DFSub         RT<-RA-RB */
APUOP(M_FA,		RR,	0x2c4,	"fa",		_A3(A_T,A_A,A_B),		00112,	FP6)	/* FAdd          RT<-RA+RB */
APUOP(M_FM,		RR,	0x2c6,	"fm",		_A3(A_T,A_A,A_B),		00112,	FP6)	/* FMul          RT<-RA*RB */
APUOP(M_FS,		RR,	0x2c5,	"fs",		_A3(A_T,A_A,A_B),		00112,	FP6)	/* FSub          RT<-RA-RB */
APUOP(M_MPY,		RR,	0x3c4,	"mpy",		_A3(A_T,A_A,A_B),		00112,	FP7)	/* MPY           RT<-RA*RB */
APUOP(M_MPYH,		RR,	0x3c5,	"mpyh",		_A3(A_T,A_A,A_B),		00112,	FP7)	/* MPYH          RT<-(RAh*RB)<<16 */
APUOP(M_MPYHH,		RR,	0x3c6,	"mpyhh",	_A3(A_T,A_A,A_B),		00112,	FP7)	/* MPYHH         RT<-RAh*RBh */
APUOP(M_MPYHHU,		RR,	0x3ce,	"mpyhhu",	_A3(A_T,A_A,A_B),		00112,	FP7)	/* MPYHHU        RT<-RAh*RBh */
APUOP(M_MPYS,		RR,	0x3c7,	"mpys",		_A3(A_T,A_A,A_B),		00112,	FP7)	/* MPYS          RT<-(RA*RB)>>16 */
APUOP(M_MPYU,		RR,	0x3cc,	"mpyu",		_A3(A_T,A_A,A_B),		00112,	FP7)	/* MPYU          RT<-RA*RB */
APUOP(M_FI,		RR,	0x3d4,	"fi",		_A3(A_T,A_A,A_B),		00112,	FP7)	/* FInterpolate  RT<-f(RA,RB) */
APUOP(M_ROT,		RR,	0x058,	"rot",		_A3(A_T,A_A,A_B),		00112,	FX3)	/* ROT%          RT<-RA<<<RB */
APUOP(M_ROTM,		RR,	0x059,	"rotm",		_A3(A_T,A_A,A_B),		00112,	FX3)	/* ROT%M         RT<-RA<<Rb */
APUOP(M_ROTMA,		RR,	0x05a,	"rotma",	_A3(A_T,A_A,A_B),		00112,	FX3)	/* ROTMA%        RT<-RA<<Rb */
APUOP(M_SHL,		RR,	0x05b,	"shl",		_A3(A_T,A_A,A_B),		00112,	FX3)	/* SHL%          RT<-RA<<Rb */
APUOP(M_ROTH,		RR,	0x05c,	"roth",		_A3(A_T,A_A,A_B),		00112,	FX3)	/* ROT%          RT<-RA<<<RB */
APUOP(M_ROTHM,		RR,	0x05d,	"rothm",	_A3(A_T,A_A,A_B),		00112,	FX3)	/* ROT%M         RT<-RA<<Rb */
APUOP(M_ROTMAH,		RR,	0x05e,	"rotmah",	_A3(A_T,A_A,A_B),		00112,	FX3)	/* ROTMA%        RT<-RA<<Rb */
APUOP(M_SHLH,		RR,	0x05f,	"shlh",		_A3(A_T,A_A,A_B),		00112,	FX3)	/* SHL%          RT<-RA<<Rb */
APUOP(M_MPYHHA,		RR,	0x346,	"mpyhha",	_A3(A_T,A_A,A_B),		00113,	FP7)	/* MPYHHA        RT<-RAh*RBh+RT */
APUOP(M_MPYHHAU,	RR,	0x34e,	"mpyhhau",	_A3(A_T,A_A,A_B),		00113,	FP7)	/* MPYHHAU       RT<-RAh*RBh+RT */
APUOP(M_DFMA,		RR,	0x35c,	"dfma",		_A3(A_T,A_A,A_B),		00113,	FPD)	/* DFMAdd        RT<-RT+RA*RB */
APUOP(M_DFMS,		RR,	0x35d,	"dfms",		_A3(A_T,A_A,A_B),		00113,	FPD)	/* DFMSub        RT<-RA*RB-RT */
APUOP(M_DFNMS,		RR,	0x35e,	"dfnms",	_A3(A_T,A_A,A_B),		00113,	FPD)	/* DFNMSub       RT<-RT-RA*RB */
APUOP(M_DFNMA,		RR,	0x35f,	"dfnma",	_A3(A_T,A_A,A_B),		00113,	FPD)	/* DFNMAdd       RT<-(-RT)-RA*RB */
APUOP(M_FMA,		RRR,	0x700,	"fma",		_A4(A_C,A_A,A_B,A_T),	02111,	FP6)	/* FMAdd         RC<-RT+RA*RB */
APUOP(M_FMS,		RRR,	0x780,	"fms",		_A4(A_C,A_A,A_B,A_T),	02111,	FP6)	/* FMSub         RC<-RA*RB-RT */
APUOP(M_FNMS,		RRR,	0x680,	"fnms",		_A4(A_C,A_A,A_B,A_T),	02111,	FP6)	/* FNMSub        RC<-RT-RA*RB */
APUOP(M_MPYA,		RRR,	0x600,	"mpya",		_A4(A_C,A_A,A_B,A_T),	02111,	FP7)	/* MPYA          RC<-RA*RB+RT */
APUOP(M_SELB,		RRR,	0x400,	"selb",		_A4(A_C,A_A,A_B,A_T),	02111,	FX2)	/* SELectBits    RC<-RA&RT|RB&!RT */
/* for system function call, this uses op-code of mtspr */
APUOP(M_SYSCALL,	RI7,    0x10c,	"syscall",      _A3(A_T,A_A,A_S7N),	00002,	SPR)        /* System Call */
/*
pseudo instruction:
system call
value of I9	operation
0	halt
1		rt[0] = open(MEM[ra[0]],	ra[1])
2		rt[0] = close(ra[0])
3		rt[0] = read(ra[0],	MEM[ra[1]],	ra[2])
4		rt[0] = write(ra[0],	MEM[ra[1]],	ra[2])
5		printf(MEM[ra[0]],	ra[1],	ra[2],	ra[3])
42		rt[0] = clock()
52		rt[0] = lseek(ra0,	ra1,	ra2)

*/


/* new multiprecision add/sub */
APUOP(M_ADDX,		RR,	0x340,	"addx",		_A3(A_T,A_A,A_B),		00113,		FX2)	/* Add_eXtended  RT<-RA+RB+RT */
APUOP(M_CG,		RR,	0x0c2,	"cg",		_A3(A_T,A_A,A_B),		00112,		FX2)	/* CarryGenerate RT<-cout(RA+RB) */
APUOP(M_CGX,		RR,	0x342,	"cgx",		_A3(A_T,A_A,A_B),		00113,		FX2)	/* CarryGen_eXtd RT<-cout(RA+RB+RT) */
APUOP(M_SFX,		RR,	0x341,	"sfx",		_A3(A_T,A_A,A_B),		00113,		FX2)	/* Add_eXtended  RT<-RA+RB+RT */
APUOP(M_BG,		RR,	0x042,	"bg",		_A3(A_T,A_A,A_B),		00112,		FX2)	/* CarryGenerate RT<-cout(RA+RB) */
APUOP(M_BGX,		RR,	0x343,	"bgx",		_A3(A_T,A_A,A_B),		00113,		FX2)	/* CarryGen_eXtd RT<-cout(RA+RB+RT) */

/*

The following ops are a subset of above except with feature bits set.
Feature bits are bits 11-17 of the instruction:

  11 - C & P feature bit
  12 - disable interrupts
  13 - enable interrupts

*/
APUOPFB(M_BID,		RR,	0x1a8,	0x20,	"bid",		_A1(A_A),		00010,	BR)	/* BI            IP<-RA */
APUOPFB(M_BIE,		RR,	0x1a8,	0x10,	"bie",		_A1(A_A),		00010,	BR)	/* BI            IP<-RA */
APUOPFB(M_BISLD,	RR,	0x1a9,	0x20,	"bisld",	_A2(A_T,A_A),	00012,	BR)	/* BISL          RT,IP<-IP,RA */
APUOPFB(M_BISLE,	RR,	0x1a9,	0x10,	"bisle",	_A2(A_T,A_A),	00012,	BR)	/* BISL          RT,IP<-IP,RA */
APUOPFB(M_IRETD,  	RR,	0x1aa,	0x20,	"iretd",	_A1(A_A), 	00010,	BR)	/* IRET          IP<-SRR0 */
APUOPFB(M_IRETD2,  	RR,	0x1aa,	0x20,	"iretd",	_A0(),	 	00010,	BR)	/* IRET          IP<-SRR0 */
APUOPFB(M_IRETE,  	RR,	0x1aa,	0x10,	"irete",	_A1(A_A), 	00010,	BR)	/* IRET          IP<-SRR0 */
APUOPFB(M_IRETE2,  	RR,	0x1aa,	0x10,	"irete",	_A0(),	 	00010,	BR)	/* IRET          IP<-SRR0 */
APUOPFB(M_BISLEDD,	RR,	0x1ab,	0x20,	"bisledd",	_A2(A_T,A_A),	00012,	BR)	/* BISLED        RT,IP<-IP,RA_if(ext) */
APUOPFB(M_BISLEDE,	RR,	0x1ab,	0x10,	"bislede",	_A2(A_T,A_A),	00012,	BR)	/* BISLED        RT,IP<-IP,RA_if(ext) */
APUOPFB(M_BIHNZD,	RR,	0x12b,	0x20,	"bihnzd",	_A2(A_T,A_A),	00011,	BR)	/* BIHNZ         IP<-RA_if(RT) */
APUOPFB(M_BIHNZE,	RR,	0x12b,	0x10,	"bihnze",	_A2(A_T,A_A),	00011,	BR)	/* BIHNZ         IP<-RA_if(RT) */
APUOPFB(M_BIHZD,	RR,	0x12a,	0x20,	"bihzd",	_A2(A_T,A_A),	00011,	BR)	/* BIHZ          IP<-RA_if(RT) */
APUOPFB(M_BIHZE,	RR,	0x12a,	0x10,	"bihze",	_A2(A_T,A_A),	00011,	BR)	/* BIHZ          IP<-RA_if(RT) */
APUOPFB(M_BINZD,	RR,	0x129,	0x20,	"binzd",	_A2(A_T,A_A),	00011,	BR)	/* BINZ          IP<-RA_if(RT) */
APUOPFB(M_BINZE,	RR,	0x129,	0x10,	"binze",	_A2(A_T,A_A),	00011,	BR)	/* BINZ          IP<-RA_if(RT) */
APUOPFB(M_BIZD,		RR,	0x128,	0x20,	"bizd",		_A2(A_T,A_A),	00011,	BR)	/* BIZ           IP<-RA_if(RT) */
APUOPFB(M_BIZE,		RR,	0x128,	0x10,	"bize",		_A2(A_T,A_A),	00011,	BR)	/* BIZ           IP<-RA_if(RT) */
APUOPFB(M_SYNCC,	RR,	0x002,	0x40,	"syncc",	_A0(),		00000,	BR)	/* SYNCC          flush_pipe */
APUOPFB(M_HBRP,		LBTI,	0x1ac,	0x40,	"hbrp",		_A0(),	        00010,	LS)	/* HBR           BTB[B9]<-M[Ra] */

/* Synonyms required by the AS manual. */
APUOP(M_LR,		RI10,	0x020,	"lr",		_A2(A_T,A_A),	00012,	FX2)	/* OR%I          RT<-RA|I10 */
APUOP(M_BIHT,		RR,	0x12b,	"biht", 	_A2(A_T,A_A),	00011,	BR)	/* BIHNZ         IP<-RA_if(RT) */
APUOP(M_BIHF,		RR,	0x12a,	"bihf",		_A2(A_T,A_A),	00011,	BR)	/* BIHZ          IP<-RA_if(RT) */
APUOP(M_BIT,		RR,	0x129,	"bit",		_A2(A_T,A_A),	00011,	BR)	/* BINZ          IP<-RA_if(RT) */
APUOP(M_BIF,		RR,	0x128,	"bif",		_A2(A_T,A_A),	00011,	BR)	/* BIZ           IP<-RA_if(RT) */
APUOPFB(M_BIHTD,	RR,	0x12b,	0x20,	"bihtd",	_A2(A_T,A_A),	00011,	BR)	/* BIHNF         IP<-RA_if(RT) */
APUOPFB(M_BIHTE,	RR,	0x12b,	0x10,	"bihte",	_A2(A_T,A_A),	00011,	BR)	/* BIHNF         IP<-RA_if(RT) */
APUOPFB(M_BIHFD,	RR,	0x12a,	0x20,	"bihfd",	_A2(A_T,A_A),	00011,	BR)	/* BIHZ          IP<-RA_if(RT) */
APUOPFB(M_BIHFE,	RR,	0x12a,	0x10,	"bihfe",	_A2(A_T,A_A),	00011,	BR)	/* BIHZ          IP<-RA_if(RT) */
APUOPFB(M_BITD, 	RR,	0x129,	0x20,	"bitd", 	_A2(A_T,A_A),	00011,	BR)	/* BINF          IP<-RA_if(RT) */
APUOPFB(M_BITE, 	RR,	0x129,	0x10,	"bite", 	_A2(A_T,A_A),	00011,	BR)	/* BINF          IP<-RA_if(RT) */
APUOPFB(M_BIFD,		RR,	0x128,	0x20,	"bifd",		_A2(A_T,A_A),	00011,	BR)	/* BIZ           IP<-RA_if(RT) */
APUOPFB(M_BIFE,		RR,	0x128,	0x10,	"bife",		_A2(A_T,A_A),	00011,	BR)	/* BIZ           IP<-RA_if(RT) */
};

const int spu_num_opcodes =
  sizeof (spu_opcodes) / sizeof (spu_opcodes[0]);

enum spu_reg
{
    SPU_REG_R0, SPU_REG_LR = 0,
    SPU_REG_R1, SPU_REG_SP = 1,
    SPU_REG_R2,
    SPU_REG_R3,
    SPU_REG_R4,
    SPU_REG_R5,
    SPU_REG_R6,
    SPU_REG_R7,
    SPU_REG_R8,
    SPU_REG_R9,
    SPU_REG_R10,
    SPU_REG_R11,
    SPU_REG_R12,
    SPU_REG_R13,
    SPU_REG_R14,
    SPU_REG_R15,
    SPU_REG_R16,
    SPU_REG_R17,
    SPU_REG_R18,
    SPU_REG_R19,
    SPU_REG_R20,
    SPU_REG_R21,
    SPU_REG_R22,
    SPU_REG_R23,
    SPU_REG_R24,
    SPU_REG_R25,
    SPU_REG_R26,
    SPU_REG_R27,
    SPU_REG_R28,
    SPU_REG_R29,
    SPU_REG_R30,
    SPU_REG_R31,
    SPU_REG_R32,
    SPU_REG_R33,
    SPU_REG_R34,
    SPU_REG_R35,
    SPU_REG_R36,
    SPU_REG_R37,
    SPU_REG_R38,
    SPU_REG_R39,
    SPU_REG_R40,
    SPU_REG_R41,
    SPU_REG_R42,
    SPU_REG_R43,
    SPU_REG_R44,
    SPU_REG_R45,
    SPU_REG_R46,
    SPU_REG_R47,
    SPU_REG_R48,
    SPU_REG_R49,
    SPU_REG_R50,
    SPU_REG_R51,
    SPU_REG_R52,
    SPU_REG_R53,
    SPU_REG_R54,
    SPU_REG_R55,
    SPU_REG_R56,
    SPU_REG_R57,
    SPU_REG_R58,
    SPU_REG_R59,
    SPU_REG_R60,
    SPU_REG_R61,
    SPU_REG_R62,
    SPU_REG_R63,
    SPU_REG_R64,
    SPU_REG_R65,
    SPU_REG_R66,
    SPU_REG_R67,
    SPU_REG_R68,
    SPU_REG_R69,
    SPU_REG_R70,
    SPU_REG_R71,
    SPU_REG_R72,
    SPU_REG_R73,
    SPU_REG_R74,
    SPU_REG_R75,
    SPU_REG_R76,
    SPU_REG_R77,
    SPU_REG_R78,
    SPU_REG_R79,
    SPU_REG_R80,
    SPU_REG_R81,
    SPU_REG_R82,
    SPU_REG_R83,
    SPU_REG_R84,
    SPU_REG_R85,
    SPU_REG_R86,
    SPU_REG_R87,
    SPU_REG_R88,
    SPU_REG_R89,
    SPU_REG_R90,
    SPU_REG_R91,
    SPU_REG_R92,
    SPU_REG_R93,
    SPU_REG_R94,
    SPU_REG_R95,
    SPU_REG_R96,
    SPU_REG_R97,
    SPU_REG_R98,
    SPU_REG_R99,
    SPU_REG_R100,
    SPU_REG_R101,
    SPU_REG_R102,
    SPU_REG_R103,
    SPU_REG_R104,
    SPU_REG_R105,
    SPU_REG_R106,
    SPU_REG_R107,
    SPU_REG_R108,
    SPU_REG_R109,
    SPU_REG_R110,
    SPU_REG_R111,
    SPU_REG_R112,
    SPU_REG_R113,
    SPU_REG_R114,
    SPU_REG_R115,
    SPU_REG_R116,
    SPU_REG_R117,
    SPU_REG_R118,
    SPU_REG_R119,
    SPU_REG_R120,
    SPU_REG_R121,
    SPU_REG_R122,
    SPU_REG_R123,
    SPU_REG_R124,
    SPU_REG_R125,
    SPU_REG_R126,
    SPU_REG_R127,
    SPU_REG_COUNT
};

enum spu_operand_type
{
    OP_INVALID = 0,
    OP_REG = 1,
    OP_IMM = 2,
    OP_MEM = 3,
};

struct spu_operand_mem
{
    unsigned int base;
    unsigned int disp;
};

struct spu_operand
{
    spu_operand_type type;
    union
    {
        unsigned int reg;
        int imm;
        spu_operand_mem mem;
    };
};

struct spu_instruction
{
    spu_insn opcode;
    char *mnemonic;
    unsigned int addr;
    int op_count;
    spu_operand operands[5];
};


#define SIGNED_EXTRACT(insn,size,pos) (((int)((insn) << (32-size-pos))) >> (32-size))
#define UNSIGNED_EXTRACT(insn,size,pos) (((insn) >> pos) & ((1 << size)-1))

#define DECODE_INSN_RT(insn) (insn & 0x7f)
#define DECODE_INSN_RA(insn) ((insn >> 7) & 0x7f)
#define DECODE_INSN_RB(insn) ((insn >> 14) & 0x7f)
#define DECODE_INSN_RC(insn) ((insn >> 21) & 0x7f)

#define DECODE_INSN_I10(insn) SIGNED_EXTRACT(insn,10,14)
#define DECODE_INSN_U10(insn) UNSIGNED_EXTRACT(insn,10,14)

/* For branching, immediate loads, hbr and  lqa/stqa. */
#define DECODE_INSN_I16(insn) SIGNED_EXTRACT(insn,16,7)
#define DECODE_INSN_U16(insn) UNSIGNED_EXTRACT(insn,16,7)

/* for stop */
#define DECODE_INSN_U14(insn) UNSIGNED_EXTRACT(insn,14,0)

/* For ila */
#define DECODE_INSN_I18(insn) SIGNED_EXTRACT(insn,18,7)
#define DECODE_INSN_U18(insn) UNSIGNED_EXTRACT(insn,18,7)

/* For rotate and shift and generate control mask */
#define DECODE_INSN_I7(insn) SIGNED_EXTRACT(insn,7,14)
#define DECODE_INSN_U7(insn) UNSIGNED_EXTRACT(insn,7,14)

/* For float <-> int conversion */
#define DECODE_INSN_I8(insn)  SIGNED_EXTRACT(insn,8,14)
#define DECODE_INSN_U8(insn) UNSIGNED_EXTRACT(insn,8,14)

/* For hbr  */
#define DECODE_INSN_I9a(insn) ((SIGNED_EXTRACT(insn,2,23) << 7) | UNSIGNED_EXTRACT(insn,7,0))
#define DECODE_INSN_I9b(insn) ((SIGNED_EXTRACT(insn,2,14) << 7) | UNSIGNED_EXTRACT(insn,7,0))
#define DECODE_INSN_U9a(insn) ((UNSIGNED_EXTRACT(insn,2,23) << 7) | UNSIGNED_EXTRACT(insn,7,0))
#define DECODE_INSN_U9b(insn) ((UNSIGNED_EXTRACT(insn,2,14) << 7) | UNSIGNED_EXTRACT(insn,7,0))

extern void  spu_decode_insn      (unsigned int memaddr, unsigned int opcode, spu_instruction *insn);
extern char *spu_disassemble_insn (unsigned int memaddr, unsigned int insn);

} /* spu */
} /* arch */
} /* nc */