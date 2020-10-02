/******************************************************************************
* Filename: TextForm.cpp
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
#include "GlobalData.h"
#include "Graphics.h"
#include "TextForm.h"
#include "Disptext.h"
#include "GlobalData.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


void TEXT_FORM::FORM_ITEM::Clear(void)
{
  m_left=m_top=m_bottom=m_right=-1;
// prs 20191221  m_color=0xffffffff;
  m_colorNum = illegalColor;
  m_text="";
  m_highlight=false;
  m_show=true;
  m_column = 0;
  m_width = 0;
  //m_iFieldName = 0;
};

TEXT_FORM::TEXT_FORM (DISPLAY_FORM *src) 
{
  DISPLAY_FORM *pdf;
  int i;
  m_numItems=0;
  //for (pdf=src; pdf->fieldName!=0; pdf++, m_numItems++) 
  //  /* empty loop */;  
  for (pdf=src; pdf->iFieldName!=0; pdf++) 
  {
    if (pdf->x_relative == autorepeat)
    {
      int repeatcount = pdf->y; // how many repeats
      int repeatitems = pdf->x; // nbr following items to repeat
      m_numItems += (repeatcount * repeatitems);
      pdf += repeatitems; // skip over repeats
    }
    else
      m_numItems++;
  }

  m_formItems=new FORM_ITEM[m_numItems];
  int srcidx=0;
  for (i=0; i<m_numItems; i++) 
  {
    if (src[srcidx].x_relative == autorepeat)
    {
      // This is not very general code. It was created for the
      // specific purpose of automatically creating 
      // place holders for vertically displayed lists
      // of data such as inventory and shop items.
      //
      int repeatcount = src[i].y; // how many repeats
      int repeatitems = src[i].x; // nbr following items to repeat
      srcidx++; // skip past repeat indicator      

      for (int rows=0;rows<repeatcount;rows++)
      {      
        for (int items=0;items<repeatitems;items++)
        {
          m_formItems[i]=src[srcidx+items];
          m_formItems[i].m_iFieldName += (repeatIncr*rows); // auto-inc the field id

          if (src[srcidx+items].x_relative & sel)
          {
            m_formItems[i].m_xRelative = m_formItems[i-(repeatitems-1)].m_iFieldName;
            m_formItems[i].m_yRelative = m_formItems[i-1].m_iFieldName;
          }
          else
          {
            // first row uses given coords,
            // the rest are relative to the previous
            if (rows>0)
            {
              m_formItems[i].m_y=0;
              m_formItems[i].m_yRelative=m_formItems[i-repeatitems].m_iFieldName+end;
            }
          }
          i++;
        }
      }
      srcidx += repeatitems; // skip over repeats
    }
    else
    {
      m_formItems[i]=src[srcidx];
      srcidx++;
    }
  }
}

TEXT_FORM::~TEXT_FORM(void) 
{
  if (m_formItems!=NULL)
  {
    delete [] m_formItems;
    m_formItems=NULL;
  };
}


void TEXT_FORM::ClearForm(void)
{
  int i;
  for (i=0; i<m_numItems; i++) m_formItems[i].Clear();
}

TEXT_FORM::FORM_ITEM *TEXT_FORM::GetFormItem(int fieldName)
{
  FORM_ITEM *curform;
  for (int i=0; i<m_numItems; i++)
  {
    curform=m_formItems+i;
    if (fieldName==m_formItems[i].m_iFieldName)
      return (&m_formItems[i]);
  }
  return NULL;
}

void TEXT_FORM::SetXY(int fieldName, int x, int y)
{
  FORM_ITEM *curform = GetFormItem(fieldName);
  if (curform==NULL) return;
  curform->m_x = x;
  curform->m_y = y;
}


const RECT TEXT_FORM::SetText(
          int fieldName,
          const char *text,
          // prs 20191221  COLORREF color)
          FONT_COLOR_NUM colorNum)
{
  RECT rect={0,0,0,0};
  FORM_ITEM *curform, *relform;
  int relx, rely;
  int f;
  int i, j;
  for (i=0; i<m_numItems; i++)
  {
    curform=m_formItems+i;
    if (fieldName==curform->m_iFieldName)
    {
      relx=0;
      if (curform->m_xRelative & rightJust)
      {
        relx=-GraphicsMgr.GetTextWidth(text,-1, curform->m_highlight);
      }
      else if (curform->m_xRelative!=0)  //Is x relative to another field?
      {
        f=curform->m_xRelative & fieldNumMask; // The other field's name including color
        for (j=0; j<m_numItems; j++)  // Search for the other field.
        {
          relform=m_formItems+j;
          if (relform->m_iFieldName==f)
          { //We found the field x is relative to.
            ASSERT (relform->m_left>=0); //It better hava placement already.
            if (curform->m_xRelative&sel) 
            { //
              curform->m_top=relform->m_top;
              curform->m_left=relform->m_left;
            }
            else if (curform->m_xRelative&end) 
              relx=relform->m_right;
            else if (curform->m_xRelative&right)
              relx=relform->m_right-GraphicsMgr.GetTextWidth(text,-1, curform->m_highlight);
            else 
              relx=relform->m_left;
            break;
          };
        };
      };

      rely=0;
      if (curform->m_yRelative!=0) //Is y relative to another field?
      {
        f=curform->m_yRelative & fieldNumMask; //The other field's name including color
        for (j=0; j<m_numItems; j++)
        { //Search for the field we are relative to.
          relform=m_formItems+j;
          if (relform->m_iFieldName==f)
          {
            ASSERT (relform->m_top>=0); //The other field better have a place!
            if (curform->m_yRelative&sel) 
            {
              curform->m_bottom=relform->m_bottom;
              curform->m_right=relform->m_right;
            }
            else if (curform->m_yRelative & end) rely=relform->m_bottom;
            else rely=relform->m_top;
            break;
          };
        };
      };
      if ( (curform->m_xRelative & sel) ==0)
          curform->m_left=curform->m_x + relx;
      if ( (curform->m_yRelative & sel) ==0)
          curform->m_top=curform->m_y + rely;
// prs 20191221      if (color==0)
      if (colorNum == blackColor)
      {
        switch (curform->m_iFieldName&colorMask)
        {
          /* prs 20191221
        case white: color=White; break;
        case green: color=Green; break;
        default:
          color=White; */
        case white: colorNum = whiteColor; break;
        case green: colorNum = greenColor; break;
        default:
          colorNum = whiteColor;
        };
      };
      if (text!=NULL)
        curform->m_text=text;
      else
        curform->m_text="";
// prs 20191221      curform->m_color=color;
      curform->m_colorNum = colorNum;

      // make sure that highlight flags are set after clearing form
      curform->m_width = GraphicsMgr.GetTextWidth(text,-1, curform->m_highlight);
      if ( (curform->m_xRelative & sel) ==0)
          curform->m_right=curform->m_left+curform->m_width;
      if ( (curform->m_yRelative & sel) ==0)
          curform->m_bottom=curform->m_top+GraphicsMgr.GetCharacterHeight('H', curform->m_highlight);
      rect.top=curform->m_top;
      rect.bottom=curform->m_bottom;
      rect.left=curform->m_left;
      rect.right=curform->m_right;
      return rect;
    };
  };
  // An error here!
  return rect;
}

// make sure that highlight flags are set after clearing form
void TEXT_FORM::Highlight(int fieldName, bool highlight) 
{
  FORM_ITEM *curform = GetFormItem(fieldName);
  if (curform==NULL) return;
  curform->m_highlight=highlight;
}

void TEXT_FORM::EnableItem(int fieldName,bool enable)
{
  FORM_ITEM *curform = GetFormItem(fieldName);
  if (curform==NULL) return;
  curform->m_show = enable;
}

int TEXT_FORM::Tab(int oldSelection)
{
  int i;
  if (oldSelection==-1) i=0; // where to start
  else
  {
    for (i=0; i<m_numItems; i++)
    {
      if (m_formItems[i].m_iFieldName==oldSelection) break;
    };
    if (i==m_numItems) return -1; // Internal error
    i++;  // Where to start
  };
  while ( (m_formItems[i].m_iFieldName & tab) ==0)
  {
    i++;
    if (i==m_numItems)
    {
      if (oldSelection==-1) return -1;
      i=0;
    };
    if (m_formItems[i].m_iFieldName==oldSelection)
    {
      return oldSelection;  // There must be only one.
    };
  };
  return m_formItems[i].m_iFieldName;
}




int TEXT_FORM::MouseClick(int x, int y)
{
  // Returns the largest (in area) selection;
  int i;
  int selection=-1;
  int area=0, testarea;
  FORM_ITEM *f;
  for (i=0; i<m_numItems; i++)
  {
    f=m_formItems+i;
    if (x < f->m_left)   continue;
    if (x > f->m_right)  continue;
    if (y < f->m_top)    continue;
    if (y > f->m_bottom) continue;
    testarea=(f->m_right-f->m_left)*(f->m_bottom-f->m_top);
    if (testarea>area)
    {
      selection=f->m_iFieldName;
      area=testarea;
    };
  };
  return selection;
}

#define MAX_COLUMN 30

void TEXT_FORM::Display(int fontNumber)
{
  BOOL CurrFontTagSetting = GraphicsMgr.GetEnableFontColorTags();
  GraphicsMgr.EnableFontColorTags(FALSE);
  int i;
  FORM_ITEM *f;
  int columnWidths[MAX_COLUMN];
  int columnStarts[MAX_COLUMN];
  int columnSpaces[MAX_COLUMN];
  int columnEnds[MAX_COLUMN];
  int adjusts[MAX_COLUMN];
  int maxCol;
  memset (columnWidths, 0, sizeof(columnWidths));
  for (i=0; i<MAX_COLUMN; i++)
  {
    columnWidths[i] = 0;       // Widest width
    columnStarts[i] = 999999;  // Earliest start
    columnSpaces[i] = 0;       // Widest space
    columnEnds[i]   = 0;       // Rightmost column ending.
    adjusts[i]      = 0;       // How far right to push the column.
  };
  maxCol = 0;
  for (i=0; i<m_numItems; i++)
  {
    f=m_formItems+i;
    if ((f->m_column != 0) && (f->m_column < MAX_COLUMN))
    {
      if (f->m_column > maxCol)
      {
        maxCol = f->m_column;
      };
      if (f->m_width > columnWidths[f->m_column])
      {
        columnWidths[f->m_column] = f->m_width;
      };
      if (f->m_left < columnStarts[f->m_column])
      {
        columnStarts[f->m_column] = f->m_left;
      };
      if (f->m_right > columnEnds[f->m_column])
      {
        columnEnds[f->m_column] = f->m_right;
      };
      if (f->m_space > columnSpaces[f->m_column])
      {
        columnSpaces[f->m_column] = f->m_space;
      };
    };
  };
  // Adjust column starts to accomodate columnWidths.
  // Each column must start to the right of all preceeding columns.
  //
  // Compute adjustments.
  for (i=0; i<=maxCol; i++)
  {
    int j, adjust;
    adjusts[i] = 0;      // How far right to push the column.
    for (j=0; j<i; j++)
    {
      adjust = columnEnds[j] + columnSpaces[i] + adjusts[j] - columnStarts[i];
      if (adjust > adjusts[i]) adjusts[i] = adjust;
    };
  };
  for (i=0; i<m_numItems; i++)
  {
    f=m_formItems+i;
    if (f->m_text.GetLength()==0) continue;
//    if (f->m_color==0xffffffff) continue;
    if (f->m_colorNum == illegalColor) continue;
    if (!f->m_show) continue;
    DisplayText(f->m_left + adjusts[f->m_column],
                f->m_top,
                fontNumber,
                f->m_text,
// prs 20191221                f->m_color,
                f->m_colorNum, FALSE,
                f->m_highlight);
  };
  GraphicsMgr.EnableFontColorTags(CurrFontTagSetting);
}

int TEXT_FORM::FirstSelectable()
{
  for (int i=0; i<m_numItems; i++)
  {
    if (m_formItems[i].m_xRelative & sel)
      return m_formItems[i].m_iFieldName;
  }
  return -1;
}


LINE_TEXT_LIST lineListText;


void LINE_TEXT_LIST::Clear()
{
  title = "";
  m_flags = 0;
  lines.RemoveAll();
}

const char *LINE_TEXT_LIST::GetLineText(int i)
{
  int col;
  static CString result;
  col = lines[i].Find('|');
  result = lines[i].Left(col);
  if (col < 0) return lines[i];
  return result;
}

const char *LINE_TEXT_LIST::GetLineLine(int i)
{
  return lines[i];
}


enum ST_LISTFORM { // Define field names as integers for quicker comparisons
  STLF_none,
  STLF_Title=TEXT_FORM::white,
  STLF_Line,
 // STSF_white=TEXT_FORM::white,    // White items  
//
  //STSF_LEVEL,
  //STSF_SELECT,
  //STSF_MEMORIZE,
  //STSF_COST,
  //STSF_NAME, 

  //STSF_MUAVAIL,
  //STSF_muavail,
  //STSF_CLERICAVAIL,
  //STSF_clericavail,
  //STSF_THIEFAVAIL,
  //STSF_thiefavail,
  //STSF_FIGHTERAVAIL,
  //STSF_fighteravail,
  //STSF_PALADINAVAIL,
  //STSF_paladinavail,
  //STSF_RANGERAVAIL,
  //STSF_rangeravail,
  //STSF_DRUIDAVAIL,  
  //STSF_druidavail,
  //STSF_TEXT1,
  //STSF_TEXT2,
  //STSF_TEXT3,
  //STSF_TEXT4,
  //STSF_TEXT5,

  //STSF_level,
  //STSF_select,
  //STSF_memorize,
  //STSF_cost,
  //STSF_name,

  STLF_REPEAT,
  STLF_repeat=TEXT_FORM::autorepeat
};



static const int pagesize=Items_Per_Page;

static const int titleX = 18;
static const int titleY = 18;
static const int lineRelX = 18;

//static const int itemsY=36;


#define STLF(item,xrel,x,yrel,y) \
  DISPLAY_FORM(STLF_##xrel,STLF_##yrel,STLF_##item,x,y),



DISPLAY_FORM lineForm[] =
{
  //
  //   enum      xrel            x          yrel       y
  // 
  STLF(Title    ,none            ,titleX    ,none     ,titleY)  // Page title
  //STSF(NAME     ,LEVEL+END       ,40        ,none     ,labelsY)  // "SPELL"

  STLF(REPEAT   ,repeat          ,1         ,none     ,pagesize)
  STLF(Line     ,Title           ,lineRelX  ,Title+END,0)
  // TEXTBOX_WIDTH varies at runtime depending on
  // current resolution mode (640,800,1024).
  //
  //STSF(TEXT1        ,none          ,0         ,none             ,0)
  //STSF(TEXT2        ,TEXT1         ,0         ,TEXT1+END        ,0)
  //STSF(TEXT3        ,TEXT1         ,0         ,TEXT2+END        ,0)
  //STSF(TEXT4        ,TEXT1         ,0         ,TEXT3+END        ,0)
  //STSF(TEXT5        ,TEXT1         ,0         ,TEXT4+END        ,0)

  //STSF(REPEAT   ,repeat         ,5       ,none     ,pagesize) // auto repeat the rest

  //STSF(level    ,LEVEL+RIGHT    ,0       ,none     ,itemsY)   // "1"
  //STSF(select   ,SELECT+RIGHT   ,0       ,none     ,itemsY)   // "Yes"
  //STSF(memorize ,MEMORIZE+RIGHT ,0       ,none     ,itemsY)   // "1"
  //STSF(cost     ,COST+RIGHT     ,0       ,none     ,itemsY)   // "100"  
  //STSF(name     ,NAME           ,0       ,none     ,itemsY)   // "Cure Light Wounds"
  
  STLF(none,none,0,none,0)  // End of list

};








void showLines(TEXT_FORM *pForm, LINE_TEXT_LIST &lineTextList)
{ 
   ASSERT(pForm!=NULL);

   // make sure that highlight flags are set after clearing form
   //pForm->ClearForm();
   
   memset(InventoryRects, 0, sizeof(InventoryRects));
   int ir_idx = 0;   

   if (lineTextList.IsSimpleLayout())
   {
	    pForm->SetText( STLF_Title,lineTextList.title);
   };
   // blank out all repeat items
   int EnumOffset = 0;
   for (int i=0;i<pagesize;i++)
   {
      pForm->SetText(STLF_Line+EnumOffset,    "");
      EnumOffset += TEXT_FORM::repeatIncr;
   }


   int totalListItems;
   totalListItems = lineTextList.GetCount();
   int currItem = startItem;
   int displaycount = 0;
   CRect tmpRECT,totalRECT;
   EnumOffset = 0;;

   while ((currItem<totalListItems) && (displaycount<pagesize))
   {      

      {
        tmpRECT = pForm->SetText(STLF_Line+EnumOffset,  lineTextList.GetLineText(currItem));
      };

      /* prs 20191221
      {
        COLORREF color;
        color = White;
        //color = Green;
        //Yellow;
      };
      */

      pForm->Highlight(STLF_Line+EnumOffset, shouldHighlight(currItem)?true:false);
      
      CopyRect(&InventoryRects[ir_idx], &totalRECT);

      ir_idx++;
      currItem++;
      displaycount++;
      EnumOffset += TEXT_FORM::repeatIncr;
   }
}


int handleLineFormInput(LINE_FORM_INPUT_MESSAGE_TYPE msg,
                        LINE_TEXT_LIST &data,
                        int P1, 
                        int P2)
// Returns 0x0001 if you should display menu and flip surfaces.
// Returns 0x0002 if we handled a keystroke and you can ignore it
{
  TEXT_FORM LineForm(lineForm);
  int result=0;
  bool flip=false, redraw=false;

  switch (msg)
  {
  case LINEFORM_initial:
    redraw=true;
    break;
  case LINEFORM_refresh:
    redraw=true;
    break;
  case LINEFORM_exit:
    break;
  default:
    ASSERT(0);
    return 0;  // Internal error
  };
  if (redraw)
  {     
    LineForm.ClearForm();
    showLines(&LineForm, data);  // To define rectangles.
    flip=true;
  };
  if (flip)
  {
    LineForm.Display(0);
  };
  return result|(flip?1:0);
}


