#ifndef INSIDECODEBAKE_H
#define INSIDECODEBAKE_H

#include <iostream>
#include <math.h>
#include <string>
//#include "infArray_fm_v0.h"
#include "sen_tr.h"
#include "time_rdtsc.h"

using namespace std;
using namespace freemem;

enum class insttype {
	IT_ADD_STACK_VARIABLE = 0,
	IT_SET_STACK_VARIABLE_CONST_1 = 1, // var = const
	IT_SET_STACK_VARIABLE_CONST_2 = 2,
	IT_SET_STACK_VARIABLE_CONST_4 = 3,
	IT_PUSH_A_CONST_1 = 4, // using
	IT_PUSH_A_CONST_2 = 5, // not using
	IT_PUSH_A_CONST_4 = 6, // using
	IT_PUSH_A_VARIABLE_1 = 7, // not using but later
	IT_PUSH_A_VARIABLE_2 = 8,
	IT_PUSH_A_VARIABLE_4 = 9,
	IT_PUSH_B_CONST_1 = 10, // using
	IT_PUSH_B_CONST_2 = 11, // not using
	IT_PUSH_B_CONST_4 = 12, // using
	IT_PUSH_B_VARIABLE_1 = 13, // not using but later
	IT_PUSH_B_VARIABLE_2 = 14,
	IT_PUSH_B_VARIABLE_4 = 15, // ? > seem good
	//IT_AXBY = 16,
	IT_AU_BYTE_ADD_A = 17,
	IT_AU_BYTE_ADD_B = 18,
	IT_AU_UBYTE_ADD_A = 19,
	IT_AU_UBYTE_ADD_B = 20,
	IT_AU_SHORT_ADD_A = 21,
	IT_AU_SHORT_ADD_B = 22,
	IT_AU_USHORT_ADD_A = 23,
	IT_AU_USHORT_ADD_B = 24,
	IT_AU_INT_ADD_A = 25,
	IT_AU_INT_ADD_B = 26,
	IT_AU_UINT_ADD_A = 27,
	IT_AU_UINT_ADD_B = 28,
	IT_AU_FLOAT_ADD_A = 29,
	IT_AU_FLOAT_ADD_B = 30,
	IT_AU_BYTE_MIN_A = 31,
	IT_AU_BYTE_MIN_B = 32,
	IT_AU_UBYTE_MIN_A = 33,
	IT_AU_UBYTE_MIN_B = 34,
	IT_AU_SHORT_MIN_A = 35,
	IT_AU_SHORT_MIN_B = 36,
	IT_AU_USHORT_MIN_A = 37,
	IT_AU_USHORT_MIN_B = 38,
	IT_AU_INT_MIN_A = 39,
	IT_AU_INT_MIN_B = 40,
	IT_AU_UINT_MIN_A = 41,
	IT_AU_UINT_MIN_B = 42,
	IT_AU_FLOAT_MIN_A = 43,
	IT_AU_FLOAT_MIN_B = 44,
	IT_AU_BYTE_MUL_A = 45,
	IT_AU_BYTE_MUL_B = 46,
	IT_AU_UBYTE_MUL_A = 47,
	IT_AU_UBYTE_MUL_B = 48,
	IT_AU_SHORT_MUL_A = 49,
	IT_AU_SHORT_MUL_B = 50,
	IT_AU_USHORT_MUL_A = 51,
	IT_AU_USHORT_MUL_B = 52,
	IT_AU_INT_MUL_A = 53,
	IT_AU_INT_MUL_B = 54,
	IT_AU_UINT_MUL_A = 55,
	IT_AU_UINT_MUL_B = 56,
	IT_AU_FLOAT_MUL_A = 57,
	IT_AU_FLOAT_MUL_B = 58,
	IT_AU_BYTE_DIV_A = 59,
	IT_AU_BYTE_DIV_B = 60,
	IT_AU_UBYTE_DIV_A = 61,
	IT_AU_UBYTE_DIV_B = 62,
	IT_AU_SHORT_DIV_A = 63,
	IT_AU_SHORT_DIV_B = 64,
	IT_AU_USHORT_DIV_A = 65,
	IT_AU_USHORT_DIV_B = 66,
	IT_AU_INT_DIV_A = 67,
	IT_AU_INT_DIV_B = 68,
	IT_AU_UINT_DIV_A = 69,
	IT_AU_UINT_DIV_B = 70,
	IT_AU_FLOAT_DIV_A = 71,
	IT_AU_FLOAT_DIV_B = 72,
	IT_AU_BYTE_PER_A = 73,
	IT_AU_BYTE_PER_B = 74,
	IT_AU_UBYTE_PER_A = 75,
	IT_AU_UBYTE_PER_B = 76,
	IT_AU_SHORT_PER_A = 77,
	IT_AU_SHORT_PER_B = 78,
	IT_AU_USHORT_PER_A = 79,
	IT_AU_USHORT_PER_B = 80,
	IT_AU_INT_PER_A = 81,
	IT_AU_INT_PER_B = 82,
	IT_AU_UINT_PER_A = 83,
	IT_AU_UINT_PER_B = 84,
	IT_LU_BOOL_AND_A = 85,
	IT_LU_BOOL_AND_B = 86,
	IT_LU_BOOL_OR_A = 87,
	IT_LU_BOOL_OR_B = 88,
	IT_LU_BOOL_NOT_A = 89,
	IT_LU_BOOL_NOT_B = 90,
	IT_CM_BOOL_SAME_A = 91,
	IT_CM_BOOL_SAME_B = 92,
	IT_CM_BOOL_NOTSAME_A = 93,
	IT_CM_BOOL_NOTSAME_B = 94,
	IT_CM_BOOL_BYTE_LBIG_A = 95,
	IT_CM_BOOL_BYTE_LBIG_B = 96,
	IT_CM_BOOL_UBYTE_LBIG_A = 97,
	IT_CM_BOOL_UBYTE_LBIG_B = 98,
	IT_CM_BOOL_SHORT_LBIG_A = 99,
	IT_CM_BOOL_SHORT_LBIG_B = 100,
	IT_CM_BOOL_USHORT_LBIG_A = 101,
	IT_CM_BOOL_USHORT_LBIG_B = 102,
	IT_CM_BOOL_INT_LBIG_A = 103,
	IT_CM_BOOL_INT_LBIG_B = 104,
	IT_CM_BOOL_UINT_LBIG_A = 105,
	IT_CM_BOOL_UINT_LBIG_B = 106,
	IT_CM_BOOL_FLOAT_LBIG_A = 107,
	IT_CM_BOOL_FLOAT_LBIG_B = 108,
	IT_CM_BOOL_BYTE_LBIGSAME_A = 109,
	IT_CM_BOOL_BYTE_LBIGSAME_B = 110,
	IT_CM_BOOL_UBYTE_LBIGSAME_A = 111,
	IT_CM_BOOL_UBYTE_LBIGSAME_B = 112,
	IT_CM_BOOL_SHORT_LBIGSAME_A = 113,
	IT_CM_BOOL_SHORT_LBIGSAME_B = 114,
	IT_CM_BOOL_USHORT_LBIGSAME_A = 115,
	IT_CM_BOOL_USHORT_LBIGSAME_B = 116,
	IT_CM_BOOL_INT_LBIGSAME_A = 117,
	IT_CM_BOOL_INT_LBIGSAME_B = 118,
	IT_CM_BOOL_UINT_LBIGSAME_A = 119,
	IT_CM_BOOL_UINT_LBIGSAME_B = 120,
	IT_CM_BOOL_FLOAT_LBIGSAME_A = 121,
	IT_CM_BOOL_FLOAT_LBIGSAME_B = 122,
	IT_CM_BOOL_BYTE_RBIG_A = 123,
	IT_CM_BOOL_BYTE_RBIG_B = 124,
	IT_CM_BOOL_UBYTE_RBIG_A = 125,
	IT_CM_BOOL_UBYTE_RBIG_B = 126,
	IT_CM_BOOL_SHORT_RBIG_A = 127,
	IT_CM_BOOL_SHORT_RBIG_B = 128,
	IT_CM_BOOL_USHORT_RBIG_A = 129,
	IT_CM_BOOL_USHORT_RBIG_B = 130,
	IT_CM_BOOL_INT_RBIG_A = 131,
	IT_CM_BOOL_INT_RBIG_B = 132,
	IT_CM_BOOL_UINT_RBIG_A = 133,
	IT_CM_BOOL_UINT_RBIG_B = 134,
	IT_CM_BOOL_FLOAT_RBIG_A = 135,
	IT_CM_BOOL_FLOAT_RBIG_B = 136,
	IT_CM_BOOL_BYTE_RBIGSAME_A = 137,
	IT_CM_BOOL_BYTE_RBIGSAME_B = 138,
	IT_CM_BOOL_UBYTE_RBIGSAME_A = 139,
	IT_CM_BOOL_UBYTE_RBIGSAME_B = 140,
	IT_CM_BOOL_SHORT_RBIGSAME_A = 141,
	IT_CM_BOOL_SHORT_RBIGSAME_B = 142,
	IT_CM_BOOL_USHORT_RBIGSAME_A = 143,
	IT_CM_BOOL_USHORT_RBIGSAME_B = 144,
	IT_CM_BOOL_INT_RBIGSAME_A = 145,
	IT_CM_BOOL_INT_RBIGSAME_B = 146,
	IT_CM_BOOL_UINT_RBIGSAME_A = 147,
	IT_CM_BOOL_UINT_RBIGSAME_B = 148,
	IT_CM_BOOL_FLOAT_RBIGSAME_A = 149,
	IT_CM_BOOL_FLOAT_RBIGSAME_B = 150,
	IT_IF = 151,
	IT_JMP = 152,
	IT_FUNC = 153,
	IT_PARAM_1 = 154,
	IT_PARAM_2 = 155,
	IT_PARAM_4 = 156,
	IT_RETURN = 157,
	IT_EXIT = 158,
	IT_PUSH_TO_A_FROM_ADDRESS_OF_VARIABLE_ID = 159,
	IT_PUSH_TO_B_FROM_ADDRESS_OF_VARIABLE_ID = 160,
	IT_PUSH_TO_LA_FROM_A = 161,
	IT_FUNCJMP = 162,
	IT_CASTING_A = 163,
	IT_CASTING_B = 164,
	IT_PUSH_A_FROM_VALUE_OF_A = 165,
	IT_PUSH_B_FROM_VALUE_OF_B = 166,
	IT_DBG_A = 167,
	IT_INP_A_PTR = 168,
	IT_PUSH_TO_VALUE_OF_LA_FROM_A_1 = 169,
	IT_PUSH_TO_VALUE_OF_LA_FROM_A_2 = 170,
	IT_PUSH_TO_VALUE_OF_LA_FROM_A_4 = 171,
	IT_SET_A_CONST_STRING = 172,
	IT_SET_B_CONST_STRING = 173,
	IT_POP_A = 174,
	IT_POP_B = 175,
	IT_POP_AB = 176,
	PARAM_N_COPY_BY_ADDRESS = 177,
	PUSH_A_GLOBAL_VARIABLE_ADDRESS = 178,
	PUSH_B_GLOBAL_VARIABLE_ADDRESS = 179,
	PUSH_A_FROM_B = 180,
	PUSH_B_FROM_A = 181,
	SET_ADDRESS_LA_FROM_ADRESS_A_N = 182,
	IT_PUSH_A_CONST_INDEX_ADDR = 183,
	IT_PUSH_B_CONST_INDEX_ADDR = 184,
	PARAM_8 = 185,
	IT_PUSH_TO_VALUE_OF_LA_FROM_A_8 = 186,
	EXTENSION_INST = 255
};

enum class casttype {
	CAST_BYTE_TO_SHORT,
	CAST_BYTE_TO_USHORT,
	CAST_BYTE_TO_INT,
	CAST_BYTE_TO_UINT,
	CAST_BYTE_TO_FLOAT,
	CAST_UBYTE_TO_FLOAT,
	CAST_SHORT_TO_BYTE,
	CAST_SHORT_TO_INT,
	CAST_SHORT_TO_FLOAT,
	CAST_USHORT_TO_FLOAT,
	CAST_INT_TO_BYTE,
	CAST_INT_TO_SHORT,
	CAST_INT_TO_FLOAT,
	CAST_UINT_TO_FLOAT,
	CAST_FLOAT_TO_BYTE,
	CAST_FLOAT_TO_UBYTE,
	CAST_FLOAT_TO_SHORT,
	CAST_FLOAT_TO_USHORT,
	CAST_FLOAT_TO_INT,
	CAST_FLOAT_TO_UINT,
};

enum class dbgtype {
	DBG_A_BYTE,
	DBG_A_UBYTE,
	DBG_A_SHORT,
	DBG_A_USHORT,
	DBG_A_INT,
	DBG_A_UINT,
	DBG_A_FLOAT,
	DBG_A_BOOL,
	DBG_A_STRING,
};

enum class inptype {
	INP_BYTE,
	INP_UBYTE,
	INP_SHORT,
	INP_USHORT,
	INP_INT,
	INP_UINT,
	INP_FLOAT,
	INP_BOOL,
	INP_STRING,
};

bool cd_eqoper(char *str)
{
	if (str[1] == '=')
		return true;
	else
		return false;
}

bool bCanBeTextblock(fmlcstr &a)
{
	bool num = false;
	bool oper = false;
	if (a.at(0) == '\"')
	{
		if (a.size() > 1 && a.at(a.size() - 1) == '\"')
		{
			return true;
		}
		else
			return false;
	}

	if (a.size() == 2 && (a[0] == '&' && a[1] == '&'))
	{
		return true;
	}

	if (a.size() == 2 && (a[0] == '&' && a[1] == '&'))
	{
		return true;
	}

	//heloo!
	bool isdot = false;
	bool isn = false;
	if (a.at(0) == '-')
	{
		for (int i = 1; i < (int)a.size(); ++i)
		{
			char c = a.at(i);
			bool n1 = ('0' <= c && c <= '9');
			if (!n1)
			{
				if ((isn && isdot == false) && c == '.')
				{
					isdot = true;
				}
				else
				{
					return false;
				}
			}
			else
			{
				isn = true;
			}
		}
		return true;
	}

	for (int i = 0; i < (int)a.size(); i++)
	{
		char c = a.at(i);
		bool b1 = 33 <= c && c <= 126;
		if (b1 == false)
		{
			return false;
		}

		bool n1 = (('0' <= c && c <= '9') || ('a' <= c && c <= 'z')) || (('A' <= c && c <= 'Z') || c == '_');
		if (n1)
		{
			if (oper == true)
			{
				return false;
			}
			num = true;
		}
		else
		{
			if (num == true)
			{
				return false;
			}
			if (oper == true)
			{
				return false;
			}
			oper = true;
		}
	}
	return true;
}

bool bCanBeTextblock_notAllowNeg(fmlcstr& a)
{
	bool num = false;
	bool oper = false;
	if (a.at(0) == '\"')
	{
		if (a.size() > 1 && a.at(a.size() - 1) == '\"')
		{
			return true;
		}
		else
			return false;
	}

	if (a.size() == 2 && (a[0] == '&' && a[1] == '&'))
	{
		return true;
	}

	if (a.size() == 2 && (a[0] == '&' && a[1] == '&'))
	{
		return true;
	}

	//heloo!
	bool isdot = false;
	bool isn = false;
	if (a.at(0) == '-') {
		if (a.size() == 1) {
			return true;
		}
		else return false;
	}


	for (int i = 0; i < (int)a.size(); i++)
	{
		char c = a.at(i);
		bool b1 = 33 <= c && c <= 126;
		if (b1 == false)
		{
			return false;
		}

		bool n1 = (('0' <= c && c <= '9') || ('a' <= c && c <= 'z')) || (('A' <= c && c <= 'Z') || c == '_');
		if (n1)
		{
			if (oper == true)
			{
				return false;
			}
			num = true;
		}
		else
		{
			if (num == true)
			{
				return false;
			}
			if (oper == true)
			{
				return false;
			}
			oper = true;
		}
	}
	return true;
}

typedef enum class TBT
{
	_value_bool = 0,
	_value_integer = 1,
	_value_float = 2,
	_value_char = 3,
	_value_str = 4,
	_str = 5,
	_operation = 6,
	_memory = 7,
	_null = 8
} TBT;

TBT DecodeTextBlock(fmlcstr& t)
{
	if (t == "true" || t == "false")
	{
		return TBT::_value_bool;
	}

	if (t.size() == 3 && (t.at(0) == '\'' && t.at(2) == '\''))
	{
		return TBT::_value_char;
	}
	else if (t.size() == 4 && ((t.at(0) == '\'' && t.at(3) == '\'') && t.at(1) == '\\'))
	{
		return TBT::_value_char;
	}

	if (t.size() >= 3 && (t.at(0) == '\"' && t.at(t.size() - 1) == '\"'))
	{
		return TBT::_value_str;
	}

	int* intarray = new int[t.size()];
	bool dot = false;
	bool num = true;

	for (int i = 0; i < (int)t.size(); i++)
	{
		char c = t.at(i);

		if ('0' <= c && c <= '9')
		{
			intarray[i] = 1;
			continue;
		}

		if ((('A' <= c && c <= 'Z') || ('a' <= c && c <= 'z')) || c == '_')
		{
			intarray[i] = 2;
			num = false;
			continue;
		}

		//음수 표현
		if ((i == 0 && c == '-') && t.size() > 1)
		{
			intarray[i] = 1;
			continue;
		}

		if (((num && dot == false) && c == '.') && i != 0)
		{
			intarray[i] = 1;
			dot = true;
			continue;
		}

		intarray[i] = 3;
		num = false;
	}

	bool bnum = true;
	bool bstr = true;
	bool boper = true;
	bool firstnum = false;
	if (intarray[0] == 1)
	{
		firstnum = true;
	}
	for (int i = 0; i < (int)t.size(); i++)
	{
		if (intarray[i] != 1)
			bnum = false;

		if (firstnum)
			bstr = false;

		if (intarray[i] != 3)
			boper = false;

		if (intarray[i] == 3)
			bstr = false;
	}

	delete[] intarray;

	if (bnum)
	{
		if (dot)
		{
			return TBT::_value_float;
		}
		else
		{
			return TBT::_value_integer;
		}
	}
	if (bstr)
		return TBT::_str;
	if (boper)
		return TBT::_operation;
	else
	{
		if (t.at(0) == '{')
		{
			return TBT::_memory;
		}
		return TBT::_null;
	}
}

fmlcstr *GetCodeTXT(const char *filename, FM_System0 *fm)
{
	FILE* fp;
	fopen_s(&fp, filename, "rt");
	if (fp != nullptr)
	{
		fmlcstr *codetxt = (fmlcstr *)fm->_New(sizeof(fmlcstr), true);
		codetxt->NULLState();
		codetxt->Init(10, false);
		int max = 0;
		fseek(fp, 0, SEEK_END);
		max = ftell(fp);
		fclose(fp);

		int stack = 0;
		fopen_s(&fp, filename, "rt");
		int k = 0;
		while (k < max)
		{
			char c;
			c = fgetc(fp);
			if (c == '/')
			{
				stack += 1;
			}
			else
			{
				stack = 0;
			}

			if (stack == 2)
			{
				codetxt->pop_back();
				int mm = 0;
				while (c != '\n' && k + mm < max)
				{
					mm += 1;
					c = fgetc(fp);
				}
				max -= mm + 1;
				continue;
			}
			codetxt->push_back(c);
			k++;
		}
		return codetxt;
	}
	else
	{
		printf("[ERROR] : %s file is nor exist.", filename);
		return nullptr;
	}
}

enum class codeKind
{
	ck_addVariable,
	ck_setVariable,
	ck_get,
	ck_if,
	ck_while,
	ck_blocks,
	ck_addFunction,
	ck_useFunction,
	ck_returnInFunction,
	ck_addStruct,
	ck_break,
	ck_continue,
	ck_addsetVariable,
	ck_none
};

struct code_sen
{
	char** sen;
	int maxlen;
	codeKind ck;
	fmvecarr<int*>* codeblocks = nullptr;

	int start_line = 0;
	int end_line = 0;
	int codeline = 0;

	code_sen() :
		start_line(0),
		end_line(0),
		codeblocks(nullptr),
		sen(nullptr),
		maxlen(0),
		codeline(0),
		ck(codeKind::ck_none)
	{

	}
};

void dbg_codesen(code_sen* cs, ostream& ofs)
{
	switch (cs->ck)
	{
	case codeKind::ck_addVariable:
		ofs << "add var : ";
		break;
	case codeKind::ck_setVariable:
		ofs << "set var : ";
		break;
	case codeKind::ck_if:
		ofs << "if_ sen : ";
		break;
	case codeKind::ck_while:
		ofs << "while__ : ";
		break;
	case codeKind::ck_blocks:
		ofs << "block__ : ";
		break;
	case codeKind::ck_addFunction:
		ofs << "addfunc : ";
		break;
	case codeKind::ck_useFunction:
		ofs << "usefunc : ";
		break;
	case codeKind::ck_returnInFunction:
		ofs << "return_ : ";
		break;
	case codeKind::ck_addStruct:
		ofs << "struct__ : ";
		break;
	case codeKind::ck_break:
		ofs << "break__ : ";
		break;
	case codeKind::ck_continue:
		ofs << "continue: ";
		break;
	case codeKind::ck_addsetVariable:
		ofs << "adsetvar: ";
		break;
	default:
		break;
	}

	if (cs->ck != codeKind::ck_blocks)
	{
		for (int i = 0; i < cs->maxlen; ++i)
		{
			ofs << cs->sen[i] << " ";
		}
		ofs << endl;
	}
	else
	{
		ofs << "{" << endl;
		for (int i = 0; i < cs->codeblocks->size(); ++i)
		{
			dbg_codesen(reinterpret_cast<code_sen*>(cs->codeblocks->at(i)), ofs);
		}
		ofs << "closed_ : }" << endl;
	}
}

struct type_data
{
	fmlcstr name;

	int typesiz;

	char typetype = 'b';
	// b - basictype / s - struct type
	// p - pointer / a - array

	int* structptr = nullptr;
	// a or p - it is subtype
	// s - struct_data*
};

struct NamingData
{
	char* name;
	type_data* td;
	int add_address;
};

struct operator_data
{
	const char* symbol;
	char mod = 'f'; // f : front / o - asmoper
	int startop = 0;
	int endop = 0;
};

enum class blockstate
{
	bs_if,
	bs_while,
	bs_function,
	bs_struct,
	bs_else
};

struct block_data
{
	byte8* start_pc;
	fmvecarr<NamingData> variable_data;
	int add_address_up = 0;
	blockstate bs;
	uint64_t parameter[5] = {};
	fmvecarr<int>* breakpoints;
	fmvecarr<int>* continuepoints;

	bool ifin = false;
	int lastcsindex = 0;
	int presentcsindex = 0;
};

struct func_data
{
	fmlcstr name;
	byte8* start_pc;
	fmvecarr<NamingData> param_data; // save sizeoftype
	type_data* returntype;
};

struct struct_data
{
	fmlcstr name;
	fmvecarr<NamingData> member_data;
};

struct temp_mem
{
	fmvecarr<byte8> mem; // 32
	type_data* valuetype_detail; // 8
	int valuetype = 0; // 4
	char registerMod = 'A'; // A, B, X, Y ...
	char isValue = true; // true > value operation state ; false > ptr operation state
};

enum class casting_type
{
	byte_to_short = 0,
	byte_to_ushort = 1,
	byte_to_int = 2,
	byte_to_uint = 3,
	byte_to_float = 4,
	ubyte_to_float = 5,
	short_to_byte = 6,
	short_to_int = 7,
	short_to_float = 8,
	ushort_to_float = 9,
	int_to_byte = 10,
	int_to_short = 11,
	int_to_float = 12,
	uint_to_float = 13,
	float_to_byte = 14,
	float_to_ubyte = 15,
	float_to_short = 16,
	float_to_ushort = 17,
	float_to_int = 18,
	float_to_uint = 19,
	nocasting = 20
};

casting_type get_cast_type(int type0, int type1)
{
	if (type0 == 0)
	{
		switch (type1)
		{
		case 2:
			return casting_type::byte_to_short;
		case 3:
			return casting_type::byte_to_ushort;
		case 4:
			return casting_type::byte_to_int;
		case 5:
			return casting_type::byte_to_uint;
		case 6:
			return casting_type::byte_to_float;
		default:
			return casting_type::nocasting;
		}
	}
	else if (type0 == 1)
	{
		switch (type1)
		{
		case 6:
			return casting_type::ubyte_to_float;
		default:
			return casting_type::nocasting;
		}
	}
	else if (type0 == 2)
	{
		switch (type1)
		{
		case 0:
			return casting_type::short_to_byte;
		case 4:
			return casting_type::short_to_int;
		case 6:
			return casting_type::short_to_float;
		default:
			return casting_type::nocasting;
		}
	}
	else if (type0 == 3)
	{
		switch (type1)
		{
		case 6:
			return casting_type::ushort_to_float;
		default:
			return casting_type::nocasting;
		}
	}
	else if (type0 == 4)
	{
		switch (type1)
		{
		case 0:
			return casting_type::int_to_byte;
		case 2:
			return casting_type::int_to_short;
		case 6:
			return casting_type::int_to_float;
		default:
			return casting_type::nocasting;
		}
	}
	else if (type0 == 5)
	{
		switch (type1)
		{
		case 6:
			return casting_type::uint_to_float;
		default:
			return casting_type::nocasting;
		}
	}
	else if (type0 == 6)
	{
		switch (type1)
		{
		case 0:
			return casting_type::float_to_byte;
		case 1:
			return casting_type::float_to_ubyte;
		case 2:
			return casting_type::float_to_short;
		case 3:
			return casting_type::float_to_ushort;
		case 4:
			return casting_type::float_to_int;
		case 5:
			return casting_type::float_to_uint;
		default:
			return casting_type::nocasting;
		}
	}
	return casting_type::nocasting;
}

struct instruct_data
{
	char name[64] = {};
	int param_num = 0;
	int param_typesiz[4] = {};
};

typedef void (*exInst)(int*); // int -> ICB_Context

struct ICB_Extension {
	fmvecarr<type_data*> exstructArr;
	fmvecarr<func_data*> exfuncArr;
	void Release();
};

enum class ICL_FLAG {
	ICB_StaticInit = 0,
	Create_New_ICB_Extension_Init = 1,
	BakeCode_AddTextBlocks = 2,
	BakeCode_ScanStructTypes = 3,
	BakeCode_AddStructTypes = 4,
	BakeCode_ScanCodes = 5,
	BakeCode_GlobalMemoryInit = 6,
	BakeCode_CompileCodes = 7,
	Create_New_ICB_Context = 8,
	Create_New_ICB_Extension_Init__Bake_Extension = 9,
	BakeCode_CompileCodes__add_var = 10,
	BakeCode_CompileCodes__set_var = 11,
	BakeCode_CompileCodes__if__sen = 12,
	BakeCode_CompileCodes__while__ = 13,
	BakeCode_CompileCodes__block__ = 14,
	BakeCode_CompileCodes__addfunc = 15,
	BakeCode_CompileCodes__usefunc = 16,
	BakeCode_CompileCodes__return_ = 17,
	BakeCode_CompileCodes__struct__ = 18,
	BakeCode_CompileCodes__break__ = 19,
	BakeCode_CompileCodes__continue = 20,
	BakeCode_CompileCodes__adsetvar = 21,
};

#define ICB_ERR_CHECK(A) if(curErrMsg[0] != 0) goto A;

class InsideCode_Bake
{
private:
	bool current_if_is_multiple = false;

public:
	// compile var
	fmvecarr<char*> allcode_sen;
	static word_base_sen_sys wbss;
	fmvecarr<code_sen*>* csarr;
	// infArray < code_sen > sen_arr;

	//code memory
	uint32_t max_mem_byte = 40960; // 40KB
	byte8* mem = nullptr;
	fmvecarr<byte8> init_datamem;

	fmvecarr<func_data*> functions;
	fmvecarr<type_data*> types;
	fmvecarr<block_data*> blockstack;
	fmvecarr<NamingData*> globalVariables;

	block_data nextbd;
	struct_data* nextsd;

	static instruct_data inst_meta[256];

	int writeup = 0;
	int datamem_up = 0;

	static constexpr int basictype_max = 8;
	static type_data basictype[basictype_max];

	static constexpr int basicoper_max = 19;
	static operator_data basicoper[basicoper_max];

	static ofstream icl; // icb log
	static uint32_t icl_optionFlag;

	fmvecarr<unsigned int> codeLineVec;
	fmvecarr<unsigned int> codeLineVec_Word;

	fmlcstr curErrMsg;
	unsigned int currentCodeLine = 0;

	bool able_to_execute = false;
	int changeHash = 0;

	fmvecarr<ICB_Extension*> extension; // 확장코드

	void UpdateErrMsg(int errorcode, const char* message, char* param) {
		//_CrtDbgBreak();
		curErrMsg.up = 0;
		curErrMsg.at(0) = 0;

		curErrMsg = (char*)"[COMPILE ERROR ";

		string numstr;
		numstr = to_string(errorcode);
		for (int i = 0; i < numstr.size(); ++i) {
			curErrMsg.push_back(numstr[i]);
		}
		curErrMsg.push_back(']');
		curErrMsg.push_back('_');
		curErrMsg.push_back('l'); curErrMsg.push_back('i'); curErrMsg.push_back('n'); curErrMsg.push_back('e');
		numstr = to_string(currentCodeLine);
		for (int i = 0; i < numstr.size(); ++i) {
			curErrMsg.push_back(numstr[i]);
		}
		curErrMsg.push_back(' '); curErrMsg.push_back(':'); curErrMsg.push_back(' ');

		int mlen = strlen(message);
		for (int i = 0; i < mlen; ++i) {
			if (message[i] == '%') {
				int plen = strlen(param);
				for (int k = 0; k < plen; ++k) {
					curErrMsg.push_back(param[k]);
				}
				++i;
			}
			curErrMsg.push_back(message[i]);
		}
	}

	fmlcstr* GetCodeTXT(const char* filename)
	{
		codeLineVec.push_back(0);
		char comment_mod = '/';
		FILE* fp;
		fopen_s(&fp, filename, "rt");
		if (fp)
		{
			fmlcstr* codetxt = (fmlcstr*)fm->_New(sizeof(fmlcstr), true);
			codetxt->NULLState();
			codetxt->Init(10, false);
			int max = 0;
			fseek(fp, 0, SEEK_END);
			max = ftell(fp);
			fclose(fp);

			int stack = 0;
			fopen_s(&fp, filename, "rt");
			int k = 0;
			while (k < max)
			{
				char c;
				c = fgetc(fp);

				//line check
				if (c == '\n') {
					codeLineVec.push_back(k + 1);
				}

				if (c == '/')
				{
					stack += 1;
				}
				else if (stack == 1 && c == '*') {
					stack += 1;
					comment_mod = '*';
				}
				else
				{
					stack = 0;
				}

				if (stack == 2)
				{
					if (comment_mod == '/') {
						codetxt->pop_back();
						int mm = 0;
						while (c != '\n' && k + mm < max)
						{
							mm += 1;
							c = fgetc(fp);
							//line check
							if (c == '\n') {
								codeLineVec.push_back(codeLineVec.last());
							}
						}
						max -= mm + 1;
						continue;
					}
					else if (comment_mod == '*') {
						codetxt->pop_back();
						int mm = 0;
						char saveC = 0;
						while ((saveC != '*' || c != '/') && k + mm < max)
						{
							mm += 1;
							saveC = c;
							c = fgetc(fp);
							//line check
							if (c == '\n') {
								codeLineVec.push_back(codeLineVec.last());
							}
						}
						max -= mm + 1;
						comment_mod = '/';
						continue;
					}
				}
				codetxt->push_back(c);
				k++;
			}
			return codetxt;
		}
		else
		{
			UpdateErrMsg(0, "<%s> file is not exist.", (char*)filename);
			//printf("[ERROR] : %s file is not exist.", filename);
			return nullptr;
		}
	}

	fmlcstr* GetCode_Lines(fmvecarr<fmlwstr>* lines) {
		char comment_mod = '/';
		int stack = 0;
		bool iscomment = false;
		if (lines != nullptr)
		{
			fmlcstr* codetxt = (fmlcstr*)fm->_New(sizeof(fmlcstr), true);
			codetxt->NULLState();
			codetxt->Init(10, false);
			for (int i = 0; i < lines->size(); ++i)
			{
				for (int k = 0; k < lines->at(i).size(); ++k) {
					wchar_t cc = lines->at(i)[k];
					char c = (char)cc;
					codetxt->push_back(c);
				}
				codetxt->push_back('\n');
			}

			for (int k = 0; k < codetxt->size(); ++k)
			{
				char c = codetxt->at(k);
				//line check
				if (c == '\n') {
					codeLineVec.push_back(k + 1);
				}

				if (c == '/')
				{
					stack += 1;
				}
				else if (stack == 1 && c == '*') {
					stack += 1;
					comment_mod = '*';
				}
				else
				{
					stack = 0;
				}

				if (stack == 2)
				{
					if (comment_mod == '/') {
						codetxt->erase(k-1);
						while (c != '\n' && k - 1 < codetxt->size())
						{
							c = codetxt->at(k - 1);
							codetxt->erase(k - 1);
							//c = fgetc(fp);
							//line check
							if (c == '\n') {
								codeLineVec.push_back(codeLineVec.last());
							}
						}
						k -= 2;
						//max -= mm + 1;
						continue;
					}
					else if (comment_mod == '*') {
						codetxt->erase(k - 1);
						char saveC = 0;
						while ((saveC != '*' || c != '/') && k - 1 < codetxt->size())
						{
							saveC = c;
							c = codetxt->at(k - 1);
							codetxt->erase(k - 1);
							//line check
							if (c == '\n') {
								codeLineVec.push_back(codeLineVec.last());
							}
						}
						comment_mod = '/';
						k -= 2;
						continue;
					}
				}
			}

			return codetxt;
		}

		return nullptr;
	}

	static void ReleaseCodeSen(code_sen* cs) {
		if ((cs->ck == codeKind::ck_blocks || cs->ck == codeKind::ck_addStruct) && cs->codeblocks != nullptr) {
			for (int i = 0; i < cs->codeblocks->size(); ++i) {
				code_sen* ccs = (code_sen*)cs->codeblocks->at(i);
				ReleaseCodeSen(ccs);
				cs->codeblocks->at(i) = nullptr;
			}
			cs->codeblocks->release();
			fm->_Delete((byte8*)cs->codeblocks, sizeof(fmvecarr<int*>));
			cs->codeblocks = nullptr;
		}

		if (cs->maxlen != 0 && cs->ck != codeKind::ck_blocks) {
			for (int i = 0; i < cs->maxlen; ++i) {
				cs->sen[i] = nullptr;
			}
			fm->_Delete((byte8*)cs->sen, cs->maxlen * sizeof(char*));
			cs->sen = nullptr;
		}

		cs->maxlen = 0;
		cs->ck = codeKind::ck_none;
		cs->start_line = 0;
		cs->end_line = 0;

		fm->_Delete((byte8*)cs, sizeof(code_sen));
	}

	static void ReleaseTypeData(type_data* td) {
		if (td->structptr != nullptr && td->typetype != 'b') {
			/*
			struct_data* sd = (struct_data*)td->structptr;
			sd->member_data.release();
			sd->member_data.NULLState();
			sd->name.release();
			sd->name.NULLState();
			*/
			if (td->typetype == 's') {
				struct_data* sd = (struct_data*)td->structptr;
				sd->member_data.release();
				sd->member_data.NULLState();
				sd->name.release();
				sd->name.NULLState();
				td->structptr = nullptr;
			}
			else {
				td->structptr = nullptr;
			}

			td->name.release();
			td->name.NULLState();
		}
		fm->_Delete((byte8*)td, sizeof(type_data));
	}

	void Release(bool all = true) {
		/*
	fmvecarr<char *> allcode_sen;
	fmvecarr<code_sen *> *csarr;
	uint32_t max_mem_byte = 40960; // 40KB
	byte8 *mem = nullptr;
	fmvecarr<byte8> init_datamem;
	fmvecarr<func_data *> functions;
	fmvecarr<type_data *> types;
	fmvecarr<block_data *> blockstack;
	fmvecarr<NamingData *> globalVariables;
	block_data nextbd;
	struct_data *nextsd;
	fmvecarr<ICB_Extension*> extension; // 확장코드
		*/

		codeLineVec.release();
		codeLineVec_Word.release();

		if (all) {
			curErrMsg.release();
		}

		for (int i = 0; i < allcode_sen.size(); ++i) {
			allcode_sen.at(i) = nullptr;
		}
		allcode_sen.release();
		allcode_sen.NULLState();

		if (csarr != nullptr) {
			for (int i = 0; i < csarr->size(); ++i) {
				code_sen* cs = csarr->at(i);
				ReleaseCodeSen(cs);
				fm->_Delete((byte8*)cs, sizeof(code_sen));
				csarr->at(i) = nullptr;
			}
			csarr->release();
			csarr->NULLState();
			fm->_Delete((byte8*)csarr, sizeof(fmvecarr<code_sen*>));
			csarr = nullptr;
		}

		if (init_datamem.maxsize != 0) {
			init_datamem.release();
			init_datamem.NULLState();
		}

		if (types.size() > basictype_max) {
			int si = basictype_max;
			for (int i = si; i < types.size(); ++i) {
				type_data* td = types.at(i);
				ReleaseTypeData(td);
				fm->_Delete((byte8*)td, sizeof(type_data));
				types.at(i) = nullptr;
			}
		}
		types.release();
		types.NULLState();

		if (functions.size() != 0) {
			for (int i = 0; i < functions.size(); ++i) {
				func_data* fd = functions.at(i);
				fd->start_pc = nullptr;
				/*
				for(int k=0;k<fd->param_data.size();++k){
					NamingData nd = fd->param_data.at(k);
					nd.add_address = 0;

					//prerelease in allcodesen release
					//nd.name

					//pre release in type release
					//ReleaseTypeData(nd.td);
					//fm->_Delete((byte8*)nd.td, sizeof(type_data));
				}
				*/
				//pre release in allcodesen
				fd->name.release();
				fd->name.NULLState();
				fd->param_data.release();
				fd->param_data.NULLState();
				fd->returntype = nullptr;
				fd->start_pc = nullptr;
				fm->_Delete((byte8*)fd, sizeof(func_data));
				functions.at(i) = nullptr;
			}
		}
		functions.release();
		functions.NULLState();

		for (int i = 0; i < blockstack.size(); ++i) {
			block_data* bd = blockstack.at(i);
			bd->start_pc = nullptr;
			bd->breakpoints->release();
			bd->breakpoints->NULLState();
			fm->_Delete((byte8*)bd->breakpoints, sizeof(fmvecarr<int>));
			bd->breakpoints = nullptr;
			bd->continuepoints->release();
			bd->continuepoints->NULLState();
			fm->_Delete((byte8*)bd->continuepoints, sizeof(fmvecarr<int>));
			bd->continuepoints = nullptr;
			bd->variable_data.release();
			bd->variable_data.NULLState();
			fm->_Delete((byte8*)bd, sizeof(block_data));
			blockstack.at(i) = nullptr;
		}
		blockstack.release();
		blockstack.NULLState();

		for (int i = 0; i < globalVariables.size(); ++i) {
			NamingData* nd = globalVariables.at(i);
			fm->_Delete((byte8*)nd, sizeof(NamingData));
			globalVariables.at(i) = nullptr;
		}
		globalVariables.release();
		globalVariables.NULLState();

		nextbd.start_pc = nullptr;
		if (nextbd.breakpoints != nullptr) {
			nextbd.breakpoints->release();
			nextbd.breakpoints->NULLState();
			fm->_Delete((byte8*)nextbd.breakpoints, sizeof(fmvecarr<int>));
			nextbd.breakpoints = nullptr;
		}

		if (nextbd.continuepoints != nullptr) {
			nextbd.continuepoints->release();
			nextbd.continuepoints->NULLState();
			fm->_Delete((byte8*)nextbd.continuepoints, sizeof(fmvecarr<int>));
			nextbd.continuepoints = nullptr;
		}

		nextbd.variable_data.release();
		nextbd.variable_data.NULLState();

		if (nextsd != nullptr) {
			nextsd->member_data.release();
			nextsd->member_data.NULLState();
			fm->_Delete((byte8*)nextsd, sizeof(struct_data));
		}

		extension.release();
		extension.NULLState();
	}

	static void SetICLFlag(ICL_FLAG flag, bool enable) {
		if (enable) {
			uint32_t temp = 1 << (unsigned int)flag;
			icl_optionFlag = icl_optionFlag | (temp);
		}
		else {
			uint32_t temp = ~(1 << (unsigned int)flag);
			icl_optionFlag = icl_optionFlag & (temp);
		}
	}

	static bool GetICLFlag(ICL_FLAG flag) {
		uint32_t temp = 1 << (unsigned int)flag;
		return icl_optionFlag & temp;
	}

	void release_tempmem(temp_mem* ptr)
	{
		if (ptr == nullptr) return;
		ptr->mem.release();
		fm->_Delete((byte8*)ptr, sizeof(temp_mem));
		// type_data memeory management reqired.. but how?
	}

	static operator_data create_oper(const char* symbo, char mo, int starto, int endo)
	{
		ofstream& icl = InsideCode_Bake::icl;
		bool icldetail = InsideCode_Bake::GetICLFlag(ICL_FLAG::ICB_StaticInit);
		if (icldetail) icl << "ICB_StaticInit Create Operation : [" << symbo << "]...";
		operator_data op;
		op.symbol = symbo;
		op.mod = mo;
		op.startop = starto;
		op.endop = endo;
		if (icldetail) icl << "finish" << endl;
		return op;
	}

	type_data* isCanPushType(type_data* td) {
		for (int i = 0; i < types.size(); ++i) {
			type_data* ttd = types.at(i);
			if (strcmp(ttd->name.c_str(), td->name.c_str()) == 0) {
				return ttd;
			}
		}

		for (int i = 0; i < extension.size(); ++i)
		{
			ICB_Extension* ext = extension.at(i);
			for (int k = 0; k < ext->exstructArr.size(); ++k) {
				type_data* ttd = ext->exstructArr.at(k);
				if (strcmp(td->name.c_str(), ttd->name.c_str()) == 0)
				{
					return ttd;
				}
			}
		}

		types.push_back(td);
		return td;
	}

	static type_data create_type(char* nam, int tsiz, char typ, int* strptr)
	{
		type_data td;
		td.name.NULLState();
		td.name.Init(2, false);
		td.name = nam;
		td.structptr = strptr;
		td.typesiz = tsiz;
		td.typetype = typ;
		return td;
	}

	static void dbg_codesen(code_sen* cs, bool coutstream = true)
	{
		ofstream* ptr = nullptr;
		if (coutstream) ptr = (ofstream*)&cout;
		else {
			ptr = &InsideCode_Bake::icl;
		}
		ofstream& ofs = *ptr;
		switch (cs->ck)
		{
		case codeKind::ck_addVariable:
			ofs << "add_var : ";
			break;
		case codeKind::ck_setVariable:
			ofs << "set_var : ";
			break;
		case codeKind::ck_if:
			ofs << "if__sen : ";
			break;
		case codeKind::ck_while:
			ofs << "while__ : ";
			break;
		case codeKind::ck_blocks:
			ofs << "block__ : ";
			break;
		case codeKind::ck_addFunction:
			ofs << "addfunc : ";
			break;
		case codeKind::ck_useFunction:
			ofs << "usefunc : ";
			break;
		case codeKind::ck_returnInFunction:
			ofs << "return_ : ";
			break;
		case codeKind::ck_addStruct:
			ofs << "struct__ : ";
			break;
		case codeKind::ck_break:
			ofs << "break__ : ";
			break;
		case codeKind::ck_continue:
			ofs << "continue: ";
			break;
		case codeKind::ck_addsetVariable:
			ofs << "adsetvar: ";
			break;
		default:
			break;
		}

		if (cs->ck != codeKind::ck_blocks)
		{
			for (int i = 0; i < cs->maxlen; ++i)
			{
				ofs << cs->sen[i] << " ";
			}
		}
		else
		{
			ofs << "{" << endl;
			for (int i = 0; i < cs->codeblocks->size(); ++i)
			{
				dbg_codesen(reinterpret_cast<code_sen*>(cs->codeblocks->at(i)), coutstream);
				ofs << endl;
			}
			ofs << "closed_ : }" << endl;
		}
	}

	func_data* get_func_with_name(char* name)
	{
		for (int i = 0; i < functions.size(); ++i)
		{
			if (strcmp(functions[i]->name.c_str(), name) == 0)
			{
				return functions[i];
			}
		}

		for (int i = 0; i < extension.size(); ++i)
		{
			ICB_Extension* ext = extension.at(i);
			for (int k = 0; k < ext->exfuncArr.size(); ++k) {
				if (strcmp(ext->exfuncArr.at(k)->name.c_str(), name) == 0)
				{
					return ext->exfuncArr.at(k);
				}
			}
		}

		UpdateErrMsg(4, "<%s> function is not exist.", name);
		return nullptr;
	}

	type_data* get_sub_type(type_data* t)
	{
		if (t->structptr == nullptr) {
			UpdateErrMsg(3, "<%s> type cannot have sub type.", t->name.c_str());
			return nullptr;
		}
		return reinterpret_cast<type_data*>(t->structptr);
	}

	static type_data* static_get_sub_type(type_data* t)
	{
		return reinterpret_cast<type_data*>(t->structptr);
	}

	static type_data* static_get_addpointer_type(type_data* td)
	{
		type_data* rtd = (type_data*)fm->_New(sizeof(type_data), true);
		rtd->name.NULLState();
		rtd->name.Init(td->name.size() + 1, false);

		//int len = strlen(td->name.c_str());
		rtd->name.operator=(td->name.c_str());
		rtd->name.push_back('*');
		rtd->structptr = reinterpret_cast<int*>(td);
		rtd->typesiz = 8;
		rtd->typetype = 'p';
		return rtd;
	}

	type_data* get_addpointer_type(type_data* td)
	{
		type_data* rtd = (type_data*)fm->_New(sizeof(type_data), true);
		rtd->name.NULLState();
		rtd->name.Init(td->name.size() + 1, false);

		//int len = strlen(td->name.c_str());
		rtd->name.operator=(td->name.c_str());
		rtd->name.push_back('*');
		rtd->structptr = reinterpret_cast<int*>(td);
		rtd->typesiz = 8;
		rtd->typetype = 'p';

		type_data* ntd = isCanPushType(rtd);
		if (rtd == ntd) {
			return rtd;
		}
		else {
			ReleaseTypeData(rtd);
			fm->_Delete((byte8*)rtd, sizeof(type_data));
			rtd = nullptr;
			return ntd;
		}
	}

	static type_data* static_get_array_type(type_data* td, int size)
	{
		type_data* rtd = (type_data*)fm->_New(sizeof(type_data), true);
		rtd->name.NULLState();
		rtd->name.Init(2, false);
		//int len = strlen(td->name.c_str());
		rtd->name = td->name.c_str();
		rtd->name.push_back('[');

		string str = to_string(size);
		int strnumsiz = str.size() + 2;

		fm->_tempPushLayer();
		char* strnum = (char*)fm->_tempNew(strnumsiz);
		strcpy_s(strnum, strnumsiz, str.c_str());
		for (int i = 0; i < strlen(strnum); ++i)
		{
			rtd->name.push_back(strnum[i]);
		}
		//fm->_Delete((byte8*)strnum, strnumsiz);
		fm->_tempPopLayer();

		rtd->name.push_back(']');

		rtd->structptr = reinterpret_cast<int*>(td);
		rtd->typesiz = td->typesiz * size;
		rtd->typetype = 'a';

		return rtd;
	}

	type_data* get_array_type(type_data* td, int size)
	{
		type_data* rtd = (type_data*)fm->_New(sizeof(type_data), true);
		rtd->name.NULLState();
		rtd->name.Init(2, false);
		//int len = strlen(td->name.c_str());
		rtd->name = td->name.c_str();
		rtd->name.push_back('[');

		string str = to_string(size);
		int strnumsiz = str.size() + 2;
		fm->_tempPushLayer();
		char* strnum = (char*)fm->_tempNew(strnumsiz);
		strcpy_s(strnum, strnumsiz, str.c_str());
		for (int i = 0; i < strlen(strnum); ++i)
		{
			rtd->name.push_back(strnum[i]);
		}
		fm->_tempPopLayer();
		rtd->name.push_back(']');

		rtd->structptr = reinterpret_cast<int*>(td);
		rtd->typesiz = td->typesiz * size;
		rtd->typetype = 'a';

		type_data* ntd = isCanPushType(rtd);
		if (rtd == ntd) {
			return rtd;
		}
		else {
			ReleaseTypeData(rtd);
			fm->_Delete((byte8*)rtd, sizeof(type_data));
			rtd = nullptr;
			return ntd;
		}

		return rtd;
	}

	static type_data* get_basic_type_with_int(int n)
	{
		switch (n)
		{
		case 0:
			return &basictype[1];
		case 1:
			return &basictype[7];
		case 2:
			return &basictype[2];
		case 3:
			return &basictype[6];
		case 4:
			return &basictype[0];
		case 5:
			return &basictype[5];
		case 6:
			return &basictype[3];
		case 7:
			return &basictype[4];
		}
		return nullptr;
	}

	static int get_int_with_basictype(type_data* td)
	{
		if (td == &basictype[1])
			return 0;
		if (td == &basictype[7])
			return 1;
		if (td == &basictype[2])
			return 2;
		if (td == &basictype[6])
			return 3;
		if (td == &basictype[0])
			return 4;
		if (td == &basictype[5])
			return 5;
		if (td == &basictype[3])
			return 6;
		if (td == &basictype[4])
			return 7;
		return 8;
	}

	InsideCode_Bake()
	{
	}

	virtual ~InsideCode_Bake()
	{
	}

	bool IsTypeString(const char* str)
	{
		for (int i = 0; i < types.size(); ++i)
		{
			if (strcmp(str, types[i]->name.c_str()) == 0)
				return true;
		}

		for (int i = 0; i < extension.size(); ++i)
		{
			ICB_Extension* ext = extension.at(i);
			for (int k = 0; k < ext->exstructArr.size(); ++k) {
				if (strcmp(str, ext->exstructArr.at(k)->name.c_str()) == 0)
				{
					return true;
				}
			}
		}

		return false;
	}

	static sen* get_sen_from_codesen(code_sen* cs)
	{
		sen* rs = (sen*)fm->_New(sizeof(sen), true);
		rs->NULLState();
		rs->Init(2, false, true);

		for (int i = 0; i < cs->maxlen; ++i)
		{
			segment seg;
			seg.type = 'w';
			seg.data.str = cs->sen[i];
			rs->push_back(seg);
		}

		return rs;
	}

	static void read_inst_table()
	{
		FILE* file;
		fopen_s(&file, "instruction_table.txt", "r"); // Open the file
		// in read
		// mode
		if (file == nullptr)
		{
			std::cout << "Failed to open the file." << std::endl;
			return;
		}

		int c;
		instruct_data id;
		int idi = 0;
		int pi = 0;
		int metai = 0;
		while ((c = fgetc(file)) != EOF)
		{
			if ((c == ':' || c == ' ') || (c == '\n'))
			{
				continue;
			}
			if (c == '+')
			{
				id.param_num += 1;
				id.name[idi] = 0;
				c = fgetc(file);
				switch (c)
				{
				case '1':
					id.param_typesiz[pi++] = 1;
					break;
				case '2':
					id.param_typesiz[pi++] = 2;
					break;
				case '4':
					id.param_typesiz[pi++] = 4;
					break;
				case '8':
					id.param_typesiz[pi++] = 8;
					break;
				}
			}
			else if (c == ';')
			{
				id.name[idi] = 0;
				inst_meta[metai++] = id;
				id.param_num = 0;
				idi = 0;
				pi = 0;
			}
			else
			{
				id.name[idi++] = c;
			}
		}

		for (int k = metai; k < 256; ++k)
		{
			inst_meta[k].param_num = -1;
		}

		fclose(file); // Close the file

		for (int i = 0; i < 256; ++i)
		{
			cout << i << " : " << inst_meta[i].name << endl;
		}
	}

	code_sen* find_codesen_with_linenum(fmvecarr<code_sen*>* csa, int line)
	{
		for (int i = 0; i < csa->size(); ++i)
		{
			code_sen* cs = csa->at(i);
			if (cs->start_line <= line && line <= cs->end_line)
			{
				if (cs->ck == codeKind::ck_blocks)
				{
					return find_codesen_with_linenum(reinterpret_cast<fmvecarr<
						code_sen*> *>(cs->codeblocks),
						line);
				}
				else
				{
					return cs;
				}
			}
		}

		return nullptr;
	}

	void dbg_temp_codemem(temp_mem* tm, bool coutstream = true) {
		ofstream* ptr = nullptr;
		if (coutstream) ptr = (ofstream*)&cout;
		else {
			ptr = &InsideCode_Bake::icl;
		}
		ofstream& ofs = *ptr;

		for (int i = 0; i <= tm->mem.size(); ++i)
		{
			if (inst_meta[tm->mem[i]].param_num < 0)
			{
				return;
			}
			if (tm->mem[i] == 214 || tm->mem[i] == 215)
			{
				ofs << i << "\t:" << inst_meta[tm->mem[i]].name << "(" << (uint)tm->mem[i] << ")";
				uint strmax = 0;
				instruct_data id = inst_meta[tm->mem[i]];
				int n;
				for (int k = 0; k < id.param_num; ++k)
				{
					switch (id.param_typesiz[k])
					{
					case 1:
						++i;
						n = (uint)tm->mem[i];
						ofs << " > " << n;
						break;
					case 2:
						++i;
						n = *reinterpret_cast<ushort*>(&tm->mem[i]);
						ofs << " > " << n << "(" << (uint)tm->mem[i] << ", " << (uint)tm->mem[i + 1] << ")";
						i += 1;
						break;
					case 4:
						++i;
						n = *reinterpret_cast<uint*>(&tm->mem[i]);
						strmax = n;
						ofs << " > " << n << "(" << (uint)tm->mem[i] << ", " << (uint)tm->mem[i + 1] << ", "
							<< (uint)tm->mem[i + 2] << ", " << (uint)tm->mem[i + 3] << ")";
						i += 3;
						break;
					case 8:
						++i;
						n = *reinterpret_cast<uint*>(&tm->mem[i]);
						strmax = n;
						ofs << " > " << n << "(" << (uint)tm->mem[i] << ", " << (uint)tm->mem[i + 1] << ", "
							<< (uint)tm->mem[i + 2] << ", " << (uint)tm->mem[i + 3]
							<< (uint)tm->mem[i + 4] << ", " << (uint)tm->mem[i + 5] << ", "
							<< (uint)tm->mem[i + 6] << ", " << (uint)tm->mem[i + 7] << ")";
						i += 7;
						break;
					}
				}
				ofs << "\"";
				for (int k = 1; k < strmax; ++k)
				{
					ofs << tm->mem[i + k];
				}
				ofs << "\"";
				ofs << endl;
				i += strmax + 1;
			}
			ofs << i << "\t:" << inst_meta[tm->mem[i]].name << "(" << (uint)tm->mem[i] << ")";
			instruct_data id = inst_meta[tm->mem[i]];
			int n;
			for (int k = 0; k < id.param_num; ++k)
			{
				switch (id.param_typesiz[k])
				{
				case 1:
					++i;
					n = (uint)tm->mem[i];
					ofs << " > " << n;
					break;
				case 2:
					++i;
					n = *reinterpret_cast<ushort*>(&tm->mem[i]);
					ofs << " > " << n << "(" << (uint)tm->mem[i] << ", " << (uint)tm->mem[i + 1] << ")";
					i += 1;
					break;
				case 4:
					++i;
					n = *reinterpret_cast<uint*>(&tm->mem[i]);
					ofs << " > " << n << "(" << (uint)tm->mem[i] << ", " << (uint)tm->mem[i + 1] << ", " << (uint)mem[i + 2] << ", " << (uint)mem[i + 3] << ")";
					i += 3;
					break;
				case 8:
					++i;
					n = *reinterpret_cast<uint*>(&tm->mem[i]);
					ofs << " > " << n << "(" << (uint)tm->mem[i] << ", " << (uint)tm->mem[i + 1] << ", " << (uint)tm->mem[i + 2] << ", " << (uint)tm->mem[i + 3]
						<< (uint)tm->mem[i + 4] << ", " << (uint)tm->mem[i + 5] << ", " << (uint)tm->mem[i + 6] << ", " << (uint)tm->mem[i + 7] << ")";
					i += 7;
					break;
				}
			}
			ofs << endl;
		}
	}

	void print_asm(int start, int end, bool coutstream = true)
	{
		ofstream* ptr = nullptr;
		if (coutstream) ptr = (ofstream*)&cout;
		else {
			ptr = &InsideCode_Bake::icl;
		}
		ofstream& ofs = *ptr;

		for (int i = start; i <= end; ++i)
		{
			if (inst_meta[mem[i]].param_num < 0)
			{
				return;
			}
			if (mem[i] == (byte8)insttype::IT_SET_A_CONST_STRING || mem[i] == (byte8)insttype::IT_SET_B_CONST_STRING)
			{
				ofs << i << "\t:" << inst_meta[mem[i]].name << "(" << (uint)mem[i] << ")";
				uint strmax = 0;
				instruct_data id = inst_meta[mem[i]];
				int n;
				for (int k = 0; k < id.param_num; ++k)
				{
					switch (id.param_typesiz[k])
					{
					case 1:
						++i;
						n = (uint)mem[i];
						ofs << " > " << n;
						break;
					case 2:
						++i;
						n = *reinterpret_cast<ushort*>(&mem[i]);
						ofs << " > " << n << "(" << (uint)mem[i] << ", " << (uint)mem[i + 1] << ")";
						i += 1;
						break;
					case 4:
						++i;
						n = *reinterpret_cast<uint*>(&mem[i]);
						strmax = n;
						ofs << " > " << n << "(" << (uint)mem[i] << ", " << (uint)mem[i + 1] << ", "
							<< (uint)mem[i + 2] << ", " << (uint)mem[i + 3] << ")";
						i += 3;
						break;
					case 8:
						++i;
						n = *reinterpret_cast<uint*>(&mem[i]);
						strmax = n;
						ofs << " > " << n << "(" << (uint)mem[i] << ", " << (uint)mem[i + 1] << ", "
							<< (uint)mem[i + 2] << ", " << (uint)mem[i + 3]
							<< (uint)mem[i + 4] << ", " << (uint)mem[i + 5] << ", "
							<< (uint)mem[i + 6] << ", " << (uint)mem[i + 7] << ")";
						i += 7;
						break;
					}
				}
				ofs << "\"";
				for (int k = 1; k < strmax; ++k)
				{
					ofs << mem[i + k];
				}
				ofs << "\"";
				ofs << endl;
				i += strmax + 1;
			}
			ofs << i << "\t:" << inst_meta[mem[i]].name << "(" << (uint)mem[i] << ")";
			instruct_data id = inst_meta[mem[i]];
			int n;
			for (int k = 0; k < id.param_num; ++k)
			{
				switch (id.param_typesiz[k])
				{
				case 1:
					++i;
					n = (uint)mem[i];
					ofs << " > " << n;
					break;
				case 2:
					++i;
					n = *reinterpret_cast<ushort*>(&mem[i]);
					ofs << " > " << n << "(" << (uint)mem[i] << ", " << (uint)mem[i + 1] << ")";
					i += 1;
					break;
				case 4:
					++i;
					n = *reinterpret_cast<uint*>(&mem[i]);
					ofs << " > " << n << "(" << (uint)mem[i] << ", " << (uint)mem[i + 1] << ", " << (uint)mem[i + 2] << ", " << (uint)mem[i + 3] << ")";
					i += 3;
					break;
				case 8:
					++i;
					n = *reinterpret_cast<uint*>(&mem[i]);
					ofs << " > " << n << "(" << (uint)mem[i] << ", " << (uint)mem[i + 1] << ", " << (uint)mem[i + 2] << ", " << (uint)mem[i + 3]
						<< (uint)mem[i + 4] << ", " << (uint)mem[i + 5] << ", " << (uint)mem[i + 6] << ", " << (uint)mem[i + 7] << ")";
					i += 7;
					break;
				}
			}
			ofs << endl;
		}
	}

	void dbg_bakecode(fmvecarr<code_sen*>* csa, int sav, bool coutstream = true)
	{
		ofstream* ptr = nullptr;
		if (coutstream) ptr = (ofstream*)&cout;
		else {
			ptr = &InsideCode_Bake::icl;
		}
		ofstream& ofs = *ptr;
		// cout << "all asm :" << endl;
		int save = sav;
		for (int i = 0; i < csa->size(); ++i)
		{
			code_sen* cs = csa->at(i);
			if (cs->start_line > save)
			{
				ofs << "\033[0;36m";
				ofs << "<nocode>" << endl;
				ofs << "\033[0;37m";
				print_asm(save, cs->start_line - 1, coutstream);
				save = cs->start_line;
				ofs << endl;
			}
			if (cs->ck == codeKind::ck_blocks)
			{
				fmvecarr<code_sen*>* css =
					reinterpret_cast<fmvecarr<code_sen*> *>(cs->codeblocks);
				dbg_bakecode(css, save, coutstream);
				save = css->last()->end_line + 1;
			}
			else
			{
				ofs << "\033[0;36m";
				dbg_codesen(cs, false);
				ofs << "\033[0;37m" << endl;
				print_asm(cs->start_line, cs->end_line, coutstream);
				save = cs->end_line + 1;
				ofs << endl;
			}
		}
	}

	static void StaticInit() {
		wbss.Init();
		icl.open("icb_dbg.txt");
		icl << "Inside Code Bake System Start" << endl;
		icl << "ICB_StaticInit...";

		bool icldetail = GetICLFlag(ICL_FLAG::ICB_StaticInit);
		if (icldetail) icl << "start" << endl;
		if (icldetail) icl << "ICB_StaticInit create basic types start" << endl;

		char name[32] = {};
		strcpy_s(name, 4, "int");
		if (icldetail) icl << "ICB_StaticInit Create Type : " << name << "...";
		basictype[0] = create_type(name, 4, 'b', nullptr);
		if (icldetail) icl << "finish" << endl;

		strcpy_s(name, 5, "char");
		if (icldetail) icl << "ICB_StaticInit Create Type : " << name << "...";
		basictype[1] = create_type(name, 1, 'b', nullptr);
		if (icldetail) icl << "finish" << endl;

		strcpy_s(name, 6, "short");
		if (icldetail) icl << "ICB_StaticInit Create Type : " << name << "...";
		basictype[2] = create_type(name, 2, 'b', nullptr);
		if (icldetail) icl << "finish" << endl;

		strcpy_s(name, 6, "float");
		if (icldetail) icl << "ICB_StaticInit Create Type : " << name << "...";
		basictype[3] = create_type(name, 4, 'b', nullptr);
		if (icldetail) icl << "finish" << endl;

		strcpy_s(name, 5, "bool");
		if (icldetail) icl << "ICB_StaticInit Create Type : " << name << "...";
		basictype[4] = create_type(name, 1, 'b', nullptr);
		if (icldetail) icl << "finish" << endl;

		strcpy_s(name, 5, "uint");
		if (icldetail) icl << "ICB_StaticInit Create Type : " << name << "...";
		basictype[5] = create_type(name, 4, 'b', nullptr);
		if (icldetail) icl << "finish" << endl;

		strcpy_s(name, 7, "ushort");
		if (icldetail) icl << "ICB_StaticInit Create Type : " << name << "...";
		basictype[6] = create_type(name, 2, 'b', nullptr);
		if (icldetail) icl << "finish" << endl;

		strcpy_s(name, 6, "uchar");
		if (icldetail) icl << "ICB_StaticInit Create Type : " << name << "...";
		basictype[7] = create_type(name, 1, 'b', nullptr);
		if (icldetail) icl << "finish" << endl;
		if (icldetail) icl << "ICB_StaticInit create basic types finish" << endl;

		if (icldetail) icl << "ICB_StaticInit create basic operation start" << endl;

		basicoper[0] = create_oper("[", 'f', 0, 0);
		basicoper[1] = create_oper(".", 'f', 0, 0);
		basicoper[2] = create_oper("->", 'f', 0, 0);
		basicoper[3] = create_oper("&", 'f', 0, 0);
		basicoper[4] = create_oper("*", 'f', 0, 0);
		basicoper[5] = create_oper("*", 'o', (byte8)insttype::IT_AU_BYTE_MUL_A, (byte8)insttype::IT_AU_FLOAT_MUL_B);
		basicoper[6] = create_oper("/", 'o', (byte8)insttype::IT_AU_BYTE_DIV_A, (byte8)insttype::IT_AU_FLOAT_DIV_B);
		basicoper[7] = create_oper("%", 'o', (byte8)insttype::IT_AU_BYTE_PER_A, (byte8)insttype::IT_AU_UINT_PER_B);
		basicoper[8] = create_oper("+", 'o', (byte8)insttype::IT_AU_BYTE_ADD_A, (byte8)insttype::IT_AU_FLOAT_ADD_B);
		basicoper[9] = create_oper("-", 'o', (byte8)insttype::IT_AU_BYTE_MIN_A, (byte8)insttype::IT_AU_FLOAT_MIN_B);
		basicoper[10] = create_oper("==", 'o', (byte8)insttype::IT_CM_BOOL_SAME_A, (byte8)insttype::IT_CM_BOOL_SAME_B);
		basicoper[11] = create_oper("<", 'o', (byte8)insttype::IT_CM_BOOL_BYTE_RBIG_A, (byte8)insttype::IT_CM_BOOL_FLOAT_RBIG_B);
		basicoper[12] = create_oper(">", 'o', (byte8)insttype::IT_CM_BOOL_BYTE_LBIG_A, (byte8)insttype::IT_CM_BOOL_FLOAT_LBIG_B);
		basicoper[13] = create_oper("<=", 'o', (byte8)insttype::IT_CM_BOOL_BYTE_RBIGSAME_A, (byte8)insttype::IT_CM_BOOL_FLOAT_RBIGSAME_B);
		basicoper[14] = create_oper(">=", 'o', (byte8)insttype::IT_CM_BOOL_BYTE_LBIGSAME_A, (byte8)insttype::IT_CM_BOOL_FLOAT_LBIGSAME_B);
		basicoper[15] = create_oper("!=", 'o', (byte8)insttype::IT_CM_BOOL_NOTSAME_A, (byte8)insttype::IT_CM_BOOL_NOTSAME_B);
		basicoper[16] = create_oper("!", 'o', (byte8)insttype::IT_LU_BOOL_NOT_A, (byte8)insttype::IT_LU_BOOL_NOT_B);
		basicoper[17] = create_oper("&&", 'o', (byte8)insttype::IT_LU_BOOL_AND_A, (byte8)insttype::IT_LU_BOOL_AND_B);
		basicoper[18] = create_oper("||", 'o', (byte8)insttype::IT_LU_BOOL_OR_A, (byte8)insttype::IT_LU_BOOL_OR_B);
		if (icldetail) icl << "ICB_StaticInit create basic operation finish" << endl;

		read_inst_table();

		if (icldetail) icl << "ICB_StaticInit ";
		icl << "finish" << endl;
	}

	static void StaticRelease() {
		cout << "instruction table" << endl;

		for (int i = 0; i < 8; ++i) {
			type_data* td = &basictype[i];
			td->name.release();
			td->name.NULLState();
		}
	}

	void init(int codemem_siz)
	{
		icl << "Create_New_ICB[" << this << "] Initialization...";
		allcode_sen.NULLState();
		allcode_sen.Init(2, false, true);
		allcode_sen.islocal = false;
		csarr = nullptr;

		types.NULLState();
		types.Init(8, false, true);
		for (int i = 0; i < basictype_max; ++i)
		{
			types.push_back(&basictype[i]);
		}

		block_data* bd = (block_data*)fm->_New(sizeof(block_data), true);
		bd->start_pc = &mem[0];
		bd->add_address_up = 0;
		bd->variable_data.NULLState();
		bd->variable_data.Init(10, false, true);
		
		max_mem_byte = codemem_siz;
		mem = (byte8*)fm->_New(max_mem_byte, true);
		for (int i = 0; i < max_mem_byte; ++i)
		{
			mem[i] = 254;
		}

		blockstack.NULLState();
		blockstack.Init(2, false, true);
		blockstack.islocal = false;

		nextbd.breakpoints = (fmvecarr<int> *)fm->_New(sizeof(fmvecarr<int>), true);
		nextbd.continuepoints = (fmvecarr<int> *)fm->_New(sizeof(fmvecarr<int>), true);
		nextbd.breakpoints->NULLState();
		nextbd.breakpoints->Init(2, false, true);
		nextbd.continuepoints->NULLState();
		nextbd.continuepoints->Init(2, false, true);

		nextsd = nullptr;

		functions.NULLState();
		functions.Init(2, false, true);

		globalVariables.NULLState();
		globalVariables.Init(2, false, true);

		extension.NULLState();
		extension.Init(8, false, true);

		curErrMsg.NULLState();
		curErrMsg.Init(128, false);

		codeLineVec.NULLState();
		codeLineVec.Init(32, false, true);

		codeLineVec_Word.NULLState();
		codeLineVec_Word.Init(32, false, true);

		icl << "finish" << endl;
	}

	void HashInit() {
		changeHash = 0;
	}

	void push_word(char* sptr)
	{
		for (int i = 0; i < wbss.wordlist.size(); ++i)
		{
			if (strcmp(sptr, wbss.wordlist.at(i)) == 0)
			{
				allcode_sen.push_back(wbss.wordlist.at(i));
				return;
			}
		}

		char* cstr = wbss.addword(sptr);
		allcode_sen.push_back(cstr);
	}

	void set_word(int index, char* sptr)
	{
		for (int i = 0; i < wbss.wordlist.size(); ++i)
		{
			if (strcmp(sptr, wbss.wordlist.at(i)) == 0)
			{
				allcode_sen[index] = wbss.wordlist.at(i);
				return;
			}
		}

		char* cstr = wbss.addword(sptr);
		allcode_sen[index] = cstr;
	}

	void AddTextBlocks(fmlcstr& codetxt)
	{
		//codeLineVec_Word.push_back(0);
		int present_line = 1;
		fmlcstr insstr;
		insstr.NULLState();
		insstr.Init(2, false);

		bool icldetail = GetICLFlag(ICL_FLAG::BakeCode_AddTextBlocks);
		unsigned int textper = 4;
		unsigned int ti = 0;
		if (icldetail) icl << "-----------------------------------------\n\n full text : \n" << codetxt.c_str() << endl;
		for (int i = 0; i < (int)codetxt.size(); i++)
		{
			insstr.push_back(codetxt.at(i));
			if (bCanBeTextblock_notAllowNeg(insstr))
			{
				if (i == codetxt.size() - 1)
				{
					if (icldetail) {
						if (ti % textper == 0) {
							icl << i << "\"" << insstr.c_str() << "\"" << endl;
						}
						else {
							icl << i << "\"" << insstr.c_str() << "\"\t";
						}
						++ti;
					}
					push_word(insstr.c_str());
					while (i + 1 >= codeLineVec.at(present_line - 1)) {
						codeLineVec_Word.push_back(allcode_sen.size() - 1);
						++present_line;
					}

					continue;
				}
				insstr.push_back(codetxt.at(i + 1));
				if (bCanBeTextblock_notAllowNeg(insstr) == false)
				{
					insstr.pop_back();
					if (icldetail) {
						if (ti % textper == 0) {
							icl << i << "\"" << insstr.c_str() << "\"" << endl;
						}
						else {
							icl << i << "\"" << insstr.c_str() << "\"\t";
						}
						++ti;
					}
					push_word(insstr.c_str());
					while (i + 1 >= codeLineVec.at(present_line - 1)) {
						codeLineVec_Word.push_back(allcode_sen.size() - 1);
						++present_line;
					}
					insstr.clear();
				}
				else
				{
					int inssiz = insstr.size() - 1;
					char c = insstr.at(inssiz);
					bool bnor = ((('0' <= c && c <= '9') || ('a' <= c && c <= 'z')) || ('A' <= c && c <= 'Z') || c == '_');
					if (bnor == false)
					{
						insstr.pop_back();
						if (icldetail) {
							if (ti % textper == 0) {
								icl << i << "\"" << insstr.c_str() << "\"" << endl;
							}
							else {
								icl << i << "\"" << insstr.c_str() << "\"\t";
							}
							++ti;
						}
						push_word(insstr.c_str());
						insstr.clear();
						insstr.push_back(c);
						while (i + 1 >= codeLineVec.at(present_line - 1)) {
							codeLineVec_Word.push_back(allcode_sen.size() - 1);
							++present_line;
						}
						if (icldetail) {
							if (ti % textper == 0) {
								icl << i << "\"" << insstr.c_str() << "\"" << endl;
							}
							else {
								icl << i << "\"" << insstr.c_str() << "\"\t";
							}
							++ti;
						}
						push_word(insstr.c_str());
						insstr.clear();
						while (i + 1 >= codeLineVec.at(present_line - 1)) {
							codeLineVec_Word.push_back(allcode_sen.size() - 1);
							++present_line;
						}
						i++;
					}
					else
						insstr.pop_back();
				}
			}
			else
			{
				if (insstr.at(0) == '\"')
				{
					continue;
				}
				else
				{
					insstr.pop_back();
				}
			}
		}

		for (int i = 0; i < (int)allcode_sen.size(); ++i)
		{
			fm->_tempPushLayer();
			fmlcstr s;
			s.NULLState();
			s.Init(8, false, false);
			s = allcode_sen[i];
			if (s == "-")
			{
				if (i == 0)
					continue;
				fmlcstr t0;
				t0.NULLState();
				t0.Init(8, false, false);
				t0 = allcode_sen[i - 1];
				fmlcstr t1;
				t1.NULLState();
				t1.Init(8, false, false);
				t1 = allcode_sen[i + 1];

				bool bequ = (DecodeTextBlock(t0) == TBT::_operation) && (DecodeTextBlock(t1) != TBT::_operation);
				if (bequ)
				{
					fmlcstr insstr;
					insstr.NULLState();
					insstr.Init(8, false, false);
					insstr = allcode_sen[i];
					if (icldetail) icl << "combine block : " << i << " : \"" << insstr.c_str() << "\" + ";
					for (int k = 0; k < t1.size(); k++)
					{
						insstr.push_back(t1[k]);
					}

					set_word(i, insstr.c_str());
					if (icldetail) icl << i + 1 << " : \"" << t1.c_str() << "\" => " << insstr.c_str() << endl;
					allcode_sen.erase(i + 1);

					for (int k = 0; k < codeLineVec_Word.size(); ++k) {
						if (codeLineVec_Word.at(k) >= i + 1) {
							codeLineVec_Word.at(k) -= 1;
						}
					}
				}
			}

			if (s == "=")
			{
				if (i == 0)
					continue;

				char c = allcode_sen[i - 1][0];
				bool bequ = (c == '!' || c == '<') || (c == '=' || c == '>');
				bequ = bequ || ((c == '+' || c == '-') || (c == '*' || c == '/'));
				bequ = bequ || (c == '%');
				if (bequ)
				{
					fmlcstr insstr;
					insstr.NULLState();
					insstr.Init(8, false, false);
					insstr = allcode_sen[i - 1];
					if (icldetail) icl << "combine block : " << i - 1 << " : \"" << insstr.c_str() << "\" + ";
					insstr.push_back('=');
					set_word(i - 1, insstr.c_str());
					if (icldetail) icl << i << " : \"" << allcode_sen[i] << "\" => \"" << insstr.c_str() << "\"" << endl;
					allcode_sen.erase(i);
					i -= 1;

					for (int k = 0; k < codeLineVec_Word.size(); ++k) {
						if (codeLineVec_Word.at(k) >= i) {
							codeLineVec_Word.at(k) -= 1;
						}
					}
				}
			}
			if (s == "|")
			{
				if (i == 0)
					continue;
				char c = allcode_sen[i + 1][0];
				bool bequ = (c == '|');
				if (bequ)
				{
					fmlcstr insstr;
					insstr.NULLState();
					insstr.Init(8, false, false);
					insstr = allcode_sen[i];
					if (icldetail) icl << "combine block : " << i - 1 << " : \"" << insstr.c_str() << "\" + ";
					insstr.push_back('|');
					set_word(i, insstr.c_str());
					if (icldetail) icl << i << " : \"" << allcode_sen[i] << "\" => \"" << insstr.c_str() << "\"" << endl;
					allcode_sen.erase(i + 1);

					for (int k = 0; k < codeLineVec_Word.size(); ++k) {
						if (codeLineVec_Word.at(k) >= i + 1) {
							codeLineVec_Word.at(k) -= 1;
						}
					}
				}
			}
			if (s == "&")
			{
				if (i == 0)
					continue;
				char c = allcode_sen[i + 1][0];
				bool bequ = (c == '&');
				if (bequ)
				{
					fmlcstr insstr;
					insstr.NULLState();
					insstr.Init(8, false, false);
					insstr = allcode_sen[i];
					if (icldetail) icl << "combine block : " << i << " : \"" << insstr.c_str() << "\" + ";
					insstr.push_back('&');
					set_word(i, insstr.c_str());
					if (icldetail) icl << i + 1 << " : \"" << allcode_sen[i + 1] << "\" => \"" << insstr.c_str() << "\"" << endl;
					allcode_sen.erase(i + 1);

					for (int k = 0; k < codeLineVec_Word.size(); ++k) {
						if (codeLineVec_Word.at(k) >= i + 1) {
							codeLineVec_Word.at(k) -= 1;
						}
					}
				}
			}
			if (s == ".")
			{
				if (i - 1 < 0 && i + 1 > allcode_sen.size() - 1)
					continue;
				fmlcstr front;
				front.NULLState();
				front.Init(8, false, false);
				front = allcode_sen[i - 1];
				fmlcstr back;
				back.NULLState();
				back.Init(8, false, false);
				back = allcode_sen[i + 1];
				bool bequ = true;
				for (int k = 0; k < front.size(); k++)
				{
					if (('0' <= front.at(k) && front.at(k) <= '9') == false)
					{
						bequ = false;
						break;
					}
				}
				for (int k = 0; k < back.size(); k++)
				{
					if (('0' <= back.at(k) && back.at(k) <= '9') == false)
					{
						bequ = false;
						break;
					}
				}
				if (bequ)
				{
					fmlcstr insstr;
					insstr.NULLState();
					insstr.Init(8, false, false);
					insstr = allcode_sen[i - 1];
					if (icldetail) icl << "combine block : " << i - 1 << " : \"" << insstr.c_str() << "\" + \".\" + ";
					insstr.push_back('.');
					for (int k = 0; k < back.size(); k++)
					{
						insstr.push_back(back[k]);
					}
					set_word(i - 1, insstr.c_str());
					if (icldetail) icl << i + 1 << " : \"" << back.c_str() << "\" => \"" << insstr.c_str() << "\"" << endl;
					allcode_sen.erase(i);
					allcode_sen.erase(i);

					for (int k = 0; k < codeLineVec_Word.size(); ++k) {
						if (codeLineVec_Word.at(k) >= i) {
							codeLineVec_Word.at(k) -= 2;
						}
					}
				}
			}

			if (s == "\'")
			{
				if (strcmp(allcode_sen[i + 2], "\'") == 0)
				{
					fmlcstr insstr;
					insstr.NULLState();
					insstr.Init(8, false, false);
					insstr = allcode_sen[i];
					fmlcstr cent;
					cent.NULLState();
					cent.Init(8, false, false);
					cent = allcode_sen[i + 1];
					fmlcstr back;
					back.NULLState();
					back.Init(8, false, false);
					back = allcode_sen[i + 2];
					for (int k = 0; k < cent.size(); k++)
					{
						insstr.push_back(cent.at(k));
					}

					for (int k = 0; k < back.size(); k++)
					{
						insstr.push_back(back.at(k));
					}

					set_word(i, insstr.c_str());
					allcode_sen.erase(i + 1);
					allcode_sen.erase(i + 1);

					for (int k = 0; k < codeLineVec_Word.size(); ++k) {
						if (codeLineVec_Word.at(k) >= i + 1) {
							codeLineVec_Word.at(k) -= 2;
						}
					}

					if (icldetail) icl << "combine block : " << i << " ~ " << i + 2 << "\"" << insstr.c_str() << "\"" << endl;
				}
				else if (strcmp(allcode_sen[i + 1], "\\") == 0 && strcmp(allcode_sen[i + 3], "\'") == 0)
				{
					fmlcstr insstr;
					insstr.NULLState();
					insstr.Init(8, false, false);
					insstr = allcode_sen[i];
					fmlcstr cent;
					cent.NULLState();
					cent.Init(8, false, false);
					cent = allcode_sen[i + 1];
					fmlcstr back;
					back.NULLState();
					back.Init(8, false, false);
					back = allcode_sen[i + 2];
					fmlcstr backback;
					backback.NULLState();
					backback.Init(8, false, false);
					backback = allcode_sen[i + 3];
					for (int k = 0; k < cent.size(); k++)
					{
						insstr.push_back(cent.at(k));
					}

					for (int k = 0; k < back.size(); k++)
					{
						insstr.push_back(back.at(k));
					}

					for (int k = 0; k < backback.size(); k++)
					{
						insstr.push_back(backback.at(k));
					}

					set_word(i, insstr.c_str());

					if (icldetail) icl << "combine block : " << i << " ~ " << i + 2 << "\"" << insstr.c_str() << "\"" << endl;

					allcode_sen.erase(i + 1);
					allcode_sen.erase(i + 1);
					allcode_sen.erase(i + 1);

					for (int k = 0; k < codeLineVec_Word.size(); ++k) {
						if (codeLineVec_Word.at(k) >= i + 1) {
							codeLineVec_Word.at(k) -= 3;
						}
					}
				}
			}

			fm->_tempPopLayer();
		}

		insstr.islocal = true;
	}

	static void set_codesen(code_sen* sen, fmvecarr<char*>& arr)
	{
		sen->maxlen = arr.size();
		sen->sen = (char**)fm->_New(sizeof(char*) * sen->maxlen, true);
		for (int i = 0; i < sen->maxlen; ++i)
		{
			sen->sen[i] = arr[i];
		}
	}

	fmvecarr<code_sen*>* AddCodeFromBlockData(fmvecarr<char*>& allcodesen, const char* ScanMod, unsigned int currentLine = 0)
	{
		// allcode_sen-> allcode_sen
		// ic -> this
		// code -> sen_arr

		fmvecarr<code_sen*>* senarr =
			(fmvecarr<code_sen*> *)fm->_New(sizeof(fmvecarr<code_sen*>), true);
		senarr->NULLState();
		senarr->Init(10, false, true);

		unsigned int clwi = currentLine + 1;

		bool readytoStart = true;
		int StartI = 0;
		if (readytoStart)
		{
			if (strcmp(ScanMod, "none") == 0)
			{
				bool icldetail = GetICLFlag(ICL_FLAG::BakeCode_ScanCodes);
				if (icldetail) icl << "start" << endl;

				for (int i = 0; i < (int)allcodesen.size() - 1; i++)
				{
					if (icldetail) icl << "[" << i << "] ~ ";
					if (strcmp(allcodesen[i], "struct") == 0)
					{
						if (icldetail) icl << "struct code jump";
						int open = 0;
						int h = 0;
						while (strcmp(allcodesen[i + h], "}") != 0 || open != 1)
						{
							if (strcmp(allcodesen[i + h], "{") == 0)
								open++;
							else if (strcmp(allcodesen[i + h], "}") == 0)
								open--;
							h++;
						}

						i += h;
						StartI = i + 1;
					}
					if (IsTypeString(allcodesen[i]))
					{
						if (strcmp(allcodesen[i + 2], "(") == 0)
						{
							// addfunction
							if (icldetail) icl << "addfunction : ";
							code_sen* cs = (code_sen*)fm->_New(sizeof(code_sen), true);
							*cs = code_sen();
							cs->ck = codeKind::ck_addFunction;
							fm->_tempPushLayer();
							fmvecarr<char*> cbs;
							cbs.NULLState();
							cbs.Init(3, false, false);
							cbs.push_back(allcodesen[i]);
							cbs.push_back(allcodesen[i + 1]);

							int startI = i + 2;
							int count = 0;
							while (1)
							{
								if (strcmp(allcodesen[startI], "(") == 0)
									count++;
								else if (strcmp(allcodesen[startI], ")") == 0)
									count--;

								cbs.push_back(allcodesen[startI]);

								startI++;

								if (count == 0)
								{
									break;
								}
							}

							set_codesen(cs, cbs);
							fm->_tempPopLayer();
							if (icldetail) dbg_codesen(cs, false);

							while (codeLineVec_Word.at(clwi) <= i + codeLineVec_Word.at(currentLine) && codeLineVec_Word.size() > clwi) {
								clwi += 1;
							}
							clwi -= 1;
							cs->codeline = clwi + 1;

							senarr->push_back(cs);
							i = startI - 1;
							StartI = i + 1;
						}
						else
						{
							int k = 1;
							int v = k;
							bool addset = false;
							while (true)
							{
								if (addset)
								{
									if (strcmp(allcodesen[i + v], ";") != 0)
									{
										v++;
									}
									else
									{
										break;
									}
								}
								else
								{
									if (strcmp(allcodesen[i + k], "=") == 0)
									{
										addset = true;
										v = k + 1;
										continue;
									}

									if (strcmp(allcodesen[i + k], ";") != 0)
									{
										k++;
									}
									else
									{
										break;
									}
								}
							}

							if (addset == false)
							{
								if (icldetail) icl << "add variable : ";
								code_sen* cs = (code_sen*)fm->_New(sizeof(code_sen), true);
								*cs = code_sen();
								cs->ck = codeKind::ck_addVariable;
								fm->_tempPushLayer();
								fmvecarr<char*> cbs;
								cbs.NULLState();
								cbs.Init(3, false, false);
								for (int j = 0; j < k; j++)
								{
									cbs.push_back(allcodesen[i + j]);
								}

								set_codesen(cs, cbs);
								fm->_tempPopLayer();
								while (codeLineVec_Word.at(clwi) <= i + codeLineVec_Word.at(currentLine) && codeLineVec_Word.size() > clwi) {
									clwi += 1;
								}
								clwi -= 1;
								cs->codeline = clwi + 1;
								if (icldetail) dbg_codesen(cs, false);
								senarr->push_back(cs);
								i += k;
								StartI = i + 1;
							}
							else
							{
								if (icldetail) icl << "add and set variable : ";
								code_sen* cs = (code_sen*)fm->_New(sizeof(code_sen), true);
								*cs = code_sen();
								cs->ck = codeKind::ck_addsetVariable;
								fm->_tempPushLayer();
								fmvecarr<char*> cbs;
								cbs.NULLState();
								cbs.Init(3, false, false);
								for (int j = 0; j < v; j++)
								{
									cbs.push_back(allcodesen[i + j]);
								}

								set_codesen(cs, cbs);

								while (codeLineVec_Word.at(clwi) <= i + codeLineVec_Word.at(currentLine) && codeLineVec_Word.size() > clwi) {
									clwi += 1;
								}
								clwi -= 1;
								cs->codeline = clwi + 1;

								fm->_tempPopLayer();
								if (icldetail) dbg_codesen(cs, false);
								senarr->push_back(cs);
								i += v;
								StartI = i + 1;
							}
						}
					}
					else if (allcodesen.size() > i + 2 && (strcmp(allcodesen[i + 2], "(") == 0 && strcmp(allcodesen[i], "void") == 0))
					{
						if (icldetail) icl << "add Function : ";
						code_sen* cs = (code_sen*)fm->_New(sizeof(code_sen), true);
						*cs = code_sen();
						cs->ck = codeKind::ck_addFunction;
						fm->_tempPushLayer();
						fmvecarr<char*> cbs;
						cbs.NULLState();
						cbs.Init(3, false, false);
						cbs.push_back(allcodesen[i]);
						cbs.push_back(allcodesen[i + 1]);

						int startI = i + 2;
						int count = 0;
						while (1)
						{
							if (strcmp(allcodesen[startI], "(") == 0)
								count++;
							else if (strcmp(allcodesen[startI], ")") == 0)
								count--;

							cbs.push_back(allcodesen[startI]);

							startI++;

							if (count == 0)
							{
								break;
							}
						}

						set_codesen(cs, cbs);

						while (codeLineVec_Word.at(clwi) <= i + codeLineVec_Word.at(currentLine) && codeLineVec_Word.size() > clwi) {
							clwi += 1;
						}
						clwi -= 1;
						cs->codeline = clwi + 1;

						fm->_tempPopLayer();
						senarr->push_back(cs);
						if (icldetail) dbg_codesen(cs, false);
						i = startI - 1;
						StartI = i + 1;
					}
					else if (strcmp(allcodesen[i], "{") == 0)
					{
						if (icldetail) icl << "blocks : ";
						code_sen* cs = (code_sen*)fm->_New(sizeof(code_sen), true);
						*cs = code_sen();
						cs->ck = codeKind::ck_blocks;
						fmvecarr<char*> cbs;
						cbs.NULLState();
						cbs.Init(2, false, true);

						int open = 0;
						int h = 1;
						// cbs.push_back(allcodesen[i]);
						// cout << "block : { ";
						while (i + h < allcodesen.size() && !(strcmp(allcodesen[i + h], "}") == 0 && open == 0))
						{
							if (strcmp(allcodesen[i + h], "{") == 0)
								open++;
							else if (strcmp(allcodesen[i + h], "}") == 0)
								open--;

							cbs.push_back(allcodesen[i + h]);
							// cout << allcodesen[i + h] << ' ';

							++h;
						}
						// cout << "}" << endl;

						// cbs.pop_back();
						// cbs.erase(0);

						while (codeLineVec_Word.at(clwi) <= i + codeLineVec_Word.at(currentLine) && codeLineVec_Word.size() > clwi) {
							clwi += 1;
						}
						//clwi -= 1;

						fmvecarr<code_sen*>* cbv = AddCodeFromBlockData(cbs, "none", clwi);
						cbs.release();
						cbs.NULLState();

						cs->codeblocks = (fmvecarr<int*> *)fm->_New(sizeof(fmvecarr<int*>), true);
						cs->codeblocks->NULLState();
						cs->codeblocks->Init(8, false, true);
						Init_VPTR<fmvecarr<int*>*>(cs->codeblocks);

						for (int u = 0; u < (int)cbv->size(); u++)
						{
							cs->codeblocks->push_back(reinterpret_cast<int*>((*cbv)[u]));
						}

						cs->codeline = clwi;

						cbv->release();
						fm->_Delete((byte8*)cbv, sizeof(fmvecarr<code_sen*>));

						senarr->push_back(cs);
						if (icldetail) dbg_codesen(cs, false);

						i += h;
						StartI = i + 1;
					}
					else if (strcmp(allcodesen[i], "=") == 0 || (strlen(allcodesen[i]) == 2 && allcodesen[i][1] == '='))
					{
						if (icldetail) icl << "set Variable : ";
						code_sen* cs = (code_sen*)fm->_New(sizeof(code_sen), true);
						*cs = code_sen();
						cs->ck = codeKind::ck_setVariable;
						fm->_tempPushLayer();
						fmvecarr<char*> cbs;
						cbs.NULLState();
						cbs.Init(3, false, false);

						int h = 0;
						for (int k = StartI; k < i; k++)
						{
							cbs.push_back(allcodesen[k]);
						}

						while (allcodesen[i + h][0] != ';')
						{
							cbs.push_back(allcodesen[i + h]);
							h++;
						}

						set_codesen(cs, cbs);

						while (codeLineVec_Word.at(clwi) <= StartI + codeLineVec_Word.at(currentLine) && codeLineVec_Word.size() > clwi) {
							clwi += 1;
						}
						clwi -= 1;
						cs->codeline = clwi + 1;

						fm->_tempPopLayer();
						senarr->push_back(cs);
						if (icldetail) dbg_codesen(cs, false);

						i += h;
						StartI = i + 1;
					}
					else if (strcmp(allcodesen[i], "if") == 0)
					{
						if (icldetail) icl << "IF : ";
						code_sen* cs = (code_sen*)fm->_New(sizeof(code_sen), true);
						*cs = code_sen();
						cs->ck = codeKind::ck_if;
						fm->_tempPushLayer();
						fmvecarr<char*> cbs;
						cbs.NULLState();
						cbs.Init(3, false, false);
						int open = 0;
						int h = 0;
						cbs.push_back(allcodesen[i]);
						while (strcmp(allcodesen[i + h], ")") != 0 || open != 1)
						{
							if (strcmp(allcodesen[i + h], "(") == 0)
								open++;
							else if (strcmp(allcodesen[i + h], ")") == 0)
								open--;
							h++;
							cbs.push_back(allcodesen[i + h]);
						}

						set_codesen(cs, cbs);

						while (codeLineVec_Word.at(clwi) <= i + codeLineVec_Word.at(currentLine) && codeLineVec_Word.size() > clwi) {
							clwi += 1;
						}
						clwi -= 1;
						cs->codeline = clwi + 1;

						fm->_tempPopLayer();
						senarr->push_back(cs);
						if (icldetail) dbg_codesen(cs, false);
						i += h;
						StartI = i + 1;
					}
					else if (strcmp(allcodesen[i], "else") == 0)
					{
						if (allcodesen.size() > i + 1 && strcmp(allcodesen[i + 1], "if") == 0)
						{
							if (icldetail) icl << "IF : ";
							code_sen* cs = (code_sen*)fm->_New(sizeof(code_sen), true);
							*cs = code_sen();
							cs->ck = codeKind::ck_if;
							fm->_tempPushLayer();
							fmvecarr<char*> cbs;
							cbs.NULLState();
							cbs.Init(3, false, false);

							int open = 0;
							int h = 1;
							cbs.push_back(allcodesen[i]);
							cbs.push_back(allcodesen[i + 1]);
							while (strcmp(allcodesen[i + h], ")") != 0 || open != 1)
							{
								if (strcmp(allcodesen[i + h], "(") == 0)
									open++;
								else if (strcmp(allcodesen[i + h], ")") == 0)
									open--;
								h++;
								cbs.push_back(allcodesen[i + h]);
							}

							set_codesen(cs, cbs);
							while (codeLineVec_Word.at(clwi) <= i + codeLineVec_Word.at(currentLine) && codeLineVec_Word.size() > clwi) {
								clwi += 1;
							}
							clwi -= 1;
							cs->codeline = clwi + 1;

							fm->_tempPopLayer();
							senarr->push_back(cs);
							if (icldetail) dbg_codesen(cs, false);
							i += h;
							StartI = i + 1;
							// else if �� ���
						}
						else
						{
							if (icldetail) icl << "IF : ";
							code_sen* cs = (code_sen*)fm->_New(sizeof(code_sen), true);
							*cs = code_sen();
							cs->ck = codeKind::ck_if;
							fm->_tempPushLayer();
							fmvecarr<char*> cbs;
							cbs.NULLState();
							cbs.Init(3, false, false);
							// �׳� else�� ���
							cbs.push_back(allcodesen[i]);
							set_codesen(cs, cbs);
							while (codeLineVec_Word.at(clwi) <= i + codeLineVec_Word.at(currentLine) && codeLineVec_Word.size() > clwi) {
								clwi += 1;
							}
							clwi -= 1;
							cs->codeline = clwi + 1;

							fm->_tempPopLayer();
							senarr->push_back(cs);
							if (icldetail) dbg_codesen(cs, false);
						}
					}
					else if (strcmp(allcodesen[i], "while") == 0)
					{
						if (icldetail) icl << "while : ";
						code_sen* cs = (code_sen*)fm->_New(sizeof(code_sen), true);
						*cs = code_sen();
						cs->ck = codeKind::ck_while;
						fm->_tempPushLayer();
						fmvecarr<char*> cbs;
						cbs.NULLState();
						cbs.Init(3, false, false);
						int open = 0;
						int h = 0;
						cbs.push_back(allcodesen[i]);
						while (strcmp(allcodesen[i + h], ")") != 0 || open != 1)
						{
							if (strcmp(allcodesen[i + h], "(") == 0)
								open++;
							else if (strcmp(allcodesen[i + h], ")") == 0)
								open--;
							h++;
							cbs.push_back(allcodesen[i + h]);
						}
						set_codesen(cs, cbs);
						while (codeLineVec_Word.at(clwi) <= i + codeLineVec_Word.at(currentLine) && codeLineVec_Word.size() > clwi) {
							clwi += 1;
						}
						clwi -= 1;
						cs->codeline = clwi + 1;

						fm->_tempPopLayer();
						senarr->push_back(cs);
						if (icldetail) dbg_codesen(cs, false);
						i += h;
						StartI = i + 1;
					}
					else if (strcmp(allcodesen[i + 1], "(") == 0)
					{
						if (icldetail) icl << "use Function : ";
						code_sen* cs = (code_sen*)fm->_New(sizeof(code_sen), true);
						*cs = code_sen();
						cs->ck = codeKind::ck_useFunction;
						fm->_tempPushLayer();
						fmvecarr<char*> cbs;
						cbs.NULLState();
						cbs.Init(3, false, false);
						int h = 0;
						while (i + h < allcodesen.size() && strcmp(allcodesen[i + h], ";") != 0)
						{
							cbs.push_back(allcodesen[i + h]);
							h++;
						}
						set_codesen(cs, cbs);
						while (codeLineVec_Word.at(clwi) <= i + codeLineVec_Word.at(currentLine) && codeLineVec_Word.size() > clwi) {
							clwi += 1;
						}
						clwi -= 1;
						cs->codeline = clwi + 1;

						fm->_tempPopLayer();
						senarr->push_back(cs);
						if (icldetail) dbg_codesen(cs, false);
						i += h;
						StartI = i + 1;
					}
					else if (strcmp(allcodesen[i], "return") == 0)
					{
						if (icldetail) icl << "return in Function : ";
						code_sen* cs = (code_sen*)fm->_New(sizeof(code_sen), true);
						*cs = code_sen();
						cs->ck = codeKind::ck_returnInFunction;
						fm->_tempPushLayer();
						fmvecarr<char*> cbs;
						cbs.NULLState();
						cbs.Init(3, false, false);
						int h = 0;
						while (strcmp(allcodesen[i + h], ";") != 0)
						{
							cbs.push_back(allcodesen[i + h]);
							h++;
						}
						set_codesen(cs, cbs);
						while (codeLineVec_Word.at(clwi) <= i + codeLineVec_Word.at(currentLine) && codeLineVec_Word.size() > clwi) {
							clwi += 1;
						}
						clwi -= 1;
						cs->codeline = clwi + 1;

						fm->_tempPopLayer();
						senarr->push_back(cs);
						if (icldetail) dbg_codesen(cs, false);
						i += h;
						StartI = i + 1;
					}
					else if (strcmp(allcodesen[i], "break") == 0)
					{
						if (icldetail) icl << "break : ";
						code_sen* cs = (code_sen*)fm->_New(sizeof(code_sen), true);
						*cs = code_sen();
						cs->ck = codeKind::ck_break;
						fm->_tempPushLayer();
						fmvecarr<char*> cbs;
						cbs.NULLState();
						cbs.Init(3, false, false);
						cbs.push_back(allcodesen[i]);
						set_codesen(cs, cbs);
						while (codeLineVec_Word.at(clwi) <= i + codeLineVec_Word.at(currentLine) && codeLineVec_Word.size() > clwi) {
							clwi += 1;
						}
						clwi -= 1;
						cs->codeline = clwi + 1;

						fm->_tempPopLayer();
						senarr->push_back(cs);
						if (icldetail) dbg_codesen(cs, false);
						StartI = i + 1;
					}
					else if (strcmp(allcodesen[i], "continue") == 0)
					{
						if (icldetail) icl << "continue : ";
						code_sen* cs = (code_sen*)fm->_New(sizeof(code_sen), true);
						*cs = code_sen();
						cs->ck = codeKind::ck_continue;
						fm->_tempPushLayer();
						fmvecarr<char*> cbs;
						cbs.NULLState();
						cbs.Init(3, false, false);
						cbs.push_back(allcodesen[i]);
						set_codesen(cs, cbs);
						while (codeLineVec_Word.at(clwi) <= i + codeLineVec_Word.at(currentLine) && codeLineVec_Word.size() > clwi) {
							clwi += 1;
						}
						clwi -= 1;
						cs->codeline = clwi + 1;

						fm->_tempPopLayer();
						senarr->push_back(cs);
						if (icldetail) dbg_codesen(cs, false);
						StartI = i + 1;
					}

					if (icldetail) icl << " ~ [" << i << "]" << endl;
				}

				if (icldetail) icl << "BakeCode_ScanCodes...";
			}
			else if (strcmp(ScanMod, "struct") == 0)
			{
				bool icldetail = GetICLFlag(ICL_FLAG::BakeCode_ScanStructTypes);
				if (icldetail) icl << "start" << endl;

				for (int i = 0; i < (int)allcodesen.size() - 1; i++)
				{

					if (strcmp(allcodesen[i], "struct") == 0)
					{
						code_sen* cs = (code_sen*)fm->_New(sizeof(code_sen), true);
						*cs = code_sen();
						cs->ck = codeKind::ck_addStruct;
						fm->_tempPushLayer();
						fmvecarr<char*> cbs;
						cbs.NULLState();
						cbs.Init(3, false, false);

						int open = 0;
						int h = 0;

						if (icldetail) icl << "BakeCode_ScanStructTypes : discover struct in " << i << "..finish" << endl;

						if (icldetail) icl << "BakeCode_ScanStructTypes : read struct inside..." << endl;

						cbs.push_back(allcodesen[i]);
						while (strcmp(allcodesen[i + h], "}") != 0 || open != 1)
						{
							if (strcmp(allcodesen[i + h], "{") == 0)
								open++;
							else if (strcmp(allcodesen[i + h], "}") == 0)
								open--;
							h++;
							cbs.push_back(allcodesen[i + h]);
						}
						if (icldetail) {
							for (int i = 0; i < cbs.size(); ++i) {
								icl << cbs.at(i) << " ";
							}
							icl << endl;

							icl << "BakeCode_ScanStructTypes : ...finish" << endl;
						}

						if (icldetail) icl << "BakeCode_ScanStructTypes : read struct member..." << endl;
						fmvecarr<char*> bd;
						bd.NULLState();
						bd.Init(10, false, false);
						for (int i = 0; i < cbs.size(); ++i)
						{
							bd.push_back(cbs[i]);
						}
						h = 0;
						while (strcmp(bd[0], "{") != 0)
						{
							bd.erase(0);
							h++;
						}
						bd.erase(0);
						bd.pop_back();
						if (icldetail) {
							for (int i = 0; i < bd.size(); ++i) {
								icl << bd.at(i) << " ";
							}
							icl << endl;

							icl << "BakeCode_ScanStructTypes : ...finish" << endl;
						}

						if (icldetail) icl << "BakeCode_ScanStructTypes : add struct member code block...";
						fmvecarr<code_sen*>* cbv = AddCodeFromBlockData(bd, "none", clwi);
						if (icldetail) icl << "finish" << endl;

						cs->codeblocks = (fmvecarr<int*> *)fm->_New(sizeof(fmvecarr<int*>), true);
						cs->codeblocks->NULLState();
						cs->codeblocks->Init(8, false, true);
						Init_VPTR<fmvecarr<int*>*>(cs->codeblocks);

						for (int u = 0; u < (int)cbv->size(); u++)
						{
							cs->codeblocks->push_back(reinterpret_cast<int*>((*cbv)[u]));
						}

						cbv->release();
						fm->_Delete((byte8*)cbv, sizeof(fmvecarr<code_sen*>));

						set_codesen(cs, cbs);
						fm->_tempPopLayer();
						senarr->push_back(cs);
					}
				}

				if (icldetail) icl << "BakeCode_ScanStructTypes...";
			}
		}

		return senarr;
	}

	int get_global_address_with_globalv_name(char* name)
	{
		for (int i = 0; i < globalVariables.size(); ++i)
		{
			NamingData nd;
			nd = *globalVariables.at(i);
			if (strcmp(nd.name, name) == 0)
			{
				return nd.add_address;
			}
		}
		return -1;
	}

	int get_address_with_name(char* name)
	{
		bool found = false;
		int si = 0;
		int sk = 0;
		for (int i = blockstack.size() - 1; i >= 0; --i)
		{
			for (int k = 0; k < blockstack[i]->variable_data.size(); ++k)
			{
				if (strcmp(blockstack[i]->variable_data[k].name, name) == 0)
				{
					found = true;
					si = i;
					sk = k;
					break;
				}
			}

			if (found)
			{
				break;
			}
		}

		if (found)
		{
			int addadd_z = 0;
			for (int i = 0; i <= si - 1; ++i)
			{
				addadd_z += blockstack[i]->add_address_up;
			}

			return addadd_z + blockstack[si]->variable_data[sk].add_address;
		}

		return -1;
	}

	type_data* get_type_with_namesen(sen* tname)
	{
		type_data* td = nullptr;
		type_data* ntd = nullptr;
		char* bt = tname->at(0).data.str;
		for (int i = 0; i < types.size(); ++i)
		{
			if (strcmp(bt, types[i]->name.c_str()) == 0)
			{
				td = types[i];
				break;
			}
		}

		if (td == nullptr) {
			for (int i = 0; i < extension.size(); ++i)
			{
				ICB_Extension* ext = extension.at(i);
				for (int k = 0; k < ext->exstructArr.size(); ++k) {
					if (strcmp(bt, ext->exstructArr.at(k)->name.c_str()) == 0)
					{
						td = ext->exstructArr.at(k);
						break;
					}
				}
				if (td != nullptr) break;
			}
		}

		if (td == nullptr) {
			UpdateErrMsg(1, "<%s> type is not exist.", bt);
			return nullptr;
		}

		for (int i = 1; i < tname->size(); ++i)
		{
			char c = 0;
			if (tname->at(i).data.str != nullptr)
			{
				//cout << (int *)tname->at(i).data.str << endl;
				c = tname->at(i).data.str[0];
			}
			if (c == '*')
			{
				ntd = get_addpointer_type(td);
				if (i != 1)
				{
					fm->_Delete((byte8*)td, sizeof(type_data));
				}
				td = ntd;
			}
			else if (c == '[')
			{
				int siz = atoi(tname->at(i + 1).data.str);
				// get array type with siz
				ntd = get_array_type(td, siz);
				td = ntd;
				i += 2;
			}
			else {
				char cstr[2] = { c, 0 };
				UpdateErrMsg(2, "<%s> type typeoperation is not exist", cstr);
				return nullptr;
			}
		}
		return td;
	}

	type_data* get_type_with_global_vname(char* name)
	{
		for (int i = 0; i < globalVariables.size(); ++i)
		{
			NamingData nd;
			nd = *globalVariables.at(i);
			if (strcmp(nd.name, name) == 0)
			{
				return nd.td;
			}
		}

		return nullptr;
	}

	type_data* get_type_with_vname(char* name)
	{
		bool found = false;
		int si = 0;
		int sk = 0;
		for (int i = blockstack.size() - 1; i >= 0; --i)
		{
			for (int k = 0; k < blockstack[i]->variable_data.size(); ++k)
			{
				if (strcmp(blockstack[i]->variable_data[k].name, name) == 0)
				{
					found = true;
					si = i;
					sk = k;
					break;
				}
			}

			if (found)
			{
				break;
			}
		}

		if (found)
		{
			return blockstack[si]->variable_data[sk].td;
		}

		return nullptr;
	}

	int combine_opertype(int type0, int type1)
	{
		int max = (type0 >= type1) ? type0 : type1;
		if ((type0 + type1) % 2 == 1 && max % 2 == 1)
		{
			return max - 1;
		}
		else
		{
			return max;
		}
	}

	temp_mem* get_asm_from_sen(sen* ten, bool is_a, bool isvalue)
	{
		// fm->dbg_fm1_lifecheck();
		temp_mem* tm = nullptr;
		fmvecarr<sen*> segs; // term
		sen* vtemp = nullptr;
		sen* temp = nullptr;
		if (ten->at(0).type == 'a')
		{
			tm = reinterpret_cast<temp_mem*>(ten->at(0).data.str);
			if (isvalue) {
				if (tm->isValue == false) {
					tm->valuetype_detail = get_sub_type(tm->valuetype_detail);
					ICB_ERR_CHECK(ERR_GETASMFROMSEN_EXIT);
					tm->valuetype = get_int_with_basictype(tm->valuetype_detail);
					tm->isValue = true;
					switch (tm->registerMod) {
					case 'A':
					{
						if (is_a) {
							tm->mem.push_back((byte8)insttype::IT_PUSH_A_FROM_VALUE_OF_A); // a = *a
						}
						else {
							tm->mem.push_back((byte8)insttype::PUSH_B_FROM_A); // b = a
							tm->mem.push_back((byte8)insttype::IT_POP_A); // pop a
							tm->mem.push_back((byte8)insttype::IT_PUSH_B_FROM_VALUE_OF_B); // b = *b
						}
					}
					break;
					case 'B':
					{
						if (is_a) {
							tm->mem.push_back((byte8)insttype::PUSH_A_FROM_B); // a = b
							tm->mem.push_back((byte8)insttype::IT_POP_B); // pop b
							tm->mem.push_back((byte8)insttype::IT_PUSH_A_FROM_VALUE_OF_A); // a = *a
						}
						else {
							tm->mem.push_back((byte8)insttype::IT_PUSH_B_FROM_VALUE_OF_B); // b = *b
						}
					}
					break;
					}
				}
				else {
					switch (tm->registerMod) {
					case 'A':
					{
						if (is_a == false) {
							tm->mem.push_back((byte8)insttype::PUSH_B_FROM_A); // b = a
							tm->mem.push_back((byte8)insttype::IT_POP_A); // pop a
						}
					}
					break;
					case 'B':
					{
						if (is_a) {
							tm->mem.push_back((byte8)insttype::PUSH_A_FROM_B); // a = b
							tm->mem.push_back((byte8)insttype::IT_POP_B); // pop b
						}
					}
					break;
					}
				}
			}
			else {
				if (tm->isValue) {
					//WARN : imposible task. waring.
				}
				else {
					switch (tm->registerMod) {
					case 'A':
					{
						if (is_a == false) {
							tm->mem.push_back((byte8)insttype::PUSH_B_FROM_A); // b = a
							tm->mem.push_back((byte8)insttype::IT_POP_A); // pop a
						}
					}
					break;
					case 'B':
					{
						if (is_a) {
							tm->mem.push_back((byte8)insttype::PUSH_A_FROM_B); // a = b
							tm->mem.push_back((byte8)insttype::IT_POP_B); // pop b
						}
					}
					break;
					}
				}
			}

			return tm;
		}

		tm = (temp_mem*)fm->_New(sizeof(temp_mem), true);
		// fm->dbg_fm1_lifecheck();
		tm->mem.NULLState();
		tm->mem.Init(2, false, true);
		tm->valuetype_detail = nullptr;

		//expr in only one function check.
		bool is_only_function = false;
		int stack = 0;
		int i;
		for (i = 0; i < ten->size(); ++i) {
			if (strcmp(ten->at(i).data.str, "(") == 0) {
				stack += 1;
			}
			else if (strcmp(ten->at(i).data.str, ")") == 0) {
				stack -= 1;
				if (stack == 0) break;
			}
		}
		if (i == ten->size() - 1) {
			is_only_function = true;
		}

		if ((ten->size() > 1 && strcmp(ten->at(1).data.str, "(") == 0) && is_only_function)
		{
			// function
			// use compile_useFunction()
			func_data* fd = nullptr;

			sen* code = ten;
			int loc = code->size() - 1;
			sen* inner_params = wbss.oc_search_inv(code, loc, "(", ")");
			if (inner_params == nullptr) {
				UpdateErrMsg(6, "Unpaired parentheses exist.", nullptr);
				ICB_ERR_CHECK(ERR_GETASMFROMSEN_USEFUNCTION_RELEASE_TM);
			}
			// wbss.dbg_sen(code);
			// wbss.dbg_sen(inner_params);
			int nameloc = loc - inner_params->size();
			char* funcname = code->at(nameloc).data.str;

			bool isext = false;
			int extID = 0;
			int exfuncID = 0;
			for (int i = 0; i < extension.size(); ++i)
			{
				ICB_Extension* ext = extension.at(i);
				for (int k = 0; k < ext->exfuncArr.size(); ++k)
				{
					func_data* efd = ext->exfuncArr[k];
					if (strcmp(funcname, efd->name.c_str()) == 0)
					{
						isext = true;
						//sen* typen = wbss.sen_cut(code, 0, nameloc - 1);
						//sen* params_sen = wbss.sen_cut(code, nameloc + 2, loc - 1);
						fd = efd;
						extID = i;
						exfuncID = k;
						break;
					}
				}
			}

			if (strcmp(funcname, "dbg") == 0)
			{
				inner_params->pop_back();
				inner_params->erase(0);
				//wbss.dbg_sen(inner_params);
				int coma = wbss.search_word_first_in_specific_oc_layer(inner_params, 0, "(", ")", 0, ",");
				int savecomma = -1;
				temp_mem* rtm = nullptr;
				sen* param_sen = nullptr;
				while (coma != -1)
				{
					param_sen = wbss.sen_cut(inner_params, savecomma + 1, coma - 1);
					//wbss.dbg_sen(param_sen);
					rtm = get_asm_from_sen(param_sen, true, true);
					ICB_ERR_CHECK(ERR_GETASMFROMSEN_USEFUNCTION_DBG_RTM);

					for (int k = 0; k < rtm->mem.size(); ++k)
					{
						tm->mem.push_back(rtm->mem[k]);
					}
					tm->mem.push_back((byte8)insttype::IT_DBG_A);
					tm->mem.push_back((byte8)rtm->valuetype);
					savecomma = coma;
					coma = wbss.search_word_first_in_specific_oc_layer(inner_params, savecomma + 1, "(", ")", 0, ",");
					release_tempmem(rtm);
					param_sen->release();
					fm->_Delete((byte8*)param_sen, sizeof(sen));
				}

				param_sen = wbss.sen_cut(inner_params, savecomma + 1, inner_params->size() - 1);
				//wbss.dbg_sen(param_sen);
				rtm = get_asm_from_sen(param_sen, true, true);
				ICB_ERR_CHECK(ERR_GETASMFROMSEN_USEFUNCTION_DBG_RTM);

				for (int k = 0; k < rtm->mem.size(); ++k)
				{
					tm->mem.push_back(rtm->mem[k]);
				}
				tm->mem.push_back((byte8)insttype::IT_DBG_A);
				tm->mem.push_back((byte8)rtm->valuetype);

				inner_params->release();
				fm->_Delete((byte8*)inner_params, sizeof(sen));
				param_sen->release();
				fm->_Delete((byte8*)param_sen, sizeof(sen));
				release_tempmem(rtm);
				return tm;

			ERR_GETASMFROMSEN_USEFUNCTION_DBG_RTM:
				inner_params->release();
				fm->_Delete((byte8*)inner_params, sizeof(sen));
				param_sen->release();
				fm->_Delete((byte8*)param_sen, sizeof(sen));
				release_tempmem(rtm);
				release_tempmem(tm);
				return nullptr;
			}
			else if (strcmp(funcname, "inp") == 0)
			{
				inner_params->pop_back();
				inner_params->erase(0);
				temp_mem* rtm = get_asm_from_sen(inner_params, true, true);
				ICB_ERR_CHECK(ERR_GETASMFROMSEN_USEFUNCTION_INP_RTM);
				for (int k = 0; k < rtm->mem.size(); ++k)
				{
					tm->mem.push_back(rtm->mem[k]);
				}
				tm->mem.push_back((byte8)insttype::IT_INP_A_PTR);
				tm->mem.push_back((byte8)rtm->valuetype);

				inner_params->release();
				fm->_Delete((byte8*)inner_params, sizeof(sen));
				release_tempmem(rtm);
				return tm;

			ERR_GETASMFROMSEN_USEFUNCTION_INP_RTM:
				inner_params->release();
				fm->_Delete((byte8*)inner_params, sizeof(sen));
				release_tempmem(rtm);
				release_tempmem(tm);
				return nullptr;
			}

			if (isext == false) {
				sen* params_sen = nullptr;
				sen* param_sen = nullptr;
				temp_mem* rtm = nullptr;
				int coma = 0;
				int savecoma = -1;
				int last = 0;
				int addadd = 0;
				int paramid = 0;
				int paramCount = 0;
				int ll = 0;
				fd = get_func_with_name(code->at(nameloc).data.str);
				ICB_ERR_CHECK(ERR_GETASMFROMSEN_USEFUNCTION_NORMAL_FD_NOTEXIST);

				params_sen = wbss.sen_cut(code, nameloc + 2, loc - 1);
				if (params_sen->size() == 0)
				{
					tm->mem.push_back((byte8)insttype::IT_FUNC); // FUNC
					tm->mem.push_back((byte8)insttype::IT_FUNCJMP); // jmp
					byte8 bb[4] = {};
					*reinterpret_cast<uint*>(bb) = (uint)(fd->start_pc - &mem[0]);
					for (int u = 0; u < 4; ++u)
					{
						tm->mem.push_back(bb[u]);
					}

					inner_params->release();
					fm->_Delete((byte8*)inner_params, sizeof(sen));

					params_sen->release();
					fm->_Delete((byte8*)params_sen, sizeof(sen));
					return tm;
				}

				//wbss.dbg_sen(params_sen);
				coma = wbss.search_word_first_in_specific_oc_layer(params_sen, 0, "(", ")", 0, ",");
				savecoma = -1;
				last = params_sen->size() - 1;

				addadd = 0;
				paramid = 0;
				paramCount = 0;

				tm->mem.push_back((byte8)insttype::IT_FUNC);

				while (coma != -1)
				{
					param_sen = wbss.sen_cut(params_sen, savecoma + 1, coma - 1);
					//wbss.dbg_sen(param_sen);
					rtm = get_asm_from_sen(param_sen, true, true);
					ICB_ERR_CHECK(ERR_GETASMFROMSEN_USEFUNCTION_NORMAL_RTM);

					if (rtm->valuetype_detail->typetype == 's')
					{
						temp_mem* ptrtm = get_asm_from_sen(param_sen, true, false);
						for (int i = 0; i < ptrtm->mem.size(); ++i)
						{
							tm->mem.push_back(ptrtm->mem[i]);
						}
						release_tempmem(ptrtm);
						ICB_ERR_CHECK(ERR_GETASMFROMSEN_USEFUNCTION_NORMAL_RTM);
					}
					else
					{
						for (int i = 0; i < rtm->mem.size(); ++i)
						{
							tm->mem.push_back(rtm->mem[i]);
						}

						casting_type ct = get_cast_type(rtm->valuetype, get_int_with_basictype(fd->param_data.at(paramCount).td));
						if (ct != casting_type::nocasting) {
							tm->mem.push_back((byte8)insttype::IT_CASTING_A);
							tm->mem.push_back((byte8)ct);
						}
						/*
						int ll = tm->mem.size();
						for(int k=0;k<4;++k){
							tm->mem.push_back(0);
						}
						*reinterpret_cast<uint*>(&mem[ll]) = (uint)ct;
						*/
					}

					if (fd->param_data[paramid].td->typetype != 's')
					{
						switch (fd->param_data[paramid].td->typesiz)
						{
						case 1:
							tm->mem.push_back((byte8)insttype::IT_PARAM_1); // param
							break;
						case 2:
							tm->mem.push_back((byte8)insttype::IT_PARAM_2); // param
							break;
						case 4:
							tm->mem.push_back((byte8)insttype::IT_PARAM_4); // param
							break;
						case 8:
							tm->mem.push_back((byte8)insttype::PARAM_8);
							break;
						}
					}
					else
					{
						tm->mem.push_back((byte8)insttype::PARAM_N_COPY_BY_ADDRESS); // param N by address(a)
						byte8* N = (byte8*)&fd->param_data[paramid].td->typesiz;
						// write siz of struct type
						for (int i = 0; i < 4; ++i)
						{
							tm->mem.push_back(N[i]);
						}
					}

					++paramid;
					savecoma = coma;
					coma = wbss.search_word_first_in_specific_oc_layer(params_sen, coma + 1, "(", ")", 0, ",");
					// coma = wbss.search_word_first(coma + 1, params_sen, ",");
					paramCount += 1;

					param_sen->release();
					fm->_Delete((byte8*)param_sen, sizeof(sen));
					release_tempmem(rtm);

					continue;


				}

				//wbss.dbg_sen(params_sen);
				param_sen = wbss.sen_cut(params_sen, savecoma + 1, last);
				//wbss.dbg_sen(param_sen);
				rtm = get_asm_from_sen(param_sen, true, true);
				ICB_ERR_CHECK(ERR_GETASMFROMSEN_USEFUNCTION_NORMAL_RTM);
				if (rtm->valuetype_detail->typetype == 's')
				{
					temp_mem* ptrtm = get_asm_from_sen(param_sen, true, false);
					for (int i = 0; i < ptrtm->mem.size(); ++i)
					{
						tm->mem.push_back(ptrtm->mem[i]);
					}
					release_tempmem(ptrtm);
					ICB_ERR_CHECK(ERR_GETASMFROMSEN_USEFUNCTION_NORMAL_RTM);
				}
				else
				{
					for (int i = 0; i < rtm->mem.size(); ++i)
					{
						tm->mem.push_back(rtm->mem[i]);
					}
					casting_type ct = get_cast_type(rtm->valuetype, get_int_with_basictype(fd->param_data.at(paramCount).td));
					if (ct != casting_type::nocasting) {
						tm->mem.push_back((byte8)insttype::IT_CASTING_A);
						tm->mem.push_back((byte8)ct);
					}
					/*
					int ll = tm->mem.size();
					for(int k=0;k<4;++k){
						tm->mem.push_back(0);
					}
					*reinterpret_cast<uint*>(&mem[ll]) = (uint)ct;
					*/
				}

				if (fd->param_data[paramid].td->typetype != 's')
				{
					switch (fd->param_data[paramid].td->typesiz)
					{
					case 1:
						tm->mem.push_back((byte8)insttype::IT_PARAM_1); // param
						break;
					case 2:
						tm->mem.push_back((byte8)insttype::IT_PARAM_2); // param
						break;
					case 4:
						tm->mem.push_back((byte8)insttype::IT_PARAM_4); // param
						break;
					case 8:
						tm->mem.push_back((byte8)insttype::PARAM_8);
						break;
					}
				}
				else
				{
					tm->mem.push_back((byte8)insttype::PARAM_N_COPY_BY_ADDRESS); // param N by address(a)
					byte8* N = (byte8*)&fd->param_data[paramid].td->typesiz;
					// write siz of struct type
					for (int i = 0; i < 4; ++i)
					{
						tm->mem.push_back(N[i]);
					}
				}

				tm->mem.push_back((byte8)insttype::IT_FUNCJMP); // jmp
				ll = tm->mem.size();
				for (int k = 0; k < 4; ++k)
				{
					tm->mem.push_back(0);
				}
				*reinterpret_cast<uint*>(&tm->mem[ll]) = (uint)(fd->start_pc - &mem[0]);

				tm->valuetype = get_int_with_basictype(fd->returntype);
				tm->valuetype_detail = fd->returntype;

				inner_params->release();
				fm->_Delete((byte8*)inner_params, sizeof(sen));

				params_sen->release();
				fm->_Delete((byte8*)params_sen, sizeof(sen));

				param_sen->release();
				fm->_Delete((byte8*)param_sen, sizeof(sen));

				release_tempmem(rtm);

				return tm;

			ERR_GETASMFROMSEN_USEFUNCTION_NORMAL_RTM:
				inner_params->release();
				fm->_Delete((byte8*)inner_params, sizeof(sen));
				params_sen->release();
				fm->_Delete((byte8*)params_sen, sizeof(sen));
				param_sen->release();
				fm->_Delete((byte8*)param_sen, sizeof(sen));
				release_tempmem(rtm);
				release_tempmem(tm);

			ERR_GETASMFROMSEN_USEFUNCTION_NORMAL_FD_NOTEXIST:
				return nullptr;
			}
			else
			{
				sen* params_sen = wbss.sen_cut(code, nameloc + 2, loc - 1);
				int last = 0;
				int coma = 0;
				int savecoma = 0;
				int paramCount = 0;
				temp_mem* rtm = nullptr;
				sen* param_sen = nullptr;
				int ll = 0;

				if (params_sen->size() == 0)
				{
					tm->mem.push_back((byte8)insttype::IT_FUNC);		// FUNC
					tm->mem.push_back((byte8)insttype::EXTENSION_INST); // jmp
					int ll = tm->mem.size();

					for (int k = 0; k < 4; ++k)
					{
						tm->mem.push_back(0);
					}
					*reinterpret_cast<uint*>(&tm->mem[ll]) = (uint)(extID);
					ll += 4;
					for (int k = 0; k < 4; ++k)
					{
						tm->mem.push_back(0);
					}
					*reinterpret_cast<uint*>(&tm->mem[ll]) = (uint)(exfuncID); // byte8* but real value is function pointer of extension.

					inner_params->release();
					fm->_Delete((byte8*)inner_params, sizeof(sen));

					params_sen->release();
					fm->_Delete((byte8*)params_sen, sizeof(sen));
					return tm;
				}

				//wbss.dbg_sen(params_sen);
				last = params_sen->size() - 1;

				coma = wbss.search_word_end_in_specific_oc_layer(params_sen, last, "(", ")", 0, ",");
				savecoma = last + 1;

				paramCount = fd->param_data.size() - 1;

				tm->mem.push_back((byte8)insttype::IT_FUNC);

				while (coma != -1)
				{
					param_sen = wbss.sen_cut(params_sen, coma + 1, savecoma - 1);
					//wbss.dbg_sen(param_sen);
					rtm = get_asm_from_sen(param_sen, true, true);
					ICB_ERR_CHECK(ERR_GETASMFROMSEN_USEFUNCTION_EXT_RTM);

					if (rtm->valuetype_detail->typetype == 's')
					{
						temp_mem* ptrtm = get_asm_from_sen(param_sen, true, false);
						for (int i = 0; i < ptrtm->mem.size(); ++i)
						{
							tm->mem.push_back(ptrtm->mem[i]);
						}
						release_tempmem(ptrtm);
						ICB_ERR_CHECK(ERR_GETASMFROMSEN_USEFUNCTION_EXT_RTM);
					}
					else
					{
						for (int i = 0; i < rtm->mem.size(); ++i)
						{
							tm->mem.push_back(rtm->mem[i]);
						}

						casting_type ct = get_cast_type(rtm->valuetype, get_int_with_basictype(fd->param_data.at(paramCount).td));
						if (ct != casting_type::nocasting) {
							tm->mem.push_back((byte8)insttype::IT_CASTING_A);
							tm->mem.push_back((byte8)ct);
						}
						/*
						int ll = tm->mem.size();
						for(int k=0;k<4;++k){
							tm->mem.push_back(0);
						}
						*reinterpret_cast<uint*>(&mem[ll]) = (uint)ct;
						*/
					}

					if (fd->param_data[paramCount].td->typetype != 's')
					{
						switch (fd->param_data[paramCount].td->typesiz)
						{
						case 1:
							tm->mem.push_back((byte8)insttype::IT_PARAM_1); // param
							break;
						case 2:
							tm->mem.push_back((byte8)insttype::IT_PARAM_2); // param
							break;
						case 4:
							tm->mem.push_back((byte8)insttype::IT_PARAM_4); // param
							break;
						case 8:
							tm->mem.push_back((byte8)insttype::PARAM_8);
							break;
						}
					}
					else
					{
						tm->mem.push_back((byte8)insttype::PARAM_N_COPY_BY_ADDRESS); // param N by address(a)
						byte8* N = (byte8*)&fd->param_data[paramCount].td->typesiz;
						// write siz of struct type
						for (int i = 0; i < 4; ++i)
						{
							tm->mem.push_back(N[i]);
						}
					}

					savecoma = coma;
					coma = wbss.search_word_end_in_specific_oc_layer(params_sen, savecoma - 1, "(", ")", 0, ",");
					// coma = wbss.search_word_first(coma + 1, params_sen, ",");
					--paramCount;

					param_sen->release();
					fm->_Delete((byte8*)param_sen, sizeof(sen));
					release_tempmem(rtm);
				}

				//wbss.dbg_sen(params_sen);
				param_sen = wbss.sen_cut(params_sen, 0, savecoma - 1);
				//wbss.dbg_sen(param_sen);
				rtm = get_asm_from_sen(param_sen, true, true);
				ICB_ERR_CHECK(ERR_GETASMFROMSEN_USEFUNCTION_EXT_RTM);
				if (rtm->valuetype_detail->typetype == 's')
				{
					temp_mem* ptrtm = get_asm_from_sen(param_sen, true, false);
					for (int i = 0; i < ptrtm->mem.size(); ++i)
					{
						tm->mem.push_back(ptrtm->mem[i]);
					}
					release_tempmem(ptrtm);
					ICB_ERR_CHECK(ERR_GETASMFROMSEN_USEFUNCTION_EXT_RTM);
				}
				else
				{
					for (int i = 0; i < rtm->mem.size(); ++i)
					{
						tm->mem.push_back(rtm->mem[i]);
					}
					casting_type ct = get_cast_type(rtm->valuetype, get_int_with_basictype(fd->param_data.at(paramCount).td));
					if (ct != casting_type::nocasting) {
						tm->mem.push_back((byte8)insttype::IT_CASTING_A);
						tm->mem.push_back((byte8)ct);
					}

					/*
					int ll = tm->mem.size();
					for(int k=0;k<4;++k){
						tm->mem.push_back(0);
					}
					*reinterpret_cast<uint*>(&mem[ll]) = (uint)ct;
					*/
				}

				if (fd->param_data[paramCount].td->typetype != 's')
				{
					switch (fd->param_data[paramCount].td->typesiz)
					{
					case 1:
						tm->mem.push_back((byte8)insttype::IT_PARAM_1); // param
						break;
					case 2:
						tm->mem.push_back((byte8)insttype::IT_PARAM_2); // param
						break;
					case 4:
						tm->mem.push_back((byte8)insttype::IT_PARAM_4); // param
						break;
					case 8:
						tm->mem.push_back((byte8)insttype::PARAM_8);
						break;
					}
				}
				else
				{
					tm->mem.push_back((byte8)insttype::PARAM_N_COPY_BY_ADDRESS); // param N by address(a)
					byte8* N = (byte8*)&fd->param_data[paramCount].td->typesiz;
					// write siz of struct type
					for (int i = 0; i < 4; ++i)
					{
						tm->mem.push_back(N[i]);
					}
				}

				tm->mem.push_back((byte8)insttype::EXTENSION_INST); // ext instruction
				ll = tm->mem.size();
				for (int k = 0; k < 4; ++k)
				{
					tm->mem.push_back(0);
				}
				*reinterpret_cast<uint*>(&tm->mem[ll]) = (uint)(extID);
				ll += 4;
				for (int k = 0; k < 4; ++k)
				{
					tm->mem.push_back(0);
				}
				*reinterpret_cast<uint*>(&tm->mem[ll]) = (uint)(exfuncID); // byte8* but real value is function pointer of extension.

				tm->valuetype = get_int_with_basictype(fd->returntype);
				tm->valuetype_detail = fd->returntype;

				inner_params->release();
				fm->_Delete((byte8*)inner_params, sizeof(sen));

				params_sen->release();
				fm->_Delete((byte8*)params_sen, sizeof(sen));

				param_sen->release();
				fm->_Delete((byte8*)param_sen, sizeof(sen));

				release_tempmem(rtm);

				return tm;

			ERR_GETASMFROMSEN_USEFUNCTION_EXT_RTM:
				inner_params->release();
				fm->_Delete((byte8*)inner_params, sizeof(sen));
				params_sen->release();
				fm->_Delete((byte8*)params_sen, sizeof(sen));
				param_sen->release();
				fm->_Delete((byte8*)param_sen, sizeof(sen));
				release_tempmem(rtm);
			}

		ERR_GETASMFROMSEN_USEFUNCTION_RELEASE_TM:
			release_tempmem(tm);
			return nullptr;
		}

		if (ten->size() == 1)
		{
			// if variable name or const
			int gvarid = get_global_address_with_globalv_name(ten->at(0).data.str);
			if (gvarid >= 0)
			{
				type_data* td = get_type_with_global_vname(ten->at(0).data.str);
				if (is_a)
				{
					tm->mem.push_back((byte8)insttype::PUSH_A_GLOBAL_VARIABLE_ADDRESS);
					byte8 add[4];
					*reinterpret_cast<uint*>(&add[0]) = gvarid;
					for (int i = 0; i < 4; ++i)
					{
						tm->mem.push_back(add[i]);
					}
				}
				else
				{
					tm->mem.push_back((byte8)insttype::PUSH_B_GLOBAL_VARIABLE_ADDRESS);
					byte8 add[4];
					*reinterpret_cast<uint*>(&add[0]) = gvarid;
					for (int i = 0; i < 4; ++i)
					{
						tm->mem.push_back(add[i]);
					}
				}

				if (isvalue && td->typetype != 's')
				{
					if (is_a)
					{
						tm->mem.push_back((byte8)insttype::IT_PUSH_A_FROM_VALUE_OF_A);
					}
					else
					{
						tm->mem.push_back((byte8)insttype::IT_PUSH_B_FROM_VALUE_OF_B);
					}

					tm->valuetype = get_int_with_basictype(td);
					tm->valuetype_detail = td;
					return tm;
				}
				else
				{
					tm->valuetype = 8;
					tm->valuetype_detail = get_addpointer_type(td);
					return tm;
				}
			}

			//wbss.dbg_sen(ten);
			int varid = get_address_with_name(ten->at(0).data.str);
			if (varid >= 0)
			{
				// variable
				type_data* td = get_type_with_vname(ten->at(0).data.str);
				if (is_a)
				{
					tm->mem.push_back((byte8)insttype::IT_PUSH_TO_A_FROM_ADDRESS_OF_VARIABLE_ID);
					byte8 add[4];
					*reinterpret_cast<uint*>(&add[0]) = varid;
					for (int i = 0; i < 4; ++i)
					{
						tm->mem.push_back(add[i]);
					}
				}
				else
				{
					tm->mem.push_back((byte8)insttype::IT_PUSH_TO_B_FROM_ADDRESS_OF_VARIABLE_ID);
					byte8 add[4];
					*reinterpret_cast<uint*>(&add[0]) = varid;
					for (int i = 0; i < 4; ++i)
					{
						tm->mem.push_back(add[i]);
					}
				}

				if (isvalue && td->typetype != 's')
				{
					if (is_a)
					{
						tm->mem.push_back((byte8)insttype::IT_PUSH_A_FROM_VALUE_OF_A);
					}
					else
					{
						tm->mem.push_back((byte8)insttype::IT_PUSH_B_FROM_VALUE_OF_B);
					}
					tm->valuetype = get_int_with_basictype(td);
					tm->valuetype_detail = td;
					return tm;
				}
				else
				{
					tm->valuetype = 8;
					tm->valuetype_detail = get_addpointer_type(td);
					return tm;
				}
			}
			else
			{
				// const
				fmlcstr str;
				str.NULLState();
				str.Init(2, false);
				str = ten->at(0).data.str;
				TBT t = DecodeTextBlock(str);

				switch (t)
				{
				case TBT::_value_bool:
				{
					bool b = true;
					if (is_a)
					{
						tm->mem.push_back((byte8)insttype::IT_PUSH_A_CONST_4);
					}
					else
					{
						tm->mem.push_back((byte8)insttype::IT_PUSH_B_CONST_4);
					}
					if (strcmp(str.c_str(), "true") == 0)
						b = true;
					else
						b = false;
					for (int k = 0; k < 4; ++k)
					{
						tm->mem.push_back(0);
					}
					*reinterpret_cast<bool*>(&tm->mem[1]) = b;
					tm->valuetype = 7;
					tm->valuetype_detail = get_basic_type_with_int(tm->valuetype);
				}
				break;
				case TBT::_value_integer:
				{
					int a = 0;
					if (is_a)
					{
						tm->mem.push_back((byte8)insttype::IT_PUSH_A_CONST_4);
					}
					else
					{
						tm->mem.push_back((byte8)insttype::IT_PUSH_B_CONST_4);
					}
					a = atoi(str.c_str());
					for (int k = 0; k < 4; ++k)
					{
						tm->mem.push_back(0);
					}
					*reinterpret_cast<int*>(&tm->mem[1]) = a;
					tm->valuetype = 4;
					tm->valuetype_detail = get_basic_type_with_int(tm->valuetype);
				}
				break;
				case TBT::_value_float:
				{
					float a = 0;
					if (is_a)
					{
						tm->mem.push_back((byte8)insttype::IT_PUSH_A_CONST_4);
					}
					else
					{
						tm->mem.push_back((byte8)insttype::IT_PUSH_B_CONST_4);
					}
					a = stof(str.c_str());
					for (int k = 0; k < 4; ++k)
					{
						tm->mem.push_back(0);
					}
					*reinterpret_cast<float*>(&tm->mem[1]) = a;
					tm->valuetype = 6;
					tm->valuetype_detail = get_basic_type_with_int(tm->valuetype);
				}
				break;
				case TBT::_value_char:
				{
					if (is_a)
					{
						tm->mem.push_back((byte8)insttype::IT_PUSH_A_CONST_1);
					}
					else
					{
						tm->mem.push_back((byte8)insttype::IT_PUSH_B_CONST_1);
					}
					if (str[1] != '\\')
					{
						tm->mem.push_back(str[1]);
					}
					else
					{
						switch (str[2])
						{
						case 'n':
							tm->mem.push_back('\n');
							break;
						case '0':
							tm->mem.push_back(0);
							break;
						case 't':
							tm->mem.push_back('\t');
							break;
						case '\\':
							tm->mem.push_back('\\');
							break;
						case '\'':
							tm->mem.push_back('\'');
							break;
						case '\"':
							tm->mem.push_back('\"');
							break;
						default:
							tm->mem.push_back(0);
							break;
						}
					}
					tm->valuetype = 0;
					tm->valuetype_detail = get_basic_type_with_int(tm->valuetype);
				}
				break;
				case TBT::_value_str:
				{
					if (is_a)
					{
						tm->mem.push_back((byte8)insttype::IT_SET_A_CONST_STRING);
					}
					else
					{
						tm->mem.push_back((byte8)insttype::IT_SET_B_CONST_STRING);
					}
					int max = strlen(str.c_str()) - 1;
					char cc[4] = {};
					*reinterpret_cast<uint*>(cc) = max;
					for (int k = 0; k < 4; ++k)
					{
						tm->mem.push_back(cc[k]);
					}
					int stack = 1;
					for (int k = 1; k < max; ++k)
					{
						if (str[k] == '\\')
						{
							stack += 1;
							k += 1;
							switch (str[k])
							{
							case 'n':
								tm->mem.push_back('\n');
								break;
							case '0':
								tm->mem.push_back(0);
								break;
							case 't':
								tm->mem.push_back('\t');
								break;
							case '\\':
								tm->mem.push_back('\\');
								break;
							case '\'':
								tm->mem.push_back('\'');
								break;
							case '\"':
								tm->mem.push_back('\"');
								break;
							default:
								tm->mem.push_back(0);
								break;
							}
							continue;
						}
						tm->mem.push_back(str[k]);
					}
					for (int k = 0; k < stack; ++k) {
						tm->mem.push_back(0);
					}
					tm->valuetype = 8;
					tm->valuetype_detail = get_addpointer_type(&basictype[1]);
				}
				break;
				default:
				{
					UpdateErrMsg(5, "Unknown identifier <%s>", str.c_str());
					str.release();
					release_tempmem(tm);
					return nullptr;
				}
				break;
				}
				str.islocal = true;
				return tm;
			}
		}

		// seperate term and operator of expr
		fm->_tempPushLayer(); // this layer pop when return.

		segs.NULLState();
		segs.Init(2, false, false);
		vtemp = (sen*)fm->_New(sizeof(sen), true);
		vtemp->NULLState();
		vtemp->Init(2, false, true);
		bool function_seg = false;

		for (int i = 0; i < ten->size(); ++i)
		{
			char c = ten->at(i).data.str[0];
			/*if (c == '\'') {
				temp = (sen*)fm->_New(sizeof(sen), true);
				temp->NULLState();
				temp->Init(1, false);
				temp->push_back(ten->at(i));

				for (int k = i+1; k < ten->size(); ++k) {
					temp->push_back(ten->at(k));
					char cc = ten->at(k).data.str[0];
					if (cc == '\'') {
						break;
					}
				}
				segs.push_back(temp);
				i += temp->size() - 1;
			}
			else if(c == '\"') {
				temp = (sen*)fm->_New(sizeof(sen), true);
				temp->NULLState();
				temp->Init(1, false);
				temp->push_back(ten->at(i));
				for (int k = i + 1; k < ten->size(); ++k) {
					temp->push_back(ten->at(k));
					char cc = ten->at(k).data.str[0];
					if (cc == '\"') {
						break;
					}
				}
				segs.push_back(temp);
				i += temp->size() - 1;
			}
			else */
			if (c == '(')
			{
				if (function_seg == false) {
					temp = wbss.oc_search(ten, i, "(", ")");
					temp->erase(0);
					temp->pop_back();
					//wbss.dbg_sen(temp);
					i += temp->size() + 1;
					//wbss.dbg_sen(segs.at(i));
					segs.push_back(temp);
				}
				else {
					// create 'a' asm type segment
					temp = wbss.oc_search(ten, i, "(", ")");
					for (int k = 0; k < temp->size(); ++k) {
						vtemp->push_back(temp->at(k));
					}
					i += temp->size() - 1;
					function_seg = false;
					segs.push_back(vtemp);
					vtemp = (sen*)fm->_New(sizeof(sen), true);
					vtemp->NULLState();
					vtemp->Init(2, false, true);
				}
			}
			else if (c == '[')
			{
				if (vtemp->size() != 0)
				{
					segs.push_back(vtemp);

					vtemp = (sen*)fm->_New(sizeof(sen), true);
					vtemp->NULLState();
					vtemp->Init(2, false, true);
				}

				vtemp->push_back(ten->at(i));
				segs.push_back(vtemp);

				vtemp = (sen*)fm->_New(sizeof(sen), true);
				vtemp->NULLState();
				vtemp->Init(2, false, true);

				temp = wbss.oc_search(ten, i, "[", "]");
				temp->pop_back();
				temp->erase(0);

				segs.push_back(temp);

				i += temp->size();
			}
			else if ((('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z')) || (c == '_' || ('0' <= c && c <= '9')))
			{
				vtemp->push_back(ten->at(i));
				if ((('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z')) || (c == '_')) {
					function_seg = true;
				}
			}
			else if (c == '\'' || c == '\"') {
				vtemp->push_back(ten->at(i));
			}
			else
			{
				if (vtemp->size() != 0)
				{
					segs.push_back(vtemp);

					vtemp = (sen*)fm->_New(sizeof(sen), true);
					vtemp->NULLState();
					vtemp->Init(2, false, true);
				}

				vtemp->push_back(ten->at(i));
				vtemp->at(0).type = 'o';
				segs.push_back(vtemp);

				vtemp = (sen*)fm->_New(sizeof(sen), true);
				vtemp->NULLState();
				vtemp->Init(2, false, true);
			}
		}

		if (vtemp->size() > 0)
		{
			segs.push_back(vtemp);
		}
		else
		{
			vtemp->release();
			vtemp->NULLState();
			fm->_Delete((byte8*)vtemp, sizeof(sen));
			vtemp = nullptr;
		}

		//for(int k=0;k<segs.size();++k){ wbss.dbg_sen(segs.at(k)); } 

		for (int k = 0; k < basicoper_max; ++k)
		{
			for (int i = 0; i < segs.size(); ++i)
			{
				sen* seg = segs.at(i);
				if (seg->at(0).type == 'a')
					continue;
				if (seg->size() == 0)
					continue;

				fmlcstr str;
				str.NULLState();
				str.Init(2, true);
				str = seg->at(0).data.str;
				TBT tbt = DecodeTextBlock(str);
				if (tbt == TBT::_operation)
				{
					for (int u = 0; u < segs.size(); ++u) {
						cout << segs.at(u)->at(0).data.str << " ][ ";
					}
					cout << endl;
					if (strcmp(str.Arr, basicoper[k].symbol) == 0)
					{
						if (basicoper[k].mod == 'o')
						{
							if (basicoper[k].endop - basicoper[k].startop >= 11)
							{
								// input num oper
								temp_mem* result_ten =
									(temp_mem*)fm->_New(sizeof(temp_mem), true);
								temp_mem* left_ten = nullptr;
								temp_mem* right_ten = nullptr;
								int opertype = 0;
								int add = 1;
								int leftcast = 0;
								int rightcast = 0;
								sen* tempseg = nullptr;

								left_ten = get_asm_from_sen(segs.at(i - 1), true, true);
								ICB_ERR_CHECK(ERR_GETASMFROMSEN_OPER_ARITHMATH_TEMPMEM);
								right_ten = get_asm_from_sen(segs.at(i + 1), false, true);
								ICB_ERR_CHECK(ERR_GETASMFROMSEN_OPER_ARITHMATH_TEMPMEM);
								/*
								wbss.dbg_sen(segs.at(i-1));
								cout << basicoper[k].symbol << endl;
								wbss.dbg_sen(segs.at(i+1));
								*/
								opertype = combine_opertype(left_ten->valuetype,
									right_ten->valuetype);
								add = 1;
								leftcast = (int)get_cast_type(left_ten->valuetype, opertype);
								rightcast = (int)get_cast_type(right_ten->valuetype, opertype);

								if (leftcast != (int)casting_type::nocasting)
									++add;

								if (rightcast != (int)casting_type::nocasting)
									++add;

								result_ten->mem.NULLState();
								result_ten->mem.Init(result_ten->mem.size() + 1, false, true);
								for (int u = 0; u < left_ten->mem.size(); ++u)
								{
									result_ten->mem.push_back(left_ten->mem[u]);
								}
								// a casting
								if (leftcast != (int)casting_type::nocasting)
								{
									result_ten->mem.push_back((byte8)insttype::IT_CASTING_A);
									result_ten->mem.push_back((byte8)leftcast);
								}
								for (int u = 0; u < right_ten->mem.size(); ++u)
								{
									result_ten->mem.push_back(right_ten->mem[u]);
								}
								// b casting
								if (rightcast != (int)casting_type::nocasting)
								{
									result_ten->mem.push_back((byte8)insttype::IT_CASTING_B);
									result_ten->mem.push_back((byte8)rightcast);
								}

								//result_ten->mem.push_back((byte8)insttype::IT_AXBY);
								//result_ten->mem.push_back((byte8)insttype::IT_POP_AB); // POP

								if (is_a)
								{
									int opp = basicoper[k].startop + 2 * opertype;
									result_ten->mem.push_back((byte8)opp);
									result_ten->mem.push_back((byte8)insttype::IT_POP_B);
									result_ten->registerMod = 'A';
								}
								else
								{
									int opp = basicoper[k].startop + 2 * opertype + 1;
									result_ten->mem.push_back((byte8)opp);
									result_ten->mem.push_back((byte8)insttype::IT_POP_A);
									result_ten->registerMod = 'B';
								}

								if (k <= 10)
								{
									result_ten->valuetype = opertype;
								}
								else
								{
									result_ten->valuetype = 7; // bool
								}
								result_ten->isValue = true;
								result_ten->valuetype_detail =
									get_basic_type_with_int(result_ten->valuetype);

								tempseg = segs.at(i + 1);
								tempseg->release();
								fm->_Delete((byte8*)tempseg, sizeof(sen));
								segs.erase(i + 1);

								segs[i]->at(0).type = 'a'; // asm
								segs[i]->at(0).data.str = reinterpret_cast<char*>(result_ten);

								tempseg = segs.at(i - 1);
								tempseg->release();
								fm->_Delete((byte8*)tempseg, sizeof(sen));
								segs.erase(i - 1);
								--i;

								release_tempmem(left_ten);
								release_tempmem(right_ten);
								continue;

							ERR_GETASMFROMSEN_OPER_ARITHMATH_TEMPMEM:
								release_tempmem(left_ten);
								release_tempmem(right_ten);
								release_tempmem(result_ten);
								goto ERR_GETASMFROMSEN_RELEASE_SEGS;
							}
							else if (basicoper[k].endop - basicoper[k].startop == 1)
							{
								if (basicoper[k].startop == (byte8)insttype::IT_CM_BOOL_SAME_A || basicoper[k].startop == (byte8)insttype::IT_CM_BOOL_NOTSAME_A) {
									// input num oper
									temp_mem* result_ten =
										(temp_mem*)fm->_New(sizeof(temp_mem), true);
									temp_mem* left_ten = nullptr;
									temp_mem* right_ten = nullptr;
									int opertype = 0;
									int add = 1;
									int leftcast = 0;
									int rightcast = 0;
									sen* tempseg = nullptr;

									left_ten = get_asm_from_sen(segs.at(i - 1), true, true);
									ICB_ERR_CHECK(ERR_GETASMFROMSEN_OPER_CONDITIONAL_TEMPMEM);
									right_ten = get_asm_from_sen(segs.at(i + 1), false, true);
									ICB_ERR_CHECK(ERR_GETASMFROMSEN_OPER_CONDITIONAL_TEMPMEM);
									/*
									wbss.dbg_sen(segs.at(i-1));
									cout << basicoper[k].symbol << endl;
									wbss.dbg_sen(segs.at(i+1));
									*/
									opertype = combine_opertype(left_ten->valuetype,
										right_ten->valuetype);
									add = 1;
									leftcast = (int)get_cast_type(left_ten->valuetype, opertype);
									rightcast = (int)get_cast_type(right_ten->valuetype, opertype);

									if (leftcast != (int)casting_type::nocasting)
										++add;

									if (rightcast != (int)casting_type::nocasting)
										++add;

									result_ten->mem.NULLState();
									result_ten->mem.Init(result_ten->mem.size() + 1, false, true);
									for (int u = 0; u < left_ten->mem.size(); ++u)
									{
										result_ten->mem.push_back(left_ten->mem[u]);
									}
									// a casting
									if (leftcast != (int)casting_type::nocasting)
									{
										result_ten->mem.push_back((byte8)insttype::IT_CASTING_A);
										result_ten->mem.push_back((byte8)leftcast);
									}
									for (int u = 0; u < right_ten->mem.size(); ++u)
									{
										result_ten->mem.push_back(right_ten->mem[u]);
									}
									// b casting
									if (rightcast != (int)casting_type::nocasting)
									{
										result_ten->mem.push_back((byte8)insttype::IT_CASTING_B);
										result_ten->mem.push_back((byte8)rightcast);
									}

									//result_ten->mem.push_back((byte8)insttype::IT_AXBY);
									//result_ten->mem.push_back((byte8)insttype::IT_POP_AB); // POP

									if (is_a)
									{
										int opp = basicoper[k].startop;
										result_ten->mem.push_back((byte8)opp);
										result_ten->mem.push_back((byte8)insttype::IT_POP_B);
										result_ten->registerMod = 'A';
									}
									else
									{
										int opp = basicoper[k].startop;
										result_ten->mem.push_back((byte8)opp);
										result_ten->mem.push_back((byte8)insttype::IT_POP_A);
										result_ten->registerMod = 'B';
									}

									if (k <= 10)
									{
										result_ten->valuetype = opertype;
									}
									else
									{
										result_ten->valuetype = 7; // bool
									}
									result_ten->valuetype_detail =
										get_basic_type_with_int(result_ten->valuetype);
									result_ten->isValue = true;

									tempseg = segs.at(i + 1);
									tempseg->release();
									fm->_Delete((byte8*)tempseg, sizeof(sen));
									segs.erase(i + 1);

									segs[i]->at(0).type = 'a'; // asm
									segs[i]->at(0).data.str = reinterpret_cast<char*>(result_ten);

									tempseg = segs.at(i - 1);
									tempseg->release();
									fm->_Delete((byte8*)tempseg, sizeof(sen));
									segs.erase(i - 1);
									--i;

									release_tempmem(left_ten);
									release_tempmem(right_ten);

									continue;

								ERR_GETASMFROMSEN_OPER_CONDITIONAL_TEMPMEM:
									release_tempmem(left_ten);
									release_tempmem(right_ten);
									release_tempmem(result_ten);
									goto ERR_GETASMFROMSEN_RELEASE_SEGS;
								}
								else if (k == 16)
								{

									// ! not
									sen* tempseg = nullptr;
									int add = 1;
									temp_mem* result_ten =
										(temp_mem*)fm->_New(sizeof(temp_mem), true);
									temp_mem* right_ten = nullptr;
									right_ten = get_asm_from_sen(segs.at(i + 1), true, true);
									ICB_ERR_CHECK(ERR_GETASMFROMSEN_NOTOPER_TEMPMEM);
									result_ten->mem.NULLState();
									result_ten->mem.Init(result_ten->mem.size() + 1, false, true);
									for (int u = 0; u < right_ten->mem.size(); ++u)
									{
										result_ten->mem.push_back(right_ten->mem[u]);
									}

									//result_ten->mem.push_back((byte8)insttype::IT_AXBY);
									//result_ten->mem.push_back((byte8)insttype::IT_POP_AB); // POP

									if (is_a)
									{
										int opp = basicoper[k].startop; // A = !X
										result_ten->mem.push_back((byte8)opp);
										result_ten->registerMod = 'A';
									}
									else
									{
										int opp = basicoper[k].startop + 1; // B = !X
										result_ten->mem.push_back((byte8)opp);
										result_ten->registerMod = 'B';
									}

									result_ten->valuetype = 7; // bool
									result_ten->valuetype_detail =
										get_basic_type_with_int(result_ten->valuetype);
									result_ten->isValue = true;

									tempseg = segs.at(i + 1);
									tempseg->release();
									fm->_Delete((byte8*)tempseg, sizeof(sen));
									segs.erase(i + 1);

									segs[i]->at(0).type = 'a'; // asm
									segs[i]->at(0).data.str =
										reinterpret_cast<char*>(result_ten);

									release_tempmem(right_ten);

									continue;

								ERR_GETASMFROMSEN_NOTOPER_TEMPMEM:
									release_tempmem(right_ten);
									release_tempmem(result_ten);
									goto ERR_GETASMFROMSEN_RELEASE_SEGS;
								}
								else
								{
									// && and ||
									sen* tempseg = nullptr;
									temp_mem* result_ten =
										(temp_mem*)fm->_New(sizeof(temp_mem), true);
									temp_mem* left_ten = nullptr;
									temp_mem* right_ten = nullptr;
									left_ten = get_asm_from_sen(segs.at(i - 1), true, true);
									ICB_ERR_CHECK(ERR_GETASMFROMSEN_BOOLOPER_TEMPMEM);
									right_ten = get_asm_from_sen(segs.at(i + 1), false, true);
									ICB_ERR_CHECK(ERR_GETASMFROMSEN_BOOLOPER_TEMPMEM);
									result_ten->mem.NULLState();
									result_ten->mem.Init(result_ten->mem.size() + 1, false, true);
									for (int u = 0; u < left_ten->mem.size(); ++u)
									{
										result_ten->mem.push_back(left_ten->mem[u]);
									}
									for (int u = 0; u < right_ten->mem.size(); ++u)
									{
										result_ten->mem.push_back(right_ten->mem[u]);
									}

									//result_ten->mem.push_back((byte8)insttype::IT_AXBY);
									//result_ten->mem.push_back((byte8)insttype::IT_POP_AB); // POP

									if (is_a)
									{
										int opp = basicoper[k].startop;
										result_ten->mem.push_back((byte8)opp);
										result_ten->registerMod = 'A';
									}
									else
									{
										int opp = basicoper[k].startop + 1;
										result_ten->mem.push_back((byte8)opp);
										result_ten->registerMod = 'B';
									}

									result_ten->valuetype = 7; // bool
									result_ten->valuetype_detail =
										get_basic_type_with_int(result_ten->valuetype);
									result_ten->isValue = true;

									tempseg = segs.at(i + 1);
									tempseg->release();
									fm->_Delete((byte8*)tempseg, sizeof(sen));
									segs.erase(i + 1);

									segs[i]->at(0).type = 'a'; // asm
									segs[i]->at(0).data.str =
										reinterpret_cast<char*>(result_ten);

									tempseg = segs.at(i - 1);
									tempseg->release();
									fm->_Delete((byte8*)tempseg, sizeof(sen));
									segs.erase(i - 1);

									release_tempmem(left_ten);
									release_tempmem(right_ten);
									--i;
									continue;

								ERR_GETASMFROMSEN_BOOLOPER_TEMPMEM:
									release_tempmem(left_ten);
									release_tempmem(right_ten);
									release_tempmem(result_ten);
									goto ERR_GETASMFROMSEN_RELEASE_SEGS;
								}
							}
						}
						else
						{
							// ptr oper
							switch (basicoper[k].symbol[0])
							{
							case '[':
							{
								//there is two arr[i] operation. 
								//case 1. type is array : it return value of (self's address + i) address.
								//case 2. type is pointer : it return value of (self's value + i) address.

								temp_mem* result_ten =
									(temp_mem*)fm->_New(sizeof(temp_mem), true);
								temp_mem* left_ten = nullptr;
								temp_mem* right_ten = nullptr;

								type_data* std = nullptr;
								char cc[4] = {};
								sen* tempseg = nullptr;
								type_data* td = nullptr;

								bool is_array_type = false;
								if (segs[i - 1]->at(0).type == 'a') {
									//if asm
									type_data* imtd = reinterpret_cast<type_data*>(reinterpret_cast<temp_mem*>(segs[i - 1]->at(0).data.str)->valuetype_detail->structptr);
									is_array_type = imtd->typetype == 'a';
								}

								type_data* ltd = get_type_with_global_vname(segs[i - 1]->at(0).data.str);;
								if (ltd == nullptr) {
									ltd = get_type_with_vname(segs[i - 1]->at(0).data.str);
								}

								if (ltd == nullptr) {
									UpdateErrMsg(5, "Unknown identifier %s", segs[i - 1]->at(0).data.str);
									release_tempmem(result_ten);
								}
								ICB_ERR_CHECK(ERR_GETASMFROMSEN_RELEASE_SEGS);

								if (ltd == nullptr) {
									is_array_type |= false;
								}
								else {
									is_array_type |= segs[i - 1]->size() == 1 && ltd->typetype == 'a';
								}

								if (is_array_type) {
									//case 1 : array type
									left_ten = get_asm_from_sen(segs.at(i - 1), true, false);
									ICB_ERR_CHECK(ERR_GETASMFROMSEN_PTROPER_ARRAYINDEX_SUBTYPE_NOTEXIST);
								}
								else {
									//case 2 : pointer type
									left_ten = get_asm_from_sen(segs.at(i - 1), true, true);
									ICB_ERR_CHECK(ERR_GETASMFROMSEN_PTROPER_ARRAYINDEX_SUBTYPE_NOTEXIST);
								}
								right_ten = get_asm_from_sen(segs.at(i + 1), true, true);
								ICB_ERR_CHECK(ERR_GETASMFROMSEN_PTROPER_ARRAYINDEX_SUBTYPE_NOTEXIST);

								result_ten->mem.NULLState();
								result_ten->mem.Init(result_ten->mem.size() + 1, false, true);

								for (int u = 0; u < right_ten->mem.size(); ++u)
								{
									result_ten->mem.push_back(right_ten->mem[u]);
								}

								td = get_sub_type(left_ten->valuetype_detail);
								ICB_ERR_CHECK(ERR_GETASMFROMSEN_PTROPER_ARRAYINDEX_SUBTYPE_NOTEXIST);

								if (is_array_type) {
									std = get_sub_type(td);
								}
								else {
									std = nullptr;
								}
								//type_data *std = get_sub_type(td);
								result_ten->mem.push_back((byte8)insttype::IT_PUSH_B_CONST_4);

								if (is_array_type) {
									*reinterpret_cast<uint*>(cc) = std->typesiz;
								}
								else {
									*reinterpret_cast<uint*>(cc) = td->typesiz;
								}
								for (int u = 0; u < 4; ++u)
									result_ten->mem.push_back(cc[u]);

								//result_ten->mem.push_back((byte8)insttype::IT_AXBY);	// axby
								//result_ten->mem.push_back((byte8)insttype::IT_POP_AB); // POP
								// b=x*y uint
								result_ten->mem.push_back((byte8)insttype::IT_AU_UINT_MUL_B);

								for (int u = 0; u < left_ten->mem.size(); ++u)
								{
									result_ten->mem.push_back(left_ten->mem[u]);
								}

								//result_ten->mem.push_back(52);	// axby
								//result_ten->mem.push_back(218); // POP

								if (is_a)
								{
									int opp = (byte8)insttype::IT_AU_INT_ADD_A;
									result_ten->mem.push_back((byte8)opp);
									result_ten->registerMod = 'A';
									/*
									if (isvalue)
									{
										if(is_array_type){
											result_ten->valuetype = get_int_with_basictype(std);
										}
										else{
											result_ten->valuetype = get_int_with_basictype(td);
										}

										if (result_ten->valuetype == 8)
										{
											if(is_array_type){
												result_ten->valuetype_detail = get_addpointer_type(std);
											}
											else{
												result_ten->valuetype_detail = get_addpointer_type(td);
											}
										}
										else
										{
											if(is_array_type){
												result_ten->valuetype_detail = std;
											}
											else{
												result_ten->valuetype_detail = td;
											}
											result_ten->mem.push_back(205);
										}
									}
									else
									{
										result_ten->valuetype = 8;
										if(is_array_type){
											result_ten->valuetype_detail = get_addpointer_type(std);
										}
										else{
											result_ten->valuetype_detail = get_addpointer_type(td);
										}
									}

									*/

								}
								else
								{
									int opp = (byte8)insttype::IT_AU_INT_ADD_B;
									result_ten->mem.push_back((byte8)opp);
									result_ten->registerMod = 'B';
									/*
									if (isvalue)
									{
										result_ten->valuetype = get_int_with_basictype(td);
										if (result_ten->valuetype == 8)
										{
											if(is_array_type){
												result_ten->valuetype_detail =
												get_addpointer_type(std);
											}
											else{
												result_ten->valuetype_detail =
												get_addpointer_type(td);
											}
										}
										else
										{
											if(is_array_type){
												result_ten->valuetype_detail =
												get_addpointer_type(std);
											}
											else{
												result_ten->valuetype_detail =
												get_addpointer_type(td);
											}
											result_ten->mem.push_back(206);
										}
									}
									else
									{
										result_ten->valuetype = 8;
										if(is_array_type){
											result_ten->valuetype_detail =
											get_addpointer_type(std);
										}
										else{
											result_ten->valuetype_detail =
											get_addpointer_type(td);
										}
									}

									*/

								}

								result_ten->valuetype = 8;
								result_ten->isValue = false;
								if (is_array_type) {
									result_ten->valuetype_detail = get_addpointer_type(std);
								}
								else {
									result_ten->valuetype_detail = get_addpointer_type(td);
								}

								tempseg = segs.at(i + 1);
								tempseg->release();
								fm->_Delete((byte8*)tempseg, sizeof(sen));
								segs.erase(i + 1);

								tempseg = segs.at(i + 1);
								tempseg->release();
								fm->_Delete((byte8*)tempseg, sizeof(sen));
								segs.erase(i + 1);

								segs[i]->at(0).type = 'a'; // asm
								segs[i]->at(0).data.str =
									reinterpret_cast<char*>(result_ten);

								tempseg = segs.at(i - 1);
								tempseg->release();
								fm->_Delete((byte8*)tempseg, sizeof(sen));
								segs.erase(i - 1);
								--i;

								release_tempmem(left_ten);
								release_tempmem(right_ten);
								break;

							ERR_GETASMFROMSEN_PTROPER_ARRAYINDEX_SUBTYPE_NOTEXIST:
								release_tempmem(left_ten);
								release_tempmem(right_ten);
								release_tempmem(result_ten);
								goto ERR_GETASMFROMSEN_RELEASE_SEGS;
							}
							case '.':
							{
								temp_mem* result_ten =
									(temp_mem*)fm->_New(sizeof(temp_mem), true);

								temp_mem* left_ten = nullptr;
								//wbss.dbg_sen(segs.at(i - 1));

								type_data* member_td = nullptr;
								int add_address = 0;
								type_data* struct_td = nullptr;

								left_ten = get_asm_from_sen(segs.at(i - 1), true, false);
								ICB_ERR_CHECK(ERR_GETASMFROMSEN_PTROPER_STRUCTMEMBER_SUBTYPE_NOTEXIST);

								struct_td = get_sub_type(left_ten->valuetype_detail);
								ICB_ERR_CHECK(ERR_GETASMFROMSEN_PTROPER_STRUCTMEMBER_SUBTYPE_NOTEXIST);
								member_td = struct_td;

								if (struct_td->typetype == 's' && segs.at(i + 1)->at(0).type == 'w') {
									struct_data* stdataPtr = (struct_data*)struct_td->structptr;
									char* member_string = segs.at(i + 1)->at(0).data.str;
									for (int i = 0; i < stdataPtr->member_data.size(); ++i) {
										if (strcmp(stdataPtr->member_data.at(i).name, member_string) == 0) {
											add_address = stdataPtr->member_data.at(i).add_address;
											member_td = stdataPtr->member_data.at(i).td;
											break;
										}
									}
									result_ten->mem.NULLState();
									result_ten->mem.Init(2, false, true);

									if (left_ten->mem.size() == 5 && left_ten->mem[0] == (byte8)insttype::IT_PUSH_TO_A_FROM_ADDRESS_OF_VARIABLE_ID)
									{
										if (is_a)
										{
											result_ten->mem.push_back((byte8)insttype::IT_PUSH_A_CONST_INDEX_ADDR);
											for (int i = 0; i < 4; ++i) {
												result_ten->mem.push_back(left_ten->mem.at(i + 1));
											}
											byte8* barr = (byte8*)&add_address;
											for (int u = 0; u < 4; ++u)
											{
												result_ten->mem.push_back(barr[u]);
											}
											result_ten->registerMod = 'A';
											// only ptr output segment
											result_ten->isValue = false;
											result_ten->valuetype_detail = get_addpointer_type(member_td);
											result_ten->valuetype = get_int_with_basictype(result_ten->valuetype_detail); // ptr
										}
										else
										{
											result_ten->mem.push_back((byte8)insttype::IT_PUSH_B_CONST_INDEX_ADDR);
											for (int i = 0; i < 4; ++i) {
												result_ten->mem.push_back(left_ten->mem.at(i + 1));
											}
											byte8* barr = (byte8*)&add_address;
											for (int u = 0; u < 4; ++u)
											{
												result_ten->mem.push_back(barr[u]);
											}
											result_ten->registerMod = 'B';
											// only ptr output segment
											result_ten->isValue = false;
											result_ten->valuetype_detail = get_addpointer_type(member_td);
											result_ten->valuetype = get_int_with_basictype(result_ten->valuetype_detail); // ptr
										}
									}
									else {
										for (int u = 0; u < left_ten->mem.size(); ++u)
										{
											result_ten->mem.push_back(left_ten->mem[u]);
										}
										//a <= instance address

										result_ten->mem.push_back((byte8)insttype::IT_PUSH_B_CONST_4);
										byte8* barr = (byte8*)&add_address;
										for (int u = 0; u < 4; ++u) {
											result_ten->mem.push_back(barr[u]);
										}
										// axby
										//result_ten->mem.push_back(52);
										//result_ten->mem.push_back(218); // POP
										if (is_a) {
											result_ten->mem.push_back((byte8)insttype::IT_AU_INT_ADD_A);
											result_ten->registerMod = 'A';
											/*
											if(isvalue){
												result_ten->mem.push_back(205);
												result_ten->memsiz = result_ten->mem.size();
												result_ten->valuetype_detail = member_td;
												result_ten->valuetype = get_int_with_basictype(member_td);
											}
											else{
												result_ten->memsiz = result_ten->mem.size();
												result_ten->valuetype_detail = get_addpointer_type(member_td);
												result_ten->valuetype = 8; // ptr
											}
											*/

										}
										else {
											result_ten->mem.push_back((byte8)insttype::IT_AU_INT_ADD_B);
											result_ten->registerMod = 'B';
											//only ptr output segment
											//result_ten->valuetype_detail = get_addpointer_type(member_td);
											//result_ten->valuetype = -1; // ptr oper state
											/*
											if(isvalue){
												result_ten->mem.push_back(206);
											}
											*/
										}
										//only ptr output segment
										result_ten->isValue = false;
										result_ten->valuetype_detail = get_addpointer_type(member_td);
										result_ten->valuetype = get_int_with_basictype(result_ten->valuetype_detail); // ptr
									}

									sen* tempseg = segs.at(i + 1);
									tempseg->release();
									fm->_Delete((byte8*)tempseg, sizeof(sen));
									segs.erase(i + 1);

									segs[i]->at(0).type = 'a'; // asm
									segs[i]->at(0).data.str = reinterpret_cast<char*>(result_ten);

									tempseg = segs.at(i - 1);
									tempseg->release();
									fm->_Delete((byte8*)tempseg, sizeof(sen));
									segs.erase(i - 1);
									i -= 2;
								}

								release_tempmem(left_ten);
								break;

							ERR_GETASMFROMSEN_PTROPER_STRUCTMEMBER_SUBTYPE_NOTEXIST:
								release_tempmem(left_ten);
								release_tempmem(result_ten);
								goto ERR_GETASMFROMSEN_RELEASE_SEGS;
							}

							case '-':
							{
								temp_mem* result_ten =
									(temp_mem*)fm->_New(sizeof(temp_mem), true);
								temp_mem* left_ten = nullptr;
								type_data* td = nullptr;
								left_ten = get_asm_from_sen(segs.at(i - 1), true, false);
								ICB_ERR_CHECK(ERR_GETASMFROMSEN_PTROPER_ARROW_TEMPMEM);
								if (segs.at(i + 1)->size() == 1 && segs.at(i + 1)->at(0).type == 'w')
								{
									type_data* td = get_type_with_global_vname(segs.at(i + 1)->at(0).data.str);
									if (td == nullptr) {
										td = get_type_with_vname(segs.at(i + 1)->at(0).data.str);
									}

									if (td == nullptr) {
										UpdateErrMsg(5, "Unknown identifier %s", segs.at(i + 1)->at(0).data.str);
										release_tempmem(result_ten);
										release_tempmem(left_ten);
									}
									ICB_ERR_CHECK(ERR_GETASMFROMSEN_RELEASE_SEGS);
									//td = get_type_with_vname(segs.at(i + 1)->at(0).data.str);
									// a.b
									uint bid = 0;
									bool perfect = false;
									type_data* returntype = nullptr;

									if (td == nullptr)
										break;
									if (td->typetype != 's')
										break;
									struct_data* sptr =
										reinterpret_cast<struct_data*>(td->structptr);
									int su = 0;
									for (int u = 0; u < sptr->member_data.size(); ++u)
									{
										if (sptr->member_data[u].name ==
											segs.at(i + 1)->at(0).data.str)
										{
											bid = sptr->member_data[u].add_address;
											su = u;
											returntype = sptr->member_data[u].td;
											perfect = true;
											break;
										}
									}

									if (perfect)
									{
										result_ten->mem.NULLState();
										result_ten->mem.Init(2, false, true);
										for (int u = 0; u < left_ten->mem.size(); ++u)
										{
											result_ten->mem.push_back(left_ten->mem[u]);
										}
										// a = value(a)
										result_ten->mem.push_back((byte8)insttype::IT_PUSH_A_FROM_VALUE_OF_A);
										// b = const
										result_ten->mem.push_back((byte8)insttype::IT_PUSH_B_CONST_4);
										char cc[4] = {};
										*reinterpret_cast<uint*>(cc) = bid;
										for (int u = 0; u < 4; ++u)
											result_ten->mem.push_back(cc[u]);

										// axby
										//result_ten->mem.push_back((byte8)insttype::IT_AXBY);
										//result_ten->mem.push_back((byte8)insttype::IT_POP_AB); // POP
										// set y const4
										//result_ten->mem.push_back((byte8)insttype::IT_SET_Y_CONST_4);

										if (is_a)
										{
											// a=x+y uint
											result_ten->mem.push_back((byte8)insttype::IT_AU_INT_ADD_A);
										}
										else
										{
											// b=x+y uint
											result_ten->mem.push_back((byte8)insttype::IT_AU_INT_ADD_B);
										}

										// ptr
										result_ten->valuetype = 8;
										result_ten->valuetype_detail =
											sptr->member_data[su].td;

										sen* tempseg = segs.at(i + 1);
										tempseg->release();
										fm->_Delete((byte8*)tempseg, sizeof(sen));
										segs.erase(i + 1);

										segs[i]->at(0).type = 'a'; // asm
										segs[i]->at(0).data.str =
											reinterpret_cast<char*>(result_ten);

										tempseg = segs.at(i - 1);
										tempseg->release();
										fm->_Delete((byte8*)tempseg, sizeof(sen));
										segs.erase(i - 1);

										--i;
									}

								}

								release_tempmem(left_ten);

								break;

							ERR_GETASMFROMSEN_PTROPER_ARROW_TEMPMEM:
								release_tempmem(left_ten);
								release_tempmem(result_ten);
								goto ERR_GETASMFROMSEN_RELEASE_SEGS;
							}
							break;
							case '&':
							{
								temp_mem* result_ten =
									(temp_mem*)fm->_New(sizeof(temp_mem), true);
								temp_mem* right_ten = nullptr;
								sen* tempseg = nullptr;
								if (is_a)
								{
									right_ten = get_asm_from_sen(segs.at(i + 1), true, false);
									ICB_ERR_CHECK(ERR_GETASMFROMSEN_PTROPER_GETADDR_TEMPMEM);
								}
								else
								{
									right_ten = get_asm_from_sen(segs.at(i + 1), false, false);
									ICB_ERR_CHECK(ERR_GETASMFROMSEN_PTROPER_GETADDR_TEMPMEM);
								}

								result_ten->mem.NULLState();
								result_ten->mem.Init(2, false, true);
								for (int u = 0; u < right_ten->mem.size(); ++u)
								{
									result_ten->mem.push_back(right_ten->mem[u]);
								}

								// ptr
								result_ten->valuetype = 8;
								result_ten->valuetype_detail = right_ten->valuetype_detail;

								tempseg = segs.at(i + 1);
								tempseg->release();
								fm->_Delete((byte8*)tempseg, sizeof(sen));
								segs.erase(i + 1);

								segs[i]->at(0).type = 'a'; // asm
								segs[i]->at(0).data.str =
									reinterpret_cast<char*>(result_ten);

								release_tempmem(right_ten);

								break;

							ERR_GETASMFROMSEN_PTROPER_GETADDR_TEMPMEM:
								release_tempmem(right_ten);
								release_tempmem(result_ten);
								goto ERR_GETASMFROMSEN_RELEASE_SEGS;
							}
							break;
							case '*':
							{
								if (segs.at(i - 1)->at(0).type != 'o')
									break;
								temp_mem* result_ten =
									(temp_mem*)fm->_New(sizeof(temp_mem), true);
								temp_mem* right_ten =
									(temp_mem*)fm->_New(sizeof(temp_mem), true);
								sen* tempseg = nullptr;
								type_data* td = get_sub_type(right_ten->valuetype_detail);
								ICB_ERR_CHECK(ERR_GETASMFROMSEN_PTROPER_GETVALUE_SUBTYPE_NOTEXIST);

								if (is_a)
								{
									right_ten = get_asm_from_sen(segs.at(i + 1), true, true);
									ICB_ERR_CHECK(ERR_GETASMFROMSEN_PTROPER_GETVALUE_SUBTYPE_NOTEXIST);
								}
								else
								{
									right_ten = get_asm_from_sen(segs.at(i + 1), false, true);
									ICB_ERR_CHECK(ERR_GETASMFROMSEN_PTROPER_GETVALUE_SUBTYPE_NOTEXIST);
								}
								result_ten->mem.NULLState();
								result_ten->mem.Init(2, false, true);
								for (int u = 0; u < right_ten->mem.size(); ++u)
								{
									result_ten->mem.push_back(right_ten->mem[u]);
								}
								if (is_a)
								{
									result_ten->mem.push_back((byte8)insttype::IT_PUSH_A_FROM_VALUE_OF_A);
								}
								else
								{
									result_ten->mem.push_back((byte8)insttype::IT_PUSH_B_FROM_VALUE_OF_B);
								}

								// ptr
								result_ten->valuetype = 8;
								result_ten->valuetype_detail = td;

								tempseg = segs.at(i + 1);
								tempseg->release();
								fm->_Delete((byte8*)tempseg, sizeof(sen));
								segs.erase(i + 1);

								segs[i]->at(0).type = 'a'; // asm
								segs[i]->at(0).data.str =
									reinterpret_cast<char*>(result_ten);
								release_tempmem(right_ten);
								break;

							ERR_GETASMFROMSEN_PTROPER_GETVALUE_SUBTYPE_NOTEXIST:
								release_tempmem(right_ten);
								release_tempmem(result_ten);
								goto ERR_GETASMFROMSEN_RELEASE_SEGS;
							}
							break;
							}
						}
					}
				}
			}
		}

		if (segs.size() == 1)
		{
			if (tm != nullptr) {
				release_tempmem(tm);
				fm->_Delete((byte8*)tm, sizeof(temp_mem));
			}
			tm = reinterpret_cast<temp_mem*>(segs[0]->at(0).data.str);
			if (isvalue)
			{
				if (tm->isValue == false)
				{
					type_data* std = get_sub_type(tm->valuetype_detail);
					ICB_ERR_CHECK(ERR_GETASMFROMSEN_RELEASE_SEGS);
					if (std->typetype == 's' || std->typetype == 'a') {
						switch (tm->registerMod)
						{
						case 'A':
						{
							if (is_a == false)
							{
								tm->mem.push_back((byte8)insttype::PUSH_B_FROM_A); // b = a
								tm->mem.push_back((byte8)insttype::IT_POP_A); // pop a
							}
						}
						break;
						case 'B':
						{
							if (is_a)
							{
								tm->mem.push_back((byte8)insttype::PUSH_A_FROM_B); // a = b
								tm->mem.push_back((byte8)insttype::IT_POP_B); // pop b
							}
						}
						break;
						}
					}
					else
					{
						tm->valuetype_detail = get_sub_type(tm->valuetype_detail);
						ICB_ERR_CHECK(ERR_GETASMFROMSEN_RELEASE_SEGS);
						tm->valuetype = get_int_with_basictype(tm->valuetype_detail);
						tm->isValue = true;
						switch (tm->registerMod)
						{
						case 'A':
						{
							if (is_a)
							{
								tm->mem.push_back((byte8)insttype::IT_PUSH_A_FROM_VALUE_OF_A); // a = *a
							}
							else
							{
								tm->mem.push_back((byte8)insttype::PUSH_B_FROM_A); // b = a
								tm->mem.push_back((byte8)insttype::IT_POP_A); // pop a
								tm->mem.push_back((byte8)insttype::IT_PUSH_B_FROM_VALUE_OF_B); // b = *b
							}
						}
						break;
						case 'B':
						{
							if (is_a)
							{
								tm->mem.push_back((byte8)insttype::PUSH_A_FROM_B); // a = b
								tm->mem.push_back((byte8)insttype::IT_POP_B); // pop b
								tm->mem.push_back((byte8)insttype::IT_PUSH_A_FROM_VALUE_OF_A); // a = *a
							}
							else
							{
								tm->mem.push_back((byte8)insttype::IT_PUSH_B_FROM_VALUE_OF_B); // b = *b
							}
						}
						break;
						}
					}
				}
				else
				{
					switch (tm->registerMod)
					{
					case 'A':
					{
						if (is_a == false)
						{
							tm->mem.push_back((byte8)insttype::PUSH_B_FROM_A); // b = a
							tm->mem.push_back((byte8)insttype::IT_POP_A); // pop a
						}
					}
					break;
					case 'B':
					{
						if (is_a)
						{
							tm->mem.push_back((byte8)insttype::PUSH_A_FROM_B); // a = b
							tm->mem.push_back((byte8)insttype::IT_POP_B); // pop b
						}
					}
					break;
					}
				}
			}
			else
			{
				if (tm->isValue)
				{
					// WARN : imposible task. waring.
				}
				else
				{
					switch (tm->registerMod)
					{
					case 'A':
					{
						if (is_a == false)
						{
							tm->mem.push_back((byte8)insttype::PUSH_B_FROM_A); // b = a
							tm->mem.push_back((byte8)insttype::IT_POP_A); // pop a
						}
					}
					break;
					case 'B':
					{
						if (is_a)
						{
							tm->mem.push_back((byte8)insttype::PUSH_A_FROM_B); // a = b
							tm->mem.push_back((byte8)insttype::IT_POP_B); // pop b
						}
					}
					break;
					}
				}
			}

			sen* tempseg = segs.at(0);
			tempseg->release();
			fm->_Delete((byte8*)tempseg, sizeof(sen));

			fm->_tempPopLayer();
			return tm;
		}

		for (int u = 0; u < segs.size(); ++u) {
			cout << segs.at(u)->at(0).data.str << " ][ ";
		}
		cout << endl;

	ERR_GETASMFROMSEN_RELEASE_SEGS:

		for (int u = 0; u < segs.size(); ++u) {
			sen* tempseg = segs.at(u);
			tempseg->release();
			fm->_Delete((byte8*)tempseg, sizeof(sen));
		}

		fm->_tempPopLayer();

	ERR_GETASMFROMSEN_EXIT:

		return nullptr;
	}

	void interpret_AddStruct(code_sen* cs) {
		sen* code = get_sen_from_codesen(cs);
		char* cname = code->at(1).data.str;
		//char *name = (char*)fm->_New(strlen(cname)+1, true);
		//strcpy(name, cname);
		struct_data* stdata = (struct_data*)fm->_New(sizeof(struct_data), true);
		stdata->name.NULLState();
		stdata->name.Init(8, false);
		stdata->name = cname;
		stdata->member_data.NULLState();
		stdata->member_data.Init(8, false, true);
		int cpivot = 3;
		int totalSiz = 0;
		while (cpivot < code->size() - 1) {
			sen* type_sen = nullptr;
			type_data* td = nullptr;
			int loc = wbss.search_word_first(cpivot, code, ";");
			sen* member_sen = wbss.sen_cut(code, cpivot, loc - 1);
			//wbss.dbg_sen(member_sen);
			NamingData nd;
			cname = member_sen->last().data.str;
			nd.name = cname; //(char*)fm->_New(strlen(cname)+1, true);
			//strcpy(nd.name, cname);
			type_sen = wbss.sen_cut(member_sen, 0, member_sen->size() - 1);
			td = get_type_with_namesen(type_sen);
			ICB_ERR_CHECK(ERR_INTERPRET_ADDSTRUCT_TD_NOTEXIST);
			nd.td = td;
			nd.add_address = totalSiz;
			totalSiz += td->typesiz;
			stdata->member_data.push_back(nd);
			cpivot = loc + 1;

			member_sen->release();
			fm->_Delete((byte8*)member_sen, sizeof(sen));

			type_sen->release();
			fm->_Delete((byte8*)type_sen, sizeof(sen));

			continue;

		ERR_INTERPRET_ADDSTRUCT_TD_NOTEXIST:
			member_sen->release();
			fm->_Delete((byte8*)member_sen, sizeof(sen));

			type_sen->release();
			fm->_Delete((byte8*)type_sen, sizeof(sen));

			stdata->member_data.release();
			stdata->name.release();

			code->release();
			fm->_Delete((byte8*)code, sizeof(sen));

			return;
		}
		type_data* newtype = (type_data*)fm->_New(sizeof(type_data), true);
		*newtype = create_type(cname, totalSiz, 's', reinterpret_cast<int*>(stdata));

		types.push_back(newtype);

		code->release();
		fm->_Delete((byte8*)code, sizeof(sen));
	}

	void compile_addVariable(code_sen* cs)
	{
		sen* code = get_sen_from_codesen(cs);
		//wbss.dbg_sen(code);
		int loc = code->up - 1;
		char* variable_name = code->at(loc).data.str;
		sen* type_name = wbss.sen_cut(code, 0, loc - 1);
		// cout << variable_name << endl;
		// wbss.dbg_sen(type_name);
		if (blockstack.size() == 0)
		{
			// global variable
			NamingData* nd = nullptr;
			type_data* td = get_type_with_namesen(type_name);
			ICB_ERR_CHECK(ERR_COMPILE_ADDVARIABLE_GLOBAL_TD_NOTEXIST);
			nd = (NamingData*)fm->_New(sizeof(NamingData), true);
			nd->name = variable_name;
			nd->td = td;
			if (globalVariables.size() == 0)
			{
				nd->add_address = 0;
			}
			else
			{
				nd->add_address =
					globalVariables.last()->add_address + globalVariables.last()->td->typesiz;
			}
			globalVariables.push_back(nd);
		}
		else
		{
			int spup = 0;
			int kst = 0;
			NamingData nd;
			type_data* td = get_type_with_namesen(type_name);
			ICB_ERR_CHECK(ERR_COMPILE_ADDVARIABLE_NORMAL_TD_NOTEXIST);
			mem[writeup] = 0;
			++writeup;
			for (int k = blockstack.size() - 1; k >= 0; --k)
			{
				if (blockstack.at(k)->bs == blockstate::bs_function)
				{
					kst = k;
				}
			}
			for (int k = kst; k < blockstack.size(); ++k)
			{
				spup += blockstack.at(k)->add_address_up;
			}
			spup += td->typesiz;
			*reinterpret_cast<int*>(&mem[writeup]) = spup;
			writeup += 4;

			nd.name = variable_name;
			nd.td = td;
			nd.add_address = blockstack.last()->add_address_up + td->typesiz;
			blockstack.last()->add_address_up += td->typesiz;
			blockstack.last()->variable_data.push_back(nd);

			/*
				bool add = false;
				if (loc - 1 == 0)
				{
					// basic or struct type
					char *typen = type_name->at(0).data.str;
					for (int id = 0; id < basictype_max; ++id)
					{
						if (strcmp(typen, basictype[id]->name.c_str()) == 0)
						{
							mem[writeup] = 0;
							++writeup;

							int spup = 0;
							int kst = 0;
							for (int k = blockstack.size() - 1; k >= 0; --k)
							{
								if (blockstack.at(k)->bs == blockstate::bs_function)
								{
									kst = k;
								}
							}
							for (int k = kst; k < blockstack.size(); ++k)
							{
								spup += blockstack.at(k)->add_address_up;
							}
							spup += basictype[id]->typesiz;
							*reinterpret_cast<int *>(&mem[writeup]) = spup;
							writeup += 4;

							NamingData nd;
							nd.name = (char *)fm->_New(strlen(variable_name) + 1, true);
							strcpy_s(nd.name, variable_name);
							nd.td = basictype[id];
							nd.add_address =
								blockstack.last()->add_address_up + basictype[id]->typesiz;
							blockstack.last()->add_address_up += basictype[id]->typesiz;

							blockstack.last()->variable_data.push_back(nd);
							add = true;
							break;
						}
					}

					if (add == false)
					{
						// struct
						for (int i = basictype_max; i < types.size(); ++i)
						{
							if (strcmp(typen, types.at(i)->name.c_str()) == 0)
							{
								mem[writeup] = 0;
								++writeup;
								*reinterpret_cast<int *>(&mem[writeup]) = types.at(i)->typesiz;
								writeup += 4;

								NamingData nd;
								nd.name = (char *)fm->_New(strlen(variable_name) + 1, true);
								strcpy_s(nd.name, variable_name);
								nd.td = types.at(i);
								nd.add_address = blockstack.last()->add_address_up + types.at(i)->typesiz;
								blockstack.last()->add_address_up += types.at(i)->typesiz;
								blockstack.last()->variable_data.push_back(nd);
								add = true;
								break;
							}
						}
					}
				}
				else
				{
					// combine type
					type_data *td = get_type_with_namesen(type_name);
					mem[writeup] = 0;
					++writeup;
					*reinterpret_cast<int *>(&mem[writeup]) = td->typesiz;
					writeup += 4;

					NamingData nd;
					nd.name = (char *)fm->_New(strlen(variable_name) + 1, true);
					strcpy_s(nd.name, variable_name);
					nd.td = td;
					nd.add_address = blockstack.last()->add_address_up + td->typesiz;
					blockstack.last()->add_address_up += td->typesiz;
					blockstack.last()->variable_data.push_back(nd);
					add = true;
				}
				*/
		}

	ERR_COMPILE_ADDVARIABLE_GLOBAL_TD_NOTEXIST:
	ERR_COMPILE_ADDVARIABLE_NORMAL_TD_NOTEXIST:

		code->release();
		fm->_Delete((byte8*)code, sizeof(sen));

		type_name->release();
		fm->_Delete((byte8*)type_name, sizeof(sen));
	}

	void compile_setVariable(code_sen* cs)
	{
		// dbg_codesen(cs);
		sen* code = get_sen_from_codesen(cs);
		int loc = wbss.search_word_first(0, code, "=");

		if (loc < 0)
		{
			// += oper
			loc = wbss.search_word_first_cd(0, code, cd_eqoper);
			char operc = code->at(loc).data.str[0];
			sen* left_expr = wbss.sen_cut(code, 0, loc - 1);
			sen* right_expr = wbss.sen_cut(code, loc + 1, code->size() - 1);
			temp_mem* left_tm_ptr = nullptr;
			temp_mem* left_tm_v = nullptr;
			temp_mem* right_tm = nullptr;
			bool need_casting = false;
			casting_type castt;
			int ltype = 0;
			type_data* lstd = nullptr;

			left_tm_ptr = get_asm_from_sen(left_expr, true, false);
			ICB_ERR_CHECK(ERR_COMPILE_SETVARIABLE_COMPACTOPER_SUBTYPE_NOTEXIST);
			left_tm_v = get_asm_from_sen(left_expr, true, true);
			ICB_ERR_CHECK(ERR_COMPILE_SETVARIABLE_COMPACTOPER_SUBTYPE_NOTEXIST);
			right_tm = get_asm_from_sen(right_expr, false, true);
			ICB_ERR_CHECK(ERR_COMPILE_SETVARIABLE_COMPACTOPER_SUBTYPE_NOTEXIST);
			lstd = get_sub_type(left_tm_ptr->valuetype_detail);
			ICB_ERR_CHECK(ERR_COMPILE_SETVARIABLE_COMPACTOPER_SUBTYPE_NOTEXIST);
			ltype = get_int_with_basictype(lstd);

			if (ltype != right_tm->valuetype)
			{
				castt = get_cast_type(right_tm->valuetype, ltype);
				if (castt != casting_type::nocasting)
				{
					need_casting = true;
				}
			}

			for (int i = 0; i < right_tm->mem.size(); ++i)
			{
				mem[writeup++] = right_tm->mem[i];
			}
			if (need_casting)
			{
				mem[writeup++] = (byte8)insttype::IT_CASTING_A;
				mem[writeup++] = (byte8)castt;
			}
			for (int i = 0; i < left_tm_v->mem.size(); ++i)
			{
				mem[writeup++] = left_tm_v->mem[i];
			}
			//mem[writeup++] = 52;

			for (int k = 0; k < basicoper_max; ++k)
			{
				if (basicoper[k].symbol[0] == operc && basicoper[k].mod == 'o')
				{
					mem[writeup++] = basicoper[k].startop + ltype * 2;
					break;
				}
			}

			for (int i = 0; i < left_tm_ptr->mem.size(); ++i)
			{
				mem[writeup++] = left_tm_ptr->mem[i];
			}
			mem[writeup++] = (byte8)insttype::IT_PUSH_TO_LA_FROM_A;

			switch (lstd->typesiz)
			{
			case 1:
				mem[writeup++] = (byte8)insttype::IT_PUSH_TO_VALUE_OF_LA_FROM_A_1;
				break;
			case 2:
				mem[writeup++] = (byte8)insttype::IT_PUSH_TO_VALUE_OF_LA_FROM_A_2;
				break;
			case 4:
				mem[writeup++] = (byte8)insttype::IT_PUSH_TO_VALUE_OF_LA_FROM_A_4;
				break;
			case 8:
				mem[writeup++] = (byte8)insttype::IT_PUSH_TO_VALUE_OF_LA_FROM_A_8;
				break;
			default:
				cout << "typesiz is more than 8." << endl;
				{
					mem[writeup++] = (byte8)insttype::SET_ADDRESS_LA_FROM_ADRESS_A_N;
					byte8 cc[4];
					*reinterpret_cast<uint*>(&cc[0]) = (uint)lstd->typesiz;
					for (int i = 0; i < 4; ++i) {
						mem[writeup++] = cc[i];
					}
				}
				break;
			}

		ERR_COMPILE_SETVARIABLE_COMPACTOPER_SUBTYPE_NOTEXIST:
			code->release();
			fm->_Delete((byte8*)code, sizeof(sen));
			left_expr->release();
			fm->_Delete((byte8*)left_expr, sizeof(sen));
			right_expr->release();
			fm->_Delete((byte8*)right_expr, sizeof(sen));
			release_tempmem(left_tm_ptr);
			release_tempmem(left_tm_v);
			release_tempmem(right_tm);
		}
		else
		{
			sen* left_expr = wbss.sen_cut(code, 0, loc - 1);
			sen* right_expr = wbss.sen_cut(code, loc + 1, code->size() - 1);

			//wbss.dbg_sen(left_expr);
			// wbss.dbg_sen(right_expr);

			temp_mem* left_tm = nullptr;
			temp_mem* right_tm = nullptr;
			//dbg_temp_codemem(right_tm);

			bool need_casting = false;
			casting_type castt;
			int ltype = 0;
			type_data* lstd;

			left_tm = get_asm_from_sen(left_expr, true, false);
			ICB_ERR_CHECK(ERR_COMPILE_SETVARIABLE_OPER_SUBTYPE_NOTEXIST);
			right_tm = get_asm_from_sen(right_expr, true, true);
			ICB_ERR_CHECK(ERR_COMPILE_SETVARIABLE_OPER_SUBTYPE_NOTEXIST);
			lstd = get_sub_type(left_tm->valuetype_detail);
			ICB_ERR_CHECK(ERR_COMPILE_SETVARIABLE_OPER_SUBTYPE_NOTEXIST);
			ltype = get_int_with_basictype(lstd);
			if (ltype != right_tm->valuetype)
			{
				castt = get_cast_type(right_tm->valuetype, ltype);
				if (castt != casting_type::nocasting)
				{
					need_casting = true;
				}
			}

			for (int i = 0; i < right_tm->mem.size(); ++i)
			{
				mem[writeup++] = right_tm->mem[i];
			}
			if (need_casting)
			{
				mem[writeup++] = (byte8)insttype::IT_CASTING_A;
				mem[writeup++] = (byte8)castt;
			}

			for (int i = 0; i < left_tm->mem.size(); ++i)
			{
				mem[writeup++] = left_tm->mem[i];
			}
			mem[writeup++] = (byte8)insttype::IT_PUSH_TO_LA_FROM_A; // LA = A;

			switch (lstd->typesiz)
			{
			case 1:
				mem[writeup++] = (byte8)insttype::IT_PUSH_TO_VALUE_OF_LA_FROM_A_1;
				break;
			case 2:
				mem[writeup++] = (byte8)insttype::IT_PUSH_TO_VALUE_OF_LA_FROM_A_2;
				break;
			case 4:
				mem[writeup++] = (byte8)insttype::IT_PUSH_TO_VALUE_OF_LA_FROM_A_4;
				break;
			case 8:
				mem[writeup++] = (byte8)insttype::IT_PUSH_TO_VALUE_OF_LA_FROM_A_8;
				break;
			default:
				cout << "typesiz is more than 8." << endl;
				{
					mem[writeup++] = (byte8)insttype::SET_ADDRESS_LA_FROM_ADRESS_A_N;
					byte8 cc[4];
					*reinterpret_cast<uint*>(&cc[0]) = (uint)lstd->typesiz;
					for (int i = 0; i < 4; ++i) {
						mem[writeup++] = cc[i];
					}
				}
				break;
			}

		ERR_COMPILE_SETVARIABLE_OPER_SUBTYPE_NOTEXIST:

			code->release();
			fm->_Delete((byte8*)code, sizeof(sen));

			left_expr->release();
			fm->_Delete((byte8*)left_expr, sizeof(sen));

			right_expr->release();
			fm->_Delete((byte8*)right_expr, sizeof(sen));

			release_tempmem(left_tm);
			release_tempmem(right_tm);
		}
	}

	void compile_if(code_sen* cs)
	{
		sen* code = get_sen_from_codesen(cs);
		wbss.dbg_sen(code, InsideCode_Bake::icl);
		int loc = wbss.search_word_first(0, code, "if");
		if (loc == -1) {
			//else
			nextbd.bs = blockstate::bs_else;
			code->release();
			fm->_Delete((byte8*)code, sizeof(sen));
			return;
		}

		sen* inner_expr = wbss.oc_search(code, loc, "(", ")");
		inner_expr->pop_back();
		inner_expr->erase(0);
		wbss.dbg_sen(inner_expr, InsideCode_Bake::icl);
		temp_mem* inner_tm = get_asm_from_sen(inner_expr, true, true);
		ICB_ERR_CHECK(ERR_COMPILEIF_PROBLEM);
		for (int i = 0; i < inner_tm->mem.size(); ++i)
		{
			mem[writeup++] = inner_tm->mem[i];
		}
		mem[writeup++] = (byte8)insttype::IT_IF;
		nextbd.bs = blockstate::bs_if;
		nextbd.parameter[0] = writeup;
		writeup += 4;

		code->release();
		fm->_Delete((byte8*)code, sizeof(sen));

		inner_expr->release();
		fm->_Delete((byte8*)inner_expr, sizeof(sen));

		release_tempmem(inner_tm);

		return;

	ERR_COMPILEIF_PROBLEM:
		code->release();
		fm->_Delete((byte8*)code, sizeof(sen));
		inner_expr->release();
		fm->_Delete((byte8*)inner_expr, sizeof(sen));
		release_tempmem(inner_tm);
	}

	void compile_while(code_sen* cs)
	{
		// fm->dbg_fm1_lifecheck();
		sen* code = get_sen_from_codesen(cs);
		// fm->dbg_fm1_lifecheck();
		//wbss.dbg_sen(code);
		int loc = wbss.search_word_first(0, code, "while");
		sen* inner_expr = wbss.oc_search(code, loc, "(", ")");

		inner_expr->pop_back();
		inner_expr->erase(0);
		//wbss.dbg_sen(inner_expr);
		int save = writeup;
		temp_mem* inner_tm = get_asm_from_sen(inner_expr, true, true);
		ICB_ERR_CHECK(ERR_COMPILEWHILE_PROBLEM);

		for (int i = 0; i < inner_tm->mem.size(); ++i)
		{
			mem[writeup++] = inner_tm->mem[i];
		}
		mem[writeup++] = (byte8)insttype::IT_IF;
		nextbd.bs = blockstate::bs_while;
		nextbd.parameter[0] = writeup;
		nextbd.parameter[1] = save;

		if (nextbd.breakpoints != nullptr) {
			nextbd.breakpoints->release();
			fm->_Delete((byte8*)nextbd.breakpoints, sizeof(fmvecarr<int>));
			nextbd.breakpoints = nullptr;
		}
		nextbd.breakpoints = (fmvecarr<int> *)fm->_New(sizeof(fmvecarr<int>), true);
		nextbd.breakpoints->NULLState();
		nextbd.breakpoints->Init(2, false, true);

		if (nextbd.continuepoints != nullptr) {
			nextbd.continuepoints->release();
			fm->_Delete((byte8*)nextbd.continuepoints, sizeof(fmvecarr<int>));
			nextbd.continuepoints = nullptr;
		}
		nextbd.continuepoints = (fmvecarr<int> *)fm->_New(sizeof(fmvecarr<int>), true);
		nextbd.continuepoints->NULLState();
		nextbd.continuepoints->Init(2, false, true);

		writeup += 4;

		code->release();
		fm->_Delete((byte8*)code, sizeof(sen));

		inner_expr->release();
		fm->_Delete((byte8*)inner_expr, sizeof(sen));

		release_tempmem(inner_tm);

		return;

	ERR_COMPILEWHILE_PROBLEM:
		code->release();
		fm->_Delete((byte8*)code, sizeof(sen));
		inner_expr->release();
		fm->_Delete((byte8*)inner_expr, sizeof(sen));
		release_tempmem(inner_tm);
	}

	void compile_block(code_sen* cs)
	{
		if (nextbd.bs == blockstate::bs_struct)
		{
			nextsd->member_data.NULLState();
			nextsd->member_data.Init(2, false, true);
			int addadd = 0;
			for (int i = 0; i < cs->codeblocks->size(); ++i)
			{
				code_sen* css = reinterpret_cast<code_sen*>(cs->codeblocks->at(i));
				if (css->ck == codeKind::ck_addVariable)
				{
					NamingData nd;
					sen* typesen = (sen*)fm->_New(sizeof(sen), true);

					nd.name = css->sen[css->maxlen - 1];
					typesen->NULLState();
					typesen->Init(2, false, true);
					for (int k = 0; k < css->maxlen - 1; ++k)
					{
						segment seg;
						seg.data.str = css->sen[k];
						seg.type = 'w';
						typesen->push_back(seg);
					}
					nd.td = get_type_with_namesen(typesen);;
					ICB_ERR_CHECK(ERR_COMPILE_BLOCK_STRUCT_TD_NOTEXIST);
					nd.add_address = addadd;
					addadd += nd.td->typesiz;
					nextsd->member_data.push_back(nd);

					typesen->release();
					fm->_Delete((byte8*)typesen, sizeof(sen));

					continue;

				ERR_COMPILE_BLOCK_STRUCT_TD_NOTEXIST:
					typesen->release();
					fm->_Delete((byte8*)typesen, sizeof(sen));

					nextsd->name.release();
					nextsd->member_data.release();
					return;
				}
			}

			type_data* td = (type_data*)fm->_New(sizeof(type_data), true);
			td->structptr = reinterpret_cast<int*>(nextsd);
			td->name = nextsd->name.c_str();
			td->typetype = 's';
			td->typesiz = addadd;
			types.push_back(td);
		}
		else
		{
			block_data* bd = (block_data*)fm->_New(sizeof(block_data), true);
			bd->add_address_up = 0;
			bd->start_pc = &mem[writeup];
			bd->variable_data.NULLState();
			bd->variable_data.Init(2, false, true);
			bd->bs = nextbd.bs;
			if (bd->bs == blockstate::bs_while)
			{
				bd->breakpoints = nextbd.breakpoints;
				//nextbd.breakpoints = nullptr;
				bd->continuepoints = nextbd.continuepoints;
				//nextbd.continuepoints = nullptr;
			}
			else
			{
				bd->breakpoints = nullptr;
				bd->continuepoints = nullptr;
			}

			for (int i = 0; i < 5; ++i)
			{
				bd->parameter[i] = nextbd.parameter[i];
			}
			blockstack.push_back(bd);

			if (blockstack.last()->bs == blockstate::bs_function)
			{
				func_data* fd = reinterpret_cast<func_data*>(nextbd.parameter[0]);
				fd->start_pc = bd->start_pc;

				for (int k = 0; k < fd->param_data.size(); ++k)
				{
					bd->variable_data.push_back(fd->param_data[k]);
				}

				if (strcmp(fd->name.c_str(), "main") == 0)
				{
					*reinterpret_cast<uint*>(&mem[2]) = (uint)(fd->start_pc - mem);
				}

				if (fd->param_data.size() != 0)
				{
					bd->add_address_up =
						fd->param_data.last().add_address + fd->param_data.last().td->typesiz;
				}
				else
				{
					bd->add_address_up = 0;
				}

				functions.push_back(fd);
			}

			for (int i = 0; i < cs->codeblocks->size(); ++i)
			{
				code_sen* css = reinterpret_cast<code_sen*>(cs->codeblocks->at(i));
				dbg_codesen(css); cout << endl;
				if (css->ck == codeKind::ck_if)
				{
					fmvecarr<int> ifptr_arr;
					ifptr_arr.NULLState();
					ifptr_arr.Init(2, true);
					int ifi = i + 2;
					int stack = 0;
					code_sen* css2 = nullptr;
					sen* css2sen = nullptr;
					if (cs->codeblocks->size() <= i + 2)
					{
						current_if_is_multiple = false;
						compile_code(css);
						ICB_ERR_CHECK(ERR_COMPILEBLOCK_PROBLEM);
						continue;
					}
					css2 = reinterpret_cast<code_sen*>(cs->codeblocks->at(i + 2));
					css2sen = get_sen_from_codesen(css2);
					while (css2 != nullptr && css2->ck == codeKind::ck_if && strcmp(css2sen->at(0).data.str, "else") == 0)
					{
						stack += 1;
						css2 = reinterpret_cast<code_sen*>(cs->codeblocks->at(ifi + 2));
						ifi += 2;
						if (css2 == nullptr)
						{
							break;
						}
					}

					css2sen->release();
					fm->_Delete((byte8*)css2sen, sizeof(sen));

					if (ifi != i + 2)
					{
						ifi -= 1;
					}

					//compile if
					if (stack == 0)
					{
						current_if_is_multiple = false;
						compile_code(css);
						ICB_ERR_CHECK(ERR_COMPILEBLOCK_PROBLEM);
						continue;
					}
					else
					{
						current_if_is_multiple = true;
						// else if if
						uint address = 0;
						compile_code(css);
						ICB_ERR_CHECK(ERR_COMPILEBLOCK_PROBLEM);
						css2 = reinterpret_cast<code_sen*>(cs->codeblocks->at(i + 1));
						address = nextbd.parameter[0];
						compile_code(css2);
						ICB_ERR_CHECK(ERR_COMPILEBLOCK_PROBLEM);
						mem[writeup++] = (byte8)insttype::IT_JMP;
						ifptr_arr.push_back(writeup);
						writeup += 4;

						*reinterpret_cast<uint*>(&mem[address]) = (uint)writeup;

						int ifk = i + 2;
						while (ifk + 1 <= ifi)
						{
							css2 = reinterpret_cast<code_sen*>(cs->codeblocks->at(ifk));
							if (css2->maxlen == 1 && strcmp(css2->sen[0], "else") == 0)
							{
								// else
								compile_code(css2);
								ICB_ERR_CHECK(ERR_COMPILEBLOCK_PROBLEM);
								css2 = reinterpret_cast<code_sen*>(cs->codeblocks->at(ifk + 1));
								compile_code(css2);
								ICB_ERR_CHECK(ERR_COMPILEBLOCK_PROBLEM);
								ifk += 2;
								continue;
							}
							else
							{
								compile_code(css2);
								ICB_ERR_CHECK(ERR_COMPILEBLOCK_PROBLEM);
								address = nextbd.parameter[0];
							}

							css2 = reinterpret_cast<code_sen*>(cs->codeblocks->at(ifk + 1));
							compile_code(css2);
							ICB_ERR_CHECK(ERR_COMPILEBLOCK_PROBLEM);
							mem[writeup++] = (byte8)insttype::IT_JMP;
							ifptr_arr.push_back(writeup);
							writeup += 4;
							*reinterpret_cast<uint*>(&mem[address]) = (uint)writeup;
							ifk += 2;
						}

						for (int k = 0; k < ifptr_arr.size(); ++k)
						{
							*reinterpret_cast<uint*>(&mem[ifptr_arr[k]]) = (uint)writeup;
						}

						i = ifi;
					}
				}
				else
				{
					compile_code(css);
					ICB_ERR_CHECK(ERR_COMPILEBLOCK_PROBLEM);
				}
			}

			switch (blockstack.last()->bs)
			{
			case blockstate::bs_if:
				// case 1 : if only - writeup;
				// case 2 : if / else or more - writeup+5;
				if (current_if_is_multiple) {
					*reinterpret_cast<uint*>(&mem[blockstack.last()->parameter[0]]) =
						(uint)writeup + 5;
				}
				else {
					*reinterpret_cast<uint*>(&mem[blockstack.last()->parameter[0]]) =
						(uint)writeup;
				}
				break;
			case blockstate::bs_while:
			{
				*reinterpret_cast<uint*>(&mem[blockstack.last()->parameter[0]]) =
					(uint)writeup + 5;
				for (int i = 0; i < blockstack.last()->breakpoints->size(); ++i)
				{
					*reinterpret_cast<uint*>(&mem[blockstack.last()->breakpoints->at(i)]) =
						(uint)writeup + 5;
				}

				for (int i = 0; i < blockstack.last()->continuepoints->size(); ++i)
				{
					*reinterpret_cast<
						uint*>(&mem[blockstack.last()->continuepoints->at(i)]) =
						(uint)blockstack.last()->parameter[1];
				}

				mem[writeup++] = (byte8)insttype::IT_JMP; // jmp
				*reinterpret_cast<uint*>(&mem[writeup]) =
					(uint)blockstack.last()->parameter[1];
				writeup += 4;
			}
			break;
			case blockstate::bs_function:
			{
				mem[writeup++] = (byte8)insttype::IT_RETURN; // return
			}
			break;
			}

			if (bd->breakpoints != nullptr)
			{
				bd->breakpoints->release();
				fm->_Delete((byte8*)bd->breakpoints, sizeof(fmvecarr<int>));
			}
			bd->variable_data.release();
			if (bd->continuepoints != nullptr)
			{
				bd->continuepoints->release();
				fm->_Delete((byte8*)bd->continuepoints, sizeof(fmvecarr<int>));
			}
			fm->_Delete((byte8*)bd, sizeof(block_data));
			blockstack.pop_back();
			return;

		ERR_COMPILEBLOCK_PROBLEM:
			if (bd->breakpoints != nullptr)
			{
				bd->breakpoints->release();
				fm->_Delete((byte8*)bd->breakpoints, sizeof(fmvecarr<int>));
			}
			bd->variable_data.release();
			if (bd->continuepoints != nullptr)
			{
				bd->continuepoints->release();
				fm->_Delete((byte8*)bd->continuepoints, sizeof(fmvecarr<int>));
			}
			fm->_Delete((byte8*)bd, sizeof(block_data));
			blockstack.pop_back();
			return;
		}
	}

	void compile_addFunction(code_sen* cs)
	{
		func_data* fd = (func_data*)fm->_New(sizeof(func_data), true);
		fd->name.NULLState();
		fd->name.Init(2, false);

		sen* code = get_sen_from_codesen(cs);
		int loc = code->size() - 1;
		sen* inner_params = wbss.oc_search_inv(code, loc, "(", ")");
		int nameloc = loc - inner_params->size();

		fd->name = code->at(nameloc).data.str;

		//sen *typen = wbss.sen_cut(code, 0, nameloc - 1);
		sen* params_sen = wbss.sen_cut(code, nameloc + 2, loc - 1);
		//wbss.dbg_sen(params_sen);
		int coma = wbss.search_word_first(0, params_sen, ",");
		int savecoma = -1;
		int last = params_sen->size() - 1;

		fd->param_data.NULLState();
		fd->param_data.Init(2, false);

		int addadd = 0;

		if (last < 0)
		{
			nextbd.bs = blockstate::bs_function;
			nextbd.parameter[0] = reinterpret_cast<uint64_t>(fd);

			code->release();
			fm->_Delete((byte8*)code, sizeof(sen));

			params_sen->release();
			fm->_Delete((byte8*)params_sen, sizeof(sen));

			inner_params->release();
			fm->_Delete((byte8*)inner_params, sizeof(sen));
			return;
		}

		while (coma != -1)
		{
			sen* param_sen = wbss.sen_cut(params_sen, savecoma + 1, coma - 1);
			NamingData nd;
			sen* typestr = (sen*)fm->_New(sizeof(sen), true);
			typestr->NULLState();
			typestr->Init(2, false);
			for (int i = 0; i < param_sen->size() - 1; ++i)
			{
				typestr->push_back(param_sen->at(i));
			}

			nd.td = get_type_with_namesen(typestr);
			ICB_ERR_CHECK(ERR_COMPILE_ADDFUNCTION_PARAM_TD_NOTEXIST);
			nd.name = param_sen->last().data.str;

			addadd += nd.td->typesiz; //
			nd.add_address = addadd;

			fd->param_data.push_back(nd);
			savecoma = coma + 1;
			coma = wbss.search_word_first(savecoma, params_sen, ",");

			savecoma = coma;
			coma = wbss.search_word_first(savecoma + 1, params_sen, ",");

			typestr->release();
			fm->_Delete((byte8*)typestr, sizeof(sen));

			param_sen->release();
			fm->_Delete((byte8*)param_sen, sizeof(sen));

			continue;

		ERR_COMPILE_ADDFUNCTION_PARAM_TD_NOTEXIST:
			typestr->release();
			fm->_Delete((byte8*)typestr, sizeof(sen));

			param_sen->release();
			fm->_Delete((byte8*)param_sen, sizeof(sen));

			param_sen->release();
			fm->_Delete((byte8*)param_sen, sizeof(sen));

			inner_params->release();
			fm->_Delete((byte8*)inner_params, sizeof(sen));

			code->release();
			fm->_Delete((byte8*)code, sizeof(sen));

			fd->name.release();
			fd->param_data.release();
			fm->_Delete((byte8*)fd, sizeof(func_data));

			return;
		}

		sen* param_sen = wbss.sen_cut(params_sen, savecoma + 1, last);
		//wbss.dbg_sen(param_sen);
		NamingData nd;

		sen* typestr = (sen*)fm->_New(sizeof(sen), true);
		typestr->NULLState();
		typestr->Init(2, false);
		for (int i = 0; i < param_sen->size() - 1; ++i)
		{
			typestr->push_back(param_sen->at(i));
		}

		//wbss.dbg_sen(typestr);
		nd.td = get_type_with_namesen(typestr);
		ICB_ERR_CHECK(ERR_COMPILE_ADDFUNCTION_LASTPARAM_TD_NOTEXIST);
		nd.name = param_sen->last().data.str;

		addadd += nd.td->typesiz;
		nd.add_address = addadd;

		fd->param_data.push_back(nd);
		savecoma = coma + 1;

		nextbd.bs = blockstate::bs_function;
		nextbd.parameter[0] = reinterpret_cast<uint64_t>(fd);

		code->release();
		fm->_Delete((byte8*)code, sizeof(sen));

		param_sen->release();
		fm->_Delete((byte8*)param_sen, sizeof(sen));

		param_sen->release();
		fm->_Delete((byte8*)param_sen, sizeof(sen));

		inner_params->release();
		fm->_Delete((byte8*)inner_params, sizeof(sen));

		typestr->release();
		fm->_Delete((byte8*)typestr, sizeof(sen));

	ERR_COMPILE_ADDFUNCTION_LASTPARAM_TD_NOTEXIST:
		typestr->release();
		fm->_Delete((byte8*)typestr, sizeof(sen));

		param_sen->release();
		fm->_Delete((byte8*)param_sen, sizeof(sen));

		param_sen->release();
		fm->_Delete((byte8*)param_sen, sizeof(sen));

		inner_params->release();
		fm->_Delete((byte8*)inner_params, sizeof(sen));

		code->release();
		fm->_Delete((byte8*)code, sizeof(sen));

		fd->name.release();
		fd->param_data.release();
		fm->_Delete((byte8*)fd, sizeof(func_data));
	}

	void compile_useFunction(code_sen* cs)
	{
		func_data* fd = nullptr;

		sen* code = get_sen_from_codesen(cs);
		int loc = code->size() - 1;
		sen* inner_params = wbss.oc_search_inv(code, loc, "(", ")");
		if (inner_params == nullptr) {
			UpdateErrMsg(6, "Unpaired parentheses exist.", nullptr);
			return;
		}
		int nameloc = loc - inner_params->size();
		char* funcname = code->at(nameloc).data.str;

		bool isext = false;
		int extID = 0;
		int exfuncID = 0;
		for (int i = 0; i < extension.size(); ++i)
		{
			ICB_Extension* ext = extension.at(i);
			for (int k = 0; k < ext->exfuncArr.size(); ++k)
			{
				func_data* efd = ext->exfuncArr[k];
				if (strcmp(funcname, efd->name.c_str()) == 0)
				{
					isext = true;
					//sen* typen = wbss.sen_cut(code, 0, nameloc - 1);
					//sen* params_sen = wbss.sen_cut(code, nameloc + 2, loc - 1);
					fd = efd;
					extID = i;
					exfuncID = k;
					break;
				}
			}
		}

		if (strcmp(funcname, "dbg") == 0)
		{
			inner_params->pop_back();
			inner_params->erase(0);
			//wbss.dbg_sen(inner_params);
			int coma = wbss.search_word_first_in_specific_oc_layer(inner_params, 0, "(", ")", 0, ",");
			int savecomma = -1;
			sen* param_sen = nullptr;
			temp_mem* tm = nullptr;
			while (coma != -1)
			{
				param_sen = wbss.sen_cut(inner_params, savecomma + 1, coma - 1);
				//wbss.dbg_sen(param_sen);
				tm = get_asm_from_sen(param_sen, true, true);
				ICB_ERR_CHECK(ERR_COMPILEUSEFUNCTION_DBG_TEMPMEM);
				for (int k = 0; k < tm->mem.size(); ++k)
				{
					mem[writeup++] = tm->mem[k];
				}
				release_tempmem(tm);
				mem[writeup++] = (byte8)insttype::IT_DBG_A;
				mem[writeup++] = (byte8)tm->valuetype;
				savecomma = coma;
				coma = wbss.search_word_first_in_specific_oc_layer(inner_params, savecomma + 1, "(", ")", 0, ",");

				param_sen->release();
				fm->_Delete((byte8*)param_sen, sizeof(sen));
			}

			param_sen = wbss.sen_cut(inner_params, savecomma + 1, inner_params->size() - 1);
			//wbss.dbg_sen(param_sen);
			tm = get_asm_from_sen(param_sen, true, true);
			ICB_ERR_CHECK(ERR_COMPILEUSEFUNCTION_DBG_TEMPMEM);

			for (int k = 0; k < tm->mem.size(); ++k)
			{
				mem[writeup++] = tm->mem[k];
			}
			mem[writeup++] = (byte8)insttype::IT_DBG_A;
			mem[writeup++] = (byte8)tm->valuetype;

		ERR_COMPILEUSEFUNCTION_DBG_TEMPMEM:

			code->release();
			fm->_Delete((byte8*)code, sizeof(sen));

			inner_params->release();
			fm->_Delete((byte8*)inner_params, sizeof(sen));

			param_sen->release();
			fm->_Delete((byte8*)param_sen, sizeof(sen));

			release_tempmem(tm);
			return;
		}
		else if (strcmp(funcname, "inp") == 0)
		{
			inner_params->pop_back();
			inner_params->erase(0);
			//wbss.dbg_sen(inner_params);
			temp_mem* tm = nullptr;
			type_data* std = nullptr;
			tm = get_asm_from_sen(inner_params, true, true);
			ICB_ERR_CHECK(ERR_COMPILE_USEFUNCTION_INP_SUBTYPE_NOTEXIST);
			for (int k = 0; k < tm->mem.size(); ++k)
			{
				mem[writeup++] = tm->mem[k];
			}
			release_tempmem(tm);
			mem[writeup++] = (byte8)insttype::IT_INP_A_PTR;

			std = get_sub_type(tm->valuetype_detail);
			ICB_ERR_CHECK(ERR_COMPILE_USEFUNCTION_INP_SUBTYPE_NOTEXIST);
			mem[writeup++] = (byte8)get_int_with_basictype(std);

		ERR_COMPILE_USEFUNCTION_INP_SUBTYPE_NOTEXIST:

			code->release();
			fm->_Delete((byte8*)code, sizeof(sen));

			inner_params->release();
			fm->_Delete((byte8*)inner_params, sizeof(sen));
			return;
		}

		if (isext == false)
		{
			sen* typen = nullptr;
			sen* params_sen = nullptr;
			int last = 0;
			//wbss.dbg_sen(params_sen);
			int coma = 0;
			int savecoma = -1;
			int addadd = 0;
			int paramid = 0;
			int paramCount = 0;
			sen* param_sen = nullptr;
			temp_mem* tm = nullptr;

			fd = get_func_with_name(code->at(nameloc).data.str);
			ICB_ERR_CHECK(ERR_COMPILE_USEFUNCTION_NORMAL_FD_NOTEXIST);
			// mem[writeup++] = fd->start_pc; // func

			typen = wbss.sen_cut(code, 0, nameloc - 1);

			params_sen = wbss.sen_cut(code, nameloc + 2, loc - 1);
			if (params_sen->size() == 0)
			{
				mem[writeup++] = (byte8)insttype::IT_FUNC;	  // FUNC
				mem[writeup++] = (byte8)insttype::IT_FUNCJMP; // jmp
				*reinterpret_cast<uint*>(&mem[writeup]) = (uint)(fd->start_pc - &mem[0]);
				writeup += 4;

				code->release();
				fm->_Delete((byte8*)code, sizeof(sen));

				inner_params->release();
				fm->_Delete((byte8*)inner_params, sizeof(sen));

				params_sen->release();
				fm->_Delete((byte8*)params_sen, sizeof(sen));
				return;
			}
			last = params_sen->size() - 1;
			//wbss.dbg_sen(params_sen);
			coma = wbss.search_word_first_in_specific_oc_layer(params_sen, 0, "(", ")", 0, ",");
			savecoma = -1;

			addadd = 0;
			paramid = 0;
			paramCount = 0;

			mem[writeup++] = (byte8)insttype::IT_FUNC; // FUNC

			while (coma != -1)
			{
				param_sen = wbss.sen_cut(params_sen, savecoma + 1, coma - 1);
				//wbss.dbg_sen(param_sen);
				tm = get_asm_from_sen(param_sen, true, true);
				ICB_ERR_CHECK(ERR_COMPILEUSEFUNCTION_NORMAL_TEMPMEM);
				if (tm->valuetype_detail->typetype == 's')
				{
					temp_mem* ptrtm = get_asm_from_sen(param_sen, true, false);
					for (int i = 0; i < ptrtm->mem.size(); ++i)
					{
						mem[writeup++] = ptrtm->mem[i];
					}
					release_tempmem(ptrtm);
					ICB_ERR_CHECK(ERR_COMPILEUSEFUNCTION_NORMAL_TEMPMEM);
				}
				else
				{
					for (int i = 0; i < tm->mem.size(); ++i)
					{
						mem[writeup++] = tm->mem[i];
					}

					casting_type ct = get_cast_type(tm->valuetype, get_int_with_basictype(fd->param_data.at(paramCount).td));
					if (ct != casting_type::nocasting)
					{
						mem[writeup++] = (byte8)insttype::IT_CASTING_A;
						mem[writeup++] = (byte8)ct;
					}
					/*
					*reinterpret_cast<uint*>(&mem[writeup]) = (uint)ct;
					writeup += 4;
					*/
				}

				if (fd->param_data[paramid].td->typetype != 's')
				{
					switch (fd->param_data[paramid].td->typesiz)
					{
					case 1:
						mem[writeup++] = (byte8)insttype::IT_PARAM_1; // param
						break;
					case 2:
						mem[writeup++] = (byte8)insttype::IT_PARAM_2; // param
						break;
					case 4:
						mem[writeup++] = (byte8)insttype::IT_PARAM_4; // param
						break;
					case 8:
						mem[writeup++] = (byte8)insttype::PARAM_8; // param
						break;
					}
				}
				else
				{
					mem[writeup++] = (byte8)insttype::PARAM_N_COPY_BY_ADDRESS; // param N by address(a)
					byte8* N = (byte8*)&fd->param_data[paramid].td->typesiz;
					// write siz of struct type
					for (int i = 0; i < 4; ++i)
					{
						mem[writeup++] = N[i];
					}
				}

				savecoma = coma;
				coma = wbss.search_word_first_in_specific_oc_layer(params_sen, savecoma + 1, "(", ")", 0, ",");
				// coma = wbss.search_word_first(savecoma+1, params_sen, ",");

				++paramid;

				param_sen->release();
				fm->_Delete((byte8*)param_sen, sizeof(sen));

				release_tempmem(tm);
				++paramCount;
			}

			//wbss.dbg_sen(params_sen);

			param_sen = wbss.sen_cut(params_sen, savecoma + 1, last);
			//wbss.dbg_sen(param_sen);
			tm = get_asm_from_sen(param_sen, true, true);
			ICB_ERR_CHECK(ERR_COMPILEUSEFUNCTION_NORMAL_TEMPMEM);
			if (tm->valuetype_detail->typetype == 's')
			{
				temp_mem* ptrtm = get_asm_from_sen(param_sen, true, false);
				for (int i = 0; i < ptrtm->mem.size(); ++i)
				{
					mem[writeup++] = ptrtm->mem[i];
				}
				release_tempmem(ptrtm);
				ICB_ERR_CHECK(ERR_COMPILEUSEFUNCTION_NORMAL_TEMPMEM);
			}
			else
			{
				for (int i = 0; i < tm->mem.size(); ++i)
				{
					mem[writeup++] = tm->mem[i];
				}

				casting_type ct = get_cast_type(tm->valuetype, get_int_with_basictype(fd->param_data.at(paramCount).td));
				if (ct != casting_type::nocasting)
				{
					mem[writeup++] = (byte8)insttype::IT_CASTING_A;
					mem[writeup++] = (byte8)ct;
				}
				/*
				*reinterpret_cast<uint*>(&mem[writeup]) = (uint)ct;
				writeup += 4;
				*/
			}

			if (fd->param_data[paramid].td->typetype != 's')
			{
				switch (fd->param_data[paramid].td->typesiz)
				{
				case 1:
					mem[writeup++] = (byte8)insttype::IT_PARAM_1; // param
					break;
				case 2:
					mem[writeup++] = (byte8)insttype::IT_PARAM_2; // param
					break;
				case 4:
					mem[writeup++] = (byte8)insttype::IT_PARAM_4; // param
					break;
				case 8:
					mem[writeup++] = (byte8)insttype::PARAM_8; // param
					break;
				}
			}
			else
			{
				mem[writeup++] = (byte8)insttype::PARAM_N_COPY_BY_ADDRESS; // param N by address(a)
				byte8* N = (byte8*)&fd->param_data[paramid].td->typesiz;
				// write siz of struct type
				for (int i = 0; i < 4; ++i)
				{
					mem[writeup++] = N[i];
				}
			}

			mem[writeup++] = (byte8)insttype::IT_FUNCJMP; // jmp
			*reinterpret_cast<uint*>(&mem[writeup]) = (uint)(fd->start_pc - &mem[0]);
			writeup += 4;

		ERR_COMPILEUSEFUNCTION_NORMAL_TEMPMEM:

			release_tempmem(tm);

			param_sen->release();
			fm->_Delete((byte8*)param_sen, sizeof(sen));

			params_sen->release();
			fm->_Delete((byte8*)params_sen, sizeof(sen));

			typen->release();
			fm->_Delete((byte8*)typen, sizeof(sen));

		ERR_COMPILE_USEFUNCTION_NORMAL_FD_NOTEXIST:

			inner_params->release();
			fm->_Delete((byte8*)inner_params, sizeof(sen));

			code->release();
			fm->_Delete((byte8*)code, sizeof(sen));
		}
		else
		{
			sen* typen = wbss.sen_cut(code, 0, nameloc - 1);
			sen* params_sen = wbss.sen_cut(code, nameloc + 2, loc - 1);
			sen* param_sen = nullptr;
			temp_mem* tm = nullptr;
			if (params_sen->size() == 0)
			{
				mem[writeup++] = (byte8)insttype::IT_FUNC;	  // FUNC
				mem[writeup++] = (byte8)insttype::EXTENSION_INST; // jmp
				*reinterpret_cast<uint*>(&mem[writeup]) = (uint)(extID);
				writeup += 4;
				*reinterpret_cast<uint*>(&mem[writeup]) = (uint)(exfuncID); // byte8* but real value is function pointer of extension.
				writeup += 4;

				code->release();
				fm->_Delete((byte8*)code, sizeof(sen));

				inner_params->release();
				fm->_Delete((byte8*)inner_params, sizeof(sen));

				params_sen->release();
				fm->_Delete((byte8*)params_sen, sizeof(sen));
				return;
			}
			int last = params_sen->size() - 1;
			//wbss.dbg_sen(params_sen);
			int coma = wbss.search_word_end_in_specific_oc_layer(params_sen, last, "(", ")", 0, ",");
			int savecoma = last + 1;

			int addadd = 0;
			int paramCount = fd->param_data.size() - 1;

			mem[writeup++] = (byte8)insttype::IT_FUNC; // FUNC

			while (coma != -1)
			{
				param_sen = wbss.sen_cut(params_sen, coma + 1, savecoma - 1);
				//wbss.dbg_sen(param_sen);
				tm = get_asm_from_sen(param_sen, true, true);
				ICB_ERR_CHECK(ERR_COMPILEUSEFUNCTION_EXT_TEMPMEM);
				if (tm->valuetype_detail->typetype == 's')
				{
					temp_mem* ptrtm = get_asm_from_sen(param_sen, true, false);
					for (int i = 0; i < ptrtm->mem.size(); ++i)
					{
						mem[writeup++] = ptrtm->mem[i];
					}
					release_tempmem(ptrtm);
					ICB_ERR_CHECK(ERR_COMPILEUSEFUNCTION_EXT_TEMPMEM);
				}
				else
				{
					for (int i = 0; i < tm->mem.size(); ++i)
					{
						mem[writeup++] = tm->mem[i];
					}

					casting_type ct = get_cast_type(tm->valuetype, get_int_with_basictype(fd->param_data.at(paramCount).td));
					if (ct != casting_type::nocasting)
					{
						mem[writeup++] = (byte8)insttype::IT_CASTING_A;
						mem[writeup++] = (byte8)ct;
					}
					/*
					*reinterpret_cast<uint*>(&mem[writeup]) = (uint)ct;
					writeup += 4;
					*/
				}

				if (fd->param_data[paramCount].td->typetype != 's')
				{
					switch (fd->param_data[paramCount].td->typesiz)
					{
					case 1:
						mem[writeup++] = (byte8)insttype::IT_PARAM_1; // param
						break;
					case 2:
						mem[writeup++] = (byte8)insttype::IT_PARAM_2; // param
						break;
					case 4:
						mem[writeup++] = (byte8)insttype::IT_PARAM_4; // param
						break;
					case 8:
						mem[writeup++] = (byte8)insttype::PARAM_8; // param
						break;
					}
				}
				else
				{
					mem[writeup++] = (byte8)insttype::PARAM_N_COPY_BY_ADDRESS; // param N by address(a)
					byte8* N = (byte8*)&fd->param_data[paramCount].td->typesiz;
					// write siz of struct type
					for (int i = 0; i < 4; ++i)
					{
						mem[writeup++] = N[i];
					}
				}

				savecoma = coma;
				coma = wbss.search_word_end_in_specific_oc_layer(params_sen, savecoma - 1, "(", ")", 0, ",");
				// coma = wbss.search_word_first(savecoma+1, params_sen, ",");

				param_sen->release();
				fm->_Delete((byte8*)param_sen, sizeof(sen));

				release_tempmem(tm);
				--paramCount;
			}

			//wbss.dbg_sen(params_sen);

			param_sen = wbss.sen_cut(params_sen, 0, savecoma - 1);
			//wbss.dbg_sen(param_sen);
			tm = get_asm_from_sen(param_sen, true, true);
			ICB_ERR_CHECK(ERR_COMPILEUSEFUNCTION_EXT_TEMPMEM);
			if (tm->valuetype_detail->typetype == 's')
			{
				temp_mem* ptrtm = get_asm_from_sen(param_sen, true, false);
				for (int i = 0; i < ptrtm->mem.size(); ++i)
				{
					mem[writeup++] = ptrtm->mem[i];
				}
				release_tempmem(ptrtm);
				ICB_ERR_CHECK(ERR_COMPILEUSEFUNCTION_EXT_TEMPMEM);
			}
			else
			{
				for (int i = 0; i < tm->mem.size(); ++i)
				{
					mem[writeup++] = tm->mem[i];
				}

				casting_type ct = get_cast_type(tm->valuetype, get_int_with_basictype(fd->param_data.at(paramCount).td));
				if (ct != casting_type::nocasting)
				{
					mem[writeup++] = (byte8)insttype::IT_CASTING_A;
					mem[writeup++] = (byte8)ct;
				}
				/*
				*reinterpret_cast<uint*>(&mem[writeup]) = (uint)ct;
				writeup += 4;
				*/
			}

			if (fd->param_data[paramCount].td->typetype != 's')
			{
				switch (fd->param_data[paramCount].td->typesiz)
				{
				case 1:
					mem[writeup++] = (byte8)insttype::IT_PARAM_1; // param
					break;
				case 2:
					mem[writeup++] = (byte8)insttype::IT_PARAM_2; // param
					break;
				case 4:
					mem[writeup++] = (byte8)insttype::IT_PARAM_4; // param
					break;
				case 8:
					mem[writeup++] = (byte8)insttype::PARAM_8; // param
					break;
				}
			}
			else
			{
				mem[writeup++] = (byte8)insttype::PARAM_N_COPY_BY_ADDRESS; // param N by address(a)
				byte8* N = (byte8*)&fd->param_data[paramCount].td->typesiz;
				// write siz of struct type
				for (int i = 0; i < 4; ++i)
				{
					mem[writeup++] = N[i];
				}
			}

			mem[writeup++] = (byte8)insttype::EXTENSION_INST; // ext instruction
			*reinterpret_cast<uint*>(&mem[writeup]) = (uint)(extID);
			writeup += 4;
			*reinterpret_cast<uint*>(&mem[writeup]) = (uint)(exfuncID); // byte8* but real value is function pointer of extension.
			writeup += 4;

		ERR_COMPILEUSEFUNCTION_EXT_TEMPMEM:

			code->release();
			fm->_Delete((byte8*)code, sizeof(sen));

			inner_params->release();
			fm->_Delete((byte8*)inner_params, sizeof(sen));

			typen->release();
			fm->_Delete((byte8*)typen, sizeof(sen));

			params_sen->release();
			fm->_Delete((byte8*)params_sen, sizeof(sen));

			param_sen->release();
			fm->_Delete((byte8*)param_sen, sizeof(sen));
			release_tempmem(tm);
		}
	}

	void compile_returnInFunction(code_sen* cs)
	{
		sen* code = get_sen_from_codesen(cs);
		int loc = wbss.search_word_first(0, code, "return");
		sen* right_expr = wbss.sen_cut(code, loc + 1, code->size() - 1);
		temp_mem* right_tm = get_asm_from_sen(right_expr, true, true);
		ICB_ERR_CHECK(ERR_COMPILERETURNINFUNCTION_PROBLEM);

		for (int i = 0; i < right_tm->mem.size(); ++i)
		{
			mem[writeup++] = right_tm->mem[i];
		}
		mem[writeup++] = (byte8)insttype::IT_RETURN;

	ERR_COMPILERETURNINFUNCTION_PROBLEM:

		code->release();
		fm->_Delete((byte8*)code, sizeof(sen));

		right_expr->release();
		fm->_Delete((byte8*)right_expr, sizeof(sen));

		release_tempmem(right_tm);
	}

	void compile_addStruct(code_sen* cs)
	{
		sen* code = get_sen_from_codesen(cs);
		int loc = wbss.search_word_first(0, code, "struct");
		struct_data* sd = (struct_data*)fm->_New(sizeof(struct_data), true);
		sd->name = code->at(loc).data.str;
		nextsd = sd;
		nextbd.bs = blockstate::bs_struct;

		code->release();
		fm->_Delete((byte8*)code, sizeof(sen));
	}

	void compile_break(code_sen* cs)
	{
		if (nextbd.breakpoints != nullptr)
		{
			mem[writeup++] = (byte8)insttype::IT_JMP;
			int last = blockstack.size() - 1;
			while (blockstack.at(last)->bs != blockstate::bs_while)
			{
				last -= 1;
			}
			blockstack.at(last)->breakpoints->push_back(writeup);
			writeup += 4;
		}
	}

	void compile_continue(code_sen* cs)
	{
		if (nextbd.continuepoints != nullptr)
		{
			mem[writeup++] = (byte8)insttype::IT_JMP;
			int last = blockstack.size() - 1;
			while (blockstack.at(last)->bs != blockstate::bs_while)
			{
				last -= 1;
			}
			blockstack.at(last)->continuepoints->push_back(writeup);
			writeup += 4;
		}
	}

	void compile_addsetVariable(code_sen* cs)
	{
		sen* code = get_sen_from_codesen(cs);
		//wbss.dbg_sen(code);
		int loc = wbss.search_word_first(0, code, "=");

		code_sen* cs0 = (code_sen*)fm->_New(sizeof(code_sen), true);
		code_sen* cs1 = nullptr;
		int loc1 = 0;
		*cs0 = code_sen();
		cs0->sen = (char**)fm->_New(sizeof(char*) * loc, true);
		cs0->maxlen = loc;
		cs0->codeblocks = nullptr;
		cs0->ck = codeKind::ck_addVariable;
		for (int i = 0; i < loc; ++i)
		{
			cs0->sen[i] = cs->sen[i];
		}
		compile_addVariable(cs0);
		ICB_ERR_CHECK(ERR_COMPILEADDSETVARIABLE_PROBLEM);

		cs1 = (code_sen*)fm->_New(sizeof(code_sen), true);
		*cs1 = code_sen();
		loc1 = cs->maxlen - loc + 1;
		cs1->maxlen = loc1;
		cs1->codeblocks = nullptr;
		cs1->ck = codeKind::ck_setVariable;
		cs1->sen = (char**)fm->_New(sizeof(char*) * loc1, true);
		for (int i = loc - 1; i < cs->maxlen; ++i)
		{
			cs1->sen[i - loc + 1] = cs->sen[i];
		}
		// dbg_codesen(cs1);
		compile_setVariable(cs1);
		ICB_ERR_CHECK(ERR_COMPILEADDSETVARIABLE_PROBLEM);

	ERR_COMPILEADDSETVARIABLE_PROBLEM:

		code->release();
		fm->_Delete((byte8*)code, sizeof(sen));

		if (cs0 != nullptr) {
			fm->_Delete((byte8*)cs0->sen, sizeof(char*) * loc);
		}

		if (cs1 != nullptr) {
			fm->_Delete((byte8*)cs1->sen, sizeof(char*) * loc1);
		}

		fm->_Delete((byte8*)cs0, sizeof(code_sen));
		fm->_Delete((byte8*)cs1, sizeof(code_sen));
	}

	int get_typesiz_with_addVariableCs(code_sen* cs)
	{
		if (cs->ck == codeKind::ck_addVariable)
		{
			sen* code = get_sen_from_codesen(cs);
			// wbss.dbg_sen(code);
			int loc = code->up - 1;
			char* variable_name = code->at(loc).data.str;
			sen* type_name = wbss.sen_cut(code, 0, loc - 1);
			type_data* td = get_type_with_namesen(type_name);
			int n = 0;
			ICB_ERR_CHECK(ERR_GETTYPESIZ_ADDVARCS_TD_NOTEXIST);

			n = td->typesiz;
			code->release();
			fm->_Delete((byte8*)code, sizeof(sen));
			type_name->release();
			fm->_Delete((byte8*)type_name, sizeof(sen));
			return n;

		ERR_GETTYPESIZ_ADDVARCS_TD_NOTEXIST:
			code->release();
			fm->_Delete((byte8*)code, sizeof(sen));
			type_name->release();
			fm->_Delete((byte8*)type_name, sizeof(sen));
			return -1;
		}
		else
		{
			sen* code = get_sen_from_codesen(cs);
			// wbss.dbg_sen(code);
			int loc = wbss.search_word_first(0, code, "=");

			code_sen* cs0 = (code_sen*)fm->_New(sizeof(code_sen), true);
			*cs0 = code_sen();
			cs0->sen = (char**)fm->_New(sizeof(char*) * loc, true);
			cs0->maxlen = loc;
			cs0->codeblocks = nullptr;
			cs0->ck = codeKind::ck_addVariable;
			for (int i = 0; i < loc; ++i)
			{
				cs0->sen[i] = cs->sen[i];
			}

			sen* code2 = get_sen_from_codesen(cs0);
			//wbss.dbg_sen(code2);
			int loc2 = code2->up - 1;
			char* variable_name = code2->at(loc2).data.str;
			sen* type_name = wbss.sen_cut(code2, 0, loc2 - 1);
			int n = 0;
			type_data* td = get_type_with_namesen(type_name);
			ICB_ERR_CHECK(ERR_GETTYPESIZ_NONADDVARCS_TD_NOTEXIST);
			n = td->typesiz;

			ReleaseCodeSen(cs0);
			fm->_Delete((byte8*)cs0, sizeof(code_sen));
			code->release();
			fm->_Delete((byte8*)code, sizeof(sen));
			code2->release();
			fm->_Delete((byte8*)code2, sizeof(sen));
			type_name->release();
			fm->_Delete((byte8*)type_name, sizeof(sen));
			return n;

		ERR_GETTYPESIZ_NONADDVARCS_TD_NOTEXIST:
			ReleaseCodeSen(cs0);
			fm->_Delete((byte8*)cs0, sizeof(code_sen));
			code->release();
			fm->_Delete((byte8*)code, sizeof(sen));
			code2->release();
			fm->_Delete((byte8*)code2, sizeof(sen));
			type_name->release();
			fm->_Delete((byte8*)type_name, sizeof(sen));
			return -1;
		}
	}

	void compile_code(code_sen* cs)
	{
		bool icldetail = GetICLFlag(ICL_FLAG::BakeCode_CompileCodes);
		cs->start_line = writeup;
		currentCodeLine = cs->codeline;
		dbg_codesen(cs, false); icl << endl;
		if (icldetail && cs->ck != codeKind::ck_blocks)
		{
			icl << "BakeCode_CompileCodes__";
			dbg_codesen(cs, false); icl << endl;
		}
		else if (icldetail) {
			icl << "BakeCode_CompileCodes Block Start {" << endl;
		}
		switch (cs->ck)
		{
		case codeKind::ck_addVariable:
			compile_addVariable(cs);
			cs->end_line = writeup - 1;
			if (GetICLFlag(ICL_FLAG::BakeCode_CompileCodes__add_var)) print_asm(cs->start_line, cs->end_line, false);
			break;
		case codeKind::ck_setVariable:
			compile_setVariable(cs);
			cs->end_line = writeup - 1;
			if (GetICLFlag(ICL_FLAG::BakeCode_CompileCodes__set_var)) print_asm(cs->start_line, cs->end_line, false);
			break;
		case codeKind::ck_if:
			compile_if(cs);
			cs->end_line = writeup - 1;
			if (GetICLFlag(ICL_FLAG::BakeCode_CompileCodes__if__sen)) print_asm(cs->start_line, cs->end_line, false);
			break;
		case codeKind::ck_while:
			compile_while(cs);
			cs->end_line = writeup - 1;
			if (GetICLFlag(ICL_FLAG::BakeCode_CompileCodes__while__)) print_asm(cs->start_line, cs->end_line, false);
			break;
		case codeKind::ck_blocks:
			compile_block(cs);
			cs->end_line = writeup - 1;
			//if (GetICLFlag(ICL_FLAG::BakeCode_CompileCodes__add_var)) print_asm(cs->start_line, cs->end_line, false);
			break;
		case codeKind::ck_addFunction:
			compile_addFunction(cs);
			cs->end_line = writeup - 1;
			if (GetICLFlag(ICL_FLAG::BakeCode_CompileCodes__addfunc)) print_asm(cs->start_line, cs->end_line, false);
			break;
		case codeKind::ck_useFunction:
			compile_useFunction(cs);
			cs->end_line = writeup - 1;
			if (GetICLFlag(ICL_FLAG::BakeCode_CompileCodes__usefunc)) print_asm(cs->start_line, cs->end_line, false);
			break;
		case codeKind::ck_returnInFunction:
			compile_returnInFunction(cs);
			cs->end_line = writeup - 1;
			if (GetICLFlag(ICL_FLAG::BakeCode_CompileCodes__return_)) print_asm(cs->start_line, cs->end_line, false);
			break;
		case codeKind::ck_addStruct:
			compile_addStruct(cs);
			cs->end_line = writeup - 1;
			//if (GetICLFlag(ICL_FLAG::addS)) print_asm(cs->start_line, cs->end_line, false);
			break;
		case codeKind::ck_break:
			compile_break(cs);
			cs->end_line = writeup - 1;
			if (GetICLFlag(ICL_FLAG::BakeCode_CompileCodes__break__)) print_asm(cs->start_line, cs->end_line, false);
			break;
		case codeKind::ck_continue:
			compile_continue(cs);
			cs->end_line = writeup - 1;
			if (GetICLFlag(ICL_FLAG::BakeCode_CompileCodes__continue)) print_asm(cs->start_line, cs->end_line, false);
			break;
		case codeKind::ck_addsetVariable:
			compile_addsetVariable(cs);
			cs->end_line = writeup - 1;
			if (GetICLFlag(ICL_FLAG::BakeCode_CompileCodes__adsetvar)) print_asm(cs->start_line, cs->end_line, false);
			break;
		}

		ICB_ERR_CHECK(ERR_COMPILE_CODE_PROBLEM);

		//dbg_bakecode(csarr, 0);
		if (icldetail && cs->ck == codeKind::ck_blocks)
		{
			icl << "}; BakeCode_CompileCodes Block Finish" << endl;
		}

	ERR_COMPILE_CODE_PROBLEM:
		return;
	}

	void read_codes(const char* filename) {
		curErrMsg[0] = 0;
		icl << "ICB[" << this << "] ReadCode start. filename : [" << filename << "]" << endl;

		icl << "ICB[" << this << "] ReadCode_GetCodeFromText...";
		char temp[256] = {};
		GetCurrentDirectoryA(256, temp);
		fmlcstr* allcodeptr = GetCodeTXT(filename);
		SetCurrentDirectoryA(temp);
		icl << "finish" << endl;

		ICB_ERR_CHECK(ERR_READCODE_GETCODETXT);
		{
			fmlcstr& allcode = *allcodeptr;
			icl << "ICB[" << this << "] ReadCode_AddTextBlocks...";
			AddTextBlocks(allcode);
			allcodeptr->release();
			fm->_Delete((byte8*)allcodeptr, sizeof(fmlcstr));
			allcodeptr = nullptr;
			icl << "finish" << endl;
		}

	ERR_READCODE_GETCODETXT:
		if (allcodeptr != nullptr) {
			allcodeptr->release();
			fm->_Delete((byte8*)allcodeptr, sizeof(fmlcstr));
			allcodeptr = nullptr;
		}
		return;
	}

	void read_codeLines(fmvecarr<fmlwstr>* codeLines) {
		curErrMsg[0] = 0;
		icl << "ICB[" << this << "] ReadCode start. filename : custom codelines" << endl;

		icl << "ICB[" << this << "] ReadCode_GetCodeFromText...";
		fmlcstr* allcodeptr = GetCode_Lines(codeLines);
		icl << "finish" << endl;

		ICB_ERR_CHECK(ERR_READCODE_GETCODETXT);
		{
			fmlcstr& allcode = *allcodeptr;
			icl << "ICB[" << this << "] ReadCode_AddTextBlocks...";
			AddTextBlocks(allcode);
			allcodeptr->release();
			fm->_Delete((byte8*)allcodeptr, sizeof(fmlcstr));
			allcodeptr = nullptr;
			icl << "finish" << endl;
		}

	ERR_READCODE_GETCODETXT:
		if (allcodeptr != nullptr) {
			allcodeptr->release();
			fm->_Delete((byte8*)allcodeptr, sizeof(fmlcstr));
			allcodeptr = nullptr;
		}
		return;
	}

	void create_codedata() {
		bool astdetail = GetICLFlag(ICL_FLAG::BakeCode_AddStructTypes);
		bool gmidetail = GetICLFlag(ICL_FLAG::BakeCode_GlobalMemoryInit);
		fmvecarr<code_sen*>* senptr = nullptr;
		fmvecarr<code_sen*>* senstptr = nullptr;
		int gs = 0;

		icl << "ICB[" << this << "] ReadCode_ScanStructTypes...";
		senstptr = AddCodeFromBlockData(allcode_sen, "struct");
		icl << "finish" << endl;

		icl << "ICB[" << this << "] ReadCode_AddStructTypes...";

		if (astdetail) icl << "start" << endl;
		for (int i = 0; i < senstptr->size(); ++i)
		{
			if (astdetail) icl << "ReadCode_AddStructTypes interpret" << endl;
			code_sen* cs = senstptr->at(i);
			dbg_codesen(cs, false); icl << endl;
			if (astdetail) icl << "...start" << endl;
			interpret_AddStruct(cs);

			if (astdetail) icl << "ReadCode_AddStructTypes interpret finish" << endl;
		}
		if (astdetail) icl << "ReadCode_AddStructTypes...";
		icl << "finish" << endl;

		icl << "ICB[" << this << "] ReadCode_ScanCodes...";
		senptr = AddCodeFromBlockData(allcode_sen, "none", 0);
		icl << "finish" << endl;
		senptr->islocal = false;
		csarr = senptr;

		cout << endl;

		icl << "ICB[" << this << "] ReadCode_GlobalMemoryInit...";

		init_datamem.NULLState();
		init_datamem.Init(8, false, true);

		if (gmidetail) icl << "start" << endl;
		for (int i = 0; i < senptr->size(); ++i)
		{
			code_sen* cs = senptr->at(i);
			if (cs->ck == codeKind::ck_addVariable || cs->ck == codeKind::ck_addsetVariable)
			{
				if (gmidetail) icl << "ReadCode_GlobalMemoryInit Scan Global Memory Code (in datamem[" << gs << "]) : ";
				if (gmidetail) { dbg_codesen(cs, false); icl << endl; }
				// global var count
				if (gmidetail) icl << "ReadCode_GlobalMemoryInit Find Type Size : ...";
				int siz = get_typesiz_with_addVariableCs(cs);
				if (gmidetail) icl << siz << endl;
				for (int k = 0; k < siz; ++k)
				{
					init_datamem.push_back(0);
				}
				byte8* bptr = &init_datamem.at(gs);
				gs += siz;

				if (cs->ck == codeKind::ck_addsetVariable)
				{
					if (gmidetail) icl << "ReadCode_GlobalMemoryInit addsetvar set initial value : ...";
					sen* code = get_sen_from_codesen(cs);
					int loc = wbss.search_word_first(0, code, "=");

					fmlcstr str;
					str.NULLState();
					str.Init(2, false);
					str = code->at(loc + 1).data.str;
					TBT t = DecodeTextBlock(str);
					switch (t)
					{
					case TBT::_value_bool:
					{
						bool b = true;
						if (strcmp(str.c_str(), "true") == 0)
							b = true;
						else
							b = false;
						*reinterpret_cast<bool*>(bptr) = b;
						if (gmidetail) icl << "(bool)" << b << endl;
					}
					break;
					case TBT::_value_integer:
					{
						int a = 0;
						a = atoi(str.c_str());
						*reinterpret_cast<int*>(bptr) = a;
						if (gmidetail) icl << "(int)" << a << endl;
					}
					break;
					case TBT::_value_float:
					{
						float a = 0;
						a = stof(str.c_str());
						*reinterpret_cast<float*>(bptr) = a;
						if (gmidetail) icl << "(float)" << a << endl;
					}
					break;
					case TBT::_value_char:
					{
						char c = 0;
						if (str[1] != '\\')
						{
							c = str[1];
						}
						else
						{
							switch (str[2])
							{
							case 'n':
								c = '\n';
								break;
							case '0':
								c = 0;
								break;
							case 't':
								c = '\t';
								break;
							case '\\':
								c = '\\';
								break;
							case '\'':
								c = '\'';
								break;
							case '\"':
								c = '\"';
								break;
							default:
								c = 0;
								break;
							}
						}
						*reinterpret_cast<char*>(bptr) = c;
						if (gmidetail) icl << "(char)" << c << endl;
					}
					break;
					case TBT::_value_str:
					{
						char* initbptr = (char*)bptr;
						int max = strlen(str.c_str()) - 1;
						max = max > siz ? siz : max;
						int stack = 1;
						int sup = 0;
						for (int k = 1; k < max; ++k)
						{
							if (str[k] == '\\')
							{
								stack += 1;
								k += 1;
								switch (str[k])
								{
								case 'n':
									*(char*)bptr = '\n';
									++bptr;
									break;
								case '0':
									*(char*)bptr = 0;
									++bptr;
									break;
								case 't':
									*(char*)bptr = '\t';
									++bptr;
									break;
								case '\\':
									*(char*)bptr = '\\';
									++bptr;
									break;
								case '\'':
									*(char*)bptr = '\'';
									++bptr;
									break;
								case '\"':
									*(char*)bptr = '\"';
									++bptr;
									break;
								default:
									*(char*)bptr = 0;
									++bptr;
									break;
								}
								continue;
							}
							*(char*)bptr = str[k];
							++bptr;
						}
						for (int k = 0; k < stack; ++k)
						{
							*(char*)bptr = 0;
							++bptr;
						}

						if (gmidetail) icl << "(char*)" << initbptr << endl;
					}
					break;
					default:
						if (gmidetail) icl << "ERROR : Invailabe Value : " << bptr << endl;
						break;
					}
					// tm->valuetype_detail = get_basic_type_with_int(tm->valuetype);
					str.islocal = true;

					code->release();
					fm->_Delete((byte8*)code, sizeof(sen));
				}
			}
		}
		datamem_up = gs;
		if (gmidetail) icl << "ReadCode_GlobalMemoryInit...";
		icl << "finish" << endl;

		senstptr->release();
		senstptr->NULLState();
		fm->_Delete((byte8*)senstptr, sizeof(fmvecarr<code_sen*>));
		return;
	}

	void compile_codes() {
		able_to_execute = false;
		bool ccdetail = GetICLFlag(ICL_FLAG::BakeCode_CompileCodes);

		writeup = 0;
		mem[writeup++] = (byte8)insttype::IT_FUNC; // func
		mem[writeup++] = (byte8)insttype::IT_FUNCJMP; // jmp
		writeup += 4;		  // start function address

		icl << "ICB[" << this << "] BakeCode_CompileCodes...";

		if (ccdetail) icl << "start" << endl;
		for (int i = 0; i < csarr->size(); ++i)
		{
			// fm->dbg_fm1_lifecheck();
			code_sen* cs = csarr->at(i);
			//dbg_codesen(cs);
			compile_code(cs);
			ICB_ERR_CHECK(ERR_BAKECODE_PROBLEM);
		}
		if (ccdetail) icl << "BakeCode_CompileCodes...";
		icl << "finish" << endl;

		cout << endl;

		mem[writeup++] = (byte8)insttype::IT_EXIT;

		//dbg_bakecode(csarr, 0);

		icl << "ICB[" << this << "] BakeCode finish." << endl;

		changeHash += 1;
		able_to_execute = true;
		return;

	ERR_BAKECODE_PROBLEM:
		writeup = 0;
		mem[writeup++] = (byte8)insttype::IT_EXIT;
		Release(false);
		return;
	}

	void bake_code(const char* filename, fmlcstr* PtrErrorCode = nullptr)
	{
		curErrMsg[0] = 0;
		bool astdetail = GetICLFlag(ICL_FLAG::BakeCode_AddStructTypes);
		bool gmidetail = GetICLFlag(ICL_FLAG::BakeCode_GlobalMemoryInit);
		bool ccdetail = GetICLFlag(ICL_FLAG::BakeCode_CompileCodes);
		int gs = 0;
		fmvecarr<code_sen*>* senptr = nullptr;
		fmvecarr<code_sen*>* senstptr = nullptr;

		icl << "ICB[" << this << "] BakeCode start. filename : [" << filename << "]" << endl;

		icl << "ICB[" << this << "] BakeCode_GetCodeFromText...";
		char temp[256] = {};
		GetCurrentDirectoryA(256, temp);
		fmlcstr* allcodeptr = GetCodeTXT(filename);
		SetCurrentDirectoryA(temp);
		icl << "finish" << endl;

		ICB_ERR_CHECK(ERR_BAKECODE_GETCODETXT);

		{
			fmlcstr& allcode = *allcodeptr;
			icl << "ICB[" << this << "] BakeCode_AddTextBlocks...";
			AddTextBlocks(allcode);
			allcodeptr->release();
			fm->_Delete((byte8*)allcodeptr, sizeof(fmlcstr));
			allcodeptr = nullptr;
			icl << "finish" << endl;
		}

		icl << "ICB[" << this << "] BakeCode_ScanStructTypes...";
		senstptr = AddCodeFromBlockData(allcode_sen, "struct");
		icl << "finish" << endl;

		icl << "ICB[" << this << "] BakeCode_AddStructTypes...";

		if (astdetail) icl << "start" << endl;
		for (int i = 0; i < senstptr->size(); ++i)
		{
			if (astdetail) icl << "BakeCode_AddStructTypes interpret" << endl;
			code_sen* cs = senstptr->at(i);
			dbg_codesen(cs, false); icl << endl;
			if (astdetail) icl << "...start" << endl;
			interpret_AddStruct(cs);

			if (astdetail) icl << "BakeCode_AddStructTypes interpret finish" << endl;
		}
		if (astdetail) icl << "BakeCode_AddStructTypes...";
		icl << "finish" << endl;

		icl << "ICB[" << this << "] BakeCode_ScanCodes...";
		senptr = AddCodeFromBlockData(allcode_sen, "none", 0);
		icl << "finish" << endl;
		senptr->islocal = false;
		csarr = senptr;

		cout << endl;

		icl << "ICB[" << this << "] BakeCode_GlobalMemoryInit...";

		init_datamem.NULLState();
		init_datamem.Init(8, false);

		if (gmidetail) icl << "start" << endl;
		for (int i = 0; i < senptr->size(); ++i)
		{
			code_sen* cs = senptr->at(i);
			if (cs->ck == codeKind::ck_addVariable || cs->ck == codeKind::ck_addsetVariable)
			{
				if (gmidetail) icl << "BakeCode_GlobalMemoryInit Scan Global Memory Code (in datamem[" << gs << "]) : ";
				if (gmidetail) { dbg_codesen(cs, false); icl << endl; }
				// global var count
				if (gmidetail) icl << "BakeCode_GlobalMemoryInit Find Type Size : ...";
				int siz = get_typesiz_with_addVariableCs(cs);
				if (gmidetail) icl << siz << endl;
				for (int k = 0; k < siz; ++k)
				{
					init_datamem.push_back(0);
				}
				byte8* bptr = &init_datamem.at(gs);
				gs += siz;

				if (cs->ck == codeKind::ck_addsetVariable)
				{
					if (gmidetail) icl << "BakeCode_GlobalMemoryInit addsetvar set initial value : ...";
					sen* code = get_sen_from_codesen(cs);
					int loc = wbss.search_word_first(0, code, "=");

					fmlcstr str;
					str.NULLState();
					str.Init(2, false);
					str = code->at(loc + 1).data.str;
					TBT t = DecodeTextBlock(str);
					switch (t)
					{
					case TBT::_value_bool:
					{
						bool b = true;
						if (strcmp(str.c_str(), "true") == 0)
							b = true;
						else
							b = false;
						*reinterpret_cast<bool*>(bptr) = b;
						if (gmidetail) icl << "(bool)" << b << endl;
					}
					break;
					case TBT::_value_integer:
					{
						int a = 0;
						a = atoi(str.c_str());
						*reinterpret_cast<int*>(bptr) = a;
						if (gmidetail) icl << "(int)" << a << endl;
					}
					break;
					case TBT::_value_float:
					{
						float a = 0;
						a = stof(str.c_str());
						*reinterpret_cast<float*>(bptr) = a;
						if (gmidetail) icl << "(float)" << a << endl;
					}
					break;
					case TBT::_value_char:
					{
						char c = 0;
						if (str[1] != '\\')
						{
							c = str[1];
						}
						else
						{
							switch (str[2])
							{
							case 'n':
								c = '\n';
								break;
							case '0':
								c = 0;
								break;
							case 't':
								c = '\t';
								break;
							case '\\':
								c = '\\';
								break;
							case '\'':
								c = '\'';
								break;
							case '\"':
								c = '\"';
								break;
							default:
								c = 0;
								break;
							}
						}
						*reinterpret_cast<char*>(bptr) = c;
						if (gmidetail) icl << "(char)" << c << endl;
					}
					break;
					case TBT::_value_str:
					{
						char* initbptr = (char*)bptr;
						int max = strlen(str.c_str()) - 1;
						max = max > siz ? siz : max;
						int stack = 1;
						int sup = 0;
						for (int k = 1; k < max; ++k)
						{
							if (str[k] == '\\')
							{
								stack += 1;
								k += 1;
								switch (str[k])
								{
								case 'n':
									*(char*)bptr = '\n';
									++bptr;
									break;
								case '0':
									*(char*)bptr = 0;
									++bptr;
									break;
								case 't':
									*(char*)bptr = '\t';
									++bptr;
									break;
								case '\\':
									*(char*)bptr = '\\';
									++bptr;
									break;
								case '\'':
									*(char*)bptr = '\'';
									++bptr;
									break;
								case '\"':
									*(char*)bptr = '\"';
									++bptr;
									break;
								default:
									*(char*)bptr = 0;
									++bptr;
									break;
								}
								continue;
							}
							*(char*)bptr = str[k];
							++bptr;
						}
						for (int k = 0; k < stack; ++k)
						{
							*(char*)bptr = 0;
							++bptr;
						}

						if (gmidetail) icl << "(char*)" << initbptr << endl;
					}
					break;
					default:
						if (gmidetail) icl << "ERROR : Invailabe Value : " << bptr << endl;
						break;
					}
					// tm->valuetype_detail = get_basic_type_with_int(tm->valuetype);
					str.islocal = true;

					code->release();
					fm->_Delete((byte8*)code, sizeof(sen));
				}
			}
		}
		datamem_up = gs;
		if (gmidetail) icl << "BakeCode_GlobalMemoryInit...";
		icl << "finish" << endl;

		able_to_execute = false;
		writeup = 0;
		mem[writeup++] = (byte8)insttype::IT_FUNC; // func
		mem[writeup++] = (byte8)insttype::IT_FUNCJMP; // jmp
		writeup += 4;		  // start function address

		icl << "ICB[" << this << "] BakeCode_CompileCodes...";

		if (ccdetail) icl << "start" << endl;
		for (int i = 0; i < senptr->size(); ++i)
		{
			// fm->dbg_fm1_lifecheck();
			code_sen* cs = senptr->at(i);
			//dbg_codesen(cs);
			compile_code(cs);
			ICB_ERR_CHECK(ERR_BAKECODE_PROBLEM);
		}
		if (ccdetail) icl << "BakeCode_CompileCodes...";
		icl << "finish" << endl;

		cout << endl;

		mem[writeup++] = (byte8)insttype::IT_EXIT;

		able_to_execute = true;

		senstptr->release();
		senstptr->NULLState();
		fm->_Delete((byte8*)senstptr, sizeof(fmvecarr<code_sen*>));

		//dbg_bakecode(csarr, 0, false);

		icl << "ICB[" << this << "] BakeCode finish." << endl;

		return;

	ERR_BAKECODE_GETCODETXT:
		if (allcodeptr != nullptr) {
			allcodeptr->release();
			fm->_Delete((byte8*)allcodeptr, sizeof(fmlcstr));
			allcodeptr = nullptr;
		}
		return;

	ERR_BAKECODE_PROBLEM:
		senstptr->release();
		senstptr->NULLState();
		fm->_Delete((byte8*)senstptr, sizeof(fmvecarr<code_sen*>));
		if (PtrErrorCode != nullptr) {
			*PtrErrorCode = curErrMsg;
		}
		Release(false);
		return;
	}

	fmvecarr<NamingData> GetLocalVariableListInBlock(code_sen* blockcs, int stack) {
		fmvecarr<NamingData> ndarr;
		ndarr.NULLState();
		ndarr.Init(8, false, true);
		int addvarstack = stack;
		for (int i = 0; i < blockcs->codeblocks->size(); ++i) {
			code_sen* cs = (code_sen*)blockcs->codeblocks->at(i);
			if (cs->ck == codeKind::ck_addVariable){
				sen* code = get_sen_from_codesen(cs);
				//wbss.dbg_sen(code);
				int loc = code->up - 1;
				char* variable_name = code->at(loc).data.str;
				sen* type_name = wbss.sen_cut(code, 0, loc - 1);
				NamingData nd;
				nd.name = variable_name;
				nd.add_address = addvarstack;
				nd.td = get_type_with_namesen(type_name);
				addvarstack += nd.td->typesiz;
				ndarr.push_back(nd);
				code->release();
				type_name->release();
			}
			else if(cs->ck == codeKind::ck_addsetVariable) {
				sen* code = get_sen_from_codesen(cs);
				//wbss.dbg_sen(code);
				int loc = wbss.search_word_first(0, code, "=");
				sen* code_0 = wbss.sen_cut(code, 0, loc - 1);
				loc = code_0->up - 1;
				char* variable_name = code_0->at(loc).data.str;
				sen* type_name = wbss.sen_cut(code_0, 0, loc - 1);
				NamingData nd;
				nd.name = variable_name;
				nd.add_address = addvarstack;
				nd.td = get_type_with_namesen(type_name);
				addvarstack += nd.td->typesiz;
				ndarr.push_back(nd);
				code->release();
				code_0->release();
				type_name->release();
			}
			else if (cs->ck == codeKind::ck_blocks) {
				fmvecarr<NamingData> sndarr = GetLocalVariableListInBlock(cs, addvarstack);
				for (int k = 0; k < sndarr.size(); ++k) {
					ndarr.push_back(sndarr.at(k));
				}
				sndarr.release();
			}
		}

		return ndarr;
	}

	fmvecarr<NamingData> GetLocalVariableListInFunction(func_data* fd) {
		fmvecarr<NamingData> ndarr;
		ndarr.NULLState();
		ndarr.Init(8, false, true);

		code_sen* blocksen = nullptr;
		for (int i = 0; i < csarr->size(); ++i) {
			code_sen* cs = csarr->at(i);
			if (csarr->at(i)->ck == codeKind::ck_addFunction) {
				sen* code = get_sen_from_codesen(cs);
				int loc = code->size() - 1;
				sen* inner_params = wbss.oc_search_inv(code, loc, "(", ")");
				int nameloc = loc - inner_params->size();
				inner_params->release();
				if (fd->name == code->at(nameloc).data.str) {
					blocksen = csarr->at(i + 1);
					break;
				}
				code->release();
			}
		}

		if (blocksen != nullptr) {
			fmvecarr<NamingData> sndarr = GetLocalVariableListInBlock(blocksen, 0);
			for (int k = 0; k < sndarr.size(); ++k) {
				ndarr.push_back(sndarr.at(k));
			}
			sndarr.release();
		}

		return ndarr;
	}
};


void EXT_ReleaseTypeData(type_data* td, ICB_Extension* ext)
{
	if (td->structptr != nullptr && td->typetype != 'b')
	{
		bool isRelease = true;
		if (td->typetype == 's')
		{
			for (int i = 0; i < ext->exstructArr.size(); ++i)
			{
				if (strcmp(td->name.c_str(), ext->exstructArr.at(i)->name.c_str()) == 0)
				{
					isRelease = false;
					break;
				}
			}
		}
		else if (td->typetype == 'b')
		{
			isRelease = false;
		}

		if (isRelease == false) return;

		if (td->typetype == 's')
		{
			struct_data* sd = (struct_data*)td->structptr;
			sd->member_data.release();
			sd->member_data.NULLState();
			sd->name.release();
			sd->name.NULLState();
			fm->_Delete((byte8*)sd, sizeof(struct_data));
			td->structptr = nullptr;
		}
		else
		{
			type_data* std = (type_data*)td->structptr;
			EXT_ReleaseTypeData(std, ext);
		}

		td->name.release();
		td->name.NULLState();

		fm->_Delete((byte8*)td, sizeof(type_data));
	}
}

void ICB_Extension::Release()
{
	for (int i = 0; i < exfuncArr.size(); ++i)
	{
		func_data* fd = exfuncArr.at(i);
		fd->name.release();
		fd->name.NULLState();
		bool isRelease = true;
		if (fd->returntype->typetype == 's') {
			for (int i = 0; i < exstructArr.size(); ++i) {
				if (strcmp(fd->returntype->name.c_str(), exstructArr.at(i)->name.c_str()) == 0) {
					isRelease = false;
					break;
				}
			}
		}
		else if (fd->returntype->typetype == 'b') {
			isRelease = false;
		}

		if (isRelease) {
			EXT_ReleaseTypeData(fd->returntype, this);
		}
		fd->returntype = nullptr;

		fd->start_pc = nullptr;

		for (int i = 0; i < fd->param_data.size(); ++i) {
			NamingData nd;
			nd = fd->param_data.at(i);
			nd.name = nullptr;

			isRelease = true;
			if (nd.td->typetype == 's')
			{
				for (int i = 0; i < exstructArr.size(); ++i)
				{
					if (strcmp(nd.td->name.c_str(), exstructArr.at(i)->name.c_str()) == 0)
					{
						isRelease = false;
						break;
					}
				}
			}
			else if (nd.td->typetype == 'b') {
				isRelease = false;
			}

			if (isRelease) {
				EXT_ReleaseTypeData(nd.td, this);
			}
			nd.td = nullptr;
		}
		fd->param_data.release();
		fd->param_data.NULLState();

		fm->_Delete((byte8*)fd, sizeof(func_data));
		exfuncArr.at(i) = nullptr;
	}
	exfuncArr.release();
	exfuncArr.NULLState();

	for (int i = 0; i < exstructArr.size(); ++i)
	{
		type_data* td = exstructArr.at(i);
		struct_data* sd = (struct_data*)td->structptr;

		for (int k = 0; k < sd->member_data.size(); ++k)
		{
			char* cstr = sd->member_data.at(k).name;
			int csiz = strlen(cstr) + 1;
			fm->_Delete((byte8*)cstr, csiz);
		}

		sd->member_data.release();
		sd->member_data.NULLState();
		sd->name.release();
		sd->name.NULLState();
		fm->_Delete((byte8*)sd, sizeof(struct_data));
		td->name.release();
		td->name.NULLState();
		td->structptr = nullptr;
		fm->_Delete((byte8*)td, sizeof(type_data));
		exstructArr.at(i) = nullptr;
	}
	exstructArr.release();
	exstructArr.NULLState();
}

class ICB_Context{
    public:
	bool ExeState = false;
    InsideCode_Bake* icb;
	byte8* codemem = nullptr;

    // execute var
	uint32_t max_mem_byte = 40960; // 40KB
	byte8 *mem = nullptr;
	fmvecarr<byte8> datamem;
	int inherit_limit = 0;
	//int dataptr = max_mem_byte;

	static constexpr unsigned int percent16 = 15;
	int apivot = 0;
	int bpivot = 0;
	uint64_t _as[16] = {};
	uint64_t _bs[16] = {};

	byte8 *pc = 0; // program counter
	byte8 *sp = 0; // stack pointer

	fmvecarr<byte8 *> fsp;
	fmvecarr<byte8 *> call_stack;

	byte8 *rfsp = 0; // function stack pos
	byte8 *lfsp = 0; // last function stack pos
	fmvecarr<byte8*> saveSP; // function save stack pos

	uint64_t _la = 0; // left address
	int changeHash = 0;

	//debug member variables
	bool isBreaking = false;
	int stopnum = -1;
	int stop_callstack = -1;
	bool isDbg = false;

    ICB_Context(){}
    ~ICB_Context(){}

	inline uint64_t& getA(int index) {
		return _as[(index + apivot) & percent16];
	}

	inline void Amove_pivot(int delta) {
		apivot += delta;
	}

	inline uint64_t& getB(unsigned int index) {
		return _bs[(index + bpivot) & percent16];
	}

	inline void Bmove_pivot(unsigned int delta) {
		apivot += delta;
	}

    void SetICB(InsideCode_Bake* picb, uint maxmembyte){
		ofstream& icl = InsideCode_Bake::icl;
		icl << "Create_New_ICB_Context From ICB[" << picb << "], with " << maxmembyte << " size memory...";
        icb = picb;
		
		codemem = icb->mem;
		pc = &codemem[0];

		fsp.NULLState();
		fsp.Init(2, false);

		call_stack.NULLState();
		call_stack.Init(2, false);

		datamem.NULLState();
		datamem.Init(icb->init_datamem.size() + 8, false);
		datamem.up = icb->init_datamem.size();
		for (int i = 0; i < datamem.up; ++i) {
			datamem[i] = icb->init_datamem.at(i);
		}

		saveSP.NULLState();
		saveSP.Init(32, false);

		max_mem_byte = maxmembyte;
		mem = (byte8*)fm->_New(max_mem_byte, true);
		sp = &mem[max_mem_byte - 1];
		lfsp = sp;
		rfsp = sp;

		for (int i = 0; i < 16; ++i) {
			_as[i] = 0;
			_bs[i] = 0;
		}
		apivot = 0;
		bpivot = 0;
		inherit_limit = 0;
		changeHash = icb->changeHash;

		isBreaking = false;
		stopnum = -1;
		isDbg = false;

		icl << "finish.";
    }

	void Push_InheritData(unsigned int size, byte8* dataptr) {
		for (int i = 0; i < size; ++i) {
			datamem[inherit_limit + i] = dataptr[i];
		}
		inherit_limit += size;
	}

	void Release() {
		ExeState = false;
		icb = nullptr;
		codemem = nullptr;
		fm->_Delete((byte8*)mem, max_mem_byte);
		mem = nullptr;
		datamem.release();
		datamem.NULLState();
		fsp.release();
		fsp.NULLState();
		call_stack.release();
		call_stack.NULLState();
		saveSP.release();
		saveSP.NULLState();
	}

    void dbg_registers()
	{
		ofstream& ofs = InsideCode_Bake::icl;
		ofs << "a : " << (uint)getA(0) << "\t(";
		for (int i = 1; i < 4; ++i)
		{
			ofs << " | " << getA(i);
		}
		ofs << endl;
		ofs << "b : " << (uint)getB(0) << "\t(";
		for (int i = 1; i < 4; ++i)
		{
			ofs << " | " << getB(i);
		}
		ofs << endl;
	}

	void dbg_stack()
	{
		ofstream& ofs = InsideCode_Bake::icl;
		ofs << "stack mem" << endl;
		for (byte8 *ptr = sp; ptr != &mem[max_mem_byte - 1]; ++ptr)
		{
			ofs << (int)*ptr << ' ';
		}
		ofs << endl;
	}

	void dbg_data(){
		ofstream& ofs = InsideCode_Bake::icl;
		ofs << "data mem" << endl;
		for (int i=0;i<datamem.size();++i)
		{
			ofs << (int)datamem[i] << ' ';
		}
		ofs << endl;
	}

    void dbg_pc()
	{
		ofstream& ofs = InsideCode_Bake::icl;
		if (icb->inst_meta[*pc].param_num < 0)
		{
			return;
		}
		code_sen *cs = icb->find_codesen_with_linenum(icb->csarr, (int)(pc - codemem));
		if (cs != nullptr)
		{
			dbg_codesen(cs, ofs);
		}
		ofs << (int)(pc - codemem) << " : " << icb->inst_meta[*pc].name << "(" << (uint)*pc << ")";
		instruct_data id = icb->inst_meta[*pc];
		int n;
		for (int k = 0; k < id.param_num; ++k)
		{
			switch (id.param_typesiz[k])
			{
			case 1:
				n = (uint) * (pc + 1);
				ofs << " > " << n;
				break;
			case 2:
				n = *reinterpret_cast<ushort *>(pc + 1);
				ofs << " > " << n << "(" << (uint) * (pc + 1) << ", " << (uint) * (pc + 2) << ")";
				break;
			case 4:
				n = *reinterpret_cast<uint *>(pc + 1);
				ofs << " > " << n << "(" << (uint) * (pc + 1) << ", " << (uint) * (pc + 2) << ", " << (uint) * (pc + 3) << ", " << (uint) * (pc + 4) << ")";
				break;
			case 8:
				n = *reinterpret_cast<uint64_t *>(pc + 1);
				ofs << " > " << n << "(" << (uint) * (pc + 1) << ", " << (uint) * (pc + 2) << ", " << (uint) * (pc + 3) << ", " << (uint) * (pc + 4)
				<< (uint) * (pc + 5) << ", " << (uint) * (pc + 6) << ", " << (uint) * (pc + 7) << ", " << (uint) * (pc + 8) << ")";
				break;
			}
		}
		ofs << endl;
	}
};

#define registerA0 _as[apivot]
#define registerB0 _bs[bpivot]
#define shiftA0(delta) apivot = (apivot + (delta)) & percent16
#define shiftB0(delta) bpivot = (bpivot + (delta)) & percent16

fmvecarr<ICB_Context *> icbarr;
int icbindex_cxt = 0;

//bool isBreaking = false; //cxt.isBreaking
//int stopnum = 1985; // cxt.stopnum
//bool isDbg = true; // icb.isDbg

int code_control(fmvecarr<ICB_Context *> *icbarr)
{
	static int stack = 0;

	/*if(isBreaking == false){
		return 1;
	}*/

	for (int i = 0; i < icbarr->size(); ++i)
	{
		//cout << "thread[ " << i << " ] next instruction" << endl;
		//icbarr->at(i)->dbg_pc();
		//icbarr->at(i)->dbg_stack();
		//icbarr->at(i)->dbg_data();
		//icbarr->at(i)->dbg_registers();
	}

	char c = 1;
	stack++;
	if (stack >= 1)
	{
		//scanf_s("%c", &c);
		stack = 0;
	}
	switch (c)
	{
	case 'e':
		return 0;
	}

	return 1; // keep going
}

void execute_switch(fmvecarr<ICB_Context*> icbarr, int execodenum,
	int (*control_func)(fmvecarr<ICB_Context*>*), float exerate, int cxtindex)
{
	constexpr unsigned int percent16 = 15;

	unsigned int ftime = getticks();
	unsigned int etime = ftime;

	byte8* tmptr_b;
	ushort* tmptr_s;
	uint* tmptr_i;
	register uint64_t _a = 0;
	register uint64_t _b = 0;
	register uint64_t _x = 0;
	register uint64_t _y = 0;
	register uint64_t _la = 0;
	uint64_t casting_value = 0;
	uint64_t casted_value = 0;
	void* castend_label = nullptr;
	float fmem = 0;
	void (*extContainer)(int* ic) = nullptr;
	ICB_Extension* CurrentExtension = nullptr;

	int n = 0;
	int max = icbarr.size();
	int maxo = max;
	int exed_num = 0;
	uint strmax = 0;
	uint paramSiz = 0;
	uint variableID = 0;
	ICB_Context* icb;

	byte8** pc = nullptr; // program counter
	byte8** sp = nullptr; // stack pointer

	byte8** pcb = nullptr;
	ushort** pcs = nullptr;
	uint** pci = nullptr;

	byte8** spb = nullptr;
	ushort** sps = nullptr;
	uint** spi = nullptr;

	fmvecarr<byte8*>* fsp = nullptr;
	fmvecarr<byte8*>* call_stack = nullptr;
	uint64_t* _as = nullptr;
	uint64_t* _bs = nullptr;
	int apivot = 0;
	int bpivot = 0;

	byte8** rfsp = 0; // function stack pos
	byte8** lfsp = 0; // last function stack pos
	fmvecarr<byte8*>* saveSP = nullptr; // function save stack pos

	byte8** rfspb = nullptr;
	ushort** rfsps = nullptr;
	uint** rfspi = nullptr;

	byte8** lfspb = nullptr;
	ushort** lfsps = nullptr;
	uint** lfspi = nullptr;

	byte8* mem = nullptr;
	byte8* codemem = nullptr;
	void** inst = nullptr;
	void** cast = nullptr;
	void** dbgt = nullptr;
	void** inpt = nullptr;

	casttype castv = (casttype)0;
	int selectRegister = 0;

	dbgtype dbg_type = (dbgtype)0;
	inptype inp_type = (inptype)0;

	goto CONTEXT_SWITCH;

CONTEXT_SWITCH:

	etime = getticks();
	float delta = getDeltaTime_per60(ftime, etime);
	//dbg << "time : " << delta << endl;
	if (delta > exerate)
	{
		goto PROGRAMQUIT;
	}

	if (n == maxo)
	{
		// control code
		n = 0;
		int k = control_func(&icbarr);
		switch (k)
		{
		case 0:
			goto PROGRAMQUIT;
			break;
		case 1:
			goto CONTEXT_SWITCH;
			break;
		}
	}
	exed_num = 0;

	while (icbarr[n]->icb->able_to_execute == false || icbarr[n]->ExeState == false) {
		//icbarr[n]->ExeState = false;
		++n;
		if (n >= icbarr.size()) {
			n = 0;
			goto PROGRAMQUIT;
			break;
		}
	}
	if (icbarr[n]->changeHash != icbarr[n]->icb->changeHash) {
		int maxmembyte = icbarr[n]->max_mem_byte;
		InsideCode_Bake* tempicb = icbarr[n]->icb;
		int inheritlimit = icbarr[n]->inherit_limit;
		byte8* inherit_data = fm->_New(icbarr[n]->inherit_limit, true);
		for (int i = 0; i < icbarr[n]->inherit_limit; ++i) {
			inherit_data[i] = icbarr[n]->datamem[i];
		}
		icbarr[n]->Release();
		icbarr[n]->SetICB(tempicb, maxmembyte);
		icbarr[n]->Push_InheritData(inheritlimit, inherit_data);
		fm->_Delete(inherit_data, icbarr[n]->inherit_limit);
		//icbarr[n]->ExeState = true;
	}

	icb = icbarr[n];

	_la = icb->_la;
	_as = icb->_as;
	_bs = icb->_bs;
	apivot = icb->apivot;
	bpivot = icb->bpivot;

	mem = icb->mem;
	codemem = icb->codemem;

	pc = &icb->pc; // program counter
	sp = &icb->sp; // stack pointer

	fsp = &icb->fsp;
	call_stack = &icb->call_stack;

	rfsp = &icb->rfsp; // function stack pos
	lfsp = &icb->lfsp; // last function stack pos
	saveSP = &icb->saveSP;

	pcb = pc;
	pcs = reinterpret_cast<ushort**>(pc);
	pci = reinterpret_cast<uint**>(pc);

	spb = sp;
	sps = reinterpret_cast<ushort**>(sp);
	spi = reinterpret_cast<uint**>(sp);

	rfspb = rfsp;
	rfsps = reinterpret_cast<ushort**>(rfsp);
	rfspi = reinterpret_cast<uint**>(rfsp);

	lfspb = lfsp;
	lfsps = reinterpret_cast<ushort**>(lfsp);
	lfspi = reinterpret_cast<uint**>(lfsp);

	goto INST_SWITCH;

PROGRAMQUIT:
	icbindex_cxt = (n + 1) % icbarr.size();
	return;

CAST_SWITCH:
	switch (castv) {
	case casttype::CAST_BYTE_TO_SHORT:
		casted_value = (short)*reinterpret_cast<char*>(&casting_value);
		break;
	case casttype::CAST_BYTE_TO_USHORT:
		casted_value = (ushort) * reinterpret_cast<char*>(&casting_value);
		break;
	case casttype::CAST_BYTE_TO_INT:
		casted_value = (int)*reinterpret_cast<char*>(&casting_value);
		break;
	case casttype::CAST_BYTE_TO_UINT:
		casted_value = (uint) * reinterpret_cast<char*>(&casting_value);
		break;
	case casttype::CAST_BYTE_TO_FLOAT:
	{
		float fmem = (char)casting_value;
		*reinterpret_cast<float*>(&casted_value) = fmem;
	}
	break;
	case casttype::CAST_UBYTE_TO_FLOAT:
	{
		float fmem = (unsigned char)casting_value;
		*reinterpret_cast<float*>(&casted_value) = fmem;
	}
	break;
	case casttype::CAST_SHORT_TO_BYTE:
		casted_value = (char)*reinterpret_cast<short*>(&casting_value);
		break;
	case casttype::CAST_SHORT_TO_INT:
		casted_value = (int)*reinterpret_cast<short*>(&casting_value);
		break;
	case casttype::CAST_SHORT_TO_FLOAT:
	{
		float fmem = (short)casting_value;
		*reinterpret_cast<float*>(&casted_value) = fmem;
	}
	break;
	case casttype::CAST_USHORT_TO_FLOAT:
	{
		float fmem = (ushort)casting_value;
		*reinterpret_cast<float*>(&casted_value) = fmem;
	}
	break;
	case casttype::CAST_INT_TO_BYTE:
		casted_value = (char)*reinterpret_cast<int*>(&casting_value);
		break;
	case casttype::CAST_INT_TO_SHORT:
		casted_value = (short)*reinterpret_cast<int*>(&casting_value);
		break;
	case casttype::CAST_INT_TO_FLOAT:
	{
		float fmem = (int)casting_value;
		*reinterpret_cast<float*>(&casted_value) = fmem;
	}
	break;
	case casttype::CAST_UINT_TO_FLOAT:
	{
		float fmem = (unsigned int)casting_value;
		*reinterpret_cast<float*>(&casted_value) = fmem;
	}
	break;
	case casttype::CAST_FLOAT_TO_BYTE:
		casted_value = (char)*reinterpret_cast<float*>(&casting_value);
		break;
	case casttype::CAST_FLOAT_TO_UBYTE:
		casted_value = (byte8) * reinterpret_cast<float*>(&casting_value);
		break;
	case casttype::CAST_FLOAT_TO_SHORT:
		casted_value = (short)*reinterpret_cast<float*>(&casting_value);
		break;
	case casttype::CAST_FLOAT_TO_USHORT:
		casted_value = (ushort) * reinterpret_cast<float*>(&casting_value);
		break;
	case casttype::CAST_FLOAT_TO_INT:
		casted_value = (int)*reinterpret_cast<float*>(&casting_value);
		break;
	case casttype::CAST_FLOAT_TO_UINT:
		casted_value = (uint) * reinterpret_cast<float*>(&casting_value);
		break;
	}

	switch (selectRegister) {
	case 0:
		registerA0 = casted_value;
		++*pc;
		goto INST_SWITCH;
	case 1:
		registerB0 = casted_value;
		++*pc;
		goto INST_SWITCH;
	}

	goto INST_SWITCH;

DBG_SWITCH:
	switch (dbg_type) {
	case dbgtype::DBG_A_BYTE:
	{
		char c = (char)registerA0;
		InsideCode_Bake::icl << c;
	}
		//printf("%c", (char)registerA0);
		break;
	case dbgtype::DBG_A_UBYTE:
	{
		unsigned char c = (unsigned char)registerA0;
		InsideCode_Bake::icl << (int)c;
	}
		//printf("%d", (byte8)registerA0);
		break;
	case dbgtype::DBG_A_SHORT:
	{
		short c = (short)registerA0;
		InsideCode_Bake::icl << (int)c;
	}
		//printf("%d", (short)registerA0);
		break;
	case dbgtype::DBG_A_USHORT:
	{
		unsigned short c = (unsigned short)registerA0;
		InsideCode_Bake::icl << (int)c;
	}
		//printf("%d", (ushort)registerA0);
		break;
	case dbgtype::DBG_A_INT:
	{
		int c = (int)registerA0;
		InsideCode_Bake::icl << c;
	}
		//printf("%d", (int)registerA0);
		break;
	case dbgtype::DBG_A_UINT:
	{
		unsigned int c = (unsigned int)registerA0;
		InsideCode_Bake::icl << c;
	}
		//printf("%d", (uint)registerA0);
		break;
	case dbgtype::DBG_A_FLOAT:
		*reinterpret_cast<uint*>(&fmem) = (uint)registerA0;
		InsideCode_Bake::icl << fmem;
		//printf("%lf", fmem);
		break;
	case dbgtype::DBG_A_BOOL: 
	{
		InsideCode_Bake::icl << ((bool)registerA0 ? "true" : "false");
	}
		//printf((bool)registerA0 ? "true" : "false");
		break;
	case dbgtype::DBG_A_STRING:
	{
		char* c = reinterpret_cast<char*>(mem + (unsigned long long)registerA0);
		InsideCode_Bake::icl << c;
	}
		//printf("%s", reinterpret_cast<char*>(mem + (unsigned long long)registerA0));
		break;
	}

	++ * pc;
	goto INST_SWITCH;

INP_SWITCH:
	switch (inp_type) {
	case inptype::INP_BYTE:
		scanf_s("%c", reinterpret_cast<char*>(mem + registerA0));
		break;
	case inptype::INP_UBYTE:
	{
		unsigned int in;
		scanf_s("%u", &in);
		*reinterpret_cast<byte8*>(mem + registerA0) = (byte8)in;
		break;
	}
	case inptype::INP_SHORT:
	{
		int in;
		scanf_s("%d", &in);
		*reinterpret_cast<short*>(mem + registerA0) = (short)in;
		break;
	}
	case inptype::INP_USHORT:
	{
		unsigned int in;
		scanf_s("%u", &in);
		*reinterpret_cast<ushort*>(mem + registerA0) = (ushort)in;
		break;
	}
	case inptype::INP_INT:
		scanf_s("%d", reinterpret_cast<int*>(mem + registerA0));
		break;
	case inptype::INP_UINT:
		scanf_s("%u", reinterpret_cast<uint*>(mem + registerA0));
		break;
	case inptype::INP_FLOAT:
		scanf_s("%f", reinterpret_cast<float*>(mem + registerA0));
		break;
	case inptype::INP_BOOL:
	{
		char str[8] = {};
		scanf_s("%s", str);
		if (strcmp(str, "true"))
		{
			*reinterpret_cast<bool*>(mem + registerA0) = true;
		}
		else
		{
			*reinterpret_cast<bool*>(mem + registerA0) = false;
		}
		break;
	}
	case inptype::INP_STRING:
		scanf_s("%s", reinterpret_cast<char*>(mem + registerA0));
		break;
	}

	++ * pc;
	goto INST_SWITCH;

INST_SWITCH:
	/*
	icb->apivot = apivot;
	icb->bpivot = bpivot;
	int k = control_func(&icbarr);
	switch (k)
	{
	case 0:
		goto PROGRAMQUIT;
		break;
	case 1:
		break;
	}
	*/

	if (exed_num >= execodenum || icb->isBreaking)
	{
		icb->_la = _la;
		icb->apivot = apivot;
		icb->bpivot = bpivot;
		++n;
		goto CONTEXT_SWITCH;
	}

	if (icb->isDbg && ((int)(icb->pc - codemem) == icb->stopnum && (icb->call_stack.size() == icb->stop_callstack || icb->stop_callstack <= 0))) {
		icb->isBreaking = true;
		icb->_la = _la;
		icb->apivot = apivot;
		icb->bpivot = bpivot;
		++n;
		goto CONTEXT_SWITCH;
		//cout << "Debug BreakPoint Check!" << endl;
	}

	++exed_num;

	insttype instn = (insttype) * *pc;

	switch (instn)
	{
	case insttype::IT_ADD_STACK_VARIABLE:
		++*pc;
		*sp = (*rfsp - **pci);
		++*pci;
		goto INST_SWITCH;
	case insttype::IT_SET_STACK_VARIABLE_CONST_1:
		++*pc;
		tmptr_b = (*rfsp - **pci);
		++*pci;
		*tmptr_b = **pcb;
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_SET_STACK_VARIABLE_CONST_2:
		++*pc;
		tmptr_s = reinterpret_cast<ushort*>(*rfsp - (**pci));
		++*pci;
		*tmptr_s = **pcs;
		++*pcs;
		goto INST_SWITCH;
	case insttype::IT_SET_STACK_VARIABLE_CONST_4:
		++*pc;
		tmptr_i = reinterpret_cast<uint*>(*rfsp - (**pci));
		++*pci;
		*tmptr_i = **pci;
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_PUSH_A_CONST_1:
		shiftA0(-1);
		registerA0 = *++ * pc;
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_PUSH_A_CONST_2:
		++*pc;
		shiftA0(-1);
		registerA0 = **pcs;
		++*pcs;
		goto INST_SWITCH;
	case insttype::IT_PUSH_A_CONST_4:
		++*pc;
		shiftA0(-1);
		registerA0 = **pci;
		++*pci;
		goto INST_SWITCH;
	case insttype::IT_PUSH_A_VARIABLE_1:
		++*pc;
		shiftA0(-1);
		registerA0 = *(*rfsp - (**pci));
		++*pci;
		goto INST_SWITCH;
	case insttype::IT_PUSH_A_VARIABLE_2:
		++*pc;
		shiftA0(-1);
		registerA0 = *reinterpret_cast<ushort*>(*rfsp - (**pci));
		++*pci;
		goto INST_SWITCH;
	case insttype::IT_PUSH_A_VARIABLE_4:
		++*pc;
		shiftA0(-1);
		registerA0 = *reinterpret_cast<uint*>(*rfsp - (**pci));
		++*pci;
		goto INST_SWITCH;
	case insttype::IT_PUSH_B_CONST_1:
		shiftB0(-1);
		registerB0 = *++ * pc;
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_PUSH_B_CONST_2:
		++*pc;
		shiftB0(-1);
		registerB0 = **pcs;
		++*pcs;
		goto INST_SWITCH;
	case insttype::IT_PUSH_B_CONST_4:
		++*pc;
		shiftB0(-1);
		registerB0 = **pci;
		++*pci;
		goto INST_SWITCH;
	case insttype::IT_PUSH_B_VARIABLE_1:
		++*pc;
		shiftB0(-1);
		registerB0 = *(*rfsp - (**pci));
		++*pci;
		goto INST_SWITCH;
	case insttype::IT_PUSH_B_VARIABLE_2:
		++*pc;
		shiftB0(-1);
		registerB0 = *reinterpret_cast<ushort*>(*rfsp - (**pci));
		++*pci;
		goto INST_SWITCH;
	case insttype::IT_PUSH_B_VARIABLE_4:
		++*pc;
		shiftB0(-1);
		registerB0 = *reinterpret_cast<uint*>(*rfsp - (**pci));
		++*pci;
		goto INST_SWITCH;
	case insttype::IT_AU_BYTE_ADD_A:
		//shiftA0(-1);
		registerA0 = (char)((char)registerA0 + (char)registerB0);
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_AU_BYTE_ADD_B:
		_b = (char)((char)registerA0 + (char)registerB0);
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_AU_UBYTE_ADD_A:
		//shiftA0(-1);
		registerA0 = (byte8)((byte8)registerA0 + (byte8)registerB0);
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_AU_UBYTE_ADD_B:
		//shiftB0(-1);
		registerB0 = (byte8)((byte8)registerA0 + (byte8)registerB0);
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_AU_SHORT_ADD_A:
		//shiftA0(-1);
		registerA0 = (short)((short)registerA0 + (short)registerB0);
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_AU_SHORT_ADD_B:
		//shiftB0(-1);
		registerB0 = (short)((short)registerA0 + (short)registerB0);
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_AU_USHORT_ADD_A:
		//shiftA0(-1);
		registerA0 = (ushort)((ushort)registerA0 + (ushort)registerB0);
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_AU_USHORT_ADD_B:
		//shiftB0(-1);
		registerB0 = (ushort)((ushort)registerA0 + (ushort)registerB0);
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_AU_INT_ADD_A:
		//shiftA0(-1);
		registerA0 = (int)((int)registerA0 + (int)registerB0);
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_AU_INT_ADD_B:
		//shiftB0(-1);
		registerB0 = (int)((int)registerA0 + (int)registerB0);
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_AU_UINT_ADD_A:
		//shiftA0(-1);
		registerA0 = (uint)((uint)registerA0 + (uint)registerB0);
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_AU_UINT_ADD_B:
		//shiftB0(-1);
		registerB0 = (uint)((uint)registerA0 + (uint)registerB0);
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_AU_FLOAT_ADD_A:
		//shiftA0(-1);
		*reinterpret_cast<float*>(&registerA0) = (float)(*reinterpret_cast<float*>(&registerA0) + *reinterpret_cast<float*>(&registerB0));
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_AU_FLOAT_ADD_B:
		//shiftB0(-1);
		*reinterpret_cast<float*>(&registerB0) = (float)(*reinterpret_cast<float*>(&registerA0) + *reinterpret_cast<float*>(&registerB0));
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_AU_BYTE_MIN_A:
		//shiftA0(-1);
		registerA0 = (char)((char)registerA0 - (char)registerB0);
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_AU_BYTE_MIN_B:
		//shiftB0(-1);
		registerB0 = (char)((char)registerA0 - (char)registerB0);
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_AU_UBYTE_MIN_A:
		//shiftA0(-1);
		registerA0 = (byte8)((byte8)registerA0 - (byte8)registerB0);
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_AU_UBYTE_MIN_B:
		//shiftB0(-1);
		registerB0 = (byte8)((byte8)registerA0 - (byte8)registerB0);
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_AU_SHORT_MIN_A:
		//shiftA0(-1);
		registerA0 = (short)((short)registerA0 - (short)registerB0);
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_AU_SHORT_MIN_B:
		//shiftB0(-1);
		registerB0 = (short)((short)registerA0 - (short)registerB0);
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_AU_USHORT_MIN_A:
		//shiftA0(-1);
		registerA0 = (ushort)((ushort)registerA0 - (ushort)registerB0);
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_AU_USHORT_MIN_B:
		//shiftB0(-1);
		registerB0 = (ushort)((ushort)registerA0 - (ushort)registerB0);
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_AU_INT_MIN_A:
		//shiftA0(-1);
		registerA0 = (int)((int)registerA0 - (int)registerB0);
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_AU_INT_MIN_B:
		//shiftB0(-1);
		registerB0 = (int)((int)registerA0 - (int)registerB0);
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_AU_UINT_MIN_A:
		//shiftA0(-1);
		registerA0 = (uint)((uint)registerA0 - (uint)registerB0);
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_AU_UINT_MIN_B:
		//shiftB0(-1);
		registerB0 = (uint)((uint)registerA0 - (uint)registerB0);
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_AU_FLOAT_MIN_A:
		//shiftA0(-1);
		*reinterpret_cast<float*>(&registerA0) = (float)(*reinterpret_cast<float*>(&registerA0) - *reinterpret_cast<float*>(&registerB0));
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_AU_FLOAT_MIN_B:
		//shiftB0(-1);
		*reinterpret_cast<float*>(&registerB0) = (float)(*reinterpret_cast<float*>(&registerA0) - *reinterpret_cast<float*>(&registerB0));
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_AU_BYTE_MUL_A:
		//shiftA0(-1);
		registerA0 = (char)((char)registerA0 * (char)registerB0);
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_AU_BYTE_MUL_B:
		//shiftB0(-1);
		registerB0 = (char)((char)registerA0 * (char)registerB0);
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_AU_UBYTE_MUL_A:
		//shiftA0(-1);
		registerA0 = (char)((byte8)registerA0 * (byte8)registerB0);
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_AU_UBYTE_MUL_B:
		//shiftB0(-1);
		registerB0 = (char)((byte8)registerA0 * (byte8)registerB0);
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_AU_SHORT_MUL_A:
		//shiftA0(-1);
		registerA0 = (short)((short)registerA0 * (short)registerB0);
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_AU_SHORT_MUL_B:
		//shiftB0(-1);
		registerB0 = (short)((short)registerA0 * (short)registerB0);
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_AU_USHORT_MUL_A:
		//shiftA0(-1);
		registerA0 = (ushort)((ushort)registerA0 * (ushort)registerB0);
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_AU_USHORT_MUL_B:
		//shiftB0(-1);
		registerB0 = (ushort)((ushort)registerA0 * (ushort)registerB0);
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_AU_INT_MUL_A:
		//shiftA0(-1);
		registerA0 = (int)((int)registerA0 * (int)registerB0);
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_AU_INT_MUL_B:
		//shiftB0(-1);
		registerB0 = (int)((int)registerA0 * (int)registerB0);
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_AU_UINT_MUL_A:
		//shiftA0(-1);
		registerA0 = (uint)((uint)registerA0 * (uint)registerB0);
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_AU_UINT_MUL_B:
		//shiftB0(-1);
		registerB0 = (uint)((uint)registerA0 * (uint)registerB0);
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_AU_FLOAT_MUL_A:
		//shiftA0(-1);
		*reinterpret_cast<float*>(&registerA0) = (float)((*reinterpret_cast<float*>(&registerA0)) * (*reinterpret_cast<float*>(&registerB0)));
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_AU_FLOAT_MUL_B:
		//shiftB0(-1);
		*reinterpret_cast<float*>(&registerB0) = (float)((*reinterpret_cast<float*>(&registerA0)) * (*reinterpret_cast<float*>(&registerB0)));
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_AU_BYTE_DIV_A:
		//shiftA0(-1);
		registerA0 = (char)((char)registerA0 / (char)registerB0);
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_AU_BYTE_DIV_B:
		//shiftB0(-1);
		registerB0 = (char)((char)registerA0 / (char)registerB0);
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_AU_UBYTE_DIV_A:
		//shiftA0(-1);
		registerA0 = (byte8)((byte8)registerA0 / (byte8)registerB0);
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_AU_UBYTE_DIV_B:
		//shiftB0(-1);
		registerB0 = (byte8)((byte8)registerA0 / (byte8)registerB0);
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_AU_SHORT_DIV_A:
		//shiftA0(-1);
		registerA0 = (short)((short)registerA0 / (short)registerB0);
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_AU_SHORT_DIV_B:
		//shiftB0(-1);
		registerB0 = (short)((short)registerA0 / (short)registerB0);
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_AU_USHORT_DIV_A:
		//shiftA0(-1);
		registerA0 = (ushort)((ushort)registerA0 / (ushort)registerB0);
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_AU_USHORT_DIV_B:
		//shiftB0(-1);
		registerB0 = (ushort)((ushort)registerA0 / (ushort)registerB0);
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_AU_INT_DIV_A:
		//shiftA0(-1);
		registerA0 = (int)((int)registerA0 / (int)registerB0);
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_AU_INT_DIV_B:
		//shiftB0(-1);
		registerB0 = (int)((int)registerA0 / (int)registerB0);
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_AU_UINT_DIV_A:
		//shiftA0(-1);
		registerA0 = (uint)((uint)registerA0 / (uint)registerB0);
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_AU_UINT_DIV_B:
		//shiftB0(-1);
		registerB0 = (uint)((uint)registerA0 / (uint)registerB0);
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_AU_FLOAT_DIV_A:
		//shiftA0(-1);
		*reinterpret_cast<float*>(&registerA0) = (float)(*reinterpret_cast<float*>(&registerA0) / *reinterpret_cast<float*>(&registerB0));
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_AU_FLOAT_DIV_B:
		//shiftB0(-1);
		*reinterpret_cast<float*>(&registerB0) = (float)(*reinterpret_cast<float*>(&registerA0) / *reinterpret_cast<float*>(&registerB0));
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_AU_BYTE_PER_A:
		//shiftA0(-1);
		registerA0 = (char)((char)registerA0 % (char)registerB0);
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_AU_BYTE_PER_B:
		//shiftB0(-1);
		registerB0 = (char)((char)registerA0 % (char)registerB0);
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_AU_UBYTE_PER_A:
		//shiftA0(-1);
		registerA0 = (char)((byte8)registerA0 % (byte8)registerB0);
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_AU_UBYTE_PER_B:
		//shiftB0(-1);
		registerB0 = (char)((byte8)registerA0 % (byte8)registerB0);
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_AU_SHORT_PER_A:
		//shiftA0(-1);
		registerA0 = (char)((short)registerA0 % (short)registerB0);
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_AU_SHORT_PER_B:
		//shiftB0(-1);
		registerB0 = (short)((short)registerA0 % (short)registerB0);
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_AU_USHORT_PER_A:
		//shiftA0(-1);
		registerA0 = (ushort)((ushort)registerA0 % (ushort)registerB0);
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_AU_USHORT_PER_B:
		//shiftB0(-1);
		registerB0 = (ushort)((ushort)registerA0 % (ushort)registerB0);
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_AU_INT_PER_A:
		//shiftA0(-1);
		registerA0 = (int)((int)registerA0 % (int)registerB0);
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_AU_INT_PER_B:
		//shiftB0(-1);
		registerB0 = (int)((int)registerA0 % (int)registerB0);
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_AU_UINT_PER_A:
		//shiftA0(-1);
		registerA0 = (uint)((uint)registerA0 % (uint)registerB0);
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_AU_UINT_PER_B:
		//shiftB0(-1);
		registerB0 = (uint)((uint)registerA0 % (uint)registerB0);
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_LU_BOOL_AND_A:
		//shiftA0(-1);
		registerA0 = (bool)((bool)registerA0 && (bool)registerB0);
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_LU_BOOL_AND_B:
		//shiftB0(-1);
		registerB0 = (bool)((bool)registerA0 && (bool)registerB0);
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_LU_BOOL_OR_A:
		//shiftA0(-1);
		registerA0 = (bool)((bool)registerA0 || (bool)registerB0);
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_LU_BOOL_OR_B:
		//shiftB0(-1);
		registerB0 = (bool)((bool)registerA0 || (bool)registerB0);
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_LU_BOOL_NOT_A:
		//shiftA0(-1);
		registerA0 = (bool)(!(bool)registerA0);
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_LU_BOOL_NOT_B:
		//shiftB0(-1);
		registerB0 = (bool)(!(bool)registerA0);
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_CM_BOOL_SAME_A:
		//shiftA0(-1);
		registerA0 = registerA0 == registerB0;
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_CM_BOOL_SAME_B:
		//shiftB0(-1);
		registerB0 = registerA0 == registerB0;
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_CM_BOOL_NOTSAME_A:
		//shiftA0(-1);
		registerA0 = registerA0 != registerB0;
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_CM_BOOL_NOTSAME_B:
		//shiftB0(-1);
		registerB0 = registerA0 != registerB0;
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_CM_BOOL_BYTE_LBIG_A:
		//shiftA0(-1);
		registerA0 = (char)registerA0 > (char)registerB0;
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_CM_BOOL_BYTE_LBIG_B:
		//shiftB0(-1);
		registerB0 = (char)registerA0 > (char)registerB0;
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_CM_BOOL_UBYTE_LBIG_A:
		//shiftA0(-1);
		registerA0 = (byte8)registerA0 > (byte8)registerB0;
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_CM_BOOL_UBYTE_LBIG_B:
		//shiftB0(-1);
		registerB0 = (byte8)registerA0 > (byte8)registerB0;
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_CM_BOOL_SHORT_LBIG_A:
		//shiftA0(-1);
		registerA0 = (short)registerA0 > (short)registerB0;
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_CM_BOOL_SHORT_LBIG_B:
		//shiftB0(-1);
		registerB0 = (short)registerA0 > (short)registerB0;
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_CM_BOOL_USHORT_LBIG_A:
		//shiftA0(-1);
		registerA0 = (ushort)registerA0 > (ushort)registerB0;
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_CM_BOOL_USHORT_LBIG_B:
		//shiftB0(-1);
		registerB0 = (ushort)registerA0 > (ushort)registerB0;
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_CM_BOOL_INT_LBIG_A:
		//shiftA0(-1);
		registerA0 = (int)registerA0 > (int)registerB0;
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_CM_BOOL_INT_LBIG_B:
		//shiftB0(-1);
		registerB0 = (int)registerA0 > (int)registerB0;
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_CM_BOOL_UINT_LBIG_A:
		//shiftA0(-1);
		registerA0 = (uint)registerA0 > (uint)registerB0;
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_CM_BOOL_UINT_LBIG_B:
		//shiftB0(-1);
		registerB0 = (uint)registerA0 > (uint)registerB0;
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_CM_BOOL_FLOAT_LBIG_A:
		//shiftA0(-1);
		registerA0 = *reinterpret_cast<float*>(&registerA0) > *reinterpret_cast<float*>(&registerB0);
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_CM_BOOL_FLOAT_LBIG_B:
		//shiftB0(-1);
		registerB0 = *reinterpret_cast<float*>(&registerA0) > *reinterpret_cast<float*>(&registerB0);
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_CM_BOOL_BYTE_LBIGSAME_A:
		//shiftA0(-1);
		registerA0 = (char)registerA0 >= (char)registerB0;
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_CM_BOOL_BYTE_LBIGSAME_B:
		//shiftB0(-1);
		registerB0 = (char)registerA0 >= (char)registerB0;
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_CM_BOOL_UBYTE_LBIGSAME_A:
		//shiftA0(-1);
		registerA0 = (byte8)registerA0 >= (byte8)registerB0;
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_CM_BOOL_UBYTE_LBIGSAME_B:
		//shiftB0(-1);
		registerB0 = (byte8)registerA0 >= (byte8)registerB0;
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_CM_BOOL_SHORT_LBIGSAME_A:
		//shiftA0(-1);
		registerA0 = (short)registerA0 >= (short)registerB0;
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_CM_BOOL_SHORT_LBIGSAME_B:
		//shiftB0(-1);
		registerB0 = (short)registerA0 >= (short)registerB0;
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_CM_BOOL_USHORT_LBIGSAME_A:
		//shiftA0(-1);
		registerA0 = (ushort)registerA0 >= (ushort)registerB0;
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_CM_BOOL_USHORT_LBIGSAME_B:
		//shiftB0(-1);
		registerB0 = (ushort)registerA0 >= (ushort)registerB0;
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_CM_BOOL_INT_LBIGSAME_A:
		//shiftA0(-1);
		registerA0 = (int)registerA0 >= (int)registerB0;
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_CM_BOOL_INT_LBIGSAME_B:
		//shiftB0(-1);
		registerB0 = (int)registerA0 >= (int)registerB0;
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_CM_BOOL_UINT_LBIGSAME_A:
		//shiftA0(-1);
		registerA0 = (uint)registerA0 >= (uint)registerB0;
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_CM_BOOL_UINT_LBIGSAME_B:
		//shiftB0(-1);
		registerB0 = (uint)registerA0 >= (uint)registerB0;
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_CM_BOOL_FLOAT_LBIGSAME_A:
		//shiftA0(-1);
		registerA0 = *reinterpret_cast<float*>(&registerA0) >= *reinterpret_cast<float*>(&registerB0);
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_CM_BOOL_FLOAT_LBIGSAME_B:
		//shiftB0(-1);
		registerB0 = *reinterpret_cast<float*>(&registerA0) >= *reinterpret_cast<float*>(&registerB0);
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_CM_BOOL_BYTE_RBIG_A:
		//shiftA0(-1);
		registerA0 = (char)registerA0 < (char)registerB0;
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_CM_BOOL_BYTE_RBIG_B:
		//shiftB0(-1);
		registerB0 = (char)registerA0 < (char)registerB0;
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_CM_BOOL_UBYTE_RBIG_A:
		//shiftA0(-1);
		registerA0 = (byte8)registerA0 < (byte8)registerB0;
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_CM_BOOL_UBYTE_RBIG_B:
		//shiftA0(-1);
		registerA0 = (byte8)registerA0 < (byte8)registerB0;
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_CM_BOOL_SHORT_RBIG_A:
		//shiftA0(-1);
		registerA0 = (short)registerA0 < (short)registerB0;
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_CM_BOOL_SHORT_RBIG_B:
		//shiftB0(-1);
		registerB0 = (short)registerA0 < (short)registerB0;
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_CM_BOOL_USHORT_RBIG_A:
		//shiftA0(-1);
		registerA0 = (ushort)registerA0 < (ushort)registerB0;
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_CM_BOOL_USHORT_RBIG_B:
		//shiftB0(-1);
		registerB0 = (ushort)registerA0 < (ushort)registerB0;
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_CM_BOOL_INT_RBIG_A:
		//shiftA0(-1);
		registerA0 = (int)registerA0 < (int)registerB0;
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_CM_BOOL_INT_RBIG_B:
		//shiftB0(-1);
		registerB0 = (int)registerA0 < (int)registerB0;
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_CM_BOOL_UINT_RBIG_A:
		//shiftA0(-1);
		registerA0 = (uint)registerA0 < (uint)registerB0;
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_CM_BOOL_UINT_RBIG_B:
		//shiftB0(-1);
		registerB0 = (uint)registerA0 < (uint)registerB0;
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_CM_BOOL_FLOAT_RBIG_A:
		//shiftA0(-1);
		registerA0 = *reinterpret_cast<float*>(&registerA0) < *reinterpret_cast<float*>(&registerB0);
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_CM_BOOL_FLOAT_RBIG_B:
		//shiftB0(-1);
		registerB0 = *reinterpret_cast<float*>(&registerA0) < *reinterpret_cast<float*>(&registerB0);
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_CM_BOOL_BYTE_RBIGSAME_A:
		//shiftA0(-1);
		registerA0 = (char)registerA0 <= (char)registerB0;
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_CM_BOOL_BYTE_RBIGSAME_B:
		//shiftB0(-1);
		registerB0 = (char)registerA0 <= (char)registerB0;
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_CM_BOOL_UBYTE_RBIGSAME_A:
		//shiftA0(-1);
		registerA0 = (byte8)registerA0 <= (byte8)registerB0;
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_CM_BOOL_UBYTE_RBIGSAME_B:
		//shiftB0(-1);
		registerB0 = (byte8)registerA0 <= (byte8)registerB0;
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_CM_BOOL_SHORT_RBIGSAME_A:
		//shiftA0(-1);
		registerA0 = (short)registerA0 <= (short)registerB0;
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_CM_BOOL_SHORT_RBIGSAME_B:
		//shiftB0(-1);
		registerB0 = (short)registerA0 <= (short)registerB0;
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_CM_BOOL_USHORT_RBIGSAME_A:
		//shiftA0(-1);
		registerA0 = (ushort)registerA0 <= (ushort)registerB0;
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_CM_BOOL_USHORT_RBIGSAME_B:
		//shiftB0(-1);
		registerB0 = (ushort)registerA0 <= (ushort)registerB0;
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_CM_BOOL_INT_RBIGSAME_A:
		//shiftA0(-1);
		registerA0 = (int)registerA0 <= (int)registerB0;
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_CM_BOOL_INT_RBIGSAME_B:
		//shiftB0(-1);
		registerB0 = (int)registerA0 <= (int)registerB0;
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_CM_BOOL_UINT_RBIGSAME_A:
		//shiftA0(-1);
		registerA0 = (uint)registerA0 <= (uint)registerB0;
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_CM_BOOL_UINT_RBIGSAME_B:
		//shiftB0(-1);
		registerB0 = (uint)registerA0 <= (uint)registerB0;
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_CM_BOOL_FLOAT_RBIGSAME_A:
		//shiftA0(-1);
		registerA0 = *reinterpret_cast<float*>(registerA0) <= *reinterpret_cast<float*>(registerB0);
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_CM_BOOL_FLOAT_RBIGSAME_B:
		//shiftB0(-1);
		registerB0 = *reinterpret_cast<float*>(registerA0) <= *reinterpret_cast<float*>(registerB0);
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_IF:
	{
		++*pc;
		tmptr_i = *pci;
		++*pci;
		unsigned char a = registerA0;
		if (!(bool)a)
			*pc = &codemem[*tmptr_i];
		goto INST_SWITCH;
	}
	case insttype::IT_JMP:
		++*pc;
		tmptr_i = *pci;
		++*pci;
		*pc = &codemem[*tmptr_i];
		goto INST_SWITCH;
	case insttype::IT_FUNC:
		saveSP->push_back(*sp);
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_PARAM_1:
		--*sp;
		**sp = (byte8)registerA0;
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_PARAM_2:
		--*sps;
		**sps = (ushort)registerA0;
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_PARAM_4:
		--*spi;
		**spi = (uint)registerA0;
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_RETURN:
		*sp = *rfsp;
		*rfsp = *lfsp;
		fsp->pop_back();
		*lfsp = (*fsp)[fsp->size() - 2];
		*pc = call_stack->last();
		call_stack->pop_back();
		++*pc;
		if (call_stack->size() == 0) {
			++*pc;
			goto PROGRAMQUIT;
		}
		goto INST_SWITCH;
	case insttype::IT_EXIT:
		++*pc;
		goto PROGRAMQUIT;
	case insttype::IT_PUSH_TO_A_FROM_ADDRESS_OF_VARIABLE_ID:
		++*pc;
		shiftA0(-1);
		registerA0 = (uint64_t)((*rfsp - **pci) - mem);
		++*pci;
		goto INST_SWITCH;
	case insttype::IT_PUSH_TO_B_FROM_ADDRESS_OF_VARIABLE_ID:
		++*pc;
		shiftB0(-1);
		registerB0 = (uint64_t)((*rfsp - **pci) - mem);
		++*pci;
		goto INST_SWITCH;
	case insttype::IT_PUSH_TO_LA_FROM_A:
		_la = registerA0;
		shiftA0(1);
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_FUNCJMP:
		*lfsp = *rfsp;
		fsp->push_back(saveSP->last());
		saveSP->pop_back();
		*rfsp = fsp->last();
		++*pc;
		tmptr_i = *pci;
		++*pci;
		call_stack->push_back(*pc - 1);
		*pc = &codemem[*tmptr_i];
		goto INST_SWITCH;
	case insttype::IT_CASTING_A:
		selectRegister = 0;
		casting_value = registerA0;
		casted_value = 0;
		castv = (casttype) * ++ * pc;
		goto CAST_SWITCH;
	case insttype::IT_CASTING_B:
		selectRegister = 1;
		casting_value = registerB0;
		casted_value = 0;
		castv = (casttype) * ++ * pc;
		goto CAST_SWITCH;
	case insttype::IT_PUSH_A_FROM_VALUE_OF_A:
		registerA0 = *reinterpret_cast<uint64_t*>(mem + (uint64_t)registerA0);
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_PUSH_B_FROM_VALUE_OF_B:
		registerB0 = *reinterpret_cast<uint64_t*>(mem + (uint64_t)registerB0);
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_DBG_A:
		dbg_type = (dbgtype) * ++ * pc;
		goto DBG_SWITCH;
	case insttype::IT_INP_A_PTR:
		inp_type = (inptype) * ++ * pc;
		goto INP_SWITCH;
	case insttype::IT_PUSH_TO_VALUE_OF_LA_FROM_A_1:
		*reinterpret_cast<byte8*>(mem + (uint64_t)_la) = registerA0;
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_PUSH_TO_VALUE_OF_LA_FROM_A_2:
		*reinterpret_cast<ushort*>(mem + (uint64_t)_la) = registerA0;
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_PUSH_TO_VALUE_OF_LA_FROM_A_4:
		*reinterpret_cast<uint*>(mem + (uint64_t)_la) = registerA0;
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_SET_A_CONST_STRING:
		++*pc;
		strmax = *reinterpret_cast<uint*>(*pci);
		++*pci;
		shiftA0(-1);
		registerA0 = *pc - mem;
		*pc += strmax;
		goto INST_SWITCH;
	case insttype::IT_SET_B_CONST_STRING:
		++*pc;
		strmax = *reinterpret_cast<uint*>(*pci);
		++*pci;
		shiftB0(-1);
		registerB0 = *pc - mem;
		*pc += strmax;
		goto INST_SWITCH;
	case insttype::IT_POP_A:
		shiftA0(1);
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_POP_B:
		shiftB0(1);
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_POP_AB:
		shiftA0(1);
		shiftB0(1);
		++*pc;
		goto INST_SWITCH;
	case insttype::PARAM_N_COPY_BY_ADDRESS:
		++*pc;
		paramSiz = *reinterpret_cast<uint*>(*pci);
		*sp -= paramSiz;
		++*pci;
		for (int i = paramSiz-1; i >= 0; --i) {
			*(*sp + i) = *(mem + (uint64_t)registerA0 + i);
		}
		goto INST_SWITCH;
	case insttype::PUSH_A_GLOBAL_VARIABLE_ADDRESS:
		++*pc;
		shiftA0(-1);
		registerA0 = &(icb->datamem.at(**pci)) - mem;
		++*pci;
		goto INST_SWITCH;
	case insttype::PUSH_B_GLOBAL_VARIABLE_ADDRESS:
		++*pc;
		shiftB0(-1);
		registerB0 = &icb->datamem.at(**pci) - mem;
		++*pci;
		goto INST_SWITCH;
	case insttype::PUSH_A_FROM_B:
		++*pc;
		shiftA0(-1);
		registerA0 = registerB0;
		goto INST_SWITCH;
	case insttype::PUSH_B_FROM_A:
		++*pc;
		shiftB0(-1);
		registerB0 = registerA0;
		goto INST_SWITCH;
	case insttype::SET_ADDRESS_LA_FROM_ADRESS_A_N:
	{
		++*pc;
		byte8* bptr = reinterpret_cast<byte8*>(mem + (uint64_t)_la);
		byte8* aptr = reinterpret_cast<byte8*>(mem + (uint64_t)registerA0);
		uint ValueSiz = **pci;
		++*pci;
		for (uint k = 0; k < ValueSiz; ++k) {
			bptr[k] = aptr[k];
		}
	}
	goto INST_SWITCH;
	case insttype::IT_PUSH_A_CONST_INDEX_ADDR:
	{
		++*pc;
		shiftA0(-1);
		registerA0 = ((*rfsp - **pci) - mem);
		++*pci;
		registerA0 += **pci;
		++*pci;
	}
	goto INST_SWITCH;
	case insttype::IT_PUSH_B_CONST_INDEX_ADDR:
	{
		++*pc;
		shiftB0(-1);
		registerB0 = ((*rfsp - **pci) - mem);
		++*pci;
		registerB0 += **pci;
		++*pci;
	}
	goto INST_SWITCH;
	case insttype::PARAM_8:
		*sp -= 8;
		*reinterpret_cast<uint64_t*>(*sp) = registerA0;
		++*pc;
		goto INST_SWITCH;
	case insttype::IT_PUSH_TO_VALUE_OF_LA_FROM_A_8:
		*reinterpret_cast<uint64_t*>(mem + (uint64_t)_la) = registerA0;
		++*pc;
		goto INST_SWITCH;
	case insttype::EXTENSION_INST:
		icb->_la = _la;
		icb->apivot = apivot;
		icb->bpivot = bpivot;

		*lfsp = *rfsp;
		fsp->push_back(saveSP->last());
		saveSP->pop_back();
		*rfsp = fsp->last();

		++*pc;
		CurrentExtension = icb->icb->extension.at(**pci);
		++*pci;
		extContainer = reinterpret_cast<exInst>(CurrentExtension->exfuncArr.at(**pci)->start_pc);
		++*pci;

		call_stack->push_back(*pc - 1);
		extContainer(reinterpret_cast<int*>(icb));

		*sp = *rfsp;
		*rfsp = *lfsp;
		fsp->pop_back();
		*lfsp = (*fsp)[fsp->size() - 2];
		*pc = call_stack->last();
		call_stack->pop_back();
		++*pc;
		goto INST_SWITCH;
	}
}
#endif