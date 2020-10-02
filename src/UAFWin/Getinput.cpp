/******************************************************************************
* Filename: Getinput.cpp
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
//#include "Dungeon.h"
#include "GetInput.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void gpdlCleanup(void);
extern int debugSeverity;
void WriteDebugString(const char *ptext, ... );
extern int ExitSignaled;


/*
char *GetDxInputError(HRESULT rc)
{
  switch (rc)
  {
  //case DI_BUFFEROVERFLOW:
  //case DI_NOEFFECT :
  //case DI_PROPNOEFFECT :
  case DI_NOTATTACHED :
    return "The device exists but is not currently attached. This value is equal to the S_FALSE standard COM return value. ";
      break;
  case DI_DOWNLOADSKIPPED :
    return "The parameters of the effect were successfully updated, but the effect could not be downloaded because the associated device was not acquired in exclusive mode. ";
      break;
  case DI_EFFECTRESTARTED :
    return "The effect was stopped, the parameters were updated, and the effect was restarted. ";
    break;    
  case DI_OK :
    return "The operation completed successfully. This value is equal to the S_OK standard COM return value. ";
    break;
  case DI_POLLEDDEVICE :
    return "The device is a polled device. As a result, device buffering does not collect any data and event notifications is not signaled until the IDirectInputDevice7::Poll method is called. ";
    break;
  case DI_TRUNCATED :
    return "The parameters of the effect were successfully updated, but some of them were beyond the capabilities of the device and were truncated to the nearest supported value. ";
    break;
  case DI_TRUNCATEDANDRESTARTED :
    return "Equal to DI_EFFECTRESTARTED | DI_TRUNCATED. ";
    break;
  case DIERR_ACQUIRED :
    return "The operation cannot be performed while the device is acquired. ";
    break;
  case DIERR_ALREADYINITIALIZED :
    return "This object is already initialized ";
    break;
  case DIERR_BADDRIVERVER :
    return "The object could not be created due to an incompatible driver version or mismatched or incomplete driver components. ";
    break;
  case DIERR_BETADIRECTINPUTVERSION :
    return "The application was written for an unsupported prerelease version of DirectInput. ";
    break;
  case DIERR_DEVICEFULL :
    return "The device is full. ";
    break;
  case DIERR_DEVICENOTREG: 
    return "The device or device instance is not registered with DirectInput. This value is equal to the REGDB_E_CLASSNOTREG standard COM return value. ";
    break;
  case DIERR_EFFECTPLAYING: 
    return "The parameters were updated in memory but were not downloaded to the device because the device does not support updating an effect while it is still playing. ";
    break;
  case DIERR_HASEFFECTS :
    return "The device cannot be reinitialized because there are still effects attached to it. ";
    break;
  case DIERR_GENERIC :
    return "An undetermined error occurred inside the DirectInput subsystem. This value is equal to the E_FAIL standard COM return value. ";
    break;
  //case DIERR_HANDLEEXISTS :
  //  return "The device already has an event notification associated with it. This value is equal to the E_ACCESSDENIED standard COM return value. ";
  //  break;
  case DIERR_INCOMPLETEEFFECT :
    return "The effect could not be downloaded because essential information is missing. For example, no axes have been associated with the effect, or no type-specific information has been supplied. ";
    break;
  case DIERR_INPUTLOST :
    return "Access to the input device has been lost. It must be reacquired. ";
    break;
  case DIERR_INVALIDPARAM :
    return "An invalid parameter was passed to the returning function, or the object was not in a state that permitted the function to be called. This value is equal to the E_INVALIDARG standard COM return value. ";
    break;
  case DIERR_MOREDATA :
    return "Not all the requested information fit into the buffer. ";
    break;
  case DIERR_NOAGGREGATION :
    return "This object does not support aggregation. ";
    break;
  case DIERR_NOINTERFACE :
    return "The specified interface is not supported by the object. This value is equal to the E_NOINTERFACE standard COM return value. ";
    break;
  case DIERR_NOTACQUIRED :
    return "The operation cannot be performed unless the device is acquired. ";
    break;
  case DIERR_NOTBUFFERED :
    return "The device is not buffered. Set the DIPROP_BUFFERSIZE property to enable buffering. ";
    break;
  case DIERR_NOTDOWNLOADED: 
    return "The effect is not downloaded. ";
    break;
  case DIERR_NOTEXCLUSIVEACQUIRED :
    return "The operation cannot be performed unless the device is acquired in DISCL_EXCLUSIVE mode. ";
    break;
  case DIERR_NOTFOUND :
    return "The requested object does not exist. ";
    break;
  case DIERR_NOTINITIALIZED :
    return "This object has not been initialized. ";
    break;
  //case DIERR_OBJECTNOTFOUND :
  //  return "The requested object does not exist. ";
  //  break;
  case DIERR_OLDDIRECTINPUTVERSION :
    return "The application requires a newer version of DirectInput. ";
    break;
  case DIERR_OTHERAPPHASPRIO :
    return "Another application has a higher priority level, preventing this call from succeeding. This value is equal to the E_ACCESSDENIED standard COM return value. This error can be returned when an application has only foreground access to a device but is attempting to acquire the device while in the background. ";
    break;
  case DIERR_OUTOFMEMORY :
    return "The DirectInput subsystem could not allocate sufficient memory to complete the call. This value is equal to the E_OUTOFMEMORY standard COM return value. ";
    break;
  //case DIERR_READONLY :
  //  return "The specified property cannot be changed. This value is equal to the E_ACCESSDENIED standard COM return value. ";
  //  break;
  case DIERR_REPORTFULL: 
    return "More information was requested to be sent than can be sent to the device. ";
    break;
  case DIERR_UNPLUGGED :
    return "The operation could not be completed because the device is not plugged in. ";
    break;
  case DIERR_UNSUPPORTED: 
    return "The function called is not supported at this time. This value is equal to th E_NOTIMPL standard COM return value. ";
    break;
  case E_HANDLE :
    return "The HWND parameter is not a valid top-level window that belongs to the process. ";
    break;
  case E_PENDING :
    return "Data is not yet available. ";
    break;
  }
  return "Unknown DirectInput error code";
}
*/

BOOL INPUT_DATA::IsUser()
{
  return (key.flags & USER_INPUT);
}

BOOL INPUT_DATA::IsSimulated()
{
  return (key.flags & SIMULATED_INPUT);
}

//*****************************************************************************
// NAME:    CInput::CInput
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
CInput::CInput(HWND hwnd)
{
  memset(&m_input, 0, sizeof(m_input));   
	ClearInput();
	memset(m_keys, 0, sizeof(m_keys));
	key_pressed = 0;
	m_TimerExpired = FALSE;
  m_TimerId=0;
	m_ForceInput = FALSE;
  m_AllowInput = TRUE;
  m_MouseInput = FALSE;
  m_MouseButton1 = FALSE;
  m_MouseButton2 = FALSE;
  m_Mx = 320;
  m_My = 240;
  IsInputFromSelf = FALSE;
  InputFromSelfKey = 0;
}


CInput::~CInput() { }

//*****************************************************************************
// NAME:    CInput::IsInput
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
BOOL CInput::IsInput()
{
  int input = (   (m_input.key.code != KC_NONE)
		           || (m_input.timer_expired)
		           || (m_input.force_input)
		           || (m_MouseInput)
               || (m_input.clockTick)
              );
  m_MouseInput = FALSE;
  return input;
}

//*****************************************************************************
// NAME:    CInput::GetInput
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
BOOL CInput::GetInput(INPUT_DATA& input)
{   
	ClearInput();
  if (CheckExitKey()) return TRUE;
  GetMouse();

  if (m_TimerExpired)
  {
	  m_TimerExpired = FALSE;
	  m_input.timer_expired = TRUE;
    m_input.timerId = m_TimerId;
    m_input.key.code = KC_TIMER;
    memcpy(&input, &m_input, sizeof(INPUT_DATA));
    return TRUE;
  }

  if (m_ForceInput)
  {
	  m_ForceInput = FALSE;
	  m_input.force_input = TRUE;
    memcpy(&input, &m_input, sizeof(INPUT_DATA));
    return TRUE;
  }
 
	GetKeyboard();
  GetClockTick();
	memcpy(&input, &m_input, sizeof(INPUT_DATA));
  
	return (IsInput());
}

//*****************************************************************************
// NAME:    CInput::ClearInput
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
void CInput::ClearInput(void)
{
  int x=m_input.mouse.x;
  int y=m_input.mouse.y;
	memset(&m_input, 0, sizeof(m_input));   
  m_input.mouse.x=x;
  m_input.mouse.y=y;
  m_input.clockTick = FALSE;
}

BOOL CInput::CheckExitKey()
{
  if ((key_pressed) && (m_keys[VK_F11]))
  {
		m_input.key.code = KC_FC11;
    WriteDebugString("Exit Game key pressed\n");
 		ExitSignaled = 1;
    //gpdlCleanup();
    return TRUE;
  }
  return FALSE;
}

//*****************************************************************************
// NAME:    CInput::GetKeyboard
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
void CInput::GetKeyboard() 
{
  if (IsInputFromSelf)
  {
    m_input.key.flags = SIMULATED_INPUT;
    LookupKeyCode(InputFromSelfKey, FALSE);
    IsInputFromSelf = FALSE;
	  memset(m_keys, 0, sizeof(m_keys));
	  key_pressed = 0;
    return;
  }

  m_input.key.code = KC_NONE;
	m_input.key.key = 0;
  m_input.key.flags = 0;

  if (!key_pressed)
	 return;

  if (!m_AllowInput)
  {
    // a special case
    if (m_keys[VK_SPACE]) // to toggle combatants on automatic control
      LookupKeyCode(VK_SPACE, FALSE);

	  memset(m_keys, 0, sizeof(m_keys));
	  key_pressed = 0;
    return;
  }

	UINT i = 1;
	BOOL found = FALSE;

	while ((i <= MAX_VKEY) && (!found))
	{
		if (m_keys[i])
      found = LookupKeyCode(i, (m_flags[i] & SHIFT_KEY));
		i++;
	}

	memset(m_keys, 0, sizeof(m_keys));
	key_pressed = 0;
}

#define VK_slash 0xbf

BOOL CInput::LookupKeyCode(int vkey, BOOL shifted)
{
  BOOL valid=TRUE;
  m_input.key.vkey = vkey;
	switch (vkey) 
  {
  case VK_TAB:
    m_input.key.code=KC_TAB;
    break;
	case VK_ESCAPE:
		m_input.key.code = KC_ESCAPE;
		//ExitSignaled = 1;
    //WriteDebugString("Exit Game key pressed\n");
		break;
  case VK_NUMPAD5:
    m_input.key.code = KC_CENTER;
    break;
  case VK_NUMPAD7:
  case VK_HOME:
    m_input.key.code = KC_NW;
    break;
  case VK_NUMPAD9:
  case VK_PRIOR:
    m_input.key.code = KC_NE;
    break;
  case VK_NUMPAD3:
  case VK_NEXT:
    m_input.key.code = KC_SE;
    break;
  case VK_NUMPAD1:
  case VK_END:
    m_input.key.code = KC_SW;
    break;
  case VK_NUMPAD0:
  case VK_INSERT:
    m_input.key.code = KC_POSITION;
    break;
	case VK_NUMPAD4:
	case VK_LEFT:
	  m_input.key.code = KC_LEFT; 
		break;
	case VK_NUMPAD6:
	case VK_RIGHT:
	  m_input.key.code = KC_RIGHT;
		break;
	case VK_NUMPAD8:
	case VK_UP:
	  m_input.key.code = KC_UP; 
		break;
	case VK_NUMPAD2:
	case VK_DOWN:
	  m_input.key.code = KC_DOWN; 
		break;
	case VK_SPACE:
		m_input.key.code = KC_SPACE; 
    m_input.key.key = vkey;
		break;
	case VK_RETURN:
		m_input.key.code = KC_RETURN; 
		break;
	case VK_BACK:
	case VK_DELETE:
		m_input.key.code = KC_BACK; 
		break;
  // although the documentation lists VK_A-VK_Z and VK_0-VK_9,
  // they are not defined and the compiler won't accept them. That's
  // why we use 'A' rather than VK_A.
	case 'A':
	case 'B':
	case 'C':
	case 'D':
	case 'E':
	case 'F':
	case 'G':
	case 'H':
	case 'I':
	case 'J':
	case 'K':
	case 'L':
	case 'M':
	case 'N':
	case 'O':
	case 'P':
	case 'Q':
	case 'R':
	case 'S':
	case 'T':
	case 'U':
	case 'V':
	case 'W':
	case 'X':
	case 'Y':
	case 'Z':
		m_input.key.code = KC_CHAR;
		m_input.key.key = vkey;
    if (!shifted)
      m_input.key.key = _tolower(m_input.key.key);
		break;

  // see above on '0' vs VK_0
	case '0':
    if (shifted)
    {
      m_input.key.code = KC_PUNCTUATION;
		  m_input.key.key = ')';
    }
    else
    {
      m_input.key.code = KC_NUM;
		  m_input.key.key = vkey;
    };
		break;
	case '1':
    if (shifted)
    {
      m_input.key.code = KC_PUNCTUATION;
		  m_input.key.key = '!';
    }
    else
    {
      m_input.key.code = KC_NUM;
		  m_input.key.key = vkey;
    };
		break;
	case '2':
    if (shifted)
    {
      m_input.key.code = KC_PUNCTUATION;
		  m_input.key.key = '@';
    }
    else
    {
      m_input.key.code = KC_NUM;
		  m_input.key.key = vkey;
    };
		break;
	case '3':
    if (shifted)
    {
      m_input.key.code = KC_PUNCTUATION;
		  m_input.key.key = '#';
    }
    else
    {
      m_input.key.code = KC_NUM;
		  m_input.key.key = vkey;
    };
		break;
	case '4':
    if (shifted)
    {
      m_input.key.code = KC_PUNCTUATION;
		  m_input.key.key = '$';
    }
    else
    {
      m_input.key.code = KC_NUM;
		  m_input.key.key = vkey;
    };
		break;
	case '5':
    if (shifted)
    {
      m_input.key.code = KC_PUNCTUATION;
		  m_input.key.key = '%';
    }
    else
    {
      m_input.key.code = KC_NUM;
		  m_input.key.key = vkey;
    };
		break;
	case '6':
    if (shifted)
    {
      m_input.key.code = KC_PUNCTUATION;
		  m_input.key.key = '^';
    }
    else
    {
      m_input.key.code = KC_NUM;
		  m_input.key.key = vkey;
    };
		break;
	case '7':
    if (shifted)
    {
      m_input.key.code = KC_PUNCTUATION;
		  m_input.key.key = '&';
    }
    else
    {
      m_input.key.code = KC_NUM;
		  m_input.key.key = vkey;
    };
		break;
	case '8':
    if (shifted)
    {
      m_input.key.code = KC_PUNCTUATION;
		  m_input.key.key = '*';
    }
    else
    {
      m_input.key.code = KC_NUM;
		  m_input.key.key = vkey;
    };
		break;
	case '9':
    if (shifted)
    {
      m_input.key.code = KC_PUNCTUATION;
		  m_input.key.key = '(';
    }
    else
    {
      m_input.key.code = KC_NUM;
		  m_input.key.key = vkey;
    };
		break;
  case VK_ADD:
    m_input.key.code=KC_PLUS;
    break;
  case VK_SUBTRACT:
    m_input.key.code=KC_MINUS;
    break;
  case VK_SHIFT:
  case VK_CONTROL:
  case VK_MENU:
    break;
  case VK_F1:
    m_input.key.code=KC_FC1;
    break;
  case VK_F2:
    m_input.key.code=KC_FC2;
    break;
  case VK_F3:
    m_input.key.code=KC_FC3;
    break;
  case VK_F4:
    m_input.key.code=KC_FC4;
    break;
  case VK_F5:
    m_input.key.code=KC_FC5;
    break;
  case VK_F6:
    m_input.key.code=KC_FC6;
    break;
  case VK_F7:
    m_input.key.code=KC_FC7;
    break;
  case VK_F8:
    m_input.key.code=KC_FC8;
    break;
  case VK_F9:
    m_input.key.code=KC_FC9;
    break;
  case VK_F10:
    m_input.key.code=KC_FC10;
    break;
  case VK_F11:
    m_input.key.code=KC_FC11;
    break;
  case VK_F12:
    m_input.key.code=KC_FC12;
    break;
  case 0xde:
    if (shifted)
    {
      m_input.key.code = KC_PUNCTUATION;
		  m_input.key.key = '"';
    }
    else
    {
      m_input.key.code = KC_PUNCTUATION;
		  m_input.key.key = '\'';
    };
		break;    
  case 0xbd: 
      m_input.key.code = KC_PUNCTUATION;
      if (shifted)
      {
        m_input.key.key = '_';
      }
      else
      {
        m_input.key.key = '-';
      };
      break;
  case VK_slash: // Defined locally PRS 20140308
      m_input.key.code = KC_PUNCTUATION;
      if (shifted)
      {
        m_input.key.key = '?';
      }
      else
      {
        m_input.key.key = '/';
      };
      break;
  default:		
    // always returns unshifted char
		m_input.key.key = (MapVirtualKey(vkey, 2) & 0x7F);
    if (m_input.key.key != 0)
    {
      m_input.key.code = KC_PUNCTUATION;
    }
    else
    {
      valid=FALSE;
      WriteDebugString("Unhandled VKEY %u in LookupKeyCode()\n", vkey);
    }
    break;
	}
  return valid;
}

//*****************************************************************************
// NAME:    CInput::GetMouse
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
void CInput::GetMouse()
{
  if ((!m_MouseInput) || (!m_AllowInput))
  {
    m_input.mouse.button1 = FALSE;
    m_input.mouse.button2 = FALSE;
    m_input.mouse.mouseMovement = FALSE;
    m_input.mouse.x = m_Mx;
    m_input.mouse.y = m_My;
    //m_Mx = 0;
    //m_My = 0;
  }
  else
  {
    m_input.mouse.button1 = m_MouseButton1;
    m_input.mouse.button2 = m_MouseButton2;
    m_input.mouse.mouseMovement = m_MouseMove;
    m_input.mouse.x = m_Mx;
    m_input.mouse.y = m_My;
    m_MouseButton1 = FALSE;
    m_MouseButton2 = FALSE;
    m_MouseMove    = FALSE;
  }
}


void CInput::GetClockTick(void)
{
  if (m_clockTick)
  {
    m_input.clockTick = TRUE;
    m_clockTick = FALSE;
  };
}

BOOL CInput::AllowKeyCodeWhenInputDisabled(int vkey)
{
  return (   (vkey==VK_ESCAPE) 
          || (vkey==VK_SPACE));
}

//*****************************************************************************
// NAME:    CInput::KeyPressed
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
void CInput::KeyPressed(UINT vkey)
{
  int flags;

  flags = (vkey >> 8) & 0xff;
  vkey &= 0xff;
  if (!m_AllowInput)
  {
    if (AllowKeyCodeWhenInputDisabled(vkey))
    {
      m_keys[vkey] = 1;
      m_flags[vkey] = flags;
      key_pressed = TRUE;
    }
    else
      WriteDebugString("Key %u pressed, but no input allowed\n", vkey);
    return;
  }

	if ((vkey > 0) && (vkey <= MAX_VKEY))
	{
		m_keys[vkey] = 1;
    m_flags[vkey] = flags;
    key_pressed = TRUE;
	}
}

void CInput::KeySimulate(UINT vkey)
{
	if ((vkey > 0) && (vkey <= MAX_VKEY))
	{
		InputFromSelfKey = vkey;
    IsInputFromSelf = TRUE;
	}
}

void CInput::SetMouseLeftClick(long x, long y)
{
  if (!m_AllowInput)
    return;

  m_Mx = x;
  m_My = y;
  m_MouseButton1 = TRUE;
  m_MouseInput = TRUE;
}

void CInput::SetMouseMove(long x, long y)
{
  m_MouseButton1 = FALSE;
  m_MouseButton2 = FALSE;
  if ( (m_Mx == x) && (m_My == y) )
  {
    m_MouseMove    = FALSE;
    m_MouseInput = FALSE;
    return;
  };
  m_Mx = x;
  m_My = y;
  m_MouseMove    = TRUE;
  m_MouseInput = TRUE;
}

void CInput::SetClockTick(void)
{
  m_clockTick = TRUE;
}

void CInput::AllowInput(BOOL allow) 
{
  //if (!allow)
  //{
  //  int kkk=1;
  //};
  m_AllowInput = allow; 

  if (!m_AllowInput)
  {
    // clear out residual
	  memset(m_keys, 0, sizeof(m_keys));
	  key_pressed = 0;
  }
}

void CInput::ClearUserInput()
{
  //m_Mx = 0;
  //m_My = 0;
  m_MouseInput = FALSE;
  m_MouseButton1 = FALSE;
  m_MouseButton2 = FALSE;

	memset(m_keys, 0, sizeof(m_keys));
	key_pressed = 0;

	InputFromSelfKey = 0;
  IsInputFromSelf = FALSE;
}


BOOL IsMovementKey(key_code key)
{
  return (   (key == KC_LEFT)
          || (key == KC_RIGHT)
          || (key == KC_UP)
          || (key == KC_DOWN)
          || (key == KC_NW)
          || (key == KC_NE)
          || (key == KC_SE)
          || (key == KC_SW)
          || (key == KC_CENTER)
          || (key == KC_POSITION));
}
