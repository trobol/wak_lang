#ifndef _WAK_LANG_LLVM_IR_INSTRUCTIONS_H
#define _WAK_LANG_LLVM_IR_INSTRUCTIONS_H
#include <stddef.h>

typedef enum {
	IR_TYPE_VOID,
	IR_TYPE_FLOAT,
	IR_TYPE_INT,
	IR_TYPE_BOOL
} IR_Type;

typedef enum {
	// control flow
	IR_INSTR_TYPE_RET,
	IR_INSTR_TYPE_BR,
	IR_INSTR_TYPE_CALL,

	// memory
	IR_INSTR_TYPE_ALLOCA,
	IR_INSTR_TYPE_LOAD,
	IR_INSTR_TYPE_STORE,

	// comparisons
	IR_INSTR_TYPE_ICMP,
	IR_INSTR_TYPE_FCMP
} IR_Instr_Type;

typedef struct IR_Val_Id { size_t id; } IR_Val_Id;
typedef struct IR_Label_Id { size_t id; } IR_Label_Id;


typedef struct IR_Instr_Ret {
	IR_Type type;
	IR_Val_Id val_index;
} IR_Instr_Ret;

typedef struct IR_Instr_Br {
	IR_Val_Id cond_index;
	IR_Label_Id iftrue;
	IR_Label_Id iffalse;
} IR_Instr_Br;
typedef struct IR_Instr_Call {
	IR_Type type;

} IR_Instr_Call;

typedef struct IR_Instr_Alloca {} IR_Instr_Alloca;
typedef struct IR_Instr_Load {} IR_Instr_Load;
typedef struct IR_Instr_Store {} IR_Instr_Store;

typedef struct IR_Instr_Icmp {} IR_Instr_Icmp;
typedef struct IR_Instr_Fcmp {} IR_Instr_Fcmp;


struct IR_Instr {
	IR_Instr_Type type;
	union {
		IR_Instr_Ret v_ret;
		IR_Instr_Br v_br;
		IR_Instr_Call v_call;

		IR_Instr_Alloca v_alloca;
		IR_Instr_Load v_load;
		IR_Instr_Store v_store;

		IR_Instr_Icmp v_icmp;
		IR_Instr_Fcmp v_fcmp;
	};
};

#endif