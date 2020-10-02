/******************************************************************************
* Filename: Getinput.h
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
#ifndef __GETINPUT_H__
#define __GETINPUT_H__

//#include "externs.h"
//#define NTDX3
//#include "CDX.h"
//#include "cdxinput.h"

#define MAX_VKEY 255

enum key_code {
	KC_NONE,
	KC_CHAR,
	KC_NUM,
	KC_LEFT,
	KC_RIGHT,
	KC_UP,
	KC_DOWN,
  KC_NW,   // num pad northwest - 7 key
  KC_NE,   // northeast - 9
  KC_SE,   // southeast - 3
  KC_SW,   // southwest - 1
  KC_CENTER,
  KC_POSITION,
	KC_SPACE,
	KC_RETURN,
	KC_BACK,
	KC_ESCAPE,
  KC_TAB,
  KC_PLUS,
  KC_MINUS,
  KC_TIMER,
  KC_PUNCTUATION, // keys like ,.;-=
  KC_FC1,
  KC_FC2,
  KC_FC3,
  KC_FC4,
  KC_FC5,
  KC_FC6,
  KC_FC7,
  KC_FC8,
  KC_FC9,
  KC_FC10,
  KC_FC11,
  KC_FC12
};

#define SHIFT_KEY 1
#define ALT_KEY   2
#define CTRL_KEY  4
#define USER_INPUT 8
#define SIMULATED_INPUT 16

struct KEY_DATA 
{
	key_code code;
	int key;
  int vkey;
  SHORT flags;
};

struct MOUSE_DATA {
	long x,y;
	BOOL button1;
	BOOL button2;
  BOOL mouseMovement;
};

struct INPUT_DATA 
{
  BOOL IsUser();
  BOOL IsSimulated();

	BOOL timer_expired;
  UINT timerId;
	BOOL force_input;

	KEY_DATA key;
  MOUSE_DATA mouse;

  BOOL clockTick;
};

class CInput
{
private:
    CInput(); // not used

		void GetKeyboard();
		void GetMouse();
    void GetClockTick(void);
    BOOL IsInput();
    BOOL CheckExitKey();
    BOOL LookupKeyCode(int vkey, BOOL shifted);
    BOOL AllowKeyCodeWhenInputDisabled(int vkey);

    BOOL IsInputFromSelf;
    int  InputFromSelfKey;

    BOOL m_AllowInput;
    BOOL key_pressed;
    BOOL m_MouseInput;
    BOOL m_MouseButton1;
    BOOL m_MouseButton2;
    BOOL m_MouseMove;
    BOOL m_clockTick;

		BOOL m_TimerExpired;
    UINT m_TimerId;
		BOOL m_ForceInput;

    long m_Mx;
    long m_My;

		BYTE m_keys[MAX_VKEY+1];
    BYTE m_flags[MAX_VKEY+1];

    INPUT_DATA m_input;

	public:
		CInput(HWND hwnd);
    ~CInput();

    // gets keyboard and mouse data
		BOOL GetInput(INPUT_DATA& input);

    // clears all input data
		void ClearInput();
    void ClearUserInput();

    // forces the specified key to be pressed (simulated)
		void KeyPressed(UINT vkey);
    void KeySimulate(UINT vkey);
    void SetClockTick(void);

		void TimerExpired(UINT id) { m_TimerExpired = TRUE; m_TimerId=id; }
		void ForceInput() { m_ForceInput = TRUE; }
    void AllowInput(BOOL allow);
    void SetMouseLeftClick(long x, long y);
    void SetMouseMove(long x, long y);
};

BOOL IsMovementKey(key_code key);

#endif
