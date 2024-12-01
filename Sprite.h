#ifndef SPRITE_H
#define SPRITE_H

#include <fstream>
#include <string>
#include <cstdlib>
#include <algorithm>
#include <functional>
#include <iostream>
#include <vector>
#include "Utill_FreeMemory.h"
#include "Utill_SpaceMath.h"
#include "ShapeObject.h"
#include "InsideCodeBake.h"
#include "ICB_Extension.h"
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
	s_data(const s_data& ref) {
		sdd = ref.sdd;
		oad = ref.oad;
		fd = ref.fd;
	}

	void operator=(const s_data& ref) {
		sdd = ref.sdd;
		oad = ref.oad;
		fd = ref.fd;
	}
	~s_data() {}
};

struct savestruct {
	fmvecarr<spritedata> spr;
	fmvecarr<objdata> obj;
	fmvecarr<s_data> data;
	fmvecarr<unsigned int> ics;
};

template <class _Tp>
struct str_equal_to : public std::binary_function<_Tp, _Tp, bool>
{
	bool operator()(const _Tp& __x, const _Tp& __y) const
	{
		return strcmp(__x, __y) == 0;
	}
};

typedef std::unordered_map<char*, InsideCode_Bake*, std::hash<char*>, str_equal_to<char*> > ICMAP;
ICMAP icmap;

fmvecarr<XMMATRIX> worldMatrixStack;

class Sprite
{
public:
	sprite_type st;
	sprdata data;
	//InsideCode* spr_ic;

	static void StaticInit() {
		worldMatrixStack.NULLState();
		worldMatrixStack.Init(8, false, true);
	}

	void null()
	{
		st = sprite_type::st_freepolygon;
		data.freepoly = nullptr;
		//spr_ic = nullptr;
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
	ICB_Context* ecs;

	void null()
	{
		source = nullptr;
		pos = shp::vec3f(0, 0, 0);
		rot = shp::vec3f(0, 0, 0);
		sca = shp::vec3f(1, 1, 1);
		ecs = nullptr;
	}
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
			worldMatrixStack.push_back(uniform.mWorld);
			for (int i = data.objs->size()-1; i >= 0; --i)
			{
				Object* obj = (Object*)data.objs->at(i);
				ConstantBuffer cb = GetCamModelCB(obj->pos, obj->rot, obj->sca, DX11Color(1, 1, 1, 1));
				cb.mWorld = XMMatrixMultiply(uniform.mWorld, cb.mWorld);
				cb.StaticColor = MergeColor_MulMode(uniform.StaticColor, cb.StaticColor);
				/*for (int i = worldMatrixStack.size() ; i >= 0; --i) {
					cb.mWorld = XMMatrixMultiply(cb.mWorld, worldMatrixStack[i]);
				}*/
				obj->source->render(cb);
			}
			worldMatrixStack.pop_back();
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
	savestruct ss;
	ss.spr.NULLState();
	ss.spr.Init(8, false);
	ss.obj.NULLState();
	ss.obj.Init(8, false);
	ss.data.NULLState();
	ss.data.Init(8, false);
	ss.ics.NULLState();
	ss.ics.Init(8, false);

	dbg << "fileload" << endl;
	// Create a file name with wide characters
	int fnlen = wcslen(filename);
	dbg << "fnlen : " << fnlen << endl;

	// Convert the file name to a char*
	fm->_tempPushLayer();
	char* fileNameChar = (char*)fm->_tempNew(fnlen * 4 + 1);
	std::wcstombs(fileNameChar, filename, fnlen * 4 + 1);
	dbg << "filename : :" << fileNameChar << endl;

	// Open the file in binary mode
	std::ifstream file(fileNameChar, std::ios::binary);

	unsigned int sprsiz = 0;
	file.read(reinterpret_cast<char*>(&sprsiz), sizeof(sprsiz));
	dbg << "sprsiz : " << sprsiz << endl;

	for (int i = 0; i < sprsiz; ++i) {
		spritedata spr;
		file.read(reinterpret_cast<char*>(&spr.dataptr), sizeof(spr.dataptr));
		file.read(reinterpret_cast<char*>(&spr.icptr), sizeof(spr.icptr));
		dbg << "index : " << i << " ] _dataptr : " << spr.dataptr << " _icptr : " << spr.icptr << endl;
		ss.spr.push_back(spr);
	}

	unsigned int objsiz = 0;
	file.read(reinterpret_cast<char*>(&objsiz), sizeof(objsiz));
	dbg << "objsiz : " << objsiz << endl;

	for (int i = 0; i < objsiz; ++i) {
		objdata obj;
		file.read(reinterpret_cast<char*>(&obj.sourceptr), sizeof(obj.sourceptr));
		file.read(reinterpret_cast<char*>(&obj.pos), sizeof(obj.pos));
		file.read(reinterpret_cast<char*>(&obj.rot), sizeof(obj.rot));
		file.read(reinterpret_cast<char*>(&obj.sca), sizeof(obj.sca));
		file.read(reinterpret_cast<char*>(&obj.icptr), sizeof(obj.icptr));
		dbg << "index : " << i << " ] _sourceptr : " << obj.sourceptr << ", _icptr : " << obj.icptr << endl;
		dbg << "pos : (" << obj.pos.x << ", " << obj.pos.y << ", " << obj.pos.z << ")" << endl;
		dbg << "rot : (" << obj.rot.x << ", " << obj.rot.y << ", " << obj.rot.z << ")" << endl;
		dbg << "sca : (" << obj.sca.x << ", " << obj.sca.y << ", " << obj.sca.z << ")" << endl;
		ss.obj.push_back(obj);
	}

	unsigned int datasiz;
	file.read(reinterpret_cast<char*>(&datasiz), sizeof(datasiz));
	dbg << "datasiz : " << datasiz << endl;
	for (int i = 0; i < datasiz; ++i) {
		s_data sd;
		unsigned char type = 0;
		file.read(reinterpret_cast<char*>(&type), 1);
		dbg << "index : " << i << ", type : " << (int)type << endl;
		if (type == 1) {
			sd.fd.type = type;
			unsigned int vsiz;
			file.read(reinterpret_cast<char*>(&vsiz), 4);
			dbg << "vsiz : " << vsiz << endl;
			sd.fd.vb.NULLState();
			sd.fd.vb.Init(vsiz, false);
			for (int i = 0; i < vsiz; ++i) {
				float v = 0;
				file.read(reinterpret_cast<char*>(&v), 4);
				dbg << "[fv" << i << " : " << v << "]" << endl;
				sd.fd.vb.push_back(v);
			}
			dbg << endl;
			unsigned int isiz;
			file.read(reinterpret_cast<char*>(&isiz), 4);
			dbg << "isiz : " << isiz << endl;
			sd.fd.ib.NULLState();
			sd.fd.ib.Init(isiz, false);
			for (int i = 0; i < isiz; ++i) {
				SimpleIndex ind = SimpleIndex(0, 0, 0);
				unsigned int indd[3] = {};
				file.read(reinterpret_cast<char*>(&indd[0]), 4);
				file.read(reinterpret_cast<char*>(&indd[1]), 4);
				file.read(reinterpret_cast<char*>(&indd[2]), 4);
				ind.pos0 = (WORD)indd[0];
				ind.pos1 = (WORD)indd[1];
				ind.pos2 = (WORD)indd[2];
				dbg << "[iv" << i << " : (" << ind.pos0 << ", " << ind.pos1 << ", " << ind.pos2 << ")]" << endl;
				sd.fd.ib.push_back(ind);
			}
			ss.data.push_back(sd);
		}
		else if (type == 2) {
			sd.oad.type = type;
			unsigned int osiz;
			file.read(reinterpret_cast<char*>(&osiz), 4);
			sd.oad.objs.NULLState();
			sd.oad.objs.Init(osiz, false);
			for (int i = 0; i < osiz; ++i) {
				unsigned int optr = 0;
				file.read(reinterpret_cast<char*>(&optr), 4);
				sd.oad.objs.push_back(optr);
			}
			ss.data.push_back(sd);
		}
		else if (type == 3) {
			sd.oad.type = type;
			unsigned int strsiz = 0;
			file.read(reinterpret_cast<char*>(&strsiz), 4);
			sd.sdd.wstr = (wchar_t*)fm->_New(sizeof(wchar_t) * strsiz, true);
			for (int i = 0; i < strsiz; ++i) {
				file.read(reinterpret_cast<char*>(&sd.sdd.wstr[i]), 4);
			}
			ss.data.push_back(sd);
		}
		else {
			char t = 0;
			file.read(reinterpret_cast<char*>(&t), 1);
		}
	}

	// Close the file and delete the char*
	file.close();
	fm->_tempPopLayer();

	Sprite* sprarr = (Sprite*)fm->_New(sizeof(Sprite) * sprsiz, true);
	Object* objarr = (Object*)fm->_New(sizeof(Object) * objsiz, true);
	for (int i = 0; i < sprsiz; ++i) {
		int n = ss.spr[i].dataptr;
		s_data sd = ss.data[n];
		sprarr[i].st = (sprite_type)sd.fd.type;
		if (sprarr[i].st == sprite_type::st_freepolygon) {
			sprarr[i].data.freepoly = (rbuffer*)fm->_New(sizeof(rbuffer), true);
			sprarr[i].data.freepoly->Init(false);
			sprarr[i].data.freepoly->begin();
			for (int k = 0; k < sd.fd.vb.size() / 7; ++k) {
				shp::vec3f f = *(shp::vec3f*)&sd.fd.vb.at(7 * k);
				DX11Color c = *(DX11Color*)&sd.fd.vb.at(7 * k + 3);
				sprarr[i].data.freepoly->av(SimpleVertex(f, c));
			}
			sprarr[i].data.freepoly->end();
		}
		if (sprarr[i].st == sprite_type::st_objects) {
			sprarr[i].data.objs = (fmvecarr<int*>*)fm->_New(sizeof(fmvecarr<int*>), true);
			sprarr[i].data.objs->NULLState();
			sprarr[i].data.objs->Init(sd.oad.objs.size(), false);
			for (int k = 0; k < sd.oad.objs.size(); ++k) {
				sprarr[i].data.objs->push_back(reinterpret_cast<int*>(&objarr[sd.oad.objs.at(k)]));
			}
		}
		if (sprarr[i].st == sprite_type::st_sprite_filedir) {
			sprarr[i].data.dir = (wchar_t*)fm->_New(sizeof(wchar_t) * sd.sdd.strsize, true);
			wcscpy(sprarr[i].data.dir, sd.sdd.wstr);
		}
	}

	for (int i = 0; i < objsiz; ++i) {
		objarr[i].source = &sprarr[ss.obj[i].sourceptr];
		objarr[i].pos = ss.obj[i].pos;
		objarr[i].rot = ss.obj[i].rot;
		objarr[i].sca = ss.obj[i].sca;
		//ecs
	}

	Sprite* lastspr = &sprarr[sprsiz - 1];
	this->st = lastspr->st;
	this->data = lastspr->data;
	//this->spr_ic = lastspr->spr_ic;
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
			file.write(reinterpret_cast<const char*>(&sd.fd.type), 1);
			unsigned int vsiz = sd.fd.vb.size();
			file.write(reinterpret_cast<const char*>(&vsiz), 4);
			for (int i = 0; i < sd.fd.vb.size(); ++i) {
				file.write(reinterpret_cast<const char*>(&sd.fd.vb.at(i)), 4);
			}
			unsigned int isiz = sd.fd.ib.size();
			file.write(reinterpret_cast<const char*>(&isiz), 4);
			
			for (int i = 0; i < isiz; ++i) {
				unsigned int indd[3] = { sd.fd.ib[i].pos0 , sd.fd.ib[i].pos1 , sd.fd.ib[i].pos2 };
				file.write(reinterpret_cast<const char*>(&indd[0]), 4);
				file.write(reinterpret_cast<const char*>(&indd[1]), 4);
				file.write(reinterpret_cast<const char*>(&indd[2]), 4);
			}
		}
		else if (sd.fd.type == 2) {
			file.write(reinterpret_cast<const char*>(&sd.oad.type), 1);
			unsigned int osiz = sd.oad.objs.size();
			file.write(reinterpret_cast<const char*>(&osiz), 4);
			for (int i = 0; i < osiz; ++i) {
				file.write(reinterpret_cast<const char*>(&sd.oad.objs[i]), 4);
			}
		}
		else if (sd.fd.type == 3) {
			file.write(reinterpret_cast<const char*>(&sd.sdd.type), 1);
			file.write(reinterpret_cast<const char*>(&sd.sdd.strsize), 4);
			for (int i = 0; i < sd.sdd.strsize; ++i) {
				file.write(reinterpret_cast<const char*>(&sd.sdd.wstr[i]), 4);
			}
		}
		else {
			char t = 0;
			file.write(reinterpret_cast<const char*>(&t), 1);
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
		//data.freeploy release seperated way.
		data.freepoly = nullptr;
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
