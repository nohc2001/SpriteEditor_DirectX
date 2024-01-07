#ifndef DX11_UI_H
#define DX11_UI_H

#include "Utill_FreeMemory.h"
#include "ShapeObject.h"

using namespace std;
using namespace freemem;
using namespace TTFFontParser;

int scw = 0;
int sch = 0;

class Page
{
public:
	char name[128] = "main";
	void (*init) (Page*) = nullptr;
	void (*render) (Page*) = nullptr;
	void (*update) (Page*, float) = nullptr;
	void (*event) (Page*, HWND, UINT, WPARAM, LPARAM) = nullptr;
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
	SLIDER = 2,
	EDITBOX = 3
};

class DX_UI
{
public:
	int id = -1;
	bool enable;
	shp::rect4f loc;
	DX_UI_TYPE kind;
	static shp::vec2f mousePos;
	static int FocusID;

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
	uintptr_t param[10] = { };
	FM_System0* fm;

	DXBtn()
	{
	}
	virtual ~DXBtn()
	{
	}

	void (*visual) (char, DXBtn*) = nullptr;
	void (*func) (DXBtn*) = nullptr;

	void init(const wchar_t* wstr, void (*vis) (char, DXBtn*), void (*fun) (DXBtn*),
		shp::rect4f location, FM_System0* fms)
	{
		wcscpy(text, wstr);
		visual = vis;
		func = fun;
		loc = location;
		fm = fms;
	}
	void Update(float delta)
	{
		if (visual != nullptr)
		{
			param[0] = (int)(delta * 1000);
			visual('u', this);
		}
	}
	void Render()
	{
		if (visual != nullptr)
		{
			visual('r', this);
		}
		else
		{
		}
	}
	void OnEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		if (func != nullptr)
		{
			if (message == WM_LBUTTONDOWN)
			{
				float x = LOWORD(lParam) * scw;
				float y = HIWORD(lParam) * sch;
				if (shp::bPointInRectRange(shp::vec2f(x, y), loc))
				{
					func(this);
				}
			}
		}
	}

	DX_UI GetOrigin() { return *((DX_UI*)this); }
};

class DXSlider :DX_UI
{
public:
	float max = 1;
	float setter = 0;
	uintptr_t obj = 0;
	char mod = 'n';				// n : int | f : float
	uintptr_t param[10] = { };
	void (*visual) (char, DXSlider*) = nullptr;
	bool horizontal = true;
	FM_System0* fm;

	DXSlider()
	{
	}
	virtual ~DXSlider()
	{
	}

	void init(float maxnum, char objmod, int* objt, shp::rect4f location,
		void (*vis) (char, DXSlider*), bool hori, FM_System0* fms)
	{
		max = maxnum;
		mod = objmod;
		obj = reinterpret_cast <uintptr_t> (objt);
		loc = location;
		visual = vis;
		horizontal = hori;
		fm = fms;
		switch (mod)
		{
		case 'n':
			setter = *reinterpret_cast <int*>(obj);
			break;
		case 'f':
			setter = *reinterpret_cast <float*>(obj);
			break;
		}
	}

	void Update(float delta)
	{
		if (visual != nullptr)
		{
			param[0] = (int)(delta * 1000);
			visual('u', this);
		}
	}
	void Render()
	{
		if (visual != nullptr)
		{
			visual('r', this);
		}
	}
	void OnEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		if (message == WM_LBUTTONDOWN)
		{
			float x = LOWORD(lParam) * scw;
			float y = HIWORD(lParam) * sch;
			if (shp::bPointInRectRange(shp::vec2f(x, y), loc))
			{
				if (horizontal)
				{
					float mx = x;
					if (mx < loc.fx)
						mx = loc.fx;
					if (mx > loc.lx)
						mx = loc.lx;

					float v = (mx - loc.fx) / loc.getw();
					setter = max * v;
				}
				else
				{
					float mx = y;
					if (mx < loc.fy)
						mx = loc.fy;
					if (mx > loc.ly)
						mx = loc.ly;

					float v = (-mx + loc.ly) / loc.geth();
					setter = max * v;
				}
				switch (mod)
				{
				case 'n':
					*reinterpret_cast <int*>(obj) = setter;
					break;
				case 'f':
					*reinterpret_cast <float*>(obj) = setter;
					break;
				}
			}
		}
	}

	DX_UI GetOrigin() { return *((DX_UI*)this); }
};

class DXEditBox : DX_UI {
public:
	int cursor = 0;
	lwstr wstr;

	char font[128] = {};
	int fontsiz;
	//drw::TextSortStruct tss;

	void (*OnAdd)(DXEditBox*, wchar_t) = nullptr; // 글자를 추가하려 했을때 작동되는 함수 - 제한할 수 있음.
	void (*RenderFunc)(DXEditBox*) = nullptr; // 그릴때 작동되는 함수

	DXEditBox() { loc = shp::rect4f(0, 0, 0, 0); kind = DX_UI_TYPE::EDITBOX; }
	DXEditBox(shp::rect4f ploc, const wchar_t* text, void (*renderfunc)(DXEditBox*), const char* Font, int Fontsiz) {
		loc = ploc;
		int i;
		wstr.NULLState();
		wstr.Init(wcslen(text) + 1, false);
		for (i = 0; i < wcslen(text) + 1; i++) {
			wstr.push_back(text[i]);
		}
		RenderFunc = renderfunc;
		enable = true; kind = DX_UI_TYPE::EDITBOX;

		strcpy_s(font, 128, Font);
		fontsiz = Fontsiz;
		//tss = Tss;
	}
	virtual ~DXEditBox() {}

	void AddFuncOnAdd(void (*func)(DXEditBox*, wchar_t)) { OnAdd = func; }
	void AddFuncOnRender(void (*func)(DXEditBox*)) { RenderFunc = func; }
	void SetLocation(shp::rect4f rt) { loc = rt; }
	void SetText(const wchar_t* text) {
		wstr.clear();
		for (int i = 0; i < wcslen(text) + 1; i++) {
			wstr.push_back(text[i]);
		}
	}

	void init(shp::rect4f ploc, const wchar_t* text, void (*renderfunc)(DXEditBox*), const char* Font, int Fontsiz)
	{
		loc = ploc;
		int i;
		wstr.NULLState();
		wstr.Init(wcslen(text) + 1, false);
		for (i = 0; i < wcslen(text) + 1; i++) {
			wstr.push_back(text[i]);
		}
		RenderFunc = renderfunc;
		enable = true; kind = DX_UI_TYPE::EDITBOX;

		strcpy_s(font, 128, Font);
		fontsiz = Fontsiz;
	}
	void OnEvent(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam) {
		if (enable == false) return;

		if (Message == WM_LBUTTONDOWN) {
			if (shp::bPointInRectRange(mousePos, loc)) {
				FocusID = id;
			}

			cursor = check_string(wstr.c_str(), wstr.size(), fontsiz, loc, nullptr, nullptr, mousePos).index;
			//if (cursor.index == -1) cursor = 0;
		}

		if (FocusID == id && cursor >= 0) {
			if (OnAdd != nullptr) {
				if (Message == WM_CHAR) {
					if (wParam == VK_BACK) {
						wstr.erase(cursor);
					}
					else {
						wchar_t c = (wchar_t)wParam;
						OnAdd(this, c);
					}
				}
			}
			else {
				if (Message == WM_KEYDOWN) {
					if (wParam == VK_LEFT) {
						if (cursor - 1 >= 0) {
							cursor--;
						}
					}
					else if (wParam == VK_RIGHT) {
						if (cursor + 1 <= wstr.size() - 1) {
							cursor++;
						}
					}
				}
				if (Message == WM_CHAR) {
					if (wParam == VK_BACK) {
						if (cursor - 1 >= 0) {
							wstr.erase(cursor - 1);
							cursor--;
						}
					}
					else if (32 <= wParam && wParam <= 126) {
						wchar_t c = wParam;
						if (cursor + 1 <= wstr.size()) {
							wstr.insert(cursor, c);
							cursor++;
						}
					}
				}
			}
		}

	}

	void Render() {
		fm->_tempPushLayer();
		static int flowtime = 0;
		if (enable == false) {
			fm->_tempPopLayer();
			return;
		}

		flowtime++;
		if (RenderFunc != nullptr) {
			RenderFunc(this);
		}
		else {
			//기본 렌더
			float a = 5;
			rbuffer::dxColor4f(0.5, 0.5, 0.5, 0.5);
			rbuffer* rect0 = reinterpret_cast<rbuffer*>(fm->_tempNew(sizeof(rbuffer)));
			rect0->Init(false);
			rect0->begin()
			//glBegin(GL_POLYGON);
			//glVertex2f(location.fx - a, location.fy - a);
			//glVertex2f(location.fx - a, location.ly + a);
			//glVertex2f(location.lx + a, location.ly + a);
			//glVertex2f(location.lx + a, location.fy - a);
			//glEnd();

			float b = 0.0f;
			if (shp::bPointInRectRange(mousePos, loc)) {
				b = 0.5f;
			}
			/*glColor4f(b, b, b, 1);
			glBegin(GL_POLYGON);
			glVertex2f(location.fx, location.fy);
			glVertex2f(location.fx, location.ly);
			glVertex2f(location.lx, location.ly);
			glVertex2f(location.lx, location.fy);
			glEnd();*/

			if (FocusID == id && (flowtime / 15) % 2 == 0) {
				shp::rect4f frt;
				if (wstr.size() > 0) {
					//frt = CheckStringNum(hdc, font, fontsiz, wstr.c_str(), location, Color4f(1, 1, 1, 1), tss, cursor);
				}
				else {
					//frt = CheckStringNum(hdc, font, fontsiz, L"EditBox", location, Color4f(1, 1, 1, 1), tss, cursor);
				}

				if (frt == shp::rect4f(0, 0, 0, 0)) {
					//frt = CheckStringNum(hdc, font, fontsiz, L"EditBox", location, Color4f(1, 1, 1, 1), tss, cursor - 1);
					float w = frt.getw();
					frt.moveValue("fx", w);
				}
				// 커서포시
				/*glColor4f(0.8f, 0.8f, 0.8f, 1);
				glBegin(GL_POLYGON);
				glVertex2f(frt.fx, frt.fy);
				glVertex2f(frt.fx, frt.ly);
				glVertex2f(frt.fx + 3, frt.ly);
				glVertex2f(frt.fx + 3, frt.fy);
				glEnd();*/
			}

			/*glColor4f(1, 1, 1, 1);
			vec2f cen = location.getCenter();
			if (wstr.size() > 0) {
				DrawString(hdc, font, fontsiz, wstr.c_str(), location, Color4f(1, 1, 1, 1), tss, true, false);
			}
			else {
				DrawString(hdc, font, fontsiz, L"EditBox", location, Color4f(1, 1, 1, 1), tss, true, false);
			}*/
		}
		fm->_tempPopLayer();
	}

	DX_UI GetOrigin() { return *((DX_UI*)this); }
};

#endif