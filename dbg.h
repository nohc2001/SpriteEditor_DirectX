#ifndef DBG_H
#define DBG_H
#include <fstream>
using namespace std;

ofstream dbg;

char callstack[128][64] = { {} };

int callup = 0;

int* pointer_follow[128] = { };

int pfup = 0;

int counting[128] = {};

#define dbgcount(N, A) if(counting[N] > 0) { A; counting[N]-=1; }

void pushf(const char* name)
{
	if (callup < 128)
	{
		int len = strlen(name);
		for (int i = 0; i < len; ++i)
		{
			callstack[callup][i] = name[i];
		}
		callstack[callup][len] = 0;
		callup += 1;

		for (int i = 0; i < callup; ++i)
		{
			dbg << " > " << (char*)callstack[i];
		}
		dbg << endl;
	}
}

void dbg_callstack()
{
	for (int i = 0; i < callup; ++i)
	{
		dbg << " > " << (char*)callstack[i];
	}
	dbg << endl;
}

void popf()
{
	callup -= 1;
}

void push_pf(int* pointer)
{
	if (pfup < 128)
	{
		pointer_follow[pfup] = pointer;
		pfup += 1;
	}
	dbg << pfup - 1 << "th pf : " << pointer << endl;
}

void clearpf()
{
	pfup = 0;
}

bool cond_call(char* str)
{
	for (int i = 0; i < callup; ++i)
	{
		if (strcmp(str, callstack[i]) == 0)
		{
			for (int i = 0; i < callup; ++i)
			{
				dbg << " > " << (char*)callstack[i];
			}
			dbg << endl;
			return true;
		}
	}
	return false;
}

//#define ISDEBUGING

#ifdef ISDEBUGING
#define func_in(A) pushf(A);
#define func_out popf();
#define watch(N, A) dbg << "value " << N << " : " << A << endl;
#define dbglook(S, A) if(cond_call(S)){dbg << A << endl;}
#endif

#ifndef ISDEBUGING
#define func_in(A)				// pushf(A);
#define func_out				// popf();
#define watch(N, A)				// A;
#define dbglook(S, A)			// if(cond_call(S)){dbg << A << endl};
#endif

#endif
