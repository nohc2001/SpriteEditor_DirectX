//version 0.1
#ifndef EXEVENT_H
#define EXEVENT_H

#include "ICB_Extension.h"
#include "exGeometry.h"
#include "Utill_SpaceMath.h"
#include "DX11_UI.h"
#include "exGraphics.h"
#include "ICB_Editor.h"

struct exTool_EventData {
    unsigned int message;
    unsigned int wP;
    unsigned int lP;
};

class exTool_EventSystem {
public:
    fmCirculArr<exTool_EventData> pool;
    unsigned int objCount = 0;

    exTool_EventSystem() {

    }

    ~exTool_EventSystem() {

    }

    void Init() {
        pool.Init(8, true);
        objCount = 0;
    }

    void PushEvent(DX_Event evt) {
        exTool_EventData evtd;
        evtd.message = evt.message;
        evtd.wP = evt.wParam;
        evtd.lP = evt.lParam;

        pool.move_pivot(1);
        pool[0] = evtd;
        objCount += 1;
    }

    exTool_EventData PopEvent() {
        if (objCount > 0) {
            exTool_EventData evt = pool[0];
            pool.move_pivot(-1);
            objCount -= 1;
            return evt;
        }
        else {
            exTool_EventData evt;
            evt.message = 0; // null
            return evt;
        }
    }
};

struct exTool_pEventSystem {
    int* data;
};

struct exTool_pRenderData {
    int* data;
};

class exTool_PostRenderManager {
public:
    exTool_PostRenderManager(){}
    ~exTool_PostRenderManager(){}

    fmvecarr<gHeapCheck>* heapBuffer;
    fmvecarr<exTool_pRenderData*> renderData;

    void Init() {
        heapBuffer = (fmvecarr<gHeapCheck>*)fm->_New(sizeof(fmvecarr<gHeapCheck>), true);
        heapBuffer->NULLState();
        heapBuffer->Init(8, false, true);

        renderData.NULLState();
        renderData.Init(8, false, true);
    }

    void ReleaseLayer(int n) {
        if (n >= 0) {
            for (int i = 0; i < heapBuffer->size(); ++i) {
                gHeapCheck hc = heapBuffer->at(i);
                if (hc.shouldRelease == n) {
                    hc.ReleaseFunc(hc.ptr);
                    heapBuffer->erase(i);
                    --i;
                }
            }
        }
        else {
            for (int i = 0; i < heapBuffer->size(); ++i) {
                gHeapCheck hc = heapBuffer->at(i);
                hc.ReleaseFunc(hc.ptr);
                heapBuffer->erase(i);
            }
            heapBuffer->release();
            fm->_Delete((byte8*)heapBuffer, sizeof(fmvecarr<gHeapCheck>));
        }
    }

    void ChangeRenderData(exTool_pRenderData* origin, exTool_pRenderData newv, int layer, void (*ReleaseFunc)(void*)) {
        bool isexist = false;
        int eindex = 0;
        for (int i = 0; i < renderData.size(); ++i) {
            if (renderData.at(i) == origin) {
                isexist = true;
                eindex = i;
                break;
            }
        }

        if (isexist) {
            gHeapCheck hc;
            hc.ptr = origin->data;
            hc.ReleaseFunc = ReleaseFunc;
            hc.shouldRelease = layer;
            heapBuffer->push_back(hc);
            origin->data = *reinterpret_cast<int**>(&newv);
        }
        else {
            renderData.push_back(origin);
            origin->data = *reinterpret_cast<int**>(&newv);
        }
    }

    void Render();
};

struct exTool_pPostRenderTarget {
    int* data;
};

struct exTool_pICBEditor {
    int* data;
};

enum class exTool_PostRenderDataType {
    prdt_Line = 1,
    prdt_FillRect = 2,
    prdt_Circle = 3,
    prdt_Text = 4
};

struct exTool_RenderData_Line {
    exTool_PostRenderDataType type;
    shp::rect4f loc;
    DX11Color color;
};
struct exTool_RenderData_Circle {
    exTool_PostRenderDataType type;
    shp::vec2f center;
    float radius;
    DX11Color color;
};
struct exTool_RenderData_FillRect {
    exTool_PostRenderDataType type;
    shp::rect4f loc;
    DX11Color color;
};
struct exTool_RenderData_Text {
    exTool_PostRenderDataType type;
    shp::rect4f loc;
    char* strptr;
    unsigned int capacity;
    DX11Color color;
    float fontsiz;
};

exTool_PostRenderManager* ToolPostRender = nullptr;
exTool_EventSystem* ToolEventSystem = nullptr;
bool using_tool;

ICB_Extension* ICB_exTool;

void exTool_ReleaseLine(void* ptr) {
    exTool_RenderData_Line* vptr = (exTool_RenderData_Line*)ptr;
    fm->_Delete((byte8*)ptr, sizeof(exTool_RenderData_Line));
}

void exTool_ReleaseCircle(void* ptr) {
    exTool_RenderData_Circle* vptr = (exTool_RenderData_Circle*)ptr;
    fm->_Delete((byte8*)ptr, sizeof(exTool_RenderData_Circle));
}

void exTool_ReleaseFillRect(void* ptr) {
    exTool_RenderData_FillRect* vptr = (exTool_RenderData_FillRect*)ptr;
    fm->_Delete((byte8*)ptr, sizeof(exTool_RenderData_FillRect));
}

void exTool_ReleaseText(void* ptr) {
    exTool_RenderData_Text* vptr = (exTool_RenderData_Text*)ptr;
    fm->_Delete((byte8*)vptr->strptr, vptr->capacity);
    fm->_Delete((byte8*)ptr, sizeof(exTool_RenderData_Text));
}

//EventData PopEvent(pEventSystem evtsys);
void exTool_PopEvent(int* pcontext)
{
    ICB_Context* icc = reinterpret_cast <ICB_Context*>(pcontext);
    exTool_pEventSystem pevtsys = *reinterpret_cast <exTool_pEventSystem*>(icc->rfsp - 8);
    exTool_EventSystem* evtsys = reinterpret_cast<exTool_EventSystem*>(pevtsys.data);
    exTool_EventData evt = evtsys->PopEvent();

    icc->sp -= sizeof(exTool_EventData);
    *reinterpret_cast <exTool_EventData*>(icc->sp) = evt;
    icc->getA(0) = icc->sp - icc->mem;
    icc->Amove_pivot(-1);
}

//bool LBtnDown(EventData evt);
void exTool_LBtnDown(int* pcontext)
{
    ICB_Context* icc = reinterpret_cast <ICB_Context*>(pcontext);
    exTool_EventData evt = *reinterpret_cast <exTool_EventData*>(icc->rfsp - 12);

    bool b = false;
    b = evt.message == WM_LBUTTONDOWN;

    icc->sp -= sizeof(bool);
    *reinterpret_cast<bool*>(&icc->getA(0)) = b;
    icc->Amove_pivot(-1);
}

//bool LBtnUp(EventData evt);
void exTool_LBtnUp(int* pcontext)
{
    ICB_Context* icc = reinterpret_cast <ICB_Context*>(pcontext);
    exTool_EventData evt = *reinterpret_cast <exTool_EventData*>(icc->rfsp - 12);

    bool b = false;
    b = evt.message == WM_LBUTTONUP;

    icc->sp -= sizeof(bool);
    *reinterpret_cast<bool*>(&icc->getA(0)) = b;
    icc->Amove_pivot(-1);
}

//bool RBtnDown(EventData evt);
void exTool_RBtnDown(int* pcontext)
{
    ICB_Context* icc = reinterpret_cast <ICB_Context*>(pcontext);
    exTool_EventData evt = *reinterpret_cast <exTool_EventData*>(icc->rfsp - 12);

    bool b = false;
    b = evt.message == WM_RBUTTONDOWN;

    icc->sp -= sizeof(bool);
    *reinterpret_cast<bool*>(&icc->getA(0)) = b;
    icc->Amove_pivot(-1);
}

//bool RBtnUp(EventData evt);
void exTool_RBtnUp(int* pcontext)
{
    ICB_Context* icc = reinterpret_cast <ICB_Context*>(pcontext);
    exTool_EventData evt = *reinterpret_cast <exTool_EventData*>(icc->rfsp - 12);

    bool b = false;
    b = evt.message == WM_LBUTTONUP;

    icc->sp -= sizeof(bool);
    *reinterpret_cast<bool*>(&icc->getA(0)) = b;
    icc->Amove_pivot(-1);
}

//bool MouseMove(EventData evt);
void exTool_MouseMove(int* pcontext)
{
    ICB_Context* icc = reinterpret_cast <ICB_Context*>(pcontext);
    exTool_EventData evt = *reinterpret_cast <exTool_EventData*>(icc->rfsp - 12);

    bool b = false;
    b = evt.message == WM_MOUSEMOVE;

    icc->sp -= sizeof(bool);
    *reinterpret_cast<bool*>(&icc->getA(0)) = b;
    icc->Amove_pivot(-1);
}

//bool KeyDown(EventData evt);
void exTool_KeyDown(int* pcontext)
{
    ICB_Context* icc = reinterpret_cast <ICB_Context*>(pcontext);
    exTool_EventData evt = *reinterpret_cast <exTool_EventData*>(icc->rfsp - 12);

    bool b = false;
    b = evt.message == WM_KEYDOWN;

    icc->sp -= sizeof(bool);
    *reinterpret_cast<bool*>(&icc->getA(0)) = b;
    icc->Amove_pivot(-1);
    /**reinterpret_cast <bool*>(icc->sp) = b;
    icc->getA(0) = icc->sp - icc->mem;
    icc->Amove_pivot(-1);*/
}

//bool KeyUp(EventData evt);
void exTool_KeyUp(int* pcontext)
{
    ICB_Context* icc = reinterpret_cast <ICB_Context*>(pcontext);
    exTool_EventData evt = *reinterpret_cast <exTool_EventData*>(icc->rfsp - 12);

    bool b = false;
    b = evt.message == WM_KEYUP;

    icc->sp -= sizeof(bool);
    *reinterpret_cast<bool*>(&icc->getA(0)) = b;
    icc->Amove_pivot(-1);
}

//vec2f GetMousePos_Screen(EventData evt);
void exTool_GetMousePos_Screen(int* pcontext)
{
    ICB_Context* icc = reinterpret_cast <ICB_Context*>(pcontext);
    exTool_EventData evt = *reinterpret_cast <exTool_EventData*>(icc->rfsp - 12);

    shp::vec2f screenpos = GetMousePos(evt.lP);

    icc->sp -= sizeof(vec2f);
    *reinterpret_cast <vec2f*>(icc->sp) = screenpos;
    icc->getA(0) = icc->sp - icc->mem;
    icc->Amove_pivot(-1);
}

//vec2f GetMousePos_Object(EventData evt);
void exTool_GetMousePos_Object(int* pcontext)
{
    ICB_Context* icc = reinterpret_cast <ICB_Context*>(pcontext);
    exTool_EventData evt = *reinterpret_cast <exTool_EventData*>(icc->rfsp - 12);

    shp::vec2f cpos = GetMousePos(evt.lP);
    shp::vec3f campos = GetObjectPos(cpos);
    cpos = campos.getv2();

    icc->sp -= sizeof(vec2f);
    *reinterpret_cast <vec2f*>(icc->sp) = cpos;
    icc->getA(0) = icc->sp - icc->mem;
    icc->Amove_pivot(-1);
}

//vec2f Convert_ScreenPosToObjectPos(vec2f screenPos);
void exTool_Convert_ScreenPosToObjectPos(int* pcontext)
{
    ICB_Context* icc = reinterpret_cast <ICB_Context*>(pcontext);
    shp::vec2f scpos = *reinterpret_cast <shp::vec2f*>(icc->rfsp - 8);

    shp::vec3f campos = GetObjectPos(scpos);
    scpos = campos.getv2();

    icc->sp -= sizeof(vec2f);
    *reinterpret_cast <vec2f*>(icc->sp) = scpos;
    icc->getA(0) = icc->sp - icc->mem;
    icc->Amove_pivot(-1);
}

//vec2f Convert_ObjectPosToScreenPos(vec2f objectPos);
void exTool_Convert_ObjectPosToScreenPos(int* pcontext)
{
    ICB_Context* icc = reinterpret_cast <ICB_Context*>(pcontext);
    shp::vec2f objpos = *reinterpret_cast <shp::vec2f*>(icc->rfsp - 8);

    shp::vec2f campos = GetScreenPos(shp::vec3f(objpos.x, objpos.y, 0));

    icc->sp -= sizeof(vec2f);
    *reinterpret_cast <vec2f*>(icc->sp) = campos;
    icc->getA(0) = icc->sp - icc->mem;
    icc->Amove_pivot(-1);
}

//unsigned int getEventKey(EventData evt);
void exTool_getEventKey(int* pcontext)
{
    ICB_Context* icc = reinterpret_cast <ICB_Context*>(pcontext);
    exTool_EventData evt = *reinterpret_cast <exTool_EventData*>(icc->rfsp - 12);

    icc->sp -= sizeof(unsigned int);
    *reinterpret_cast<unsigned*>(&icc->getA(0)) = evt.wP;
    icc->Amove_pivot(-1);

    //*reinterpret_cast <unsigned int*>(icc->sp) = evt.wP;
    //icc->getA(0) = icc->sp - icc->mem;
    //icc->Amove_pivot(-1);
}

//pRenderData _rdLine(float fx, float fy, float lx, float ly, gcolor color);
void exTool__rdLine(int* pcontext)
{
    ICB_Context* icc = reinterpret_cast <ICB_Context*>(pcontext);
    shp::rect4f loc = *reinterpret_cast <shp::rect4f*>(icc->rfsp - 32);
    gcolor color = *reinterpret_cast <gcolor*>(icc->rfsp - 16);

    exTool_RenderData_Line* rd = (exTool_RenderData_Line*)fm->_New(sizeof(exTool_RenderData_Line), true);
    rd->type = exTool_PostRenderDataType::prdt_Line;
    rd->loc = loc;
    rd->color = DX11Color(color.r, color.g, color.b, color.a);

    exTool_pRenderData rdptr;
    rdptr.data = reinterpret_cast<int*>(rd);

    icc->sp -= sizeof(exTool_pRenderData);
    *reinterpret_cast <exTool_pRenderData*>(icc->sp) = rdptr;
    icc->getA(0) = icc->sp - icc->mem;
    icc->Amove_pivot(-1);
}

//pRenderData _rdFillRect(float fx, float fy, float lx, float ly, gcolor color);
void exTool__rdFillRect(int* pcontext)
{
    ICB_Context* icc = reinterpret_cast <ICB_Context*>(pcontext);
    shp::rect4f loc = *reinterpret_cast <shp::rect4f*>(icc->rfsp - 32);
    gcolor color = *reinterpret_cast <gcolor*>(icc->rfsp - 16);

    exTool_RenderData_FillRect* rd = (exTool_RenderData_FillRect*)fm->_New(sizeof(exTool_RenderData_FillRect), true);
    rd->type = exTool_PostRenderDataType::prdt_FillRect;
    rd->loc = loc;
    rd->color = DX11Color(color.r, color.g, color.b, color.a);

    exTool_pRenderData rdptr;
    rdptr.data = reinterpret_cast<int*>(rd);

    icc->sp -= sizeof(exTool_pRenderData);
    *reinterpret_cast <exTool_pRenderData*>(icc->sp) = rdptr;
    icc->getA(0) = icc->sp - icc->mem;
    icc->Amove_pivot(-1);
}

//pRenderData _rdCircle(float cx, float cy, float raidus, gcolor color);
void exTool__rdCircle(int* pcontext)
{
    ICB_Context* icc = reinterpret_cast <ICB_Context*>(pcontext);
    shp::vec2f cen = *reinterpret_cast <shp::vec2f*>(icc->rfsp - 28);
    float rad = *reinterpret_cast <float*>(icc->rfsp - 20);
    gcolor color = *reinterpret_cast <gcolor*>(icc->rfsp - 16);

    exTool_RenderData_Circle* rd = (exTool_RenderData_Circle*)fm->_New(sizeof(exTool_RenderData_Circle), true);
    rd->type = exTool_PostRenderDataType::prdt_Circle;
    rd->center = cen;
    rd->radius = rad;
    rd->color = DX11Color(color.r, color.g, color.b, color.a);

    exTool_pRenderData rdptr;
    rdptr.data = reinterpret_cast<int*>(rd);

    icc->sp -= sizeof(exTool_pRenderData);
    *reinterpret_cast <exTool_pRenderData*>(icc->sp) = rdptr;
    icc->getA(0) = icc->sp - icc->mem;
    icc->Amove_pivot(-1);
}

//pRenderData _rdText(float fx, float fy, float lx, float ly, char* string, gcolor color, float fontsiz);
void exTool__rdText(int* pcontext)
{
    ICB_Context* icc = reinterpret_cast <ICB_Context*>(pcontext);
    shp::rect4f loc = *reinterpret_cast <shp::rect4f*>(icc->rfsp - 44);
    char* strptr = *reinterpret_cast <char**>(icc->rfsp - 28);
    gcolor color = *reinterpret_cast <gcolor*>(icc->rfsp - 20);
    float fontsiz = *reinterpret_cast <float*>(icc->rfsp - 4);

    exTool_RenderData_Text* rd = (exTool_RenderData_Text*)fm->_New(sizeof(exTool_RenderData_Text), true);
    rd->type = exTool_PostRenderDataType::prdt_Text;
    rd->loc = loc;
    unsigned int n = strlen(strptr) + 1;
    rd->strptr = (char*)fm->_New(n, true);
    strcpy_s(rd->strptr, n, strptr);
    rd->capacity = n;
    rd->color = DX11Color(color.r, color.g, color.b, color.a);
    rd->fontsiz = fontsiz;

    exTool_pRenderData rdptr;
    rdptr.data = reinterpret_cast<int*>(rd);

    icc->sp -= sizeof(exTool_pRenderData);
    *reinterpret_cast <exTool_pRenderData*>(icc->sp) = rdptr;
    icc->getA(0) = icc->sp - icc->mem;
    icc->Amove_pivot(-1);
}

//void ChangeRenderData(pRenderData* rdptr, pRenderData new_rd, int layer);
void exTool_ChangeRenderData(int* pcontext)
{
    ICB_Context* icc = reinterpret_cast <ICB_Context*>(pcontext);
    uint64_t logicAddr = *reinterpret_cast <uint64_t*>(icc->rfsp - 20);
    exTool_pRenderData* prd = reinterpret_cast <exTool_pRenderData*>(&icc->mem[logicAddr]);
    exTool_pRenderData newrd = *reinterpret_cast <exTool_pRenderData*>(icc->rfsp - 12);
    int layer = *reinterpret_cast<int*>(icc->rfsp - 4);

    void (*ReleaseFunc)(void*) = nullptr;
    //prd 에 아무것도 적용되지 않았을때 접근하는 것이 오류임.

    if (prd->data != nullptr) {
        switch (*(exTool_PostRenderDataType*)prd->data) {
        case exTool_PostRenderDataType::prdt_Line:
            ReleaseFunc = exTool_ReleaseLine;
            break;
        case exTool_PostRenderDataType::prdt_FillRect:
            ReleaseFunc = exTool_ReleaseFillRect;
            break;
        case exTool_PostRenderDataType::prdt_Circle:
            ReleaseFunc = exTool_ReleaseCircle;
            break;
        case exTool_PostRenderDataType::prdt_Text:
            ReleaseFunc = exTool_ReleaseText;
            break;
        }

        exTool_pPostRenderTarget prtarget;
        prtarget.data = *(int**)&icc->datamem[8];
        exTool_PostRenderManager* prmanager = (exTool_PostRenderManager*)prtarget.data;
        prmanager->ChangeRenderData(prd, newrd, layer, ReleaseFunc);
    }
    else {
        exTool_pPostRenderTarget prtarget;
        prtarget.data = *(int**)&icc->datamem[8];
        exTool_PostRenderManager* prmanager = (exTool_PostRenderManager*)prtarget.data;
        prmanager->ChangeRenderData(prd, newrd, layer, ReleaseFunc);
    }
}

//void ReleaseRenderData(int layer);
void exTool_ReleaseRenderData(int* pcontext)
{
    ICB_Context* icc = reinterpret_cast <ICB_Context*>(pcontext);
    int layer = *reinterpret_cast<int*>(icc->rfsp - 4);

    exTool_pPostRenderTarget prtarget;
    prtarget.data = *(int**)&icc->datamem[8];
    exTool_PostRenderManager* prmanager = (exTool_PostRenderManager*)prtarget.data;
    prmanager->ReleaseLayer(layer);
}

//void ICBE_SetFocus(pICBEditor icbe, bool enable);
void exTool_ICBE_SetFocus(int* pcontext)
{
    ICB_Context* icc = reinterpret_cast <ICB_Context*>(pcontext);
    exTool_pICBEditor icbe = *reinterpret_cast<exTool_pICBEditor*>(icc->rfsp - 9);
    bool b = *reinterpret_cast<bool*>(icc->rfsp - 1);
    ICB_Editor* editor = (ICB_Editor*)icbe.data;
    editor->focus = b;
}

//void ICBE_SelectLine(pICBEditor icbe, int line);
void exTool_ICBE_SelectLine(int* pcontext)
{
    ICB_Context* icc = reinterpret_cast <ICB_Context*>(pcontext);
    exTool_pICBEditor icbe = *reinterpret_cast<exTool_pICBEditor*>(icc->rfsp - 12);
    int line = *reinterpret_cast<int*>(icc->rfsp - 4);
    ICB_Editor* editor = (ICB_Editor*)icbe.data;
    editor->selected_codeline = line;
}

//void ICBE_SelectCharIndex(pICBEditor icbe, int cindex);
void exTool_ICBE_SelectCharIndex(int* pcontext)
{
    ICB_Context* icc = reinterpret_cast <ICB_Context*>(pcontext);
    exTool_pICBEditor icbe = *reinterpret_cast<exTool_pICBEditor*>(icc->rfsp - 12);
    int line = *reinterpret_cast<int*>(icc->rfsp - 4);
    ICB_Editor* editor = (ICB_Editor*)icbe.data;
    editor->selected_charindex = line;
}

//void ICBE_InsertLine(pICBEditor icbe, int line);
void exTool_ICBE_InsertLine(int* pcontext)
{
    ICB_Context* icc = reinterpret_cast <ICB_Context*>(pcontext);
    exTool_pICBEditor icbe = *reinterpret_cast<exTool_pICBEditor*>(icc->rfsp - 12);
    int line = *reinterpret_cast<int*>(icc->rfsp - 4);
    ICB_Editor* editor = (ICB_Editor*)icbe.data;
    editor->codeLines.insert(line, fmlwstr());
    editor->codeLines.at(line).NULLState();
    editor->codeLines.at(line).Init(8, false);
}

//void ICBE_AddString(pICBEditor icbe, char* str);
void exTool_ICBE_AddString(int* pcontext)
{
    ICB_Context* icc = reinterpret_cast <ICB_Context*>(pcontext);
    exTool_pICBEditor icbe = *reinterpret_cast<exTool_pICBEditor*>(icc->rfsp - 16);
    char* str = *reinterpret_cast<char**>(icc->rfsp - 8);
    ICB_Editor* editor = (ICB_Editor*)icbe.data;
    
    int len = strlen(str);
    for (int i = 0; i < len; ++i) {
        editor->WhenCharInsert(editor->selected_codeline, editor->selected_charindex, (wchar_t)str[i]);
    }
}

//int ICBE_GetCurrentLine(pICBEditor icbe);
void exTool_ICBE_GetCurrentLine(int* pcontext)
{
    ICB_Context* icc = reinterpret_cast <ICB_Context*>(pcontext);
    exTool_pICBEditor icbe = *reinterpret_cast<exTool_pICBEditor*>(icc->rfsp - 8);
    ICB_Editor* editor = (ICB_Editor*)icbe.data;

    icc->sp -= sizeof(int);
    *reinterpret_cast<int*>(&icc->getA(0)) = editor->selected_codeline;
    icc->Amove_pivot(-1);
}

//char* FloatToString(float f, int siz);
void exTool_FloatToString(int* pcontext) {
    ICB_Context* icc = reinterpret_cast<ICB_Context*>(pcontext);
    float f = *reinterpret_cast<float*>(icc->rfsp - 8);
    string str = to_string(f);
    unsigned int capacity = *reinterpret_cast<unsigned int*>(icc->rfsp - 4);
    unsigned int Siz = str.size()+1;
    icc->sp -= Siz;
    char* out = (char*)icc->sp;
    for (int i = 0; i < Siz; ++i) {
        out[i] = str[i];
    }
    out[str.size()] = 0;
    icc->getA(0) = icc->sp - icc->mem;
    icc->Amove_pivot(-1);
}

ICB_Extension* Init_exTool() {
    //확장을 입력.
    ofstream& icl = InsideCode_Bake::icl;
    icl << "Create_New_ICB_Extension_Init : exEvent Initialization...";

    bool icldetail = InsideCode_Bake::GetICLFlag(ICL_FLAG::Create_New_ICB_Extension_Init);
    if (icldetail) icl << "start" << endl;

    if (icldetail) icl << "Create_New_ICB_Extension_Init Allocate Extension Data Memory...";
    ICB_Extension* ext = (ICB_Extension*)fm->_New(sizeof(ICB_Extension), true);
    ext->exfuncArr.NULLState();
    ext->exfuncArr.Init(64, false);
    ext->exstructArr.NULLState();
    ext->exstructArr.Init(32, false);
    if (icldetail) icl << "finish" << endl;

    if (icldetail) icl << "Create_New_ICB_Extension_Init__Bake_Extension...";
    bake_Extension("Extensions/exTool.txt", ext);
    if (icldetail) icl << "finish" << endl;

    if (icldetail) icl << "Create_New_ICB_Extension_Init Set_Function_Pointers start" << endl;
    int i = -1;
    ext->exfuncArr[++i]->start_pc = reinterpret_cast <byte8*>(exTool_PopEvent);
    ext->exfuncArr[++i]->start_pc = reinterpret_cast <byte8*>(exTool_LBtnDown);
    ext->exfuncArr[++i]->start_pc = reinterpret_cast <byte8*>(exTool_LBtnUp);
    ext->exfuncArr[++i]->start_pc = reinterpret_cast <byte8*>(exTool_RBtnDown);
    ext->exfuncArr[++i]->start_pc = reinterpret_cast <byte8*>(exTool_RBtnUp);
    ext->exfuncArr[++i]->start_pc = reinterpret_cast <byte8*>(exTool_MouseMove);
    ext->exfuncArr[++i]->start_pc = reinterpret_cast <byte8*>(exTool_KeyDown);
    ext->exfuncArr[++i]->start_pc = reinterpret_cast <byte8*>(exTool_KeyUp);
    ext->exfuncArr[++i]->start_pc = reinterpret_cast <byte8*>(exTool_GetMousePos_Screen);
    ext->exfuncArr[++i]->start_pc = reinterpret_cast <byte8*>(exTool_GetMousePos_Object);
    ext->exfuncArr[++i]->start_pc = reinterpret_cast <byte8*>(exTool_Convert_ScreenPosToObjectPos);
    ext->exfuncArr[++i]->start_pc = reinterpret_cast <byte8*>(exTool_Convert_ObjectPosToScreenPos);
    ext->exfuncArr[++i]->start_pc = reinterpret_cast <byte8*>(exTool_getEventKey);
    ext->exfuncArr[++i]->start_pc = reinterpret_cast <byte8*>(exTool__rdLine);
    ext->exfuncArr[++i]->start_pc = reinterpret_cast <byte8*>(exTool__rdFillRect);
    ext->exfuncArr[++i]->start_pc = reinterpret_cast <byte8*>(exTool__rdCircle);
    ext->exfuncArr[++i]->start_pc = reinterpret_cast <byte8*>(exTool__rdText);
    ext->exfuncArr[++i]->start_pc = reinterpret_cast <byte8*>(exTool_ChangeRenderData);
    ext->exfuncArr[++i]->start_pc = reinterpret_cast <byte8*>(exTool_ReleaseRenderData);
    ext->exfuncArr[++i]->start_pc = reinterpret_cast <byte8*>(exTool_ICBE_SetFocus);
    ext->exfuncArr[++i]->start_pc = reinterpret_cast <byte8*>(exTool_ICBE_SelectLine);
    ext->exfuncArr[++i]->start_pc = reinterpret_cast <byte8*>(exTool_ICBE_SelectCharIndex);
    ext->exfuncArr[++i]->start_pc = reinterpret_cast <byte8*>(exTool_ICBE_InsertLine);
    ext->exfuncArr[++i]->start_pc = reinterpret_cast <byte8*>(exTool_ICBE_AddString);
    ext->exfuncArr[++i]->start_pc = reinterpret_cast <byte8*>(exTool_ICBE_GetCurrentLine);
    ext->exfuncArr[++i]->start_pc = reinterpret_cast <byte8*>(exTool_FloatToString);
    icl << "finish" << endl;
    return ext;
}

#endif