//version 0.1
#ifndef EXEVENT_H
#define EXEVENT_H

#include "ICB_Extension.h"
#include "exGeometry.h"

struct EventData {
    bool keydownhit;
    bool keyuphit;
    bool keydown;
    char key;

    float mouseX;
    float mouseY;

    bool mouse_leftdownhit;
    bool mouse_rightdownhit;
    bool mouse_leftuphit;
    bool mouse_rightuphit;
    bool mouse_leftdown;
    bool mouse_rightdown;

    unsigned int currentEvent;
};

struct RenderData {
    float params[4];
    wchar_t* ptr;
};
// if ptr != nullptr -> text
// if param[3] == nan -> circle
// else -> line

struct rdBuffer {
    fmvecarr<RenderData>* data[2];
};

//RenderData _rdLine(float fx, float fy, float lx, float ly);
void exTool__rdLine(int* pcontext) {
    ICB_Context* icc = reinterpret_cast<ICB_Context*>(pcontext);
    RenderData rd;
    rd.params[0] = *reinterpret_cast<float*>(icc->rfsp - 32);
    rd.params[1] = *reinterpret_cast<float*>(icc->rfsp - 24);
    rd.params[2] = *reinterpret_cast<float*>(icc->rfsp - 16);
    rd.params[3] = *reinterpret_cast<float*>(icc->rfsp - 8);
    rd.ptr = nullptr;
    icc->sp -= sizeof(RenderData);
    *reinterpret_cast<RenderData*>(icc->sp) = rd;

    icc->getA(0) = icc->sp - icc->mem;
    icc->Amove_pivot(-1);
}

//RenderData _rdCircle(float cx, float cy, float raidus);
void exTool__rdCircle(int* pcontext) {
    ICB_Context* icc = reinterpret_cast<ICB_Context*>(pcontext);
    RenderData rd;
    rd.params[0] = *reinterpret_cast<float*>(icc->rfsp - 24);
    rd.params[1] = *reinterpret_cast<float*>(icc->rfsp - 16);
    rd.params[2] = *reinterpret_cast<float*>(icc->rfsp - 8);
    rd.params[3] = NAN;
    rd.ptr = nullptr;
    icc->sp -= sizeof(RenderData);
    *reinterpret_cast<RenderData*>(icc->sp) = rd;

    icc->getA(0) = icc->sp - icc->mem;
    icc->Amove_pivot(-1);
}

//RenderData _rdText(float fx, float fy, float lx, float ly, char* string);
void exTool__rdText(int* pcontext) {
    ICB_Context* icc = reinterpret_cast<ICB_Context*>(pcontext);
    RenderData rd;
   
    rd.params[0] = *reinterpret_cast<float*>(icc->rfsp - 40);
    rd.params[1] = *reinterpret_cast<float*>(icc->rfsp - 32);
    rd.params[2] = *reinterpret_cast<float*>(icc->rfsp - 24);
    rd.params[3] = *reinterpret_cast<float*>(icc->rfsp - 16);
    char* strptr = *reinterpret_cast<char**>(icc->rfsp - 8);
    int len = strlen(strptr);
    rd.ptr = (wchar_t*)fm->_New(sizeof(wchar_t) * (len + 1), true);
    for (int i = 0; i < len; ++i) {
        rd.ptr[i] = (wchar_t)strptr[i];
    }
    rd.ptr[len] = 0;

    icc->sp -= sizeof(RenderData);
    *reinterpret_cast<RenderData*>(icc->sp) = rd;

    icc->getA(0) = icc->sp - icc->mem;
    icc->Amove_pivot(-1);
}

//void PushRD_ready(rdbuffer rdb, RenderData rd);
void exTool_PushRD_ready(int* pcontext) {
    ICB_Context* icc = reinterpret_cast<ICB_Context*>(pcontext);
    
    rdBuffer rdb = *reinterpret_cast<rdBuffer*>(icc->rfsp - 40);
    RenderData rd = *reinterpret_cast<RenderData*>(icc->rfsp - 24);

    rdb.data[1]->push_back(rd);
}

//void rdbuffer_push(rdbuffer rdb);
void exTool_rdbuffer_push(int* pcontext) {
    ICB_Context* icc = reinterpret_cast<ICB_Context*>(pcontext);

    rdBuffer rdb = *reinterpret_cast<rdBuffer*>(icc->rfsp - 16);
    fmvecarr<RenderData>* temp = nullptr;
    temp = rdb.data[0];
    rdb.data[0] = rdb.data[1];
    rdb.data[1] = temp;
    for (int i = 0; i < rdb.data[1]->size(); ++i) {
        if (rdb.data[1]->at(i).ptr != nullptr) {
            wchar_t* wstr = rdb.data[1]->at(i).ptr;
            int len = wcslen(wstr);
            fm->_Delete((byte8*)wstr, sizeof(wchar_t) * (len + 1));
            rdb.data[1]->at(i).ptr = nullptr;
        }
    }
    rdb.data[1]->up = 0;
}

//vec2f GetObjPosFromCamPos(vec2f cp);
//vec2f GetCamPosFromObjPos(vec2f op);
//void CameraMove(vec2f dest);

ICB_Extension* Init_exEvent() {
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
    bake_Extension("exEvent.txt", ext);
    if (icldetail) icl << "finish" << endl;

    if (icldetail) icl << "Create_New_ICB_Extension_Init Set_Function_Pointers start" << endl;
    int i = -1;
    icl << "finish" << endl;
    return ext;
}

#endif