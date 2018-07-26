#pragma once

#include "SDK.h"

void __fastcall Hooked_FrameStageNotify(void* _this, void* _edx, ClientFrameStage_t stage);
bool __fastcall Hooked_CreateMove(PVOID ClientMode, int edx, float input_sample_frametime, CUserCmd* pCommand);
int __fastcall Hooked_KeyEvent(PVOID CHLClient, int edx, int eventcode, int keynum, const char *currentBinding);
