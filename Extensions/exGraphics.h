// version 0.1
#ifndef EXGRAPHICS_H
#define EXGRAPHICS_H

#include "ShapeObject.h"
#include "Sprite.h"
#include "hancom.h"
#include "ICB_Extension.h"
#include "exGeometry.h"
#include "time_rdtsc.h"
#include <math.h>

ICB_Extension *Init_exGraphics();

typedef InsideCode_Bake *pICB;

typedef ICB_Context *pECS;

struct prbuffer
{
	int *data;
};

struct gcolor
{
	float r;
	float g;
	float b;
	float a;
};

struct gpos
{
	float x;
	float y;
	float z;
};

struct gindex
{
	unsigned short v0;
	unsigned short v1;
	unsigned short v2;
};

struct mat4
{
	float x00;
	float x01;
	float x02;
	float x03;
	float x10;
	float x11;
	float x12;
	float x13;
	float x20;
	float x21;
	float x22;
	float x23;
	float x30;
	float x31;
	float x32;
	float x33;
};

typedef Sprite *pSprite;

typedef Object *pObject;

struct gHeapCheck {
	void* ptr = nullptr; // 힙 메모리 주소
	int shouldRelease = -1; // -1이면 유지, n이면 n번째 레이어가 해제될때 헤제됨.
	void (*ReleaseFunc)(void*); // 해제때 쓰이는 함수.
};

// pICB get_icb(char *filename);
void exGraphics_get_icb(int* pcontext)
{
	ICB_Context *icc = reinterpret_cast < ICB_Context * >(pcontext);
	char* filename = *reinterpret_cast < char**>(icc->rfsp - 8);
	
	InsideCode_Bake* icb = nullptr;
	if(icmap.find(filename) == icmap.end()){
		icb = (InsideCode_Bake*)fm->_New(sizeof(InsideCode_Bake), true);
		icb->init(40960);
    	ICB_Extension* ext = Init_exGeometry();
    	icb->extension.push_back(ext);
    	ext = Init_exGraphics();
    	icb->extension.push_back(ext);
		icb->bake_code(filename);
		
		int len = strlen(filename) + 1;
		char* nstr = (char*)fm->_New(sizeof(char)*len, true);
		strcpy(nstr, filename);
		icmap.insert(ICMAP::value_type(nstr, icb));
	}
	else{
		icb = icmap[filename];
	}
	
	icc->sp -= sizeof(InsideCode_Bake*);
	*reinterpret_cast < InsideCode_Bake** >(icc->sp) = icb;
	icc->getA(0) = icc->sp - icc->mem;
    icc->Amove_pivot(-1);
}

// pECS create_ecs(pICB icb, uint memsiz);
void exGraphics_create_ecs(int *pcontext)
{
	ICB_Context *icc = reinterpret_cast < ICB_Context * >(pcontext);
	InsideCode_Bake *icb = *reinterpret_cast < InsideCode_Bake ** >(icc->rfsp - 12);
	unsigned int memsiz = *reinterpret_cast < unsigned int *>(icc->rfsp - 4);

	ICB_Context *ecs = (ICB_Context *) fm->_New(sizeof(ICB_Context), true);
	ecs->SetICB(icb, memsiz);
	//ctx->Push_InheritData(8, (byte8*)o);

	icc->sp -= sizeof(ICB_Context *);
	*reinterpret_cast < ICB_Context ** >(icc->sp) = ecs;
	icc->getA(0) = icc->sp - icc->mem;
    icc->Amove_pivot(-1);
}

// pICB ecs_get_icb(pECS ecs);
void exGraphics_ecs_get_icb(int *pcontext)
{
	ICB_Context *icc = reinterpret_cast < ICB_Context * >(pcontext);
	ICB_Context *ecs = *reinterpret_cast < ICB_Context ** >(icc->rfsp - 8);

	icc->sp -= sizeof(InsideCode_Bake *);
	*reinterpret_cast < InsideCode_Bake ** >(icc->sp) = ecs->icb;
	icc->getA(0) = icc->sp - icc->mem;
    icc->Amove_pivot(-1);
}

// gpos apos(float x, float y, float z);
void exGraphics__gpos(int *pcontext)
{
	ICB_Context *icc = reinterpret_cast < ICB_Context * >(pcontext);
	gpos v;
	v = *reinterpret_cast < gpos * >(icc->rfsp - 12);

	icc->sp -= sizeof(gpos);
	*reinterpret_cast < gpos * >(icc->sp) = v;
	icc->getA(0) = icc->sp - icc->mem;
    icc->Amove_pivot(-1);
}

// gindex aindex(ushort n0, ushort n1, ushort n2);
void exGraphics__gindex(int *pcontext)
{
	ICB_Context *icc = reinterpret_cast < ICB_Context * >(pcontext);
	gindex v;
	v = *reinterpret_cast < gindex * >(icc->rfsp - 6);

	icc->sp -= sizeof(gindex);
	*reinterpret_cast < gindex * >(icc->sp) = v;
	icc->getA(0) = icc->sp - icc->mem;
    icc->Amove_pivot(-1);
}

// gpos _gcolor(uchar r, uchar g, uchar b, uchar a);
void exGraphics__gcolor(int* pcontext)
{
	ICB_Context* icc = reinterpret_cast <ICB_Context*>(pcontext);
	unsigned char* colorparam;
	colorparam = reinterpret_cast <unsigned char*>(icc->rfsp - 4);
	gcolor v;
	v.r = (float)colorparam[0] / 255.0f;
	v.g = (float)colorparam[1] / 255.0f;
	v.b = (float)colorparam[2] / 255.0f;
	v.a = (float)colorparam[3] / 255.0f;
	icc->sp -= sizeof(gcolor);
	*reinterpret_cast <gcolor*>(icc->sp) = v;
	icc->getA(0) = icc->sp - icc->mem;
	icc->Amove_pivot(-1);
}

// int rbuff_get_vertexsiz(prbuffer buff, int index);
void exGraphics_rbuff_get_vertexsiz(int *pcontext)
{
	ICB_Context *icc = reinterpret_cast < ICB_Context * >(pcontext);
	rbuffer *rb = *reinterpret_cast < rbuffer ** >(icc->rfsp - 12);
	int index = *reinterpret_cast < int *>(icc->rfsp - 4);
	int ret = rb->get_vertexsiz(index);

	icc->sp -= sizeof(int);
	*reinterpret_cast < int *>(icc->sp) = ret;
	icc->getA(0) = icc->sp - icc->mem;
    icc->Amove_pivot(-1);
}

// int rbuff_get_choice(prbuffer buff);
void exGraphics_rbuff_get_choice(int *pcontext)
{
	ICB_Context *icc = reinterpret_cast < ICB_Context * >(pcontext);
	rbuffer *rb = *reinterpret_cast < rbuffer ** >(icc->rfsp - 8);

	int choice = rb->get_choice();

	icc->sp -= sizeof(int);
	*reinterpret_cast < int *>(icc->sp) = choice;
	icc->getA(0) = icc->sp - icc->mem;
    icc->Amove_pivot(-1);
}

// bool rbuff_get_inherit(prbuffer buff);
void exGraphics_rbuff_get_inherit(int *pcontext)
{
	ICB_Context *icc = reinterpret_cast < ICB_Context * >(pcontext);
	rbuffer *rb = *reinterpret_cast < rbuffer ** >(icc->rfsp - 8);

	bool ret = rb->get_inherit();

	icc->sp -= sizeof(bool);
	*reinterpret_cast < bool * >(icc->sp) = ret;
	icc->getA(0) = icc->sp - icc->mem;
    icc->Amove_pivot(-1);
}

// void rbuff_set_choice(prbuffer buff, uint n);
void exGraphics_rbuff_set_choice(int *pcontext)
{
	ICB_Context *icc = reinterpret_cast < ICB_Context * >(pcontext);
	rbuffer *rb = *reinterpret_cast < rbuffer ** >(icc->rfsp - 12);
	unsigned int n = *reinterpret_cast < unsigned int *>(icc->rfsp - 4);
	rb->set_choice(n);
}

// void rbuff_set_inherit(prbuffer buff, bool b);
void exGraphics_rbuff_set_inherit(int *pcontext)
{
	ICB_Context *icc = reinterpret_cast < ICB_Context * >(pcontext);
	rbuffer *rb = *reinterpret_cast < rbuffer ** >(icc->rfsp - 9);
	bool b = *reinterpret_cast < bool * >(icc->rfsp - 1);
	rb->set_inherit(b);
}

// prbuffer _prbuffer(bool local);
void exGraphics__prbuffer(int *pcontext)
{
	ICB_Context *icc = reinterpret_cast < ICB_Context * >(pcontext);
	bool b = *reinterpret_cast < bool * >(icc->rfsp - 1);
	rbuffer* rb = (rbuffer*)fm->_New(sizeof(rbuffer), true);
	rb->Init(false);

	icc->sp -= sizeof(int*);
	*reinterpret_cast <rbuffer**>(icc->sp) = rb;
	icc->getA(0) = icc->sp - icc->mem;
	icc->Amove_pivot(-1);
}

// void begin(prbuffer buff, bool inherit = false);
void exGraphics_begin(int *pcontext)
{
	ICB_Context *icc = reinterpret_cast < ICB_Context * >(pcontext);
	rbuffer *rb = *reinterpret_cast < rbuffer ** >(icc->rfsp - 9);
	bool b = *reinterpret_cast < bool * >(icc->rfsp - 1);
	rb->begin();
	//rb->begin(rendertype::color_nouv, b);
}

// void end(prbuffer buff);
void exGraphics_end(int *pcontext)
{
	ICB_Context *icc = reinterpret_cast < ICB_Context * >(pcontext);
	rbuffer *rb = *reinterpret_cast < rbuffer ** >(icc->rfsp - 8);
	rb->end();
}

// void av(prbuffer buff, sdlpos pos, gcolor color);
void exGraphics_av(int *pcontext)
{
	ICB_Context *icc = reinterpret_cast < ICB_Context * >(pcontext);
	rbuffer *rb = *reinterpret_cast < rbuffer ** >(icc->rfsp - 36);
	shp::vec3f pos = *reinterpret_cast <shp::vec3f* >(icc->rfsp - 28);
	DX11Color col = *reinterpret_cast <DX11Color* >(icc->rfsp - 16);
	rb->av(SimpleVertex(pos, col));
}

// void rbuff_set_static_color(prbuffer buff, gcolor color);
void exGraphics_rbuff_set_static_color(int *pcontext)
{
	ICB_Context *icc = reinterpret_cast < ICB_Context * >(pcontext);
	rbuffer *rb = *reinterpret_cast < rbuffer ** >(icc->rfsp - 24);
	DX11Color col = *reinterpret_cast <DX11Color* >(icc->rfsp - 16);
	//rb->set_static_color(col);
}

// void rbuff_clear(prbuffer buff);
void exGraphics_rbuff_clear(int *pcontext)
{
	ICB_Context *icc = reinterpret_cast < ICB_Context * >(pcontext);
	rbuffer *rb = *reinterpret_cast < rbuffer ** >(icc->rfsp - 8);
	rb->clear();
}

// void spr_render(pSprite spr, mat4 pmat);
void exGraphics_spr_render(int *pcontext)
{
	ICB_Context *icc = reinterpret_cast < ICB_Context * >(pcontext);
	Sprite *spr = *reinterpret_cast < Sprite ** >(icc->rfsp - 70);
	//sdlmat4 mat = *reinterpret_cast < sdlmat4 * >(icc->rfsp - 64);
	//spr->render(mat);
}

// void spr_load(pSprite spr, char *filename);
void exGraphics_spr_load(int *pcontext)
{
	ICB_Context *icc = reinterpret_cast < ICB_Context * >(pcontext);
	Sprite *spr = *reinterpret_cast < Sprite ** >(icc->rfsp - 16);
	char *filename = *reinterpret_cast < char **>(icc->rfsp - 8);
	//spr->load((wchar_t*)utf8_to_wstr(filename).c_str());
}

// void spr_save(pSprite spr, char *filename);
void exGraphics_spr_save(int *pcontext)
{
	ICB_Context *icc = reinterpret_cast < ICB_Context * >(pcontext);
	Sprite *spr = *reinterpret_cast < Sprite ** >(icc->rfsp - 16);
	char *filename = *reinterpret_cast < char **>(icc->rfsp - 8);
	//spr->save((wchar_t*)utf8_to_wstr(filename).c_str());
}

// bool spr_isExistSpr(pSprite spr, int *objptr);
void exGraphics_spr_isExistSpr(int *pcontext)
{
	ICB_Context *icc = reinterpret_cast < ICB_Context * >(pcontext);
	Sprite *spr = *reinterpret_cast < Sprite ** >(icc->rfsp - 16);
	int *objptr = *reinterpret_cast < int **>(icc->rfsp - 8);
	bool b = spr->isExistSpr(objptr);

	icc->sp -= sizeof(bool);
	*reinterpret_cast < bool * >(icc->sp) = b;
	icc->getA(0) = icc->sp - icc->mem;
    icc->Amove_pivot(-1);
}

//void spr_push_rbuffer(pSprite spr, prbuffer rb);
void exGraphics_spr_push_rbuffer(int* pcontext)
{
	ICB_Context* icc = reinterpret_cast <ICB_Context*>(pcontext);
	Sprite* spr = *reinterpret_cast <Sprite**>(icc->rfsp - 16);
	rbuffer* rb = *reinterpret_cast <rbuffer**>(icc->rfsp - 8);
	spr->Release();
	spr->st = sprite_type::st_freepolygon;
	spr->data.freepoly = rb;
	//spr->save((wchar_t*)utf8_to_wstr(filename).c_str());
}

// pObject _pObject(pSprite spr, gpos p, gpos r, gpos s, int* ecs);
void exGraphics__pObject(int *pcontext)
{
	ICB_Context *icc = reinterpret_cast < ICB_Context * >(pcontext);
	Sprite *spr = *reinterpret_cast < Sprite ** >(icc->rfsp - 52);
	shp::vec3f p = *reinterpret_cast <shp::vec3f* >(icc->rfsp - 44);
	shp::vec3f r = *reinterpret_cast <shp::vec3f* >(icc->rfsp - 32);
	shp::vec3f s = *reinterpret_cast <shp::vec3f* >(icc->rfsp - 20);
	pECS ecs = *reinterpret_cast < pECS*>(icc->rfsp - 8);
	Object *newobj = (Object *) fm->_New(sizeof(Object), true);
	newobj->null();
	newobj->source = spr;
	newobj->pos = p;
	newobj->rot = r;
	newobj->sca = s;
	newobj->ecs = ecs;

	icc->sp -= sizeof(Object *);
	*reinterpret_cast < Object ** >(icc->sp) = newobj;
	icc->getA(0) = icc->sp - icc->mem;
    icc->Amove_pivot(-1);
}

// pSprite _pSprite(int sprtype);
/*
* sprtype == 0 : freepolygon
* sprtype == 1 : objs
* sprtype == 2 : sprdir
*/
void exGraphics__pSprite(int* pcontext)
{
	ICB_Context* icc = reinterpret_cast <ICB_Context*>(pcontext);
	int mod = *reinterpret_cast <int*>(icc->rfsp - 4);
	Sprite* spr;
	spr = (Sprite*)fm->_New(sizeof(Sprite), true);
	spr->null();

	spr->st = sprite_type::st_none;
	spr->data.freepoly = nullptr;
	switch (mod) {
	case 0:
	{
		spr->st = sprite_type::st_freepolygon;
		spr->data.freepoly = nullptr;
	}
		break;
	case 1:
	{
		spr->st = sprite_type::st_objects;
		spr->data.objs = (fmvecarr<int*>*)fm->_New(sizeof(fmvecarr<int*>), true);
		spr->data.objs->NULLState();
		spr->data.objs->Init(2, false, true);
	}
		break;
	case 2:
	{
		spr->st = sprite_type::st_sprite_filedir;
		spr->data.dir = nullptr;
	}
		break;
	}

	icc->sp -= sizeof(Sprite*);
	*reinterpret_cast <Sprite**>(icc->sp) = spr;
	icc->getA(0) = icc->sp - icc->mem;
	icc->Amove_pivot(-1);
}

// pObject spr_getchild(pSprite spr, int id);
void exGraphics_spr_getchild(int *pcontext)
{
	ICB_Context *icc = reinterpret_cast < ICB_Context * >(pcontext);
	Sprite *spr = *reinterpret_cast < Sprite ** >(icc->rfsp - 12);
	int id = *reinterpret_cast < int *>(icc->rfsp - 4);
	Object *obj = (Object*)spr->data.objs->at(id);

	icc->sp -= sizeof(Object *);
	*reinterpret_cast < Object ** >(icc->sp) = obj;
	icc->getA(0) = icc->sp - icc->mem;
    icc->Amove_pivot(-1);
}

// void spr_erasechild(pSprite spr, int id);
void exGraphics_spr_erasechild(int *pcontext)
{
	ICB_Context *icc = reinterpret_cast < ICB_Context * >(pcontext);
	Sprite *spr = *reinterpret_cast < Sprite ** >(icc->rfsp - 12);
	int id = *reinterpret_cast < int *>(icc->rfsp - 4);
	spr->st = sprite_type::st_objects;
	spr->data.objs->erase(id);
}

// void spr_pushchild(pSprite spr, pObject obj);
void exGraphics_spr_pushchild(int *pcontext)
{
	ICB_Context *icc = reinterpret_cast < ICB_Context * >(pcontext);
	Sprite *spr = *reinterpret_cast < Sprite ** >(icc->rfsp - 16);
	Object *obj = *reinterpret_cast < Object ** >(icc->rfsp - 8);
	spr->st = sprite_type::st_objects;
	spr->data.objs->push_back((int *)obj);
}

//void spr_clearchild(pSprite spr);
void exGraphics_spr_clearchild(int* pcontext)
{
	ICB_Context* icc = reinterpret_cast <ICB_Context*>(pcontext);
	Sprite* spr = *reinterpret_cast <Sprite**>(icc->rfsp - 8);
	spr->st = sprite_type::st_objects;
	spr->data.objs->up = 0;
}

// gpos obj_getpos(pObject obj);
void exGraphics_obj_getpos(int *pcontext)
{
	ICB_Context *icc = reinterpret_cast < ICB_Context * >(pcontext);
	Object *obj = *reinterpret_cast < Object ** >(icc->rfsp - 8);

	icc->sp -= sizeof(shp::vec3f);
	*reinterpret_cast < shp::vec3f * >(icc->sp) = obj->pos;
	icc->getA(0) = icc->sp - icc->mem;
    icc->Amove_pivot(-1);
}

// gpos obj_getrot(pObject obj);
void exGraphics_obj_getrot(int *pcontext)
{
	ICB_Context *icc = reinterpret_cast < ICB_Context * >(pcontext);
	Object *obj = *reinterpret_cast < Object ** >(icc->rfsp - 8);

	icc->sp -= sizeof(shp::vec3f);
	*reinterpret_cast <shp::vec3f* >(icc->sp) = obj->rot;
	icc->getA(0) = icc->sp - icc->mem;
    icc->Amove_pivot(-1);
}

// gpos obj_getsca(pObject obj);
void exGraphics_obj_getsca(int *pcontext)
{
	ICB_Context *icc = reinterpret_cast < ICB_Context * >(pcontext);
	Object *obj = *reinterpret_cast < Object ** >(icc->rfsp - 8);

	icc->sp -= sizeof(shp::vec3f);
	*reinterpret_cast <shp::vec3f* >(icc->sp) = obj->sca;
	icc->getA(0) = icc->sp - icc->mem;
    icc->Amove_pivot(-1);
}

// pSprite obj_getsource(pObject obj);
void exGraphics_obj_getsource(int *pcontext)
{
	ICB_Context *icc = reinterpret_cast < ICB_Context * >(pcontext);
	Object *obj = *reinterpret_cast < Object ** >(icc->rfsp - 8);

	icc->sp -= sizeof(Sprite*);
	*reinterpret_cast < Sprite** >(icc->sp) = obj->source;
	icc->getA(0) = icc->sp - icc->mem;
    icc->Amove_pivot(-1);
}

// pECS obj_getecs(pObject obj);
void exGraphics_obj_getecs(int *pcontext)
{
	ICB_Context *icc = reinterpret_cast < ICB_Context * >(pcontext);
	Object *obj = *reinterpret_cast < Object ** >(icc->rfsp - 8);

	icc->sp -= sizeof(pECS);
	*reinterpret_cast < pECS * >(icc->sp) = obj->ecs;
	icc->getA(0) = icc->sp - icc->mem;
    icc->Amove_pivot(-1);
}

// void obj_setpos(pObject obj, gpos pos);
void exGraphics_obj_setpos(int *pcontext)
{
	ICB_Context *icc = reinterpret_cast < ICB_Context * >(pcontext);
	Object *obj = *reinterpret_cast < Object ** >(icc->rfsp - 20);
	shp::vec3f pos = *reinterpret_cast <shp::vec3f* >(icc->rfsp - 12);

	obj->pos = pos;
}

// void obj_setrot(pObject obj, gpos rot);
void exGraphics_obj_setrot(int *pcontext)
{
	ICB_Context *icc = reinterpret_cast < ICB_Context * >(pcontext);
	Object *obj = *reinterpret_cast < Object ** >(icc->rfsp - 20);
	shp::vec3f rot = *reinterpret_cast <shp::vec3f* >(icc->rfsp - 12);

	obj->rot = rot;
}

// void obj_setsca(pObject obj, gpos sca);
void exGraphics_obj_setsca(int *pcontext)
{
	ICB_Context *icc = reinterpret_cast < ICB_Context * >(pcontext);
	Object *obj = *reinterpret_cast < Object ** >(icc->rfsp - 20);
	shp::vec3f sca = *reinterpret_cast <shp::vec3f* >(icc->rfsp - 12);

	obj->sca = sca;
}

// void obj_setsource(pObject obj, pSprite spr);
void exGraphics_obj_setsource(int *pcontext)
{
	ICB_Context *icc = reinterpret_cast < ICB_Context * >(pcontext);
	Object *obj = *reinterpret_cast < Object ** >(icc->rfsp - 16);
	Sprite *spr = *reinterpret_cast < Sprite ** >(icc->rfsp - 8);

	obj->source = spr;
}

// void obj_setecs(pObject, pECS ecs);
void exGraphics_obj_setecs(int *pcontext)
{
	ICB_Context *icc = reinterpret_cast < ICB_Context * >(pcontext);
	Object *obj = *reinterpret_cast < Object ** >(icc->rfsp - 16);
	pECS ecs = *reinterpret_cast < pECS * >(icc->rfsp - 8);

	obj->ecs = ecs;
}

//uint rdtsc();
void exGraphics_rdtsc(int* pcontext){
	ICB_Context *icc = reinterpret_cast < ICB_Context * >(pcontext);
	icc->sp -= sizeof(unsigned int);
	*reinterpret_cast < unsigned int * >(icc->sp) = getticks();
	icc->getA(0) = icc->sp - icc->mem;
    icc->Amove_pivot(-1);
}

//float get_deltatime(uint t0, uint t1);
void exGraphics_get_deltatime(int* pcontext){
	ICB_Context *icc = reinterpret_cast < ICB_Context * >(pcontext);
	
	unsigned int t0 = *reinterpret_cast < unsigned int* >(icc->rfsp - 8);
	unsigned int t1 = *reinterpret_cast < unsigned int* >(icc->rfsp - 4);
	icc->sp -= sizeof(float);
	*reinterpret_cast < float * >(icc->sp) = getDeltaTime(t0, t1);
	icc->getA(0) = icc->sp - icc->mem;
    icc->Amove_pivot(-1);
}

//void Graphics_Init(int** ptr);
void exGraphics_Graphics_Init(int* pcontext) {
	ICB_Context* icc = reinterpret_cast <ICB_Context*>(pcontext);

	uint64_t logicAddr = *reinterpret_cast <uint64_t*>(icc->rfsp - 8);
	int** dest = (int**)&icc->mem[logicAddr];
	fmvecarr<gHeapCheck>* heapBuffer;
	heapBuffer = (fmvecarr<gHeapCheck>*)fm->_New(sizeof(fmvecarr<gHeapCheck>), true);
	heapBuffer->NULLState();
	heapBuffer->Init(8, false);
	*dest = (int*)heapBuffer;
}

//void Release(int layer);
void exGraphics_Release(int* pcontext) {
	ICB_Context* icc = reinterpret_cast <ICB_Context*>(pcontext);

	fmvecarr<gHeapCheck>* heapBuffer = *(fmvecarr<gHeapCheck>**) & icc->mem[icc->max_mem_byte - 9];
	int t0 = *reinterpret_cast <int*>(icc->rfsp - 4);

	if (t0 != -1) {
		for (int i = 0; i < heapBuffer->size(); ++i) {
			if (heapBuffer->at(i).shouldRelease == t0) {
				//fm->_Delete((byte8*)heapBuffer->at(i).ptr, heapBuffer->at(i).size);
				heapBuffer->at(i).ReleaseFunc(heapBuffer->at(i).ptr);
				heapBuffer->erase(i);
			}
		}
	}
	else {
		// all release
		for (int i = 0; i < heapBuffer->size(); ++i) {
			heapBuffer->at(i).ReleaseFunc(heapBuffer->at(i).ptr);
		}
		heapBuffer->release();
	}
}

void ICBRelease(void* ptr) {
	InsideCode_Bake* icb = (InsideCode_Bake*)ptr;
	icb->Release(true);
	fm->_Delete((byte8*)icb, sizeof(InsideCode_Bake));
}

void ECSRelease(void* ptr) {
	ICB_Context* cxt = (ICB_Context*)ptr;
	cxt->Release();
	fm->_Delete((byte8*)cxt, sizeof(ICB_Context));
}

void rbufferRelease(void* ptr) {
	rbuffer* rb = (rbuffer*)ptr;
	rb->Release();
	fm->_Delete((byte8*)rb, sizeof(rbuffer));
}

void SprRelease(void* ptr) {
	Sprite* spr = (Sprite*)ptr;
	spr->Release();
	fm->_Delete((byte8*)spr, sizeof(Sprite));
}

void ObjRelease(void* ptr) {
	Object* obj = (Object*)ptr;
	fm->_Delete((byte8*)obj, sizeof(Object));
}

//void ChangeICB(pICB* dest, pICB newv, int layer);
void exGraphics_ChangeICB(int* pcontext) {
	ICB_Context* icc = reinterpret_cast <ICB_Context*>(pcontext);

	fmvecarr<gHeapCheck>* heapBuffer = (fmvecarr<gHeapCheck>*) & icc->mem[icc->max_mem_byte - 8];
	uint64_t logicAddr = *reinterpret_cast <uint64_t*>(icc->rfsp - 20);
	pICB* dest = (pICB*)&icc->mem[logicAddr];
	pICB newv = *reinterpret_cast <pICB*>(icc->rfsp - 12);
	int layer = *reinterpret_cast <int*>(icc->rfsp - 4);

	if (*dest != nullptr) {
		pICB oldv = *dest;
		gHeapCheck hc;
		hc.ptr = oldv;
		hc.shouldRelease = layer;
		hc.ReleaseFunc = ICBRelease;
		heapBuffer->push_back(hc);
	}
	*dest = newv;
}

//void ChangeECS(pECS* dest, pECS newv, int layer);
void exGraphics_ChangeECS(int* pcontext) {
	ICB_Context* icc = reinterpret_cast <ICB_Context*>(pcontext);

	fmvecarr<gHeapCheck>* heapBuffer = (fmvecarr<gHeapCheck>*) & icc->mem[icc->max_mem_byte - 8];
	uint64_t logicAddr = *reinterpret_cast <uint64_t*>(icc->rfsp - 20);
	pECS* dest = (pECS*)&icc->mem[logicAddr];
	pECS newv = *reinterpret_cast <pECS*>(icc->rfsp - 12);
	int layer = *reinterpret_cast <int*>(icc->rfsp - 4);

	if (*dest != nullptr) {
		pECS oldv = *dest;
		gHeapCheck hc;
		hc.ptr = oldv;
		hc.shouldRelease = layer;
		hc.ReleaseFunc = ECSRelease;
		heapBuffer->push_back(hc);
	}
	*dest = newv;
}

//void ChangeRBuf(prbuffer* dest, prbuffer newv, int layer);
void exGraphics_ChangeRBuf(int* pcontext) {
	ICB_Context* icc = reinterpret_cast <ICB_Context*>(pcontext);

	fmvecarr<gHeapCheck>* heapBuffer = (fmvecarr<gHeapCheck>*) & icc->mem[icc->max_mem_byte - 8];
	uint64_t logicAddr = *reinterpret_cast <uint64_t*>(icc->rfsp - 20);
	prbuffer* dest = (prbuffer*)&icc->mem[logicAddr];
	prbuffer newv = *reinterpret_cast <prbuffer*>(icc->rfsp - 12);
	int layer = *reinterpret_cast <int*>(icc->rfsp - 4);

	if ((*dest).data != nullptr) {
		prbuffer oldv = *dest;
		gHeapCheck hc;
		hc.ptr = (void*)oldv.data;
		hc.shouldRelease = layer;
		hc.ReleaseFunc = rbufferRelease;
		heapBuffer->push_back(hc);
	}
	*dest = newv;
}

//void ChangeSpr(pSprite* dest, pSprite newv, int layer);
void exGraphics_ChangeSpr(int* pcontext) {
	ICB_Context* icc = reinterpret_cast <ICB_Context*>(pcontext);

	fmvecarr<gHeapCheck>* heapBuffer = (fmvecarr<gHeapCheck>*) & icc->mem[icc->max_mem_byte - 8];
	uint64_t logicAddr = *reinterpret_cast <uint64_t*>(icc->rfsp - 20);
	pSprite* dest = (pSprite*) &icc->mem[logicAddr];
	pSprite newv = *reinterpret_cast <pSprite*>(icc->rfsp - 12);
	int layer = *reinterpret_cast <int*>(icc->rfsp - 4);

	if (*dest != nullptr) {
		pSprite oldv = *dest;
		gHeapCheck hc;
		hc.ptr = oldv;
		hc.shouldRelease = layer;
		hc.ReleaseFunc = SprRelease;
		heapBuffer->push_back(hc);
	}
	*dest = newv;
}

//void ChangeObj(pObject* dest, pObject newv, int layer);
void exGraphics_ChangeObj(int* pcontext) {
	ICB_Context* icc = reinterpret_cast <ICB_Context*>(pcontext);

	fmvecarr<gHeapCheck>* heapBuffer = (fmvecarr<gHeapCheck>*) & icc->mem[icc->max_mem_byte - 8];
	uint64_t logicAddr = *reinterpret_cast <uint64_t*>(icc->rfsp - 20);
	pObject* dest = (pObject*)&icc->mem[logicAddr];
	pObject newv = *reinterpret_cast <pObject*>(icc->rfsp - 12);
	int layer = *reinterpret_cast <int*>(icc->rfsp - 4);

	if (*dest != nullptr) {
		pObject oldv = *dest;
		gHeapCheck hc;
		hc.ptr = oldv;
		hc.shouldRelease = layer;
		hc.ReleaseFunc = ObjRelease;
		heapBuffer->push_back(hc);
	}
	*dest = newv;
}

ICB_Extension *Init_exGraphics()
{
	// 확장을 입력.
	ICB_Extension *ext = (ICB_Extension *) fm->_New(sizeof(ICB_Extension), true);
	ext->exfuncArr.NULLState();
	ext->exfuncArr.Init(64, false);
	ext->exstructArr.NULLState();
	ext->exstructArr.Init(32, false);
	//dbg << "bake" << endl;
	bake_Extension("Extensions/exGraphics.txt", ext);
	int i = -1;
	//dbg << "pc input" << endl;
	ext->exfuncArr[++i]->start_pc = reinterpret_cast <byte8*>(exGraphics_get_icb);
	ext->exfuncArr[++i]->start_pc = reinterpret_cast <byte8*>(exGraphics_create_ecs);
	ext->exfuncArr[++i]->start_pc = reinterpret_cast <byte8*>(exGraphics_ecs_get_icb);
	ext->exfuncArr[++i]->start_pc = reinterpret_cast <byte8*>(exGraphics__gpos);
	ext->exfuncArr[++i]->start_pc = reinterpret_cast <byte8*>(exGraphics__gindex);
	ext->exfuncArr[++i]->start_pc = reinterpret_cast <byte8*>(exGraphics__gcolor);
	ext->exfuncArr[++i]->start_pc = reinterpret_cast <byte8*>(exGraphics_rbuff_get_vertexsiz);
	ext->exfuncArr[++i]->start_pc = reinterpret_cast <byte8*>(exGraphics_rbuff_get_choice);
	ext->exfuncArr[++i]->start_pc = reinterpret_cast <byte8*>(exGraphics_rbuff_get_inherit);
	ext->exfuncArr[++i]->start_pc = reinterpret_cast <byte8*>(exGraphics_rbuff_set_choice);
	ext->exfuncArr[++i]->start_pc = reinterpret_cast <byte8*>(exGraphics_rbuff_set_inherit);
	ext->exfuncArr[++i]->start_pc = reinterpret_cast <byte8*>(exGraphics__prbuffer);
	ext->exfuncArr[++i]->start_pc = reinterpret_cast <byte8*>(exGraphics_begin);
	ext->exfuncArr[++i]->start_pc = reinterpret_cast <byte8*>(exGraphics_end);
	ext->exfuncArr[++i]->start_pc = reinterpret_cast <byte8*>(exGraphics_av);
	ext->exfuncArr[++i]->start_pc = reinterpret_cast <byte8*>(exGraphics_rbuff_set_static_color);
	ext->exfuncArr[++i]->start_pc = reinterpret_cast <byte8*>(exGraphics_rbuff_clear);
	ext->exfuncArr[++i]->start_pc = reinterpret_cast <byte8*>(exGraphics_spr_render);
	ext->exfuncArr[++i]->start_pc = reinterpret_cast <byte8*>(exGraphics_spr_load);
	ext->exfuncArr[++i]->start_pc = reinterpret_cast <byte8*>(exGraphics_spr_save);
	ext->exfuncArr[++i]->start_pc = reinterpret_cast <byte8*>(exGraphics_spr_isExistSpr);
	ext->exfuncArr[++i]->start_pc = reinterpret_cast <byte8*>(exGraphics_spr_push_rbuffer);
	ext->exfuncArr[++i]->start_pc = reinterpret_cast <byte8*>(exGraphics__pObject);
	ext->exfuncArr[++i]->start_pc = reinterpret_cast <byte8*>(exGraphics__pSprite);
	ext->exfuncArr[++i]->start_pc = reinterpret_cast <byte8*>(exGraphics_spr_getchild);
	ext->exfuncArr[++i]->start_pc = reinterpret_cast <byte8*>(exGraphics_spr_erasechild);
	ext->exfuncArr[++i]->start_pc = reinterpret_cast <byte8*>(exGraphics_spr_pushchild);
	ext->exfuncArr[++i]->start_pc = reinterpret_cast <byte8*>(exGraphics_spr_clearchild);
	ext->exfuncArr[++i]->start_pc = reinterpret_cast <byte8*>(exGraphics_obj_getpos);
	ext->exfuncArr[++i]->start_pc = reinterpret_cast <byte8*>(exGraphics_obj_getrot);
	ext->exfuncArr[++i]->start_pc = reinterpret_cast <byte8*>(exGraphics_obj_getsca);
	ext->exfuncArr[++i]->start_pc = reinterpret_cast <byte8*>(exGraphics_obj_getsource);
	ext->exfuncArr[++i]->start_pc = reinterpret_cast <byte8*>(exGraphics_obj_getecs);
	ext->exfuncArr[++i]->start_pc = reinterpret_cast <byte8*>(exGraphics_obj_setpos);
	ext->exfuncArr[++i]->start_pc = reinterpret_cast <byte8*>(exGraphics_obj_setrot);
	ext->exfuncArr[++i]->start_pc = reinterpret_cast <byte8*>(exGraphics_obj_setsca);
	ext->exfuncArr[++i]->start_pc = reinterpret_cast <byte8*>(exGraphics_obj_setsource);
	ext->exfuncArr[++i]->start_pc = reinterpret_cast <byte8*>(exGraphics_obj_setecs);
	ext->exfuncArr[++i]->start_pc = reinterpret_cast <byte8*>(exGraphics_rdtsc);
	ext->exfuncArr[++i]->start_pc = reinterpret_cast <byte8*>(exGraphics_get_deltatime);
	ext->exfuncArr[++i]->start_pc = reinterpret_cast <byte8*>(exGraphics_Graphics_Init);
	ext->exfuncArr[++i]->start_pc = reinterpret_cast <byte8*>(exGraphics_Release);
	ext->exfuncArr[++i]->start_pc = reinterpret_cast <byte8*>(exGraphics_ChangeICB);
	ext->exfuncArr[++i]->start_pc = reinterpret_cast <byte8*>(exGraphics_ChangeECS);
	ext->exfuncArr[++i]->start_pc = reinterpret_cast <byte8*>(exGraphics_ChangeRBuf);
	ext->exfuncArr[++i]->start_pc = reinterpret_cast <byte8*>(exGraphics_ChangeSpr);
	ext->exfuncArr[++i]->start_pc = reinterpret_cast <byte8*>(exGraphics_ChangeObj);
	return ext;
}


#endif
