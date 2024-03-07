#ifndef DXUI_H
#define DXUI_H
#include "ShapeObject.h"

float scw;
float sch;

struct DX_Event {
	HWND hWnd;
	UINT message; 
	WPARAM wParam; 
	LPARAM lParam;
};

class Page
{
public:
	char name[128] = "main";
	void (*init_func) (Page*) = nullptr;
	void (*render_func) (Page*) = nullptr;
	void (*update_func) (Page*, float) = nullptr;
	void (*event_func) (Page*, DX_Event) = nullptr;
	freemem::FM_Model0 pfm;
	int param_array_siz = 0;

	Page()
	{
	}
	virtual ~Page()
	{
	}
};

Page* pagestack[32] = { };

int toppage = 0;

void pagestacking(Page* p)
{
	pagestack[toppage] = p;
	toppage += 1;
}

enum class DX_UI_TYPE
{
	NONE = 0,
	BTN = 1,
	SLIDER = 2
};

class DX_UI
{
public:
	bool enable;
	shp::rect4f loc;
	DX_UI_TYPE kind;

	DX_UI() :enable(true), loc(shp::rect4f(0, 0, 0, 0))
	{
	}

	virtual ~DX_UI()
	{
	}
};

class DXBtn :DX_UI
{
public:
	wchar_t text[32] =
	{
	};
	int* param[8] = { };

	DXBtn()
	{
	}
	virtual ~DXBtn()
	{
	}

	DX_UI* sup() {
		return reinterpret_cast<DX_UI*>(this);
	}

	void (*render_func) (DXBtn*) = nullptr;
	void (*update_func) (DXBtn*, float) = nullptr;
	void (*event_func) (DXBtn*, DX_Event) = nullptr;

	void init(const wchar_t* wstr, void(*init)(DXBtn*), void (*rend) (DXBtn*), void (*upda) (DXBtn*, float), void(*even)(DXBtn*, DX_Event), shp::rect4f location)
	{
		wcscpy(text, wstr);
		render_func = rend;
		update_func = upda;
		event_func = even;
		loc = location;
		for (int i = 0; i < 8; ++i) {
			param[i] = nullptr;
		}
		init(this);
	}
	void Update(float delta)
	{
		if (update_func != nullptr)
		{
			update_func(this, delta);
		}
	}
	void Render()
	{
		if (render_func != nullptr)
		{
			render_func(this);
		}
	}
	void Event(DX_Event event)
	{
		if (event_func != nullptr)
		{
			event_func(this, event);
		}
	}
};

class DXSlider :DX_UI
{
public:
	float max = 1;
	float setter = 0;
	int* obj = 0;
	char mod = 'n';				// n : int | f : float
	int* param[10] = { };
	bool horizontal = true;
	void (*render_func) (DXSlider*) = nullptr;
	void (*update_func) (DXSlider*, float) = nullptr;
	void (*event_func) (DXSlider*, DX_Event) = nullptr;

	DXSlider()
	{
	}
	virtual ~DXSlider()
	{
	}

	DX_UI* sup() {
		return reinterpret_cast<DX_UI*>(this);
	}

	void init(float maxnum, char objmod, int* objt, shp::rect4f location, void(*init)(DXSlider*), void (*rend) (DXSlider*), void (*upda) (DXSlider*, float), void(*even)(DXSlider*, DX_Event), bool hori)
	{
		render_func = rend;
		update_func = upda;
		event_func = even;
		max = maxnum;
		mod = objmod;
		obj = objt;
		loc = location;
		horizontal = hori;
		switch (mod)
		{
		case 'n':
			setter = *reinterpret_cast <int*>(obj);
			break;
		case 'f':
			setter = *reinterpret_cast <float*>(obj);
			break;
		}
		init(this);
	}

	void update(float delta)
	{
		if (update_func != nullptr)
		{
			update_func(this, delta);
		}
	}
	void render()
	{
		if (render_func != nullptr)
		{
			render_func(this);
		}
	}
	void Event(DX_Event event)
	{
		if (event_func != nullptr)
		{
			event_func(this, event);
		}
	}
};

#endif