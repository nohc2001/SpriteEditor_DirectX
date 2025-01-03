﻿//--------------------------------------------------------------------------------------
// File: Tutorial05.cpp
//
// This application demonstrates animation using matrix transformations
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
#include <windows.h>
//#include <xnamath.h>
#include "resource.h"
#include <tchar.h>
//#include "ShapeObject.h"
#include "dbg.h"
#include "Sprite.h"
#include "Animation.h"
#include "DX11_UI.h"
#include "hancom.h"
#include "InsideCodeBake.h"
#include "ICB_Extension.h"
#include "Extensions\exGeometry.h"
#include "Extensions\exGraphics.h"
#include "Extensions\exTool.h"
#include "ICB_Editor.h"
//#include "DX11_UI.h"

//--------------------------------------------------------------------------------------
// Global Variables
//--------------------------------------------------------------------------------------

word_base_sen_sys InsideCode_Bake::wbss;
type_data InsideCode_Bake::basictype[basictype_max];
operator_data InsideCode_Bake::basicoper[basicoper_max];
ofstream InsideCode_Bake::icl;
uint32_t InsideCode_Bake::icl_optionFlag;
instruct_data InsideCode_Bake::inst_meta[256];

HINSTANCE               g_hInst = NULL;
HWND                    g_hWnd = NULL;
D3D_DRIVER_TYPE         g_driverType = D3D_DRIVER_TYPE_NULL;
D3D_FEATURE_LEVEL       g_featureLevel = D3D_FEATURE_LEVEL_11_0;
IDXGISwapChain*         g_pSwapChain = NULL;
ID3D11RenderTargetView* g_pRenderTargetView = NULL;
ID3D11Texture2D*        g_pDepthStencil = NULL;
ID3D11DepthStencilView* g_pDepthStencilView = NULL;
ID3D11DepthStencilState* g_pDepthStencilState = NULL;
ID3D11VertexShader*     g_pVertexShader = NULL;
ID3D11PixelShader*      g_pPixelShader = NULL;
ID3D11InputLayout*      g_pVertexLayout = NULL;
ID3D11Buffer*           g_pVertexBuffer = NULL;
ID3D11Buffer*           g_pIndexBuffer = NULL;
ID3D11BlendState* g_pBlendStateBlend = NULL;
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
Sprite* basicSprite = nullptr;
Sprite* fpSprite = nullptr;
bool fpedit = false;

Page* mainpage = nullptr;
Page* colorpage = nullptr;
Page* texteditpage = nullptr;
Page* filepage = nullptr;

rbuffer* linedrt;
rbuffer* basic_circleRT;
bool press_ef = false;
float zoomrate;
shp::vec2f scwh;

UINT width;
UINT height;
rbuffer cursor_obj;
rbuffer polygon_obj;
rbuffer dbgpos_obj;

char sprloadmod = 'n';

LayerManager layerManager;
// n none, d : sprdir_start, l : loadfromfile_start, D : sprdir_end, L : loadfromfile_end

wchar_t* GetFileNameFromDlg_open() {
	OPENFILENAME OFN;
	TCHAR filePathName[256] = L"";
	TCHAR lpstrFile[256] = L"";
	static TCHAR filter[] = L"모든 파일\0*.*\0바이너리 파일\0*.bin\0bin 파일\0*.bin";

	memset(&OFN, 0, sizeof(OPENFILENAME));
	OFN.lStructSize = sizeof(OPENFILENAME);
	OFN.hwndOwner = g_hWnd;
	OFN.lpstrFilter = filter;
	OFN.lpstrFile = lpstrFile;
	OFN.nMaxFile = 256;
	OFN.lpstrInitialDir = L".";
	
	wchar_t* rstr = nullptr;

	if (GetOpenFileName(&OFN) != 0) {
		wsprintf(filePathName, L"%s 파일을 열겠습니까?", OFN.lpstrFile);
		MessageBox(g_hWnd, filePathName, L"열기 선택", MB_OK);

		wchar_t* str = OFN.lpstrFile;
		int len = (wcslen(str) + 1);
		rstr = (wchar_t*)fm->_New(sizeof(wchar_t) * len, true);
		wcscpy_s(rstr, len, str);
		return rstr;
	}
	return nullptr;
}

wchar_t* GetFileNameFromDlg_save() {
	OPENFILENAME OFN;
	TCHAR filePathName[256] = L"";
	TCHAR lpstrFile[256] = L"";
	static TCHAR filter[] = L"모든 파일\0*.*\0바이너리 파일\0*.bin\0bin 파일\0*.bin";

	memset(&OFN, 0, sizeof(OPENFILENAME));
	OFN.lStructSize = sizeof(OPENFILENAME);
	OFN.hwndOwner = g_hWnd;
	OFN.lpstrFilter = filter;
	OFN.lpstrFile = lpstrFile;
	OFN.nMaxFile = 256;
	OFN.lpstrInitialDir = L".";

	wchar_t* rstr = nullptr;

	if (GetSaveFileName(&OFN) != 0) {
		wsprintf(filePathName, L"%s 파일을 저장하겠습니까?", OFN.lpstrFile);
		MessageBox(g_hWnd, filePathName, L"저장 선택", MB_OK);

		wchar_t* str = OFN.lpstrFile;
		int len = (wcslen(str)+1);
		rstr = (wchar_t*)fm->_New(sizeof(wchar_t) * len, true);
		wcscpy_s(rstr, len, str);
		return rstr;
	}
	return rstr;
}

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
	, stacktime = 2381
	, selectclear = 2385
	, opencolorpage = 2585
	, openobjlist = 2785
	, showobjlist = 2985
	, closeobjlist = 2986
	, savefilestr = 3186
	, tabbtn_objs = 3696
	, tabbtn_property = 3896
	, list_height = 4096
	, tabSlider = 4104
	, objselect_id = 4288
	, tabselect_id = 4292
	, paramselect_id = 4296
	, sprdirbtn = 4300
	, loadbydirbtn = 4500
	, fpsprbtn = 4700
	, addicbtn = 4900
	, loadicbtn = 5100
	, layer_backinfo = 5300
	, layer_obj = 5308
	, layer_ui = 5316
	, layer_uitext = 5324
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
	, layer_backinfo = 2556
	, layer_obj = 2564
	, layer_ui = 2572
	, layer_uitext = 2580
};

enum class texteditpm {
	VKeyboard = 0
	, ishan = 10000
	, isshift = 10001
	, deststring = 10002
};

void drawline(shp::vec2f p0, shp::vec2f p1, float linewidth, DX11Color color, float Z = 0.0f)
{
    shp::vec2f delta = p1 - p0;
    float radian = shp::angle2f::usedxdy(delta.x, delta.y).radian;
    ConstantBuffer cb = GetBasicModelCB(shp::vec3f(0.5f * (p0.x + p1.x), 0.5f * (p0.y + p1.y), Z), shp::vec3f(0, 0, radian),
            shp::vec3f(sqrtf(delta.x * delta.x + delta.y * delta.y), linewidth, 1), color);
    linedrt->render(cb);
}

void drawline_cam(shp::vec2f p0, shp::vec2f p1, float linewidth, DX11Color color, float Z = 0.0f)
{
	shp::vec2f delta = p1 - p0;
	float radian = shp::angle2f::usedxdy(delta.x, delta.y).radian;
	ConstantBuffer cb = GetCamModelCB(shp::vec3f(0.5f * (p0.x + p1.x), 0.5f * (p0.y + p1.y), 0.0f), shp::vec3f(0, 0, radian),
		shp::vec3f(sqrtf(delta.x * delta.x + delta.y * delta.y), linewidth, 1), color);
	linedrt->render(cb);
}

void exTool_PostRenderManager::Render() {
	for (int i = 0; i < renderData.size(); ++i) {
		exTool_pRenderData* prd = renderData[i];
		exTool_PostRenderDataType t = *reinterpret_cast<exTool_PostRenderDataType*>(prd->data);
		switch (t) {
		case exTool_PostRenderDataType::prdt_Line:
		{
			exTool_RenderData_Line* rdl = reinterpret_cast<exTool_RenderData_Line*>(prd->data);
			drawline(shp::vec2f(rdl->loc.fx, rdl->loc.fy), shp::vec2f(rdl->loc.lx, rdl->loc.ly), 3, rdl->color, 0.0f);
		}
		break;
		case exTool_PostRenderDataType::prdt_FillRect:
		{
			exTool_RenderData_FillRect* rdl = reinterpret_cast<exTool_RenderData_FillRect*>(prd->data);
			drawline(shp::vec2f(rdl->loc.fx, rdl->loc.getCenter().y), shp::vec2f(rdl->loc.lx, rdl->loc.getCenter().y), rdl->loc.geth(), rdl->color, 0.0f);
		}
		break;
		case exTool_PostRenderDataType::prdt_Circle:
		{
			exTool_RenderData_Circle* rdl = reinterpret_cast<exTool_RenderData_Circle*>(prd->data);
			ConstantBuffer cb = GetBasicModelCB(shp::vec3f(rdl->center.x, rdl->center.y, 0), shp::vec3f(0, 0, 0), shp::vec3f(rdl->radius, rdl->radius, 1), rdl->color);
			basic_circleRT->render(cb);
		}
		break;
		case exTool_PostRenderDataType::prdt_Text:
		{
			exTool_RenderData_Text* rd = reinterpret_cast<exTool_RenderData_Text*>(prd->data);
			fmlwstr temp;
			temp.NULLState();
			temp.Init(rd->capacity, false);
			int capacity = rd->capacity;
			for (int i = 0; i < capacity; ++i) {
				temp.push_back((wchar_t)rd->strptr[i]);
			}
			draw_string(temp.c_str(), capacity - 1, rd->fontsiz, rd->loc, rd->color, 0.0f);
		}
		break;
		}
	}
}

fmvecarr < ICB_Extension* >basic_ext;
fmvecarr < ICB_Context* >ecss;
float exerate = 0.01f;

void ICB_Editor::StaticInit() {
	PlayBtnSpr = (Sprite*)fm->_New(sizeof(Sprite), true);
	PlayBtnSpr->null();
	PlayBtnSpr->load(L"SpriteData/PlayBtn.bin");

	PauseBtnSpr = (Sprite*)fm->_New(sizeof(Sprite), true);
	PauseBtnSpr->null();
	PauseBtnSpr->load(L"SpriteData/PauseBtn.bin");

	StopBtnSpr = (Sprite*)fm->_New(sizeof(Sprite), true);
	StopBtnSpr->null();
	StopBtnSpr->load(L"SpriteData/StopBtn.bin");

	StepBtnSpr = (Sprite*)fm->_New(sizeof(Sprite), true);
	StepBtnSpr->null();
	StepBtnSpr->load(L"SpriteData/StepBtn.bin");

	StepInBtnSpr = (Sprite*)fm->_New(sizeof(Sprite), true);
	StepInBtnSpr->null();
	StepInBtnSpr->load(L"SpriteData/StepInBtn.bin");

	PresentExecutionSpr = (Sprite*)fm->_New(sizeof(Sprite), true);
	PresentExecutionSpr->null();
	PresentExecutionSpr->load(L"SpriteData/PresentExecution.bin");
}
/*
how to : add ui
1. write ui render code in render function.
2. copy code that calculate ui rect data and paste to event code. and then add featrues.
*/

void ICB_Editor::RenderEditorWindow() {
	draw_string(currentErrorMsg.c_str(), currentErrorMsg.size(), codeline_height * 0.25f, shp::rect4f(loc.fx, loc.ly + 20, loc.lx, loc.ly + 100), DX11Color(1.0f, 0.5f, 0.5f, 1.0f), 0.1f);
	DX11Color col;
	if (focus) {
		col = DX11Color(0.0f, 0.0f, 0.0f, 1.0f);
	}
	else {
		col = DX11Color(0.0f, 0.0f, 0.0f, 0.5f);
	}
	drawline(shp::vec2f(loc.fx, loc.getCenter().y), shp::vec2f(loc.lx, loc.getCenter().y), loc.geth(), col, 0.3f);
	drawline(shp::vec2f(loc.lx - 30.0f, loc.fy + 15.0f), shp::vec2f(loc.lx, loc.fy + 15.0f), 30, DX11Color(1.0f, 1.0f, 1.0f, 1.0f), 0.1f);
	drawline(shp::vec2f(additionalTabLoc.fx, additionalTabLoc.getCenter().y), shp::vec2f(additionalTabLoc.lx, additionalTabLoc.getCenter().y), additionalTabLoc.geth(), DX11Color(0.8f, 1.0f, 1.0f, 0.9f), 0.25f);
	shp::rect4f headerRT = shp::rect4f(loc.fx, loc.ly - headerRate * codeline_height, loc.lx, loc.ly);
	drawline(shp::vec2f(headerRT.fx, headerRT.getCenter().y), shp::vec2f(headerRT.lx, headerRT.getCenter().y), headerRT.geth(), DX11Color(0.0f, 0.5f, 1.0f, 1.0f), 0.1f);

	shp::rect4f sliderLoc = shp::rect4f(loc.fx + loc.getw() * CodeEditorRate, loc.fy, loc.fx + loc.getw() * CodeEditorRate + SliderWidRate * codeline_height, headerRT.fy);
	drawline(shp::vec2f(sliderLoc.fx, sliderLoc.ly - SliderRate * sliderLoc.geth()), shp::vec2f(sliderLoc.lx, 2 * tipmov * life[5] + sliderLoc.ly - SliderRate * sliderLoc.geth()), 20, DX11Color(1.0f, 1.0f, 1.0f, 1.0f), 0.1f);
}

void ICB_Editor::RenderCodeLines() {
	float fontsiz = codeline_height * 0.25f;
	shp::rect4f codeEditorLoc = shp::rect4f(loc.fx, loc.fy, loc.fx + loc.getw() * CodeEditorRate, loc.ly);
	shp::rect4f headerRT = shp::rect4f(loc.fx, loc.ly - headerRate * codeline_height, loc.lx, loc.ly);
	shp::rect4f lineloc = shp::rect4f(codeEditorLoc.fx, headerRT.fy - 2 * codeline_height, codeEditorLoc.lx, headerRT.fy - codeline_height);
	lineloc.fy += codeline_height; lineloc.ly += codeline_height;
	shp::rect4f linenumloc = lineloc;
	lineloc.fx += codeline_height * linenumwid;

	int max_express = loc.geth() / codeline_height;
	int maxx = max_express + codeLines.size() * SliderRate;
	if (maxx > codeLines.size()) maxx = codeLines.size();
	int i = codeLines.size() * SliderRate;
	int smax = max_express;
	int subi = 0;
	int maxX = (int)(1.5f * codeEditorLoc.getw() / codeline_height);

	int exeline = 0;
	if (SelectedCXT >= 0 && SelectedCXT < ecss.size()) {
		ICB_Context* ecs = ecss.at(SelectedCXT);
		int present_codeline = ecs->pc - ecs->codemem;
		code_sen* exe_cs = ecs->icb->find_codesen_with_linenum(ecs->icb->csarr, present_codeline);
		if (exe_cs != nullptr) {
			exeline = exe_cs->codeline;
		}
	}

	for (int s = 0; s < smax; ++s) {
		if (i >= codeLines.size()) break;

		// render breakpoint & present execution icon
		shp::rect4f breakIrect = shp::rect4f(codeEditorLoc.fx, lineloc.fy, codeEditorLoc.fx + lineloc.geth(), lineloc.ly);
		for (int i0 = 0; i0 < CodeLineBreakPoints.size(); ++i0) {
			if (CodeLineBreakPoints.at(i0) == i) {
				drawline(shp::vec2f(breakIrect.fx, breakIrect.getCenter().y), shp::vec2f(breakIrect.lx, breakIrect.getCenter().y), breakIrect.geth(), DX11Color(1.0f, 0.0f, 0.0f, 1.0f), 0.2f);
			}
		}

		if (exeline == i) {
			PresentExecutionSpr->render(GetBasicModelCB(shp::vec3f(lineloc.fx - 20.0f, lineloc.getCenter().y, 0.09f), shp::vec3f(0, 0, 0), shp::vec3f(0.05f, 0.05f, 1.0f), DX11Color(1.0f, 1.0f, 1.0f, 1.0f)));
		}

		wstring wstr;
		wstr = to_wstring(i);
		draw_string((wchar_t*)wstr.c_str(), wstr.size(), fontsiz, linenumloc, DX11Color(1.0f, 1.0f, 1.0f, 1.0f), 0.1f);
		shp::vec2i lim = GetExpendLimit(i);
		bool hold = false;
		if (lim.x >= 0) {
			if (GetExpend(i) == 0) {
				hold = true;
				shp::rect4f expbtn = shp::rect4f(lineloc.fx - codeline_height, lineloc.ly - codeline_height, lineloc.fx, lineloc.ly);
				drawline(shp::vec2f(expbtn.fx, expbtn.getCenter().y), shp::vec2f(expbtn.lx, expbtn.getCenter().y), expbtn.geth(), DX11Color(1.0f, 1.0f, 1.0f, 1.0f), 0.05f);
				shp::rect4f minrect = shp::rect4f(expbtn.fx, expbtn.getCenter().y - codeline_height * 0.2f, expbtn.lx, expbtn.getCenter().y + codeline_height * 0.2f);
				drawline(shp::vec2f(minrect.fx + 1, minrect.getCenter().y), shp::vec2f(minrect.lx - 1, minrect.getCenter().y), minrect.geth(), DX11Color(0.0f, 0.0f, 0.0f, 1.0f), 0.02f);
				shp::rect4f plsrect = shp::rect4f(expbtn.getCenter().x - codeline_height * 0.2f, expbtn.fy, expbtn.getCenter().x + codeline_height * 0.2f, expbtn.ly);
				drawline(shp::vec2f(plsrect.fx + 1, plsrect.getCenter().y), shp::vec2f(plsrect.lx - 1, plsrect.getCenter().y), plsrect.geth(), DX11Color(0.0f, 0.0f, 0.0f, 1.0f), 0.02f);

				fmlwstr wfstr;
				wfstr.NULLState();
				wfstr.Init(codeLines[i].size(), false);
				int stack = 0;
				for (int k = subi + Xofffset; k < codeLines[i].size(); ++k) {
					if (codeLines[i][k] == L'{') {
						stack += 1;
						wfstr.push_back(L'{');
						wfstr.push_back(L'.');
						wfstr.push_back(L'.');
						wfstr.push_back(L'.');
						wfstr.push_back(L'}');
					}
					else if (codeLines[i][k] == L'}') {
						stack -= 1;
					}

					if (stack == 0) {
						wfstr.push_back(codeLines[i][k]);
					}
				}
				if (wfstr.up > maxX) wfstr.up = maxX;

				if (i == selected_codeline) {
					drawline(shp::vec2f(lineloc.fx, lineloc.getCenter().y), shp::vec2f(lineloc.lx, lineloc.getCenter().y), codeline_height * 0.9f, DX11Color(0.2f, 0.2f, 0.7f, life[7]), 0.2f);
					shp::rect4f textcursorLoc = GetLoc_stringIndex(wfstr.Arr, wfstr.size(), fontsiz, lineloc, selected_charindex);
					draw_string(wfstr.Arr, wfstr.size(), fontsiz, lineloc, DX11Color(1.0f, 1.0f, 1.0f, 1.0f), 0.1f);
					drawline(shp::vec2f(textcursorLoc.fx, textcursorLoc.ly), shp::vec2f(textcursorLoc.fx + 3, textcursorLoc.ly), codeline_height, DX11Color(0.8f, 0.8f, 0.7f, life[8]), 0.05f);
				
				}
				else {
					drawline(shp::vec2f(lineloc.fx, lineloc.getCenter().y), shp::vec2f(lineloc.lx, lineloc.getCenter().y), codeline_height * 0.9f, DX11Color(0.2f, 0.2f, 0.4f, 0.5f), 0.2f);
					draw_string(wfstr.Arr, wfstr.size(), fontsiz, lineloc, DX11Color(1.0f, 1.0f, 1.0f, 1.0f), 0.1f);
				}
				wfstr.release();

				i = lim.x;
				subi = lim.y + 1;
				if (codeLines[i].size() <= subi) {
					i += 1;
					subi = 0;
				}
			}
			else {
				shp::rect4f expbtn = shp::rect4f(lineloc.fx - codeline_height, lineloc.ly - codeline_height, lineloc.fx, lineloc.ly);
				drawline(shp::vec2f(expbtn.fx, expbtn.getCenter().y), shp::vec2f(expbtn.lx, expbtn.getCenter().y), expbtn.geth(), DX11Color(1.0f, 1.0f, 1.0f, 1.0f), 0.05f);
				shp::rect4f minrect = shp::rect4f(expbtn.fx, expbtn.getCenter().y - codeline_height * 0.2f, expbtn.lx, expbtn.getCenter().y + codeline_height * 0.2f);
				drawline(shp::vec2f(minrect.fx + 1, minrect.getCenter().y), shp::vec2f(minrect.lx - 1, minrect.getCenter().y), minrect.geth(), DX11Color(0.0f, 0.0f, 0.0f, 1.0f), 0.02f);
			}
		}
		if (hold == false) {
			fm->_tempPushLayer();
			fmlwstr wfstr;
			wfstr.NULLState();
			wfstr.Init(codeLines[i].size(), false, false);
			for (int k = subi + Xofffset; k < codeLines[i].size(); ++k) {
				wfstr.push_back(codeLines[i][k]);
			}
			if (wfstr.up > maxX) wfstr.up = maxX;

			if (i != selected_codeline) {
				drawline(shp::vec2f(lineloc.fx, lineloc.getCenter().y), shp::vec2f(lineloc.lx, lineloc.getCenter().y), codeline_height * 0.9f, DX11Color(0.2f, 0.2f, 0.4f, 0.5f), 0.2f);
				draw_string(wfstr.Arr, wfstr.size(), fontsiz, lineloc, DX11Color(1.0f, 1.0f, 1.0f, 1.0f), 0.1f);
			}
			else {
				drawline(shp::vec2f(lineloc.fx, lineloc.getCenter().y), shp::vec2f(lineloc.lx, lineloc.getCenter().y), codeline_height * 0.9f, DX11Color(0.2f, 0.2f, 0.7f, life[7]), 0.2f);
				shp::rect4f textcursorLoc = GetLoc_stringIndex(wfstr.Arr, wfstr.size(), fontsiz, lineloc, selected_charindex);
				draw_string(wfstr.Arr, wfstr.size(), fontsiz, lineloc, DX11Color(1.0f, 1.0f, 1.0f, 1.0f), 0.1f);
				drawline(shp::vec2f(textcursorLoc.fx, textcursorLoc.ly), shp::vec2f(textcursorLoc.fx + 3, textcursorLoc.ly), codeline_height, DX11Color(0.8f, 0.8f, 0.7f, life[8]), 0.05f);
			}

			fm->_tempPopLayer();

			++i;
			subi = 0;
		}
		lineloc.fy -= codeline_height * linemargin; lineloc.ly -= codeline_height * linemargin;
		linenumloc.fy -= codeline_height * linemargin; linenumloc.ly -= codeline_height * linemargin;
	}
}

void ICB_Editor::RenderDbgToolBox() {
	shp::rect4f dbgtoolbox_rt = shp::rect4f(codeEditorLoc.lx - 200, headerRT.fy - 50, codeEditorLoc.lx, headerRT.fy);
	drawline(shp::vec2f(dbgtoolbox_rt.fx, dbgtoolbox_rt.getCenter().y), shp::vec2f(dbgtoolbox_rt.lx, dbgtoolbox_rt.getCenter().y), dbgtoolbox_rt.geth(), DX11Color(0.0f, 0.0f, 0.0f, 1.0f), 0.15f);

	ICB_Context* ecs = ecss.at(SelectedCXT);
	DX11Color BreakC;
	DX11Color PlayC;
	if (ecs->isBreaking) {
		BreakC = DX11Color(1.0f, 1.0f, 1.0f, 1.0f);
		PlayC = DX11Color(1.0f, 1.0f, 1.0f, 0.3f);
	}
	else {
		BreakC = DX11Color(1.0f, 1.0f, 1.0f, 0.3f);
		PlayC = DX11Color(1.0f, 1.0f, 1.0f, 1.0f);
	}
	PlayBtnSpr->render(GetBasicModelCB(shp::vec3f(dbgtoolbox_rt.fx + 20, dbgtoolbox_rt.ly - 20, 0), shp::vec3f(0, 0, 0), shp::vec3f(0.1f, 0.1f, 0.1f), BreakC));
	PauseBtnSpr->render(GetBasicModelCB(shp::vec3f(dbgtoolbox_rt.fx + 60, dbgtoolbox_rt.ly - 20, 0.1f), shp::vec3f(0, 0, 0), shp::vec3f(0.1f, 0.1f, 0.1f), PlayC));
	StopBtnSpr->render(GetBasicModelCB(shp::vec3f(dbgtoolbox_rt.fx + 100, dbgtoolbox_rt.ly - 20, 0.1f), shp::vec3f(0, 0, 0), shp::vec3f(0.1f, 0.1f, 0.1f), PlayC));
	StepBtnSpr->render(GetBasicModelCB(shp::vec3f(dbgtoolbox_rt.fx + 140, dbgtoolbox_rt.ly - 20, 0), shp::vec3f(0, 0, 0), shp::vec3f(0.1f, 0.1f, 0.1f), BreakC));
	StepInBtnSpr->render(GetBasicModelCB(shp::vec3f(dbgtoolbox_rt.fx + 180, dbgtoolbox_rt.ly - 20, 0), shp::vec3f(0, 0, 0), shp::vec3f(0.1f, 0.1f, 0.1f), BreakC));
}

void ICB_Editor::RenderExtentionTools() {
	float tipf = 2 * tipmov * life[3];
	shp::rect4f foldbtnLoc = shp::rect4f(loc.fx + tipf, loc.ly - 2 * codeline_height + tipf, loc.fx - tipf + 2 * codeline_height, loc.ly - tipf);
	drawline(shp::vec2f(foldbtnLoc.fx, foldbtnLoc.getCenter().y), shp::vec2f(foldbtnLoc.lx, foldbtnLoc.getCenter().y), foldbtnLoc.geth(), DX11Color(1.0f, 1.0f, 0.5f, 1.0f), 0.05f);

	wchar_t tempstr4[16] = L"compass";
	shp::rect4f CompasbtnLoc;
	if (tools[0].ecs->ExeState) {
		CompasbtnLoc = shp::rect4f(additionalTabLoc.lx + life[5] * 4 * codeline_height, additionalTabLoc.ly - 2 * codeline_height, additionalTabLoc.lx + 4 * codeline_height, additionalTabLoc.ly);
	}
	else {
		CompasbtnLoc = shp::rect4f(additionalTabLoc.lx, additionalTabLoc.ly - 2 * codeline_height, additionalTabLoc.lx + 4 * codeline_height, additionalTabLoc.ly);
	}
	drawline(shp::vec2f(CompasbtnLoc.fx, CompasbtnLoc.getCenter().y), shp::vec2f(CompasbtnLoc.lx, CompasbtnLoc.getCenter().y), CompasbtnLoc.geth(), DX11Color(0.1f, 0.5f, 0.5f, 1.0f), 0.2f);
	draw_string(tempstr4, 7, codeline_height * 0.25f, CompasbtnLoc, DX11Color(1.0f, 1.0f, 1.0f, 1.0f), 0.1f);
}

void ICB_Editor::RenderContextList() {
	// context page render
	shp::rect4f varbtnLoc = shp::rect4f(additionalTabLoc.fx + atmargin, additionalTabLoc.ly - 70, additionalTabLoc.fx + 7 * atmargin, additionalTabLoc.ly - 10);
	fmvecarr<ui32> cxtindexes; // list context using this icb.
	cxtindexes.NULLState();
	cxtindexes.Init(8, false, false);
	for (int i = 0; i < ecss.size(); ++i) {
		if (ecss.at(i)->icb == SelectedICB) {
			cxtindexes.push_back(i);
		}
	}

	for (int i = 0; i < cxtindexes.size(); ++i) {
		shp::rect4f cxtrt = shp::rect4f(additionalTabLoc.fx + atmargin, varbtnLoc.fy - atmargin - (i + 1) * 30, additionalTabLoc.lx - 2 * atmargin, varbtnLoc.fy - atmargin - i * 30);
		DX11Color BackColor = DX11Color(0.0f, 0.0f, 0.5f, 1.0f);
		if (SelectedCXT == cxtindexes.at(i)) {
			BackColor = DX11Color(0.0f, 0.0f, 0.5f, 1.0f);
		}
		else {
			BackColor = DX11Color(0.0f, 0.0f, 0.0f, 0.5f);
		}
		drawline(shp::vec2f(cxtrt.fx, cxtrt.getCenter().y), shp::vec2f(cxtrt.lx, cxtrt.getCenter().y), cxtrt.geth(), BackColor, 0.15f);
		wstring wstr = to_wstring(i);
		wstr.insert(wstr.begin(), L't');
		wstr.insert(wstr.begin(), L'x');
		wstr.insert(wstr.begin(), L'c');
		draw_string((wchar_t*)wstr.c_str(), 9, atmargin * 0.3f, cxtrt, DX11Color(1.0f, 1.0f, 1.0f, 1.0f), 0.1f);

		ICB_Context* cxt = ecss.at(cxtindexes.at(i));
		if (cxt->isDbg) {
			// debug mod
			cxtrt.fx = cxtrt.lx - atmargin * 2;
			draw_string(L"dbg", 3, atmargin * 0.3f, cxtrt, DX11Color(1.0f, 0.5f, 0.5f, 1.0f), 0.1f);
		}
		else {
			// execute mod
			cxtrt.fx = cxtrt.lx - atmargin * 2;
			draw_string(L"exe", 3, atmargin * 0.3f, cxtrt, DX11Color(0.25, 1.0f, 0.8f, 1.0f), 0.1f);
		}

		if (cxt->isBreaking) {
			//stoping
			cxtrt.fx = cxtrt.fx - atmargin * 6;
			draw_string(L"breaking", 8, atmargin * 0.3f, cxtrt, DX11Color(1.0f, 0.5f, 0.5f, 1.0f), 0.1f);
		}
		else {
			//playing
			float tipf = 2 * tipmov * life[4];
			cxtrt.fx = cxtrt.fx - atmargin * 6;
			cxtrt.fy += tipf;
			draw_string(L"playing", 7, atmargin * 0.3f, cxtrt, DX11Color(0.25, 1.0f, 0.8f, 1.0f), 0.1f);
		}
	}

	cxtindexes.release();
}

void ICB_Editor::RenderFuntionList() {
	// function page render
	shp::rect4f varbtnLoc = shp::rect4f(additionalTabLoc.fx + atmargin, additionalTabLoc.ly - 70, additionalTabLoc.fx + 7 * atmargin, additionalTabLoc.ly - 10);
	if (SelectedCXT >= 0) {
		constexpr float ymargin = 30;
		shp::vec2f stackp = shp::vec2f(additionalTabLoc.fx, varbtnLoc.fy);
		shp::rect4f stackloc = shp::rect4f(additionalTabLoc.fx, varbtnLoc.fy - ymargin, additionalTabLoc.lx, varbtnLoc.fy);
		float height = varbtnLoc.fy - additionalTabLoc.fy;

		ICB_Context* ecs = ecss.at(SelectedCXT);

		byte8* sps = ecs->mem + ecs->max_mem_byte - 1;

		wchar_t funcstr[1024] = {};

		for (int i = 0; i < ecs->icb->globalVariables.size(); ++i) {
			// render global Variables
			NamingData* nd = ecs->icb->globalVariables.at(i);
			byte8* vptr = &ecs->datamem.at(nd->add_address);
			stackp = RenderVariableState(*nd, stackp, additionalTabLoc, vptr, -1);
			stackp.y -= ymargin / 2.0f;
		}

		for (int i = ecs->call_stack.size() - 1; i >= 0; --i) {
			//select function of call_stack context.
			si64 m = 1 << 30;
			func_data* fd = nullptr;

			for (int k = 0; k < ecs->icb->functions.size(); ++k) {
				si64 d = (si64)ecs->icb->functions.at(k)->start_pc - (si64)ecs->call_stack.at(k);
				if (m > d) {
					m = d;
					fd = ecs->icb->functions.at(k);
				}
			}

			int up = 0;
			if (fd != nullptr) {
				for (int k = 0; k < fd->name.size(); ++k) {
					funcstr[k] = (wchar_t)fd->name[k];
					up += 1;
				}
				funcstr[up] = L'('; up += 1;
				for (int k = 0; k < fd->param_data.size(); ++k) {
					for (int u = 0; u < fd->param_data[k].td->name.size(); ++u) {
						funcstr[up] = (wchar_t)fd->param_data[k].td->name[u]; up += 1;
					}
					funcstr[up] = L' '; up += 1;
					int fdp_len = strlen(fd->param_data[k].name);
					for (int u = 0; u < fdp_len; ++u) {
						funcstr[up] = (wchar_t)fd->param_data[k].name[u]; up += 1;
					}
					if (fd->param_data.size() - 1 > k) {
						funcstr[up] = L','; up += 1;
					}
				}
				funcstr[up] = L')'; up += 1;
				funcstr[up] = 0;
			}

			draw_string(funcstr, up, fontsiz, shp::rect4f(stackp.x, stackp.y - ymargin, stackp.x, stackp.y), DX11Color(0.0f, 0.0f, 0.0f, 1.0f), 0.1f);
			stackp.y -= ymargin;

			if (isVariableTableShowExpending(ecs->call_stack.at(i))) {
				fmvecarr<NamingData> ndarr = ecs->icb->GetLocalVariableListInFunction(fd);

				for (int i = 0; i < ndarr.size(); ++i) {
					//render variable table object
					NamingData nd = ndarr.at(i);
					byte8* vptr = sps - nd.add_address - nd.td->typesiz;
					stackp = RenderVariableState(nd, stackp, additionalTabLoc, vptr, -1);
					stackp.y -= ymargin;
				}

				ndarr.release();
			}
		}

		si64 m = 1 << 30;
		func_data* fd = nullptr;

		for (int k = 0; k < ecs->icb->functions.size(); ++k) {
			si64 d = (si64)ecs->icb->functions.at(k)->start_pc - (si64)ecs->pc;
			if (m > d) {
				m = d;
				fd = ecs->icb->functions.at(k);
			}
		}

		fmvecarr<NamingData> ndarr = ecs->icb->GetLocalVariableListInFunction(fd);
		//present context function stack render
		for (int i = 0; i < fd->param_data.size(); ++i) {
			//render variable table object
			NamingData nd = ndarr.at(i);
			byte8* vptr = sps - nd.add_address - nd.td->typesiz;
			stackp = RenderVariableState(nd, stackp, additionalTabLoc, vptr, -1);
			stackp.y -= ymargin;
		}
		ndarr.release();
	}

}

void ICB_Editor::Render() {
	RenderMetaDataUpdate();
	if (befold == false) {
		RenderEditorWindow();
		RenderCodeLines();

		//right info page render
		wchar_t tempstr0[16] = L"function";
		wchar_t tempstr1[16] = L"contexts";
		wchar_t tempstr2[16] = L"import";
		wchar_t tempstr3[16] = L"compile";
		shp::rect4f varbtnLoc = shp::rect4f(additionalTabLoc.fx + atmargin, additionalTabLoc.ly - 70, additionalTabLoc.fx + 7 * atmargin, additionalTabLoc.ly - 10);
		shp::rect4f funcbtnLoc = shp::rect4f(additionalTabLoc.fx + 9 * atmargin, additionalTabLoc.ly - 70, additionalTabLoc.fx + 15 * atmargin, additionalTabLoc.ly - 10);
		shp::rect4f importbtnLoc = shp::rect4f(additionalTabLoc.fx + atmargin, additionalTabLoc.fy, additionalTabLoc.fx + 10 * atmargin, additionalTabLoc.fy + 40);
		shp::rect4f compilebtnLoc = shp::rect4f(additionalTabLoc.fx + atmargin, additionalTabLoc.fy + 50, additionalTabLoc.fx + 10 * atmargin, additionalTabLoc.fy + 90);
		drawline(shp::vec2f(varbtnLoc.fx, varbtnLoc.getCenter().y), shp::vec2f(varbtnLoc.lx, varbtnLoc.getCenter().y), varbtnLoc.geth(), DX11Color(0.0f, 0.0f, 0.0f, 1.0f), 0.15f);
		draw_string(tempstr0, 9, atmargin * 0.3f, varbtnLoc, DX11Color(1.0f, 1.0f, 1.0f, 1.0f), 0.1f);
		drawline(shp::vec2f(funcbtnLoc.fx, funcbtnLoc.getCenter().y), shp::vec2f(funcbtnLoc.lx, funcbtnLoc.getCenter().y), funcbtnLoc.geth(), DX11Color(0.0f, 0.0f, 0.0f, 1.0f), 0.15f);
		draw_string(tempstr1, 8, atmargin * 0.3f, funcbtnLoc, DX11Color(1.0f, 1.0f, 1.0f, 1.0f), 0.1f);
		drawline(shp::vec2f(importbtnLoc.fx, importbtnLoc.getCenter().y), shp::vec2f(importbtnLoc.lx, importbtnLoc.getCenter().y), importbtnLoc.geth(), DX11Color(0.0f, 0.0f, 0.0f, 1.0f), 0.15f);
		draw_string(tempstr2, 6, atmargin * 0.5f, importbtnLoc, DX11Color(1.0f, 1.0f, 1.0f, 1.0f), 0.1f);
		drawline(shp::vec2f(compilebtnLoc.fx, compilebtnLoc.getCenter().y), shp::vec2f(compilebtnLoc.lx, compilebtnLoc.getCenter().y), compilebtnLoc.geth(), DX11Color(0.0f, 0.0f, 0.0f, 1.0f), 0.15f);
		draw_string(tempstr3, 7, atmargin * 0.5f, compilebtnLoc, DX11Color(1.0f, 1.0f, 1.0f, 1.0f), 0.1f);

		if (showVarmod) {
			RenderFuntionList();
		}
		else {
			RenderContextList();
		}

		//debuging tool box
		if ((SelectedCXT >= 0 && ecss.size() > SelectedCXT) && ecss.at(SelectedCXT)->isDbg) {
			RenderDbgToolBox();
		}

		RenderExtentionTools();
	}
	else {
		float tipf = 2 * tipmov * life[3];
		shp::rect4f temp = shp::rect4f(object_position.x - codeline_height, object_position.y - codeline_height, object_position.x + codeline_height, object_position.y + codeline_height);
		shp::rect4f foldLoc = shp::rect4f(temp.fx + tipf, temp.ly - 2 * codeline_height + tipf, temp.fx - tipf + 2 * codeline_height, temp.ly - tipf);
		drawline_cam(shp::vec2f(foldLoc.fx, foldLoc.getCenter().y), shp::vec2f(foldLoc.lx, foldLoc.getCenter().y), foldLoc.geth(), DX11Color(1.0f, 1.0f, 0.5f, 1.0f), 0.05f);
	}
}

void ICB_Editor::basicToolInit() {
	ICBE_ToolData icbt;
	icbt.name = (char*)fm->_New(8, true);
	strcpy_s(icbt.name, 8, "compass");
	icbt.len = 8;
	icbt.ecs = nullptr;

	InsideCode_Bake* compassicb;
	if (icmap.find("ECS_examples/tool_Compas.txt") == icmap.end()) {
		compassicb = (InsideCode_Bake*)fm->_New(sizeof(InsideCode_Bake), true);
		compassicb->HashInit();
		compassicb->init(40960);
		compassicb->extension.push_back(basic_ext[0]);

		compassicb->extension.push_back(basic_ext[1]);
		compassicb->extension.push_back(ICB_exTool);
		compassicb->bake_code("ECS_examples/tool_Compas.txt");

		if (compassicb->curErrMsg[0] != 0) {
			currentErrorMsg.up = 0;
			for (int i = 0; i < compassicb->curErrMsg.size(); ++i) {
				currentErrorMsg.push_back((wchar_t)compassicb->curErrMsg[i]);
			}
		}
	}
	else {
		compassicb = icmap["\ECS_examples\tool_Compass.txt"];
	}

	icbt.ecs = (ICB_Context*)fm->_New(sizeof(ICB_Context), true);
	icbt.ecs->SetICB(compassicb, 40960);
	tools.push_back(icbt);
	icbt.ecs->ExeState = false;
	*reinterpret_cast<exTool_EventSystem**>(&icbt.ecs->datamem[0]) = ToolEventSystem;
	*reinterpret_cast<exTool_PostRenderManager**>(&icbt.ecs->datamem[8]) = ToolPostRender;
	*reinterpret_cast<ICB_Editor**>(&icbt.ecs->datamem[16]) = this;
	ecss.push_back(icbt.ecs);
	using_tool = false;
	SelectedCXT = -1;
}
void ICB_Editor::icb_Init(InsideCode_Bake* sicb) {
	SelectedICB = sicb;

	if (SelectedICB != nullptr) {
		if (SelectedICB->curErrMsg.Arr != nullptr && SelectedICB->curErrMsg[0] != 0) {
			currentErrorMsg.up = 0;
			currentErrorMsg.Arr[0] = 0;
			for (int i = 0; i < SelectedICB->curErrMsg.size(); ++i) {
				currentErrorMsg.push_back((wchar_t)SelectedICB->curErrMsg[i]);
			}
		}

		//update datas
		for (int i = 0; i < codeLines.size(); ++i) {
			codeLines[i].release();
			codeLines[i].NULLState();
		}
		codeLines.up = 0;

		codeLine_expend.up = 0;

		if (SelectedICB->curErrMsg[0] == 0) {
			for (int i = 0; i < SelectedICB->csarr->size(); ++i) {
				code_sen* cs = SelectedICB->csarr->at(i);
				if (cs->ck != codeKind::ck_blocks) {
					PushNewCodeLine();
					//codeLine_expend.push_back(false);
					for (int k = 0; k < cs->maxlen; ++k) {
						char* word = cs->sen[k];
						int wlen = strlen(word);
						for (int u = 0; u < wlen; ++u) {
							codeLines.last().push_back((wchar_t)word[u]);
							//WhenCharInsert(codeLines.size() - 1, codeLines.last().size() - 1, word[u]);
						}
						codeLines.last().push_back(L' ');
						//WhenCharInsert(codeLines.size() - 1, codeLines.last().size() - 1, word[u]);
					}

					if (cs->ck != codeKind::ck_addFunction && (cs->ck != codeKind::ck_while && cs->ck != codeKind::ck_if)) {
						codeLines.last().push_back(L';');
						//WhenCharInsert(codeLines.size() - 1, codeLines.last().size() - 1, word[u]);
					}

					codeLines.last().c_str();
				}
				else {
					ImportBlockToCodeLine(cs);
				}
			}
		}

	}
}
void ICB_Editor::icb_Init(const char* filename) {
	if (icmap.find((char*)filename) == icmap.end()) {
		ReadCodelines_FromFile(filename);
		SelectedICB = (InsideCode_Bake*)fm->_New(sizeof(InsideCode_Bake), true);
		SelectedICB->init(40960);
		SelectedICB->HashInit();
		for (int i = 0; i < basic_ext.size(); ++i) {
			SelectedICB->extension.push_back(basic_ext[i]);
		}
		SelectedICB->read_codeLines(&codeLines);
		SelectedICB->create_codedata();
		SelectedICB->compile_codes();
		icmap.insert(ICMAP::value_type((char*)filename, SelectedICB));
	}
	else {
		SelectedICB = icmap[(char*)filename];
	}

	if (SelectedICB != nullptr) {
		if (SelectedICB->curErrMsg.Arr != nullptr && SelectedICB->curErrMsg[0] != 0) {
			currentErrorMsg.up = 0;
			currentErrorMsg.Arr[0] = 0;
			for (int i = 0; i < SelectedICB->curErrMsg.size(); ++i) {
				currentErrorMsg.push_back((wchar_t)SelectedICB->curErrMsg[i]);
			}
		}
	}
}

void ICB_Editor::Init(shp::rect4f _loc, InsideCode_Bake* sicb)
{
	basicInit(_loc);
	basicToolInit();
	icb_Init(sicb);
}

void ICB_Editor::Init(shp::rect4f _loc, const char* filename) {
	basicInit(_loc);
	basicToolInit();
	icb_Init(filename);
}

void ICB_Editor::CodelineEvent(DX_Event evt) {
	float slideDelta = 1.2f / codeLines.size();
	int max_express = loc.geth() / codeline_height;
	int minx = codeLines.size() * SliderRate;
	int maxx = max_express + codeLines.size() * SliderRate;
	if (maxx > codeLines.size()) maxx = codeLines.size();
	// input charactor in codeline event
	if (evt.message == WM_KEYDOWN) {
		wchar_t key = evt.wParam;
		bool bcharinput = 41 <= key && key <= 126;
		bool alphainput = 'A' <= key && key <= 'Z';
		bool numinput = '0' <= key && key <= '9';
		int casedelta = 'a' - 'A';
		if (bcharinput) {
			if (alphainput) {
				if (BeShift == false && CapsLock == false) {
					key += casedelta;
				}
				else if (CapsLock && BeShift) {
					key += casedelta;
				}
			}
			if (numinput && BeShift) {
				switch (key) {
				case L'0':
					key = L')';
					break;
				case L'1':
					key = L'!';
					break;
				case L'2':
					key = L'@';
					break;
				case L'3':
					key = L'#';
					break;
				case L'4':
					key = L'$';
					break;
				case L'5':
					key = L'%';
					break;
				case L'6':
					key = L'^';
					break;
				case L'7':
					key = L'&';
					break;
				case L'8':
					key = L'*';
					break;
				case L'9':
					key = L'(';
					break;
				}
			}
			codeLines[selected_codeline].insert(selected_charindex, key);
			codeLines[selected_codeline].c_str();
			++selected_charindex;
		}
		else if (((186 <= key && key <= 192) || key == 219) || (221 <= key && key <= 222)) {
			if (BeShift) {
				switch (key) {
				case 186:
					key = L':';
					break;
				case 187:
					key = L'+';
					break;
				case 188:
					key = L'<';
					break;
				case 189:
					key = L'_';
					break;
				case 190:
					key = L'>';
					break;
				case 191:
					key = L'?';
					break;
				case 192:
					key = L'~';
					break;
				case 219:
					key = L'{';
					break;
				case 221:
					key = L'}';
					break;
				case 222:
					key = L'\"';
					break;
				}
			}
			else {
				switch (key) {
				case 186:
					key = L';';
					break;
				case 187:
					key = L'=';
					break;
				case 188:
					key = L',';
					break;
				case 189:
					key = L'-';
					break;
				case 190:
					key = L'.';
					break;
				case 191:
					key = L'/';
					break;
				case 192:
					key = L'`';
					break;
				case 219:
					key = L'[';
					break;
				case 221:
					key = L']';
					break;
				case 222:
					key = L'\'';
					break;
				}
			}

			codeLines[selected_codeline].insert(selected_charindex, key);
			WhenCharInsert(selected_codeline, selected_charindex, key);
			codeLines[selected_codeline].c_str();
			++selected_charindex;
		}
		else if (evt.wParam == VK_BACK) {
			if (codeLines[selected_codeline].size() >= selected_charindex - 1) {
				--selected_charindex;
				BeforeCharErase(selected_codeline, selected_charindex);
				codeLines[selected_codeline].erase(selected_charindex);
			}
			else if (selected_charindex == 0 && selected_codeline != 0) {
				fmlwstr temp;
				temp.NULLState();
				temp.Init(8, false);
				temp = codeLines.at(selected_codeline);
				codeLines.at(selected_codeline).release();
				codeLines.erase(selected_codeline);
				WhenLineErase(selected_codeline);
				--selected_codeline;
				selected_charindex = codeLines.at(selected_codeline).size();
				for (int i = 0; i < temp.size(); ++i) {
					codeLines.at(selected_codeline).push_back(temp[i]);
				}
				temp.release();
			}
		}
		else if (evt.wParam == VK_SPACE) {
			codeLines[selected_codeline].insert(selected_charindex, L' ');
			codeLines[selected_codeline].c_str();
			++selected_charindex;
		}
		else if (evt.wParam == VK_RETURN) {
			//enter
			codeLines.insert(selected_codeline + 1, fmlwstr());
			WhenLineInsert(selected_codeline + 1);
			selected_codeline += 1;
			codeLines[selected_codeline].NULLState();
			codeLines[selected_codeline].Init(8, false);
			selected_charindex = 0;
		}
		else if (evt.wParam == VK_SHIFT) {
			BeShift = true;
		}
		else if (evt.wParam == VK_CAPITAL) {
			CapsLock != CapsLock;
		}
		else if (evt.wParam == VK_UP) {
			if (selected_codeline != 0) {
				shp::vec2i lim = GetExpendStart(selected_codeline - 1);
				if (lim.x >= 0 && GetExpend(lim.x) == 0) {
					selected_codeline = lim.x;
					selected_charindex = lim.y - 1;
					if (selected_charindex < 0) {
						selected_charindex = 0;
					}
				}
				else {
					unsigned int prevCL = selected_codeline;
					--selected_codeline;
					selected_charindex = (unsigned int)(float)(codeLines[selected_codeline].size() + 1) * ((float)selected_charindex / (float)(codeLines[prevCL].size() + 1));
				}
			}
		}
		else if (evt.wParam == VK_DOWN) {
			if (codeLines.size() > selected_codeline + 1) {
				shp::vec2i lim = GetExpendLimit(selected_codeline);
				if (lim.x >= 0 && GetExpend(selected_codeline) == 0) {
					selected_codeline = lim.x;
					selected_charindex = lim.y + 1;
					if (selected_charindex >= codeLines[selected_codeline].size()) {
						selected_codeline += 1;
						selected_charindex = 0;
					}
				}
				else {
					++selected_codeline;
					selected_charindex = (unsigned int)(float)(codeLines[selected_codeline].size() + 1) * ((float)selected_charindex / (float)(codeLines[selected_codeline - 1].size() + 1));
				}
			}
		}
		else if (evt.wParam == VK_LEFT) {
			if (inCtrl) {
				Xofffset -= 5;
				if (Xofffset < 0) Xofffset = 0;
			}
			else {
				if (selected_charindex != 0) {
					selected_charindex -= 1;
				}
				else {
					if (selected_codeline != 0) {
						shp::vec2i lim = GetExpendStart(selected_codeline - 1);
						if (lim.x >= 0 && GetExpend(lim.x) == 0) {
							selected_codeline = lim.x;
							selected_charindex = lim.y - 1;
							if (selected_charindex < 0) {
								selected_charindex = 0;
							}
						}
						else {
							selected_codeline -= 1;
							selected_charindex = codeLines[selected_codeline].size();
						}
					}
				}
			}
		}
		else if (evt.wParam == VK_RIGHT) {
			if (inCtrl) {
				Xofffset += 5;
			}
			else {
				if (selected_charindex < codeLines[selected_codeline].size()) {
					selected_charindex += 1;
				}
				else {
					if (selected_codeline < codeLines.size()) {
						shp::vec2i lim = GetExpendLimit(selected_codeline);
						if (lim.x >= 0 && GetExpend(selected_codeline) == 0) {
							selected_codeline = lim.x;
							selected_charindex = lim.y + 1;
							if (selected_charindex >= codeLines[selected_codeline].size()) {
								selected_codeline += 1;
								selected_charindex = 0;
							}
						}
						else {
							selected_codeline += 1;
							selected_charindex = 0;
						}
					}
				}
			}
		}
		else if (key == 9) {
			//TAB
			codeLines[selected_codeline].insert(selected_charindex, L' ');
			codeLines[selected_codeline].insert(selected_charindex, L' ');
			codeLines[selected_codeline].c_str();
			selected_charindex += 2;
		}
		else if (key == 17) {
			inCtrl = true;
		}

		if (selected_codeline > maxx) {
			SliderRate += slideDelta;
		}

		if (selected_codeline < minx) {
			SliderRate -= slideDelta;
		}
	}
	else if (evt.message == WM_KEYUP) {
		if (evt.wParam == VK_SHIFT) {
			BeShift = false;
		}
		else if (evt.wParam == 17) {
			inCtrl = false;
		}
	}

	if (evt.message == WM_LBUTTONDOWN) {
		shp::vec2f mpos = GetMousePos(evt.lParam);
		//codeline slider event
		shp::rect4f sliderLoc = shp::rect4f(loc.fx + loc.getw() * CodeEditorRate, loc.fy, loc.fx + loc.getw() * CodeEditorRate + SliderWidRate * codeline_height, headerRT.fy);
		if (shp::bPointInRectRange(mpos, sliderLoc)) {
			float x = sliderLoc.ly - mpos.y;
			x = x / sliderLoc.geth();
			SliderRate = x;
		}

		//codelines breakpoint add / delete events
		shp::rect4f additionalTabLoc = shp::rect4f(loc.fx + loc.getw() * CodeEditorRate + SliderWidRate * codeline_height, loc.fy, loc.lx, loc.ly);
		float fontsiz = codeline_height * 0.25f;
		shp::rect4f headerRT = shp::rect4f(loc.fx, loc.ly - headerRate * codeline_height, loc.lx, loc.ly);
		sliderLoc = shp::rect4f(loc.fx + loc.getw() * CodeEditorRate, loc.fy, loc.fx + loc.getw() * CodeEditorRate + SliderWidRate * codeline_height, headerRT.fy);
		shp::rect4f lineloc = shp::rect4f(codeEditorLoc.fx, headerRT.fy - 2 * codeline_height, codeEditorLoc.lx, headerRT.fy - codeline_height);
		lineloc.fy += codeline_height; lineloc.ly += codeline_height;
		shp::rect4f linenumloc = lineloc;
		lineloc.fx += codeline_height * linenumwid;

		int max_express = loc.geth() / codeline_height;
		int maxx = max_express + codeLines.size() * SliderRate;
		if (maxx > codeLines.size()) maxx = codeLines.size();
		int i = codeLines.size() * SliderRate;
		int smax = max_express;
		int subi = 0;
		int maxX = (int)(1.5f * codeEditorLoc.getw() / codeline_height);
		for (int s = 0; s < smax; ++s) {
			if (i >= codeLines.size()) break;

			shp::rect4f breakIrect = shp::rect4f(codeEditorLoc.fx, lineloc.fy, codeEditorLoc.fx + lineloc.geth(), lineloc.ly);
			if (shp::bPointInRectRange(mpos, breakIrect)) {
				bool bAdd = true;
				for (int i0 = 0; i0 < CodeLineBreakPoints.size(); ++i0) {
					if (CodeLineBreakPoints.at(i0) == i) {
						CodeLineBreakPoints.erase(i0);
						bAdd = false;
						break;
					}
				}
				if (bAdd) {
					CodeLineBreakPoints.push_back(i);
				}
			}

			wstring wstr;
			wstr = to_wstring(i);
			shp::vec2i lim = GetExpendLimit(i);
			bool hold = false;
			if (lim.x >= 0) {
				if (GetExpend(i) == 0) {
					hold = true;
					shp::rect4f expbtn = shp::rect4f(lineloc.fx - codeline_height, lineloc.ly - codeline_height, lineloc.fx, lineloc.ly);
					shp::rect4f minrect = shp::rect4f(expbtn.fx, expbtn.getCenter().y - codeline_height * 0.2f, expbtn.lx, expbtn.getCenter().y + codeline_height * 0.2f);
					shp::rect4f plsrect = shp::rect4f(expbtn.getCenter().x - codeline_height * 0.2f, expbtn.fy, expbtn.getCenter().x + codeline_height * 0.2f, expbtn.ly);

					i = lim.x;
					subi = lim.y + 1;
					if (codeLines[i].size() <= subi) {
						i += 1;
						subi = 0;
					}
				}
				else {
					shp::rect4f expbtn = shp::rect4f(lineloc.fx - codeline_height, lineloc.ly - codeline_height, lineloc.fx, lineloc.ly);
					shp::rect4f minrect = shp::rect4f(expbtn.fx, expbtn.getCenter().y - codeline_height * 0.2f, expbtn.lx, expbtn.getCenter().y + codeline_height * 0.2f);
				}
			}
			if (hold == false) {
				fmlwstr wfstr;
				wfstr.NULLState();
				wfstr.Init(codeLines[i].size(), false);
				for (int k = subi + Xofffset; k < codeLines[i].size(); ++k) {
					wfstr.push_back(codeLines[i][k]);
				}
				if (wfstr.up > maxX) wfstr.up = maxX;

				if (i != selected_codeline) {
				}
				else {
					shp::rect4f textcursorLoc = GetLoc_stringIndex(wfstr.Arr, wfstr.size(), fontsiz, lineloc, selected_charindex);
				}
				wfstr.release();

				++i;
				subi = 0;
			}
			lineloc.fy -= codeline_height * linemargin; lineloc.ly -= codeline_height * linemargin;
			linenumloc.fy -= codeline_height * linemargin; linenumloc.ly -= codeline_height * linemargin;
		}

		//codeline expending event
		lineloc = shp::rect4f(codeEditorLoc.fx, headerRT.fy - 2 * codeline_height, codeEditorLoc.lx, headerRT.fy - codeline_height);
		lineloc.fy += codeline_height; lineloc.ly += codeline_height;
		linenumloc = lineloc;
		lineloc.fx += codeline_height * linenumwid;
		max_express = loc.geth() / codeline_height;
		maxx = max_express + codeLines.size() * SliderRate;
		if (maxx > codeLines.size()) maxx = codeLines.size();
		i = codeLines.size() * SliderRate;
		smax = max_express;
		subi = 0;
		for (int s = 0; s < smax; ++s) {
			if (i >= codeLines.size()) break;
			shp::vec2i lim = GetExpendLimit(i);
			bool hold = false;
			if (lim.x >= 0) {
				shp::rect4f expbtn = shp::rect4f(lineloc.fx - codeline_height, lineloc.ly - codeline_height, lineloc.fx, lineloc.ly);
				if (shp::bPointInRectRange(mpos, expbtn)) {
					if (GetExpend(i) == 0) {
						SetExpend(i, true);
					}
					else {
						SetExpend(i, false);
					}
				}
			}
			if (hold == false) {
				++i;
			}
			lineloc.fy -= codeline_height * linemargin; lineloc.ly -= codeline_height * linemargin;
			linenumloc.fy -= codeline_height * linemargin; linenumloc.ly -= codeline_height * linemargin;
		}
	}
}
void ICB_Editor::CompileBtnEvent(){
	if (SelectedICB != nullptr) {
		bool b = SelectedICB->curErrMsg[0] == 0;
		SelectedICB->curErrMsg.up = 0;
		SelectedICB->curErrMsg[0] = 0;
		if (b) {
			SelectedICB->Release();
		}

		SelectedICB->init(40960);

		for (int i = 0; i < basic_ext.size(); ++i) {
			SelectedICB->extension.push_back(basic_ext[i]);
		}

		SelectedICB->read_codeLines(&codeLines);
		SelectedICB->create_codedata();
		SelectedICB->compile_codes();
	}
	else {
		SelectedICB = (InsideCode_Bake*)fm->_New(sizeof(InsideCode_Bake), true);
		SelectedICB->init(40960);
		for (int i = 0; i < basic_ext.size(); ++i)
		{
			SelectedICB->extension.push_back(basic_ext[i]);
		}

		SelectedICB->read_codeLines(&codeLines);
		SelectedICB->create_codedata();
		SelectedICB->compile_codes();
	}

	bool success = false;
	if (SelectedICB->curErrMsg.Arr != nullptr && SelectedICB->curErrMsg[0] != 0) {
		currentErrorMsg.up = 0;
		currentErrorMsg.Arr[0] = 0;
		for (int i = 0; i < SelectedICB->curErrMsg.size(); ++i) {
			currentErrorMsg.push_back((wchar_t)SelectedICB->curErrMsg[i]);
		}
	}
	else {
		currentErrorMsg.up = 0;
		currentErrorMsg.Arr[0] = 0;
		success = true;
	}

	//SelectedICB->curErrMsg.release();
}
void ICB_Editor::ImportBtnEvent(){
	wchar_t* loadfiledir = GetFileNameFromDlg_open();
	if (loadfiledir != nullptr) {
		string filename = wstr_to_utf8(loadfiledir);
		fm->_Delete((byte8*)loadfiledir, sizeof(wchar_t) * (wcslen(loadfiledir) + 1));
		if (icmap.find((char*)filename.c_str()) == icmap.end())
		{
			SelectedICB = (InsideCode_Bake*)fm->_New(sizeof(InsideCode_Bake), true);
			SelectedICB->init(40960);
			for (int i = 0; i < basic_ext.size(); ++i)
			{
				SelectedICB->extension.push_back(basic_ext[i]);
			}
			//dbg << "bake" << endl;
			SelectedICB->read_codes((char*)filename.c_str());
			SelectedICB->create_codedata();
			//SelectedICB->bake_code((char*)filename.c_str());

			bool success = false;
			if (SelectedICB->curErrMsg.Arr != nullptr && SelectedICB->curErrMsg[0] != 0) {
				currentErrorMsg.up = 0;
				currentErrorMsg.Arr[0] = 0;
				for (int i = 0; i < SelectedICB->curErrMsg.size(); ++i) {
					currentErrorMsg.push_back((wchar_t)SelectedICB->curErrMsg[i]);
				}
			}
			else {
				currentErrorMsg.up = 0;
				currentErrorMsg.Arr[0] = 0;
				success = true;
			}

			SelectedICB->curErrMsg.release();

			//dbg << "s0" << endl;
			if (success) {
				icmap.insert(ICMAP::value_type((char*)filename.c_str(), SelectedICB));
			}
			else {
				SelectedICB = nullptr;
			}
			//dbg << "s0" << endl;
		}
		else
		{
			SelectedICB = icmap[(char*)filename.c_str()];
		}

		if (SelectedICB == nullptr) {
			CodeLinesClear();

			PushNewCodeLine();
			int ss = 0;

			FILE* fp;
			fopen_s(&fp, (char*)filename.c_str(), "rt");
			if (fp)
			{
				int max = 0;
				fseek(fp, 0, SEEK_END);
				max = ftell(fp);
				fclose(fp);

				int stack = 0;
				fopen_s(&fp, (char*)filename.c_str(), "rt");
				int k = 0;
				while (k < max)
				{
					char c;
					c = fgetc(fp);
					if (c == '\n') {
						codeLines.push_back(fmlwstr());
						ss += 1;
						codeLines[ss].NULLState();
						codeLines[ss].Init(8, false);
						max -= 1;
					}
					else {
						codeLines[ss].push_back((wchar_t)c);
						WhenCharInsert(ss, codeLines.last().size() - 1, (wchar_t)c);
					}
					++k;
				}
			}

			return;
		}

		//update datas
		CodeLinesClear();

		codeLine_expend.up = 0;

		for (int i = 0; i < SelectedICB->csarr->size(); ++i) {
			code_sen* cs = SelectedICB->csarr->at(i);
			if (cs->ck != codeKind::ck_blocks) {
				codeLines.push_back(fmlwstr());
				codeLines.last().NULLState();
				codeLines.last().Init(8, false);
				//codeLine_expend.push_back(false);
				for (int k = 0; k < cs->maxlen; ++k) {
					char* word = cs->sen[k];
					int wlen = strlen(word);
					for (int u = 0; u < wlen; ++u) {
						codeLines.last().push_back((wchar_t)word[u]);
						//WhenCharInsert(codeLines.size() - 1, codeLines.last().size() - 1, word[u]);
					}
					codeLines.last().push_back(L' ');
					//WhenCharInsert(codeLines.size() - 1, codeLines.last().size() - 1, word[u]);
				}

				if (cs->ck != codeKind::ck_addFunction && (cs->ck != codeKind::ck_while && cs->ck != codeKind::ck_if)) {
					codeLines.last().push_back(L';');
					//WhenCharInsert(codeLines.size() - 1, codeLines.last().size() - 1, word[u]);
				}

				codeLines.last().c_str();
			}
			else {
				ImportBlockToCodeLine(cs);
			}
		}
	}
}
void ICB_Editor::FunctionPageEvent(shp::rect4f funcbtnLoc, shp::vec2f mpos) {
	// function page event
	if (SelectedCXT >= 0) {
		constexpr float ymargin = 30;
		shp::vec2f stackp = shp::vec2f(additionalTabLoc.fx, funcbtnLoc.fy);
		float height = funcbtnLoc.fy - additionalTabLoc.fy;

		ICB_Context* ecs = ecss.at(SelectedCXT);

		byte8* sps = ecs->mem + ecs->max_mem_byte - 1;

		wchar_t funcstr[128] = {};

		for (int i = 0; i < ecs->icb->globalVariables.size(); ++i) {
			// render global Variables
			NamingData* nd = ecs->icb->globalVariables.at(i);
			byte8* vptr = &ecs->datamem.at(nd->add_address);
			stackp = EventVariableState(*nd, stackp, additionalTabLoc, vptr, -1, mpos);
			stackp.y -= ymargin / 2.0f;
		}

		for (int i = ecs->call_stack.size() - 1; i >= 0; --i) {
			//select function of call_stack context.
			si64 m = 1 << 30;
			func_data* fd = nullptr;

			for (int k = 0; k < ecs->icb->functions.size(); ++k) {
				si64 d = (si64)ecs->icb->functions.at(k)->start_pc - (si64)ecs->call_stack.at(k);
				if (m > d) {
					m = d;
					fd = ecs->icb->functions.at(k);
				}
			}

			shp::rect4f cloc = shp::rect4f(stackp.x, stackp.y - ymargin, additionalTabLoc.lx, stackp.y);
			stackp.y -= ymargin;

			if (shp::bPointInRectRange(mpos, cloc)) {
				if (isVariableTableShowExpending(ecs->call_stack.at(i))) {
					ShrinkingVariableTable(ecs->call_stack.at(i));
				}
				else {
					ExpendVariableTable(ecs->call_stack.at(i));
				}
			}

			if (isVariableTableShowExpending(ecs->call_stack.at(i))) {
				fmvecarr<NamingData> ndarr = ecs->icb->GetLocalVariableListInFunction(fd);

				for (int i = 0; i < ndarr.size(); ++i) {
					//render variable table object
					NamingData nd = ndarr.at(i);
					byte8* vptr = sps - nd.add_address - nd.td->typesiz;
					stackp = EventVariableState(nd, stackp, additionalTabLoc, vptr, -1, mpos);
					stackp.y -= ymargin;
				}

				ndarr.release();
			}
		}

		si64 m = 1 << 30;
		func_data* fd = nullptr;

		for (int k = 0; k < ecs->icb->functions.size(); ++k) {
			si64 d = (si64)ecs->icb->functions.at(k)->start_pc - (si64)ecs->pc;
			if (m > d) {
				m = d;
				fd = ecs->icb->functions.at(k);
			}
		}

		fmvecarr<NamingData> ndarr = ecs->icb->GetLocalVariableListInFunction(fd);
		//present context function stack render
		for (int i = 0; i < fd->param_data.size(); ++i) {
			//render variable table object
			NamingData nd = ndarr.at(i);
			byte8* vptr = sps - nd.add_address - nd.td->typesiz;
			stackp = EventVariableState(nd, stackp, additionalTabLoc, vptr, -1, mpos);
			stackp.y -= ymargin;
		}
		ndarr.release();
	}
}
void ICB_Editor::ContextPageEvent(shp::rect4f funcbtnLoc, shp::vec2f mpos){
	// context page event
	fmvecarr<ui32> cxtindexes; // list context using this icb.
	cxtindexes.NULLState();
	cxtindexes.Init(8, false, false);
	for (int i = 0; i < ecss.size(); ++i) {
		if (ecss.at(i)->icb == SelectedICB) {
			cxtindexes.push_back(i);
		}
	}

	for (int i = 0; i < cxtindexes.size(); ++i) {
		shp::rect4f cxtrt = shp::rect4f(additionalTabLoc.fx + atmargin, funcbtnLoc.fy - atmargin - (i + 1) * 30, additionalTabLoc.lx - 2 * atmargin, funcbtnLoc.fy - atmargin - i * 30);
		if (shp::bPointInRectRange(mpos, cxtrt)) {
			SelectedCXT = cxtindexes.at(i);
		}

		ICB_Context* cxt = ecss.at(cxtindexes.at(i));
		cxtrt.fx = cxtrt.lx - atmargin * 2;
		if (shp::bPointInRectRange(mpos, cxtrt)) {
			if (cxt->isDbg) cxt->isDbg = false;
			else cxt->isDbg = true;
		}
		cxtrt.lx = cxtrt.fx - 10;
		cxtrt.fx = cxtrt.fx - atmargin * 6;
		if (shp::bPointInRectRange(mpos, cxtrt)) {
			if (cxt->isBreaking) {
				cxt->isBreaking = false;
				cxt->stop_callstack = -1;
			}
			else cxt->isBreaking = true;
		}
	}

	cxtindexes.release();
}
void ICB_Editor::DbgToolBoxEvent(shp::vec2f mpos) {
	shp::rect4f dbgtoolbox_rt = shp::rect4f(codeEditorLoc.lx - 200, headerRT.fy - 50, codeEditorLoc.lx, headerRT.fy);
	float dbgtbw = dbgtoolbox_rt.getw() / 5;
	shp::rect4f dbgtoolbox_playrt = shp::rect4f(dbgtoolbox_rt.fx, dbgtoolbox_rt.fy, dbgtoolbox_rt.fx + dbgtbw, dbgtoolbox_rt.ly);
	shp::rect4f dbgtoolbox_pausert = shp::rect4f(dbgtoolbox_playrt.lx, dbgtoolbox_rt.fy, dbgtoolbox_playrt.lx + dbgtbw, dbgtoolbox_rt.ly);
	shp::rect4f dbgtoolbox_Stoprt = shp::rect4f(dbgtoolbox_pausert.lx, dbgtoolbox_rt.fy, dbgtoolbox_pausert.lx + dbgtbw, dbgtoolbox_rt.ly);
	shp::rect4f dbgtoolbox_steprt = shp::rect4f(dbgtoolbox_Stoprt.lx, dbgtoolbox_rt.fy, dbgtoolbox_Stoprt.lx + dbgtbw, dbgtoolbox_rt.ly);
	shp::rect4f dbgtoolbox_stepinrt = shp::rect4f(dbgtoolbox_steprt.lx, dbgtoolbox_rt.fy, dbgtoolbox_steprt.lx + dbgtbw, dbgtoolbox_rt.ly);
	ICB_Context* ecs = ecss.at(SelectedCXT);
	DX11Color BreakC;
	DX11Color PlayC;
	if (ecs->isBreaking) {
		if (shp::bPointInRectRange(mpos, dbgtoolbox_playrt)) {
			//play btn
			ecs->isBreaking = false;
			ecs->stopnum = -1;
			ecs->stop_callstack = -1;
		}
		else if (shp::bPointInRectRange(mpos, dbgtoolbox_steprt)) {
			//step btn

			int present_codeline = ecs->pc - ecs->codemem;
			//find next code_sen
			bool bejmp = false;
			code_sen* cs = ecs->icb->find_codesen_with_linenum(ecs->icb->csarr, present_codeline);
			for (int i = present_codeline; i <= cs->end_line; ++i) {
				//if
				if (ecs->codemem[i] == (byte8)insttype::IT_JMP) {
					int nextstopnum = *(unsigned int*)&ecs->codemem[i + 1];
					ecs->stopnum = nextstopnum;
					ecs->stop_callstack = ecs->call_stack.size();
					ecs->isBreaking = false;
					bejmp = true;
					break;
				}
				if (InsideCode_Bake::inst_meta[ecs->codemem[i]].param_num < 0)
				{
					return;
				}
				if (ecs->codemem[i] == (byte8)insttype::IT_SET_A_CONST_STRING || ecs->codemem[i] == (byte8)insttype::IT_SET_B_CONST_STRING)
				{
					uint strmax = 0;
					instruct_data id = InsideCode_Bake::inst_meta[ecs->codemem[i]];
					for (int k = 0; k < id.param_num; ++k)
					{
						i += id.param_typesiz[k];
					}
					i += strmax + 1;
				}
				else {
					instruct_data id = InsideCode_Bake::inst_meta[ecs->codemem[i]];
					for (int k = 0; k < id.param_num; ++k)
					{
						i += id.param_typesiz[k];
					}
				}
			}
			if (bejmp == false) {
				int next_codeline = cs->end_line;
			ICBE_EVENT_DBGTOOLBOX_STEP_GETNEXTCODELINE:
				code_sen* ncs = ecs->icb->find_codesen_with_linenum(ecs->icb->csarr, next_codeline);
				if (ncs == cs || ncs == nullptr) {
					next_codeline += 1;
					goto ICBE_EVENT_DBGTOOLBOX_STEP_GETNEXTCODELINE;
				}
				else if (ncs != cs) {
					for (int i = cs->end_line + 1; i <= ncs->end_line; ++i) {
						//if
						if (ecs->codemem[i] == (byte8)insttype::IT_JMP) {
							int nextstopnum = *(unsigned int*)&ecs->codemem[i + 1];
							ecs->stopnum = nextstopnum;
							ecs->stop_callstack = ecs->call_stack.size();
							ecs->isBreaking = false;
							bejmp = true;
							break;
						}
						if (InsideCode_Bake::inst_meta[ecs->codemem[i]].param_num < 0)
						{
							return;
						}
						if (ecs->codemem[i] == (byte8)insttype::IT_SET_A_CONST_STRING || ecs->codemem[i] == (byte8)insttype::IT_SET_B_CONST_STRING)
						{
							uint strmax = 0;
							instruct_data id = InsideCode_Bake::inst_meta[ecs->codemem[i]];
							for (int k = 0; k < id.param_num; ++k)
							{
								i += id.param_typesiz[k];
							}
							i += strmax + 1;
						}
						else {
							instruct_data id = InsideCode_Bake::inst_meta[ecs->codemem[i]];
							for (int k = 0; k < id.param_num; ++k)
							{
								i += id.param_typesiz[k];
							}
						}
					}
				}
				if (bejmp == false) {
					ecs->stopnum = ncs->start_line;
					ecs->stop_callstack = ecs->call_stack.size();
					ecs->isBreaking = false;
				}
			}
		}
		else if (shp::bPointInRectRange(mpos, dbgtoolbox_stepinrt)) {

			//step in btn
			int present_codeline = ecs->pc - ecs->codemem;
			//if present codesen is use function type, step in
			code_sen* cs = ecs->icb->find_codesen_with_linenum(ecs->icb->csarr, present_codeline);
			bool bejmp = false;
			// just find funcjmp inst in start ~ endline -> operand of funcjmp is stopnum
			for (int i = present_codeline; i <= cs->end_line; ++i) {
				//if
				if (ecs->codemem[i] == (byte8)insttype::IT_JMP) {
					int nextstopnum = *(unsigned int*)&ecs->mem[i + 1];
					ecs->stopnum = nextstopnum;
					ecs->stop_callstack = ecs->call_stack.size();
					ecs->isBreaking = false;
					bejmp = true;
					break;
				}
				if (ecs->codemem[i] == (byte8)insttype::IT_FUNCJMP) {
					int nextstopnum = *(unsigned int*)&ecs->codemem[i + 1];
					ecs->stopnum = nextstopnum;
					ecs->stop_callstack = ecs->call_stack.size() + 1;
					ecs->isBreaking = false;
					bejmp = true;
					break;
				}
				if (InsideCode_Bake::inst_meta[ecs->codemem[i]].param_num < 0)
				{
					return;
				}
				if (ecs->mem[i] == (byte8)insttype::IT_SET_A_CONST_STRING || ecs->codemem[i] == (byte8)insttype::IT_SET_B_CONST_STRING)
				{
					uint strmax = 0;
					instruct_data id = InsideCode_Bake::inst_meta[ecs->codemem[i]];
					for (int k = 0; k < id.param_num; ++k)
					{
						i += id.param_typesiz[k];
					}
					i += strmax + 1;
				}
				else {
					instruct_data id = InsideCode_Bake::inst_meta[ecs->codemem[i]];
					for (int k = 0; k < id.param_num; ++k)
					{
						i += id.param_typesiz[k];
					}
				}
			}
			//else step
			//find next code_sen
			if (bejmp == false) {
				int next_codeline = cs->end_line;
			ICBE_EVENT_DBGTOOLBOX_STEPIN_GETNEXTCODELINE:
				code_sen* ncs = ecs->icb->find_codesen_with_linenum(ecs->icb->csarr, next_codeline);
				if (ncs == cs || ncs == nullptr) {
					next_codeline += 1;
					goto ICBE_EVENT_DBGTOOLBOX_STEPIN_GETNEXTCODELINE;
				}
				else if (ncs != cs) {
					for (int i = cs->end_line + 1; i <= ncs->end_line; ++i) {
						//if
						if (ecs->codemem[i] == (byte8)insttype::IT_JMP) {
							int nextstopnum = *(unsigned int*)&ecs->codemem[i + 1];
							ecs->stopnum = nextstopnum;
							ecs->stop_callstack = ecs->call_stack.size();
							ecs->isBreaking = false;
							bejmp = true;
							break;
						}
						if (ecs->codemem[i] == (byte8)insttype::IT_FUNCJMP) {
							int nextstopnum = *(unsigned int*)&ecs->codemem[i + 1];
							ecs->stopnum = nextstopnum;
							ecs->stop_callstack = ecs->call_stack.size() + 1;
							ecs->isBreaking = false;
							bejmp = true;
							break;
						}
						if (InsideCode_Bake::inst_meta[ecs->codemem[i]].param_num < 0)
						{
							return;
						}
						if (ecs->codemem[i] == (byte8)insttype::IT_SET_A_CONST_STRING || ecs->codemem[i] == (byte8)insttype::IT_SET_B_CONST_STRING)
						{
							uint strmax = 0;
							instruct_data id = InsideCode_Bake::inst_meta[ecs->codemem[i]];
							for (int k = 0; k < id.param_num; ++k)
							{
								i += id.param_typesiz[k];
							}
							i += strmax + 1;
						}
						else {
							instruct_data id = InsideCode_Bake::inst_meta[ecs->codemem[i]];
							for (int k = 0; k < id.param_num; ++k)
							{
								i += id.param_typesiz[k];
							}
						}
					}
				}
				if (bejmp == false) {
					ecs->stopnum = ncs->start_line;
					ecs->stop_callstack = ecs->call_stack.size();
					ecs->isBreaking = false;
				}
			}
		}
	}
	else {
		if (shp::bPointInRectRange(mpos, dbgtoolbox_pausert)) {
			//pause btn
			ecs->isBreaking = true;
		}
		else if (shp::bPointInRectRange(mpos, dbgtoolbox_Stoprt)) {
			//stop btn
			ecs->isBreaking = true;
			ecs->isDbg = false;
		}
	}
}

void ICB_Editor::Event(DX_Event evt) {
	if (befold == false) {
		shp::rect4f headerRT = shp::rect4f(loc.fx, loc.ly - headerRate * codeline_height, loc.lx, loc.ly);
		if (evt.message == WM_LBUTTONDOWN) {
			shp::vec2f mpos = GetMousePos(evt.lParam);
			if (shp::bPointInRectRange(mpos, loc)) {
				focus = true;
			}
			else {
				focus = false;
			}

			if (shp::bPointInRectRange(mpos, headerRT)) {
				isMoving = true;
				movOffset = shp::rect4f(mpos.x - loc.fx, mpos.y - loc.fy, mpos.x - loc.lx, mpos.y - loc.ly);
			}

			shp::rect4f expendbtnloc = shp::rect4f(loc.lx - 50, loc.fy, loc.lx, loc.fy + 50);
			if (shp::bPointInRectRange(mpos, expendbtnloc)) {
				isExpend = true;
			}

			shp::rect4f sliderLoc = shp::rect4f(loc.fx + loc.getw() * CodeEditorRate, loc.fy, loc.fx + loc.getw() * CodeEditorRate + SliderWidRate * codeline_height, headerRT.fy);

			shp::rect4f additionalTabLoc = shp::rect4f(loc.fx + loc.getw() * CodeEditorRate + SliderWidRate * codeline_height, loc.fy, loc.lx, loc.ly);
			shp::rect4f CompasbtnLoc = shp::rect4f(additionalTabLoc.lx, additionalTabLoc.ly - 2 * codeline_height, additionalTabLoc.lx + 4 * codeline_height, additionalTabLoc.ly);
			if (shp::bPointInRectRange(mpos, CompasbtnLoc)) {
				if (tools[0].ecs->ExeState) {
					tools[0].ecs->ExeState = false;
					using_tool = false;
				}
				else {
					tools[0].ecs->ExeState = true;
					using_tool = true;
				}
			}

			if (focus) {
				
			}
		}

		if (evt.message == WM_LBUTTONUP) {
			isMoving = false;
			isExpend = false;
		}

		if (evt.message == WM_MOUSEMOVE) {
			shp::vec2f mpos = GetMousePos(evt.lParam);
			if (isMoving && press_ef) {
				loc.fx = mpos.x - movOffset.fx;
				loc.fy = mpos.y - movOffset.fy;
				loc.lx = mpos.x - movOffset.lx;
				loc.ly = mpos.y - movOffset.ly;
			}

			shp::rect4f sliderLoc = shp::rect4f(loc.fx + loc.getw() * CodeEditorRate, loc.fy, loc.fx + loc.getw() * CodeEditorRate + SliderWidRate * codeline_height, headerRT.fy);
			if (focus && press_ef) {
				if (shp::bPointInRectRange(mpos, sliderLoc)) {
					float x = sliderLoc.ly - mpos.y;
					x = x / sliderLoc.geth();
					SliderRate = x;
				}
			}

			if (isExpend && press_ef) {
				loc.lx = mpos.x;
				loc.fy = mpos.y;
			}
		}

		if (focus) {
			CodelineEvent(evt);
			
			if (evt.message == WM_LBUTTONDOWN) {
				shp::vec2f mpos = GetMousePos(evt.lParam);

				// side tab btns event
				shp::rect4f funcbtnLoc = shp::rect4f(additionalTabLoc.fx + atmargin, additionalTabLoc.ly - 70, additionalTabLoc.fx + 7 * atmargin, additionalTabLoc.ly - 10);
				shp::rect4f cxtbtnLoc = shp::rect4f(additionalTabLoc.fx + 9 * atmargin, additionalTabLoc.ly - 70, additionalTabLoc.fx + 15 * atmargin, additionalTabLoc.ly - 10);
				shp::rect4f importbtnLoc = shp::rect4f(additionalTabLoc.fx + atmargin, additionalTabLoc.fy, additionalTabLoc.fx + 10 * atmargin, additionalTabLoc.fy + 40);
				shp::rect4f compilebtnLoc = shp::rect4f(additionalTabLoc.fx + atmargin, additionalTabLoc.fy + 50, additionalTabLoc.fx + 10 * atmargin, additionalTabLoc.fy + 90);
				shp::rect4f foldbtnLoc = shp::rect4f(loc.fx, loc.ly - 2 * codeline_height, loc.fx + 2 * codeline_height, loc.ly);

				if (shp::bPointInRectRange(mpos, funcbtnLoc)) {
					showVarmod = true;
				}

				if (shp::bPointInRectRange(mpos, cxtbtnLoc)) {
					showVarmod = false;
				}

				if (showVarmod) {
					FunctionPageEvent(funcbtnLoc, mpos);
				}
				else {
					ContextPageEvent(funcbtnLoc, mpos);
				}

				//import btn event
				if (shp::bPointInRectRange(mpos, importbtnLoc)) {
					ImportBtnEvent();
				}

				//compile btn event
				if (shp::bPointInRectRange(mpos, compilebtnLoc)) {
					CompileBtnEvent();
				}

				// window folding event
				if (shp::bPointInRectRange(mpos, foldbtnLoc)) {
					//showVarmod = false;
					//fold
					befold = true;
					shp::vec2f pc = *(shp::vec2f*)&mainpage->pfm.Data[(int)mainpm::present_center];
					object_position = GetObjectPos(shp::vec2f(loc.fx, loc.ly)).getv2();
				}

				//debuging tool box event
				if ((SelectedCXT >= 0 && ecss.size() > SelectedCXT) && ecss.at(SelectedCXT)->isDbg) {
					DbgToolBoxEvent(mpos);
				}
			}

			
		}
	}
	else {
		// when editor window fold.
		if (evt.message == WM_LBUTTONDOWN) {
			shp::vec2f mpos = GetMousePos(evt.lParam);
			shp::rect4f ploc = shp::rect4f(object_position.x - codeline_height, object_position.y - codeline_height, object_position.x + codeline_height, object_position.y + codeline_height);
			shp::vec2f pc = *(shp::vec2f*)&mainpage->pfm.Data[(int)mainpm::present_center];
			shp::vec2f viewpos = GetObjectPos(shp::vec2f(mpos.x, mpos.y)).getv2();
			if (shp::bPointInRectRange(viewpos, ploc)) {
				//showVarmod = false;
				//fold
				befold = false;
				float w = loc.getw();
				float h = loc.geth();
				shp::vec2f scpos = GetScreenPos(shp::vec3f(viewpos.x, viewpos.y, 0.0f));
				loc.fx = scpos.x;
				loc.ly = scpos.y;
				loc.lx = loc.fx + w;
				loc.fy = loc.ly - h;
			}
		}
	}
}

fmvecarr<ICB_Editor*> icbe_pool;

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

extern FM_System0* fm;
bool FinishInit = false;
//ICB_Editor icbE;

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

	shp::rect4f loc = btn->sup()->loc;
	loc.fx += loc.getw() / 4.0f;
	loc.fy += loc.geth() / 4.0f;

	rb->render(cb);
	draw_string(btn->text, wcslen(btn->text), 15.0f * expendrate, loc, DX11Color(1.0f, 1.0f, 1.0f, 1.0f));
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

	if (evt.message == WM_LBUTTONDOWN)
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
	linedrt->render(cb);
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

	if (evt.message == WM_LBUTTONDOWN)
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
	float layer_uitext = **(float**)&mainpage->pfm.Data[(int)mainpm::layer_uitext];
	float layer_ui = **(float**)&mainpage->pfm.Data[(int)mainpm::layer_ui];

	rbuffer* rb = (rbuffer*)btn->param[0];
	shp::vec2f* flow = (shp::vec2f*)btn->param[1];
	bool* enable = (bool*)btn->param[2];

	float rate = AnimClass::EaseOut(flow->x / flow->y, 5.0f);
	shp::vec2f cen = btn->sup()->loc.getCenter();

	float expendrate = 0.6f + 0.5f * rate;
	ConstantBuffer cb = GetBasicModelCB(shp::vec3f(cen.x, cen.y, layer_ui),
		shp::vec3f(0, 0, 0), shp::vec3f(btn->sup()->loc.getw() * expendrate, 
			btn->sup()->loc.geth() * expendrate, 1), DX11Color(1.0f, 1.0f, 1.0f, 1.0f));
	

	expendrate = 0.5f + 0.5f * rate;
	ConstantBuffer cb2 = GetBasicModelCB(shp::vec3f(cen.x, cen.y, layer_ui), shp::vec3f(0, 0, 0),
		shp::vec3f(btn->sup()->loc.getw() * expendrate,
			btn->sup()->loc.geth() * expendrate, 1), DX11Color(1.0f, 1.0f, 1.0f, 1.0f));
	
	shp::rect4f loc = btn->sup()->loc;
	loc.fx += loc.getw() / 4.0f;
	loc.fy += loc.geth() / 4.0f;
	
	rb->render(cb2);
	draw_string(btn->text, wcslen(btn->text), 20.0f * expendrate, loc, DX11Color(1.0f, 1.0f, 1.0f, 1.0f), layer_uitext);
	if (*enable)
	{
		linedrt->render(cb);
	}
}

void freepolybtn_event(DXBtn* btn, DX_Event evt)
{
	shp::vec2f* flow = (shp::vec2f*)btn->param[1];
	bool* enable = (bool*)btn->param[2];

	if (evt.message == WM_LBUTTONDOWN)
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
				else if (*btn->param[4] == 2)
				{
					// makeobj
					if (mainSprite == nullptr)
					{
						dbgcount(0, dbg << "mainSprite_init" << endl);
						mainSprite = (Sprite*)fm->_New(sizeof(Sprite), true);
						mainSprite->null();
						mainSprite->st = sprite_type::st_objects;
						mainSprite->data.objs =
							(fmvecarr < int*>*)fm->_New(sizeof(fmvecarr < int*>), true);
						mainSprite->data.objs->NULLState();
						mainSprite->data.objs->Init(8, false, true);
					}
					Object* newobj = (Object*)fm->_New(sizeof(Object), true);
					newobj->source = basicSprite;
					newobj->pos = shp::vec3f(0, 0, 0);
					newobj->rot = shp::vec3f(0, 0, 0);
					newobj->sca = shp::vec3f(1, 1, 1);
					mainSprite->data.objs->push_back(reinterpret_cast <int*>(newobj));
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

wchar_t currentEditTextData[256];
HWND EditTextHandle;
bool EditTextForMakeObject = false;

void CreateTextObjectToMainSprite() {
	if (mainSprite->st != sprite_type::st_objects) {
		return;
	}

	int len = wcslen(currentEditTextData);
	Object* obj = (Object*)fm->_New(sizeof(Object), true);
	obj->null();
	Sprite* spr = (Sprite*)fm->_New(sizeof(Sprite), true);
	spr->null();
	spr->st = sprite_type::st_objects;
	spr->data.objs = (fmvecarr<int*>*)fm->_New(sizeof(fmvecarr<int*>), true);
	spr->data.objs->NULLState();

	obj->source = spr;
	//obj->sca = shp::vec3f(0.1f, 0.1f, 0.1f);
	int cap = 0;
	for (int i = 0; i < len; ++i) {
		wchar_t c = currentEditTextData[i];
		if (char_map.find((unsigned int)c) == char_map.end())
		{
			CharBuffer* cbuf = (CharBuffer*)fm->_New(sizeof(CharBuffer), true);
			cbuf->ready((unsigned int)c, g_pVertexShader, g_pPixelShader);
			char_map.insert(CharMap::value_type((unsigned int)c, cbuf));
		}
		CharBuffer* cBuff = char_map[c];
		cap += cBuff->frag.size();
	}
	spr->data.objs->Init(cap, false, true);

	shp::vec3f stackPos = shp::vec3f(0, 0, 0);
	for (int i = 0; i < len; ++i) {
		wchar_t c = currentEditTextData[i];
		CharBuffer* cBuff = char_map[c];
		for (int k = 0; k < cBuff->frag.size(); ++k) {
			rbuffer* rb = cBuff->frag.at(k);
			rbuffer* crb = rb->copy_new_rbuff();
			Sprite* cspr = (Sprite*)fm->_New(sizeof(Sprite), true);
			cspr->null();
			cspr->st = sprite_type::st_freepolygon;
			cspr->data.freepoly = crb;
			Object* cobj = (Object*)fm->_New(sizeof(Object), true);
			cobj->null();
			cobj->pos = stackPos;
			cobj->source = cspr;
			spr->data.objs->push_back((int*)cobj);
		}
		stackPos.x += cBuff->range.getw();
	}
	
	mainSprite->data.objs->push_back((int*)obj);
}

si64 CALLBACK TextDataDialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
	case WM_INITDIALOG:
		EditTextHandle = GetDlgItem(hDlg, IDC_EDIT1);
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK:
			GetWindowText(EditTextHandle, currentEditTextData, 256);
			currentEditTextData[255] = 0;

			if (EditTextForMakeObject) {
				CreateTextObjectToMainSprite();
			}

			EndDialog(hDlg, TRUE);
			break;
		case IDCANCEL:
			currentEditTextData[0] = 0;
			EndDialog(hDlg, TRUE);
			break;
		}
	}
}

void textobjbtn_event(DXBtn* btn, DX_Event evt) {
	shp::vec2f* flow = (shp::vec2f*)btn->param[1];

	if (evt.message == WM_LBUTTONDOWN)
	{
		shp::vec2f mpos = GetMousePos(evt.lParam);
		// dbg << "mpos : " << x << ", " << y << endl;
		if (shp::bPointInRectRange(shp::vec2f(mpos.x, mpos.y), btn->sup()->loc) && press_ef)
		{
			press_ef = false;
			flow->x = 0;
			// operate
			EditTextForMakeObject = true;
			DialogBox(g_hInst, MAKEINTRESOURCE(IDD_TEXTDATA_DIALOG), g_hWnd, TextDataDialogProc);
		}
	}
}

void savebtn_event(DXBtn* btn, DX_Event evt)
{
	shp::vec2f* flow = (shp::vec2f*)btn->param[1];
	bool* enable = (bool*)btn->param[2];

	if (evt.message == WM_LBUTTONDOWN)
	{
		shp::vec2f mpos = GetMousePos(evt.lParam);
		
		if (shp::bPointInRectRange(shp::vec2f(mpos.x, mpos.y), btn->sup()->loc) && press_ef)
		{
			press_ef = false;
			flow->x = 0;
			/*wchar_t** wptr = (wchar_t**)&texteditpage->pfm.Data[(int)texteditpm::deststring];
			*wptr = (wchar_t*)&mainpage->pfm.Data[(int)mainpm::savefilestr];
			pagestacking(texteditpage);*/

			wchar_t* savefiledir = GetFileNameFromDlg_save();
			if (savefiledir != nullptr) {
				mainSprite->save(savefiledir);
				fm->_Delete((byte8*)savefiledir, sizeof(wchar_t) * (1 + wcslen(savefiledir)));
			}
		}
	}
}

void translatebtn_event(DXBtn* btn, DX_Event evt)
{
	shp::vec2f* flow = (shp::vec2f*)btn->param[1];
	bool* enable = (bool*)btn->param[2];

	if (evt.message == WM_LBUTTONDOWN)
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

	if (evt.message == WM_LBUTTONDOWN)
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
				fmvecarr<SimpleVertex>* bptr = ap->buffer[ap->get_renderChoice()];
				SimpleVertex* farr = (SimpleVertex*)fm->_tempNew(bptr->size() * sizeof(SimpleVertex));
				for (int i = 0; i < bptr->size(); ++i)
				{
					farr[i] = bptr->at(i);
				}
				int vsiz = ap->get_vertexsiz(ap->get_renderChoice());
				// dbgcount(0, dbg << "ap vertex size : " <<
				// ap->get_vertexsiz(ap->get_renderChoice()) << endl)
				ap->clear();
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
						shp::vec3f p = shp::vec3f(farr[i].Pos.x, farr[i].Pos.y, farr[i].Pos.z);
						DX11Color c = DX11Color(farr[i].Color.x, farr[i].Color.y, farr[i].Color.z, farr[i].Color.w);
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
	float layer_uitext = **(float**)&mainpage->pfm.Data[(int)mainpm::layer_uitext];
	float layer_ui = **(float**)&mainpage->pfm.Data[(int)mainpm::layer_ui];

	float rate = AnimClass::EaseOut(flow->x / flow->y, 5.0f);
	shp::vec2f cen = btn->sup()->loc.getCenter();
	float expendrate = 0.5f + 0.5f * rate;
	ConstantBuffer cb = GetBasicModelCB(shp::vec3f(cen.x, cen.y, layer_ui), shp::vec3f(0, 0, 0),
		shp::vec3f(btn->sup()->loc.getw() * expendrate,
			btn->sup()->loc.geth() * expendrate, 1), DX11Color(1.0f, 1.0f, 1.0f, 1.0f));
	
	shp::rect4f loc = btn->sup()->loc;
	loc.fx += loc.getw() / 4.0f;
	loc.fy += loc.geth() / 4.0f;
	
	rb->render(cb);
	draw_string(btn->text, wcslen(btn->text), 20.0f * expendrate, loc, DX11Color(1.0f, 1.0f, 1.0f, 1.0f), layer_uitext);
}

void objselectbtn_event(DXBtn* btn, DX_Event evt)
{
	shp::vec2f* flow = (shp::vec2f*)btn->param[1];
	shp::vec2i* mod = (shp::vec2i*)btn->param[2];

	if (evt.message == WM_LBUTTONDOWN)
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

	if (evt.message == WM_LBUTTONDOWN)
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

	if (evt.message == WM_LBUTTONDOWN)
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

	if (evt.message == WM_LBUTTONDOWN)
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

	if (evt.message == WM_LBUTTONDOWN)
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

void tabbtn_event(DXBtn* btn, DX_Event evt)
{
	shp::vec2f* flow = (shp::vec2f*)btn->param[1];

	if (evt.message == WM_LBUTTONDOWN)
	{
		shp::vec2f mpos = GetMousePos(evt.lParam);
		// dbg << "mpos : " << x << ", " << y << endl;
		if (shp::bPointInRectRange(shp::vec2f(mpos.x, mpos.y), btn->sup()->loc) && press_ef)
		{
			press_ef = false;
			flow->x = 0;
			unsigned int* a = (unsigned int*)&mainpage->pfm.Data[(int)mainpm::tabselect_id];
			if (*btn->param[2] == 0)
			{
				*a = 0;
			}
			else
			{
				*a = 1;
			}
			// operate
		}
	}
}

void sprdirbtn_event(DXBtn* btn, DX_Event evt)
{
	shp::vec2f* flow = (shp::vec2f*)btn->param[1];

	if (evt.message == WM_LBUTTONDOWN)
	{
		shp::vec2f mpos = GetMousePos(evt.lParam);
		// dbg << "mpos : " << x << ", " << y << endl;
		if (shp::bPointInRectRange(shp::vec2f(mpos.x, mpos.y), btn->sup()->loc) && press_ef)
		{
			press_ef = false;
			flow->x = 0;
			// operate
			sprloadmod = 'd';
			pagestacking(filepage);
		}
	}
}

void loadfromfilebtn_event(DXBtn* btn, DX_Event evt)
{
	shp::vec2f* flow = (shp::vec2f*)btn->param[1];

	if (evt.message == WM_LBUTTONDOWN)
	{
		shp::vec2f mpos = GetMousePos(evt.lParam);
		// dbg << "mpos : " << x << ", " << y << endl;
		if (shp::bPointInRectRange(shp::vec2f(mpos.x, mpos.y), btn->sup()->loc) && press_ef)
		{
			press_ef = false;
			flow->x = 0;
			// operate

			wchar_t* loadfiledir = GetFileNameFromDlg_save();
			if (loadfiledir != nullptr) {
				int objselect_id = mainpage->pfm.Data[(int)mainpm::objselect_id];
				Object* obj = (Object*)mainSprite->data.objs->at(objselect_id);
				Sprite* newspr = (Sprite*)fm->_New(sizeof(Sprite), true);
				newspr->null();
				newspr->load(loadfiledir);
				fm->_Delete((byte8*)loadfiledir, sizeof(wchar_t) * (wcslen(loadfiledir) + 1));
				Sprite* origin_source = obj->source;
				obj->source = newspr;
				if ((origin_source != basicSprite) && mainSprite->isExistSpr((int*)origin_source)) {
					origin_source->Release();
					fm->_Delete((byte8*)origin_source, sizeof(Sprite));
					origin_source = nullptr;
				}
			}

			/*sprloadmod = 'l';
			pagestacking(filepage);*/
		}
	}
}

void fpsprbtn_event(DXBtn* btn, DX_Event evt)
{
	shp::vec2f* flow = (shp::vec2f*)btn->param[1];

	if (evt.message == WM_LBUTTONDOWN)
	{
		shp::vec2f mpos = GetMousePos(evt.lParam);
		// dbg << "mpos : " << x << ", " << y << endl;
		if (shp::bPointInRectRange(shp::vec2f(mpos.x, mpos.y), btn->sup()->loc) && press_ef)
		{
			press_ef = false;
			flow->x = 0;
			// operate
			
			if (fpedit == false && fpSprite == nullptr) {
				fpSprite = (Sprite*)fm->_New(sizeof(Sprite), true);
				fpSprite->null();
				fpSprite->st = sprite_type::st_freepolygon;
				fpSprite->data.freepoly = (rbuffer*)fm->_New(sizeof(rbuffer), true);
				fpSprite->data.freepoly->Init(false);
				fpSprite->data.freepoly->begin();
				fpSprite->data.freepoly->set_inherit(true);
				fpSprite->data.freepoly->end();
				fpedit = true;
			}
		}
	}
}

void addicbtn_event(DXBtn* btn, DX_Event evt)
{
	shp::vec2f* flow = (shp::vec2f*)btn->param[1];

	if (evt.message == WM_LBUTTONDOWN)
	{
		shp::vec2f mpos = GetMousePos(evt.lParam);
		// dbg << "mpos : " << x << ", " << y << endl;
		if (shp::bPointInRectRange(shp::vec2f(mpos.x, mpos.y), btn->sup()->loc) && press_ef)
		{
			press_ef = false;
			flow->x = 0;
			// operate
			wchar_t* loadfiledir = GetFileNameFromDlg_open();
			int objselect_id = mainpage->pfm.Data[(int)mainpm::objselect_id];
			if (loadfiledir != nullptr) {
				// load ic
				//dbg << "loadic" << endl;
				string filename = wstr_to_utf8(loadfiledir);
				InsideCode_Bake* icb = nullptr;
				if (icmap.find((char*)filename.c_str()) == icmap.end())
				{
					icb = (InsideCode_Bake*)fm->_New(sizeof(InsideCode_Bake), true);
					icb->init(40960);
					for (int i = 0; i < basic_ext.size(); ++i)
					{
						icb->extension.push_back(basic_ext[i]);
					}
					//dbg << "bake" << endl;
					icb->bake_code((char*)filename.c_str());
					//dbg << "s0" << endl;
					icmap.insert(ICMAP::value_type((char*)filename.c_str(), icb));
					//dbg << "s0" << endl;
				}
				else
				{
					icb = icmap[(char*)filename.c_str()];
				}

				//dbg << "sec0" << endl;

				ICB_Context* ctx = (ICB_Context*)fm->_New(sizeof(ICB_Context), true);
				ctx->SetICB(icb, 4096);	// 40KB
				Object* o = (Object*)mainSprite->data.objs->at(objselect_id);
				ctx->Push_InheritData(8, (byte8*)o);
				if (o->ecs != nullptr)
				{
					// todo : disable origin ecs
				}
				o->ecs = ctx;
				ecss.push_back(ctx);
				sprloadmod = 'n';
				//dbg << "icend" << endl;
				//counting[0] = 1000;
			}
		}
	}
}

void loadicbtn_event(DXBtn* btn, DX_Event evt)
{
	shp::vec2f* flow = (shp::vec2f*)btn->param[1];

	if (evt.message == WM_LBUTTONDOWN)
	{
		shp::vec2f mpos = GetMousePos(evt.lParam);
		// dbg << "mpos : " << x << ", " << y << endl;
		if (shp::bPointInRectRange(shp::vec2f(mpos.x, mpos.y), btn->sup()->loc) && press_ef)
		{
			press_ef = false;
			flow->x = 0;
			// operate
			char temp[256] = {};
			GetCurrentDirectoryA(256, temp);
			wchar_t* loadfiledir = GetFileNameFromDlg_open();
			SetCurrentDirectoryA(temp);
			int objselect_id = mainpage->pfm.Data[(int)mainpm::objselect_id];
			if (loadfiledir != nullptr) {
				// load ic
				//dbg << "loadic" << endl;
				string filename = wstr_to_utf8(loadfiledir);
				InsideCode_Bake* icb = nullptr;
				if (icmap.find((char*)filename.c_str()) == icmap.end())
				{
					ICB_Editor* icbe = (ICB_Editor*)fm->_New(sizeof(ICB_Editor), true);
					icbe->Init(shp::rect4f(-200, -100, 200, 100), filename.c_str());
					icbe_pool.push_back(icbe);
					icb = icbe->SelectedICB;
				}
				else
				{
					icb = icmap[(char*)filename.c_str()];
				}

				//dbg << "sec0" << endl;

				ICB_Context* ctx = (ICB_Context*)fm->_New(sizeof(ICB_Context), true);
				ctx->SetICB(icb, 40960);	// 40KB
				Object* o = (Object*)mainSprite->data.objs->at(objselect_id);
				ctx->Push_InheritData(8, (byte8*)&o);
				if (o->ecs != nullptr)
				{
					// todo : disable origin ecs
				}
				o->ecs = ctx;
				ecss.push_back(ctx);
				sprloadmod = 'n';
				//dbg << "icend" << endl;
				//counting[0] = 1000;
			}
		}
	}
}

void mainpagebtn_render(DXBtn* btn)
{
	float layer_uitext = **(float**)&mainpage->pfm.Data[(int)mainpm::layer_uitext];
	float layer_ui = **(float**)&mainpage->pfm.Data[(int)mainpm::layer_ui];

	rbuffer* rb = (rbuffer*)btn->param[0];
	shp::vec2f* flow = (shp::vec2f*)btn->param[1];

	float rate = AnimClass::EaseOut(flow->x / flow->y, 5.0f);
	shp::vec2f cen = btn->sup()->loc.getCenter();
	float expendrate = 0.5f + 0.5f * rate;
	ConstantBuffer cb = GetBasicModelCB(shp::vec3f(cen.x, cen.y, layer_ui), shp::vec3f(0, 0, 0),
		shp::vec3f(btn->sup()->loc.getw() * expendrate, btn->sup()->loc.geth() * expendrate, 1),
		DX11Color(1.0f, 1.0f, 1.0f, 1.0f));

	shp::rect4f loc = btn->sup()->loc;
	loc.fx += loc.getw() / 4.0f;
	loc.fy += loc.geth() / 4.0f;

	rb->render(cb);
	draw_string(btn->text, wcslen(btn->text), 15.0f * expendrate, loc, DX11Color(1.0f, 1.0f, 1.0f, 1.0f), layer_uitext);
}

void mainpageslider_render(DXSlider* slider)
{
	float layer_ui = **(float**)&mainpage->pfm.Data[(int)mainpm::layer_ui];
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

	ConstantBuffer cb = GetBasicModelCB(shp::vec3f(bar.getCenter().x, bar.getCenter().y, layer_ui), shp::vec3f(0, 0, 0),
		shp::vec3f(bar.getw(), bar.geth(), 1), DX11Color(1.0f, 1.0f, 1.0f, 0.5f));
	DX11Color col = DX11Color(1, 1, 1, 0.5f);


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

	ConstantBuffer cb2 = GetBasicModelCB(shp::vec3f(pos.getCenter().x, pos.getCenter().y, layer_ui), shp::vec3f(0, 0, shp::PI / 4.0f),
		shp::vec3f(pos.getw() * 0.7f, pos.geth() * 0.7f, 1), DX11Color(0.1f, 0.4f, 0.6f, 1.0f));
	linedrt->render(cb2);
	linedrt->render(cb);
}

void main_init(Page* p)
{
	

	basicSprite = (Sprite*)fm->_New(sizeof(Sprite), true);
	basicSprite->null();
	basicSprite->st == sprite_type::st_freepolygon;
	basicSprite->data.freepoly = (rbuffer*)fm->_New(sizeof(rbuffer), true);
	basicSprite->data.freepoly->Init(false);
	basicSprite->data.freepoly->begin();
	basicSprite->data.freepoly->av(SimpleVertex(shp::vec3f(-100, -100, 0), DX11Color(1, 1, 1, 1)));
	basicSprite->data.freepoly->av(SimpleVertex(shp::vec3f(100, -100, 0), DX11Color(1, 1, 1, 1)));
	basicSprite->data.freepoly->av(SimpleVertex(shp::vec3f(100, 100, 0), DX11Color(1, 1, 1, 1)));
	basicSprite->data.freepoly->av(SimpleVertex(shp::vec3f(-100, 100, 0), DX11Color(1, 1, 1, 1)));
	basicSprite->data.freepoly->end();

	dbg << (int)mainpm::opencolorpage << endl;
	static int behaveid_arr[6] = { 0, 1, 2, 3, 4, -1 };
	static int tabid_arr[2] = { 0, 1 };
	dbg << "enum class mainpm{" << endl;

	p->pfm.SetHeapData(new byte8[8192], 8192);
	dbg << "present_center = " << p->pfm.Fup << endl;
	Init_VPTR_x64<freemem::FM_Model0>(&p->pfm);
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
	behavetop->init(L"+", behavetopbtn_init, mainpagebtn_render, basicbtn_update, behavetopbtn_event,
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
		textobjbtn_event, shp::rect4f(-scw / 2.0f, sch / 2.0f - 340,
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
	opencolorpage->init(L"C", basicbtn_init, mainpagebtn_render, basicbtn_update,
		opencolorpagebtn_event, shp::rect4f(-scw / 2.0f + 200, sch / 2.0f - 100.0f,
			-scw / 2.0f + 300, sch / 2.0f));


	dbg << ", openobjlist = " << p->pfm.Fup << endl;
	DXBtn* openobjlist = (DXBtn*)p->pfm._New(sizeof(DXBtn));
	openobjlist->init(L"<", basicbtn_init, mainpagebtn_render, basicbtn_update, openobjlistbtn_event,
		shp::rect4f(scw / 2.0f - 100, -sch / 2.0f, scw / 2.0f, -sch / 2.0f + 100));

	dbg << ", showobjlist = " << p->pfm.Fup << endl;
	bool* showobjlist = (bool*)p->pfm._New(sizeof(bool));
	*showobjlist = false;

	dbg << ", closeobjlist = " << p->pfm.Fup << endl;
	DXBtn* closeobjlist = (DXBtn*)p->pfm._New(sizeof(DXBtn));
	closeobjlist->init(L">", basicbtn_init, mainpagebtn_render, basicbtn_update,
		closeobjlistbtn_event, shp::rect4f(scw / 2.0f - 500, -sch / 2.0f,
			scw / 2.0f - 400, -sch / 2.0f + 100));

	openobjlist->param[2] = (int*)showobjlist;
	closeobjlist->param[2] = (int*)showobjlist;

	dbg << ", savefilestr = " << p->pfm.Fup << endl;
	wchar_t* savefilestr = (wchar_t*)p->pfm._New(sizeof(wchar_t) * 255);
	savefilestr[0] = 0;

	dbg << ", tabbtn_objs = " << p->pfm.Fup << endl;
	DXBtn* tabbtn_objs = (DXBtn*)p->pfm._New(sizeof(DXBtn));
	tabbtn_objs->init(L"objs", basicbtn_init, mainpagebtn_render, basicbtn_update, tabbtn_event,
		shp::rect4f(scw / 2.0f - 400, sch / 2.0f - 100.0f, scw / 2.0f - 200,
			sch / 2.0f));

	dbg << ", tabbtn_property = " << p->pfm.Fup << endl;
	DXBtn* tabbtn_property = (DXBtn*)p->pfm._New(sizeof(DXBtn));
	tabbtn_property->init(L"property", basicbtn_init, mainpagebtn_render, basicbtn_update,
		tabbtn_event, shp::rect4f(scw / 2.0f - 200, sch / 2.0f - 100.0f,
			scw / 2.0f, sch / 2.0f));
	tabbtn_objs->param[2] = &tabid_arr[0];
	tabbtn_property->param[2] = &tabid_arr[1];

	dbg << ", list_height = " << p->pfm.Fup << endl;
	float* list_height = (float*)p->pfm._New(sizeof(float) * 2);
	list_height[0] = 0;
	list_height[1] = 0;

	dbg << ", tabSlider = " << p->pfm.Fup << endl;
	DXSlider* tabSlider = (DXSlider*)p->pfm._New(sizeof(DXSlider));
	tabSlider->init(1.0f, 'f', (int*)list_height,
		shp::rect4f(scw / 2.0f - 60.0f, -sch / 2.0f, scw / 2.0f, sch / 2.0f - 100.0f),
		basicslider_init, mainpageslider_render, basicslider_update, basicslider_event,
		false);

	dbg << ", objselect_id = " << p->pfm.Fup << endl;
	unsigned int* objselect_id = (unsigned int*)p->pfm._New(sizeof(unsigned int));
	*objselect_id = 0;

	dbg << ", tabselect_id = " << p->pfm.Fup << endl;
	unsigned int* tabselect_id = (unsigned int*)p->pfm._New(sizeof(unsigned int));
	*tabselect_id = 0;

	dbg << ", paramselect_id = " << p->pfm.Fup << endl;
	unsigned int* paramselect_id = (unsigned int*)p->pfm._New(sizeof(unsigned int));
	*paramselect_id = 0;

	dbg << ", sprdirbtn = " << p->pfm.Fup << endl;
	DXBtn* sprdirbtn = (DXBtn*)p->pfm._New(sizeof(DXBtn));
	sprdirbtn->init(L"sprdir", basicbtn_init, mainpagebtn_render, basicbtn_update, sprdirbtn_event,
		shp::rect4f(-700, -sch / 2.0f, -400, -sch / 2.0f + 100));

	dbg << ", loadbydirbtn = " << p->pfm.Fup << endl;
	DXBtn* loadbydirbtn = (DXBtn*)p->pfm._New(sizeof(DXBtn));
	loadbydirbtn->init(L"loadByDir", basicbtn_init, mainpagebtn_render, basicbtn_update,
		loadfromfilebtn_event, shp::rect4f(-300, -sch / 2.0f, 300, -sch / 2.0f + 100));

	dbg << ", fpsprbtn = " << p->pfm.Fup << endl;
	DXBtn* fpsprbtn = (DXBtn*)p->pfm._New(sizeof(DXBtn));
	fpsprbtn->init(L"Fp", basicbtn_init, mainpagebtn_render, basicbtn_update, fpsprbtn_event,
		shp::rect4f(400, -sch / 2.0f, 700, -sch / 2.0f + 100));

	dbg << ", addicbtn = " << p->pfm.Fup << endl;
	DXBtn* addicbtn = (DXBtn*)p->pfm._New(sizeof(DXBtn));
	addicbtn->init(L"AddIC", basicbtn_init, mainpagebtn_render, basicbtn_update, addicbtn_event,
		shp::rect4f(-600, -sch / 2.0f, -400, -sch / 2.0f + 100));

	dbg << ", loadicbtn = " << p->pfm.Fup << endl;
	DXBtn* loadicbtn = (DXBtn*)p->pfm._New(sizeof(DXBtn));
	loadicbtn->init(L"LoadIC", basicbtn_init, mainpagebtn_render, basicbtn_update, loadicbtn_event,
		shp::rect4f(-300, -sch / 2.0f, 300, -sch / 2.0f + 100));

	LayerInfo* li;

	dbg << ", layer_backinfo = " << p->pfm.Fup << endl;
	float** layer_backinfo = (float**)p->pfm._New(sizeof(float*));
	li = layerManager.PushLayer("main_backinfo");
	*layer_backinfo = &li->Z;

	dbg << ", layer_obj = " << p->pfm.Fup << endl;
	float** layer_obj = (float**)p->pfm._New(sizeof(float*));
	li = layerManager.PushLayer("main_obj");
	*layer_obj = &li->Z;

	dbg << ", layer_ui = " << p->pfm.Fup << endl;
	float** layer_ui = (float**)p->pfm._New(sizeof(float*));
	li = layerManager.PushLayer("main_UI");
	*layer_ui = &li->Z;

	dbg << ", layer_uitext = " << p->pfm.Fup << endl;
	float** layer_uitext = (float**)p->pfm._New(sizeof(float*));
	li = layerManager.PushLayer("main_UI_text");
	*layer_uitext = &li->Z;

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

	int* behave_selected = (int*)&p->pfm.Data[(int)mainpm::behave_selected];
	float layer_uitext = **(float**)&p->pfm.Data[(int)mainpm::layer_uitext];
	float layer_ui = **(float**)&p->pfm.Data[(int)mainpm::layer_ui];
	float layer_obj = **(float**)&p->pfm.Data[(int)mainpm::layer_obj];
	float layer_backinfo = **(float**)&p->pfm.Data[(int)mainpm::layer_backinfo];

	// ui rendering


	behavetop->Render();
	ocp->Render();

	DXBtn* openobjlist = (DXBtn*)&p->pfm.Data[(int)mainpm::openobjlist];
	DXBtn* closeobjlist = (DXBtn*)&p->pfm.Data[(int)mainpm::closeobjlist];
	bool* showobjlist = (bool*)&p->pfm.Data[(int)mainpm::showobjlist];

	if (*showobjlist)
	{
		// obj list

		closeobjlist->Render();
		shp::rect4f tabrt = shp::rect4f(scw / 2.0f - 400, +sch / 2.0f, scw / 2.0f, -sch / 2.0f);
		
		unsigned int* tabselect_id = (unsigned int*)&p->pfm.Data[(int)mainpm::tabselect_id];
		DXBtn* tabbtn_objs = (DXBtn*)&p->pfm.Data[(int)mainpm::tabbtn_objs];
		DXBtn* tabbtn_property = (DXBtn*)&p->pfm.Data[(int)mainpm::tabbtn_property];
		DXSlider* tabSlider = (DXSlider*)&p->pfm.Data[(int)mainpm::tabSlider];
		tabbtn_objs->Render();
		tabbtn_property->Render();
		tabSlider->render();

		if (mainSprite == nullptr) goto PASS_Render_0;

		if (*tabselect_id == 0)
		{
			float tab_height = *(float*)&p->pfm.Data[(int)mainpm::list_height];
			unsigned int* objselect_id = (unsigned int*)&p->pfm.Data[(int)mainpm::objselect_id];
			// render lists

			fmvecarr < int*>* objarr = mainSprite->data.objs;
			tab_height = tab_height * (float)objarr->size() * 100;
			shp::rect4f objrt =
				shp::rect4f(scw / 2.0f - 400, +sch / 2.0f - 190 + tab_height, scw / 2.0f,
					sch / 2.0f - 100.0f + tab_height);
			for (int i = 0; i < objarr->size(); ++i)
			{
				wchar_t nstr[16];
				wcscpy_s(nstr, (wchar_t*)to_wstring(i).c_str());
				int len = wcslen(nstr);
				draw_string(nstr, len, 30, objrt, DX11Color(1, 1, 1, 1), layer_uitext);
				
				if (*objselect_id == i)
				{
					drawline(shp::vec2f(objrt.getCenter().x, objrt.fy),
						shp::vec2f(objrt.getCenter().x, objrt.ly), objrt.getw(), DX11Color(0.2f,
							0.7f,
							0.8f,
							1.0f), layer_ui);
				}
				else
				{
					drawline(shp::vec2f(objrt.getCenter().x, objrt.fy),
						shp::vec2f(objrt.getCenter().x, objrt.ly), objrt.getw(), DX11Color(0.2f,
							0.3f,
							0.3f,
							1.0f), layer_ui);
				}
				objrt.fy -= 100;
				objrt.ly -= 100;
			}

			drawline(shp::vec2f(tabrt.getCenter().x, tabrt.fy),
				shp::vec2f(tabrt.getCenter().x, tabrt.ly), tabrt.geth() / 2.0f, DX11Color(0, 0,
					0,
					0.8f), layer_ui);
		}
		else
		{
			// property
			unsigned int* objselect_id = (unsigned int*)&p->pfm.Data[(int)mainpm::objselect_id];
			unsigned int* paramselect_id =
				(unsigned int*)&p->pfm.Data[(int)mainpm::paramselect_id];
			Object* obj = (Object*)mainSprite->data.objs->at(*objselect_id);
			fmvecarr < int*>* objarr = mainSprite->data.objs;
			float* tab_height = (float*)&p->pfm.Data[(int)mainpm::list_height];
			float th = tab_height[1] * (float)objarr->size() * 50.0f;
			shp::rect4f objrt =
				shp::rect4f(scw / 2.0f - 400, +sch / 2.0f - 190 + th, scw / 2.0f,
					sch / 2.0f - 150.0f + th);

			wchar_t nstr[5][16] = { L"spr", L"pos", L"rot", L"sca", L"ic" };
			// source
			for (int i = 0; i < 5; ++i)
			{
				DX11Color col = DX11Color(0, 0, 0, 1);
				col =
					(*paramselect_id == i) ? DX11Color(0.2f, 0.7f, 0.8f, 0.8f) : DX11Color(0.2f, 0.3f,
						0.3f, 0.8f);
				int len = wcslen(nstr[i]);
				drawline(shp::vec2f(objrt.getCenter().x, objrt.fy),
					shp::vec2f(objrt.getCenter().x, objrt.ly), objrt.getw(), col, layer_ui);
				draw_string(nstr[i], len, 15, objrt, DX11Color(1, 1, 1, 1), layer_uitext);
				objrt.fy -= 50;
				objrt.ly -= 50;
			}

			if (obj->ecs != nullptr) {
				//additional IC Param
				fmvecarr<NamingData*> params = obj->ecs->icb->globalVariables;
				for (int i = 0; i < params.size(); ++i) {
					NamingData nd = *params.at(i);
					DX11Color col = DX11Color(0, 0, 0, 1);
					col = (*paramselect_id == i+5) ? DX11Color(0.2f, 0.7f, 0.8f, 0.8f) : DX11Color(0.2f, 0.3f, 0.3f, 0.8f);
					
					wstring wstr = utf8_to_wstr(nd.name);
					drawline(shp::vec2f(objrt.getCenter().x, objrt.fy),
						shp::vec2f(objrt.getCenter().x, objrt.ly), objrt.getw(), col, layer_ui);
					draw_string((wchar_t*)wstr.c_str(), wstr.size(), 15, objrt, DX11Color(1, 1, 1, 1), layer_uitext);
					objrt.fy -= 50;
					objrt.ly -= 50;
				}
			}

			switch (*paramselect_id)
			{
			case 0:
				// spr
			{
				DXBtn* sprdirbtn = (DXBtn*)&p->pfm.Data[(int)mainpm::sprdirbtn];
				DXBtn* loadbydirbtn = (DXBtn*)&p->pfm.Data[(int)mainpm::loadbydirbtn];
				DXBtn* fpsprbtn = (DXBtn*)&p->pfm.Data[(int)mainpm::fpsprbtn];
				
				sprdirbtn->Render();
				loadbydirbtn->Render();
				fpsprbtn->Render();
				
			}
			break;
			case 1:
				// pos
			{

			}
			break;
			case 2:
				// rot
			{
			}
			break;
			case 3:
				// sca
			{
			}
			break;
			case 4:
				// ic
			{
				DXBtn* addicbtn = (DXBtn*)&p->pfm.Data[(int)mainpm::addicbtn];
				addicbtn->Render();

				DXBtn* loadicbtn = (DXBtn*)&p->pfm.Data[(int)mainpm::loadicbtn];
				loadicbtn->Render();
			}
			break;
			}
		}

	PASS_Render_0:

		drawline(shp::vec2f(tabrt.getCenter().x, tabrt.fy),
			shp::vec2f(tabrt.getCenter().x, tabrt.ly), tabrt.geth() / 2.0f, DX11Color(0, 0, 0,
				0.5f), layer_ui);
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

	// obj redering_2d
	XMVECTOR Eye = XMVectorSet(pc->x, pc->y, -500.0f, 0.0f);
	XMVECTOR At = XMVectorSet(pc->x, pc->y, 0.0f, 0.0f);
	XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	CamView = XMMatrixLookAtLH(Eye, At, Up);

	float farZ = 1000.0f;
	float nearZ = 0.1f;
	CamProj = XMMATRIX(
		2.0f / (float)width * zoomrate, 0, 0, 0,
		0, 2.0f / (float)height * zoomrate, 0, 0,
		0, 0, 1.0f / (farZ - nearZ), 0,
		0, 0, 1.0f / (nearZ - farZ), 1);

	ConstantBuffer cb = GetCamModelCB(shp::vec3f(0, 0, layer_obj), shp::vec3f(0, 0, 0), shp::vec3f(1, 1, 1), DX11Color(1.0f, 1.0f, 1.0f, 1.0f));
	linedrt->render(cb);

	if (fpSprite != nullptr && fpedit) {
		rbuffer* ap = (rbuffer*)fpSprite->data.freepoly;
		// dbgcount(0, ap->dbg_rbuffer());
		// ap->render();
		XMFLOAT3 temp = ap->buffer[ap->get_renderChoice()]->at(0).Pos;
		shp::vec3f savpos = shp::vec3f(temp.x, temp.y, temp.z);
		fmvecarr < pos_select_obj >* sarr =
			(fmvecarr < pos_select_obj > *) & p->pfm.Data[(int)mainpm::select_arr];

		for (int i = 0; i < ap->get_vertexsiz(ap->get_renderChoice()); ++i)
		{
			temp = ap->buffer[ap->get_renderChoice()]->at(i).Pos;
			shp::vec3f pos = shp::vec3f(temp.x, temp.y, temp.z);
			ConstantBuffer normalCB = GetCamModelCB(pos, shp::vec3f(0, 0, 0), shp::vec3f(10 * zoomrate, 10 * zoomrate, 1), DX11Color(0, 1.0f, 1.0f, 1.0f));
			ConstantBuffer selectCB = GetCamModelCB(pos, shp::vec3f(0, 0, *stacktime * shp::PI),
				shp::vec3f(zoomrate * (30.0f + 10.0f * sinf(*stacktime * 3.0f)),
					zoomrate * (30.0f + 10.0f * sinf(*stacktime * 3.0f)), 1),
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

			drawline_cam(shp::vec2f(savpos.x, savpos.y), shp::vec2f(pos.x, pos.y), 4 * zoomrate,
				DX11Color(1, 1, 1, 1), layer_obj);
			savpos = pos;
		}
		temp = ap->buffer[ap->get_renderChoice()]->at(0).Pos;
		shp::vec3f initpos = shp::vec3f(temp.x, temp.y, temp.z);
		drawline_cam(shp::vec2f(initpos.x, initpos.y), shp::vec2f(savpos.x, savpos.y),
			4 * zoomrate, DX11Color(1, 1, 1, 1), layer_obj);
		fpSprite->render(cb);
	}
	// dbgcount(0, dbg << "try render sprite" << endl);
	if (mainSprite != nullptr)
	{
		if (*behave_selected == 0
			&& (mainSprite->st == sprite_type::st_freepolygon
				&& mainSprite->data.freepoly != nullptr))
		{
			rbuffer* ap = (rbuffer*)mainSprite->data.freepoly;
			// dbgcount(0, ap->dbg_rbuffer());
			// ap->render();
			XMFLOAT3 temp = ap->buffer[ap->get_renderChoice()]->at(0).Pos;
			shp::vec3f savpos = shp::vec3f(temp.x, temp.y, temp.z);
			fmvecarr < pos_select_obj >* sarr =
				(fmvecarr < pos_select_obj > *) & p->pfm.Data[(int)mainpm::select_arr];

			for (int i = 0; i < ap->get_vertexsiz(ap->get_renderChoice()); ++i)
			{
				temp = ap->buffer[ap->get_renderChoice()]->at(i).Pos;
				shp::vec3f pos = shp::vec3f(temp.x, temp.y, temp.z);
				ConstantBuffer normalCB = GetCamModelCB(pos, shp::vec3f(0, 0, 0), shp::vec3f(10 * zoomrate, 10 * zoomrate, 1), DX11Color(0, 1.0f, 1.0f, 1.0f));
				ConstantBuffer selectCB = GetCamModelCB(pos, shp::vec3f(0, 0, *stacktime * shp::PI),
					shp::vec3f(zoomrate * (30.0f + 10.0f * sinf(*stacktime * 3.0f)),
						zoomrate * (30.0f + 10.0f * sinf(*stacktime * 3.0f)), 1),
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

				drawline_cam(shp::vec2f(savpos.x, savpos.y), shp::vec2f(pos.x, pos.y), 4 * zoomrate,
					DX11Color(1, 1, 1, 1), layer_obj);
				savpos = pos;
			}
			temp = ap->buffer[ap->get_renderChoice()]->at(0).Pos;
			shp::vec3f initpos = shp::vec3f(temp.x, temp.y, temp.z);
			drawline_cam(shp::vec2f(initpos.x, initpos.y), shp::vec2f(savpos.x, savpos.y),
				4 * zoomrate, DX11Color(1, 1, 1, 1), layer_obj);
		}

		mainSprite->render(cb);
	}

	shp::rect4f* selectrt = (shp::rect4f*)&p->pfm.Data[(int)mainpm::select_rect];
	drawline_cam(shp::vec2f(selectrt->fx, selectrt->fy), shp::vec2f(selectrt->fx, selectrt->ly),
		4 * zoomrate, DX11Color(1, 1, 1, 1), layer_backinfo);
	drawline_cam(shp::vec2f(selectrt->fx, selectrt->ly), shp::vec2f(selectrt->lx, selectrt->ly),
		4 * zoomrate, DX11Color(1, 1, 1, 1), layer_backinfo);
	drawline_cam(shp::vec2f(selectrt->lx, selectrt->ly), shp::vec2f(selectrt->lx, selectrt->fy),
		4 * zoomrate, DX11Color(1, 1, 1, 1), layer_backinfo);
	drawline_cam(shp::vec2f(selectrt->lx, selectrt->fy), shp::vec2f(selectrt->fx, selectrt->fy),
		4 * zoomrate, DX11Color(1, 1, 1, 1), layer_backinfo);

	float d = 50.0f * (int)(zoomrate);
	float lw = 2.0f;
	float alpha = 0.2f;
	drawline_cam(shp::vec2f(pc->x - scwh.x, 0), shp::vec2f(pc->x + scwh.x, 0), 4.0f * zoomrate,
		DX11Color(1, 1, 1, 1.0f), layer_backinfo);
	drawline_cam(shp::vec2f(0, pc->y - scwh.y), shp::vec2f(0, pc->y + scwh.y), 4.0f * zoomrate,
		DX11Color(1, 1, 1, 1.0f), layer_backinfo);
	for (int k = 0; k < 3; ++k)
	{
		// dbg << "scwh.x : " << scwh.x << endl;

		int ax = (pc->y - scwh.y) / d;
		int max = (pc->y + scwh.y) / d;
		for (; ax < max; ++ax)
		{
			drawline_cam(shp::vec2f(pc->x - scwh.x, (float)ax * d),
				shp::vec2f(pc->x + scwh.x, (float)ax * d), lw * zoomrate, DX11Color(1, 1, 1, alpha), layer_backinfo);
		}
		ax = (pc->x - scwh.x) / d;
		max = (pc->x + scwh.x) / d;
		for (; ax < max; ++ax)
		{
			drawline_cam(shp::vec2f((float)ax * d, pc->y - scwh.y),
				shp::vec2f((float)ax * d, pc->y + scwh.y), lw * zoomrate, DX11Color(1, 1, 1, alpha), layer_backinfo);
		}
		d = d * 2.0f;
		lw = lw + 1.0f;
		alpha *= 2.0f;
	}
}

void main_update(Page* p, float delta)
{
	//icbE.Update(delta);
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

	unsigned int* objselect_id = (unsigned int*)&p->pfm.Data[(int)mainpm::objselect_id];

	if (*showobjlist)
	{
		DXBtn* tabbtn_objs = (DXBtn*)&p->pfm.Data[(int)mainpm::tabbtn_objs];
		DXBtn* tabbtn_property = (DXBtn*)&p->pfm.Data[(int)mainpm::tabbtn_property];
		tabbtn_objs->Update(delta);
		tabbtn_property->Update(delta);
		closeobjlist->Update(delta);
		DXSlider* tabSlider = (DXSlider*)&p->pfm.Data[(int)mainpm::tabSlider];
		tabSlider->update(delta);

		DXBtn* sprdirbtn = (DXBtn*)&p->pfm.Data[(int)mainpm::sprdirbtn];
		DXBtn* loadbydirbtn = (DXBtn*)&p->pfm.Data[(int)mainpm::loadbydirbtn];
		DXBtn* fpsprbtn = (DXBtn*)&p->pfm.Data[(int)mainpm::fpsprbtn];
		DXBtn* addicbtn = (DXBtn*)&p->pfm.Data[(int)mainpm::addicbtn];
		DXBtn* loadicbtn = (DXBtn*)&p->pfm.Data[(int)mainpm::loadicbtn];
		sprdirbtn->Update(delta);
		loadbydirbtn->Update(delta);
		fpsprbtn->Update(delta);
		addicbtn->Update(delta);
		loadicbtn->Update(delta);
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
		DXBtn* tabbtn_objs = (DXBtn*)&p->pfm.Data[(int)mainpm::tabbtn_objs];
		DXBtn* tabbtn_property = (DXBtn*)&p->pfm.Data[(int)mainpm::tabbtn_property];
		closeobjlist->Event(evt);
		DXSlider* tabSlider = (DXSlider*)&p->pfm.Data[(int)mainpm::tabSlider];
		shp::rect4f tabrt = shp::rect4f(scw / 2.0f - 400, +sch / 2.0f, scw / 2.0f, -sch / 2.0f);
		unsigned int* tabselect_id = (unsigned int*)&p->pfm.Data[(int)mainpm::tabselect_id];
		if (mainSprite == nullptr) goto PASS_EVENT_0;

		if (*tabselect_id == 0)
		{

			float tab_height = *(float*)&p->pfm.Data[(int)mainpm::list_height];
			unsigned int* objselect_id = (unsigned int*)&p->pfm.Data[(int)mainpm::objselect_id];
			// render lists

			fmvecarr < int*>* objarr = mainSprite->data.objs;
			tab_height = tab_height * (float)objarr->size() * 100;
			shp::rect4f objrt =
				shp::rect4f(scw / 2.0f - 400, +sch / 2.0f - 190 + tab_height, scw / 2.0f,
					sch / 2.0f - 100.0f + tab_height);
			if (evt.message == WM_LBUTTONDOWN)
			{
				shp::vec2f mpos = GetMousePos(evt.lParam);
				for (int i = 0; i < objarr->size(); ++i)
				{
					if (shp::bPointInRectRange(shp::vec2f(mpos.x, mpos.y), objrt))
					{
						*objselect_id = i;
					}
					objrt.fy -= 100;
					objrt.ly -= 100;
				}
			}
		}
		else
		{
			// property
			unsigned int* objselect_id = (unsigned int*)&p->pfm.Data[(int)mainpm::objselect_id];
			unsigned int* paramselect_id =
				(unsigned int*)&p->pfm.Data[(int)mainpm::paramselect_id];
			Object* obj = (Object*)mainSprite->data.objs->at(*objselect_id);
			fmvecarr < int*>* objarr = mainSprite->data.objs;
			float* tab_height = (float*)&p->pfm.Data[(int)mainpm::list_height];
			float th = tab_height[1] * (float)objarr->size() * 50;
			shp::rect4f objrt =
				shp::rect4f(scw / 2.0f - 400, +sch / 2.0f - 190 + th, scw / 2.0f,
					sch / 2.0f - 150.0f + th);

			wchar_t nstr[5][16] = { L"spr", L"pos", L"rot", L"sca", L"ic" };
			// source

			if (evt.message == WM_LBUTTONDOWN)
			{
				shp::vec2f mpos = GetMousePos(evt.lParam);
				for (int i = 0; i < 5; ++i)
				{
					if (shp::bPointInRectRange(shp::vec2f(mpos.x, mpos.y), objrt))
					{
						*paramselect_id = i;
					}
					objrt.fy -= 50;
					objrt.ly -= 50;
				}

				if (obj->ecs != nullptr) {
					//additional IC Param
					shp::vec2f mpos = GetMousePos(evt.lParam);
					fmvecarr<NamingData*> params = obj->ecs->icb->globalVariables;
					for (int i = 0; i < params.size(); ++i) {
						if (shp::bPointInRectRange(shp::vec2f(mpos.x, mpos.y), objrt))
						{
							*paramselect_id = i+5;
						}
						objrt.fy -= 50;
						objrt.ly -= 50;
					}
				}
			}

			switch (*paramselect_id)
			{
			case 0:
				// spr
			{

				DXBtn* sprdirbtn = (DXBtn*)&p->pfm.Data[(int)mainpm::sprdirbtn];
				DXBtn* loadbydirbtn = (DXBtn*)&p->pfm.Data[(int)mainpm::loadbydirbtn];
				DXBtn* fpsprbtn = (DXBtn*)&p->pfm.Data[(int)mainpm::fpsprbtn];
				sprdirbtn->Event(evt);
				loadbydirbtn->Event(evt);
				fpsprbtn->Event(evt);
			}
			break;
			case 1:
			{
				if (evt.message == WM_LBUTTONDOWN)
				{
					*present_center = shp::vec2f(obj->pos.x, obj->pos.y);
					*centerorigin = *present_center;
				}
				if (evt.message == WM_MOUSEMOVE)
				{
					if (*finger_pressed)
					{
						shp::vec2f mpos = GetMousePos(evt.lParam);
						//shp::vec2f temppos = GetMousePos_notcenter(evt.lParam);
						/*shp::vec2f viewpos =
							shp::vec2f(present_center->x + (scwh.x / scw) * temppos.x -
								scwh.x / 2.0f,
								present_center->y - (scwh.y / sch) * temppos.x +
								scwh.y / 2.0f);*/
						shp::vec2f dv = shp::vec2f(mpos.x - presspos->x, mpos.y - presspos->y);
						*present_center = *centerorigin - dv.operator*(zoomrate);
						obj->pos.x = present_center->x;
						obj->pos.y = present_center->y;
					}
				}
			}
			// pos
			break;
			case 2:
				// rot
			{
				if (*finger_pressed)
				{
					shp::vec2f mpos = GetMousePos(evt.lParam);
					float y = mpos.y;
					if (y > 0)
					{
						obj->rot.z += 0.001f;
					}
					else
					{
						obj->rot.z -= 0.001f;
					}
				}
			}
			break;
			case 3:
				// sca
			{
				if (*finger_pressed)
				{
					shp::vec2f mpos = GetMousePos(evt.lParam);
					float y = mpos.y;
					if (y > 0)
					{
						obj->sca.x *= 1.001f;
						obj->sca.y *= 1.001f;
						obj->sca.z *= 1.001f;
					}
					else
					{
						obj->sca.x /= 1.001f;
						obj->sca.y /= 1.001f;
						obj->sca.z /= 1.001f;
					}
				}
			}
			break;
			case 4:
				// ic
				DXBtn * addicbtn = (DXBtn*)&p->pfm.Data[(int)mainpm::addicbtn];
				addicbtn->Event(evt);

				DXBtn* loadicbtn = (DXBtn*)&p->pfm.Data[(int)mainpm::loadicbtn];
				loadicbtn->Event(evt);
				break;
			}
		}

	PASS_EVENT_0:

		tabSlider->Event(evt);
		tabbtn_objs->Event(evt);
		tabbtn_property->Event(evt);
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
		//fpedit = false;
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
	
	//이거 왜 실행 안됨??
	if (evt.message == WM_KEYDOWN && evt.wParam == 'S') {
		if (fpedit && fpSprite != nullptr) {
			unsigned int* objselect_id = (unsigned int*)&p->pfm.Data[(int)mainpm::objselect_id];
			rbuffer* rb = fpSprite->data.freepoly;
			Object* obj = (Object*)mainSprite->data.objs->at(*objselect_id);
			int rc = rb->get_renderChoice();
			for (int i = 0; i < rb->get_vertexsiz(rc); ++i) {
				XMFLOAT3& ref = rb->buffer[rc]->at(i).Pos;
				ref.x -= obj->pos.x;
				ref.y -= obj->pos.y;
				ref.z -= obj->pos.z;
				//dbg << obj->rot.z/shp::PI << endl;
				shp::vec2f beforeRotatePos = shp::GetRotatePos(shp::vec2f(0, 0), shp::vec2f(ref.x, ref.y), shp::angle2f(-obj->rot.z, true));
				//dbg << "origin[" << i << "] : " << ref.x << ", " << ref.y << endl;
				//dbg << "trpos[" << i << "] : " << beforeRotatePos.x << ", " << beforeRotatePos.y << endl;
				ref.x = beforeRotatePos.x;
				ref.y = beforeRotatePos.y;
				ref.x = ref.x / obj->sca.x;
				ref.y = ref.y / obj->sca.y;
				ref.z = ref.z / obj->sca.z;
			}
			rb->set_choice(1);
			rb->end();
			if (obj->source == basicSprite) {
				obj->source = fpSprite;
				fpSprite = nullptr;
				fpedit = false;
			}
			else {
				Sprite* tempSpr = obj->source;
				obj->source = nullptr;
				if (mainSprite->isExistSpr((int*)tempSpr) == false) {
					fm->_Delete((byte8*)tempSpr, sizeof(Sprite));
				}
				obj->source = fpSprite;
				fpSprite = nullptr;
				fpedit = false;
			}
		}
	}
	if (evt.message == WM_LBUTTONDOWN)
	{
		*finger_pressed = true;
		//int n = SDL_GetNumTouchFingers(GetMousePos_notcenter(evt.lParam).touchId);
		shp::vec2f mpos = GetMousePos(evt.lParam);
		
		*presspos = shp::vec2f(mpos.x, mpos.y);
		shp::vec2f viewpos =
			shp::vec2f(present_center->x + (scwh.x/scw) * GetMousePos_notcenter(evt.lParam).x - scwh.x / 2.0f,
				present_center->y - (scwh.y / sch) * GetMousePos_notcenter(evt.lParam).y + scwh.y / 2.0f);
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

		if (behavemod->x == 0 && (fpedit && press_ef)) {
			press_ef = false;
			rbuffer* ap = (rbuffer*)fpSprite->data.freepoly;
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

		if ((fpedit && fpSprite != nullptr) && (objmod->x == 1 && *selection_enable)) {
			shp::rect4f* rt = (shp::rect4f*)&p->pfm.Data[(int)mainpm::select_rect];
			rt->fx = viewpos.x;
			rt->fy = viewpos.y;
		}
	}
	else if (evt.message == WM_LBUTTONUP)
	{
		if (*finger_pressed)
		{
			shp::vec2f mpos = GetMousePos(evt.lParam);
			
			shp::vec2f viewpos =
				shp::vec2f(present_center->x + (scwh.x / scw) * GetMousePos_notcenter(evt.lParam).x - scwh.x / 2.0f,
					present_center->y - (scwh.y / sch) * GetMousePos_notcenter(evt.lParam).y + scwh.y / 2.0f);
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
			for (int i = 0; i < ap->get_vertexsiz(ap->get_renderChoice()); ++i)
			{
				XMFLOAT3 temp = ap->buffer[ap->get_renderChoice()]->at(i).Pos;
				shp::vec3f pos = shp::vec3f(temp.x, temp.y, temp.z);
				XMFLOAT4 ctemp = ap->buffer[ap->get_renderChoice()]->at(i).Color;
				DX11Color col = DX11Color(ctemp.x, ctemp.y, ctemp.z, ctemp.w);
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

		if ((fpedit && fpSprite != nullptr) && (objmod->x == 1 && *selection_enable)) {
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
			rbuffer* ap = (rbuffer*)fpSprite->data.freepoly;
			for (int i = 0; i < ap->get_vertexsiz(ap->get_renderChoice()); ++i)
			{
				XMFLOAT3 temp = ap->buffer[ap->get_renderChoice()]->at(i).Pos;
				shp::vec3f pos = shp::vec3f(temp.x, temp.y, temp.z);
				XMFLOAT4 ctemp = ap->buffer[ap->get_renderChoice()]->at(i).Color;
				DX11Color col = DX11Color(ctemp.x, ctemp.y, ctemp.z, ctemp.w);
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
			fmvecarr<SimpleVertex>* bptr = ap->buffer[ap->get_renderChoice()];
			XMFLOAT3 temp;
			for (int i = 0; i < sarr->size(); ++i)
			{
				temp = bptr->at(sarr->at(i).index).Pos;
				sarr->at(i).origin_pos = shp::vec3f(temp.x, temp.y, temp.z);
			}
		}

		if ((fpedit && fpSprite != nullptr) && (objmod->x == 1 && *(bool*)translate->param[2])) {
			rbuffer* ap = (rbuffer*)fpSprite->data.freepoly;
			fmvecarr<SimpleVertex>* bptr = ap->buffer[ap->get_renderChoice()];
			XMFLOAT3 temp;
			for (int i = 0; i < sarr->size(); ++i)
			{
				temp = bptr->at(sarr->at(i).index).Pos;
				sarr->at(i).origin_pos = shp::vec3f(temp.x, temp.y, temp.z);
			}
		}
	}
	else if (evt.message == WM_MOUSEMOVE)
	{
		if (*finger_pressed)
		{
			shp::vec2f mpos = GetMousePos(evt.lParam);
			
			shp::vec2f viewpos =
				shp::vec2f(present_center->x + (scwh.x / scw) * GetMousePos_notcenter(evt.lParam).x - scwh.x / 2.0f,
					present_center->y - (scwh.y / sch) * GetMousePos_notcenter(evt.lParam).y + scwh.y / 2.0f);
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
						zoomrate = zoomrate + 0.001f;
					}
					else if (dv.y < 0)
					{
						zoomrate = zoomrate - 0.001f;
					}

					if (zoomrate <= 0.001f) {
						zoomrate = 0.001f;
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
				fmvecarr<SimpleVertex>* bptr = ap->buffer[ap->get_renderChoice()];
				SimpleVertex* farr = (SimpleVertex*)fm->_tempNew(bptr->size() * sizeof(SimpleVertex));
				for (int i = 0; i < bptr->size(); ++i)
				{
					farr[i] = bptr->at(i);
					// dbgcount(0, dbg << farr[i] << endl)
				}
				int vsiz = ap->get_vertexsiz(ap->get_renderChoice());
				// dbgcount(0, dbg << "ap vertex size : " <<
				// ap->get_vertexsiz(ap->get_renderChoice()) << endl)
				ap->clear();
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
						shp::vec3f p = shp::vec3f(farr[i].Pos.x, farr[i].Pos.y, farr[i].Pos.z);
						DX11Color c = DX11Color(farr[i].Color.x, farr[i].Color.y, farr[i].Color.z, farr[i].Color.w);
						ap->av(SimpleVertex(p, c));
					}
				}
				ap->end();
				/*if (ap->buffer[ap->get_choice()] == 0) {
					cout << "hey" << endl;
				}*/
				fm->_tempPopLayer();
			}

			if (((behavemod->x == 1) && (objmod->x == 1 && *(bool*)translate->param[2])) && (fpedit && fpSprite != nullptr))
			{
				fm->_tempPushLayer();
				rbuffer* ap = (rbuffer*)fpSprite->data.freepoly;
				fmvecarr<SimpleVertex>* bptr = ap->buffer[ap->get_renderChoice()];
				SimpleVertex* farr = (SimpleVertex*)fm->_tempNew(bptr->size() * sizeof(SimpleVertex));
				for (int i = 0; i < bptr->size(); ++i)
				{
					farr[i] = bptr->at(i);
					// dbgcount(0, dbg << farr[i] << endl)
				}
				int vsiz = ap->get_vertexsiz(ap->get_renderChoice());
				// dbgcount(0, dbg << "ap vertex size : " <<
				// ap->get_vertexsiz(ap->get_renderChoice()) << endl)
				ap->clear();
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
						shp::vec3f p = shp::vec3f(farr[i].Pos.x, farr[i].Pos.y, farr[i].Pos.z);
						DX11Color c = DX11Color(farr[i].Color.x, farr[i].Color.y, farr[i].Color.z, farr[i].Color.w);
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

			if ((fpedit && fpSprite != nullptr) && (objmod->x == 1 && *selection_enable)) {
				shp::rect4f* rt = (shp::rect4f*)&p->pfm.Data[(int)mainpm::select_rect];
				rt->lx = viewpos.x;
				rt->ly = viewpos.y;
			}
		}
	}
}

void colorpage_donebtn_event(DXBtn* btn, DX_Event evt)
{
	shp::vec2f* flow = (shp::vec2f*)btn->param[1];

	if (evt.message == WM_LBUTTONDOWN)
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

void colorpagebtn_render(DXBtn* btn)
{
	float layer_uitext = **(float**)&colorpage->pfm.Data[(int)colorpm::layer_uitext];
	float layer_ui = **(float**)&colorpage->pfm.Data[(int)colorpm::layer_ui];

	rbuffer* rb = (rbuffer*)btn->param[0];
	shp::vec2f* flow = (shp::vec2f*)btn->param[1];

	float rate = AnimClass::EaseOut(flow->x / flow->y, 5.0f);
	shp::vec2f cen = btn->sup()->loc.getCenter();
	float expendrate = 0.5f + 0.5f * rate;
	ConstantBuffer cb = GetBasicModelCB(shp::vec3f(cen.x, cen.y, layer_ui), shp::vec3f(0, 0, 0),
		shp::vec3f(btn->sup()->loc.getw() * expendrate, btn->sup()->loc.geth() * expendrate, 1),
		DX11Color(1.0f, 1.0f, 1.0f, 1.0f));

	shp::rect4f loc = btn->sup()->loc;
	loc.fx += loc.getw() / 4.0f;
	loc.fy += loc.geth() / 4.0f;

	rb->render(cb);
	draw_string(btn->text, wcslen(btn->text), 15.0f * expendrate, loc, DX11Color(1.0f, 1.0f, 1.0f, 1.0f), layer_uitext);
}

void colorpageslider_render(DXSlider* slider)
{
	float layer_ui = **(float**)&colorpage->pfm.Data[(int)colorpm::layer_ui];
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

	ConstantBuffer cb = GetBasicModelCB(shp::vec3f(bar.getCenter().x, bar.getCenter().y, layer_ui), shp::vec3f(0, 0, 0),
		shp::vec3f(bar.getw(), bar.geth(), 1), DX11Color(1.0f, 1.0f, 1.0f, 0.5f));
	DX11Color col = DX11Color(1, 1, 1, 0.5f);

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

	ConstantBuffer cb2 = GetBasicModelCB(shp::vec3f(pos.getCenter().x, pos.getCenter().y, layer_ui), shp::vec3f(0, 0, shp::PI / 4.0f),
		shp::vec3f(pos.getw() * 0.7f, pos.geth() * 0.7f, 1), DX11Color(0.1f, 0.4f, 0.6f, 1.0f));
	linedrt->render(cb2);
	linedrt->render(cb);
}

void colorpage_init(Page* p)
{
	p->pfm.SetHeapData(new byte8[4096], 4096);
	Init_VPTR_x64<freemem::FM_Model0>(&p->pfm);

	dbg << "enum class colorpm{" << endl;
	dbg << "presentcolor = " << p->pfm.Fup << endl;
	DX11Color* presentcolor = (DX11Color*)p->pfm._New(sizeof(DX11Color));	// 0

	dbg << ", RSlider = " << p->pfm.Fup << endl;
	DXSlider* RSlider = (DXSlider*)p->pfm._New(sizeof(DXSlider));
	RSlider->init(1.0f, 'f', (int*)&presentcolor->r, shp::rect4f(-500, 450, 500, 550), basicslider_init, colorpageslider_render, basicslider_update, basicslider_event, true);	// 16

	dbg << ", GSlider = " << p->pfm.Fup << endl;
	DXSlider* GSlider = (DXSlider*)p->pfm._New(sizeof(DXSlider));
	GSlider->init(1.0f, 'f', (int*)&presentcolor->g, shp::rect4f(-500, 300, 500, 400), basicslider_init, colorpageslider_render, basicslider_update, basicslider_event, true);	// 200

	dbg << ", BSlider = " << p->pfm.Fup << endl;
	DXSlider* BSlider = (DXSlider*)p->pfm._New(sizeof(DXSlider));
	BSlider->init(1.0f, 'f', (int*)&presentcolor->b, shp::rect4f(-500, 150, 500, 250), basicslider_init, colorpageslider_render, basicslider_update, basicslider_event, true);	// 384

	dbg << ", ASlider = " << p->pfm.Fup << endl;
	DXSlider* ASlider = (DXSlider*)p->pfm._New(sizeof(DXSlider));
	ASlider->init(1.0f, 'f', (int*)&presentcolor->a, shp::rect4f(-500, 0, 500, 100), basicslider_init, colorpageslider_render, basicslider_update, basicslider_event, true);	// 568

	dbg << ", done = " << p->pfm.Fup << endl;
	DXBtn* done = (DXBtn*)p->pfm._New(sizeof(DXBtn));
	done->init(L"done", basicbtn_init, colorpagebtn_render, basicbtn_update, colorpage_donebtn_event, shp::rect4f(scw / 2.0f - 500, sch / 2.0f - 500, scw / 2.0f, sch / 2.0f));	// 752

	dbg << ", pallete = " << p->pfm.Fup << endl;
	DX11Color* pallete = (DX11Color*)p->pfm._New(sizeof(DX11Color) * 20 * 5);	// 10*5
	for (int i = 0; i < 100; ++i)
	{
		pallete[i] = DX11Color(1, 1, 1, 0.2f);
	}							// 1016

	dbg << ", selectnum = " << p->pfm.Fup << endl;
	int* selectnum = (int*)p->pfm._New(sizeof(int));
	*selectnum = 0;				// 2616

	LayerInfo* li;
	dbg << ", layer_backinfo = " << p->pfm.Fup << endl;
	float** layer_backinfo = (float**)p->pfm._New(sizeof(float*));
	li = layerManager.PushLayer("color_backinfo");
	*layer_backinfo = &li->Z;

	dbg << ", layer_obj = " << p->pfm.Fup << endl;
	float** layer_obj = (float**)p->pfm._New(sizeof(float*));
	li = layerManager.PushLayer("color_obj");
	*layer_obj = &li->Z;

	dbg << ", layer_ui = " << p->pfm.Fup << endl;
	float** layer_ui = (float**)p->pfm._New(sizeof(float*));
	li = layerManager.PushLayer("color_UI");
	*layer_ui = &li->Z;

	dbg << ", layer_uitext = " << p->pfm.Fup << endl;
	float** layer_uitext = (float**)p->pfm._New(sizeof(float*));
	li = layerManager.PushLayer("color_UI_text");
	*layer_uitext = &li->Z;

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

			shp::rect4f cloc =
				shp::rect4f(sloc.fx + margin, sloc.fy + margin, sloc.lx - margin,
					sloc.ly - margin);
			ConstantBuffer cb = GetBasicModelCB(shp::vec3f(cloc.getCenter().x, cloc.getCenter().y, 0), shp::vec3f(0, 0, 0),
				shp::vec3f(cloc.getw(), cloc.geth(), 1), pallete[5 * x + y]);
			linedrt->render(cb);

			if (*selectnum == x * 5 + y)
			{
				ConstantBuffer cb = GetBasicModelCB(shp::vec3f(sloc.getCenter().x, sloc.getCenter().y, 0),
					shp::vec3f(0, 0, 0), shp::vec3f(sloc.getw(), sloc.geth(), 1), DX11Color(1, 1, 1, 0.4f));
				linedrt->render(cb);
			}
		}
	}

	shp::rect4f pcloc = shp::rect4f(scloc.fx, scloc.ly - 500.0f, scloc.fx + 500.0f, scloc.ly);
	ConstantBuffer cb2 = GetBasicModelCB(shp::vec3f(pcloc.getCenter().x, pcloc.getCenter().y, 0), shp::vec3f(0, 0, 0),
		shp::vec3f(pcloc.getw(), pcloc.geth(), 1), *presentcolor);
	linedrt->render(cb2);
	
	ConstantBuffer cb = GetBasicModelCB(shp::vec3f(scloc.getCenter().x, scloc.getCenter().y, 0), shp::vec3f(0, 0, 0),
			shp::vec3f(scloc.getw(), scloc.geth(), 1), DX11Color(0, 0, 0, 0.4f));
	linedrt->render(cb);
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
	if (evt.message == WM_LBUTTONDOWN)
	{
		float mx = GetMousePos(evt.lParam).x;
		float my = GetMousePos(evt.lParam).y;
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

	if (evt.message == WM_LBUTTONDOWN)
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
	Init_VPTR_x64<freemem::FM_Model0>(&p->pfm);

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
	locale::global(locale(".UTF-8"));
	static chrono::steady_clock::time_point ft, et;
    dbg.open("dbg.txt", ios::ate);

    fm = new freemem::FM_System0();
    fm->SetHeapData(4096, 4096, 65536, 1048576);
    fm->_tempPushLayer();
    const char* font_path = "FiraCode-Regular.ttf";

    uint8_t condition_variable = 0;
    int8_t error = TTFFontParser::parse_file(font_path, &font_data, &font_parsed, &condition_variable);

    sub_font_data[0] = new TTFFontParser::FontData();
    const char* subfont_path = "malgun.ttf";
    error = TTFFontParser::parse_file(subfont_path, sub_font_data[0], &font_parsed, &condition_variable);

	layerManager.Init();
	icbe_pool.NULLState();
	icbe_pool.Init(8, false, true);

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

				for (int i = 0; i < icbe_pool.size(); ++i) {
					icbe_pool[i]->Update(delta);
				}

				ft = chrono::high_resolution_clock::now();

				if (ecss.size() > 0)
				{
					// performance pi is 2/10(exerate)
					//dbgcount(0, dbg << "execute" << endl);
					execute_switch(ecss, 100, code_control, exerate, icbindex_cxt);
					//dbgcount(0, dbg << "executeend" << endl);
				}

				Render();
			}
        }
    }

    CleanupDevice();

    fm->_tempPopLayer();

	layerManager.Release();

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
	wcex.hCursor = NULL;//LoadCursor( NULL, IDC_ARROW );
    wcex.hbrBackground = ( HBRUSH )( COLOR_WINDOW + 1 );
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = L"TutorialWindowClass";
    wcex.hIconSm = LoadIcon( wcex.hInstance, ( LPCTSTR )IDI_ICON2);
    if( !RegisterClassEx( &wcex ) )
        return E_FAIL;

    // Create window
    g_hInst = hInstance;
    RECT rc = { 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN)-70};
	width = GetSystemMetrics(SM_CXSCREEN);
	height = GetSystemMetrics(SM_CYSCREEN);
    AdjustWindowRect( &rc, WS_OVERLAPPEDWINDOW, FALSE );
    g_hWnd = CreateWindow( L"TutorialWindowClass", L"EuclidMachine", WS_OVERLAPPEDWINDOW,
                           0, 0, rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, hInstance,
                           NULL );
    if( !g_hWnd )
        return E_FAIL;

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
	zoomrate = 1;
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

	/*D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
	ZeroMemory(&rtvDesc, sizeof(rtvDesc));
	rtvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;*/
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
	ZeroMemory( &descDSV, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC) );
    descDSV.Format = descDepth.Format;
    descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    descDSV.Texture2D.MipSlice = 0;
    hr = g_pd3dDevice->CreateDepthStencilView( g_pDepthStencil, &descDSV, &g_pDepthStencilView );
    if( FAILED( hr ) )
        return hr;

    g_pImmediateContext->OMSetRenderTargets( 1, &g_pRenderTargetView, g_pDepthStencilView );

	/*D3D11_DEPTH_STENCIL_DESC dsd;
	ZeroMemory(&descDSV, sizeof(D3D11_DEPTH_STENCIL_DESC));
	dsd.DepthEnable = TRUE;
	dsd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsd.DepthFunc = D3D11_COMPARISON_LESS;
	dsd.StencilEnable = TRUE;
	dsd.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	dsd.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
	dsd.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsd.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	dsd.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsd.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	dsd.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsd.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	dsd.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsd.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	hr = g_pd3dDevice->CreateDepthStencilState(&dsd, &g_pDepthStencilState);
	if (FAILED(hr)) {
		return hr;
	}*/
	
	D3D11_BLEND_DESC BlendState;
	ZeroMemory(&BlendState, sizeof(D3D11_BLEND_DESC));
	BlendState.RenderTarget[0].BlendEnable = TRUE;
	BlendState.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	BlendState.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	BlendState.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	BlendState.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	BlendState.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	BlendState.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	BlendState.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	//BlendState.RenderTarget[0].BlendEnable = TRUE;
	//BlendState.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	g_pd3dDevice->CreateBlendState(&BlendState, &g_pBlendStateBlend);

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
	rasterizerDesc.AntialiasedLineEnable = true;
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
	XMVECTOR Eye = XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f);
	XMVECTOR At = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	g_View = XMMatrixLookAtLH( Eye, At, Up );

    // Initialize the projection matrix
    float farZ = 100.0f;
    float nearZ = 0.01f;

    //원근투영
	g_Projection_3d = XMMatrixPerspectiveFovLH(XM_PIDIV2, width / (FLOAT)height, nearZ, farZ);
	//g_Projection_2d = XMMatrixPerspectiveFovLH( XM_PIDIV2, width / (FLOAT)height, nearZ, farZ);
    //직교투영
	/*
	* g_Projection_2d = XMMATRIX(
        2.0f/(float)width, 0, 0, 0,
        0, 2.0f/(float)height, 0, 0,
        0, 0, 1.0f / (farZ - nearZ), 0,
        0, 0, 1.0f / (nearZ - farZ), 1);
	*/
	
	g_Projection_2d = XMMatrixOrthographicLH(width, height, nearZ, farZ);

	// init

	InsideCode_Bake::SetICLFlag(ICL_FLAG::ICB_StaticInit, false);
	InsideCode_Bake::SetICLFlag(ICL_FLAG::Create_New_ICB_Extension_Init, false);
	{
		InsideCode_Bake::SetICLFlag(ICL_FLAG::Create_New_ICB_Extension_Init__Bake_Extension,
			false);
	}

	InsideCode_Bake::SetICLFlag(ICL_FLAG::BakeCode_AddTextBlocks, false);

	InsideCode_Bake::SetICLFlag(ICL_FLAG::BakeCode_ScanStructTypes, false);

	InsideCode_Bake::SetICLFlag(ICL_FLAG::BakeCode_AddStructTypes, false);

	InsideCode_Bake::SetICLFlag(ICL_FLAG::BakeCode_ScanCodes, false);

	InsideCode_Bake::SetICLFlag(ICL_FLAG::BakeCode_GlobalMemoryInit, false);

	InsideCode_Bake::SetICLFlag(ICL_FLAG::BakeCode_CompileCodes, false);
	{
		InsideCode_Bake::SetICLFlag(ICL_FLAG::BakeCode_CompileCodes__add_var, false);
		InsideCode_Bake::SetICLFlag(ICL_FLAG::BakeCode_CompileCodes__set_var, false);
		InsideCode_Bake::SetICLFlag(ICL_FLAG::BakeCode_CompileCodes__if__sen, false);
		InsideCode_Bake::SetICLFlag(ICL_FLAG::BakeCode_CompileCodes__while__, false);
		InsideCode_Bake::SetICLFlag(ICL_FLAG::BakeCode_CompileCodes__block__, false);
		InsideCode_Bake::SetICLFlag(ICL_FLAG::BakeCode_CompileCodes__addfunc, false);
		InsideCode_Bake::SetICLFlag(ICL_FLAG::BakeCode_CompileCodes__usefunc, false);
		InsideCode_Bake::SetICLFlag(ICL_FLAG::BakeCode_CompileCodes__return_, false);
		InsideCode_Bake::SetICLFlag(ICL_FLAG::BakeCode_CompileCodes__struct__, false);
		InsideCode_Bake::SetICLFlag(ICL_FLAG::BakeCode_CompileCodes__break__, false);
		InsideCode_Bake::SetICLFlag(ICL_FLAG::BakeCode_CompileCodes__continue, false);
		InsideCode_Bake::SetICLFlag(ICL_FLAG::BakeCode_CompileCodes__adsetvar, false);
	}

	InsideCode_Bake::SetICLFlag(ICL_FLAG::Create_New_ICB_Context, false);

	InsideCode_Bake::StaticInit();

	ecss.NULLState();
	ecss.Init(8, false, true);

	basic_ext.NULLState();
	basic_ext.Init(8, false, true);
	ICB_Extension* ext = Init_exGeometry();
	basic_ext.push_back(ext);
	//dbg << "init graphics" << endl;
	ext = Init_exGraphics();
	//dbg << "init graphics end" << endl;
	basic_ext.push_back(ext);

	ICB_exTool = Init_exTool();

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
	mainpage->init_func(mainpage);
	colorpage->init_func(colorpage);
	texteditpage->init_func(texteditpage);

	pagestacking(mainpage);
    
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

	basic_circleRT = (rbuffer*)fm->_New(sizeof(rbuffer), true);
	basic_circleRT->Init(false);
	basic_circleRT->begin();
	float tempangle = 0;
	constexpr float delta_tempangle = 3.141592f / 16.0f;
	shp::vec3f tempvertex = shp::vec3f(1, 0, 0);
	for (int i = 0; i < 32; ++i) {
		basic_circleRT->av(SimpleVertex(tempvertex, color));
		tempangle += delta_tempangle;
		tempvertex.x = cosf(tempangle);
		tempvertex.y = sinf(tempangle);
	}
	basic_circleRT->end();

	ToolPostRender = (exTool_PostRenderManager*)fm->_New(sizeof(exTool_PostRenderManager), true);
	ToolPostRender->Init();

	ToolEventSystem = (exTool_EventSystem*)fm->_New(sizeof(exTool_EventSystem), true);
	ToolEventSystem->Init();

	ICB_Editor::StaticInit();
	Sprite::StaticInit();

	FinishInit = true;

	//icbE.Init(shp::rect4f(0, 0, 700, 500), nullptr);

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
        //case WM_KEYDOWN:
        //    //PostQuitMessage(0);
        //    break;
		case WM_LBUTTONDOWN:
			press_ef = true;
			break;
		case WM_LBUTTONUP:
			press_ef = false;
			break;
        case WM_MOUSEMOVE:
            mx = LOWORD(lParam);
            my = HIWORD(lParam);
            g_CursorWorld = XMMatrixTranslation((float)mx - (float)width/2.0f, -1.0f * (float)my + (float)height/2.0f, 0.0f);
            break;
        case WM_DESTROY:
            PostQuitMessage( 0 );
            break;
    }

	if (FinishInit) {
		presentPage->event_func(presentPage, evt);
	}

	for (int i = 0; i < icbe_pool.size(); ++i) {
		icbe_pool[i]->Event(evt);
	}

	if (ToolEventSystem != nullptr && using_tool) {
		bool b = evt.message == WM_LBUTTONDOWN;
		b = b || evt.message == WM_KEYDOWN;
		if (b) {
			ToolEventSystem->PushEvent(evt);
		}
	}
	//icbE.Event(evt);

    return DefWindowProc(hWnd, message, wParam, lParam);;
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
    g_pImmediateContext->ClearDepthStencilView( g_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0 );

	float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	UINT sampleMask = 0xffffffff;
	g_pImmediateContext->OMSetBlendState(g_pBlendStateBlend, blendFactor, sampleMask);

	g_pImmediateContext->OMSetRenderTargets(1, &g_pRenderTargetView, g_pDepthStencilView);

	//g_pImmediateContext->OMSetDepthStencilState(g_pDepthStencilState, NULL);

    //
    // Update variables for the first cube
    //
	ConstantBuffer cursor_cb = SetCB(g_CursorWorld, g_View, g_Projection_2d, DX11Color(1.0f, 1.0f, 1.0f, 1.0f));
	g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cursor_cb, 0, 0);
	cursor_obj.render(cursor_cb);
    
    //char_map.at(L'안')->render(cursor_cb);
    
    g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &polygon_cb, 0, 0);
    //polygon_obj.render(polygon_cb);

    //draw_string(L"안녕하세용!! World!!", 16, 30, shp::rect4f(0, 0, 100, 100), DX11Color(1, 1, 1, 1));
    //
    // Present our back buffer to our front buffer
    //

	Glyph g;
	int fontindex = -1;
	uint32_t unicode = L'ㄴ';
	unordered_map<uint32_t, TTFFontParser::Glyph>* glyphmap = &font_data.glyphs;
	while (glyphmap->find(unicode) == glyphmap->end()) {
		++fontindex;
		glyphmap = &sub_font_data[fontindex]->glyphs;
		if (fontindex >= max_sub_font) {
			return;
		}
	}
	g = glyphmap->at(unicode);

	float rate = 0.2f;
	float R = 0;
	
	Curve c;
	shp::vec2f v;
	shp::vec2f lastpos = shp::vec2f(0, 0);
	for (int i = 0; i < g.path_list.size(); ++i) {
		R = 1;
		c = g.path_list.at(i).geometry.at(0);
		shp::vec2f lastpos = shp::vec2f(rate*c.p0.x, rate*c.p0.y);
		float dr = 1.0f / (float)g.path_list.at(i).geometry.size();
		for (int k = 0; k < g.path_list.at(i).geometry.size(); ++k) {
			//R += dr;
			c = g.path_list.at(i).geometry.at(k);
			if (c.is_curve) {
				v = shp::vec2f(rate * c.p0.x, rate * c.p0.y);
				drawline(v, lastpos, 2, DX11Color(R, 0, 0, 1.0f), 0.01f);
				drawline(shp::vec2f(v.x - 5, v.y), shp::vec2f(v.x + 5, v.y), 5, DX11Color(0, 1.0f, 0, 1.0f), 0.01f);
				lastpos = shp::vec2f(rate * c.p0.x, rate * c.p0.y);

				float dt = 0.25f;
				float lt = 1.0f - dt;
				float t = dt;
				for (int u = 1; u < 3; ++u) {
					shp::vec2f sp = shp::vec2f(lt * c.p0.x + t * c.p1.x, lt * c.p0.y + t * c.p1.y);
					shp::vec2f ep = shp::vec2f(lt * c.p1.x + t * c.c.x, lt * c.p1.y + t * c.c.y);
					shp::vec2f rp = shp::vec2f(lt * sp.x + t * ep.x, lt * sp.y + t * ep.y);
					v = shp::vec2f(rate * rp.x, rate * rp.y);
					drawline(v, lastpos, 2, DX11Color(R, 0, 0, 1.0f), 0.01f);
					drawline(shp::vec2f(v.x - 2.5f, v.y), shp::vec2f(v.x + 2.5f, v.y), 5, DX11Color(0, 1.0f, 1.0f, 1.0f), 0.01f);
					lastpos = v;
					lt -= dt;
					t += dt;
				}
				
				/*v = shp::vec2f(rate * c.c.x, rate * c.c.y);
				drawline(v, lastpos, 2, DX11Color(R, 0, 0, 1.0f), 0.01f);
				drawline(shp::vec2f(v.x - 2.5f, v.y), shp::vec2f(v.x + 2.5f, v.y), 10, DX11Color(0, 0, 1.0f, 1.0f), 0.01f);
				lastpos = v;*/
			}
			else {
				//rbuff->av(SimpleVertex(c.p0.x, c.p0.y, 0, 255, 255, 255, 255));
				v = shp::vec2f(rate * c.p0.x, rate * c.p0.y);
				drawline(v, lastpos, 2, DX11Color(R, 0, 0, 1.0f), 0.01f);
				drawline(shp::vec2f(v.x - 5, v.y), shp::vec2f(v.x + 5, v.y), 5, DX11Color(0, 1.0f, 0, 1.0f), 0.01f);
				lastpos = shp::vec2f(rate*c.p0.x, rate*c.p0.y);

				/*v = shp::vec2f(rate * c.p1.x, rate * c.p1.y);
				drawline(v, lastpos, 2, DX11Color(R, 0, 0, 1.0f), 0.01f);
				drawline(shp::vec2f(v.x - 2.5f, v.y), shp::vec2f(v.x + 2.5f, v.y), 10, DX11Color(0, 0, 1.0f, 1.0f), 0.01f);
				lastpos = shp::vec2f(rate * c.p1.x, rate * c.p1.y);*/
			}
		}

		//rbuff->av(SimpleVertex(sc.p0.x, sc.p0.x, 0, 255, 255, 255, 255));
	}

	for (int i = maxpage-1; i >= 0; --i)
	{
		// dbgcount(0, dbg << "render : " << i << endl);
		pagestack[i]->render_func(pagestack[i]);
	}

	for (int i = 0; i < icbe_pool.size(); ++i) {
		icbe_pool[i]->Render();
	}

	//// current directory debug
	//wchar_t temp[256] = {};
	//GetCurrentDirectory(256, temp);
	//draw_string(temp, wcslen(temp), 5, shp::rect4f(100, 100, 200, 200), DX11Color(1, 1, 1, 1), 0.01f);
	
	//icbE.Render();

	if (ToolPostRender != nullptr && using_tool) {
		ToolPostRender->Render();
	}

    g_pSwapChain->Present( 0, 0 );
    fm->_tempPopLayer();
}
