/******************************************************************************
* Filename: RestTimeForm.cpp
* Copyright (c) 2000, UAF Development Team (email CocoaSpud@hotmail.com)
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
******************************************************************************/

#include "..\Shared\stdafx.h"

//#include "externs.h"
#include "Dungeon.h"
#include "Graphics.h"
#include "GlobalData.h"
#include "GetInput.h"
#include "Viewport.h"
#include "RestTimeForm.h"
#include "GameMenu.h"
#include "Screen.h"
#include "FormattedText.h"
#include "DispText.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static const int STARTX = textboxX;
static const int STARTY = textboxY+(16*3); // top of box + 3 lines

// As displayed:
// REST TIME:  00:00:00

enum RESTTIME_FORM // Define field names as integers for quicker comparisons
{   
  RTF_none,
  RTF_white=TEXT_FORM::white,    // White items
  
  RTF_HEADER,
  
  RTF_days,
  RTF_DAYSCOLON,
  RTF_hours,  
  RTF_HOURSCOLON,
  RTF_minutes,
  
  RTF_Days=TEXT_FORM::green+TEXT_FORM::tab, 
  RTF_Hours, // Selection rectangle includes more than one field
  RTF_Minutes //   ''     ''
};

#if _MSC_VER >= 1310
#define RTF(item,xrel,x,yrel,y) \
  DISPLAY_FORM(RTF_##xrel,RTF_##yrel,RTF_##item,x,y),
#else
#define RTF(item,xrel,x,yrel,y) \
  DISPLAY_FORM(RTF_##xrel,RTF_##yrel,RTF_##item,x,y),
#endif

#define RTFB(item,xrel,x,yrel,y) \
  DISPLAY_FORM(RTF_##xrel,RTF_##yrel,RTF_##item,x,y)

static DISPLAY_FORM restForm[] =
{
  // name, xrel, x, yrel, y
  RTF(HEADER     ,none           ,STARTX    ,none          ,STARTY)  // The header "REST TIME"
  RTF(days       ,HEADER+END     ,48        ,none          ,STARTY)  // 00  
  RTF(Days       ,days+SEL       ,0         ,days+SEL      ,0) // 
  RTF(DAYSCOLON  ,days+END       ,0         ,none          ,STARTY)  // :
  RTF(hours      ,DAYSCOLON+END  ,0         ,none          ,STARTY)  // 00
  RTF(Hours      ,hours+SEL      ,0         ,hours+SEL     ,0) //   
  RTF(HOURSCOLON ,hours+END      ,0         ,none          ,STARTY)  // :
  RTF(minutes    ,HOURSCOLON+END ,0         ,none          ,STARTY)  // 00 
  RTF(Minutes    ,minutes+SEL    ,0         ,minutes+SEL   ,0) //   

  RTF(none,none,0,none,0)  // End of list
};

static DISPLAY_FORM *pRestFormData=NULL;

// some form info is dynamic, such as the location of textboxX,textboxY depending
// on the game screen res. 
//
// create a form using current runtime values if needed
//
DISPLAY_FORM *GetRestForm()
{
  if (pRestFormData!=NULL)
    return pRestFormData;

  int STARTX2 = textboxX;
  int STARTY2 = textboxY+ (GetTextBoxCharHeight()/2); // middle of the box

  pRestFormData = new DISPLAY_FORM[10];

  pRestFormData[0] = RTFB(HEADER     ,none           ,STARTX2   ,none          ,STARTY2);  // The header "REST TIME"
  pRestFormData[1] = RTFB(days       ,HEADER+END     ,48        ,none          ,STARTY2);  // 00  
  pRestFormData[2] = RTFB(Days       ,days+SEL       ,0         ,days+SEL      ,0); // 
  pRestFormData[3] = RTFB(DAYSCOLON  ,days+END       ,0         ,none          ,STARTY2);  // :
  pRestFormData[4] = RTFB(hours      ,DAYSCOLON+END  ,0         ,none          ,STARTY2);  // 00
  pRestFormData[5] = RTFB(Hours      ,hours+SEL      ,0         ,hours+SEL     ,0); //   
  pRestFormData[6] = RTFB(HOURSCOLON ,hours+END      ,0         ,none          ,STARTY2);  // :
  pRestFormData[7] = RTFB(minutes    ,HOURSCOLON+END ,0         ,none          ,STARTY2);  // 00 
  pRestFormData[8] = RTFB(Minutes    ,minutes+SEL    ,0         ,minutes+SEL   ,0); //   
  pRestFormData[9] = RTFB(none,none,0,none,0);

  return pRestFormData;
}

void DestroyRestForm()
{
  if (pRestFormData!=NULL)
    delete [] pRestFormData;
  pRestFormData=NULL;
}

void showRestTime(TEXT_FORM *pForm, REST_MENU_DATA *pData)
{ 
   CString temp;
// prs 20191221   COLORREF color=White;
   FONT_COLOR_NUM colorNum = whiteColor;
   ASSERT(pForm!=NULL);
   pForm->ClearForm();
   
   pForm->SetText(RTF_HEADER , "REST TIME");     

   temp.Format("%02I64i", pData->m_days);
   pForm->SetText(RTF_days,    temp, colorNum);

   pForm->SetText(RTF_DAYSCOLON, ":", colorNum);

   temp.Format("%02I64i", pData->m_hours);
   pForm->SetText(RTF_hours,   temp, colorNum);

   pForm->SetText(RTF_HOURSCOLON, ":", colorNum);

   temp.Format("%02I64i", pData->m_mins);
   pForm->SetText(RTF_minutes, temp, colorNum);   
}

void displayRestTime(REST_MENU_DATA *pData)
{  
  //TEXT_FORM RestForm(restForm);
  TEXT_FORM RestForm(GetRestForm());
  showRestTime(&RestForm,pData);
  RestForm.Display(0);
}
 

static bool RTF_highlight (TEXT_FORM *frm, 
                      int *oldSelection,
                      int newSelection) 
{
  // See if new selection is valid.
  // If so, set old selection.
  // If old selection changes, highlight it and return true.
  int hi1=-1, hi2=-1;
  switch (newSelection)
  {
  case RTF_Days:
  case RTF_days:
    newSelection=RTF_Days;
    hi1=RTF_days;
    break;
  case RTF_Hours:
  case RTF_hours:
    newSelection=RTF_Hours;
    hi1=RTF_hours;
    break;
  case RTF_Minutes:
  case RTF_minutes:
    newSelection=RTF_Minutes;
    hi1=RTF_minutes;
    break;
   default:  // leave things as they are.
    newSelection=-1;
  };
  if ((newSelection!=-1) && (newSelection!=*oldSelection))
  {
    frm->Highlight(hi1);
    frm->Highlight(hi2);
    *oldSelection=newSelection;
    return true;
  };
  return false;
}

bool RTF_IncrStat(int selection, REST_MENU_DATA *pData)
{
  // Returns true if something has changed.
  switch (selection )
  {
  case RTF_Days:    
    pData->m_days++;
    break;
  case RTF_Hours:
    pData->m_hours++;
    if (pData->m_hours >= 24)
    {
      pData->m_hours=0;
      pData->m_days++;
    }
    break;
  case RTF_Minutes:
    pData->m_mins++;
    if (pData->m_mins >= 60)
    {
      pData->m_mins=0;
      pData->m_hours++;
    }
    if (pData->m_hours >= 24)
    {
      pData->m_hours=0;
      pData->m_days++;
    }
    break;
  default: return false;
  }
  return true;
}

bool RTF_DecrStat(int selection, REST_MENU_DATA *pData)
{
  // Returns true if something has changed.
  switch (selection )
  {
  case RTF_Days:    
    if (pData->m_days == 0) return false;
    pData->m_days--;
    if (pData->m_days < 0) pData->m_days = 0;
    break;
  case RTF_Hours:
    if (pData->m_hours == 0) return false;
    pData->m_hours--;
    if (pData->m_hours < 0) pData->m_hours = 0;
    break;
  case RTF_Minutes:
    if (pData->m_mins == 0) return false;
    pData->m_mins--;
    if (pData->m_mins < 0) pData->m_mins = 0;
    break;
  default: return false;
  }
  return true;
}

int handleRestTimeInput(RESTTIME_INPUT_MESSAGE_TYPE msg,
                         REST_MENU_DATA *pData,
                         int P1, 
                         int P2)
// Returns 0x0001 if you should display menu and flip surfaces.
// Returns 0x0002 if we handled a keystroke and you can ignore it
{
  TEXT_FORM RestForm(GetRestForm());

  static bool chooseActive=false;
  static int currentSelection;
  int newSelection;
  int result=0;
  bool flip=false, redraw=false;

  switch (msg)
  {
  case RESTTIME_initial:
    ASSERT(chooseActive==false);
    chooseActive=true;
    currentSelection=-1;
    redraw=true;
    break;

  case RESTTIME_refresh:
    ASSERT(chooseActive);
    redraw=true;
    break;

  case RESTTIME_mouseClick:
    ASSERT(chooseActive);
    showRestTime(&RestForm, pData);  // To define rectangles.
    newSelection=RestForm.MouseClick(P1, P2);  // P1=x, P2=y
    if (RTF_highlight(&RestForm,&currentSelection,newSelection)) flip=true;
    switch (newSelection)
    {
    case RTF_days: menu.setCurrentItem(2); break;
    case RTF_hours: menu.setCurrentItem(3); break;
    case RTF_minutes: menu.setCurrentItem(4); break;
    }
    break;

  case RESTTIME_exit:
    DestroyRestForm();
    chooseActive=false;
    break;

  case RESTTIME_activate:
    ASSERT(chooseActive);
    switch (P1)
    {
    case RTF_DAYS_FIELD:
      newSelection=RTF_Days;
      break;
    case RTF_HOURS_FIELD:
      newSelection=RTF_Hours;
      break;
    case RTF_MINUTES_FIELD:
      newSelection=RTF_Minutes;
      break;
    default: 
      return 0;
    }
    if (RTF_highlight(&RestForm,&currentSelection,newSelection)) flip=true;
    break;

  case RESTTIME_key:
    ASSERT(chooseActive);
    switch(P1)
    {
    /*
    case KC_TAB:
      newSelection=RestForm.Tab(currentSelection);
      if (RTF_highlight(&RestForm,&currentSelection,newSelection)) redraw=true;
      result|=2;
      break;
    */
    case KC_PLUS:
    case KC_UP:
      if (RTF_IncrStat(currentSelection,pData))redraw=true;
      result|=2;
      break;
    case KC_MINUS:
    case KC_DOWN:
      if (RTF_DecrStat(currentSelection,pData)) redraw=true;
      result|=2;
      break;
    };
    break;

  default:
    ASSERT(0);
    return 0;  // Internal error
  }

  if (redraw)
  { 
    int junk=-1;
    showRestTime(&RestForm, pData);
    RTF_highlight(&RestForm,&junk,currentSelection);
    flip=true;
  }

  if (flip)
  {
    GraphicsMgr.ClearAdventureBackground(BACKBUFFERNUM);
    drawBorder();
    drawViewport(TRUE,TRUE,TRUE,TRUE);
    blitView(SmallPicDib | SpriteDib, TRUE, &currPic);
    displayPartyNames(party, false, true, NULL, 0);
    RestForm.Display(0);
    menu.DisplayMenu(0);
    DisplayTextAndFlip(0);    
  };
  return result|(flip?1:0);
}
