// Menus.cpp

#include <Arduino.h>
#include "A1Main.h"
#include "Menus.h"
#include "PotKnob.h"
#include "ButtonUtil.h"
#include "MorseCode.h"
#include "debug.h"

unsigned long menuIdleTimeOut = 0;
boolean menuCycle = true;

// ###############################################################################
void checkKnob(int menu) {
    int dir;
    
    dir = doPotKnob();
    
    if (!dir) return;
 
    menuIdleTimeOut = 0;
    
    if (menuCycle) {
        menuActive += dir;
        menuActive = constrain (menuActive, 1, MENUS);
        refreshDisplay++;
        updateDisplayMenu(menuActive);
        return;
    }
  
    switch(menu) {
        case 6:
          cw_wpm += dir;        
          cw_wpm = constrain (cw_wpm, 1, 99);
          //refreshDisplay++;
          break;
        case 7:
          if (qrssDitTime>1000) {
              qrssDitTime += dir * 1000;
              qrssDitTime = qrssDitTime/1000 * 1000;
              qrssDitTime = constrain (qrssDitTime, 1000, 60000);
          }
          else qrssDitTime += dir * 10;        
          qrssDitTime = constrain (qrssDitTime, 250, 60000);
          //refreshDisplay++;
          break;
        case 8:
          blinkTime += dir * 1000;
          blinkTime = constrain (blinkTime, 5000, 300000);
          //refreshDisplay++;
          break;
        case 9:
          blinkRate += dir * 10;
          blinkRate = constrain (blinkRate, 100, 2000);
          //refreshDisplay++;
          break;
        default:;
    }
    refreshDisplay++;
}

// ###############################################################################
void doMenus(int menu) {
    
    if (!menuIdleTimeOut) menuIdleTimeOut = millis() + 1000L * MENU_IDLE_TIMEOUT_SEC;
  
    checkKnob(menu);       
    checkButtonMenu();
    
    if (menuIdleTimeOut && menuIdleTimeOut < millis()) { // If IdleTimeOut, Abort
      menuCycle = true;
      menuIdleTimeOut = 0;
      menuActive = 0;
      refreshDisplay++;
    }
      
    if(menuActive) updateDisplayMenu(menu);
}

// ###############################################################################
void updateDisplayMenu(int menu) {

  if (refreshDisplay > 0) {
      refreshDisplay--;

      //sprintf(c, P("%0.2d-Menu:"), menu);
      //printLineCEL(MENU_PROMPT_LINE, c);
      switch (menu) {
          case 0:
             sprintf(c, P("Exit Menu"), menu);
             printLineCEL(MENU_PROMPT_LINE, c);
             printLineCEL(MENU_ITEM_LINE, P(" "));
             break;
          case 6:
             sprintf(c, P("%0.2dMACRO CW SPD"), menu);
             printLineCEL(MENU_PROMPT_LINE, c);
             sprintf(c, P("WPM: %0.2d"), cw_wpm);
             if(!menuCycle) sprintf(c, P2("%s<"), c);
             printLineCEL(MENU_ITEM_LINE, c);
             break;
          case 7:
             sprintf(c, P("%0.2dMACRO QRSS DIT"), menu);
             printLineCEL(MENU_PROMPT_LINE, c);
             if (qrssDitTime>=1000) sprintf(c, P(" SECs: %0.2d"), qrssDitTime/1000);
             else sprintf(c, P("MSECs: %d"), qrssDitTime);
             if(!menuCycle) sprintf(c, P2("%s<"), c);
             printLineCEL(MENU_ITEM_LINE, c);
             break;
          case 8:
             sprintf(c, P("%0.2dBlink TimeOut"), menu);
             printLineCEL(MENU_PROMPT_LINE, c);
             sprintf(c, P("SECs: %d"), blinkTime/1000);
             if(!menuCycle) sprintf(c, P2("%s<"), c);
             printLineCEL(MENU_ITEM_LINE, c);
             break;
          case 9:
             sprintf(c, P("%0.2dBlink Rate"), menu);
             printLineCEL(MENU_PROMPT_LINE, c);
             sprintf(c, P("MSECs: %d"), blinkRate);
             if(!menuCycle) sprintf(c, P2("%s<"), c);
             printLineCEL(MENU_ITEM_LINE, c);
             break;
          default: 
             sprintf(c, P("%0.2dMenu:"), menu);
             printLineCEL(MENU_PROMPT_LINE, c);
             printLineCEL(MENU_ITEM_LINE, P("---"));
             break;
      }
  }
}

// -------------------------------------------------------------------------------
void checkButtonMenu() {
#define DEBUG(x ...)
//#define DEBUG(x ...) debugUnique(x)    // UnComment for Debug
//#define DEBUG(x ...) debug(x)    // UnComment for Debug
  int btn;
  
  btn = btnDown();
  if (btn) DEBUG(P("%s %d: btn %d"), __func__, __LINE__, btn);

  menuPrev = menuActive;
  switch (btn) {
    case 0: return; // Abort
    case UP_BTN: menuActive = constrain (menuActive+1, 1, MENUS); break;
    case DN_BTN: menuActive = constrain (menuActive-1, 1, MENUS); break;
    case RT_BTN: switch (getButtonPushMode(btn)) {
            case MOMENTARY_PRESS: menuCycle = !menuCycle; break;
            case DOUBLE_PRESS: menuActive = 0; menuCycle = true; refreshDisplay++; break; // Return to VFO Display Mode
            default: break;
            }
  }
  DEBUG(P("%s %d: MenuActive %d"), __func__, __LINE__, menuActive);
  menuIdleTimeOut = 0;
  refreshDisplay++;
  updateDisplayMenu(menuActive);
  deDounceBtnRelease(); // Wait for Button Release
}


// ENd
