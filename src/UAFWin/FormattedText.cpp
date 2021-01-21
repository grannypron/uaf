/******************************************************************************
* Filename: FormattedText.cpp
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
#include "Party.h"
#include "Viewport.h"
#include "MouseThread.h"
#include "Path.h"
#include "DrawTile.h"

#include "FormattedText.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// holds multi-line text data that will be displayed to 
// the user, with 'HIT RETURN' prompted after each
// box-full is displayed.
//
int TEXTBOX_LINES = 5;                            //DLD 12/20/00
int TEXTBOX_WIDTH = SCREEN_WIDTH - (textboxX*2);
int TEXTBOX_HEIGHT = 5 * 16;

int JOURNAL_TEXTBOX_LINES = 20;
TEXT_DISPLAY_DATA textData;
TEXT_DISPLAY_DATA errorTextData;
TEXT_DISPLAY_DATA pausedTextData;
TEXT_DISPLAY_DATA journalTextData;
TEXT_DISPLAY_DATA combatTextData;

static BOOL ftHighlightOn = FALSE;
static BOOL ftWhiteOn = FALSE;
static BOOL ftYellowOn = FALSE;
static BOOL ftRedOn = FALSE;
static BOOL ftGreenOn = FALSE;
static BOOL ftBlueOn = FALSE;
static BOOL ftBlackOn = FALSE;
static BOOL ftOrangeOn = FALSE;
static BOOL ftCyanOn = FALSE;
static BOOL ftMagentaOn = FALSE;
static BOOL ftBrightOrangeOn = FALSE;
static BOOL ftSilverOn = FALSE;
static BOOL ftCustom1On = FALSE;
static int  ftFontNum = 0;

//Here we decide what to do when text has been
// displayed and either the page is full or 
// the last line of text has been displayed.
// [0]  No more text follows  No global WaitForReturn    Last line has not '/n'
// [1]  No more text follows    No global WaitForReturn    Last line contains '/n'
// [2]  No more text follows    Global WaitForReturn       Last line has no '/n/
// [3]  No more text follows    Global WaitForReturn       Last line has '/n'
// [4]  More text follows  No global WaitForReturn    Last line has not '/n'
// [5]  More text follows    No global WaitForReturn    Last line contains '/n'
// [6]  More text follows    Global WaitForReturn       Last line has no '/n/
// [7]  More text follows    Global WaitForReturn       Last line has '/n'
// 'E' --> End text processing
// 'C' --> Continue to scroll text
// 'W' --> Wait for player input
char MultiBoxTextAction[] = "EWWWCWCW";



//*****************************************************************************
//    NAME: BOOL IsWhiteSpace
//
// PURPOSE: 
//
//*****************************************************************************
static inline BOOL IsWhiteSpace(const char *Char)
{
   if (Char[0] == ' ')  return true;
   if (Char[0] == '\n') return true;
   if (Char[0] == '\r') return true;
   if (Char[0] == '\t') return true;
   if ( (Char[0] == '/') && (Char[1] == 'n') ) return true;
   return false;
}

//*****************************************************************************
//    NAME: BOOL IsNewLine
//
// PURPOSE: 
//
//*****************************************************************************
static inline BOOL IsNewLine(const char *Char)
{
   if ( (Char[0] == '\n') || (Char[0] == '\r') ) return true;
   if ( (Char[0] == '/' ) && (Char[1] == 'n' ) ) return true;
   return false;
}

//*****************************************************************************
//    NAME: ResetFontColorTags
//
// PURPOSE: 
//
//*****************************************************************************
static void ResetFontColorTags(void)
{
  ftWhiteOn = FALSE;
  ftYellowOn = FALSE;
  ftRedOn = FALSE;
  ftGreenOn = FALSE;
  ftBlueOn = FALSE;
  ftBlackOn = FALSE;
  ftOrangeOn = FALSE;
  ftCyanOn = FALSE;
  ftMagentaOn = FALSE;
  ftBrightOrangeOn = FALSE;
  ftSilverOn = FALSE;
  ftCustom1On=FALSE;
  ftHighlightOn = FALSE;
  ftFontNum = 0;
}

//*****************************************************************************
//    NAME: HighlightText
//
// PURPOSE: 
//
//*****************************************************************************
static void HighlightText(BOOL enable)
{
  ResetFontColorTags();
  ftHighlightOn = enable;
}

//*****************************************************************************
//    NAME: WhiteText
//
// PURPOSE: 
//
//*****************************************************************************
static void WhiteText(BOOL enable)
{
  ResetFontColorTags();
  ftWhiteOn = enable;
}
//*****************************************************************************
//    NAME: YellowText
//
// PURPOSE: 
//
//*****************************************************************************
static void YellowText(BOOL enable)
{
  ResetFontColorTags();
  ftYellowOn = enable;
}
//*****************************************************************************
//    NAME: RedText
//
// PURPOSE: 
//
//*****************************************************************************
static void RedText(BOOL enable)
{
  ResetFontColorTags();
  ftRedOn = enable;
}
//*****************************************************************************
//    NAME: GreenText
//
// PURPOSE: 
//
//*****************************************************************************
static void GreenText(BOOL enable)
{
  ResetFontColorTags();
  ftGreenOn = enable;
}
//*****************************************************************************
//    NAME: BlueText
//
// PURPOSE: 
//
//*****************************************************************************
static void BlueText(BOOL enable)
{
  ResetFontColorTags();
  ftBlueOn = enable;
}
static void BlackText(BOOL enable)
{
  ResetFontColorTags();
  ftBlackOn = enable;
}
static void OrangeText(BOOL enable)
{
  ResetFontColorTags();
  ftOrangeOn = enable;
}
static void CyanText(BOOL enable)
{
  ResetFontColorTags();
  ftCyanOn = enable;
}
/*   PRS 20200208
static void MagentaText(BOOL enable)
{
  ResetFontColorTags();
  ftMagentaOn = enable;
}
*/
static void BrightOrangeText(BOOL enable)
{
  ResetFontColorTags();
  ftBrightOrangeOn = enable;
}
static void SilverText(BOOL enable)
{
  ResetFontColorTags();
  ftSilverOn = enable;
}
static void NewTextFont(int n)
{
  ftFontNum = n;
}

//*****************************************************************************
//    NAME: Custom1Text
//
// PURPOSE: 
//
//*****************************************************************************
/*  PRS 20200208  
static void Custom1Text(BOOL enable)
{
  ResetFontColorTags();
  ftCustom1On = enable;
}
*/
//*****************************************************************************
//    NAME: CheckColorTag
//
// PURPOSE: 
//
//*****************************************************************************
/*
static void CheckColorTag(const char **pText)
{
  char color;
  if (*pText==NULL) return;
  color = *((*pText)++);
  if (color != '/') return;
  color = tolower(**pText);
  switch (color)
  {
  case 'h': HighlightText(!ftHighlightOn); break;
  case 'w': WhiteText(!ftWhiteOn); break;
  case 'y': YellowText(!ftYellowOn); break;
  case 'r': RedText(!ftRedOn); break;
  case 'g': GreenText(!ftGreenOn); break;
  case 'b': BlueText(!ftBlueOn); break;
  case 'k': BlackText(!ftBlackOn); break;
  case 'o': OrangeText(!ftOrangeOn); break;
  case 'v': CyanText(!ftCyanOn); break;
  case 'm': MagentaText(!ftMagentaOn); break;
  case 't': BrightOrangeText(!ftBrightOrangeOn); break;
  case 's': SilverText(!ftSilverOn); break;
  case 'c': Custom1Text(!ftCustom1On);break;
  case '1':
  case '2':
  case '3':
  case '4':
  case '5':
  case '6':
  case '7':
  case '8':
  case '9':
  case '0':
    switch ((*pText)[1])
    {
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      NewTextFont(10 * ((*pText)[0] - '0') + (*pText)[1] - '0');
      (*pText)++;
      break;
    default: return;
    };
    break;
  default: return;
  }
  (*pText)++;
}
*/

//*****************************************************************************
//    NAME: ScanForColorTags
//
// PURPOSE: 
//
//*****************************************************************************
/*
static void ScanForColorTags(const char *pText)
{
  while ((pText!=NULL) && (*pText!='\0'))
  {
    CheckColorTag(&pText);
//    pText++;
  }
}
*/
//*****************************************************************************
//    NAME: CheckJournalColorTag
//
// PURPOSE: 
//
//*****************************************************************************
static BOOL CheckJournalColorTag(const char *pText)
{
  if (pText==NULL) return FALSE;  
  //CheckColorTag(pText);
  return (*pText == '\b');
}

//*****************************************************************************
//    NAME: CurrentColor
//
// PURPOSE: 
//
//*****************************************************************************
static CString CurrentColor()
{
  if (ftHighlightOn) return "/h";
  if (ftWhiteOn) return "/w";
  if (ftYellowOn) return "/y";
  if (ftRedOn) return "/r";
  if (ftGreenOn) return "/g";
  if (ftBlueOn) return "/b";
  if (ftCustom1On) return "/c";
  return "";
}

//*****************************************************************************
//    NAME: NeedEndingColorTag
//
// PURPOSE: 
//
//*****************************************************************************
static BOOL NeedEndingColorTag(CString &str, CString &curr)
{
  bool found=false;
  char *pChar = str.GetBuffer(2);
  int i=str.GetLength()-2;
  while ((i>=0)&&(!found))
  {
    if (strncmp(&pChar[i],curr,2)==0)
      found=true;
    else if (pChar[i]!=' ')
      break;
    i--;
  }
  str.ReleaseBuffer();
  return (!found);
}
//*****************************************************************************
//    NAME: NeedStartingColorTag
//
// PURPOSE: 
//
//*****************************************************************************
static BOOL NeedStartingColorTag(CString &str, CString &curr)
{
  bool found=false;
  char *pChar = str.GetBuffer(2);
  int i=0;
  while ((i<str.GetLength()-1)&&(!found))
  {
    if (strncmp(&pChar[i],curr,2)==0)
      found=true;
    else if (pChar[i]!=' ')
      break;
    i++;
  }
  str.ReleaseBuffer();
  return (!found);
}

//*****************************************************************************
//    NAME: AddColorTags
//
// PURPOSE: 
//
//*****************************************************************************
static CString AddColorTags(CString &str)
{
  CString curr = CurrentColor();
  if (curr=="") return "";
  if (str.GetLength()==0) return curr;  
  //if (NeedEndingColorTag(str, curr))
  //  str += curr;
  return curr;
}

//*****************************************************************************
//    NAME: TEXT_DISPLAY_DATA::TEXT_DISPLAY_DATA
//
// PURPOSE: 
//
//*****************************************************************************
TEXT_DISPLAY_DATA::TEXT_DISPLAY_DATA() 
{ 
 highlightAll=FALSE;
 slowText=FALSE;
 InitialBoxDisplay=TRUE;
 numLines=0;
 currLine=0;
 startTime=0;
 pauseText=FALSE;
 text.RemoveAll();
}

//*****************************************************************************
//    NAME: TEXT_DISPLAY_DATA::NeedsFrontBuffer
//
// PURPOSE: 
//
//*****************************************************************************
BOOL TEXT_DISPLAY_DATA::NeedsFrontBuffer()
{
  return ( (UseSlowText()) || (pauseText) );
}

//*****************************************************************************
//    NAME: TEXT_DISPLAY_DATA::UseSlowText
//
// PURPOSE: 
//
//*****************************************************************************
BOOL TEXT_DISPLAY_DATA::UseSlowText()
{
  return ((InitialBoxDisplay) && (slowText));
}


//*****************************************************************************
//    NAME: TEXT_DISPLAY_DATA::IsFirstBox
//
// PURPOSE: 
//
//*****************************************************************************
BOOL TEXT_DISPLAY_DATA::IsFirstBox()
{
  return (currLine == 0);
}


BOOL TEXT_DISPLAY_DATA::WaitForReturn()
{
  int i;
  for (i=0; i<TEXTBOX_LINES; i++)
  {
    if (currLine+i >= numLines) break;
    if (PeekTextLine(currLine+i)->waitForReturn) return true;
  };
  return false;
}
//*****************************************************************************
//    NAME: TEXT_DISPLAY_DATA::IsLastBox
//
// PURPOSE: 
//
//*****************************************************************************
BOOL TEXT_DISPLAY_DATA::IsLastBox()
{
  int i; // Search for the line that terminated the box
  for (i=0; i<TEXTBOX_LINES; i++)
  {
    if (currLine+i+1 >= numLines) return true;
    if (PeekTextLine(currLine+i)->waitForReturn) return false;
  };
  return currLine+i >= numLines;
}
//*****************************************************************************
//    NAME: TEXT_DISPLAY_DATA::NextBox
//
// PURPOSE: 
//
//*****************************************************************************
void TEXT_DISPLAY_DATA::NextBox()
{
  int i; // Search for the line that terminated the display
  InitialBoxDisplay = TRUE;
  for (i=0; i<TEXTBOX_LINES; i++)
  {
    if (currLine+i > numLines) 
    {
      currLine = numLines;
      return;
    };
    if (PeekTextLine(currLine+i)->waitForReturn)
    {
      currLine = currLine+i+1;
      if (currLine > numLines)
      {
        currLine = numLines;
      };
      return;
    };
  };
  currLine = currLine + i;
}

//*****************************************************************************
//    NAME: TEXT_DISPLAY_DATA::PrevBox
//
// PURPOSE: 
//
//*****************************************************************************
void TEXT_DISPLAY_DATA::PrevBox()
{
  int i;
  if (currLine > 0)
  {
    currLine -= 2;
    for (i=2; i<TEXTBOX_LINES; i++)
    {
      if (currLine < 0) break;
      if (PeekTextLine(currLine)->waitForReturn)
      {
        currLine = currLine+1;
        break;
      }
      else
      {
        currLine--;
      };
    };
    if (currLine < 0) currLine = 0;
    InitialBoxDisplay = TRUE;
  }
}

void TEXT_DISPLAY_DATA::FirstBox()
{
  currLine = 0;
  //  InitialBoxDisplay = TRUE;
}


//*****************************************************************************
//    NAME: TEXT_DISPLAY_DATA::IsFirstJournalBox
//
// PURPOSE: 
//
//*****************************************************************************
BOOL TEXT_DISPLAY_DATA::IsFirstJournalBox()
{
  return (currLine == 0);
}
//*****************************************************************************
//    NAME: TEXT_DISPLAY_DATA::IsLastJournalBox
//
// PURPOSE: 
//
//*****************************************************************************
BOOL TEXT_DISPLAY_DATA::IsLastJournalBox()
{
  return (currLine+JOURNAL_TEXTBOX_LINES >= numLines);
}
//*****************************************************************************
//    NAME: TEXT_DISPLAY_DATA::PrevJournalBox
//
// PURPOSE: 
//
//*****************************************************************************
void TEXT_DISPLAY_DATA::PrevJournalBox()
{
  if (currLine > 0)
  {
    currLine -= JOURNAL_TEXTBOX_LINES;
    if (currLine < 0) currLine = 0;
    InitialBoxDisplay = TRUE;
  }
}

//*****************************************************************************
//    NAME: TEXT_DISPLAY_DATA::NextJournalBox
//
// PURPOSE: 
//
//*****************************************************************************
void TEXT_DISPLAY_DATA::NextJournalBox()
{
  if (currLine < numLines)
  {
    currLine += JOURNAL_TEXTBOX_LINES;
    if (currLine > numLines) currLine = numLines;
    InitialBoxDisplay = TRUE;
  }
}
//*****************************************************************************
//    NAME: TEXT_DISPLAY_DATA::FirstJournalBox
//
// PURPOSE: 
//
//*****************************************************************************
void TEXT_DISPLAY_DATA::FirstJournalBox()
{
  currLine = 0;
}
//*****************************************************************************
//    NAME: TEXT_DISPLAY_DATA::LastJournalBox
//
// PURPOSE: 
//
//*****************************************************************************
void TEXT_DISPLAY_DATA::LastJournalBox()
{
  currLine = numLines - JOURNAL_TEXTBOX_LINES;

  if (currLine < 0) currLine = 0;
  if (currLine >= numLines) currLine = numLines-JOURNAL_TEXTBOX_LINES;
}


//*****************************************************************************
//    NAME: TEXT_DISPLAY_DATA::Quit
//
// PURPOSE: 
//
//*****************************************************************************
void TEXT_DISPLAY_DATA::Quit()
{
  while (currLine < numLines)
    currLine += TEXTBOX_LINES;

  currLine -= TEXTBOX_LINES;
  if (currLine < 0) currLine = 0;
  if (currLine >= numLines) currLine = numLines-TEXTBOX_LINES;
}

// width of textbox area in pixels
//*****************************************************************************
//    NAME: GetTextBoxCharWidth
//
// PURPOSE: 
//
//*****************************************************************************
unsigned int GetTextBoxCharWidth(void)
{  
  unsigned int width = 0;

  int maxCharWidth = GraphicsMgr.GetMaxCharacterWidth();
  ASSERT( maxCharWidth > 0 );

  //TEXTBOX_WIDTH = SCREEN_WIDTH - (textboxX*2); // actual width (used elsewhere)
  width = TEXTBOX_WIDTH - (maxCharWidth/2); // usable width for text
  ASSERT( width > 0 );
  return width;
}

// height of textbox area in pixels
//*****************************************************************************
//    NAME: GetTextBoxCharHeight
//
// PURPOSE: 
//
//*****************************************************************************
unsigned int GetTextBoxCharHeight(void)
{  
  //return (HorzBarBotY - textboxY - GraphicsMgr.GetMaxCharacterHeight() - 16);
  unsigned int height = 0;

  int maxCharHeight = GraphicsMgr.GetMaxCharacterHeight();
  ASSERT( maxCharHeight > 0 );
  
  height = TEXTBOX_HEIGHT;// - maxCharHeight; 
  TEXTBOX_LINES = height / maxCharHeight;

  return (height);
}

//*****************************************************************************
//    NAME: ClearFormattedTextScreenArea
//
// PURPOSE: 
//
//*****************************************************************************
void ClearFormattedTextScreenArea(BOOL front)
{
  RECT rect;
  rect.left = textboxX;
  rect.top = textboxY;
  rect.right = textboxX+GetTextBoxCharWidth();
  rect.bottom = textboxY+GetTextBoxCharHeight();

  if (front)
    GraphicsMgr.ClearSurface(FRONTBUFFERNUM, BACKGROUND_FILL, &rect);
  else
    GraphicsMgr.ClearSurface(BACKBUFFERNUM, BACKGROUND_FILL, &rect);
}

//*****************************************************************************
//    NAME: ClearFormattedText
//
// PURPOSE: 
//
//*****************************************************************************
void ClearFormattedText(TEXT_DISPLAY_DATA &data)
{   
   GraphicsMgr.HighlightText(FALSE);
   /*   PRS 20200208
   GraphicsMgr.ResetFontColorTags();
   GraphicsMgr.EnableFontColorTags(FALSE);
   */
   data.InitialBoxDisplay = TRUE;
   data.slowText = TRUE;
   data.numLines = 0;
   data.currLine = 0;
   data.pauseText = FALSE;
   //data.text.RemoveAll();
   data.RemoveAll();
}

//*****************************************************************************
//    NAME: TextIsMultiBox
//
// PURPOSE: 
//
//*****************************************************************************
BOOL TextIsMultiBox(TEXT_DISPLAY_DATA& data)
{
  int i;
  if (data.numLines > TEXTBOX_LINES) return true;
  for (i=0; i<data.numLines-1; i++)
  {
    if (data.PeekTextLine(i)->waitForReturn) return true;
  };
  return false;
}

//*****************************************************************************
//    NAME: StripInvalidChars
//
// PURPOSE: 
//
//*****************************************************************************
void StripInvalidChars(char *pIn)
{
  char *pChar=pIn;
  while ((pChar != NULL) && (*pChar != '\0'))
  {
    if ((*pChar < -127) || (*pChar > 255))
      *pChar = ' ';
    pChar++;
  }   
}

//*****************************************************************************
//    NAME: NumDigits
//
// PURPOSE: 
//
//*****************************************************************************
int NumDigits(int val)
{
  int sign = (val < 0);
  val = abs(val);

  if (val < 10) return (1+sign);
  if (val < 100) return (2+sign);
  if (val < 1000) return (3+sign);
  if (val < 10000) return (4+sign);
  if (val < 100000) return (5+sign);
  if (val < 1000000) return (6+sign);
  if (val < 10000000) return (7+sign);
  if (val < 100000000) return (8+sign);
  if (val < 1000000000) return (9+sign);
  return (10+sign);
}

// strips illegal ascii from text, and performs party
// name substitution into text.
//
// ^ = active party member name
// ^1..^8 = name of party member 1..8
// ^D = current game day
// ^H = current game hour
// ^M = current game minute
// ^a - ^z = temp ASL named 'a' - 'z'
//
//*****************************************************************************
//    NAME: PreProcessText
//
// PURPOSE: 
//
//*****************************************************************************
void PreProcessText(const char *pIn, char **pNew)
{
  CString out;
   const char TokenStartMarker = '^';
   if (pIn == NULL)
     return;
  if (pNew == NULL)
     return;

  //int TokenCount=0;
  //int SubstituteTextLength=0;
   const char *pChar=pIn;
/*
   while ((pChar != NULL) && (*pChar != '\0'))
   {
      if (*pChar == TokenStartMarker)
      {
        int partyOrdinal;
        partyOrdinal = 0;
        TokenCount++;
        // check to see if we have ^ tokens, and figure
        // out the required length of the new string
        if (   (pChar != NULL)
            && (*(pChar+1) != '\0'))
        {
          if ( (*(pChar+1) >= '1') && (*(pChar+1) <= '8') )
          {
            pChar++; // inc past ^
            char tmp[2];
            tmp[0] = *pChar;
            tmp[1] = '\0';
            int dude = atoi(tmp);
            dude -= 1; // zero-based in party data
            if ((dude < 0) || (dude >= party.numCharacters))
              dude = party.numCharacters-1;
            SubstituteTextLength += party.characters[dude].GetName().GetLength();
          }
          else if (*(pChar+1) == 'D')
          {
            pChar++; // inc past ^
            SubstituteTextLength += NumDigits(party.days);
          }
          else if (*(pChar+1) == 'H')
          {
            pChar++; // inc past ^
            SubstituteTextLength += 2; 
          }
          else if (*(pChar+1) == 'M')
          {
            pChar++; // inc past ^
            SubstituteTextLength += 2; 
          }
          else if ( (*(pChar+1) >= 'a') && (*(pChar+1) <= 'z') )
          {
            pChar++; // inc past ^
            CString key = *pChar;
            CString val = globalData.temp_asl.Lookup((LPCSTR)key);
            SubstituteTextLength += val.GetLength();
          }
          else // just regular ^
            SubstituteTextLength += party.getActiveChar().GetName().GetLength();
        }
        else // ^ is last in string
          SubstituteTextLength += party.getActiveChar().GetName().GetLength();
      }
      pChar++;
   }   
   if (TokenCount == 0)
   {    
     *pOut = new char[strlen(pIn)+1];
     strcpy(*pOut, pIn);
     StripInvalidChars(*pOut);
     return;
   }
*/

   // create new string with appropriate length

   pChar = pIn;
   out = CString(' ', strlen(pIn) + 200); 
   // Make some room available so we don't alloc new buffer for each character.
   out = "";
   // copy old string to new string
   while (*pChar != '\0')
   {
      if (*pChar == TokenStartMarker)
      {
        // check to see if we have ^ and substitute replacement
        // text for ^ token
        if (pChar[1] != '\0')
        {
          int dude = -1;
          if ( (pChar[1]=='1') && (pChar[2]>='0') && (pChar[2]<='2') )
          {
            dude = 10 + pChar[2] - '0' - 1;
            pChar += 2;
          }
          else if ( (pChar[1] >= '1') && (pChar[1] <= '8') )
          {
            dude = pChar[1] - '0' - 1;
            pChar++;
          }
          else if (pChar[1] == 'D')
          {
            pChar++; // inc past ^
            CString num;
            num.Format("%i", party.days);
            out += num;            
          }
          else if (pChar[1] == 'H')
          {
            pChar++; // inc past ^
            CString num;
            num.Format("%02i", party.hours);
            out += num;            
          }
          else if (pChar[1] == 'M')
          {
            pChar++; // inc past ^
            CString num;
            num.Format("%02i", party.minutes);
            out +=num;            
          }
          else if ( (*(pChar+1) >= 'a') && (*(pChar+1) <= 'z') )
          {
            pChar++; // inc past ^
            CString key = *pChar;
            CString val = globalData.global_asl.Lookup((LPCSTR)key);
            out += val; 
          }
          else // just regular ^
            out += party.getActiveChar().GetName();

          if (dude >= party.numCharacters)
          {
             dude = party.numCharacters-1; 
          };
          if (dude >= 0)
          {      
            out += party.characters[dude].GetName();            
          };
        }
        else // ^ is last in string
          out += party.getActiveChar().GetName();        
      }
      else
        out += pChar[0];

      pChar++;
   }
  //*pOut = new char[strlen(pIn) + SubstituteTextLength + 1];
  *pNew = new char[out.GetLength() + 1];
  memcpy(*pNew, (LPCSTR)out, out.GetLength());
  (*pNew)[out.GetLength()] = 0;
   // remove unprintable characters
  StripInvalidChars(*pNew);
}

//*****************************************************************************
//    NAME: PostProcessText
//
// PURPOSE: 
//
//*****************************************************************************
void PostProcessText(TEXT_DISPLAY_DATA &data)
{
  // not likely to be a character set that is less than
  // 5 pixels in width on average
  WORD MaxOneLineWidthInChars = SCREEN_WIDTH/5;
  char ch[2];
  ch[1] = '\0';

  char *temp;
  temp = new char[MaxOneLineWidthInChars+1];

	for (int i=0;i<data.numLines;i++)
	{
    temp[0] = '\0';

    //int max = data.text[i].GetLength();
    int max = data.PeekTextLine(i)->text.GetLength();
    max = min( max, MaxOneLineWidthInChars );
    for (int l=0; l < max; l++)
    {
      if (   (data.PeekTextLine(i)->text[l] != '\n')
          && (data.PeekTextLine(i)->text[l] != '\r')
          && (data.PeekTextLine(i)->text[l] != '\b')
          && (data.PeekTextLine(i)->text[l] != '\t'))
      {
        ch[0] = data.PeekTextLine(i)->text[l];
        strncat(temp, ch, 1);
      }
    }

    //data.text[i] = temp;
    data.GetTextLine(i)->text = temp;
	}

  if (temp != NULL)
    delete [] temp;
}

void FormatMultiLineText(const char* text, int lineWidth, TEXT_DISPLAY_DATA* data)
{
  FORMATTED_TEXT FT;
  FT.Initialize(text, 99999, 'W', whiteColor, 0, 0);
  data->RemoveAll();
  FORMATTED_TEXT::FTStatus ftStat;
  char curChar;
  int currLineWidth;
  CString preamble;

  // While there is more text, divide it into lines that
  // will fit in the text area of the screen.
  currLineWidth = 0;
  preamble.Empty();  // The first line needs no preamble.
  while ((ftStat = FT.NextChar()) != FORMATTED_TEXT::FTEOF)
  {
    switch (ftStat)
    {
    case FORMATTED_TEXT::FTPrintable:
      // Add with of character to currLineWidth;
      curChar = FT.CurrentCharacter();
      currLineWidth += GraphicsMgr.GetTextWidth(&curChar, 1);
      if (currLineWidth > lineWidth)
      {
        FT.Backup();  // Back to nearest whitespace
        data->Add(FT.GetString(&preamble));
        FT.SkipSpace();  // Skip leading whitespace in next line.
        data->numLines++;
        currLineWidth = 0;
        break;
      };
      break;
    case FORMATTED_TEXT::FTEscape:
      break;    // Simply skip over it.
    case FORMATTED_TEXT::FTColor:
      break;    // Skip over color tags
    case FORMATTED_TEXT::FTDigit:
      break;    // Skip over font tag first digit
    case FORMATTED_TEXT::FTFontTag:
      break;    // Skip over legal font tag
    case FORMATTED_TEXT::FTCR:
      // We have a complete line of text that will fit.
      data->Add(FT.GetString(&preamble));  // Updates FT to start of new line.
      data->numLines++;
      currLineWidth = 0;
      break;
    case FORMATTED_TEXT::FTCRNL:
      break;  // Ignore newline after carriage return.
    case FORMATTED_TEXT::FTNLCR:
      break;  // Ignore carriage return after newline.
    case FORMATTED_TEXT::FTNL:
      break;  // We only process FTCR.
    case FORMATTED_TEXT::WAIT:
        data->Add(FT.GetString(&preamble));  // Updates FT to start of new line.
        data->numLines++;
        currLineWidth = 0;
        data->GetTextLine(data->GetTextLineCount()-1)->waitForReturn = TRUE;
        break;  // We only process FTCR.
    case FORMATTED_TEXT::SKIPNEXTCOLOR:
        break; // ignore
    default:
      die(0x4a744);
    };

  };
  {
    CString lastString;
    lastString = FT.GetString(&preamble);
    if (!lastString.IsEmpty())
    {
      data->Add(lastString);
      data->numLines++;
    };
  };
}

// keep it short, you only get 1 box of text when using paused text
//*****************************************************************************
//    NAME: FormatPausedText
//
// PURPOSE: 
//
//*****************************************************************************
void FormatPausedText(TEXT_DISPLAY_DATA &data, const char *pText)
{
  ResetFontColorTags();
  ClearFormattedText(data);
  
  if (pText == NULL)
    return;
  if (strlen(pText) == 0)
    return;
  
  char *buffer=NULL;
  PreProcessText(pText, &buffer);
  
  data.slowText = FALSE;
  data.highlightAll = FALSE;
  data.pauseText = TRUE;
  
  int char_pixels_per_line = GetTextBoxCharWidth();
  if (char_pixels_per_line <= 0)
    return;

  FormatMultiLineText(buffer, char_pixels_per_line, &data);

  /*
  BOOL done = FALSE;
  const char *start = buffer;
  const char *end = buffer;
  int currLineWidth = 0;
  
  while ((!done) && (start != NULL) && (*start != '\0'))
  {
    currLineWidth = 0;
    
    while ( (!done) && (currLineWidth < char_pixels_per_line) && (!IsNewLine(end)))
    {
      currLineWidth += GraphicsMgr.GetTextWidth(end, 1);
      end++;      
      if (*end == '\0') done = TRUE;
    }
    
    CString NewString("");        

    if (done)
    {
       NewString+=start;
    }
    else
    {
       CString tmpNewString("");
       const char *temp = end;

       while ((!IsWhiteSpace(temp)) && (temp != start))
          temp--;

       if (temp != start)
       {              
          tmpNewString.Format("%.*s", temp-start, start);
          end = temp;
       }
       else
       {
         tmpNewString.Format("%.*s", end-start, start);
       }
       NewString += tmpNewString;
    }

    // text wrapped to next line doesn't need to start with a space
    if (NewString[0]==' ') NewString.Delete(0);
    data.Add(NewString);
    
    if (!done)
    {
      while ( (*end == '\r') || (*end == '\n') )
        end++;
    }
    
    data.numLines++;
    start = end;
  }
  
  if (buffer != NULL)
    delete [] buffer;
  */
  PostProcessText(data);
  data.startTime = virtualGameTime;
}

//*****************************************************************************
//    NAME: FormatJournalText
//
// PURPOSE: 
//
//*****************************************************************************
void FormatJournalText(TEXT_DISPLAY_DATA &data, JOURNAL_DATA &jdata)
{
   ResetFontColorTags();
   ClearFormattedText(data);
   if (jdata.GetCount() <= 0) return;

   CString text="";
   int count=0;
   POSITION pos = jdata.jlist.GetHeadPosition();
   while (pos != NULL)
   {
     if (jdata.jlist.PeekAtPos(pos).text.GetLength() > 0)
     {
       count++;
       text += jdata.jlist.PeekAtPos(pos).text;
       if (count < jdata.GetCount()) 
         text += "\b\n\n";
     }
     jdata.jlist.GetNext(pos);
   }

   if (text.GetLength() <= 0) return;

   char *pText = new char[text.GetLength()+1];
   strcpy(pText, text);
   pText[text.GetLength()]='\0';

    data.slowText = FALSE;
    data.highlightAll = FALSE;
    int char_pixels_per_line = GetTextBoxCharWidth();
    if (char_pixels_per_line <= 0)
      return;

    FormatMultiLineText(pText, char_pixels_per_line, &data);

/*


    BOOL done = FALSE;
    const char *start = pText;
    const char *end = pText;
    int currLineWidth = 0;
    CString CurrColor("");
    BOOL ResetColor=FALSE;

    while ((!done) && (start != NULL) && (*start != '\0'))
    {
       ResetColor=FALSE;
       currLineWidth = 0;

        while ( (!done) && (currLineWidth < char_pixels_per_line) && (!IsNewLine(end)))
        {
          currLineWidth += GraphicsMgr.GetTextWidth(end, 1);
          ResetColor = CheckJournalColorTag(end);
          end++;
          if (*end == '\0') done = TRUE;
        }

        CString NewString("");

        if (done)
        {
          NewString+=start;
        }
        else
        {
           CString tmpNewString("");
           const char *temp = end;

           while ((!IsWhiteSpace(temp)) && (temp != start))
              temp--;

           if (temp != start)
           {
              tmpNewString.Format("%.*s", temp-start, start);
              end = temp;
           }
           else
           {
              tmpNewString.Format("%.*s", end-start, start);
           }
           NewString += tmpNewString;
        }

        // text wrapped to next line doesn't need to start with a space
        if (NewString[0]==' ') NewString.Delete(0);

        ScanForColorTags(NewString);

        if ((CurrColor!="")&&(NeedStartingColorTag(NewString, CurrColor)))
          NewString = CurrColor+NewString;

        CurrColor=AddColorTags(NewString); 
        if (NewString[0]==' ') NewString.Delete(0);
        //data.text.Add(NewString);
        data.Add(NewString);

        if (ResetColor)
        {
          CurrColor="";
          ResetFontColorTags();
        }

        if (!done)
        {
          while ( (*end == '\r') || (*end == '\n') )
          {
            //data.text.Add(" ");

            // Deleted 20170815 prs.....user complained of 
            // blank lines in text display.
            //data.Add(" ");
            //data.numLines++;

            char curr = *end;
            end++;
            
            // edit boxes use "\r\n" to skip lines
            // but only need to add one blank line to
            // represent that sequence
            if ( (*end == '\r') || (*end == '\n') )
            {
              if (curr != *end)
                end++;
            }
          }
        }

        data.numLines++;
        start = end;
    }
*/
   PostProcessText(data);
   data.startTime = virtualGameTime;
   if (pText != NULL)
     delete [] pText;
}

FORMATTED_TEXT::FORMATTED_TEXT(void)
{
}
  
void FORMATTED_TEXT::Initialize(const char    *text,
                                int            length,
                                char           colorChar,
                                FONT_COLOR_NUM colorNum,
                                int            fontNum,
                                int            index)
{
  m_pText               = text;
  m_length              = length;
  m_startingFontNum     = fontNum;
  m_currentFontNum      = fontNum;
  m_startingColorNum    = colorNum;
  m_currentColorNum     = colorNum;
  m_customColorActive   = FALSE;
  m_startingColorChar   = colorChar;
  m_currentColorChar    = colorChar;
  m_currentCharIndex    = index;
  m_lastLineBreakIndex  = -1;
  m_currentChar         = 0;
  m_prevStatus          = FTStart;
}

void FORMATTED_TEXT::TestNextChar(void)
{
  char escChar = '/';
  // Ready for a printable character.
  // Sets m_prevStatus to resulting status
  if (     (m_pText[m_currentCharIndex] == 0)
       ||  (m_currentCharIndex >= m_length) )
  {
    m_prevStatus = FTEOF;
    return;
  };
  if (m_pText[m_currentCharIndex] == escChar)
  {
    m_prevStatus = FTEscape;
    m_currentCharIndex++;
    return;
  };
  if (m_pText[m_currentCharIndex] == '\r')
  {
    if (m_prevStatus == FTNL)
    {
      m_prevStatus = FTNLCR;
    }
    else
    {
      m_prevStatus = FTCR;
    };
    m_currentCharIndex++;
    return;
  };
  if (m_pText[m_currentCharIndex] == '\n')
  {
    if (m_prevStatus == FTCR)
    {
      m_prevStatus = FTCRNL;
    }
    else
    {
      m_prevStatus = FTNL;
    };
    m_currentCharIndex++;
    return;
  };
  m_currentChar = m_pText[m_currentCharIndex];
  if (IsWhiteSpace(&m_currentChar))
  {
    m_lastLineBreakIndex = m_currentCharIndex;
  };
  m_currentCharIndex++;
  m_prevStatus = FTPrintable;
  return;
}

FORMATTED_TEXT::FTStatus FORMATTED_TEXT::NextChar(void)
{
  switch (m_prevStatus)
  {
  case FTStart:
    TestNextChar();
    return m_prevStatus;
//  case FTStartLine:
//    TestNextChar();
//    return m_prevStatus;
  case FTColor:
    TestNextChar();
    return m_prevStatus;
  case FTFontTag:
    TestNextChar();
    return m_prevStatus;
  case FTPrintable:
    TestNextChar();
    return m_prevStatus;
  case FTEscape:
    m_currentChar = m_pText[m_currentCharIndex++];
    m_prevStatus = FTColor;  // Just a guess.
    if (!GraphicsMgr.GetSkipNextFontColor()) {
      switch (m_currentChar)
        {
        case 'W': case 'w':
          m_customColorActive = FALSE;
          m_currentColorNum = whiteColor;
          m_currentColorChar = 'W'; 
          break;
        case 'Y': case 'y':
          m_customColorActive = FALSE;
          m_currentColorNum = yellowColor;
          m_currentColorChar = 'Y'; 
          break;
        case 'O': case 'o':
          m_customColorActive = FALSE;
          m_currentColorNum = orangeColor;
          m_currentColorChar = 'O';
          break;
        case 'T': case 't':
          m_customColorActive = FALSE;
          m_currentColorNum = brightOrangeColor;
          m_currentColorChar = 'T'; 
          break;
        case 'R': case 'r':
          m_customColorActive = FALSE;
          m_currentColorNum = redColor;
          m_currentColorChar = 'R'; 
          break;
        case 'G': case 'g':
          m_customColorActive = FALSE;
          m_currentColorNum = greenColor;
          m_currentColorChar = 'G'; 
          break;
        case 'B': case 'b':
          m_customColorActive = FALSE;
          m_currentColorNum = blueColor;
          m_currentColorChar = 'B'; 
          break;
        case 'V': case 'v':
          m_customColorActive = FALSE;
          m_currentColorNum = cyanColor;
          m_currentColorChar = 'V'; 
          break;
        case 'K': case 'k':
          m_customColorActive = FALSE;
          m_currentColorNum = blackColor;
          m_currentColorChar = 'K'; 
          break;
        case 'M': case 'm':
          m_customColorActive = FALSE;
          m_currentColorNum = magentaColor;
          m_currentColorChar = 'M'; 
          break;
        case 'S': case 's':
          m_customColorActive = FALSE;
          m_currentColorNum = silverColor;
          m_currentColorChar = 'S'; 
          break;
        case 'H': case 'h':
          m_currentColorChar = 'H';
          GraphicsMgr.HighlightText(true);  // This will be turned off in Graphics.DrawText after this string has been processed
          break;
        case 'N': case 'n':
          return FTStatus::WAIT;
          break;
        case '#':
          GraphicsMgr.EnableSkipNextFontColor(true);
          return FTStatus::SKIPNEXTCOLOR;
          break;
       case 'C': case 'c':
          m_customColorActive = TRUE;
          // What are we to do?  There are as many as 100 different fonts.
          // What font are we to assign to this custom color?
          // Here is our plan.....All 100 fonts can have the color overridden
          // with the "/c" escape sequence.
          // So for example:
          //   /26/M/C  will print in the font 35 in the custom color.
          //   The /C will cease to apply when any other color is specified (/M /B etc)
          //   Specifying a new base font ( /27 for example) leaves /C in effect.
          //   So the /C simply overrides the RGB value of the currently selected font.
          break;
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
          m_prevStatus = FTDigit;
          return m_prevStatus;
        default:
          // backup and send the characters as printable.
          m_currentCharIndex -= 1;
          m_currentChar = '/';
          m_prevStatus = FTPrintable;
          return m_prevStatus;
        };
    }
    else {
      GraphicsMgr.EnableSkipNextFontColor(FALSE);
    }
    return m_prevStatus;
  case FTDigit:
    m_currentChar = m_pText[m_currentCharIndex++];
    switch (m_currentChar)
    {
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      m_currentFontNum = 10 *(m_pText[m_currentCharIndex - 2] - '0')
                            + m_pText[m_currentCharIndex - 1] - '0';
      m_prevStatus = FTFontTag;
      return m_prevStatus;
    default:
      // Backup and treat it like a printable character
      m_currentCharIndex -= 2;
      m_currentChar = '/';
      m_prevStatus = FTPrintable;
      return m_prevStatus;
    };
  case FTCR:
    TestNextChar();
    return m_prevStatus;
  case FTCRNL:
    TestNextChar();
    return m_prevStatus;
  case FTEOF:
    return m_prevStatus;
  case FTNL:
    TestNextChar();
    return m_prevStatus;
  default:
    die(0x551b0a);
    return FTEOF;
  };

//  die(0x551ca8);
};

void FORMATTED_TEXT::Backup(void)
{
  int length;
  FORMATTED_TEXT FT1;
  if (m_lastLineBreakIndex <= 0) return;
  length = m_lastLineBreakIndex;
  FT1.Initialize(m_pText, 
                 length, 
                 m_startingColorChar,
                 m_startingColorNum,
                 m_startingFontNum, 
                 0);
  // Now we need to re-process the line of text because we
  // may have backed up over formatting tags.
  while (FT1.NextChar() != FTEOF) {};
  m_currentColorNum  = FT1.m_currentColorNum;
  m_currentColorChar = FT1.m_currentColorChar;
  m_currentFontNum   = FT1.m_currentFontNum;
  m_currentCharIndex   = length;
}

void FORMATTED_TEXT::SkipSpace(void) // Skip any leading whitewpace.
{
  if (m_currentCharIndex != 0) return;
  if (m_pText[0] == ' ')
  {
    m_pText++;
  };
}

CString FORMATTED_TEXT::GetString(CString *preamble)
{
  CString result;
  int length;
  length = m_currentCharIndex;
  while ((length > 0) && (((unsigned char)m_pText[length - 1]) < ' '))
  {
    length--;
  };
  result = *preamble + CString(m_pText, length);
  m_pText += m_currentCharIndex;
  m_currentCharIndex  = 0;
  m_startingColorChar = m_currentColorChar;
  m_startingColorNum  = m_currentColorNum;
  m_startingFontNum   = m_currentFontNum;
//  m_prevStatus = FTStartLine;
  if (m_startingColorNum == 0)
    if (m_startingFontNum == 0)
      (*preamble).Empty();
    else
      (*preamble).Format("/%02d", m_startingFontNum);
  else
    if (m_startingFontNum == 0)
      (*preamble).Format("/%c", m_startingColorChar);
    else
      (*preamble).Format("/%c/%02d", m_startingColorChar, m_startingFontNum);
  return result;
}

//*****************************************************************************
//    NAME: FormatDisplayText
//
// PURPOSE: 
//
//*****************************************************************************
void FormatDisplayText(TEXT_DISPLAY_DATA& data,
  const char* pText,
  BOOL slowText,
  BOOL HighlightAll,
  BOOL requireEnter)
{
  ResetFontColorTags();
  ClearFormattedText(data);

  data.eventRequiresReturn = requireEnter;

  if (pText == NULL)
    return;
  if (strlen(pText) == 0)
    return;

  char* buffer = NULL;
  PreProcessText(pText, &buffer); // Handle special character '^'

  if (buffer != NULL)
  {
    pText = buffer;
  };

  if (INTER_CHARACTER_DELAY > 0)
    data.slowText = slowText;
  else
    data.slowText = FALSE;

  data.highlightAll = HighlightAll;
  int char_pixels_per_line = GetTextBoxCharWidth(); //DLD 12/20/00 now a changable var //unsigned int char_pixels_per_line = GetTextBoxCharWidth();
  if (char_pixels_per_line <= 0)
  {
    if (buffer != NULL)
    {
      delete[] buffer;
    }
    return;
  };
  /*
      BOOL done = FALSE;
      const char *start = buffer;
      const char *end = buffer;
      int currLineWidth = 0;
      int waitForReturn;
      CString CurrColor("");

      while ((!done) && (start != NULL) && (*start != '\0'))
      {
         currLineWidth = 0;
         waitForReturn = FALSE;

          while ( (!done) && (currLineWidth < char_pixels_per_line) && (!IsNewLine(end)))
          {
            currLineWidth += GraphicsMgr.GetTextWidth(end, 1);
            end++;
            if (*end == '\0') done = TRUE;
          }

          CString NewString("");

          if (done)
          {
             NewString+=start;
          }
          else
          {
             CString tmpNewString("");
             const char *temp = end;

             while ((!IsWhiteSpace(temp)) && (temp != start))
                temp--;

             if (temp != start)
             {
                tmpNewString.Format("%.*s", temp-start, start);
                end = temp;
             }
             else
             {
               tmpNewString.Format("%.*s", end-start, start);
             }
             NewString += tmpNewString;
          }

          // text wrapped to next line doesn't need to start with a space
          if (NewString[0]==' ') NewString.Delete(0);

          ScanForColorTags(NewString);  // Process special character '/'

          if ((CurrColor!="")&&(NeedStartingColorTag(NewString, CurrColor)))
            NewString = CurrColor+NewString;

          CurrColor=AddColorTags(NewString);
          if (NewString[0]==' ') NewString.Delete(0);
          //data.text.Add(NewString);
          data.Add(NewString);
          if ( (end[0] == '/') && (end[1] == 'n') )
          {
            data.GetTextLine(data.GetTextLineCount()-1)->waitForReturn = TRUE;
            end = end+2;
          }

          if (!done)
          {
            //while ( (*end == '\r') || (*end == '\n') )
            //  end++;
            {
              while ( (end[0] == '\r') || (end[0] == '\n') )
              {
                //data.text.Add(" ");

              // Deleted 20170815 prs.....user complained of
              // blank lines in text display.
              //  data.Add(" ");
              //  data.numLines++;

                char curr = *end;
                end++;

                // edit boxes use "\r\n" to skip lines
                // but only need to add one blank line to
                // represent that sequence
                if ( (*end == '\r') || (*end == '\n') )
                {
                  if (curr != *end)
                    end++;
                }
              }
            };
          }

          data.numLines++;
          start = end;
      }

     if (buffer != NULL)
       delete [] buffer;
  */
  FormatMultiLineText(pText, char_pixels_per_line, &data);
  /*
// Measure the characters and decide what will fit on each line.
   {
     FORMATTED_TEXT FT(pText,    // First character of text
                       999999,   // length
                       'W',      // color char
                       0,        // color Number
                       0,        // font number
                       0);       // index
     FORMATTED_TEXT::FTStatus ftStat;
     char curChar;
     int currLineWidth;
     CString preamble;

     // While there is more text, devide it into lines that
     // will fit in the text area of the screen.
     currLineWidth = 0;
     preamble.Empty();  // The first line needs no preamble.
     while ((ftStat = FT.NextChar()) != FORMATTED_TEXT::FTEOF)
     {
       switch (ftStat)
       {
         case FORMATTED_TEXT::FTPrintable:
           // Add with of character to currLineWidth;
           curChar = FT.CurrentCharacter();
           currLineWidth += GraphicsMgr.GetTextWidth(&curChar, 1);
           if (currLineWidth > char_pixels_per_line)
           {
             FT.Backup();  // Back to nearest whitespace
             data.Add(FT.GetString(&preamble));
             FT.SkipSpace();  // Skip leading whitespace in next line.
             data.numLines++;
             currLineWidth = 0;
             break;
           };
           break;
         case FORMATTED_TEXT::FTEscape:
           break;    // Simply skip over it.
         case FORMATTED_TEXT::FTColor:
           break;    // Skip over color tags
         case FORMATTED_TEXT::FTDigit:
           break;    // Skip over font tag first digit
         case FORMATTED_TEXT::FTFontTag:
           break;    // Skip over legal font tag
         case FORMATTED_TEXT::FTCR:
           // We have a complete line of text that will fit.
           data.Add(FT.GetString(&preamble));  // Updates FT to start of new line.
           data.numLines++;
           currLineWidth = 0;
           break;
         case FORMATTED_TEXT::FTCRNL:
           break;  // Ignore newline after carriage return.
         case FORMATTED_TEXT::FTNLCR:
           break;  // Ignore carriage return after newline.
         case FORMATTED_TEXT::FTNL:
           break;  // We only process FTCR.
         default:
           die(0x4a744);
       };
     };
     {
       CString lastString;
       lastString = FT.GetString(&preamble);
       if (!lastString.IsEmpty())
       {
         data.Add(lastString);
         data.numLines++;
       };
     };
   };
   */
  PostProcessText(data);
  data.startTime = virtualGameTime;
  if (buffer != NULL)
  {
    delete[] buffer;
  };
}

//*****************************************************************************
//    NAME: FormatCombatText
//
// PURPOSE: 
//
//*****************************************************************************
void FormatCombatText(TEXT_DISPLAY_DATA &data, const char *pText)
{
   ResetFontColorTags();
   ClearFormattedText(data);

   if (pText == NULL) return;
   if (strlen(pText) == 0) return;

   data.slowText = FALSE;
   data.highlightAll = FALSE;
   
   int char_pixels_per_line = COMBAT_STAT_BOX_WIDTH - 
                              //GraphicsMgr.GetMaxCharacterWidth() - 
                              (VertBarSrc.right-VertBarSrc.left) - 16;
    if (char_pixels_per_line <= 0)
    {
      char_pixels_per_line = 240;
    };
    BOOL done = FALSE;
    const char *start = pText;
    const char *end = pText;
    int currLineWidth = 0;

    while ((!done) && (start != NULL) && (*start != '\0'))
    {
       currLineWidth = 0;

        while ( (!done) && (currLineWidth < char_pixels_per_line) && (*end != '\n') && (*end != '\r'))
        {
          currLineWidth += GraphicsMgr.GetTextWidth(end, 1);
          end++;
          if (*end == '\0') done = TRUE;
        }

        CString NewString("");        

        if (done)
        {
           NewString+=start;
        }
        else
        {
           CString tmpNewString("");
           const char *temp = end;

           while ((*temp != ' ') && (*temp != '\n') && (*temp != '\r') && (temp != start))
              temp--;

           if (temp != start)
           {             
              tmpNewString.Format("%.*s", temp-start, start);
              end = temp;
           }
           else
           {
             tmpNewString.Format("%.*s", end-start, start);
           }
           NewString += tmpNewString;
        }

        if (NewString[0]==' ') NewString.Delete(0);
        //data.text.Add(NewString);
        data.Add(NewString);

        if (!done)
        {
          while ( (*end == '\r') || (*end == '\n') )
            end++;
        }

        data.numLines++;
        start = end;
    }
}

// keep it short, you only get 1 box of text when using paused text
//*****************************************************************************
//    NAME: DisplayPausedText
//
// PURPOSE: 
//
//*****************************************************************************
// prs  20191220 void DisplayPausedText(TEXT_DISPLAY_DATA &data, COLORREF color)
void DisplayPausedText(TEXT_DISPLAY_DATA& data, FONT_COLOR_NUM colorNum, int fontNumber)
{
  if (data.numLines <= 0) return;

  int x = textboxX;
  int y = textboxY + (GetTextBoxCharHeight()/2); // middle of box height
  int curr = data.currLine;
  int last = curr + TEXTBOX_LINES;
  if (last > data.numLines)
    last = data.numLines;
  BOOL useFrontbuffer = data.NeedsFrontBuffer();
  BOOL useSlowText = data.UseSlowText();

  if (useFrontbuffer) MouseRender.ShowMouseCursor(FALSE);

  // clear text area on screen
  ClearFormattedTextScreenArea(useFrontbuffer);

  /*  PRS  20200208
  GraphicsMgr.ResetFontColorTags();
  //GraphicsMgr.EnableFontColorTags(TRUE);
  */
  while (curr < last)
  {
    // to allow for slow text, formatted text is drawn directly to the front
    // buffer, so this function must be called after the back buffer is flipped
    //displayText(x,y, data.text[curr], color, data.highlightAll, useSlowText, !useFrontbuffer);
    DisplayText(x,y, fontNumber, 
                data.PeekTextLine(curr)->text, 
                colorNum, 
                FALSE,
                data.highlightAll, 
                useSlowText, 
                !useFrontbuffer);
    y += 18;
    curr++;
  }

  if (useFrontbuffer) MouseRender.ShowMouseCursor(TRUE);

  //GraphicsMgr.EnableFontColorTags(FALSE);
  data.InitialBoxDisplay = FALSE;
  theApp.AppSleep(GetMsgTimeDelay());
}

//*****************************************************************************
//    NAME: DisplayFormattedText
//
// PURPOSE: 
//
//*****************************************************************************
// prs 20191220 void DisplayFormattedText(TEXT_DISPLAY_DATA &data, COLORREF color)
void DisplayFormattedText(TEXT_DISPLAY_DATA& data, FONT_COLOR_NUM colorNum, int fontNumber)
{
   if (data.numLines <= 0) return;
   int x = textboxX;
   int y = textboxY;

   int curr = data.currLine;
   int last = curr + TEXTBOX_LINES;

   if (last > data.numLines)
      last = data.numLines;

   BOOL useFrontbuffer = data.NeedsFrontBuffer();
   BOOL useSlowText = data.UseSlowText();

   if (useFrontbuffer) MouseRender.ShowMouseCursor(FALSE);

   /*  PRS  20200208
   GraphicsMgr.ResetFontColorTags();
   GraphicsMgr.EnableFontColorTags(TRUE);
   */
   while (curr < last)
   {
      // to allow for slow text, formatted text is drawn directly to the front
      // buffer, so this function must be called after the back buffer is flipped
      if (data.PeekTextLine(curr)->waitForReturn)
      {
        last = curr+1;
      };
      /*  PRS  20200208
      GraphicsMgr.ResetFontColorTags();
      */
      //displayText(x,y, data.text[curr], color, data.highlightAll, useSlowText, !useFrontbuffer);
      DisplayText(x,y, 
                  fontNumber, 
                  data.PeekTextLine(curr)->text, 
                  colorNum, FALSE,
                  data.highlightAll, useSlowText, !useFrontbuffer);
      //y += 18;
      y += GraphicsMgr.GetMaxCharacterHeight();
      curr++;
   }

   /*  PRS  20200208
   GraphicsMgr.EnableFontColorTags(FALSE);
   */
   if (useFrontbuffer) MouseRender.ShowMouseCursor(TRUE);

   data.InitialBoxDisplay = FALSE;
}

//*****************************************************************************
//    NAME: DisplayJournalText
//
// PURPOSE: 
//
//*****************************************************************************
// prs 20191220 void DisplayJournalText(TEXT_DISPLAY_DATA &data, COLORREF color)
void DisplayJournalText(TEXT_DISPLAY_DATA& data, FONT_COLOR_NUM colorNum, int fontNumber)
{  
   if (data.numLines <= 0) return;

   int x = 18;
   int y = 38;

   int curr = data.currLine;
   int last = curr + JOURNAL_TEXTBOX_LINES;

   if (last > data.numLines)
      last = data.numLines;

   BOOL useFrontbuffer = data.NeedsFrontBuffer();
   BOOL useSlowText = data.UseSlowText();

   if (useFrontbuffer) MouseRender.ShowMouseCursor(FALSE);
   
   GraphicsMgr.EnableFontColorTags(TRUE);

   while (curr < last)
   {
      // to allow for slow text, formatted text is drawn directly to the front
      // buffer, so this function must be called after the back buffer is flipped
  /*  PRS  20200208
      GraphicsMgr.ResetFontColorTags();
      */
      //displayText(x,y, data.text[curr], color, data.highlightAll, useSlowText, !useFrontbuffer);
      DisplayText(x,y, 
                  fontNumber, 
                  data.PeekTextLine(curr)->text, 
                  colorNum, FALSE,
                  data.highlightAll, useSlowText, !useFrontbuffer);
      y += 18;
      curr++;
   }

   GraphicsMgr.EnableFontColorTags(FALSE);

   if (useFrontbuffer) MouseRender.ShowMouseCursor(TRUE);

   data.InitialBoxDisplay = FALSE;
}
