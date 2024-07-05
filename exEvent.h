//version 0.1
#ifndef EXEVENT_H
#define EXEVENT_H

#include "ICB_Extension.h"

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