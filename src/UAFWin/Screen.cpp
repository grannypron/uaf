/******************************************************************************
* Filename: Screen.cpp
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

#include "..\shared\stdafx.h"


#include "GlobalData.h"
#include "SoundMgr.h"
#include "Graphics.h"
#include "Viewport.h"
#include "GameEvent.h"
#include "Disptext.h"
#include "CharStatsForm.h"
#include "GameMenu.h"
#include "path.h"
#include "DrawTile.h"
#include "party.h"
#include "FormattedText.h"
#include "..\Shared\GPDLopCodes.h"
#include "GPDLexec.h"
#include "combatants.h"
#include "MouseThread.h"

#include "Screen.h" 
#include "GPDLcomp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#ifdef TRACE_TIMER_DETAILS
class DETAIL_TRACE
{
  FILE *f;
public:
  DETAIL_TRACE(void);
  ~DETAIL_TRACE(void);
//  DWORD    m_id[1000];
//  DWORD    m_data[1000];
//  DWORD    m_time[1000];
//  DWORD    m_virtualTime[1000];
//  int      m_nextIndex;
  void     trace (DWORD id, DWORD data);
};
extern DETAIL_TRACE detailTrace;
#endif

extern CString frontEnd;
extern CString backEnd;
extern const char *SAentries[2];
#ifdef UAFEngine
extern A_CStringPAIR_L globalSA_debug;
#endif


#ifdef TRACE_TIMER_DETAILS
#define DETAILTRACE(a,b) detailTrace.trace((a),(b));
#else
#define DETAILTRACE(a,b)
#endif

int specialGraphicsActive = 0;

class MONITORACTIVEFLAG
{
public:
  MONITORACTIVEFLAG(void) { specialGraphicsActive++; };
  ~MONITORACTIVEFLAG(void)  {specialGraphicsActive--; }; 
};

void RunSpecialGraphics(char selection)
{
  HOOK_PARAMETERS hookParameters;
  SCRIPT_CONTEXT scriptContext;
  SPECABILITY_DEF  *pSpecAb;
  const ASLENTRY *pSpecString;
  const CString *pKey;
  MONITORACTIVEFLAG maf;  // Make sure specialGraphicsActive is 
                          // decremented unconditionally when we exit.
  if (specialGraphicsActive != 1) return;

  {
    // 10000002
    // 40000004
    // 80000057
  };


  // First we find the Global Special Ability named 'SpecialGraphics'
  pSpecAb = specialAbilitiesData.FindAbility(SPECIAL_GRAPHICS);
  if (pSpecAb == NULL) return;
  //Now we search th Special Ability for any scrips with names containing
  //the 'selection' character before a '$'.
  POSITION pos;
  pos = pSpecAb->GetStartPosition();
  while (pos != NULL)
  {
    int i, len;
    pSpecAb->GetNextEntry(pos, &pSpecString);
    pKey = pSpecString->pKey();
    len = pKey->GetLength();
    for (i=0; i<len; i++)
    {
      char c;
      c = pKey->GetAt(i);
      if (c == selection) break;
      if (c == '$') {i=len; break;};
    };
    if (i == len) continue;  // No match in Script name.

//  HOOK_PARAMETERS hookParameters;
    CStringPAIR  saAbility[MAX_SPEC_AB];
    CString binScript;
//  const CStringPAIR *pAbility = NULL;


    if (pSpecString->Flags() == SPECAB_SCRIPT)
    {
      GPDLCOMP gpdlcomp;
      binScript = gpdlcomp.CompileScript(frontEnd+pSpecString->Value()+backEnd, SAentries);
      if (binScript[0] != 0)
      {
        ASLENTRY *pEnt;
        pEnt = const_cast<ASLENTRY *>(pSpecString);
        //WriteDebugString("* * * * Script Error in %s[%s]\n", pSpecAb->Name(),scriptName);
        WriteDebugString("* * * * Script Error in %s[%s]\n", pSpecAb->Name(), *pKey);
        //pSpecString->Value(binScript);
        pEnt->Value(binScript);
        //pSpecString->Flags(SPECAB_SCRIPTERROR);
        pEnt->Flags(SPECAB_SCRIPTERROR);
        //pSpecAb->Insert(scriptName, binScript, SPECAB_SCRIPTERROR);
        pSpecAb->Insert(*pKey, binScript, SPECAB_SCRIPTERROR);
      }
      else
      {
        ASLENTRY *pEnt;
        pEnt = const_cast<ASLENTRY *>(pSpecString);
        //pSpecString->Value(binScript);
        pEnt->Value(binScript);
        //pSpecString->Flags(SPECAB_BINARYCODE);
        pEnt->Flags(SPECAB_BINARYCODE);
        //pSpecAb->Insert(scriptName, binScript, SPECAB_BINARYCODE);
        pSpecAb->Insert(*pKey, binScript, SPECAB_BINARYCODE);
      };
    };
    if (pSpecString->Flags() == SPECAB_BINARYCODE)
    {
      CStringPAIR origSA;
      SPECIAL_ABILITIES *pOrigContext;
      gpdlStack.Push();
      //if (nullSA)
      {
        //CStringPAIR SAcontext(SAname, "");
        CStringPAIR SAcontext(SPECIAL_GRAPHICS, "");
        origSA = pScriptContext->specAb;
        pOrigContext = pScriptContext->context;
        pScriptContext->SetAbility(NULL, &SAcontext);
        //pScriptContext->SetSA_ScriptName(scriptName);
        pScriptContext->SetSA_ScriptName(*pKey);
      };
      hookParameters[0].Empty();
      hookParameters[1] = selection;
      hookParameters[2].Empty();
      hookParameters[3].Empty();
      hookParameters[4].Empty();
      hookParameters[5].Empty();
      hookParameters[6].Empty();
      hookParameters[7].Empty();
      hookParameters[8].Empty();
      hookParameters[9].Empty();
      hookParameters[0] =  gpdlStack.activeGPDL()->ExecuteScript(pSpecString->Value(),1, NULL, 0); 
      //(*p_hook_parameters)[0] = gpdlStack.activeGPDL()->ExecuteScript(pSpecString->Value(),1, NULL, 0); 
      //hookParameters[0] = gpdlStack[++activeGPDL].ExecuteScript(pSpecString->Value(),1, NULL, 0); 
      gpdlStack.Pop();
      //if (nullSA)
      {
        pScriptContext->specAb = origSA;
      };
      //callbackResult = ScriptCallback_RunAllScripts(CBF_EXAMINESCRIPT, &(*p_hook_parameters)[0], NULL);
      if (globalSA_debug.Find(SPECIAL_GRAPHICS) != NULL)
      {
        //WriteDebugString("@@SA \"%s\" Script \"%s\": %s%s\n", SAname, scriptName, NULL, NULL); 
        WriteDebugString("@@SA \"%s\" Script \"%s\": %s%s\n", SPECIAL_GRAPHICS, *pKey, NULL, NULL); 
      };
  
      if (!hookParameters[3].IsEmpty())
      {
        CString fname;
        long l;
        RECT srcRect, dstRect;
  
        srcRect.left     = atoi(hookParameters[6]);
        srcRect.top      = atoi(hookParameters[7]);
        srcRect.right    = atoi(hookParameters[8]) + srcRect.left;
        srcRect.bottom   = atoi(hookParameters[9]) + srcRect.top;;
  
        dstRect.left     = atoi(hookParameters[4]);
        dstRect.top      = atoi(hookParameters[5]);
        dstRect.right    = dstRect.left + srcRect.right - srcRect.left;
        dstRect.bottom   = dstRect.top + srcRect.bottom - srcRect.top;
  
        fname = hookParameters[3];
        if (hookParameters[2].IsEmpty())
        {
          // Opaque
          l = GraphicsMgr.AddFileSurface(fname, SpecialGraphicsOpaqueDib, rte.PicArtDir());
          GraphicsMgr.BlitImage(BACKBUFFERNUM, l, &srcRect, &dstRect);
        }
        else
        {
          // Transparent
          l = GraphicsMgr.AddFileSurface(fname, SpecialGraphicsTransparentDib, rte.PicArtDir());
          GraphicsMgr.BlitTransImage(BACKBUFFERNUM, l, &srcRect, &dstRect);
        };
        GraphicsMgr.ReleaseSurface(l);
      };
    };
  };
}


























const int IconPickX = 100;
const int IconPickY = 100;

// Decides what to draw in viewport
// If currPic has something to draw it will be done
// draw viewport frame
// draw viewport contents if needed (no pic? then walls or map)
// draw currPic data
void blitView(DWORD flags, BOOL drawViewportPic, PIC_DATA *pPic)
{
  //if ((globalData.BackgroundSurf >= 0) && (currPic.key > 0))
  //{
  //  // so that pic area is clear of background image
  //  RECT rect = { 0, 0, SCREEN_WIDTH, HorzBarMidY };
  //  GraphicsMgr.ClearSurface(BACKBUFFERNUM, BACKGROUND_FILL, &rect, FALSE);
  //}
  drawBorder();
  
  if ((currPic.key > 0) && (currPic.picType == BigPicDib) && (flags & BigPicDib))
    drawViewport(FALSE, FALSE, drawViewportPic, TRUE);
  else
    drawViewport(TRUE, TRUE, drawViewportPic, TRUE);
  
  if (drawViewportPic)
  {
    //if (currPic.key > 0)
    if (pPic->key > 0)
    {
      //if ((currPic.picType == BigPicDib) && (flags & BigPicDib))
      if ((pPic->picType == BigPicDib) && (flags & BigPicDib))
      {
        //GraphicsMgr.BlitSprite(currPic.sx, currPic.sy, currPic.key);
        GraphicsMgr.BlitSprite(pPic->sx, pPic->sy, pPic->key);
      }
      //else if ((currPic.picType == SpriteDib) && (flags & SpriteDib))
      else if ((pPic->picType == SpriteDib) && (flags & SpriteDib))
      {
        updateViewport(party.Posx, party.Posy, party.facing, BACKBUFFERNUM, TRUE);
        //GraphicsMgr.BlitSprite(currPic.sx, currPic.sy, currPic.key);
        GraphicsMgr.BlitSprite(pPic->sx, pPic->sy, pPic->key);
      }
      //else if ((currPic.picType == SmallPicDib) && (flags & SmallPicDib))
      else if ((pPic->picType == SmallPicDib) && (flags & SmallPicDib))
      {
        updateViewport(party.Posx, party.Posy, party.facing, BACKBUFFERNUM, FALSE);
        //GraphicsMgr.BlitSprite(currPic.sx, currPic.sy, currPic.key);
        GraphicsMgr.BlitSprite(pPic->sx, pPic->sy, pPic->key);
      }
      else
        updateViewport(party.Posx, party.Posy, party.facing, BACKBUFFERNUM, FALSE);
    }
    else
      updateViewport(party.Posx, party.Posy, party.facing, BACKBUFFERNUM, FALSE);
  };
}

void DisplayTextAndFlip(int fontNumber)
{  
  TEXT_DISPLAY_DATA *pText;

  if (errorTextData.numLines > 0)
    pText = &errorTextData;
  else
    pText = &textData;

  if (pText->NeedsFrontBuffer())
  {
    GraphicsMgr.FlipSurfaces();
    DisplayFormattedText(*pText, whiteColor, fontNumber);
  }
  else
  {
    DisplayFormattedText(*pText, whiteColor, fontNumber);
    GraphicsMgr.FlipSurfaces();
  }
}

void UpdateBigSmallSprite(BOOL drawViewportPic, 
                          GameEvent * /*pEvent*/, 
                          char specialGraphicCircumstance)
{
  GraphicsMgr.ClearAdventureBackground(BACKBUFFERNUM);  
  blitView(SmallPicDib | BigPicDib | SpriteDib, drawViewportPic, &currPic);
  menu.DisplayMenu(0);
  displayPartyNames(party, false, true, NULL, 0);
  if (specialGraphicCircumstance != 0) RunSpecialGraphics(specialGraphicCircumstance);
  DisplayTextAndFlip(0);
}

void UpdateSmall(char specialGraphicsCircumstance)
{
  GraphicsMgr.ClearAdventureBackground(BACKBUFFERNUM);
  blitView(SmallPicDib, TRUE, &currPic);
  displayPartyNames(party, false, true, NULL, 0);
  menu.DisplayMenu(0);
  if (specialGraphicsCircumstance != 0) RunSpecialGraphics(specialGraphicsCircumstance);
  DisplayTextAndFlip(0);
}

void UpdateBig(void)
{
  GraphicsMgr.ClearAdventureBackground(BACKBUFFERNUM);
  blitView(BigPicDib, TRUE, &currPic);
  menu.DisplayMenu(0);
  DisplayTextAndFlip(0);
}

void UpdateSmallSprite(char specialGraphicsCircumstance)
{
  GraphicsMgr.ClearAdventureBackground(BACKBUFFERNUM);
  blitView(SmallPicDib | SpriteDib, TRUE, &currPic);
  displayPartyNames(party, false, true, NULL, 0);
  menu.DisplayMenu(0);
  if (specialGraphicsCircumstance != 0) RunSpecialGraphics(specialGraphicsCircumstance);
  DisplayTextAndFlip(0);
}

void DisplayCombatBackground(void)
{
  if (globalData.BackgroundSurf >= 0)
  {
    die("BackgroundSurf >= 0");
  }
  else
  {
    if (globalData.BackgroundArt != "")
    {
      globalData.BackgroundSurf = GraphicsMgr.AddFileSurface(globalData.BackgroundArt, TransBufferDib, rte.BackgroundArtDir());
      if (globalData.BackgroundSurf < 0)
      {
        if (!debugStrings.AlreadyNoted(CString("FTLBS02")+globalData.BackgroundArt))
        {
          debugSeverity = 5;
          WriteDebugString("Failed to load background art: %s\n", globalData.BackgroundArt);
        };
      }
      else
      {
        GraphicsMgr.ClearAdventureBackground(BACKBUFFERNUM);
        GraphicsMgr.ReleaseSurface(globalData.BackgroundSurf);
      };
    };
  };
}

void DisplayCombatData(BOOL ShowCursor=TRUE, BOOL FullIcon=TRUE)
{
  GraphicsMgr.ClearSurface(BACKBUFFERNUM, BACKGROUND_COMBAT_FILL, NULL, TRUE); 
  DisplayCombatBackground();



  displayCombatWalls();    
  DisplayAnimateList();
  combatData.DisplayCombatants();
  combatData.DisplayAURAs();
  if (ShowCursor)
    combatData.displayCursor(FullIcon);
  displayCombatBorders();
}

void DisplayCombatDataWithSprite(BOOL ShowCursor=TRUE, BOOL FullIcon=TRUE)
{
  GraphicsMgr.ClearSurface(BACKBUFFERNUM, BACKGROUND_COMBAT_FILL); 
   DisplayCombatBackground();
  displayCombatWalls();    
  DisplayAnimateList();
  combatData.DisplayCombatants();
  combatData.DisplayAURAs();
  if (ShowCursor) 
    combatData.displayCursor(FullIcon);
  combatSprite.Display();
  displayCombatBorders();
}

///////////////////////////////////////
// Load and Display Credits Screen
///////////////////////////////////////
BOOL Credits()
{  
  WriteDebugString("Loading credits screen and music\n");
  //return GraphicsMgr.FadeInImage(globalData.CreditsBgArt, TitleDib, rte.CreditsArtDir());
  return true;
}

void UpdateStartScreen(const SELECTION_PAGE_FORMAT *format, int fontNumber)
{
  int offsetX, offsetY;
  if ((format != NULL) && (format->Menu_x < 0))
  {
    format = NULL;
  };
  GraphicsMgr.ClearAdventureBackground(BACKBUFFERNUM);
  drawBorder();
  displayPartyNames(party, true, true, format, 0);
  menu.DisplayMenu(0);

  // format version as text
  CString DesignVersionInfo;
  if ((format == NULL) || (format->Title_x <= 0))
  {
    DesignVersionInfo.Format("%s  ver %01.6f",
      globalData.designName,
      ((float)globalData.version));
      // determine coords for top/center of screen
      if ((dsgnVerX >= 0) && (dsgnVerY >= 0))
      {
        offsetX = dsgnVerX;
        offsetY = dsgnVerY;
      }
      else
      {
        offsetX = (SCREEN_WIDTH / 2) -
          (GraphicsMgr.GetTextWidth(DesignVersionInfo, DesignVersionInfo.GetLength()) / 2) - 1;
        offsetY = -1; //DLD 12/20/00 change due to new art layout //offsetY = 1;
      };
    // blit to primary surface
// prs 20191221    displayText(offsetX, offsetY, DesignVersionInfo, White, FALSE, FALSE, TRUE);
    DisplayText(offsetX, offsetY, 
                fontNumber, 
                DesignVersionInfo, 
                whiteColor, FALSE,
                FALSE, FALSE, TRUE);
  }
  else
  {
    DisplayColoredText(format->Title_x,
                       format->Title_y,
                       fontNumber,
                       format->Title_Format,
                      // prs 20191221
                       whiteColor, FALSE,
                       FALSE, FALSE, TRUE);
  };
  DisplayTextAndFlip(0);
}

void UpdateIconPickScreen(void)
{
  int offsetX=0, offsetY=0;
  int width, height, bits, temp;
  //RECT rect;

  if (GraphicsMgr.GetSpriteSurfaceStats(character.icon.key, width, height, bits))
  {  
    temp = width / COMBAT_TILE_WIDTH;
    temp /= (character.icon.NumFrames/2);  // Number of images
    if (temp > 4)
      offsetX = 0;
    else if (temp == 4)
      offsetX = COMBAT_TILE_WIDTH;
    else if (temp == 2)
      offsetX = 2*COMBAT_TILE_WIDTH;
    else 
    {
      offsetX = 0;
      WriteDebugString("Invalid icon width for GS_IconPick in UpdateIconPickScreen\n");
    };
    temp = height / COMBAT_TILE_HEIGHT;
    if (temp == 3)
       offsetY = 0;
    else if (temp == 2)
       offsetY = COMBAT_TILE_HEIGHT;
    else if (temp == 1)
       offsetY = 2*COMBAT_TILE_HEIGHT;
    else 
    {
       WriteDebugString("Invalid icon height for GS_IconPick in UpdateIconPickScreen\n");
       offsetY = 0;
    }
  }

  GraphicsMgr.ClearAdventureBackground(BACKBUFFERNUM);

  drawBorder();
  if (SpriteBkGndArt > 0)
     GraphicsMgr.BlitImage(IconPickX, IconPickY, SpriteBkGndArt, NULL);

  if (character.icon.key >= 0)
  {
    character.icon.SetFrame(0);
    GraphicsMgr.BlitSprite( IconPickX+offsetX, 
                            IconPickY+offsetY, 
                            character.icon.key);  
    character.icon.SetFrame(1);
    offsetX +=  (COMBAT_TILE_WIDTH + (width/2));
    GraphicsMgr.BlitSprite( IconPickX+offsetX, 
                            IconPickY+offsetY, 
                            character.icon.key);  
  };
  menu.DisplayMenu(0);
  DisplayTextAndFlip(0);
}

void UpdateSmallPicScreen(void)
{
  GraphicsMgr.ClearAdventureBackground(BACKBUFFERNUM);
  drawBorder();
  if (character.smallPic.key > 0)
     GraphicsMgr.BlitSprite(IconPickX, IconPickY, character.smallPic.key);
  menu.DisplayMenu(0);
  DisplayTextAndFlip(0);
}
void UpdateAdventureScreen(void)
{
  DETAILTRACE(0x4c3321,0);
  GraphicsMgr.ClearAdventureBackground(BACKBUFFERNUM);
  DETAILTRACE(0x4c3322,0);
  drawBorder();
  DETAILTRACE(0x4c3323,0);
  drawViewport(TRUE, TRUE, TRUE, TRUE);
  DETAILTRACE(0x4c3324,0);
  updateViewport(party.Posx, party.Posy, party.facing, BACKBUFFERNUM, FALSE);
  DETAILTRACE(0x4c3325,0);
  displayPartyNames(party, false, true, NULL, 0);         

  DETAILTRACE(0x4c3326,0);
  menu.DisplayMenu(0);
  DETAILTRACE(0x4c3327,0);

  RunSpecialGraphics('A');
  DisplayTextAndFlip(0);
}         

void UpdateTextEventScreen(GameEvent *pEvent, char specialGraphicCircumstance)
{
  UpdateBigSmallSprite(TRUE, pEvent, specialGraphicCircumstance);
}

void UpdateQuestionYesNoScreen(char specialGraphicsCircumstance)
{
  UpdateBigSmallSprite(TRUE, NULL, specialGraphicsCircumstance);
}

void UpdateQuestionListScreen(char specialGraphicsCircumstance)
{
  UpdateBigSmallSprite(TRUE, NULL, specialGraphicsCircumstance);
}

void UpdateQuestionButtonScreen(char specialGraphicsCircumstance)
{
  UpdateBigSmallSprite(TRUE, NULL, specialGraphicsCircumstance);
}

void UpdateVaultScreen(char specialGraphicsCircumstance)
{
  UpdateSmallSprite(specialGraphicsCircumstance);
}

void UpdateGiveTreasureScreen(char specialGraphicsCircumstance)
{
  UpdateSmallSprite(specialGraphicsCircumstance);
}

void UpdateViewCharacterScreen(void)
{
  GraphicsMgr.ClearAdventureBackground(BACKBUFFERNUM);
  drawBorder();
  drawViewportFrame(VPFrameXOffset, VPFrameY+VPFrameYOffset, &CharViewFrameVPArt);
  if (currPic.key > 0)
  {
    if (currPic.picType == SmallPicDib)
       GraphicsMgr.BlitSprite(VPFrameXOffset+(ViewportX-VPFrameX), VPFrameY+VPFrameYOffset+(ViewportY-VPFrameY), currPic.key);
  }
  menu.DisplayMenu(0);
  //if (errorTextData.numLines == 0)
  displayCharStats(party.getActiveChar());
  DisplayTextAndFlip(0);
}

void UpdateCombatViewCharacterScreen(void)
{  
  GraphicsMgr.ClearAdventureBackground(BACKBUFFERNUM);
  drawBorder();
  drawViewportFrame(VPFrameXOffset, VPFrameY+VPFrameYOffset, &CharViewFrameVPArt);
  if (currPic.key > 0)
  {
    if (currPic.picType == SmallPicDib)
       GraphicsMgr.BlitSprite(VPFrameXOffset+(ViewportX-VPFrameX), VPFrameY+VPFrameYOffset+(ViewportY-VPFrameY), currPic.key);
  }
  menu.DisplayMenu(0);
  if (errorTextData.numLines == 0)
    displayCharStats(*combatData.getActiveCombatCharacter().m_pCharacter);
  DisplayTextAndFlip(0);
}

void UpdateEncampScreen(char specialGraphicsCircumstance)
{
  UpdateSmallSprite(specialGraphicsCircumstance);
}

void UpdateTransferMenuScreen(char specialGraphicsCircumstance)
{
  UpdateBigSmallSprite(TRUE, NULL, specialGraphicsCircumstance);
}
    
void UpdateTransferMsgScreen(void)
{
  GraphicsMgr.ClearAdventureBackground(BACKBUFFERNUM);
  drawBorder();
  drawViewport(TRUE, TRUE, TRUE, TRUE);
  updateViewport(party.Posx, party.Posy, party.facing, BACKBUFFERNUM, FALSE);
  displayPartyNames(party, false, true, NULL, 0);
  menu.DisplayMenu(0);
  DisplayTextAndFlip(0);
}

void UpdateTavernMenuScreen(char specialGraphicsCircumstance)
{
  UpdateSmallSprite(specialGraphicsCircumstance);
}

void UpdateGiveDamageScreen(char specialGraphicsCircumstance)
{
  UpdateBigSmallSprite(TRUE, NULL, specialGraphicsCircumstance);
}

void UpdateGainExpScreen(char specialGraphicsCircumstance)
{
  UpdateBigSmallSprite(TRUE, NULL, specialGraphicsCircumstance);
}

void UpdateHealPartyScreen(char specialGraphicsCircumstance)
{
  UpdateBigSmallSprite(TRUE, NULL, specialGraphicsCircumstance);
}

void UpdateWhoPaysScreen(char specialGraphicsCircumstance)
{
  UpdateSmall(specialGraphicsCircumstance);
}

void UpdatePassTimeScreen(char specialGraphicsCircumstance)
{
  UpdateSmall(specialGraphicsCircumstance);
}

void UpdateSmallTownScreen(char specialGraphicsCircumstance)
{
  UpdateSmall(specialGraphicsCircumstance);
}
void UpdateRandomEventScreen(char specialGraphicsCircumstance)
{
  UpdateSmall(specialGraphicsCircumstance);
}
void UpdateAddNPCScreen(char specialGraphicsCircumstance)
{
  UpdateBigSmallSprite(TRUE, NULL, specialGraphicsCircumstance);
}

void UpdateRemoveNPCScreen(char specialGraphicsCircumstance)
{
  UpdateBigSmallSprite(TRUE, NULL, specialGraphicsCircumstance);
}

void UpdateNPCSaysScreen(char specialGraphicsCircumstance)
{
  UpdateBigSmallSprite(TRUE, NULL, specialGraphicsCircumstance);
}

void UpdateShopScreen(char specialGraphicsCircumstance)
{
  UpdateSmallSprite(specialGraphicsCircumstance);
}

void UpdateSpecialItemKeyScreen(char specialGraphicsCircumstance)
{
  UpdateSmall(specialGraphicsCircumstance);
}


void UpdateGPDLScreen(GPDL *pGPDL, PIC_DATA *pPic)
{
  GPDL_STATE dscState;
  dscState=pGPDL->State();
  if (dscState!=GPDL_UNINITIALIZED) 
  {
    GraphicsMgr.ClearAdventureBackground(BACKBUFFERNUM);
    blitView(SmallPicDib | BigPicDib | SpriteDib, TRUE, pPic);
    displayPartyNames(party, false, true, NULL, 0);
    menu.DisplayMenu(0);
       
    if (errorTextData.numLines == 0)
    {
    // DisplayFormattedText(textData, White);
      int x = textboxX;
      int y = textboxY;
      //discourse.Display();
      /*  PRS 20200208
      GraphicsMgr.ResetFontColorTags();
      GraphicsMgr.EnableFontColorTags(TRUE);
      */
      for (int i=0; i<pDiscourseLine->discourseLine.GetSize(); i++)
      {
        /*  PRS 20200208
          GraphicsMgr.ResetFontColorTags();
          */
// prs 20191221        displayText(x,y+18*i, pDiscourseLine->discourseLine[i], White, FALSE, FALSE, TRUE);
        DisplayText(x, y + 18 * i, 
                    0, 
                    pDiscourseLine->discourseLine[i], 
                    whiteColor, FALSE,
                    FALSE, FALSE, TRUE);
      };
      RunSpecialGraphics('m');
      GraphicsMgr.FlipSurfaces();
      /*  PRS 20200208
      GraphicsMgr.EnableFontColorTags(FALSE);
      */
    }
    else
      RunSpecialGraphics('m');
      DisplayTextAndFlip(0);    
  }; // if (!GPDL_UNINITIALIZED)
}

void UpdateTakePartyItemsScreen(char specialGraphicsCircumstance)
{
  UpdateBigSmallSprite(TRUE, NULL, specialGraphicsCircumstance);
}

void UpdateTrainHallWelcomeScreen(char specialGraphicsCircumstance)
{
  UpdateSmall(specialGraphicsCircumstance);
}

void UpdateWhoTriesScreen(char specialGraphicsCircumstance)
{
  UpdateSmall(specialGraphicsCircumstance);
}

void UpdateTempleScreen(char specialGraphicsCircumstance)
{
  UpdateSmallSprite(specialGraphicsCircumstance);
}

void UpdateTempleGiveScreen(void)
{
  GraphicsMgr.ClearAdventureBackground(BACKBUFFERNUM);
  blitView(SmallPicDib | SpriteDib, TRUE, &currPic);
  displayPartyNames(party,false, true, NULL, 0);
  menu.DisplayMenu(0);
  if (errorTextData.numLines == 0) displayPoolGoldTotal(0);
  DisplayTextAndFlip(0);
}

void UpdatePasswordScreen(char specialGraphicsCircumstance)
{
  UpdateBigSmallSprite(TRUE, NULL, specialGraphicsCircumstance);
}

void UpdateEncounterScreen(char specialGraphicsCircumstance)
{
  UpdateBigSmallSprite(TRUE, NULL, specialGraphicsCircumstance);
}

void UpdateQuestScreen(char specialGraphicsCircumstance)
{
  UpdateBigSmallSprite(TRUE, NULL, specialGraphicsCircumstance);
}

void UpdateGuidedTourScreen(char specialGraphicCircumstance)
{
  UpdateBigSmallSprite(TRUE, NULL, specialGraphicCircumstance);
}

void UpdateGuidedTourMoveScreen(char specialGraphicCircumstance)
{
  GraphicsMgr.ClearAdventureBackground(BACKBUFFERNUM);
  drawBorder();
  drawViewport(TRUE, TRUE, TRUE, TRUE);
  updateViewport(party.Posx, party.Posy, party.facing, BACKBUFFERNUM, FALSE);
  displayPartyNames(party, false, true, NULL, 0);
  RunSpecialGraphics(specialGraphicCircumstance);
  DisplayTextAndFlip(0);
}

void UpdateSpeedCombatScreen(void)
{
  DisplayCombatData();
  menu.DisplayMenu(0);
  displayCombatStats(getCombatantInCell(combatData.m_iCursorX, combatData.m_iCursorY, 1, 1), 0);
  DisplayTextAndFlip(0);  
}

void UpdateSpeedScreen(char specialGraphicsCircumstance)
{
  UpdateBigSmallSprite(TRUE, NULL, specialGraphicsCircumstance);
}

void UpdateCombatRoundDelayScreen(void)
{
  DisplayCombatData();
  DisplayTextAndFlip(0);
}

void UpdateCombatTimeDelayMsg(int dude)
{
  DETAILTRACE(0x2ac700,dude);
  DisplayCombatDataWithSprite();
  menu.DisplayMenu(0);
  if (dude < 0) dude = combatData.GetCurrCombatant();
  displayCombatStats(dude, 0);
  DisplayTextAndFlip(0);
}

void UpdateSpriteAnimateScreen(int dude)
{
  DETAILTRACE(0x2ac701,dude);
  DisplayCombatDataWithSprite(FALSE); // no cursor
  menu.DisplayMenu(0);
  displayCombatStats(dude, 0);
  DisplayTextAndFlip(0);
}

void UpdateCombatMoveScreen(void)
{
  DisplayCombatData(FALSE); // no cursor
  menu.DisplayMenu(0);
  displayCombatStats(combatData.GetCurrCombatant(), 0);
  DisplayTextAndFlip(0);
}

void UpdateCombatAimManualScreen(void)
{
  DisplayCombatData(TRUE,FALSE);
  menu.DisplayMenu(0);
  
  int dude = getCombatantInCell(combatData.m_iCursorX, combatData.m_iCursorY, 1,1);
  if (dude == NO_DUDE)
    dude = getDeadCombatantInCell(combatData.m_iCursorX, combatData.m_iCursorY, 1,1);

  displayCombatStats(dude, 0);
  
#ifdef _DEBUG
  displayCursorStats(0);
#endif
  DisplayTextAndFlip(0);
}

void UpdateCombatAimScreen(void)
{
  DisplayCombatData();
  menu.DisplayMenu(0);
  displayCombatStats(getCombatantInCell(combatData.m_iCursorX, combatData.m_iCursorY, 1, 1), 0);
  DisplayTextAndFlip(0);
}

void UpdateCombatMainMenuScreen(void)
{
  DETAILTRACE(0x2ab080,0);
  DisplayCombatData();
  DETAILTRACE(0x2ab081,0);
  menu.DisplayMenu(0);
  DETAILTRACE(0x2ab082,0);
  displayCombatStats(getCombatantInCell(combatData.m_iCursorX, combatData.m_iCursorY, 1, 1), 0);  
  DETAILTRACE(0x2ab083,0);
  DisplayTextAndFlip(0);
  DETAILTRACE(0x2ab084,0);
}
/*
void UpdateHumanChangeClassScreen(void)
{
  UpdateBigSmallSprite(FALSE);
}
*/
void UpdateTradeTakerSelectScreen(char specialGraphicsCircumstance)
{
  UpdateBigSmallSprite(TRUE, NULL, specialGraphicsCircumstance);
}

void UpdateCombatTradeTakerSelectScreen(void)
{
  GraphicsMgr.ClearAdventureBackground(BACKBUFFERNUM);
  drawBorder();
  drawViewport(TRUE, TRUE, TRUE, TRUE);

  if (currPic.key > 0)
  {
    if (currPic.picType == SmallPicDib)
       GraphicsMgr.BlitSprite(ViewportX, ViewportY, currPic.key);
  }
  displayPartyNames(party, false, true, NULL, 0);
  menu.DisplayMenu(0);
  DisplayTextAndFlip(0);
}

void UpdateAlterGameScreen(char specialGraphicsCircumstance)
{
  UpdateBigSmallSprite(TRUE, NULL, specialGraphicsCircumstance);
}

void UpdateAlterOrderScreen(char specialGraphicsCircumstance)
{
  UpdateBigSmallSprite(TRUE, NULL, specialGraphicsCircumstance);
}

void UpdatePartyDiedScreen(void)
{
  GraphicsMgr.ClearAdventureBackground(BACKBUFFERNUM);
  drawBorder();
  // don't have a picture for this yet
  //blitView(SmallPicDib | BigPicDib | SpriteDib);
  //if (currPic.picType != BigPicDib)
  //   displayPartyNames(party, false, true);
  menu.DisplayMenu(0);
  DisplayTextAndFlip(0);
}

void UpdateCombatResultsScreen(void)
{
  GraphicsMgr.ClearAdventureBackground(BACKBUFFERNUM);
  drawBorder();
  //blitView(SmallPicDib | BigPicDib | SpriteDib);
  //if (currPic.picType != BigPicDib)
  //   displayPartyNames(party, false, true);
  menu.DisplayMenu(0);
  DisplayTextAndFlip(0);
}

void UpdateCombatMagicMenuScreen(void)
{
  DisplayCombatData();
  menu.DisplayMenu(0);
  displayCombatStats(getCombatantInCell(combatData.m_iCursorX, combatData.m_iCursorY, 1, 1), 0);  
  DisplayTextAndFlip(0);
}

void UpdateMagicMenuScreen(char specialGraphicsCircumstance)
{
  UpdateBigSmallSprite(TRUE, NULL, specialGraphicsCircumstance);
}

void UpdateScribeMenuScreen(char specialGraphicsCircumstance)
{
  UpdateBigSmallSprite(TRUE, NULL, specialGraphicsCircumstance);
}
/*
void UpdateCombatDisplaySpellEffectsMenuScreen(void)
{
  GraphicsMgr.ClearAdventureBackground(BACKBUFFERNUM);
  drawBorder();
  displaySpells();
  menu.DisplayMenu();
  DisplayTextAndFlip();
}
*/
/*
void UpdateDisplaySpellEffectsMenuScreen(void)
{
  GraphicsMgr.ClearAdventureBackground(BACKBUFFERNUM);
  drawBorder();
  displaySpells();
  menu.DisplayMenu();
  DisplayTextAndFlip();
}
*/

/*
void UpdateMemorizeVerifyMenuScreen(void)
{
  GraphicsMgr.ClearAdventureBackground(BACKBUFFERNUM);
  drawBorder();
  displaySpells();
  menu.DisplayMenu();
  DisplayTextAndFlip();
}
*/
/*
void UpdateCombatCastMenuScreen(void)
{
  GraphicsMgr.ClearAdventureBackground(BACKBUFFERNUM);
  drawBorder();
  displaySpells();
  menu.DisplayMenu();
  DisplayTextAndFlip();
}
*/
/*
void UpdateCastMenuScreen(void)
{
  GraphicsMgr.ClearAdventureBackground(BACKBUFFERNUM);
  drawBorder();
  displaySpells();
  menu.DisplayMenu();
  DisplayTextAndFlip();
}
*/
/*
void UpdateMemorizeMenuScreen(void)
{
  GraphicsMgr.ClearAdventureBackground(BACKBUFFERNUM);
  drawBorder();
  // draw middle horz bar, but 
  // not viewport and middle ver bar
  drawViewport(FALSE, FALSE);
  displaySpells();
  menu.DisplayMenu();
  DisplayTextAndFlip();
}
*/
void UpdateRestMenuScreen(void)
{

}

void UpdateAppraiseSelectMenuScreen(void)
{
  GraphicsMgr.ClearAdventureBackground(BACKBUFFERNUM);
  drawBorder();
  menu.DisplayMenu(0);
  DisplayTextAndFlip(0);
}

void UpdateAppraiseEvaluateMenuScreen(void)
{
  GraphicsMgr.ClearAdventureBackground(BACKBUFFERNUM);
  drawBorder();
  menu.DisplayMenu(0);
  DisplayTextAndFlip(0);
}

void UpdateAskYesNoMenuScreen(void)
{
  GraphicsMgr.ClearAdventureBackground(BACKBUFFERNUM);
  drawBorder();
  menu.DisplayMenu(0);
  DisplayTextAndFlip(0);
}
/*
// this screen is displayed before a design is loaded, which
// means that the usual art files are not loaded - frames,
// cursors, horz/vert bars, etc.
//
void UpdateDesignSelectMenuScreen(void)
{
  GraphicsMgr.ClearAdventureBackground(BACKBUFFERNUM);
  displaySpells(); // design list built using spell list code
  menu.DisplayMenu();  
  DisplayTextAndFlip();
}
*/
void UpdateTargetSelectNonCombatScreen(char specialGraphicsCircumstance)
{
  UpdateSmall(specialGraphicsCircumstance);
}

void UpdateDisplayPartyJournalScreen(void)
{
  GraphicsMgr.ClearAdventureBackground(BACKBUFFERNUM);
  drawBorder();
  menu.DisplayMenu(0);

  if (journalTextData.NeedsFrontBuffer())
  {
    GraphicsMgr.FlipSurfaces();
    DisplayJournalText(journalTextData, whiteColor, 0);
  }
  else
  {
    DisplayJournalText(journalTextData, whiteColor, 0);
    GraphicsMgr.FlipSurfaces();
  }
}

void UpdateMovieBigPicWindow(void)
{
  GraphicsMgr.ClearAdventureBackground(BACKBUFFERNUM);
  drawBorder();
  drawViewport(FALSE, FALSE, TRUE, TRUE);
  menu.DisplayMenu(0);
  DisplayTextAndFlip(0);
}

void UpdateMovieSmallPicWindow(void)
{
  GraphicsMgr.ClearAdventureBackground(BACKBUFFERNUM);
  drawBorder();
  drawViewport(TRUE, TRUE, TRUE, TRUE);
  displayPartyNames(party, false, true, NULL, 0); // useWX=0, ShowNPC=1
  menu.DisplayMenu(0);
  DisplayTextAndFlip(0);
}

void UpdateRaceMenuScreen(const SELECTION_PAGE_FORMAT& format)
{
  int shopY;
  GraphicsMgr.ClearAdventureBackground(BACKBUFFERNUM);
  drawBorder();

  //int totalListItems = spellListText.GetCount();
  int totalListItems = spellListText.races.GetSize();
  if (totalListItems == 0)
  {
    CString tmp;
    tmp.Format("NO RACES AVAILABLE FOR SELECTION");
// prs 20191221    displayText(18, 18, tmp);
    DisplayText(18, 18, 0, tmp, whiteColor, FALSE);
    menu.DisplayMenu(0);
    DisplayTextAndFlip(0);
    return;
  }

  memset(InventoryRects, 0, sizeof(InventoryRects));
  int ir_idx = 0;

// prs 20191221  displayColoredText(format.Title_x, format.Title_y, spellListText.title, White);
  DisplayColoredText(format.Title_x, format.Title_y, 
                     0, 
                     spellListText.title, 
                     whiteColor, FALSE,
                     FALSE, FALSE, TRUE);
  int currItem = startItem;
  //int shopY = ITEMY;
  shopY = format.Table_y;

// prs 20191221  displayColoredText(format.Header_x, format.Header_y, format.Header_Format, White);
  DisplayColoredText(format.Header_x, format.Header_y, 
                     0, 
                     format.Header_Format, 
                     whiteColor, FALSE,
                     FALSE, FALSE, TRUE);

  while (currItem < totalListItems)
  {
    //raceType rt = (raceType)spellListText.GetSpellIndex(currItem);    
    RACE_ID raceID = spellListText.GetRace(currItem);    
    CString raceName;
    if (shouldHighlight(currItem))
    {
      raceName.Format(format.Table_Selected_Format, raceID.UniqueName());
    }
    else
    {
      raceName.Format(format.Table_Format, raceID.UniqueName());
    };

    //displayText(18, shopY, raceData.GetRaceName(rt), White, shouldHighlight(currItem));
// prs 20191221    displayColoredText(format.Table_x, shopY, raceName, White);
    DisplayColoredText(format.Table_x, shopY, 
                       0,
                       raceName, 
                       whiteColor, FALSE,
                       FALSE, FALSE, TRUE);

    //InventoryRects[ir_idx].left = 18;
    InventoryRects[ir_idx].left = format.Table_x;

    InventoryRects[ir_idx].top = shopY;
    //InventoryRects[ir_idx].right = 18 + GraphicsMgr.GetTextWidth(spellListText.GetSpellText(currItem), strlen(spellListText.GetSpellText(currItem)));
    InventoryRects[ir_idx].right = format.Table_x + GraphicsMgr.GetTextWidth(raceName, strlen(raceName));
    InventoryRects[ir_idx].bottom = shopY + GraphicsMgr.GetMaxCharacterHeight();
    ir_idx++;

    //if (totalListItems > Items_Per_Page)
    if (totalListItems > format.Lines_Per_Page)
    {
      //if (! ((currItem+1) % Items_Per_Page) )
      if (! ((currItem+1) % format.Lines_Per_Page) )
      {
          shopY=ITEMY;
          break;
       }
    }
    currItem++;
    //shopY += 18;
    shopY += format.Line_Spacing;
  }

  menu.DisplayMenu(0);
  DisplayTextAndFlip(0);
}

void UpdateGenderMenuScreen(const SELECTION_PAGE_FORMAT& format)
{
  GraphicsMgr.ClearAdventureBackground(BACKBUFFERNUM);
  drawBorder();

  int totalListItems = spellListText.GetGenderCount();
  if (totalListItems == 0)
  {
    CString tmp;
    tmp.Format("NO GENDERS AVAILABLE FOR SELECTION");
// prs 20191221    displayColoredText(format.Header_x, format.Header_y, tmp);
    DisplayColoredText(format.Header_x, format.Header_y, 
                       0, 
                       tmp, 
                       whiteColor, FALSE,
                       FALSE, FALSE, TRUE);
    menu.DisplayMenu(0);
    DisplayTextAndFlip(0);
    return;
  }

  memset(InventoryRects, 0, sizeof(InventoryRects));
  int ir_idx = 0;

// prs 20191221  displayColoredText(format.Title_x, format.Title_y, format.Title_Format);
  DisplayColoredText(format.Title_x, format.Title_y, 
                     0, 
                     format.Title_Format, 
                     whiteColor, FALSE,
                     FALSE, FALSE, TRUE);
// prs 20191221  displayColoredText(format.Header_x, format.Header_y, format.Header_Format);
  DisplayColoredText(format.Header_x, format.Header_y, 
                     0, 
                     format.Header_Format, 
                     whiteColor, FALSE,
                     FALSE, FALSE, TRUE);
  int currItem = startItem;
  //int shopY = ITEMY;
  int shopY = format.Table_y;

  while (currItem < totalListItems)
  {
    //genderType type = (genderType)spellListText.GetSpellIndex(currItem);    
    genderType type = spellListText.GetGender(currItem);    
    CString genderName;
    if (shouldHighlight(currItem))
    {
      genderName.Format(format.Table_Selected_Format, GetGenderName(type));
    }
    else
    {
      genderName.Format(format.Table_Format, GetGenderName(type));
    }
    //displayText(18, shopY, genderName, White, shouldHighlight(currItem));
// prs 20191221    displayColoredText(format.Table_x, shopY, genderName, White);
    DisplayColoredText(format.Table_x, shopY, 
                       0, 
                       genderName, 
                       whiteColor, FALSE,
                       FALSE, FALSE, TRUE);

    InventoryRects[ir_idx].left = format.Table_x;
    InventoryRects[ir_idx].top = shopY;
    InventoryRects[ir_idx].right = format.Table_x + GraphicsMgr.GetTextWidth(genderName, strlen(genderName));
    InventoryRects[ir_idx].bottom = shopY + GraphicsMgr.GetMaxCharacterHeight();
    ir_idx++;

    if (totalListItems > Items_Per_Page)
    {
       if (! ((currItem+1) % Items_Per_Page) )
       {
          shopY=ITEMY;
          break;
       }
    }
    currItem++;
    shopY += format.Line_Spacing;
  }

  menu.DisplayMenu(0);
  DisplayTextAndFlip(0);
}
void UpdateClassMenuScreen(const SELECTION_PAGE_FORMAT& format)
{
  // some class names can trigger color codes: 'Cleric/Ranger'
  //BOOL CurrFontTagSetting = GraphicsMgr.GetEnableFontColorTags();
  //GraphicsMgr.EnableFontColorTags(FALSE);

  GraphicsMgr.ClearAdventureBackground(BACKBUFFERNUM);
  drawBorder();

  int totalListItems = spellListText.GetClassCount();
  if (totalListItems == 0)
  {
    CString tmp;
    tmp.Format("NO CLASSES AVAILABLE FOR SELECTION");
    //displayText(18, 18, tmp);
// prs 20191221    displayText(format.Title_x, format.Title_y, tmp);
    DisplayText(format.Title_x, format.Title_y, 0, tmp, whiteColor, FALSE);
    menu.DisplayMenu(0);
    DisplayTextAndFlip(0);
    return;
  }

  memset(InventoryRects, 0, sizeof(InventoryRects));
  int ir_idx = 0;

  //displayText(18, 18, spellListText.title);
// prs 20191221  displayColoredText(format.Title_x, format.Title_y, format.Title_Format);
  DisplayColoredText(format.Title_x, format.Title_y, 
                     0, 
                     format.Title_Format, 
                     whiteColor, FALSE,
                     FALSE, FALSE, TRUE);
// prs 20191221  displayColoredText(format.Header_x, format.Header_y, format.Header_Format);
  DisplayColoredText(format.Header_x, format.Header_y, 
                     0, 
                     format.Header_Format, 
                     whiteColor, FALSE,
                     FALSE, FALSE, TRUE);
  int currItem = startItem;
  //int shopY = ITEMY;
  int shopY = format.Table_y;

  while (currItem < totalListItems)
  {
    //classType ct = (classType)spellListText.GetSpellIndex(currItem);        
    CLASS_ID classID = spellListText.GetClass(currItem);        
    CString className;
    //className = classID.UniqueName();
    if (shouldHighlight(currItem))
    {
      className.Format(format.Table_Selected_Format, classID.UniqueName());
    }
    else
    {
      className.Format(format.Table_Format, classID.UniqueName());
    };
    //displayText(18, shopY, className , White);
// prs 20191221    displayColoredText(format.Table_x, shopY, className, White);
    DisplayColoredText(format.Table_x, shopY, 
                       0, 
                       className, 
                       whiteColor, FALSE,
                       FALSE, FALSE, TRUE);

    InventoryRects[ir_idx].left = format.Table_x;
    InventoryRects[ir_idx].top = shopY;
    InventoryRects[ir_idx].right = format.Table_x + GraphicsMgr.GetTextWidth(className, strlen(className));
    InventoryRects[ir_idx].bottom = shopY + GraphicsMgr.GetMaxCharacterHeight();
    ir_idx++;

    if (totalListItems > format.Lines_Per_Page)
    {
       if (! ((currItem+1) % format.Lines_Per_Page) )
       {
          shopY=format.Table_y;
          break;
       }
    }
    currItem++;
    shopY += format.Line_Spacing;
  }

  //GraphicsMgr.EnableFontColorTags(CurrFontTagSetting);

  menu.DisplayMenu(0);
  DisplayTextAndFlip(0);  
}
void UpdateAlignmentMenuScreen(const SELECTION_PAGE_FORMAT& format)
{
  GraphicsMgr.ClearAdventureBackground(BACKBUFFERNUM);
  drawBorder();

  int totalListItems = spellListText.GetAlignmentCount();
  if (totalListItems == 0)
  {
    CString tmp;
    tmp.Format("NO ALIGNMENTS AVAILABLE FOR SELECTION");
// prs 20191221    displayText(format.Title_x, format.Title_y, tmp);
    DisplayText(format.Title_x, format.Title_y, 0, tmp, whiteColor, FALSE);
    menu.DisplayMenu(0);
    DisplayTextAndFlip(0);
    return;
  }

  memset(InventoryRects, 0, sizeof(InventoryRects));
  int ir_idx = 0;

// prs 20191221  displayColoredText(format.Title_x, format.Title_y, format.Title_Format);
  DisplayColoredText(format.Title_x, format.Title_y, 
                     0, 
                     format.Title_Format, 
                     whiteColor, FALSE,
                     FALSE, FALSE, TRUE);
// prs 20191221  displayColoredText(format.Header_x, format.Header_y, format.Header_Format);
  DisplayColoredText(format.Header_x, format.Header_y, 
                     0, 
                     format.Header_Format, 
                     whiteColor, FALSE,
                     FALSE, FALSE, TRUE);
  int currItem = startItem;
  int shopY = format.Table_y;
  int shopX = format.Table_x;
  /*
  enum alignmentType { LawfulGood,    NeutralGood, ChaoticGood, 
                       LawfulNeutral, TrueNeutral, ChaoticNeutral,  
                       LawfulEvil,    NeutralEvil, ChaoticEvil };
  */

  while (currItem < totalListItems)
  {
    //alignmentType type = (alignmentType)spellListText.GetSpellIndex(currItem);    
    alignmentType type = spellListText.GetAlignment(currItem);
    CString alignmentName;
    if (shouldHighlight(currItem))
    {
      alignmentName.Format(format.Table_Selected_Format,GetAlignmentName(type));
    }
    else
    {
      alignmentName.Format(format.Table_Format, GetAlignmentName(type));
    };
// prs 20191221    displayColoredText(shopX, shopY, alignmentName, White);
    DisplayColoredText(shopX, shopY, 
                       0, 
                       alignmentName, 
                       whiteColor, FALSE,
                       FALSE, FALSE, TRUE);

    InventoryRects[ir_idx].left = format.Table_x;
    InventoryRects[ir_idx].top = shopY;
    InventoryRects[ir_idx].right = format.Table_x + GraphicsMgr.GetTextWidth(alignmentName, strlen(alignmentName));
    InventoryRects[ir_idx].bottom = shopY + GraphicsMgr.GetMaxCharacterHeight();
    ir_idx++;

    if (totalListItems > format.Lines_Per_Page)
    {
       if (! ((currItem+1) % format.Lines_Per_Page) )
       {
          shopY=format.Table_y;
          break;
       }
    }
    currItem++;
    shopY += format.Line_Spacing;
  }

  menu.DisplayMenu(0);
  DisplayTextAndFlip(0);
}
