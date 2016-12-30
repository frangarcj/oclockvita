// Button Swap taiHEN plugin
// Copyright (c) 2016 Scorp

#include <psp2/kernel/modulemgr.h>
#include <psp2/display.h>
#include <psp2/ctrl.h>
#include <psp2/power.h>
#include <taihen.h>
#include "blit.h"

#define LEFT_LABEL_X CENTER(18)
#define RIGHT_LABEL_X CENTER(0)

static SceUID g_hooks[5];
int showMenu = 0;
int mode = 0;

static int profile_max_battery[] = {111, 111, 111, 111};
static int profile_default[] = {266, 166, 166, 111};
static int profile_max_performance[] = {444, 222, 222, 166};
static int profile_game[] = {444, 222, 222, 166};
static int* profiles[4] = {profile_game,profile_default,profile_max_performance,profile_max_battery};

static tai_hook_ref_t ref_hook0;
int sceDisplaySetFrameBuf_patched(const SceDisplayFrameBuf *pParam, int sync) {
    blit_set_frame_buf(pParam);
    if(showMenu){
      blit_set_color(0x00FFFFFF, 0x0033CC33);
			blit_stringf(LEFT_LABEL_X, 88, "SIMPLE OVERCLOCK");

			blit_set_color(0x00FFFFFF, 0x0033CC33);
			blit_stringf(LEFT_LABEL_X, 120, "PROFILE    ");

			switch(mode) {
            		case 3: //max battery
                		blit_stringf(RIGHT_LABEL_X, 120, "Max Batt.");
                		break;
            		case 1: //default
                		blit_stringf(RIGHT_LABEL_X, 120, "Default  ");
                		break;
            		case 2: //default
                		blit_stringf(RIGHT_LABEL_X, 120, "Max Perf.");
                		break;
            		case 0:
                		blit_stringf(RIGHT_LABEL_X, 120, "Game Def.");
                		break;
			}

			blit_stringf(LEFT_LABEL_X, 136, "CPU CLOCK  ");
			blit_stringf(RIGHT_LABEL_X, 136, "%-4d  MHz", scePowerGetArmClockFrequency());
			blit_stringf(LEFT_LABEL_X, 152, "BUS CLOCK  ");
			blit_stringf(RIGHT_LABEL_X, 152, "%-4d  MHz", scePowerGetBusClockFrequency());
			blit_stringf(LEFT_LABEL_X, 168, "GPU CLOCK  ");
			blit_stringf(RIGHT_LABEL_X, 168, "%-4d  MHz", scePowerGetGpuClockFrequency());
			blit_stringf(LEFT_LABEL_X, 184, "XBAR CLOCK ");
			blit_stringf(RIGHT_LABEL_X, 184, "%-4d  MHz", scePowerGetGpuXbarClockFrequency());

    }
    return TAI_CONTINUE(int, ref_hook0, pParam, sync);
}   

int checkButtons(int port, tai_hook_ref_t ref_hook, SceCtrlData *ctrl, int count) {
  int ret;

  if (ref_hook == 0)
     ret = 1;
  else
  {
    ret = TAI_CONTINUE(int, ref_hook, port, ctrl, count);     
          
    if(showMenu){
      
      if (mode > 0 && (ctrl->buttons & SCE_CTRL_LEFT)){
        mode--;
        scePowerSetArmClockFrequency(profiles[mode][0]);
        scePowerSetBusClockFrequency(profiles[mode][1]);
	      scePowerSetGpuClockFrequency(profiles[mode][2]);
        scePowerSetGpuXbarClockFrequency(profiles[mode][3]);
      }else if (mode <3 && (ctrl->buttons & SCE_CTRL_RIGHT)){
        mode++;
        scePowerSetArmClockFrequency(profiles[mode][0]);
        scePowerSetBusClockFrequency(profiles[mode][1]);
	      scePowerSetGpuClockFrequency(profiles[mode][2]);
        scePowerSetGpuXbarClockFrequency(profiles[mode][3]);
      }else if ((ctrl->buttons & SCE_CTRL_SELECT) && (ctrl->buttons & SCE_CTRL_DOWN)){
        showMenu = 0;
      }

      ctrl->buttons = 0;
       
     }else{
       
       if ((ctrl->buttons & SCE_CTRL_SELECT) && (ctrl->buttons & SCE_CTRL_UP)){         
         if(mode==0){
            profile_game[0] = scePowerGetArmClockFrequency();
          	profile_game[1] = scePowerGetBusClockFrequency();
          	profile_game[2] = scePowerGetGpuClockFrequency();
          	profile_game[3] = scePowerGetGpuXbarClockFrequency();
         }
         showMenu = 1;
       }
       
     }
     
  }

  return ret;
}

static tai_hook_ref_t ref_hook1;
static int keys_patched1(int port, SceCtrlData *ctrl, int count) {
    return checkButtons(port, ref_hook1, ctrl, count);
}   

static tai_hook_ref_t ref_hook2;
static int keys_patched2(int port, SceCtrlData *ctrl, int count) {
    return checkButtons(port, ref_hook2, ctrl, count);
}   

static tai_hook_ref_t ref_hook3;
static int keys_patched3(int port, SceCtrlData *ctrl, int count) {
    return checkButtons(port, ref_hook3, ctrl, count);
}   

static tai_hook_ref_t ref_hook4;
static int keys_patched4(int port, SceCtrlData *ctrl, int count) {
    return checkButtons(port, ref_hook4, ctrl, count);
}   

void _start() __attribute__ ((weak, alias ("module_start")));

int module_start(SceSize argc, const void *args) {

  g_hooks[0] = taiHookFunctionImport(&ref_hook0, 
                                      TAI_MAIN_MODULE,
                                      TAI_ANY_LIBRARY,
                                      0x7A410B64, // sceDisplaySetFrameBuf
                                      sceDisplaySetFrameBuf_patched);
  g_hooks[1] = taiHookFunctionImport(&ref_hook1, 
                                      TAI_MAIN_MODULE,
                                      TAI_ANY_LIBRARY,
                                      0xA9C3CED6, // sceCtrlPeekBufferPositive
                                      keys_patched1);

  g_hooks[2] = taiHookFunctionImport(&ref_hook2, 
                                      TAI_MAIN_MODULE,
                                      TAI_ANY_LIBRARY,
                                      0x15F81E8C, // sceCtrlPeekBufferPositive2
                                      keys_patched2);

  g_hooks[3] = taiHookFunctionImport(&ref_hook3, 
                                      TAI_MAIN_MODULE,
                                      TAI_ANY_LIBRARY,
                                      0x67E7AB83, // sceCtrlReadBufferPositive
                                      keys_patched3);

  g_hooks[4] = taiHookFunctionImport(&ref_hook4, 
                                      TAI_MAIN_MODULE,
                                      TAI_ANY_LIBRARY,
                                      0xC4226A3E, // sceCtrlReadBufferPositive2
                                      keys_patched4);

  return SCE_KERNEL_START_SUCCESS;
}

int module_stop(SceSize argc, const void *args) {

  // free hooks that didn't fail
  if (g_hooks[0] >= 0) taiHookRelease(g_hooks[0], ref_hook0);
  if (g_hooks[1] >= 0) taiHookRelease(g_hooks[1], ref_hook1);
  if (g_hooks[2] >= 0) taiHookRelease(g_hooks[2], ref_hook2);
  if (g_hooks[3] >= 0) taiHookRelease(g_hooks[3], ref_hook3);
  if (g_hooks[4] >= 0) taiHookRelease(g_hooks[4], ref_hook4);

  return SCE_KERNEL_STOP_SUCCESS;
}
