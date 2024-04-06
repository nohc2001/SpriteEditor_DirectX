//--------------------------------------------------------------------------------------
// File: Tutorial05.cpp
//
// This application demonstrates animation using matrix transformations
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
#include <windows.h>
//#include <xnamath.h>
#include "resource.h"
//#include "ShapeObject.h"
#include "dbg.h"
#include "Sprite.h"
#include "Animation.h"
#include "DX11_UI.h"
#include "hancom.h"
//#include "DX11_UI.h"


//--------------------------------------------------------------------------------------
// Global Variables
//--------------------------------------------------------------------------------------
HINSTANCE               g_hInst = NULL;
HWND                    g_hWnd = NULL;
D3D_DRIVER_TYPE         g_driverType = D3D_DRIVER_TYPE_NULL;
D3D_FEATURE_LEVEL       g_featureLevel = D3D_FEATURE_LEVEL_11_0;
IDXGISwapChain*         g_pSwapChain = NULL;
ID3D11RenderTargetView* g_pRenderTargetView = NULL;
ID3D11Texture2D*        g_pDepthStencil = NULL;
ID3D11DepthStencilView* g_pDepthStencilView = NULL;
ID3D11VertexShader*     g_pVertexShader = NULL;
ID3D11PixelShader*      g_pPixelShader = NULL;
ID3D11InputLayout*      g_pVertexLayout = NULL;
ID3D11Buffer*           g_pVertexBuffer = NULL;
ID3D11Buffer*           g_pIndexBuffer = NULL;
XMMATRIX                g_World1;
XMMATRIX                g_World2;
XMMATRIX                g_CursorWorld;
XMMATRIX                g_DBGWorld;
XMMATRIX                g_View;
XMMATRIX                g_Projection_3d;
XMMATRIX                g_Projection_2d;
XMMATRIX				CamView;
XMMATRIX				CamProj;
ConstantBuffer polygon_cb;

Sprite* mainSprite = nullptr;

Page* mainpage = nullptr;
Page* colorpage = nullptr;
Page* texteditpage = nullptr;
Page* filepage = nullptr;

rbuffer* linedrt;
bool press_ef = false;
float zoomrate;
shp::vec2f scwh;

UINT width;
UINT height;
rbuffer cursor_obj;
rbuffer polygon_obj;
rbuffer dbgpos_obj;

struct pos_select_obj
{
	int index;
	shp::vec3f origin_pos;
	DX11Color origin_color;

	pos_select_obj(){}
};

enum class mainpm {
	present_center = 0
	, finger_pressed = 8
	, presspos = 9
	, centerorigin = 17
	, behavetop = 25
	, freepoly = 225
	, textoutline = 425
	, makeobj = 625
	, translate = 825
	, scale = 1025
	, objselect = 1225
	, rotation = 1425
	, selection = 1625
	, setcolor = 1825
	, behave_selected = 2025
	, save = 2029
	, adding_polygon = 2229
	, present_color = 2301
	, select_rect = 2317
	, select_arr = 2341
	, stacktime = 2373
	, selectclear = 2377
	, opencolorpage = 2577
	, openobjlist = 2777
	, showobjlist = 2977
	, closeobjlist = 2978
	, savefilestr = 3178
};
enum class colorpm {
	presentcolor = 0
	, RSlider = 16
	, GSlider = 200
	, BSlider = 384
	, ASlider = 568
	, done = 752
	, pallete = 952
	, selectnum = 2552
};
enum class texteditpm {
	VKeyboard = 0
	, ishan = 10000
	, isshift = 10001
	, deststring = 10002
};

void drawline(shp::vec2f p0, shp::vec2f p1, float linewidth, DX11Color color)
{
    shp::vec2f delta = p1 - p0;
    float radian = shp::angle2f::usedxdy(delta.x, delta.y).radian;
    ConstantBuffer cb = GetBasicModelCB(shp::vec3f(0.5f * (p0.x + p1.x), 0.5f * (p0.y + p1.y), 0), shp::vec3f(0, 0, radian),
            shp::vec3f(sqrtf(delta.x * delta.x + delta.y * delta.y), linewidth, 1), color);
    linedrt->render(cb);
}

shp::vec2f GetMousePos(LPARAM lParam) {
	int x = LOWORD(lParam);
	int y = HIWORD(lParam);
	return shp::vec2f((float)x - (float)scw / 2.0f, -1.0f * (float)y + (float)sch / 2.0f);
}

shp::vec2f GetMousePos_notcenter(LPARAM lParam) {
	int x = LOWORD(lParam);
	int y = HIWORD(lParam);
	return shp::vec2f((float)x, (float)y);
}

//control
bool ctrl_3d = true;
//shp::vec2f DX_UI::mousePos = shp::vec2f(0, 0);
//int DX_UI::FocusID = -1;
XMFLOAT4 rbuffer::InputColor;
//--------------------------------------------------------------------------------------
// Forward declarations
//--------------------------------------------------------------------------------------
HRESULT InitWindow( HINSTANCE hInstance, int nCmdShow );
HRESULT InitDevice();
void CleanupDevice();
LRESULT CALLBACK    WndProc( HWND, UINT, WPARAM, LPARAM );
void Render();

FM_System0* fm;
bool FinishInit = false;


void basicbtn_init(DXBtn* btn)
{
	if (btn->param[0] == nullptr)
	{
		btn->param[0] = (int*)fm->_New(sizeof(rbuffer), true);
		rbuffer* rb = (rbuffer*)btn->param[0];
		rb->Init(false);
		rb->begin();
		rb->av(SimpleVertex(shp::vec3f(-0.5f, -0.5f, 0), DX11Color(0.4f, 0.4f, 0.4f, 0.7f)));
		rb->av(SimpleVertex(shp::vec3f(0.5f, -0.5f, 0), DX11Color(0.5f, 0.6f, 0.5f, 0.8f)));
		rb->av(SimpleVertex(shp::vec3f(0.5f, 0.5f, 0), DX11Color(0.6f, 0.6f, 0.6f, 0.9f)));
		rb->av(SimpleVertex(shp::vec3f(-0.5f, 0.5f, 0), DX11Color(0.5f, 0.5f, 0.6f, 0.8f)));
		rb->end();
	}

	if (btn->param[1] == nullptr)
	{
		btn->param[1] = (int*)fm->_New(sizeof(shp::vec2f), true);
		shp::vec2f* flow = (shp::vec2f*)btn->param[1];
		*flow = shp::vec2f(0, 1.0f);
	}
}

void basicbtn_render(DXBtn* btn)
{
	rbuffer* rb = (rbuffer*)btn->param[0];
	shp::vec2f* flow = (shp::vec2f*)btn->param[1];

	float rate = AnimClass::EaseOut(flow->x / flow->y, 5.0f);
	shp::vec2f cen = btn->sup()->loc.getCenter();
	float expendrate = 0.5f + 0.5f * rate;
	ConstantBuffer cb = GetBasicModelCB(shp::vec3f(cen.x, cen.y, 0), shp::vec3f(0, 0, 0),
		shp::vec3f(btn->sup()->loc.getw() * expendrate, btn->sup()->loc.geth() * expendrate, 1),
		DX11Color(1.0f, 1.0f, 1.0f, 1.0f));
	rb->render(cb);

	shp::rect4f loc = btn->sup()->loc;
	loc.fx += loc.getw() / 4.0f;
	loc.fy += loc.geth() / 4.0f;
	draw_string(btn->text, wcslen(btn->text), 40.0f * expendrate, loc, DX11Color(1.0f, 1.0f, 1.0f, 1.0f));
}

void basicbtn_update(DXBtn* btn, float delta)
{
	shp::vec2f* flow = (shp::vec2f*)btn->param[1];

	flow->x += delta;
	// dbg << "flow : " << flow->x << endl;
	if (flow->x > flow->y)
	{
		flow->x = flow->y;
	}
}

void basicbtn_event(DXBtn* btn, DX_Event evt)
{
	shp::vec2f* flow = (shp::vec2f*)btn->param[1];

	if (evt.wParam == WM_LBUTTONDOWN)
	{
		shp::vec2f mpos = GetMousePos(evt.lParam);
		// dbg << "mpos : " << x << ", " << y << endl;
		if (shp::bPointInRectRange(shp::vec2f(mpos.x, mpos.y), btn->sup()->loc) && press_ef)
		{
			press_ef = false;
			flow->x = 0;
			// operate
		}
	}
}

void behavetopbtn_init(DXBtn* btn)
{
	if (btn->param[0] == nullptr)
	{
		btn->param[0] = (int*)fm->_New(sizeof(rbuffer), true);
		rbuffer* rb = (rbuffer*)btn->param[0];
		rb->Init(false);
		rb->begin();
		rb->av(SimpleVertex(shp::vec3f(-0.5f, -0.5f, 0), DX11Color(0.1f, 0.1f, 0.1f, 1.0f)));
		rb->av(SimpleVertex(shp::vec3f(0.5f, -0.5f, 0), DX11Color(0.5f, 0.6f, 0.5f, 0.8f)));
		rb->av(SimpleVertex(shp::vec3f(0.5f, 0.5f, 0), DX11Color(0.6f, 0.6f, 0.6f, 0.9f)));
		rb->av(SimpleVertex(shp::vec3f(-0.5f, 0.5f, 0), DX11Color(0.5f, 0.5f, 0.6f, 0.8f)));
		rb->end();
	}

	if (btn->param[1] == nullptr)
	{
		btn->param[1] = (int*)fm->_New(sizeof(shp::vec2f), true);
		shp::vec2f* flow = (shp::vec2f*)btn->param[1];
		*flow = shp::vec2f(0, 1.0f);
	}

	if (btn->param[2] == nullptr)
	{
		btn->param[2] = (int*)fm->_New(sizeof(shp::vec2i), true);
		shp::vec2i* mod = (shp::vec2i*)btn->param[2];
		*mod = shp::vec2i(0, 2);
	}
}

void behavetopbtn_event(DXBtn* btn, DX_Event evt)
{
	shp::vec2f* flow = (shp::vec2f*)btn->param[1];
	shp::vec2i* mod = (shp::vec2i*)btn->param[2];

	if (evt.wParam == WM_LBUTTONDOWN)
	{
		shp::vec2f mpos = GetMousePos(evt.lParam);
		// dbg << "mpos : " << x << ", " << y << endl;
		if (shp::bPointInRectRange(shp::vec2f(mpos.x, mpos.y), btn->sup()->loc) && press_ef)
		{
			press_ef = false;
			flow->x = 0;
			// operate
			mod->x += 1;
			if (mod->x >= mod->y)
			{
				mod->x = 0;
			}

			switch (mod->x)
			{
			case 0:
				wcscpy(btn->text, L"+");
				break;
			case 1:
				wcscpy(btn->text, L"=");
				break;
			}
		}
	}
}

void freepolybtn_init(DXBtn* btn)
{
	if (btn->param[0] == nullptr)
	{
		btn->param[0] = (int*)fm->_New(sizeof(rbuffer), true);
		rbuffer* rb = (rbuffer*)btn->param[0];
		rb->Init(false);
		rb->begin();
		rb->av(SimpleVertex(shp::vec3f(-0.5f, -0.5f, 0), DX11Color(0.4f, 0.4f, 0.4f, 0.7f)));
		rb->av(SimpleVertex(shp::vec3f(0.5f, -0.5f, 0), DX11Color(0.5f, 0.6f, 0.5f, 0.8f)));
		rb->av(SimpleVertex(shp::vec3f(0.5f, 0.5f, 0), DX11Color(0.6f, 0.6f, 0.6f, 0.9f)));
		rb->av(SimpleVertex(shp::vec3f(-0.5f, 0.5f, 0), DX11Color(0.5f, 0.5f, 0.6f, 0.8f)));
		rb->end();
	}

	if (btn->param[1] == nullptr)
	{
		btn->param[1] = (int*)fm->_New(sizeof(shp::vec2f), true);
		shp::vec2f* flow = (shp::vec2f*)btn->param[1];
		*flow = shp::vec2f(0, 1.0f);
	}

	if (btn->param[2] == nullptr)
	{
		btn->param[2] = (int*)fm->_New(sizeof(bool), true);
		bool* enable = (bool*)btn->param[2];
		*enable = false;
	}
}

void freepolybtn_render(DXBtn* btn)
{
	rbuffer* rb = (rbuffer*)btn->param[0];
	shp::vec2f* flow = (shp::vec2f*)btn->param[1];
	bool* enable = (bool*)btn->param[2];

	float rate = AnimClass::EaseOut(flow->x / flow->y, 5.0f);
	shp::vec2f cen = btn->sup()->loc.getCenter();

	float expendrate = 0.6f + 0.5f * rate;
	ConstantBuffer cb = GetBasicModelCB(shp::vec3f(cen.x, cen.y, 0), 
		shp::vec3f(0, 0, 0), shp::vec3f(btn->sup()->loc.getw() * expendrate, 
			btn->sup()->loc.geth() * expendrate, 1), DX11Color(1.0f, 1.0f, 1.0f, 1.0f));
	if (*enable)
	{
		linedrt->render(cb);
	}

	expendrate = 0.5f + 0.5f * rate;
	ConstantBuffer cb2 = GetBasicModelCB(shp::vec3f(cen.x, cen.y, 0), shp::vec3f(0, 0, 0),
		shp::vec3f(btn->sup()->loc.getw() * expendrate,
			btn->sup()->loc.geth() * expendrate, 1), DX11Color(1.0f, 1.0f, 1.0f, 1.0f));
	rb->render(cb2);

	shp::rect4f loc = btn->sup()->loc;
	loc.fx += loc.getw() / 4.0f;
	loc.fy += loc.geth() / 4.0f;
	draw_string(btn->text, wcslen(btn->text), 20.0f * expendrate, loc, DX11Color(1.0f, 1.0f, 1.0f, 1.0f));
}

void freepolybtn_event(DXBtn* btn, DX_Event evt)
{
	shp::vec2f* flow = (shp::vec2f*)btn->param[1];
	bool* enable = (bool*)btn->param[2];

	if (evt.wParam == WM_LBUTTONDOWN)
	{
		shp::vec2f mpos = GetMousePos(evt.lParam);
		
		if (shp::bPointInRectRange(shp::vec2f(mpos.x, mpos.y), btn->sup()->loc) && press_ef)
		{
			press_ef = false;
			flow->x = 0;
			*enable = !*enable;

			Page* p = (Page*)btn->param[3];
			if (*enable)
			{
				counting[0] = 1000;
				*reinterpret_cast <int*>(&p->pfm.Data[(int)mainpm::behave_selected]) =
					*btn->param[4];
				// dbg << *reinterpret_cast < int *>(&p->pfm.Data[2929]) <<
				// endl;

				if (*btn->param[4] == 0)
				{
					dbgcount(0, dbg << "p4 : " << (int*)mainSprite << endl);
					if (mainSprite == nullptr)
					{
						dbgcount(0, dbg << "mainSprite_init" << endl);
						mainSprite = (Sprite*)fm->_New(sizeof(Sprite), true);
						mainSprite->null();
						mainSprite->st = sprite_type::st_freepolygon;
						mainSprite->data.freepoly = (rbuffer*)fm->_New(sizeof(rbuffer), true);
						mainSprite->data.freepoly->Init(false);
						mainSprite->data.freepoly->begin();
						mainSprite->data.freepoly->set_inherit(true);
						mainSprite->data.freepoly->end();

					}
				}
			}
			else
			{
				*reinterpret_cast <int*>(&p->pfm.Data[(int)mainpm::behave_selected]) = -1;
				if (*btn->param[4] == 0)
				{
					rbuffer* rb = (rbuffer*)mainSprite->data.freepoly;
					//rb->clear();
					rb->begin();
					rb->set_inherit(true);
					fmvecarr < pos_select_obj >* sarr =
						(fmvecarr < pos_select_obj >
							*) & mainpage->pfm.Data[(int)mainpm::select_arr];
					sarr->clear();
				}
			}
		}
	}
}

void savebtn_event(DXBtn* btn, DX_Event evt)
{
	shp::vec2f* flow = (shp::vec2f*)btn->param[1];
	bool* enable = (bool*)btn->param[2];

	if (evt.wParam == WM_LBUTTONDOWN)
	{
		shp::vec2f mpos = GetMousePos(evt.lParam);
		
		if (shp::bPointInRectRange(shp::vec2f(mpos.x, mpos.y), btn->sup()->loc) && press_ef)
		{
			press_ef = false;
			flow->x = 0;
			wchar_t** wptr = (wchar_t**)&texteditpage->pfm.Data[(int)texteditpm::deststring];
			*wptr = (wchar_t*)&mainpage->pfm.Data[(int)mainpm::savefilestr];
			pagestacking(texteditpage);
		}
	}
}

void translatebtn_event(DXBtn* btn, DX_Event evt)
{
	shp::vec2f* flow = (shp::vec2f*)btn->param[1];
	bool* enable = (bool*)btn->param[2];

	if (evt.wParam == WM_LBUTTONDOWN)
	{
		shp::vec2f mpos = GetMousePos(evt.lParam);
		
		// dbg << "mpos : " << x << ", " << y << endl;
		if (shp::bPointInRectRange(shp::vec2f(mpos.x, mpos.y), btn->sup()->loc) && press_ef)
		{
			press_ef = false;
			flow->x = 0;
			*enable = !*enable;
			if (*enable)
			{
				*(bool*)btn->param[4] = false;
				*(bool*)btn->param[5] = false;
				*(bool*)btn->param[6] = false;
				*(bool*)btn->param[7] = false;
			}
			// counting[0] = 100;
			// operate
		}
	}
}

void setcolorbtn_event(DXBtn* btn, DX_Event evt)
{
	shp::vec2f* flow = (shp::vec2f*)btn->param[1];
	bool* enable = (bool*)btn->param[2];

	if (evt.wParam == WM_LBUTTONDOWN)
	{
		shp::vec2f mpos = GetMousePos(evt.lParam);
		
		// dbg << "mpos : " << x << ", " << y << endl;
		if (shp::bPointInRectRange(shp::vec2f(mpos.x, mpos.y), btn->sup()->loc) && press_ef)
		{
			press_ef = false;
			flow->x = 0;
			*enable = !*enable;
			if (*enable)
			{
				*(bool*)btn->param[4] = false;
				*(bool*)btn->param[5] = false;
				*(bool*)btn->param[6] = false;
				*(bool*)btn->param[7] = false;

				DX11Color* present_color =
					(DX11Color*)&mainpage->pfm.Data[(int)mainpm::present_color];
				DXBtn* selection = (DXBtn*)&mainpage->pfm.Data[(int)mainpm::selection];
				bool* selection_enable = (bool*)selection->param[2];
				fmvecarr < pos_select_obj >* sarr =
					(fmvecarr < pos_select_obj > *) & mainpage->pfm.Data[(int)mainpm::select_arr];
				fm->_tempPushLayer();
				rbuffer* ap = (rbuffer*)mainSprite->data.freepoly;
				fmvecarr<SimpleVertex>* bptr = ap->buffer[ap->get_choice()];
				fmvecarr<SimpleVertex>* farr = (fmvecarr<SimpleVertex>*)fm->_tempNew(bptr->size() * sizeof(SimpleVertex));
				for (int i = 0; i < bptr->size(); ++i)
				{
					farr->at(i) = bptr->at(i);
				}
				int vsiz = ap->get_vertexsiz(ap->get_choice());
				// dbgcount(0, dbg << "ap vertex size : " <<
				// ap->get_vertexsiz(ap->get_choice()) << endl)
				//ap->clear();
				ap->begin();
				for (int i = 0; i < vsiz; ++i)
				{
					bool isSelect = false;
					// dbgcount(0, dbg << "sarr size : " << sarr->size()
					// << endl)
					for (int k = 0; k < sarr->size(); ++k)
					{
						if (sarr->at(k).index == i)
						{
							// 
							isSelect = true;
							shp::vec3f pos;
							pos_select_obj pso = sarr->at(k);
							pos.x = pso.origin_pos.x;
							pos.y = pso.origin_pos.y;
							pos.z = pso.origin_pos.z;
							ap->av(SimpleVertex(pos, *present_color));
							// dbgcount(0, dbg << "pos : " << pos.x << ",
							// " << pos.y << ", " << pos.z << endl) 
							break;
						}
					}

					if (isSelect == false)
					{
						shp::vec3f p = *(shp::vec3f*)&farr[sizeof(SimpleVertex) * i + 4];
						DX11Color c = *(DX11Color*)&farr[sizeof(SimpleVertex) * i];
						ap->av(SimpleVertex(p, c));
					}
				}
				ap->end();
				fm->_tempPopLayer();
			}
			// counting[0] = 100;
			// operate
		}
	}
}

void objselectbtn_init(DXBtn* btn)
{
	if (btn->param[0] == nullptr)
	{
		btn->param[0] = (int*)fm->_New(sizeof(rbuffer), true);
		rbuffer* rb = (rbuffer*)btn->param[0];
		rb->Init(false);
		rb->begin();
		rb->av(SimpleVertex(shp::vec3f(-0.5f, -0.5f, 0), DX11Color(0.1f, 0.1f, 0.1f, 1.0f)));
		rb->av(SimpleVertex(shp::vec3f(0.5f, -0.5f, 0), DX11Color(0.5f, 0.6f, 0.5f, 0.8f)));
		rb->av(SimpleVertex(shp::vec3f(0.5f, 0.5f, 0), DX11Color(0.6f, 0.6f, 0.6f, 0.9f)));
		rb->av(SimpleVertex(shp::vec3f(-0.5f, 0.5f, 0), DX11Color(0.5f, 0.5f, 0.6f, 0.8f)));
		rb->end();
	}

	if (btn->param[1] == nullptr)
	{
		btn->param[1] = (int*)fm->_New(sizeof(shp::vec2f), true);
		shp::vec2f* flow = (shp::vec2f*)btn->param[1];
		*flow = shp::vec2f(0, 1.0f);
	}

	if (btn->param[2] == nullptr)
	{
		btn->param[2] = (int*)fm->_New(sizeof(shp::vec2i), true);
		shp::vec2i* mod = (shp::vec2i*)btn->param[2];
		*mod = shp::vec2i(0, 2);
	}
}

void objselectbtn_render(DXBtn* btn)
{
	rbuffer* rb = (rbuffer*)btn->param[0];
	shp::vec2f* flow = (shp::vec2f*)btn->param[1];

	float rate = AnimClass::EaseOut(flow->x / flow->y, 5.0f);
	shp::vec2f cen = btn->sup()->loc.getCenter();
	float expendrate = 0.5f + 0.5f * rate;
	ConstantBuffer cb = GetBasicModelCB(shp::vec3f(cen.x, cen.y, 0), shp::vec3f(0, 0, 0),
		shp::vec3f(btn->sup()->loc.getw() * expendrate,
			btn->sup()->loc.geth() * expendrate, 1), DX11Color(1.0f, 1.0f, 1.0f, 1.0f));
	rb->render(cb);

	shp::rect4f loc = btn->sup()->loc;
	loc.fx += loc.getw() / 4.0f;
	loc.fy += loc.geth() / 4.0f;
	draw_string(btn->text, wcslen(btn->text), 20.0f * expendrate, loc, DX11Color(1.0f, 1.0f, 1.0f, 1.0f));
}

void objselectbtn_event(DXBtn* btn, DX_Event evt)
{
	shp::vec2f* flow = (shp::vec2f*)btn->param[1];
	shp::vec2i* mod = (shp::vec2i*)btn->param[2];

	if (evt.wParam == WM_LBUTTONDOWN)
	{
		shp::vec2f mpos = GetMousePos(evt.lParam);
		
		// dbg << "mpos : " << x << ", " << y << endl;
		if (shp::bPointInRectRange(shp::vec2f(mpos.x, mpos.y), btn->sup()->loc) && press_ef)
		{
			press_ef = false;
			flow->x = 0;
			// operate
			mod->x += 1;
			if (mod->x >= mod->y)
			{
				mod->x = 0;
			}

			switch (mod->x)
			{
			case 0:
				wcscpy(btn->text, L"Cam");
				break;
			case 1:
				wcscpy(btn->text, L"Obj");
				break;
			}

			// counting[0] = 1000;
		}
	}
}

void selectclearbtn_event(DXBtn* btn, DX_Event evt)
{
	shp::vec2f* flow = (shp::vec2f*)btn->param[1];

	if (evt.wParam == WM_LBUTTONDOWN)
	{
		shp::vec2f mpos = GetMousePos(evt.lParam);
		
		// dbg << "mpos : " << x << ", " << y << endl;
		if (shp::bPointInRectRange(shp::vec2f(mpos.x, mpos.y), btn->sup()->loc) && press_ef)
		{
			press_ef = false;
			flow->x = 0;
			// operate
			fmvecarr < pos_select_obj >* select_arr = (fmvecarr < pos_select_obj > *)btn->param[3];
			select_arr->clear();
		}
	}
}

void opencolorpagebtn_event(DXBtn* btn, DX_Event evt)
{
	shp::vec2f* flow = (shp::vec2f*)btn->param[1];

	if (evt.wParam == WM_LBUTTONDOWN)
	{
		shp::vec2f mpos = GetMousePos(evt.lParam);
		
		// dbg << "mpos : " << x << ", " << y << endl;
		if (shp::bPointInRectRange(shp::vec2f(mpos.x, mpos.y), btn->sup()->loc) && press_ef)
		{
			press_ef = false;
			flow->x = 0;

			pagestacking(colorpage);
			// counting[0] = 1000;
		}
	}
}

void openobjlistbtn_event(DXBtn* btn, DX_Event evt)
{
	shp::vec2f* flow = (shp::vec2f*)btn->param[1];

	if (evt.wParam == WM_LBUTTONDOWN)
	{
		shp::vec2f mpos = GetMousePos(evt.lParam);
		
		// dbg << "mpos : " << x << ", " << y << endl;
		if (shp::bPointInRectRange(shp::vec2f(mpos.x, mpos.y), btn->sup()->loc) && press_ef)
		{
			press_ef = false;
			flow->x = 0;
			*(bool*)btn->param[2] = true;
			// pagestacking(colorpage);
			// counting[0] = 1000;
		}
	}
}

void closeobjlistbtn_event(DXBtn* btn, DX_Event evt)
{
	shp::vec2f* flow = (shp::vec2f*)btn->param[1];

	if (evt.wParam == WM_LBUTTONDOWN)
	{
		shp::vec2f mpos = GetMousePos(evt.lParam);
		
		// dbg << "mpos : " << x << ", " << y << endl;
		if (shp::bPointInRectRange(shp::vec2f(mpos.x, mpos.y), btn->sup()->loc) && press_ef)
		{
			press_ef = false;
			flow->x = 0;
			*(bool*)btn->param[2] = false;
			// pagestacking(colorpage);
			// counting[0] = 1000;
		}
	}
}

void main_init(Page* p)
{
	dbg << (int)mainpm::opencolorpage << endl;
	static int behaveid_arr[6] = { 0, 1, 2, 3, 4, -1 };
	dbg << "enum class mainpm{" << endl;

	p->pfm.SetHeapData(new byte8[8192], 8192);
	dbg << "present_center = " << p->pfm.Fup << endl;
	shp::vec2f* present_center = (shp::vec2f*)(p->pfm._New(sizeof(shp::vec2f)));
	*present_center = shp::vec2f(0, 0);

	dbg << ", finger_pressed = " << p->pfm.Fup << endl;
	bool* finger_pressed = (bool*)p->pfm._New(sizeof(bool));
	*finger_pressed = false;

	dbg << ", presspos = " << p->pfm.Fup << endl;
	shp::vec2f* presspos = (shp::vec2f*)p->pfm._New(sizeof(shp::vec2f));
	*presspos = shp::vec2f(0, 0);

	dbg << ", centerorigin = " << p->pfm.Fup << endl;
	shp::vec2f* centerorigin = (shp::vec2f*)p->pfm._New(sizeof(shp::vec2f));
	*centerorigin = shp::vec2f(0, 0);

	dbg << ", behavetop = " << p->pfm.Fup << endl;
	DXBtn* behavetop = (DXBtn*)p->pfm._New(sizeof(DXBtn));
	behavetop->init(L"+", behavetopbtn_init, basicbtn_render, basicbtn_update, behavetopbtn_event,
		shp::rect4f(-scw / 2.0f, sch / 2.0f - 100, -scw / 2.0f + 100, sch / 2.0f));

	dbg << ", freepoly = " << p->pfm.Fup << endl;
	DXBtn* freepoly = (DXBtn*)p->pfm._New(sizeof(DXBtn));
	freepoly->init(L"FP", freepolybtn_init, freepolybtn_render, basicbtn_update, freepolybtn_event,
		shp::rect4f(-scw / 2.0f, sch / 2.0f - 220, -scw / 2.0f + 100,
			sch / 2.0f - 120));
	freepoly->param[3] = (int*)p;
	freepoly->param[4] = &behaveid_arr[0];

	/*
	   dbg << ", euclid = " << p->pfm.Fup << endl; DXBtn *euclid = (DXBtn *)
	   p->pfm._New(sizeof(DXBtn)); euclid->init(L"Euc", freepolybtn_init,
	   freepolybtn_render, basicbtn_update, freepolybtn_event,
	   shp::rect4f(-scw / 2.0f, sch / 2.0f - 340, -scw / 2.0f + 100, sch /
	   2.0f - 240)); euclid->param[3] = (int *)p; euclid->param[4] =
	   &behaveid_arr[1]; */

	dbg << ", textoutline = " << p->pfm.Fup << endl;
	DXBtn* textoutline = (DXBtn*)p->pfm._New(sizeof(DXBtn));
	textoutline->init(L"Tex", freepolybtn_init, freepolybtn_render, basicbtn_update,
		freepolybtn_event, shp::rect4f(-scw / 2.0f, sch / 2.0f - 340,
			-scw / 2.0f + 100, sch / 2.0f - 240));
	textoutline->param[3] = (int*)p;
	textoutline->param[4] = &behaveid_arr[1];

	dbg << ", makeobj = " << p->pfm.Fup << endl;
	DXBtn* makeobj = (DXBtn*)p->pfm._New(sizeof(DXBtn));
	makeobj->init(L"Obj", freepolybtn_init, freepolybtn_render, basicbtn_update, freepolybtn_event,
		shp::rect4f(-scw / 2.0f, sch / 2.0f - 460, -scw / 2.0f + 100, sch / 2.0f - 360));
	makeobj->param[3] = (int*)p;
	makeobj->param[4] = &behaveid_arr[2];

	/*
	   dbg << ", makespr = " << p->pfm.Fup << endl; DXBtn *makespr = (DXBtn
	   *) p->pfm._New(sizeof(DXBtn)); makespr->init(L"Spr", freepolybtn_init,
	   freepolybtn_render, basicbtn_update, freepolybtn_event,
	   shp::rect4f(-scw / 2.0f, sch / 2.0f - 700, -scw / 2.0f + 100, sch /
	   2.0f - 600)); makespr->param[3] = (int *)p; makespr->param[4] =
	   &behaveid_arr[4]; */

	   // ------*-------
	dbg << ", translate = " << p->pfm.Fup << endl;
	DXBtn* translate = (DXBtn*)p->pfm._New(sizeof(DXBtn));
	translate->init(L"Tr", freepolybtn_init, freepolybtn_render, basicbtn_update,
		translatebtn_event, shp::rect4f(-scw / 2.0f, sch / 2.0f - 220,
			-scw / 2.0f + 100, sch / 2.0f - 120));

	dbg << ", scale = " << p->pfm.Fup << endl;
	DXBtn* scale = (DXBtn*)p->pfm._New(sizeof(DXBtn));
	scale->init(L"Sca", freepolybtn_init, freepolybtn_render, basicbtn_update, translatebtn_event,
		shp::rect4f(-scw / 2.0f, sch / 2.0f - 340, -scw / 2.0f + 100, sch / 2.0f - 240));

	dbg << ", objselect = " << p->pfm.Fup << endl;
	DXBtn* objselect = (DXBtn*)p->pfm._New(sizeof(DXBtn));
	objselect->init(L"Cam", objselectbtn_init, objselectbtn_render, basicbtn_update,
		objselectbtn_event, shp::rect4f(-scw / 2.0f, sch / 2.0f - 460,
			-scw / 2.0f + 100, sch / 2.0f - 360));

	dbg << ", rotation = " << p->pfm.Fup << endl;
	DXBtn* rotation = (DXBtn*)p->pfm._New(sizeof(DXBtn));
	rotation->init(L"Rot", freepolybtn_init, freepolybtn_render, basicbtn_update,
		translatebtn_event, shp::rect4f(-scw / 2.0f, sch / 2.0f - 580,
			-scw / 2.0f + 100, sch / 2.0f - 480));

	dbg << ", selection = " << p->pfm.Fup << endl;
	DXBtn* selection = (DXBtn*)p->pfm._New(sizeof(DXBtn));
	selection->init(L"Sel", freepolybtn_init, freepolybtn_render, basicbtn_update,
		translatebtn_event, shp::rect4f(-scw / 2.0f, sch / 2.0f - 700,
			-scw / 2.0f + 100, sch / 2.0f - 600));

	dbg << ", setcolor = " << p->pfm.Fup << endl;
	DXBtn* setcolor = (DXBtn*)p->pfm._New(sizeof(DXBtn));
	setcolor->init(L"col", freepolybtn_init, freepolybtn_render, basicbtn_update,
		setcolorbtn_event, shp::rect4f(-scw / 2.0f, sch / 2.0f - 820, -scw / 2.0f + 100,
			sch / 2.0f - 720));

	translate->param[4] = scale->param[2];
	translate->param[5] = rotation->param[2];
	translate->param[6] = selection->param[2];
	translate->param[7] = setcolor->param[2];

	scale->param[4] = translate->param[2];
	scale->param[5] = rotation->param[2];
	scale->param[6] = selection->param[2];
	scale->param[7] = setcolor->param[2];

	rotation->param[4] = scale->param[2];
	rotation->param[5] = translate->param[2];
	rotation->param[6] = selection->param[2];
	rotation->param[7] = setcolor->param[2];

	selection->param[4] = scale->param[2];
	selection->param[5] = rotation->param[2];
	selection->param[6] = translate->param[2];
	selection->param[7] = setcolor->param[2];

	setcolor->param[4] = scale->param[2];
	setcolor->param[5] = rotation->param[2];
	setcolor->param[6] = selection->param[2];
	setcolor->param[7] = translate->param[2];

	dbg << ", behave_selected = " << p->pfm.Fup << endl;
	int* behave_selected = (int*)p->pfm._New(sizeof(int));
	*behave_selected = -1;
	// -1 : none
	// 0 : freepoly
	// 1 : euclidean construct
	// 2 : text outline
	// 3 : add obj
	// 4 : add spr

	dbg << ", save = " << p->pfm.Fup << endl;
	DXBtn* save = (DXBtn*)p->pfm._New(sizeof(DXBtn));
	save->init(L"save", freepolybtn_init, freepolybtn_render, basicbtn_update, savebtn_event,
		shp::rect4f(-scw / 2.0f, -sch / 2.0f, -scw / 2.0f + 100, -sch / 2.0f + 100));
	save->param[3] = (int*)p;
	save->param[4] = &behaveid_arr[5];

	dbg << ", adding_polygon = " << p->pfm.Fup << endl;
	rbuffer* adding_polygon = (rbuffer*)p->pfm._New(sizeof(rbuffer));
	adding_polygon->Init(false);
	adding_polygon->begin();
	adding_polygon->set_inherit(true);

	dbg << ", present_color = " << p->pfm.Fup << endl;
	DX11Color* present_color = (DX11Color*)p->pfm._New(sizeof(DX11Color));
	*present_color = DX11Color(1, 1, 1, 1);

	dbg << ", select_rect = " << p->pfm.Fup << endl;
	shp::rect4f* select_rect = (shp::rect4f*)p->pfm._New(sizeof(shp::rect4f));
	*select_rect = shp::rect4f(0, 0, 0, 0);

	dbg << ", select_arr = " << p->pfm.Fup << endl;
	fmvecarr < pos_select_obj >* select_arr =
		(fmvecarr < pos_select_obj > *)p->pfm._New(sizeof(fmvecarr < pos_select_obj >));
	select_arr->NULLState();
	select_arr->Init(8, false, true);

	dbg << ", stacktime = " << p->pfm.Fup << endl;
	float* stacktime = (float*)p->pfm._New(sizeof(float));
	*stacktime = 0;

	dbg << ", selectclear = " << p->pfm.Fup << endl;
	DXBtn* selectclear = (DXBtn*)p->pfm._New(sizeof(DXBtn));
	selectclear->init(L"scl", freepolybtn_init, freepolybtn_render, basicbtn_update,
		selectclearbtn_event, shp::rect4f(-scw / 2.0f, -sch / 2.0f,
			-scw / 2.0f + 100, -sch / 2.0f + 100));
	selectclear->param[3] = (int*)select_arr;

	dbg << ", opencolorpage = " << p->pfm.Fup << endl;
	DXBtn* opencolorpage = (DXBtn*)p->pfm._New(sizeof(DXBtn));
	opencolorpage->init(L"C", basicbtn_init, basicbtn_render, basicbtn_update,
		opencolorpagebtn_event, shp::rect4f(-scw / 2.0f + 200, sch / 2.0f - 100.0f,
			-scw / 2.0f + 300, sch / 2.0f));


	dbg << ", openobjlist = " << p->pfm.Fup << endl;
	DXBtn* openobjlist = (DXBtn*)p->pfm._New(sizeof(DXBtn));
	openobjlist->init(L"<", basicbtn_init, basicbtn_render, basicbtn_update, openobjlistbtn_event,
		shp::rect4f(scw / 2.0f - 100, -sch / 2.0f, scw / 2.0f, -sch / 2.0f + 100));

	dbg << ", showobjlist = " << p->pfm.Fup << endl;
	bool* showobjlist = (bool*)p->pfm._New(sizeof(bool));
	*showobjlist = false;

	dbg << ", closeobjlist = " << p->pfm.Fup << endl;
	DXBtn* closeobjlist = (DXBtn*)p->pfm._New(sizeof(DXBtn));
	closeobjlist->init(L">", basicbtn_init, basicbtn_render, basicbtn_update,
		closeobjlistbtn_event, shp::rect4f(scw / 2.0f - 500, -sch / 2.0f,
			scw / 2.0f - 400, -sch / 2.0f + 100));

	openobjlist->param[2] = (int*)showobjlist;
	closeobjlist->param[2] = (int*)showobjlist;

	dbg << ", savefilestr = " << p->pfm.Fup << endl;
	wchar_t* savefilestr = (wchar_t*)p->pfm._New(sizeof(wchar_t) * 255);
	savefilestr[0] = 0;

	dbg << "};" << endl;
}

bool first = true;

void main_render(Page* p)
{
	scwh = shp::vec2f((float)scw * zoomrate, (float)sch * zoomrate);
	// dbgcount(0, vertexdrt->dbg_rbuffer())
	float* stacktime = (float*)&p->pfm.Data[(int)mainpm::stacktime];
	shp::vec2f* pc = (shp::vec2f*)&p->pfm.Data[(int)mainpm::present_center];
	DXBtn* behavetop = (DXBtn*)&p->pfm.Data[(int)mainpm::behavetop];
	shp::vec2i* behavemod = (shp::vec2i*)behavetop->param[2];
	DXBtn* ocp = (DXBtn*)&p->pfm.Data[(int)mainpm::opencolorpage];

	// obj redering_2d
	XMVECTOR Eye = XMVectorSet(pc->x, pc->y, -5.0f, 0.0f);
	XMVECTOR At = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	CamView = XMMatrixLookAtLH(Eye, At, Up);

	float farZ = 1000.0f;
	float nearZ = 0.1f;
	CamProj = XMMATRIX(
		2.0f / (float)width * zoomrate, 0, 0, 0,
		0, 2.0f / (float)height * zoomrate, 0, 0,
		0, 0, 1.0f / (farZ - nearZ), 0,
		0, 0, 1.0f / (nearZ - farZ), 1);

	float d = 50.0f * (int)(zoomrate);
	float lw = 1.0f;
	float alpha = 0.2f;
	drawline(shp::vec2f(pc->x - scwh.x, 0), shp::vec2f(pc->x + scwh.x, 0), 4.0f * zoomrate,
		DX11Color(1, 1, 1, 1));
	drawline(shp::vec2f(0, pc->y - scwh.y), shp::vec2f(0, pc->y + scwh.y), 4.0f * zoomrate,
		DX11Color(1, 1, 1, 1));
	for (int k = 0; k < 3; ++k)
	{
		// dbg << "scwh.x : " << scwh.x << endl;

		int ax = (pc->y - scwh.y) / d;
		int max = (pc->y + scwh.y) / d;
		for (; ax < max; ++ax)
		{
			drawline(shp::vec2f(pc->x - scwh.x, (float)ax * d),
				shp::vec2f(pc->x + scwh.x, (float)ax * d), lw * zoomrate, DX11Color(1, 1, 1, alpha));
		}
		ax = (pc->x - scwh.x) / d;
		max = (pc->x + scwh.x) / d;
		for (; ax < max; ++ax)
		{
			drawline(shp::vec2f((float)ax * d, pc->y - scwh.y),
				shp::vec2f((float)ax * d, pc->y + scwh.y), lw * zoomrate, DX11Color(1, 1, 1, alpha));
		}
		d = d * 2.0f;
		lw = lw + 1.0f;
		alpha *= 2.0f;
	}

	ConstantBuffer cb = GetBasicModelCB(shp::vec3f(0, 0, 0), shp::vec3f(0, 0, 0), shp::vec3f(1, 1, 1), DX11Color(1.0f, 1.0f, 1.0f, 1.0f));
	linedrt->render(cb);

	int* behave_selected = (int*)&p->pfm.Data[(int)mainpm::behave_selected];
	// dbgcount(0, dbg << "try render sprite" << endl);
	if (mainSprite != nullptr)
	{
		mainSprite->render(cb);
		if (*behave_selected == 0
			&& (mainSprite->st == sprite_type::st_freepolygon
				&& mainSprite->data.freepoly != nullptr))
		{
			rbuffer* ap = (rbuffer*)mainSprite->data.freepoly;
			// dbgcount(0, ap->dbg_rbuffer());
			// ap->render();
			shp::vec3f savpos = *(shp::vec3f*)&ap->buffer[ap->get_choice()]->Arr[4];
			fmvecarr < pos_select_obj >* sarr =
				(fmvecarr < pos_select_obj > *) & p->pfm.Data[(int)mainpm::select_arr];

			for (int i = 0; i < ap->get_vertexsiz(ap->get_choice()); ++i)
			{
				shp::vec3f pos =
					*(shp::vec3f*)&ap->buffer[ap->get_choice()]->Arr[sizeof(SimpleVertex) * i + 4];
				ConstantBuffer normalCB = GetBasicModelCB(pos, shp::vec3f(0, 0, 0), shp::vec3f(2 * zoomrate, 2 * zoomrate, 1), DX11Color(0, 1.0f, 1.0f, 0.5f));
				ConstantBuffer selectCB = GetBasicModelCB(pos, shp::vec3f(0, 0, *stacktime * shp::PI), 
					shp::vec3f(zoomrate * (3.0f + 1.0f * sinf(*stacktime * 3.0f)), 
					zoomrate * (3.0f + 1.0f * sinf(*stacktime * 3.0f)), 1) , 
					DX11Color(1.0f, 0, 1.0f, 1.0f));
				bool BeSelect = false;
				for (int k = 0; k < sarr->size(); ++k)
				{
					if (sarr->at(k).index == i)
					{
						linedrt->render(selectCB);
						BeSelect = true;
						break;
					}
				}

				if (!BeSelect) linedrt->render(normalCB);

				drawline(shp::vec2f(savpos.x, savpos.y), shp::vec2f(pos.x, pos.y), 4 * zoomrate,
					DX11Color(1, 1, 1, 1));
				savpos = pos;
			}
			shp::vec3f initpos = *(shp::vec3f*)&ap->buffer[ap->get_choice()]->Arr[4];
			drawline(shp::vec2f(initpos.x, initpos.y), shp::vec2f(savpos.x, savpos.y),
				4 * zoomrate, DX11Color(1, 1, 1, 1));
		}
	}


	shp::rect4f* selectrt = (shp::rect4f*)&p->pfm.Data[(int)mainpm::select_rect];
	drawline(shp::vec2f(selectrt->fx, selectrt->fy), shp::vec2f(selectrt->fx, selectrt->ly),
		4 * zoomrate, DX11Color(1, 1, 1, 1));
	drawline(shp::vec2f(selectrt->fx, selectrt->ly), shp::vec2f(selectrt->lx, selectrt->ly),
		4 * zoomrate, DX11Color(1, 1, 1, 1));
	drawline(shp::vec2f(selectrt->lx, selectrt->ly), shp::vec2f(selectrt->lx, selectrt->fy),
		4 * zoomrate, DX11Color(1, 1, 1, 1));
	drawline(shp::vec2f(selectrt->lx, selectrt->fy), shp::vec2f(selectrt->fx, selectrt->fy),
		4 * zoomrate, DX11Color(1, 1, 1, 1));

	// ui rendering

	behavetop->Render();
	ocp->Render();

	DXBtn* openobjlist = (DXBtn*)&p->pfm.Data[(int)mainpm::openobjlist];
	DXBtn* closeobjlist = (DXBtn*)&p->pfm.Data[(int)mainpm::closeobjlist];
	bool* showobjlist = (bool*)&p->pfm.Data[(int)mainpm::showobjlist];

	if (*showobjlist)
	{
		closeobjlist->Render();
	}
	else
	{
		openobjlist->Render();
	}

	if (behavemod->x == 0)
	{
		DXBtn* freepoly = (DXBtn*)&p->pfm.Data[(int)mainpm::freepoly];
		// DXBtn *euclid = (DXBtn *) & p->pfm.Data[(int)mainpm::euclid];
		DXBtn* textoutline = (DXBtn*)&p->pfm.Data[(int)mainpm::textoutline];
		DXBtn* makeobj = (DXBtn*)&p->pfm.Data[(int)mainpm::makeobj];
		// DXBtn *makespr = (DXBtn *) & p->pfm.Data[(int)mainpm::makespr];

		freepoly->Render();
		// euclid->Render();
		textoutline->Render();
		makeobj->Render();
		// makespr->Render();

		if (*behave_selected >= 0)
		{
			DXBtn* save = (DXBtn*)&p->pfm.Data[(int)mainpm::save];
			save->Render();
		}
	}
	else
	{
		DXBtn* translate = (DXBtn*)&p->pfm.Data[(int)mainpm::translate];
		DXBtn* scale = (DXBtn*)&p->pfm.Data[(int)mainpm::scale];
		DXBtn* objselect = (DXBtn*)&p->pfm.Data[(int)mainpm::objselect];

		translate->Render();
		scale->Render();
		objselect->Render();

		shp::vec2i* objmod = (shp::vec2i*)objselect->param[2];
		if (objmod->x == 1)
		{
			DXBtn* rotation = (DXBtn*)&p->pfm.Data[(int)mainpm::rotation];
			DXBtn* selection = (DXBtn*)&p->pfm.Data[(int)mainpm::selection];
			DXBtn* setcolor = (DXBtn*)&p->pfm.Data[(int)mainpm::setcolor];

			rotation->Render();
			selection->Render();
			setcolor->Render();

			fmvecarr < pos_select_obj >* sarr =
				(fmvecarr < pos_select_obj > *) & p->pfm.Data[(int)mainpm::select_arr];
			if (sarr->size() > 0)
			{
				DXBtn* selectclear = (DXBtn*)&p->pfm.Data[(int)mainpm::selectclear];
				selectclear->Render();
			}
		}
	}
}

void main_update(Page* p, float delta)
{
	float* stacktime = (float*)&p->pfm.Data[(int)mainpm::stacktime];
	*stacktime += delta;
	DXBtn* behavetop = (DXBtn*)&p->pfm.Data[(int)mainpm::behavetop];
	shp::vec2i* behavemod = (shp::vec2i*)behavetop->param[2];
	DXBtn* ocp = (DXBtn*)&p->pfm.Data[(int)mainpm::opencolorpage];
	DXBtn* openobjlist = (DXBtn*)&p->pfm.Data[(int)mainpm::openobjlist];
	DXBtn* closeobjlist = (DXBtn*)&p->pfm.Data[(int)mainpm::closeobjlist];
	bool* showobjlist = (bool*)&p->pfm.Data[(int)mainpm::showobjlist];
	wchar_t* savefilestr = (wchar_t*)&p->pfm.Data[(int)mainpm::savefilestr];
	if (savefilestr[0] != 0) {
		//save mainsprite with name
	}

	if (*showobjlist)
	{
		closeobjlist->Update(delta);
	}
	else
	{
		openobjlist->Update(delta);
	}

	behavetop->Update(delta);
	ocp->Update(delta);
	if (behavemod->x == 0)
	{
		DXBtn* freepoly = (DXBtn*)&p->pfm.Data[(int)mainpm::freepoly];
		// DXBtn *euclid = (DXBtn *) & p->pfm.Data[(int)mainpm::euclid];
		DXBtn* textoutline = (DXBtn*)&p->pfm.Data[(int)mainpm::textoutline];
		DXBtn* makeobj = (DXBtn*)&p->pfm.Data[(int)mainpm::makeobj];
		// DXBtn *makespr = (DXBtn *) & p->pfm.Data[(int)mainpm::makespr];

		freepoly->Update(delta);
		// euclid->Update(delta);
		textoutline->Update(delta);
		makeobj->Update(delta);
		// makespr->Update(delta);

		int* behave_selected = (int*)&p->pfm.Data[(int)mainpm::behave_selected];
		if (*behave_selected >= 0)
		{
			DXBtn* save = (DXBtn*)&p->pfm.Data[(int)mainpm::save];
			save->Update(delta);
		}
	}
	else
	{
		DXBtn* translate = (DXBtn*)&p->pfm.Data[(int)mainpm::translate];
		DXBtn* scale = (DXBtn*)&p->pfm.Data[(int)mainpm::scale];
		DXBtn* objselect = (DXBtn*)&p->pfm.Data[(int)mainpm::objselect];

		translate->Update(delta);
		scale->Update(delta);
		objselect->Update(delta);

		shp::vec2i* objmod = (shp::vec2i*)objselect->param[2];
		if (objmod->x == 1)
		{
			DXBtn* rotation = (DXBtn*)&p->pfm.Data[(int)mainpm::rotation];
			DXBtn* selection = (DXBtn*)&p->pfm.Data[(int)mainpm::selection];
			DXBtn* setcolor = (DXBtn*)&p->pfm.Data[(int)mainpm::setcolor];

			rotation->Update(delta);
			selection->Update(delta);
			setcolor->Update(delta);

			fmvecarr < pos_select_obj >* sarr =
				(fmvecarr < pos_select_obj > *) & p->pfm.Data[(int)mainpm::select_arr];
			if (sarr->size() > 0)
			{
				DXBtn* selectclear = (DXBtn*)&p->pfm.Data[(int)mainpm::selectclear];
				selectclear->Update(delta);
			}
		}
	}
}

void main_event(Page* p, DX_Event evt)
{
	shp::vec2f* present_center = (shp::vec2f*)&p->pfm.Data[(int)mainpm::present_center];
	bool* finger_pressed = (bool*)&p->pfm.Data[(int)mainpm::finger_pressed];
	shp::vec2f* presspos = (shp::vec2f*)&p->pfm.Data[(int)mainpm::presspos];
	shp::vec2f* centerorigin = (shp::vec2f*)&p->pfm.Data[(int)mainpm::centerorigin];
	DXBtn* behavetop = (DXBtn*)&p->pfm.Data[(int)mainpm::behavetop];
	DXBtn* objselect = (DXBtn*)&p->pfm.Data[(int)mainpm::objselect];
	shp::vec2i* behavemod = (shp::vec2i*)behavetop->param[2];
	shp::vec2i* objmod = (shp::vec2i*)objselect->param[2];
	DXBtn* ocp = (DXBtn*)&p->pfm.Data[(int)mainpm::opencolorpage];

	int* behave_selected = (int*)&p->pfm.Data[(int)mainpm::behave_selected];
	behavetop->Event(evt);
	ocp->Event(evt);

	DXBtn* openobjlist = (DXBtn*)&p->pfm.Data[(int)mainpm::openobjlist];
	DXBtn* closeobjlist = (DXBtn*)&p->pfm.Data[(int)mainpm::closeobjlist];
	bool* showobjlist = (bool*)&p->pfm.Data[(int)mainpm::showobjlist];

	if (*showobjlist)
	{
		closeobjlist->Event(evt);
	}
	else
	{
		openobjlist->Event(evt);
	}

	if (behavemod->x == 0)
	{
		DXBtn* freepoly = (DXBtn*)&p->pfm.Data[(int)mainpm::freepoly];
		// DXBtn *euclid = (DXBtn *) & p->pfm.Data[(int)mainpm::euclid];
		DXBtn* textoutline = (DXBtn*)&p->pfm.Data[(int)mainpm::textoutline];
		DXBtn* makeobj = (DXBtn*)&p->pfm.Data[(int)mainpm::makeobj];
		// DXBtn *makespr = (DXBtn *) & p->pfm.Data[(int)mainpm::makespr];
		DXBtn* save = (DXBtn*)&p->pfm.Data[(int)mainpm::save];

		switch (*behave_selected)
		{
		case -1:
			freepoly->Event(evt);
			// euclid->Event(evt);
			textoutline->Event(evt);
			makeobj->Event(evt);
			// makespr->Event(evt);
			break;
		case 0:
			freepoly->Event(evt);
			save->Event(evt);
			break;
		case 1:
			textoutline->Event(evt);
			save->Event(evt);
			break;
		case 2:
			makeobj->Event(evt);
			save->Event(evt);
			break;
		}
	}
	else
	{
		DXBtn* translate = (DXBtn*)&p->pfm.Data[(int)mainpm::translate];
		DXBtn* scale = (DXBtn*)&p->pfm.Data[(int)mainpm::scale];
		DXBtn* objselect = (DXBtn*)&p->pfm.Data[(int)mainpm::objselect];

		translate->Event(evt);
		scale->Event(evt);
		objselect->Event(evt);

		if (objmod->x == 1)
		{
			DXBtn* rotation = (DXBtn*)&p->pfm.Data[(int)mainpm::rotation];
			DXBtn* selection = (DXBtn*)&p->pfm.Data[(int)mainpm::selection];
			DXBtn* setcolor = (DXBtn*)&p->pfm.Data[(int)mainpm::setcolor];

			rotation->Event(evt);
			selection->Event(evt);
			setcolor->Event(evt);

			fmvecarr < pos_select_obj >* sarr =
				(fmvecarr < pos_select_obj > *) & p->pfm.Data[(int)mainpm::select_arr];
			if (sarr->size() > 0)
			{
				DXBtn* selectclear = (DXBtn*)&p->pfm.Data[(int)mainpm::selectclear];
				selectclear->Event(evt);
			}
		}
	}

	if (evt.wParam == WM_LBUTTONDOWN)
	{
		*finger_pressed = true;
		//int n = SDL_GetNumTouchFingers(GetMousePos_notcenter(evt.lParam).touchId);
		shp::vec2f mpos = GetMousePos(evt.lParam);
		
		*presspos = shp::vec2f(mpos.x, mpos.y);
		shp::vec2f viewpos =
			shp::vec2f(present_center->x + GetMousePos_notcenter(evt.lParam).x * scwh.x - scwh.x / 2.0f,
				present_center->y - GetMousePos_notcenter(evt.lParam).y * scwh.y + scwh.y / 2.0f);
		*centerorigin = *present_center;

		// addpoint in freepoly
		if ((behavemod->x == 0 && *behave_selected == 0)
			&& (press_ef
				&& (mainSprite != nullptr
					&& (mainSprite->st == sprite_type::st_freepolygon
						&& mainSprite->data.freepoly != nullptr))))
		{
			press_ef = false;
			rbuffer* ap = (rbuffer*)mainSprite->data.freepoly;
			DX11Color presentcolor = *(DX11Color*)&p->pfm.Data[(int)mainpm::present_color];
			ap->set_inherit(true);
			ap->begin();
			ap->av(SimpleVertex(shp::vec3f(viewpos.x, viewpos.y, 0), presentcolor));
			ap->end();
		}

		// select point in freepoly
		DXBtn* selection = (DXBtn*)&p->pfm.Data[(int)mainpm::selection];
		bool* selection_enable = (bool*)selection->param[2];
		if ((behavemod->x == 1 && *behave_selected == 0) && (objmod->x == 1 && *selection_enable))
		{
			shp::rect4f* rt = (shp::rect4f*)&p->pfm.Data[(int)mainpm::select_rect];
			rt->fx = viewpos.x;
			rt->fy = viewpos.y;
		}
	}
	else if (evt.wParam == WM_LBUTTONUP)
	{
		if (*finger_pressed)
		{
			shp::vec2f mpos = GetMousePos(evt.lParam);
			
			shp::vec2f viewpos =
				shp::vec2f(present_center->x + GetMousePos_notcenter(evt.lParam).x * scwh.x - scwh.x / 2.0f,
					present_center->y - GetMousePos_notcenter(evt.lParam).y * scwh.y + scwh.y / 2.0f);
			shp::vec2f dv = shp::vec2f(mpos.x - presspos->x, mpos.y - presspos->y);
			DXBtn* translate = (DXBtn*)&p->pfm.Data[(int)mainpm::translate];
			DXBtn* scale = (DXBtn*)&p->pfm.Data[(int)mainpm::scale];
			if (behavemod->x == 1 && objmod->x == 0)
			{
				if (*(bool*)translate->param[2])
				{
					*present_center = *centerorigin - dv.operator*(zoomrate);
				}
				if (*(bool*)scale->param[2])
				{
					if (dv.y > 0)
					{
						zoomrate = 0.01f + zoomrate * 1.01f;
					}
					else if (dv.y < 0)
					{
						zoomrate = 0.01f + zoomrate * 0.99f;
						if (zoomrate < 0.01f)
							zoomrate = 0.01f;
					}
				}
			}

			*finger_pressed = false;
		}

		// select point in freepoly
		DXBtn* selection = (DXBtn*)&p->pfm.Data[(int)mainpm::selection];
		bool* selection_enable = (bool*)selection->param[2];
		fmvecarr < pos_select_obj >* sarr =
			(fmvecarr < pos_select_obj > *) & p->pfm.Data[(int)mainpm::select_arr];
		if (((behavemod->x == 1 && *behave_selected == 0) && (objmod->x == 1 && *selection_enable))
			&& (mainSprite->st == sprite_type::st_freepolygon
				&& mainSprite->data.freepoly != nullptr))
		{
			shp::rect4f* rt = (shp::rect4f*)&p->pfm.Data[(int)mainpm::select_rect];
			if (rt->fx > rt->lx)
			{
				float f = rt->fx;
				rt->fx = rt->lx;
				rt->lx = f;
			}
			if (rt->fy > rt->ly)
			{
				float f = rt->fy;
				rt->fy = rt->ly;
				rt->ly = f;
			}

			// add selection points in freepolygon
			rbuffer* ap = (rbuffer*)mainSprite->data.freepoly;
			for (int i = 0; i < ap->get_vertexsiz(ap->get_choice()); ++i)
			{
				shp::vec3f pos =
					*(shp::vec3f*)&ap->buffer[ap->get_choice()]->Arr[sizeof(SimpleVertex) * i + 4];
				DX11Color col =
					*(DX11Color*)&ap->buffer[ap->get_choice()]->Arr[sizeof(SimpleVertex) * i];
				shp::vec2f p2 = shp::vec2f(pos.x, pos.y);
				pos_select_obj pso;
				pso.index = i;
				pso.origin_pos = pos;
				pso.origin_color = col;
				if (shp::bPointInRectRange(p2, *rt))
				{
					sarr->push_back(pso);
				}
			}

			*rt = shp::rect4f(0, 0, 0, 0);
		}

		// sav selection position
		DXBtn* translate = (DXBtn*)&p->pfm.Data[(int)mainpm::translate];
		if ((mainSprite != nullptr && mainSprite->st == sprite_type::st_freepolygon)
			&& ((behavemod->x == 1 && *behave_selected == 0)
				&& (objmod->x == 1 && *(bool*)translate->param[2])))
		{
			rbuffer* ap = (rbuffer*)mainSprite->data.freepoly;
			fmvecarr<SimpleVertex>* bptr = ap->buffer[ap->get_choice()];
			for (int i = 0; i < sarr->size(); ++i)
			{
				sarr->at(i).origin_pos =
					*(shp::vec3f*)&bptr->at(sarr->at(i).index * sizeof(SimpleVertex) + 4);
			}
		}
	}
	else if (evt.wParam == WM_MOUSEMOVE)
	{
		if (*finger_pressed)
		{
			shp::vec2f mpos = GetMousePos(evt.lParam);
			
			shp::vec2f viewpos =
				shp::vec2f(present_center->x + GetMousePos_notcenter(evt.lParam).x * scwh.x - scwh.x / 2.0f,
					present_center->y - GetMousePos_notcenter(evt.lParam).y * scwh.y + scwh.y / 2.0f);
			shp::vec2f dv = shp::vec2f(mpos.x - presspos->x, mpos.y - presspos->y);
			// camera move
			DXBtn* translate = (DXBtn*)&p->pfm.Data[(int)mainpm::translate];
			DXBtn* scale = (DXBtn*)&p->pfm.Data[(int)mainpm::scale];
			if (behavemod->x == 1 && objmod->x == 0)
			{
				if (*(bool*)translate->param[2])
				{
					*present_center = *centerorigin - dv.operator*(zoomrate);
				}
				if (*(bool*)scale->param[2])
				{
					if (dv.y > 0)
					{
						zoomrate = 0.01f + zoomrate * 1.01f;
					}
					else if (dv.y < 0)
					{
						zoomrate = 0.01f + zoomrate * 0.99f;
						if (zoomrate < 0.01f)
							zoomrate = 0.01f;
					}
				}
			}

			// freepoly vertex location change
			DXBtn* selection = (DXBtn*)&p->pfm.Data[(int)mainpm::selection];
			bool* selection_enable = (bool*)selection->param[2];
			fmvecarr < pos_select_obj >* sarr =
				(fmvecarr < pos_select_obj > *) & p->pfm.Data[(int)mainpm::select_arr];
			// DXBtn *translate = (DXBtn *) &
			// p->pfm.Data[(int)mainpm::translate];
			if (((behavemod->x == 1 && *behave_selected == 0)
				&& (objmod->x == 1 && *(bool*)translate->param[2]))
				&& (mainSprite->st == sprite_type::st_freepolygon
					&& mainSprite->data.freepoly != nullptr))
			{
				fm->_tempPushLayer();
				rbuffer* ap = (rbuffer*)mainSprite->data.freepoly;
				fmvecarr<SimpleVertex>* bptr = ap->buffer[ap->get_choice()];
				SimpleVertex* farr = (SimpleVertex*)fm->_tempNew(bptr->size() * sizeof(SimpleVertex));
				for (int i = 0; i < bptr->size(); ++i)
				{
					farr[i] = bptr->at(i);
					// dbgcount(0, dbg << farr[i] << endl)
				}
				int vsiz = ap->get_vertexsiz(ap->get_choice());
				// dbgcount(0, dbg << "ap vertex size : " <<
				// ap->get_vertexsiz(ap->get_choice()) << endl)
				//ap->clear();
				ap->begin();
				for (int i = 0; i < vsiz; ++i)
				{
					bool isSelect = false;
					// dbgcount(0, dbg << "sarr size : " << sarr->size()
					// << endl)
					for (int k = 0; k < sarr->size(); ++k)
					{
						if (sarr->at(k).index == i)
						{
							// 
							isSelect = true;
							shp::vec3f pos;
							pos_select_obj pso = sarr->at(k);
							pos.x = pso.origin_pos.x + zoomrate * dv.x;
							pos.y = pso.origin_pos.y + zoomrate * dv.y;
							pos.z = pso.origin_pos.z;
							ap->av(SimpleVertex(pos, pso.origin_color));
							// dbgcount(0, dbg << "pos : " << pos.x << ",
							// " << pos.y << ", " << pos.z << endl) 
							break;
						}
					}

					if (isSelect == false)
					{
						shp::vec3f p = *(shp::vec3f*)&farr[sizeof(SimpleVertex) * i + 4];
						DX11Color c = *(DX11Color*)&farr[sizeof(SimpleVertex) * i];
						ap->av(SimpleVertex(p, c));
					}
				}
				ap->end();
				fm->_tempPopLayer();
			}

			// select point in freepoly
			if ((behavemod->x == 1 && *behave_selected == 0)
				&& (objmod->x == 1 && *selection_enable))
			{
				shp::rect4f* rt = (shp::rect4f*)&p->pfm.Data[(int)mainpm::select_rect];
				rt->lx = viewpos.x;
				rt->ly = viewpos.y;
			}
		}
	}
}

void basicslider_init(DXSlider* slider)
{
}

void basicslider_render(DXSlider* slider)
{
	shp::vec2f cen = slider->sup()->loc.getCenter();
	shp::vec2f wh = shp::vec2f(slider->sup()->loc.getw() / 2, slider->sup()->loc.geth() / 2);
	//GLuint shader = linedrt->get_shader();
	shp::rect4f bar;
	if (slider->horizontal)
	{
		bar = shp::rect4f(cen.x - wh.x, cen.y - wh.y / 3, cen.x + wh.x, cen.y + wh.y / 3);
	}
	else
	{
		bar = shp::rect4f(cen.x - wh.x / 3, cen.y - wh.y, cen.x + wh.x / 3, cen.y + wh.y);
	}

	ConstantBuffer cb = GetBasicModelCB(shp::vec3f(bar.getCenter().x, bar.getCenter().y, 0), shp::vec3f(0, 0, 0),
		shp::vec3f(bar.getw(), bar.geth(), 1), DX11Color(1.0f, 1.0f, 1.0f, 0.5f));
	DX11Color col = DX11Color(1, 1, 1, 0.5f);
	linedrt->render(cb);

	float rate = slider->setter / slider->max;
	shp::rect4f pos;
	shp::vec2f setp;
	if (slider->horizontal)
	{
		setp = shp::vec2f(cen.x - wh.x + rate * wh.x * 2, cen.y);
		pos = shp::rect4f(setp.x - wh.y, setp.y - wh.y, setp.x + wh.y, setp.y + wh.y);
	}
	else
	{
		setp = shp::vec2f(cen.x, cen.y + wh.y - rate * wh.y * 2);
		pos = shp::rect4f(setp.x - wh.x, setp.y - wh.x, setp.x + wh.x, setp.y + wh.x);
	}

	ConstantBuffer cb2 = GetBasicModelCB(shp::vec3f(pos.getCenter().x, pos.getCenter().y, 0), shp::vec3f(0, 0, shp::PI / 4.0f),
			shp::vec3f(pos.getw() * 0.7f, pos.geth() * 0.7f, 1), DX11Color(0.1f, 0.4f, 0.6f, 1.0f));
	linedrt->render(cb2);
}

void basicslider_update(DXSlider* slider, float delta)
{
}

void basicslider_event(DXSlider* slider, DX_Event evt)
{
	if (press_ef)
	{
		//int n = SDL_GetNumTouchFingers(GetMousePos_notcenter(evt.lParam).touchId);
		shp::vec2f mpos = GetMousePos(evt.lParam);
		
		shp::rect4f loc = slider->sup()->loc;
		if (shp::bPointInRectRange(shp::vec2f(mpos.x, mpos.y), loc))
		{
			if (slider->horizontal)
			{
				float mx = mpos.x;
				if (mx < loc.fx)
					mx = loc.fx;
				if (mx > loc.lx)
					mx = loc.lx;

				float v = (mx - loc.fx) / loc.getw();
				slider->setter = slider->max * v;
			}
			else
			{
				float mx = mpos.y;
				if (mx < loc.fy)
					mx = loc.fy;
				if (mx > loc.ly)
					mx = loc.ly;

				float v = (-mx + loc.ly) / loc.geth();
				slider->setter = slider->max * v;
			}
			switch (slider->mod)
			{
			case 'n':
				*reinterpret_cast <int*>(slider->obj) = slider->setter;
				break;
			case 'f':
				*reinterpret_cast <float*>(slider->obj) = slider->setter;
				break;
			}
		}
	}
}

void colorpage_donebtn_event(DXBtn* btn, DX_Event evt)
{
	shp::vec2f* flow = (shp::vec2f*)btn->param[1];

	if (evt.wParam == WM_LBUTTONDOWN)
	{
		shp::vec2f mpos = GetMousePos(evt.lParam);
		
		// dbg << "mpos : " << x << ", " << y << endl;
		if (shp::bPointInRectRange(shp::vec2f(mpos.x, mpos.y), btn->sup()->loc) && press_ef)
		{
			press_ef = false;
			flow->x = 0;
			// operate
			toppage -= 1;
			DX11Color* main_present_color =
				(DX11Color*)&mainpage->pfm.Data[(int)mainpm::present_color];
			DX11Color* presentcolor = (DX11Color*)&colorpage->pfm.Data[0];
			*main_present_color = *presentcolor;
		}
	}
}

void colorpage_init(Page* p)
{
	p->pfm.SetHeapData(new byte8[4096], 4096);

	dbg << "enum class colorpm{" << endl;
	dbg << "presentcolor = " << p->pfm.Fup << endl;
	DX11Color* presentcolor = (DX11Color*)p->pfm._New(sizeof(DX11Color));	// 0

	dbg << ", RSlider = " << p->pfm.Fup << endl;
	DXSlider* RSlider = (DXSlider*)p->pfm._New(sizeof(DXSlider));
	RSlider->init(1.0f, 'f', (int*)&presentcolor->r, shp::rect4f(-500, 450, 500, 550), basicslider_init, basicslider_render, basicslider_update, basicslider_event, true);	// 16

	dbg << ", GSlider = " << p->pfm.Fup << endl;
	DXSlider* GSlider = (DXSlider*)p->pfm._New(sizeof(DXSlider));
	GSlider->init(1.0f, 'f', (int*)&presentcolor->g, shp::rect4f(-500, 300, 500, 400), basicslider_init, basicslider_render, basicslider_update, basicslider_event, true);	// 200

	dbg << ", BSlider = " << p->pfm.Fup << endl;
	DXSlider* BSlider = (DXSlider*)p->pfm._New(sizeof(DXSlider));
	BSlider->init(1.0f, 'f', (int*)&presentcolor->b, shp::rect4f(-500, 150, 500, 250), basicslider_init, basicslider_render, basicslider_update, basicslider_event, true);	// 384

	dbg << ", ASlider = " << p->pfm.Fup << endl;
	DXSlider* ASlider = (DXSlider*)p->pfm._New(sizeof(DXSlider));
	ASlider->init(1.0f, 'f', (int*)&presentcolor->a, shp::rect4f(-500, 0, 500, 100), basicslider_init, basicslider_render, basicslider_update, basicslider_event, true);	// 568

	dbg << ", done = " << p->pfm.Fup << endl;
	DXBtn* done = (DXBtn*)p->pfm._New(sizeof(DXBtn));
	done->init(L"done", basicbtn_init, basicbtn_render, basicbtn_update, colorpage_donebtn_event, shp::rect4f(scw / 2.0f - 500, sch / 2.0f - 500, scw / 2.0f, sch / 2.0f));	// 752

	dbg << ", pallete = " << p->pfm.Fup << endl;
	DX11Color* pallete = (DX11Color*)p->pfm._New(sizeof(DX11Color) * 20 * 5);	// 10*5
	for (int i = 0; i < 100; ++i)
	{
		pallete[i] = DX11Color(1, 1, 1, 0.2f);
	}							// 1016

	dbg << ", selectnum = " << p->pfm.Fup << endl;
	int* selectnum = (int*)p->pfm._New(sizeof(int));
	*selectnum = 0;				// 2616

	dbg << "};" << endl;

	dbg << "total fup : " << p->pfm.Fup << endl;
}

void colorpage_render(Page* p)
{
	DX11Color* presentcolor = (DX11Color*)&p->pfm.Data[(int)colorpm::presentcolor];
	DXSlider* RSlider = (DXSlider*)&p->pfm.Data[(int)colorpm::RSlider];
	DXSlider* GSlider = (DXSlider*)&p->pfm.Data[(int)colorpm::GSlider];
	DXSlider* BSlider = (DXSlider*)&p->pfm.Data[(int)colorpm::BSlider];
	DXSlider* ASlider = (DXSlider*)&p->pfm.Data[(int)colorpm::ASlider];
	DXBtn* done = (DXBtn*)&p->pfm.Data[(int)colorpm::done];
	DX11Color* pallete = (DX11Color*)&p->pfm.Data[(int)colorpm::pallete];
	int* selectnum = (int*)&p->pfm.Data[(int)colorpm::selectnum];

	shp::rect4f scloc = shp::rect4f(-scw / 2.0f, -sch / 2.0f, scw / 2.0f, sch / 2.0f);
	//GLuint shader = linedrt->get_shader();
	ConstantBuffer cb = GetBasicModelCB(shp::vec3f(scloc.getCenter().x, scloc.getCenter().y, 0), shp::vec3f(0, 0, 0),
			shp::vec3f(scloc.getw(), scloc.geth(), 1), DX11Color(0, 0, 0, 0.4f));
	linedrt->render(cb);

	shp::rect4f pcloc = shp::rect4f(scloc.fx, scloc.ly - 500.0f, scloc.fx + 500.0f, scloc.ly);
	ConstantBuffer cb2 = GetBasicModelCB(shp::vec3f(pcloc.getCenter().x, pcloc.getCenter().y, 0), shp::vec3f(0, 0, 0),
			shp::vec3f(pcloc.getw(), pcloc.geth(), 1), DX11Color(0, 0, 0, 0.4f));
	linedrt->render(cb2);

	RSlider->render();
	GSlider->render();
	BSlider->render();
	ASlider->render();
	done->Render();

	shp::rect4f hscloc = shp::rect4f(-scw / 2.0f, -sch / 2.0f, scw / 2.0f, 0);
	shp::vec2f wh = shp::vec2f(hscloc.getw() / 20.0f, hscloc.geth() / 5.0f);
	float margin = 5.0f;
	for (int x = 0; x < 20; ++x)
	{
		for (int y = 0; y < 5; ++y)
		{
			shp::rect4f sloc =
				shp::rect4f(hscloc.fx + wh.x * x, hscloc.fy + wh.y * y, hscloc.fx + wh.x * (x + 1),
					hscloc.fy + wh.y * (y + 1));
			if (*selectnum == x * 5 + y)
			{	
				ConstantBuffer cb = GetBasicModelCB(shp::vec3f(sloc.getCenter().x, sloc.getCenter().y, 0),
					shp::vec3f(0, 0, 0), shp::vec3f(sloc.getw(), sloc.geth(), 1), DX11Color(1, 1, 1, 0.4f));
				linedrt->render(cb);
			}
			shp::rect4f cloc =
				shp::rect4f(sloc.fx + margin, sloc.fy + margin, sloc.lx - margin,
					sloc.ly - margin);
			ConstantBuffer cb = GetBasicModelCB(shp::vec3f(cloc.getCenter().x, cloc.getCenter().y, 0), shp::vec3f(0, 0, 0),
				shp::vec3f(cloc.getw(), cloc.geth(), 1), pallete[5 * x + y]);
			linedrt->render(cb);
		}
	}
}

void colorpage_update(Page* p, float delta)
{
	DXSlider* RSlider = (DXSlider*)&p->pfm.Data[(int)colorpm::RSlider];
	DXSlider* GSlider = (DXSlider*)&p->pfm.Data[(int)colorpm::GSlider];
	DXSlider* BSlider = (DXSlider*)&p->pfm.Data[(int)colorpm::BSlider];
	DXSlider* ASlider = (DXSlider*)&p->pfm.Data[(int)colorpm::ASlider];
	DXBtn* done = (DXBtn*)&p->pfm.Data[(int)colorpm::done];

	RSlider->update(delta);
	GSlider->update(delta);
	BSlider->update(delta);
	ASlider->update(delta);
	done->Update(delta);
}

void colorpage_event(Page* p, DX_Event evt)
{
	DX11Color* presentcolor = (DX11Color*)&p->pfm.Data[(int)colorpm::presentcolor];
	DXSlider* RSlider = (DXSlider*)&p->pfm.Data[(int)colorpm::RSlider];
	DXSlider* GSlider = (DXSlider*)&p->pfm.Data[(int)colorpm::GSlider];
	DXSlider* BSlider = (DXSlider*)&p->pfm.Data[(int)colorpm::BSlider];
	DXSlider* ASlider = (DXSlider*)&p->pfm.Data[(int)colorpm::ASlider];
	DXBtn* done = (DXBtn*)&p->pfm.Data[(int)colorpm::done];
	DX11Color* pallete = (DX11Color*)&p->pfm.Data[(int)colorpm::pallete];
	int* selectnum = (int*)&p->pfm.Data[(int)colorpm::selectnum];

	RSlider->Event(evt);
	GSlider->Event(evt);
	BSlider->Event(evt);
	ASlider->Event(evt);
	done->Event(evt);
	pallete[*selectnum] = *presentcolor;

	shp::rect4f hscloc = shp::rect4f(-scw / 2.0f, -sch / 2.0f, scw / 2.0f, 0);
	if (evt.wParam == WM_LBUTTONDOWN)
	{
		float mx = GetMousePos_notcenter(evt.lParam).x * scw - scw / 2.0f;
		float my = -GetMousePos_notcenter(evt.lParam).y * sch + sch / 2.0f;
		shp::vec2f wh = shp::vec2f(hscloc.getw() / 20.0f, hscloc.geth() / 5.0f);
		float margin = 5.0f;
		for (int x = 0; x < 20; ++x)
		{
			for (int y = 0; y < 5; ++y)
			{
				shp::rect4f sloc =
					shp::rect4f(hscloc.fx + wh.x * x, hscloc.fy + wh.y * y,
						hscloc.fx + wh.x * (x + 1), hscloc.fy + wh.y * (y + 1));
				if (shp::bPointInRectRange(shp::vec2f(mx, my), sloc))
				{
					*selectnum = x * 5 + y;
					*presentcolor = pallete[*selectnum];
				}
			}
		}
	}
}

class texteditsys
{
public:
	texteditsys()
	{
	}
	~texteditsys()
	{
	}

	constexpr static int editstr_max = 1024;
	wchar_t editstr[editstr_max] = { };
	int esup = 0;
	bool editin = false;

	void keyin(wchar_t unicode)
	{
		if (esup + 1 < editstr_max)
		{
			editstr[esup] = unicode;
			editstr[esup + 1] = 0;
			esup += 1;
			hankey_arrange(&editstr[0]);
			esup = wcslen(editstr);
		}
	}

	void backspace()
	{
		editstr[esup - 1] = 0;
		esup -= 1;
	}

	void editreset()
	{
		esup = 0;
		editstr[0] = 0;
	}
};

texteditsys tes;

void keybtn_init(DXBtn* btn)
{
	// param 0 is page ptr
	btn->param[1] = (int*)fm->_New(sizeof(float), true);
	*(float*)btn->param[1] = 0;
}

void keybtn_update(DXBtn* btn, float delta)
{
	float* flow = (float*)btn->param[1];
	if (*flow >= 0)
	{
		*flow += delta;
	}
	if (*flow > 1)
	{
		*flow = -1.0f;
	}
}

void keybtn_render(DXBtn* btn)
{
	DX_UI* uip = btn->sup();
	float* flow = (float*)btn->param[1];
	float rate = AnimClass::EaseOut(*flow, 5);
	if (*flow < 0) rate = 0;

	if (*flow >= 0)
	{
		shp::rect4f chrt = uip->loc;
		chrt.fy = uip->loc.fy - 200.0f * AnimClass::EaseIn(*flow, 4);
		chrt.ly = uip->loc.ly - 200.0f * AnimClass::EaseOut(*flow / 2.0f, 4);
		drawline(shp::vec2f(chrt.fx, chrt.getCenter().y), shp::vec2f(chrt.lx, chrt.getCenter().y), chrt.geth(), DX11Color(1, 1, 1, 1.0f - *flow));
	}

	drawline(shp::vec2f(uip->loc.fx, uip->loc.getCenter().y), shp::vec2f(uip->loc.lx, uip->loc.getCenter().y), uip->loc.geth() * rate, DX11Color(0.2f, 0.3f, 0.3f, 0.5f));
	shp::rect4f tloc = uip->loc;
	tloc.fx = (tloc.getCenter().x + tloc.fx) / 2.0f;
	tloc.fy = (tloc.getCenter().y + tloc.fy) / 2.0f;
	draw_string(btn->text, wcslen(btn->text), 30, tloc, DX11Color(1.0f, 1.0f, 1.0f, 1.0f));
}

void keybtn_event(DXBtn* btn, DX_Event evt)
{
	bool keyinput = false;
	float* flow = (float*)btn->param[1];
	DXBtn* Vkeyboard = (DXBtn*)&texteditpage->pfm.Data[(int)texteditpm::VKeyboard];
	bool* isshift = (bool*)&texteditpage->pfm.Data[(int)texteditpm::isshift];
	bool* ishan = (bool*)&texteditpage->pfm.Data[(int)texteditpm::ishan];

	if (evt.wParam == WM_LBUTTONDOWN)
	{
		shp::vec2f mpos = GetMousePos(evt.lParam);
		
		// dbg << "mpos : " << x << ", " << y << endl;
		if (shp::bPointInRectRange(shp::vec2f(mpos.x, mpos.y), btn->sup()->loc) && press_ef)
		{
			press_ef = false;
			// operate
			keyinput = true;
		}
	}

	if (keyinput == false) return;
	*flow = 0.01f;
	if (wcscmp(btn->text, L"<]") == 0)
	{
		tes.backspace();
		return;
	}
	else if (wcscmp(btn->text, L"[_____]") == 0)
	{
		tes.keyin(L' ');
		return;
	}
	else if (wcscmp(btn->text, L"한/영") == 0)
	{
		*isshift = false;
		if (*ishan)
			*ishan = false;
		else
			*ishan = true;

		if (*ishan == false)
		{
			// eng
			for (int y = 0; y < 2; ++y)
			{
				for (int x = 0; x < 10; ++x)
				{
					wchar_t wstr[2];
					wstr[0] = key_sym[y][x];
					wstr[1] = 0;
					wcscpy(Vkeyboard[10 * y + x].text, wstr);
				}
			}

			for (int y = 2; y < 4; ++y)
			{
				float addx = y - 2;
				for (int x = 0; x < 9 - addx * 2; ++x)
				{
					wchar_t wstr[2];
					wstr[0] = key_sym[y][x];
					wstr[1] = 0;
					wcscpy(Vkeyboard[10 * y + x].text, wstr);
				}
			}
		}
		else
		{
			// han
			for (int y = 0; y < 2; ++y)
			{
				for (int x = 0; x < 10; ++x)
				{
					wchar_t wstr[2];
					wstr[0] = hankey_sym[y][x];
					wstr[1] = 0;
					wcscpy(Vkeyboard[10 * y + x].text, wstr);
				}
			}

			for (int y = 2; y < 4; ++y)
			{
				float addx = y - 2;
				for (int x = 0; x < 9 - addx * 2; ++x)
				{
					wchar_t wstr[2];
					wstr[0] = hankey_sym[y][x];
					wstr[1] = 0;
					wcscpy(Vkeyboard[10 * y + x].text, wstr);
				}
			}
		}
		return;
	}
	else if (wcscmp(btn->text, L"Shift") == 0)
	{
		if (*isshift)
			*isshift = false;
		else
			*isshift = true;

		if (*ishan == false)
		{
			// eng
			for (int y = 0; y < 2; ++y)
			{
				for (int x = 0; x < 10; ++x)
				{
					wchar_t wstr[2];
					if (*isshift)
					{
						wstr[0] = shift_key_sym[y][x];
					}
					else
					{
						wstr[0] = key_sym[y][x];
					}
					wstr[1] = 0;
					wcscpy(Vkeyboard[10 * y + x].text, wstr);
				}
			}

			for (int y = 2; y < 4; ++y)
			{
				float addx = y - 2;
				for (int x = 0; x < 9 - addx * 2; ++x)
				{
					wchar_t wstr[2];
					if (*isshift)
					{
						wstr[0] = shift_key_sym[y][x];
					}
					else
					{
						wstr[0] = key_sym[y][x];
					}
					wstr[1] = 0;
					wcscpy(Vkeyboard[10 * y + x].text, wstr);
				}
			}
		}
		else
		{
			// han
			for (int y = 0; y < 2; ++y)
			{
				for (int x = 0; x < 10; ++x)
				{
					wchar_t wstr[2];
					if (*isshift)
					{
						wstr[0] = shift_hankey_sym[y][x];
					}
					else
					{
						wstr[0] = hankey_sym[y][x];
					}
					wstr[1] = 0;
					wcscpy(Vkeyboard[10 * y + x].text, wstr);
				}
			}

			for (int y = 2; y < 4; ++y)
			{
				float addx = y - 2;
				for (int x = 0; x < 9 - addx * 2; ++x)
				{
					wchar_t wstr[2];
					if (*isshift)
					{
						wstr[0] = shift_hankey_sym[y][x];
					}
					else
					{
						wstr[0] = hankey_sym[y][x];
					}
					wstr[1] = 0;
					wcscpy(Vkeyboard[10 * y + x].text, wstr);
				}
			}
		}
		return;
	}
	else if (wcscmp(btn->text, L"Enter") == 0)
	{
		tes.editin = false;
		//save_edit_text(tes.editstr);
		wchar_t* str = (wchar_t*)texteditpage->pfm.Data[(int)texteditpm::deststring];
		wcscpy(str, tes.editstr);
		tes.editreset();
		toppage -= 1;
		return;
	}
	tes.keyin(btn->text[0]);
}

void texteditpage_init(Page* p)
{
	p->pfm.SetHeapData(new byte8[14336], 14336);

	dbg << "enum class texteditpm{" << endl;
	// params
	// init alphabets
	dbg << "VKeyboard = " << p->pfm.Fup << endl;
	DXBtn* Vkeyboard = (DXBtn*)p->pfm._New(sizeof(DXBtn) * 50);
	dbg << ", ishan = " << p->pfm.Fup << endl;
	bool* ishan = (bool*)p->pfm._New(sizeof(bool));
	dbg << ", isshift = " << p->pfm.Fup << endl;
	bool* isshift = (bool*)p->pfm._New(sizeof(bool));
	dbg << ", deststring = " << p->pfm.Fup << endl;
	wchar_t** deststring = (wchar_t**)p->pfm._New(sizeof(wchar_t*));

	shp::vec2f usiz = shp::vec2f(sch, scw);
	if (sch > scw) {
		usiz = shp::vec2f(scw, sch);
	}
	shp::rect4f scloc = shp::rect4f(-usiz.x / 2.0f, -usiz.y / 2.0f, usiz.x / 2.0f, usiz.y / 2.0f);
	shp::vec2f siz = shp::vec2f(usiz.x, (float)usiz.y * 15.0f / 16.0f);
	float starty = scloc.ly - (float)siz.y * 11.0f / 14.0f;
	siz.y = (float)siz.y * (5.0f / 14.0f);
	shp::vec2f wh = shp::vec2f((float)siz.x / 10.0f, (float)siz.y / 5.0f);

	for (int y = 0; y < 2; ++y)
	{
		for (int x = 0; x < 10; ++x)
		{
			wchar_t wstr[2];
			wstr[0] = hankey_sym[y][x];
			wstr[1] = 0;
			Vkeyboard[10 * y + x].init(wstr, keybtn_init, keybtn_render, keybtn_update, keybtn_event, shp::rect4f(scloc.fx + x * wh.x, starty - wh.y * y, scloc.fx + x * wh.x + wh.x, starty - wh.y * y + wh.y));
		}
	}

	for (int y = 2; y < 4; ++y)
	{
		float addx = y - 2;
		for (int x = 0; x < 9 - addx * 2; ++x)
		{
			wchar_t wstr[2];
			wstr[0] = hankey_sym[y][x];
			wstr[1] = 0;
			Vkeyboard[10 * y + x].init(wstr, keybtn_init, keybtn_render, keybtn_update, keybtn_event, shp::rect4f(scloc.fx + wh.x / 2 + x * wh.x + addx * wh.x, starty - wh.y * y, scloc.fx + wh.x / 2 + x * wh.x + wh.x + wh.x * addx, starty - wh.y * y + wh.y));
		}
	}

	// shift
	wchar_t wstr[128] = { };
	wcscpy(wstr, L"Shift");
	Vkeyboard[40].init(wstr, keybtn_init, keybtn_render, keybtn_update, keybtn_event, shp::rect4f(scloc.fx, starty - wh.y * 3, scloc.fx + wh.x * 1.5f, starty - wh.y * 2));

	// backspace
	wcscpy(wstr, L"<]");
	Vkeyboard[41].init(wstr, keybtn_init, keybtn_render, keybtn_update, keybtn_event, shp::rect4f(scloc.fx + siz.x - wh.x * 1.5f, starty - wh.y * 3, scloc.fx + siz.x, starty - wh.y * 2));

	// space
	wcscpy(wstr, L"[_____]");
	Vkeyboard[42].init(wstr, keybtn_init, keybtn_render, keybtn_update, keybtn_event, shp::rect4f(scloc.fx + siz.x / 2 - wh.x * 2, starty - wh.y * 4, scloc.fx + siz.x / 2 + wh.x * 2, starty - wh.y * 3));

	// . ,
	wcscpy(wstr, L",");
	Vkeyboard[43].init(wstr, keybtn_init, keybtn_render, keybtn_update, keybtn_event, shp::rect4f(scloc.fx + siz.x / 2 - wh.x * 3, starty - wh.y * 4, scloc.fx + siz.x / 2 - wh.x * 2, starty - wh.y * 3));

	wcscpy(wstr, L".");
	Vkeyboard[44].init(wstr, keybtn_init, keybtn_render, keybtn_update, keybtn_event, shp::rect4f(scloc.fx + siz.x / 2 + wh.x * 2, starty - wh.y * 4, scloc.fx + siz.x / 2 + wh.x * 3, starty - wh.y * 3));

	// han/eng
	wcscpy(wstr, L"한/영");
	Vkeyboard[45].init(wstr, keybtn_init, keybtn_render, keybtn_update, keybtn_event, shp::rect4f(scloc.fx, starty - wh.y * 4, scloc.fx + wh.x * 2, starty - wh.y * 3));

	// enter
	wcscpy(wstr, L"Enter");
	Vkeyboard[46].init(wstr, keybtn_init, keybtn_render, keybtn_update, keybtn_event, shp::rect4f(scloc.fx + siz.x - wh.x * 2, starty - wh.y * 4, scloc.fx + siz.x, starty - wh.y * 3));
	dbg << "};" << endl;
}

void texteditpage_render(Page* p)
{
	DXBtn* VKeyboard = (DXBtn*)&p->pfm.Data[(int)texteditpm::VKeyboard];
	bool* isshift = (bool*)&p->pfm.Data[(int)texteditpm::isshift];
	bool* ishan = (bool*)&p->pfm.Data[(int)texteditpm::ishan];

	drawline(shp::vec2f(-scw / 2.0f, 0), shp::vec2f(scw / 2.0f, 0), sch, DX11Color(0, 0, 0, 0.6f));
	for (int i = 0; i < 50; ++i) {
		VKeyboard[i].Render();
	}

	draw_string(tes.editstr, wcslen(tes.editstr), 40, shp::rect4f(-scw / 2.0f, 0, scw / 2.0f, 100), DX11Color(1.0f, 1.0f, 1.0f, 1.0f));
}

void texteditpage_update(Page* p, float delta)
{
	DXBtn* VKeyboard = (DXBtn*)&p->pfm.Data[(int)texteditpm::VKeyboard];
	bool* isshift = (bool*)&p->pfm.Data[(int)texteditpm::isshift];
	bool* ishan = (bool*)&p->pfm.Data[(int)texteditpm::ishan];
	for (int i = 0; i < 50; ++i) {
		VKeyboard[i].Update(delta);
	}
}

void texteditpage_event(Page* p, DX_Event evt)
{
	DXBtn* VKeyboard = (DXBtn*)&p->pfm.Data[(int)texteditpm::VKeyboard];
	bool* isshift = (bool*)&p->pfm.Data[(int)texteditpm::isshift];
	bool* ishan = (bool*)&p->pfm.Data[(int)texteditpm::ishan];
	for (int i = 0; i < 50; ++i) {
		VKeyboard[i].Event(evt);
	}
}

//--------------------------------------------------------------------------------------
// Entry point to the program. Initializes everything and goes into a message processing 
// loop. Idle time is used to render the scene.
//--------------------------------------------------------------------------------------
int WINAPI wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow )
{
	static chrono::steady_clock::time_point ft, et;
    dbg.open("dbg.txt", ios::ate);

    fm = new freemem::FM_System0();
    fm->SetHeapData(4096, 4096, 65536, 1048576);
    fm->_tempPushLayer();
    const char* font_path = "Cafe24Ssurround-v2.0.ttf";

    uint8_t condition_variable = 0;
    int8_t error = TTFFontParser::parse_file(font_path, &font_data, &font_parsed, &condition_variable);

    sub_font_data[0] = new TTFFontParser::FontData();
    const char* subfont_path = "ARLRDBD.TTF";
    error = TTFFontParser::parse_file(subfont_path, sub_font_data[0], &font_parsed, &condition_variable);

    UNREFERENCED_PARAMETER( hPrevInstance );
    UNREFERENCED_PARAMETER( lpCmdLine );

    if( FAILED( InitWindow( hInstance, nCmdShow ) ) )
        return 0;

    if( FAILED( InitDevice() ) )
    {
        CleanupDevice();
        return 0;
    }

	ft = chrono::high_resolution_clock::now();

    // Main message loop
    MSG msg = {0};
    while( WM_QUIT != msg.message )
    {
        if( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
        {
            TranslateMessage( &msg );
            DispatchMessage( &msg );
        }
        else
        {
			if (FinishInit) {
				Page* presentPage = pagestack[toppage - 1];
				int maxpage = toppage;

				et = chrono::high_resolution_clock::now();
				chrono::duration < float >codetime = et - ft;
				float delta = codetime.count();
				// dbgcount(0, dbg << "update : " << toppage - 1 << endl);
				presentPage->update_func(presentPage, delta);

				ft = chrono::high_resolution_clock::now();

				Render();
			}
        }
    }

    CleanupDevice();

    fm->_tempPopLayer();
    return ( int )msg.wParam;
}


//--------------------------------------------------------------------------------------
// Register class and create window
//--------------------------------------------------------------------------------------
HRESULT InitWindow( HINSTANCE hInstance, int nCmdShow )
{
    // Register class
	FinishInit = false;
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof( WNDCLASSEX );
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon( hInstance, ( LPCTSTR )IDI_ICON2 );
    wcex.hCursor = LoadCursor( NULL, IDC_ARROW );
    wcex.hbrBackground = ( HBRUSH )( COLOR_WINDOW + 1 );
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = L"TutorialWindowClass";
    wcex.hIconSm = LoadIcon( wcex.hInstance, ( LPCTSTR )IDI_ICON2);
    if( !RegisterClassEx( &wcex ) )
        return E_FAIL;

    // Create window
    g_hInst = hInstance;
    RECT rc = { 0, 0, 1600, 900 };
    AdjustWindowRect( &rc, WS_OVERLAPPEDWINDOW, FALSE );
    g_hWnd = CreateWindow( L"TutorialWindowClass", L"SpriteEditor_DirectX", WS_OVERLAPPEDWINDOW,
                           CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, hInstance,
                           NULL );
    if( !g_hWnd )
        return E_FAIL;

	// init
	mainpage = (Page*)fm->_New(sizeof(Page), true);
	mainpage->init_func = main_init;
	mainpage->render_func = main_render;
	mainpage->event_func = main_event;
	mainpage->update_func = main_update;

	colorpage = (Page*)fm->_New(sizeof(Page), true);
	colorpage->init_func = colorpage_init;
	colorpage->render_func = colorpage_render;
	colorpage->update_func = colorpage_update;
	colorpage->event_func = colorpage_event;

	texteditpage = (Page*)fm->_New(sizeof(Page), true);
	texteditpage->init_func = texteditpage_init;
	texteditpage->render_func = texteditpage_render;
	texteditpage->update_func = texteditpage_update;
	texteditpage->event_func = texteditpage_event;

	mainSprite = nullptr;

	pagestacking(mainpage);

    ShowWindow( g_hWnd, nCmdShow );

    return S_OK;
}


//--------------------------------------------------------------------------------------
// Helper for compiling shaders with D3DX11
//--------------------------------------------------------------------------------------
HRESULT CompileShaderFromFile( WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut )
{
    HRESULT hr = S_OK;

    DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
    // Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
    // Setting this flag improves the shader debugging experience, but still allows 
    // the shaders to be optimized and to run exactly the way they will run in 
    // the release configuration of this program.
    dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

    ID3DBlob* pErrorBlob;
    hr = D3DX11CompileFromFile( szFileName, NULL, NULL, szEntryPoint, szShaderModel, 
        dwShaderFlags, 0, NULL, ppBlobOut, &pErrorBlob, NULL );
    if( FAILED(hr) )
    {
        if( pErrorBlob != NULL )
            OutputDebugStringA( (char*)pErrorBlob->GetBufferPointer() );
        if( pErrorBlob ) pErrorBlob->Release();
        return hr;
    }
    if( pErrorBlob ) pErrorBlob->Release();

    return S_OK;
}


//--------------------------------------------------------------------------------------
// Create Direct3D device and swap chain
//--------------------------------------------------------------------------------------


void GetScreenWH() {
    RECT rc;
    GetClientRect(g_hWnd, &rc);
    width = rc.right - rc.left;
    height = rc.bottom - rc.top;
    scw = (float)width;
    sch = (float)height;
}

HRESULT InitDevice()
{

    HRESULT hr = S_OK;

    GetScreenWH();

    UINT createDeviceFlags = 0;
#ifdef _DEBUG
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_DRIVER_TYPE driverTypes[] =
    {
        D3D_DRIVER_TYPE_HARDWARE,
        D3D_DRIVER_TYPE_WARP,
        D3D_DRIVER_TYPE_REFERENCE,
    };
    UINT numDriverTypes = ARRAYSIZE( driverTypes );

	D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
	};
	UINT numFeatureLevels = ARRAYSIZE( featureLevels );

    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory( &sd, sizeof( sd ) );
    sd.BufferCount = 2;
    sd.BufferDesc.Width = width;
    sd.BufferDesc.Height = height;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = g_hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;

    for( UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++ )
    {
        g_driverType = driverTypes[driverTypeIndex];
        hr = D3D11CreateDeviceAndSwapChain( NULL, g_driverType, NULL, createDeviceFlags, featureLevels, numFeatureLevels,
                                            D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &g_featureLevel, &g_pImmediateContext );
        if( SUCCEEDED( hr ) )
            break;
    }
    if( FAILED( hr ) )
        return hr;

    // Create a render target view
    ID3D11Texture2D* pBackBuffer = NULL;
    hr = g_pSwapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), ( LPVOID* )&pBackBuffer );
    if( FAILED( hr ) )
        return hr;

    hr = g_pd3dDevice->CreateRenderTargetView( pBackBuffer, NULL, &g_pRenderTargetView );
    pBackBuffer->Release();
    if( FAILED( hr ) )
        return hr;

    // Create depth stencil texture
    D3D11_TEXTURE2D_DESC descDepth;
	ZeroMemory( &descDepth, sizeof(descDepth) );
    descDepth.Width = width;
    descDepth.Height = height;
    descDepth.MipLevels = 1;
    descDepth.ArraySize = 1;
    descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    descDepth.SampleDesc.Count = 1;
    descDepth.SampleDesc.Quality = 0;
    descDepth.Usage = D3D11_USAGE_DEFAULT;
    descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    descDepth.CPUAccessFlags = 0;
    descDepth.MiscFlags = 0;
    hr = g_pd3dDevice->CreateTexture2D( &descDepth, NULL, &g_pDepthStencil );
    if( FAILED( hr ) )
        return hr;

    // Create the depth stencil view
    D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	ZeroMemory( &descDSV, sizeof(descDSV) );
    descDSV.Format = descDepth.Format;
    descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    descDSV.Texture2D.MipSlice = 0;
    hr = g_pd3dDevice->CreateDepthStencilView( g_pDepthStencil, &descDSV, &g_pDepthStencilView );
    if( FAILED( hr ) )
        return hr;

    g_pImmediateContext->OMSetRenderTargets( 1, &g_pRenderTargetView, g_pDepthStencilView );

    // Setup the viewport
    D3D11_VIEWPORT vp;
    vp.Width = (FLOAT)width;
    vp.Height = (FLOAT)height;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    g_pImmediateContext->RSSetViewports( 1, &vp );

    D3D11_RASTERIZER_DESC rasterizerDesc;
    ZeroMemory(&rasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
    rasterizerDesc.CullMode = D3D11_CULL_NONE;
    rasterizerDesc.FillMode = D3D11_FILL_SOLID;
    rasterizerDesc.FrontCounterClockwise = false;
    ID3D11RasterizerState* rasterizerState;
    g_pd3dDevice->CreateRasterizerState(&rasterizerDesc, &rasterizerState);

    // Set the rasterizer state
    g_pImmediateContext->RSSetState(rasterizerState);

	// Compile the vertex shader
	ID3DBlob* pVSBlob = NULL;
    hr = CompileShaderFromFile( L"SpriteEditor_DirectX.fx", "VS", "vs_4_0", &pVSBlob );
    if( FAILED( hr ) )
    {
        MessageBox( NULL,
                    L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK );
        return hr;
    }

	// Create the vertex shader
	hr = g_pd3dDevice->CreateVertexShader( pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), NULL, &g_pVertexShader );
	if( FAILED( hr ) )
	{	
		pVSBlob->Release();
        return hr;
	}

    // Define the input layout
    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT numElements = ARRAYSIZE( layout );

    // Create the input layout
	hr = g_pd3dDevice->CreateInputLayout( layout, numElements, pVSBlob->GetBufferPointer(),
                                          pVSBlob->GetBufferSize(), &g_pVertexLayout );
	pVSBlob->Release();
	if( FAILED( hr ) )
        return hr;

    // Set the input layout
    g_pImmediateContext->IASetInputLayout( g_pVertexLayout );

	// Compile the pixel shader
	ID3DBlob* pPSBlob = NULL;
    hr = CompileShaderFromFile( L"SpriteEditor_DirectX.fx", "PS", "ps_4_0", &pPSBlob );
    if( FAILED( hr ) )
    {
        MessageBox( NULL,
                    L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK );
        return hr;
    }


	// Create the pixel shader
	hr = g_pd3dDevice->CreatePixelShader( pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), NULL, &g_pPixelShader );
	pPSBlob->Release();
    if( FAILED( hr ) )
        return hr;

    // Create vertex buffer
    SimpleVertex vertices[] =
    {
        SimpleVertex( -1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 255, 255 ),
        SimpleVertex( 1.0f, 1.0f, -1.0f, 0.0f, 255, 0.0f, 255 ),
        SimpleVertex( 1.0f, 1.0f, 1.0f, 0.0f, 255, 255, 255 ),
        SimpleVertex( -1.0f, 1.0f, 1.0f, 255, 0.0f, 0.0f, 255 ),
        SimpleVertex( -1.0f, -1.0f, -1.0f, 255, 0.0f, 255, 255 ),
        SimpleVertex( 1.0f, -1.0f, -1.0f, 255, 255, 0.0f, 255 ),
        SimpleVertex( 1.0f, -1.0f, 1.0f, 255, 255, 255, 255 ),
        SimpleVertex( -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 255 ),
    };

    D3D11_BUFFER_DESC bd;
	ZeroMemory( &bd, sizeof(bd) );
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof( SimpleVertex ) * 8;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
    D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory( &InitData, sizeof(InitData) );
    InitData.pSysMem = vertices;
    hr = g_pd3dDevice->CreateBuffer( &bd, &InitData, &g_pVertexBuffer );
    if( FAILED( hr ) )
        return hr;

    // Set vertex buffer
    UINT stride = sizeof( SimpleVertex );
    UINT offset = 0;
    g_pImmediateContext->IASetVertexBuffers( 0, 1, &g_pVertexBuffer, &stride, &offset );

    // Create index buffer
    WORD indices[] =
    {
        3,1,0,
        2,1,3,

        0,5,4,
        1,5,0,

        3,4,7,
        0,4,3,

        1,6,5,
        2,6,1,

        2,7,6,
        3,7,2,

        6,4,5,
        7,4,6,
    };
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof( WORD ) * 36;        // 36 vertices needed for 12 triangles in a triangle list
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
    InitData.pSysMem = indices;
    hr = g_pd3dDevice->CreateBuffer( &bd, &InitData, &g_pIndexBuffer );
    if( FAILED( hr ) )
        return hr;

    // Set index buffer
    g_pImmediateContext->IASetIndexBuffer( g_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0 );

    // Set primitive topology
    g_pImmediateContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

	// Create the constant buffer
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ConstantBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
    hr = g_pd3dDevice->CreateBuffer( &bd, NULL, &g_pConstantBuffer );
    if( FAILED( hr ) )
        return hr;

    // Initialize the world matrix
	g_World1 = XMMatrixIdentity();
	g_World2 = XMMatrixIdentity();
    g_CursorWorld = XMMatrixIdentity();

    // Initialize the view matrix
	XMVECTOR Eye = XMVectorSet(0.0f, 4.0f, -10.0f, 0.0f);
	XMVECTOR At = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	g_View = XMMatrixLookAtLH( Eye, At, Up );

    // Initialize the projection matrix
    float farZ = 100.0f;
    float nearZ = 0.01f;

    //원근투영
	g_Projection_3d = XMMatrixPerspectiveFovLH(XM_PIDIV2, width / (FLOAT)height, nearZ, farZ);
	//g_Projection_2d = XMMatrixPerspectiveFovLH( XM_PIDIV2, width / (FLOAT)height, nearZ, farZ);
    //직교투영
	g_Projection_2d = XMMATRIX(
        2.0f/(float)width, 0, 0, 0,
        0, 2.0f/(float)height, 0, 0,
        0, 0, 1.0f / (farZ - nearZ), 0,
        0, 0, 1.0f / (nearZ - farZ), 1);
    

    cursor_obj.Init(false);
    cursor_obj.begin();
    cursor_obj.av(SimpleVertex(0.0f, -30.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f));
    cursor_obj.av(SimpleVertex(30.0f, -30.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f));
    cursor_obj.av(SimpleVertex(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f));
    cursor_obj.end();

    polygon_obj.Init(false);
    polygon_obj.begin();
    polygon_obj.av(SimpleVertex(100.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f));
    polygon_obj.av(SimpleVertex(0.0f, 100.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f));
    polygon_obj.av(SimpleVertex(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f));
    polygon_obj.end();

    dbgpos_obj.Init(false);
    dbgpos_obj.begin();
    dbgpos_obj.av(SimpleVertex(10.0f, -10.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f));
    dbgpos_obj.av(SimpleVertex(10.0f, 10.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f));
    dbgpos_obj.av(SimpleVertex(-10.0f, 10.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f));
    dbgpos_obj.av(SimpleVertex(-10.0f, -10.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f));
    dbgpos_obj.end();

    polygon_cb.mWorld = XMMatrixTranspose(g_World1);
    polygon_cb.mView = XMMatrixTranspose(g_View);
    polygon_cb.mProjection = XMMatrixTranspose(g_Projection_2d);

    CharBuffer* cbuf = (CharBuffer*)fm->_New(sizeof(CharBuffer), true);
    cbuf->ready(L'안', g_pVertexShader, g_pPixelShader);
    char_map.insert(CharMap::value_type(L'안', cbuf));

    DX11Color color = DX11Color(1.0f, 1.0f, 1.0f, 1.0f);
    linedrt = (rbuffer*)fm->_New(sizeof(rbuffer), true);
    linedrt->Init(false);
    linedrt->begin();
    linedrt->av(SimpleVertex(shp::vec3f(-0.5f, -0.5f, 0), color));
    linedrt->av(SimpleVertex(shp::vec3f(0.5f, -0.5f, 0), color));
    linedrt->av(SimpleVertex(shp::vec3f(0.5f, 0.5f, 0), color));
    linedrt->av(SimpleVertex(shp::vec3f(-0.5f, 0.5f, 0), color));
    linedrt->end();

	mainpage->init_func(mainpage);
	colorpage->init_func(colorpage);
	texteditpage->init_func(texteditpage);

	FinishInit = true;

    return S_OK;
}


//--------------------------------------------------------------------------------------
// Clean up the objects we've created
//--------------------------------------------------------------------------------------
void CleanupDevice()
{
    if( g_pImmediateContext ) g_pImmediateContext->ClearState();

    if( g_pConstantBuffer ) g_pConstantBuffer->Release();
    if( g_pVertexBuffer ) g_pVertexBuffer->Release();
    if( g_pIndexBuffer ) g_pIndexBuffer->Release();
    if( g_pVertexLayout ) g_pVertexLayout->Release();
    if( g_pVertexShader ) g_pVertexShader->Release();
    if( g_pPixelShader ) g_pPixelShader->Release();
    if( g_pDepthStencil ) g_pDepthStencil->Release();
    if( g_pDepthStencilView ) g_pDepthStencilView->Release();
    if( g_pRenderTargetView ) g_pRenderTargetView->Release();
    if( g_pSwapChain ) g_pSwapChain->Release();
    if( g_pImmediateContext ) g_pImmediateContext->Release();
    if( g_pd3dDevice ) g_pd3dDevice->Release();
}

//--------------------------------------------------------------------------------------
// Called every time the application receives a message
//--------------------------------------------------------------------------------------
int mx, my;

LRESULT CALLBACK WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    PAINTSTRUCT ps;
    HDC hdc;
	scwh = shp::vec2f((float)scw * zoomrate, (float)sch * zoomrate);
	DX_Event evt;
	evt.hWnd = hWnd;
	evt.message = message;
	evt.lParam = lParam;
	evt.wParam = wParam;
	Page* presentPage = pagestack[toppage - 1];
	int maxpage = toppage;

    //pageEvent
    switch( message )
    {
        case WM_PAINT:
            hdc = BeginPaint( hWnd, &ps );
            EndPaint( hWnd, &ps );
            break;
        case WM_KEYDOWN:
            PostQuitMessage(0);
            break;
		case WM_LBUTTONDOWN:
			press_ef = true;
			break;
		case WM_LBUTTONUP:
			press_ef = false;
			break;
        case WM_MOUSEMOVE:
            mx = LOWORD(lParam);
            my = HIWORD(lParam);
            //DX_UI::mousePos = shp::vec2f((float)mx, (float)my);
            g_CursorWorld = XMMatrixTranslation((float)mx - (float)width/2.0f, -1.0f * (float)my + (float)height/2.0f, 0.9f);
            break;
        case WM_DESTROY:
            PostQuitMessage( 0 );
            break;

        default:
            return DefWindowProc( hWnd, message, wParam, lParam );
    }

	if (FinishInit) {
		presentPage->event_func(presentPage, evt);
	}
	

    return 0;
}


//--------------------------------------------------------------------------------------
// Render a frame
//--------------------------------------------------------------------------------------
void Render()
{
	Page* presentPage = pagestack[toppage - 1];
	int maxpage = toppage;
    GetScreenWH();

    fm->_tempPushLayer();
    // Update our time
    static float t = 0.0f;
    if( g_driverType == D3D_DRIVER_TYPE_REFERENCE )
    {
        t += ( float )XM_PI * 0.0125f;
    }
    else
    {
        static DWORD dwTimeStart = 0;
        DWORD dwTimeCur = GetTickCount();
        if( dwTimeStart == 0 )
            dwTimeStart = dwTimeCur;
        t = ( dwTimeCur - dwTimeStart ) / 1000.0f;
    }

    // 1st Cube: Rotate around the origin
	//g_World1 = XMMatrixRotationY( t );

    //
    // Clear the back buffer
    //
    float ClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f }; //red, green, blue, alpha
    g_pImmediateContext->ClearRenderTargetView( g_pRenderTargetView, ClearColor );

    //
    // Clear the depth buffer to 1.0 (max depth)
    //
    g_pImmediateContext->ClearDepthStencilView( g_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0 );

    //
    // Update variables for the first cube
    //
    
    
    //char_map.at(L'안')->render(cursor_cb);
    
    g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &polygon_cb, 0, 0);
    //polygon_obj.render(polygon_cb);

    draw_string(L"안녕하세용!! World!!", 16, 30, shp::rect4f(0, 0, 100, 100), DX11Color(1, 1, 1, 1));
    //
    // Present our back buffer to our front buffer
    //

	for (int i = 0; i < maxpage; ++i)
	{
		// dbgcount(0, dbg << "render : " << i << endl);
		pagestack[i]->render_func(pagestack[i]);
	}

	ConstantBuffer cursor_cb = SetCB(g_CursorWorld, g_View, g_Projection_2d, DX11Color(1.0f, 1.0f, 1.0f, 1.0f));
	g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cursor_cb, 0, 0);
	cursor_obj.render(cursor_cb);

    g_pSwapChain->Present( 0, 0 );
    fm->_tempPopLayer();
}
