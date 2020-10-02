/******************************************************************************
* Filename: TextForm.h
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
#ifndef __TEXTFORM_H__
#define __TEXTFORM_H__

#include "externs.h"

struct DISPLAY_FORM
{
  DISPLAY_FORM() { memset(this, 0, sizeof(DISPLAY_FORM)); }

  DISPLAY_FORM(int xrel, int yrel, __int32 fname,  __int16 xpos, __int16 ypos)
  {
    iFieldName=fname;
    x_relative=xrel;
    x=xpos;
    y_relative=yrel;
    y=ypos;
    column = 0;
    space = 0;
  }
  DISPLAY_FORM(int xrel, int yrel, __int32 fname,  __int16 xpos, __int16 ypos, __int16 c, __int16 s)
  {
    iFieldName=fname;
    x_relative=xrel;
    x=xpos;
    y_relative=yrel;
    y=ypos;
    column = c;
    space = s;
  }

  int x_relative;  // Relative to right side
  int y_relative;  // Relative to bottom
  __int32 iFieldName;   
  __int16 x; 
  __int16 y;
  __int16 left;    // Filled in when displayed
  __int16 right;   // Filled in when displayed
  __int16 top;     // Filled in when displayed
  __int16 bottom;  // Filled in when displayed
  __int16 column;  // column numbers...we allocate room for longest entry
  __int16 space;   // Space in front of column
};


class TEXT_FORM
{
  class FORM_ITEM
  {
  public:
    FORM_ITEM(void) {Clear();};
    ~FORM_ITEM(void){};
    void Clear(void);
    void operator =(const DISPLAY_FORM& src) 
    {
      // Only used one place.  Might as well define it here.
      Clear();
      m_xRelative=src.x_relative;
      m_yRelative=src.y_relative;
      m_x=src.x;
      m_y=src.y;
      m_iFieldName=src.iFieldName;
      m_column = src.column;
      m_space = src.space;
    };
    int m_xRelative;  // Relative to right side
    int m_yRelative;  // Relative to bottom
    __int32 m_iFieldName;   
    __int16 m_x; 
    __int16 m_y;
    __int16 m_left;    // Filled in when displayed
    __int16 m_right;   // Filled in when displayed
    __int16 m_top;     // Filled in when displayed
    __int16 m_bottom;  // Filled in when displayed
    __int16 m_column;
    __int16 m_space;
    __int16 m_width;
    CString m_text;
// prs 20191221    COLORREF m_color;
    FONT_COLOR_NUM m_colorNum;
    bool   m_highlight;
    bool   m_show;
  };

private:
  FORM_ITEM *m_formItems;
  int m_numItems;
  TEXT_FORM(); // no implementation
  TEXT_FORM& operator=(const TEXT_FORM& src); // no implementation
  TEXT_FORM(const TEXT_FORM& src); // no implementation
  FORM_ITEM *GetFormItem(int fieldName);

public:
  enum flags 
  { 
    tab         =0x40000000,    
    white       =0x10000000,
    green       =0x20000000,    
    end         =0x01000000,    
    sel         =0x02000000,
    autorepeat  =0x03000000,
    right       =0x04000000,
    rightJust   =0x08000000,
    repeatIncr  =0x00010000,
    colorMask   =0x30000000,
    fieldNumMask=0x30ffffff,  // Includes the color!
  };

  TEXT_FORM( DISPLAY_FORM *src );
  ~TEXT_FORM (void);
  void ClearForm(void);
// prs 20191221  const RECT SetText(int fieldName, const char *text, COLORREF color=0);  
  const RECT SetText(int fieldName, const char* text, FONT_COLOR_NUM colorNum = blackColor);
  void Highlight(int fieldName, bool highlight=true);
  void Display(int fontNumber);
  void EnableItem(int fieldName,bool enable=true);
  int MouseClick(int x, int y); // Find selected item
  int Tab(int oldSelection); // Go to next tab stop
  int NumItems() const { return m_numItems; }
  int FirstSelectable();
  void SetXY(int fieldName, int x, int y);
};

#define END       (TEXT_FORM::end)
#define SEL       (TEXT_FORM::sel)
#define REPEAT    (TEXT_FORM::autorepeat)
#define RIGHT     (TEXT_FORM::right)
#define RIGHTJUST (TEXT_FORM::rightJust)

#endif
