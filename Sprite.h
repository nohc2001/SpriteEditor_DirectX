#ifndef SPRITE_H
#define SPRITE_H

#include <fstream>
#include <string>
#include <cstdlib>
#include "Utill_FreeMemory.h"
#include "Utill_SpaceMath.h"
#include "ShapeObject.h"
//#include "dbg.h"

using namespace freemem;
using namespace shp;
using namespace std;

struct InsideCode;
struct ExecutableContext;

enum class sprite_type
{
	st_none = 0,
	st_freepolygon = 1,
	st_objects = 2,
	st_sprite_filedir = 3
};

union sprdata
{
	fmvecarr < int*>* objs;
	rbuffer* freepoly;
	wchar_t* dir;
};

struct spritedata {
	unsigned int dataptr;
	unsigned int icptr;

	int* originptr;
};

struct objdata {
	unsigned int sourceptr;
	shp::vec3f pos;
	shp::vec3f rot;
	shp::vec3f sca;
	unsigned int icptr;

	int* originptr;
};

struct sprdirdata {
	byte8 type;
	unsigned int strsize;
	wchar_t* wstr;
};

struct objarrdata {
	byte8 type;
	fmvecarr<unsigned int> objs;
};

struct freepolydata {
	byte8 type;
	fmvecarr<float> vb;
	fmvecarr<SimpleIndex> ib;
};

union s_data {
	sprdirdata sdd;
	objarrdata oad;
	freepolydata fd;

	s_data() {}
	~s_data() {}
};

struct savestruct {
	fmvecarr<spritedata> spr;
	fmvecarr<objdata> obj;
	fmvecarr<s_data> data;
	fmvecarr<unsigned int> ics;
};

class Sprite
{
public:
	sprite_type st;
	sprdata data;
	InsideCode* spr_ic;

	void null()
	{
		st = sprite_type::st_freepolygon;
		data.freepoly = nullptr;
		spr_ic = nullptr;
	}

	void render(const ConstantBuffer& uniform);

	void load(wchar_t* filename);

	void save(wchar_t* filename);

	bool isExistSpr(int* objptr);

	void Release();
};

unordered_map<wchar_t*, Sprite*> sprdirData;

class Object
{
public:
	Sprite* source;
	shp::vec3f pos;
	shp::vec3f rot;
	shp::vec3f sca;
	vecarr < ExecutableContext* >ecs;
};

void Sprite::render(const ConstantBuffer& uniform)
{
	//sdlmat4 premat = pmat;
	//dbgcount(0, dbg << "st : " << (int)st << endl);
	//dbgcount(0, dbg << "data : " << (int *)data.freepoly << endl);
	if (st == sprite_type::st_freepolygon)
	{
		if (data.freepoly != nullptr)
		{
			data.freepoly->render(uniform);
		}
	}
	else
	{
		if (data.objs != nullptr)
		{
			for (int i = data.objs->size()-1; i >= 0; --i)
			{
				Object* obj = (Object*)data.objs->at(i);
				ConstantBuffer cb = GetCamModelCB(obj->pos, obj->rot, obj->sca, DX11Color(1, 1, 1, 1));
				obj->source->render(cb);
			}
		}
	}
}

void stackdata_savedata(savestruct* ss, sprite_type datatype, sprdata data);

void stackspr_savestruct(savestruct* ss, Sprite* spr) {
	spritedata sd;
	stackdata_savedata(ss, spr->st, spr->data);
	sd.dataptr = ss->data.up - 1;

	sd.icptr = 0;
	ss->spr.push_back(sd);
}

int isExistSpr(savestruct* ss, Sprite* spr) {
	for (int i = 0; i < ss->spr.size(); ++i) {
		if (ss->spr[i].originptr == reinterpret_cast<int*>(spr)) return i;
	}
	return -1;
}

void stackobj_savestruct(savestruct* ss, Object* obj) {
	objdata od;
	int a = isExistSpr(ss, obj->source);
	if (a == -1) {
		stackspr_savestruct(ss, obj->source);
		od.sourceptr = ss->spr.up - 1;
	}
	else {
		od.sourceptr = a;
	}

	od.pos = obj->pos;
	od.rot = obj->rot;
	od.sca = obj->sca;
	od.icptr = 0;
	od.originptr = reinterpret_cast<int*>(obj);
	ss->obj.push_back(od);
}

int isExistObj(savestruct* ss, Object* obj) {
	for (int i = 0; i < ss->obj.size(); ++i) {
		if (ss->obj[i].originptr == reinterpret_cast<int*>(obj)) return i;
	}
	return -1;
}

void stackdata_savedata(savestruct* ss, sprite_type datatype, sprdata data) {
	if (datatype == sprite_type::st_freepolygon) {
		s_data sd;
		freepolydata fpd;
		fpd.type = 1;
		fmvecarr<SimpleVertex>* fv = data.freepoly->buffer[data.freepoly->get_choice()];
		fmvecarr<SimpleIndex>* iv = data.freepoly->index_buf[data.freepoly->get_choice()];

		for (int i = 0; i < fv->size(); ++i) {
			fpd.vb.push_back(fv->at(i).Pos.x);
			fpd.vb.push_back(fv->at(i).Pos.y);
			fpd.vb.push_back(fv->at(i).Pos.z);
			fpd.vb.push_back(fv->at(i).Color.x);
			fpd.vb.push_back(fv->at(i).Color.y);
			fpd.vb.push_back(fv->at(i).Color.z);
			fpd.vb.push_back(fv->at(i).Color.w);
		}
		for (int i = 0; i < iv->size(); ++i) {
			fpd.ib.push_back(iv->at(i));
		}
		sd.fd = fpd;
		ss->data.push_back(sd);
	}
	else if (datatype == sprite_type::st_objects) {
		s_data sd;
		objarrdata oad;
		oad.type = 2;
		for (int i = 0; i < data.objs->size(); ++i) {
			int a = isExistObj(ss, reinterpret_cast<Object*>(data.objs->at(i)));
			if (a >= 0) {
				oad.objs.push_back(a);
			}
			else {
				stackobj_savestruct(ss, reinterpret_cast<Object*>(data.objs->at(i)));
				oad.objs.push_back(ss->obj.size() - 1);
			}
		}
		sd.oad = oad;
		ss->data.push_back(sd);
	}
	else if (datatype == sprite_type::st_sprite_filedir) {
		s_data sd;
		sprdirdata sdd;
		sdd.type = 3;
		sdd.strsize = wcslen(data.dir) + 1;
		sdd.wstr = data.dir;
		sd.sdd = sdd;
		ss->data.push_back(sd);
	}
}

void stackic_savedata()
{
}

void Sprite::load(wchar_t* filename)
{

}

void Sprite::save(wchar_t* filename)
{
	savestruct ss;
	ss.spr.NULLState();
	ss.spr.Init(8, false);
	ss.obj.NULLState();
	ss.obj.Init(8, false);
	ss.data.NULLState();
	ss.data.Init(8, false);
	ss.ics.NULLState();
	ss.ics.Init(8, false);
	stackspr_savestruct(&ss, this);

	// Create a file name with wide characters
	int fnlen = wcslen(filename);

	// Convert the file name to a char*
	fm->_tempPushLayer();
	char* fileNameChar = (char*)fm->_tempNew(fnlen * 4 + 1);
	std::wcstombs(fileNameChar, filename, fnlen * 4 + 1);

	// Open the file in binary mode
	std::ofstream file(fileNameChar, std::ios::binary);

	// Write some data to the file
	//int data = 42;
	//file.write(reinterpret_cast<const char*>(&data), sizeof(data));

	unsigned int sprsiz = ss.spr.size();
	file.write(reinterpret_cast<const char*>(&sprsiz), sizeof(sprsiz));

	for (int i = 0; i < sprsiz; ++i) {
		spritedata spr = ss.spr[i];
		file.write(reinterpret_cast<const char*>(&spr.dataptr), sizeof(spr.dataptr));
		file.write(reinterpret_cast<const char*>(&spr.icptr), sizeof(spr.icptr));
	}

	unsigned int objsiz = ss.obj.size();
	file.write(reinterpret_cast<const char*>(&objsiz), sizeof(objsiz));

	for (int i = 0; i < objsiz; ++i) {
		objdata obj = ss.obj[i];
		file.write(reinterpret_cast<const char*>(&obj.sourceptr), sizeof(obj.sourceptr));
		file.write(reinterpret_cast<const char*>(&obj.pos), sizeof(obj.pos));
		file.write(reinterpret_cast<const char*>(&obj.rot), sizeof(obj.rot));
		file.write(reinterpret_cast<const char*>(&obj.sca), sizeof(obj.sca));
		file.write(reinterpret_cast<const char*>(&obj.icptr), sizeof(obj.icptr));
	}

	unsigned int datasiz = ss.data.size();
	file.write(reinterpret_cast<const char*>(&datasiz), sizeof(datasiz));
	for (int i = 0; i < datasiz; ++i) {
		s_data sd = ss.data[i];
		if (sd.fd.type == 1) {

		}
		else if (sd.fd.type == 2) {
		}
		else if (sd.fd.type == 3) {
		}
		else {
		}
	}

	// Close the file and delete the char*
	file.close();
	fm->_tempPopLayer();
}

bool Sprite::isExistSpr(int* objptr) {
	if (this->st == sprite_type::st_objects) {
		for (int i = 0; i < data.objs->size(); ++i) {
			Object* mobj = (Object*)data.objs->at(i);
			if (mobj->source == nullptr) continue;
			if ((Sprite*)objptr == mobj->source) {
				return true;
			}
			bool b = mobj->source->isExistSpr(objptr);
			if (b) {
				return true;
			}
		}
	}
	return false;
}

void Sprite::Release() {
	if (st == sprite_type::st_freepolygon) {
		if (data.freepoly != nullptr) {
			data.freepoly->Release();
			fm->_Delete((byte8*)data.freepoly, sizeof(rbuffer));
		}
	}
	else if (st == sprite_type::st_objects) {
		if (data.objs != nullptr) {
			for (int i = 0; i < data.objs->size(); ++i) {
				// check obj exist??
				Object* obj = (Object*)data.objs->at(i);
				if (obj != nullptr) {
					fm->_Delete((byte8*)obj, sizeof(Object));
					data.objs->at(i) = nullptr;
				}
			}

			data.objs->release();
			fm->_Delete((byte8*)data.objs, sizeof(fmvecarr<int*>));
			data.objs = nullptr;
		}
	}
	else if (st == sprite_type::st_sprite_filedir) {
		if (data.dir != nullptr) {
			int maxCap = wcslen(data.dir) + 1;
			fm->_Delete((byte8*)data.dir, maxCap * sizeof(wchar_t));
			data.dir = nullptr;
		}
	}
}
#endif
