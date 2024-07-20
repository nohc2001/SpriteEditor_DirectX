//version 0.1
#ifndef EXEVENT_H
#define EXEVENT_H

#include "ICB_Extension.h"
#include "exGeometry.h"
#include "Utill_SpaceMath.h"
#include "DX11_UI.h"
#include "exGraphics.h"

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
    }

    void ReleaseLayer(int n) {
        if (n > 0) {
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
            hc.ptr = newv.data;
            hc.ReleaseFunc = ReleaseFunc;
            hc.shouldRelease = layer;
            heapBuffer->push_back(hc);
        }
        else {
            renderData.push_back(origin);
        }
    }
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
};
struct exTool_RenderData_Circle {
    exTool_PostRenderDataType type;
    shp::vec2f center;
    float radius;
};
struct exTool_RenderData_FillRect {
    exTool_PostRenderDataType type;
    shp::rect4f loc;
};
struct exTool_RenderData_Text {
    exTool_PostRenderDataType type;
    shp::rect4f loc;
    char* strptr;
    unsigned int capacity;
};

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
    exTool_EventSystem evtsys = *reinterpret_cast<exTool_EventSystem*>(pevtsys.data);
    exTool_EventData evt = evtsys.PopEvent();

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
    *reinterpret_cast <bool*>(icc->sp) = b;
    icc->getA(0) = icc->sp - icc->mem;
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
    *reinterpret_cast <bool*>(icc->sp) = b;
    icc->getA(0) = icc->sp - icc->mem;
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
    *reinterpret_cast <bool*>(icc->sp) = b;
    icc->getA(0) = icc->sp - icc->mem;
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
    *reinterpret_cast <bool*>(icc->sp) = b;
    icc->getA(0) = icc->sp - icc->mem;
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
    *reinterpret_cast <bool*>(icc->sp) = b;
    icc->getA(0) = icc->sp - icc->mem;
    icc->Amove_pivot(-1);
}

//bool KeyUp(EventData evt);
void exTool_KeyUp(int* pcontext)
{
    ICB_Context* icc = reinterpret_cast <ICB_Context*>(pcontext);
    exTool_EventData evt = *reinterpret_cast <exTool_EventData*>(icc->rfsp - 12);

    bool b = false;
    b = evt.message == WM_KEYUP;

    icc->sp -= sizeof(bool);
    *reinterpret_cast <bool*>(icc->sp) = b;
    icc->getA(0) = icc->sp - icc->mem;
    icc->Amove_pivot(-1);
}

//unsigned int getEventKey(EventData evt);
void exTool_getEventKey(int* pcontext)
{
    ICB_Context* icc = reinterpret_cast <ICB_Context*>(pcontext);
    exTool_EventData evt = *reinterpret_cast <exTool_EventData*>(icc->rfsp - 12);

    icc->sp -= sizeof(unsigned int);
    *reinterpret_cast <unsigned int*>(icc->sp) = evt.wP;
    icc->getA(0) = icc->sp - icc->mem;
    icc->Amove_pivot(-1);
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

    exTool_pRenderData rdptr;
    rdptr.data = reinterpret_cast<int*>(rd);

    icc->sp -= sizeof(exTool_pRenderData);
    *reinterpret_cast <exTool_pRenderData*>(icc->sp) = rdptr;
    icc->getA(0) = icc->sp - icc->mem;
    icc->Amove_pivot(-1);
}

//pRenderData _rdText(float fx, float fy, float lx, float ly, char* string, gcolor color);
void exTool__rdText(int* pcontext)
{
    ICB_Context* icc = reinterpret_cast <ICB_Context*>(pcontext);
    shp::rect4f loc = *reinterpret_cast <shp::rect4f*>(icc->rfsp - 40);
    char* strptr = *reinterpret_cast <char**>(icc->rfsp - 24);
    gcolor color = *reinterpret_cast <gcolor*>(icc->rfsp - 16);

    exTool_RenderData_Text* rd = (exTool_RenderData_Text*)fm->_New(sizeof(exTool_RenderData_Text), true);
    rd->type = exTool_PostRenderDataType::prdt_Text;
    rd->loc = loc;
    unsigned int n = strlen(strptr) + 1;
    rd->strptr = (char*)fm->_New(n, true);
    strcpy_s(rd->strptr, n, strptr);
    rd->capacity = n;

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

    void (*ReleaseFunc)(void*) = nullptr;;
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

ICB_Extension* Init_exTool() {
    //확장을 입력.
    ofstream& icl = InsideCode_Bake::icl;
    icl << "Create_New_ICB_Extension_Init : exEvent Initialization...";

    bool icldetail = InsideCode_Bake::GetICLFlag(ICL_FLAG::Create_New_ICB_Extension_Init);
    if (icldetail) icl << "start" << endl;

    if (icldetail) icl << "Create_New_ICB_Extension_Init Allocate Extension Data Memory...";
    ICB_Extension* ext = (ICB_Extension*)fm->_New(sizeof(ICB_Extension), true);
    ext->exfuncArr.NULLState();
    ext->exfuncArr.Init(32, false);
    ext->exstructArr.NULLState();
    ext->exstructArr.Init(32, false);
    if (icldetail) icl << "finish" << endl;

    if (icldetail) icl << "Create_New_ICB_Extension_Init__Bake_Extension...";
    bake_Extension("Extensions/exTool.txt", ext);
    if (icldetail) icl << "finish" << endl;

    if (icldetail) icl << "Create_New_ICB_Extension_Init Set_Function_Pointers start" << endl;
    int i = -1;
    icl << "finish" << endl;
    return ext;
}

#endif