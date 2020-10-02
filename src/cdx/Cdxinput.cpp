//////////////////////////////////////////////////////////////////////////////////
// $Source: /cvsroot/uaf/UAF/src/cdx/Cdxinput.cpp,v $
// $Author: stevens-madison $
//
// $Log: Cdxinput.cpp,v $
// Revision 1.7  2015/08/27 19:34:05  stevens-madison
// Update to Visual Studio 2015 Community
//
// Revision 1.6  2013/12/16 01:35:59  cocoaspud
// fixed a few issues pointed out by static analysis tool
//
// Revision 1.5  2002/03/02 13:37:08  cocoaspud
// Completed menu updates for char creation menus
//
// Revision 1.16  2002/01/02 01:39:32  wasteland
// Fixed an initialization bug in the joystick code
//
// Revision 1.15  2002/01/01 23:42:25  wasteland
// Added support for multiple joysticks
//
// Revision 1.14  2001/11/02 12:06:06  bsimser
// Fixed stupid bug in CDXInput Create method.
//
// Revision 1.13  2001/11/01 01:04:04  bsimser
// Added default position for mouse in Create method based on current cursor position.
//
// Revision 1.12  2001/01/26 19:55:47  mindcry
// Finally CDX got a way(RawToAscii) for real scancode to character converting.
//
// Revision 1.11  2000/12/17 22:14:10  mindcry
// Fixed the problem with CDX Appz crashing when the window looses the
// focus (normaly with clicking other apps or icons on desktop)
//
// Revision 1.10  2000/12/04 19:58:44  wasteland
// Added scroll mouse functionality, created by Jacek Ringwelski
//
// Revision 1.9  2000/11/18 18:20:08  mindcry
// Minor speed optimization
//
// Revision 1.8  2000/08/18 13:42:51  mindcry
// All 'Exculsive' typos have been fixed to 'Exclusive'
// Thanks to all reporters of this
//
// Revision 1.7  2000/05/10 23:29:26  hebertjo
// Cleaned up all the old code that was commented out.  Version 3.0 is getting
// very close to release.
//
// Revision 1.6  2000/05/09 01:05:34  hebertjo
// Changed the way SetMouseAccelerator() works.
//
// Revision 1.5  2000/05/08 20:04:30  hebertjo
// Fixed an issue with NT compiling.
//
// Revision 1.4  2000/05/07 20:00:35  hebertjo
// Added a new function GetKeyName() and InitKeyText().
//
// Revision 1.3  2000/05/05 15:46:15  hebertjo
// Added 3 functions to set the cooperation level for the mouse, keyboard,
// and joystick.  Set MouseCooperation() was renamed to SetMouseExclusive().
//
// Revision 1.2  2000/05/02 21:48:23  hebertjo
// Added MindCry's fix to be able to reset the last key.
//
// Revision 1.1.1.1  2000/04/22 16:10:09  hebertjo
// Initial checkin of v3.0 to SourceForge CVS.
//
// Revision 2.9  2000/01/19 23:14:30  jhebert
// First checkin for v3.0
// - non-exclusive mouse access is the default now.
//
// 
//
// $Revision: 1.7 $
//////////////////////////////////////////////////////////////////////////////////
#ifdef SAVE_RCSID
static char rcsid[] = "@(#) $Id: Cdxinput.cpp,v 1.7 2015/08/27 19:34:05 stevens-madison Exp $";
#endif

#include "CDX.h"
#include "cdxinput.h"


// Global variables used by the enumeration callback function
static LPDIRECTINPUTDEVICE lpDID[MAX_JOYSTICKS] = {NULL};
static int g_iJoystickCount = 0;

//////////////////////////////////////////////////////////////////////////////////
// NAME: EnumJoystick
// 
// PURPOSE: Enumerate all the joystick devices on the system.         
//
// INPUT: pdinst - 
//        pvRef  - 
//
// RETURNS: DIENUM_CONTINUE - more joysticks to retrieve
//          DIENUM_STOP     - No more joysticks
//
//////////////////////////////////////////////////////////////////////////////////
BOOL FAR PASCAL EnumJoystick(LPCDIDEVICEINSTANCE pdinst, LPVOID pvRef)
{
	LPDIRECTINPUT pDI = (LPDIRECTINPUT)pvRef;

	// This loops through every joystick that it can create and as soon as it finds one
	// that isn't valid, it stops.  This *may* fail if the user has multiple joysticks
	// and the first one is not plugged in.  
	if( FAILED(pDI->CreateDevice(pdinst->guidInstance, &lpDID[g_iJoystickCount], NULL)) )
		// Failed
		return DIENUM_STOP;

	g_iJoystickCount++;
	return DIENUM_CONTINUE;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: Default constructor
// 
// PURPOSE: Create an instance of the object and initializes the member data.        
//
// INPUT: none
//
// RETURNS: nothing
//
//////////////////////////////////////////////////////////////////////////////////
CDXInput::CDXInput(void)
{
	int iJoyNum;
	m_lpDI					= NULL;
	m_lpDIDKeyboard			= NULL;
	m_lpDIDMouse			= NULL;

#if DIRECTINPUT_VERSION >= CDX_DIVER
	for (iJoyNum=0; iJoyNum<MAX_JOYSTICKS; iJoyNum++) {
		m_lpDIDJoystick[iJoyNum]		= 0;
	}
#endif

	m_iJoystickCount		= 0;
	m_bMouse				= FALSE;
	m_bKeyboard				= FALSE;
	m_bJoystick				= FALSE;
	for (iJoyNum=0; iJoyNum<MAX_JOYSTICKS; iJoyNum++) {
		m_JoystickType[iJoyNum]       = 0;
		m_joystickSensitivity[iJoyNum]= 1.0;
		m_joystickMinX[iJoyNum]		= 0x80000000;
		m_joystickMinY[iJoyNum]		= 0x80000000; 
		m_joystickMaxX[iJoyNum]		= 0x7fffffff;
		m_joystickMaxY[iJoyNum]		= 0x7fffffff; 
		m_joystickX[iJoyNum]			= 0;
		m_joystickY[iJoyNum]			= 0;
		m_joystickFreeX[iJoyNum]		= 0;
		m_joystickFreeY[iJoyNum]		= 0;
		m_joystickDeltaX[iJoyNum]		= 0;
		m_joystickDeltaY[iJoyNum]		= 0;
	}
	m_mouseSensitivity		= 1.0;
	m_mouseMinX				= 0x80000000;
	m_mouseMinY				= 0x80000000; 
	m_mouseMaxX				= 0x7fffffff;
	m_mouseMaxY				= 0x7fffffff; 
	m_mouseX				= 0;
	m_mouseY				= 0;
	m_mouseFreeX			= 0;
	m_mouseFreeY			= 0;
	m_mouseDeltaX			= 0;
	m_mouseDeltaY			= 0;
    m_lastKey               = 0;
    m_mouseThreshold2       = 6;
    m_mouseThreshold1       = 2;
    m_mouseMultiplier2      = 2.0;
    m_mouseMultiplier1      = 1.5;

	// wipe our internal key data
	FlushKeyboardData();

    
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: Destructor
// 
// PURPOSE: Releases all devices that accquired        
//
// INPUT: none
//
// RETURNS: nothing
//
//////////////////////////////////////////////////////////////////////////////////
CDXInput::~CDXInput(void)
{
	// make sure we're initialized
	if (!m_lpDI)
		return;
	
	// shutdown keyboard
	if (m_lpDIDKeyboard)
	{
		UnacquireKeyboard();
		m_lpDIDKeyboard->Release();
		m_lpDIDKeyboard = NULL;
	}
	
	// shutdown mouse
	if (m_lpDIDMouse)
	{
		UnacquireMouse();
		m_lpDIDMouse->Release();
		m_lpDIDMouse = NULL;
	}
	
	// shutdown joystick
#if DIRECTINPUT_VERSION >= CDX_DIVER
	if (m_lpDIDJoystick)
	{
		for (int i=0; i<m_iJoystickCount; i++) {
			UnacquireJoystick(i);
			m_lpDIDJoystick[i]->Release();
			m_lpDIDJoystick[i] = NULL;
		}
	}
#endif

	// kill directinput
	m_lpDI->Release();
	m_lpDI = NULL;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: Create
// 
// PURPOSE: Try to accquire all devices. Use SetActiveDevices() if you do not want 
//          some devices.       
//
// INPUT: hInst - 
//        hWnd  - 
//
// RETURNS: 0 or > - success
//          < 0    - failure
//
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXInput::Create(void *hInst, void *hWnd)
{
	HRESULT rval;

    m_hWnd = (HWND)hWnd;

    // Create the Direct Input object
	rval = DirectInputCreate( (HINSTANCE)hInst, DIRECTINPUT_VERSION, &m_lpDI, NULL);
	if( FAILED(rval) ) 
        return rval;

	// Create the mouse device
	rval = m_lpDI->CreateDevice(GUID_SysMouse, &m_lpDIDMouse, NULL);
	if( SUCCEEDED(rval) )
	{
		m_lpDIDMouse->SetDataFormat(&c_dfDIMouse);
	    m_lpDIDMouse->SetCooperativeLevel((HWND)hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);

		rval = AcquireMouse();
		if( FAILED(rval) && rval!=DIERR_OTHERAPPHASPRIO ) 
			return rval;

		// Set the default position for the mouse
		POINT point;
		GetCursorPos(&point);
		m_mouseX = point.x;
		m_mouseY = point.y;
	}

	// Create the keyboard device
	rval = m_lpDI->CreateDevice(GUID_SysKeyboard, &m_lpDIDKeyboard, NULL);
	if( SUCCEEDED(rval) )
	{
		m_lpDIDKeyboard->SetDataFormat(&c_dfDIKeyboard);
		m_lpDIDKeyboard->SetCooperativeLevel((HWND)hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);

		rval = AcquireKeyboard();
		if( FAILED(rval) && rval!=DIERR_OTHERAPPHASPRIO ) 
			return rval;

		// set up DIK_<keys> remap
		InitDIKToCDXKEY();
        InitCDXKEYToDIK();
		InitShiftedKeys();
        InitKeyText();
	}

	// Enumerate the joystick device (DirectX 5 or higher)
	m_iJoystickCount=0;
#if DIRECTINPUT_VERSION >= CDX_DIVER
	g_iJoystickCount=0;
	rval = m_lpDI->EnumDevices(DIDEVTYPE_JOYSTICK, EnumJoystick, m_lpDI, DIEDFL_ATTACHEDONLY);
	m_iJoystickCount = g_iJoystickCount;
	for (int i = 0; i<g_iJoystickCount; i++) {
		if(lpDID[i] != NULL)
		{
			rval = lpDID[i]->QueryInterface(IID_IDirectInputDevice2, (LPVOID *)&m_lpDIDJoystick[i]);
			if( FAILED(rval) ) 
				return rval;

			RELEASE(lpDID[i]);

			m_lpDIDJoystick[i]->SetDataFormat(&c_dfDIJoystick);
			m_lpDIDJoystick[i]->SetCooperativeLevel((HWND)hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);

			// Set the X-axis range (-1000 to +1000)
			DIPROPRANGE diprg;
			diprg.diph.dwSize = sizeof(diprg);
			diprg.diph.dwHeaderSize = sizeof(diprg.diph);
			diprg.diph.dwObj = DIJOFS_X;
			diprg.diph.dwHow = DIPH_BYOFFSET;
			diprg.lMin       = -1000;
			diprg.lMax       = +1000;

			rval = m_lpDIDJoystick[i]->SetProperty(DIPROP_RANGE, &diprg.diph);
			if( FAILED(rval) )
				return rval;

			// And again for Y-axis range
			diprg.diph.dwObj = DIJOFS_Y;

			rval = m_lpDIDJoystick[i]->SetProperty(DIPROP_RANGE, &diprg.diph);
			if( FAILED(rval) )
				return rval;

			// Set X axis dead zone to 10%
			DIPROPDWORD dipdw;
			dipdw.diph.dwSize = sizeof(dipdw);
			dipdw.diph.dwHeaderSize = sizeof(dipdw.diph);
			dipdw.diph.dwObj = DIJOFS_X;
			dipdw.diph.dwHow = DIPH_BYOFFSET;
			dipdw.dwData     = 1000;

			rval = m_lpDIDJoystick[i]->SetProperty(DIPROP_DEADZONE, &dipdw.diph);
			if( FAILED(rval) )
				return rval;

			dipdw.diph.dwObj = DIJOFS_Y;

			// Set Y axis dead zone to 10%
			rval = m_lpDIDJoystick[i]->SetProperty(DIPROP_DEADZONE, &dipdw.diph);
			if( FAILED(rval) )
				return rval;

			rval = AcquireJoystick(i);
			if( FAILED(rval) && rval!=DIERR_OTHERAPPHASPRIO ) 
				return rval;

			// get the joystick type
			DIDEVICEINSTANCE devinfo;

			ZeroMemory( &devinfo , sizeof( devinfo ) );
			devinfo.dwSize = sizeof( devinfo );
			m_lpDIDJoystick[i]->GetDeviceInfo( &devinfo );
			m_JoystickType[i] = GET_DIDEVICE_SUBTYPE( devinfo.dwDevType );
		}
	}
#endif

	return 0;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: SetActiveDevices
// 
// PURPOSE: Controls which devices you have accquired.      
//
// INPUT: bMouse     - TRUE if you want to acquire the mouse, else FALSE
//        bKeyboard  - TRUE if you want to acquire the keyboard, else FALSE
//        bJoystick  - TRUE if you want to acquire the joystick, else FALSE
//
// RETURNS: 0 or > - success
//          < 0    - failure
//
/////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CDXInput::SetActiveDevices(BOOL bMouse, BOOL bKeyboard, BOOL bJoystick)
{
    HRESULT rval;

	if (m_bMouse = bMouse)
		rval = AcquireMouse();
	else
		rval = UnacquireMouse();

	if (m_bKeyboard = bKeyboard)
		rval = AcquireKeyboard();
	else 
		rval = UnacquireKeyboard();

#if DIRECTINPUT_VERSION >= CDX_DIVER
	if (m_bJoystick = bJoystick) {
		for (int i=0; i<m_iJoystickCount; i++) {
			rval = AcquireJoystick(i);
		}
	} else {
		for (int i=0; i<m_iJoystickCount; i++) {
			rval = UnacquireJoystick(i);
		}
	}
#endif

    return rval;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: SetMouseExclusive
// 
// PURPOSE:      
//
// INPUT: TRUE  - Get exclusive access to the mouse
//        FALSE - Get shared access to the mouse
//
// RETURNS: 0 or > - success
//          < 0    - failure
//
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXInput::SetMouseExclusive(BOOL ExclusiveMouseAccess)
{
	HRESULT rval;

	// Unacquire the Mouse
    rval = UnacquireMouse();

	if( SUCCEEDED(rval) )
	{
		rval = m_lpDIDMouse->SetDataFormat(&c_dfDIMouse);
        if( FAILED(rval) ) 
			return rval;

		if( ExclusiveMouseAccess == FALSE )
			rval = m_lpDIDMouse->SetCooperativeLevel(m_hWnd, DISCL_FOREGROUND);
		else
			rval = m_lpDIDMouse->SetCooperativeLevel(m_hWnd, DISCL_EXCLUSIVE | DISCL_FOREGROUND);

        if( FAILED(rval) ) 
			return rval;

		rval = AcquireMouse();
        
	}

    return rval;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: SetKeyboardExclusive
// 
// PURPOSE:       
//
// INPUT: bExclusiveKeyboardAccess - TRUE  - set keyboard for exclusive access
//                                   FALSE - set keyboard for non-exclusive access
//
// RETURNS: 0 or > - success
//          < 0    - failure
//
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXInput::SetKeyboardExclusive(BOOL bExclusiveKeyboardAccess)
{
    HRESULT rval;

	// Unacquire the keyboard
    rval = UnacquireKeyboard();

	if( SUCCEEDED(rval) )
	{
		rval = m_lpDIDKeyboard->SetDataFormat(&c_dfDIKeyboard);
        if( FAILED(rval) ) 
			return rval;

		if( bExclusiveKeyboardAccess == FALSE )
			rval = m_lpDIDKeyboard->SetCooperativeLevel((HWND)m_hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
		else
			rval = m_lpDIDKeyboard->SetCooperativeLevel((HWND)m_hWnd, DISCL_EXCLUSIVE | DISCL_FOREGROUND);

        if( FAILED(rval) ) 
			return rval;

		rval = AcquireKeyboard();
	}

    return rval;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: SetJoystickExclusive
// 
// PURPOSE:       
//
// INPUT:  iJoyNum - Joystick ID (starting from 0)
//         bExclusiveJoystickAccess - TRUE  - set joystick for exclusive access
//                                   FALSE - set joystick for non-exclusive access
//
// RETURNS: 0 or > - success
//          < 0    - failure
//
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXInput::SetJoystickExclusive(int iJoyNum, BOOL bExclusiveJoystickAccess)
{
    HRESULT rval = -1;

#if DIRECTINPUT_VERSION >= CDX_DIVER
	// Unacquire the keyboard
    rval = UnacquireJoystick(iJoyNum);

	if( SUCCEEDED(rval) )
	{
		rval = m_lpDIDJoystick[iJoyNum]->SetDataFormat(&c_dfDIJoystick);
        if( FAILED(rval) ) 
			return rval;

		if( bExclusiveJoystickAccess == FALSE )
			rval = m_lpDIDJoystick[iJoyNum]->SetCooperativeLevel((HWND)m_hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
		else
			rval = m_lpDIDJoystick[iJoyNum]->SetCooperativeLevel((HWND)m_hWnd, DISCL_EXCLUSIVE | DISCL_FOREGROUND);

        if( FAILED(rval) ) 
			return rval;

		rval = AcquireJoystick(iJoyNum);
	}
#endif

    return rval;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: SetMouseAbs
// 
// PURPOSE: Sets the axis mode of the mouse to absolute.      
//
// INPUT: none
//
// RETURNS: 0 or > - success
//          < 0    - failure
//
/////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CDXInput::SetMouseAbs(void)
{
    HRESULT rval;
	DIPROPDWORD dipdw;

	dipdw.diph.dwSize       = sizeof(DIPROPDWORD);
	dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
	dipdw.diph.dwObj        = 0;
	dipdw.diph.dwHow        = DIPH_DEVICE;
	dipdw.dwData            = DIPROPAXISMODE_ABS;

	rval = m_lpDIDMouse->SetProperty(DIPROP_AXISMODE, &dipdw.diph);

    return rval;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: SetJoystickAbs
// 
// PURPOSE: Sets the axis mode of the joystick to absolute.      
//
// INPUT: iJoyNum - Joystick ID (starting from 0)
//        
// RETURNS: 0 or > - success
//          < 0    - failure
//
/////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CDXInput::SetJoystickAbs(int iJoyNum)
{
    HRESULT rval = -1;
#if DIRECTINPUT_VERSION >= CDX_DIVER
	DIPROPDWORD dipdw;

	dipdw.diph.dwSize       = sizeof(DIPROPDWORD);
	dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
	dipdw.diph.dwObj        = 0;
	dipdw.diph.dwHow        = DIPH_DEVICE;
	dipdw.dwData            = DIPROPAXISMODE_ABS;

	rval = m_lpDIDJoystick[iJoyNum]->SetProperty(DIPROP_AXISMODE, &dipdw.diph);
#endif

    return rval;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: RunMouseControlPanel
// 
// PURPOSE: Runs the DirectInput control panel associated with the mouse. If the
//          device does not have a control panel associated with it, the default 
//          device control panel is launched.     
//
// INPUT: hWnd - 
//
// RETURNS: 0 or > - success
//          < 0    - failure
//
/////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CDXInput::RunMouseControlPanel(void *hWnd)
{
    HRESULT rval;

	rval = m_lpDIDMouse->RunControlPanel((HWND)hWnd, 0);

    return rval;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: RunJoystickControlPanel
// 
// PURPOSE: Runs the DirectInput control panel associated with the joystick. If the
//          device does not have a control panel associated with it, the default 
//          device control panel is launched.     
//
// INPUT: iJoyNum - Joystick ID (starting from 0)
//        hWnd - 
//
// RETURNS: 0 or > - success
//          < 0    - failure
//
/////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CDXInput::RunJoystickControlPanel(int iJoyNum, void *hWnd)
{
    HRESULT rval = -1;

#if DIRECTINPUT_VERSION >= CDX_DIVER
	rval = m_lpDIDJoystick[iJoyNum]->RunControlPanel((HWND)hWnd, 0);
#endif

    return rval;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: FlushKeyboardData
// 
// PURPOSE: Wipes out the internal key data.    
//
// INPUT: none
//
// RETURNS: nothing
//
/////////////////////////////////////////////////////////////////////////////////////////////
void CDXInput::FlushKeyboardData(void)
{
	memset(m_keyStates, 0, sizeof(m_keyStates));
	memset(m_keyRaw, 0, sizeof(m_keyRaw));
    memset(m_oldKeyRaw, 0, sizeof(m_oldKeyRaw));
	memset(m_keyPressTimes, 0, sizeof(m_keyPressTimes));
	memset(m_keyDragStartPositions, 0, sizeof(m_keyDragStartPositions));
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: SetMouseLimits
// 
// PURPOSE: This restricts the mouse to a defined area.  
//
// INPUT: x1 - first X value
//        y1 - first Y value
//        x2 - second Y value
//        y2 - second Y value
//
// RETURNS: nothing
//
/////////////////////////////////////////////////////////////////////////////////////////////
void CDXInput::SetMouseLimits(LONG x1, LONG y1, LONG x2, LONG y2)
{
    if(x1 < x2)
    {
	    m_mouseMinX = x1;
        m_mouseMaxX = x2;
    }
    else
    {
        m_mouseMinX = x2;
        m_mouseMaxX = x1;
    }

    if(y1 < y2)
    {
	    m_mouseMinY = y1;
	    m_mouseMaxY = y2;
    }
    else
    {
        m_mouseMinY = y2;
	    m_mouseMaxY = y1;
    }
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: SetJoystickLimits
// 
// PURPOSE: This restricts the joystick to a defined area. 
//
// INPUT: iJoyNum - Joystick ID (starting from 0)
//        x1 - first X value
//        y1 - first Y value
//        x2 - second Y value
//        y2 - second Y value
//
// RETURNS: nothing
//
/////////////////////////////////////////////////////////////////////////////////////////////
void CDXInput::SetJoystickLimits(int iJoyNum, LONG x1, LONG y1, LONG x2, LONG y2)
{
    if(x1 < x2)
    {
	    m_joystickMinX[iJoyNum] = x1;
        m_joystickMaxX[iJoyNum] = x2;
    }
    else
    {
        m_joystickMinX[iJoyNum] = x2;
        m_joystickMaxX[iJoyNum] = x1;
    }

    if(y1 < y2)
    {
	    m_joystickMinY[iJoyNum] = y1;
	    m_joystickMaxY[iJoyNum] = y2;
    }
    else
    {
        m_joystickMinY[iJoyNum] = y2;
	    m_joystickMaxY[iJoyNum] = y1;
    }
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: SetMousePos
// 
// PURPOSE: Sets the mouse position. This restricts the position to the physical 
//          display.
//
// INPUT: x - X position of the mouse
//        y - Y position of the mouse
//
// RETURNS: nothing
//
///////////////////////////////////////////////////////////////////////////////////
void CDXInput::SetMousePos(LONG x, LONG y)
{
	// clamp non-free mouse values to limits
	if ((m_mouseX = x) >= m_mouseMaxX)
		m_mouseX = m_mouseMaxX-1;

	if ((m_mouseY = y) >= m_mouseMaxY)
		m_mouseY = m_mouseMaxY-1;

	if ((m_mouseX = x) <= m_mouseMinX)
		m_mouseX = m_mouseMinX+1;

	if ((m_mouseY = y) <= m_mouseMinY)
		m_mouseY = m_mouseMinY+1;

	// Zero out the Deltas
	m_mouseDeltaX = 0;
	m_mouseDeltaY = 0;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: SetMouseFreePos
// 
// PURPOSE: Sets the mouse position. This allows you to set the mouse to 
//          anywhere (no limits).
//
// INPUT: x - X position of the mouse
//        y - Y position of the mouse
//
// RETURNS: nothing
//
/////////////////////////////////////////////////////////////////////////////////////////////
void CDXInput::SetMouseFreePos(LONG x, LONG y)
{
	m_mouseFreeX  = x;
	m_mouseFreeY  = y;

	// Zero out the Deltas
	m_mouseDeltaX = 0;
	m_mouseDeltaY = 0;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: SetJoystickPos
// 
// PURPOSE: Sets the joystick position. This restricts the position to the 
//          physical display.
//
// INPUT: iJoyNum - Joystick ID (starting from 0)
//        x - X position of the joystick
//        y - Y position of the joystick
//
// RETURNS: nothing
//
/////////////////////////////////////////////////////////////////////////////////////
void CDXInput::SetJoystickPos(int iJoyNum, LONG x, LONG y)
{
	// clamp non-free joystick values to limits
	if ((m_joystickX[iJoyNum] = x) >= m_joystickMaxX[iJoyNum])
		m_joystickX[iJoyNum] = m_joystickMaxX[iJoyNum]-1;

	if ((m_joystickY[iJoyNum] = y) >= m_joystickMaxY[iJoyNum])
		m_joystickY[iJoyNum] = m_joystickMaxY[iJoyNum]-1;

	if ((m_joystickX[iJoyNum] = x) <= m_joystickMinX[iJoyNum])
		m_joystickX[iJoyNum] = m_joystickMinX[iJoyNum]+1;

	if ((m_joystickY[iJoyNum] = y) <= m_joystickMinY[iJoyNum])
		m_joystickY[iJoyNum] = m_joystickMinY[iJoyNum]+1;

	// Zero out the Deltas
	m_joystickDeltaX[iJoyNum] = 0;
	m_joystickDeltaY[iJoyNum] = 0;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: SetJoystickFreePos
// 
// PURPOSE: Sets the joystick position. This allows you to set the joystick to 
//          anywhere (no limits).
//
// INPUT: iJoyNum - Joystick ID (starting from 0)
//        x - X position of the joystick
//        y - Y position of the joystick
//
// RETURNS: nothing
//
///////////////////////////////////////////////////////////////////////////////////
void CDXInput::SetJoystickFreePos(int iJoyNum, LONG x, LONG y)
{
	m_joystickFreeX[iJoyNum]  = x;
	m_joystickFreeY[iJoyNum]  = y;

	// Zero out the Deltas
	m_joystickDeltaX[iJoyNum] = 0;
	m_joystickDeltaY[iJoyNum] = 0;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: SetMouseSensitivity
// 
// PURPOSE: Sets the mouse sensitivity factor.
//
// INPUT: factor - multiplier 0 or >
//
// RETURNS: nothing
//
//////////////////////////////////////////////////////////////////////////////////
void CDXInput::SetMouseSensitivity(float factor)
{
    if(factor < 0 )
        factor = 0;

	m_mouseSensitivity = factor;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: GetMouseSensitivity
// 
// PURPOSE: Retrieves the current mouse sensitivity factor.
//
// INPUT: none
//
// RETURNS: nothing
//
///////////////////////////////////////////////////////////////////////////////////
float CDXInput::GetMouseSensitivity()
{
	return(m_mouseSensitivity);
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: SetJoystickSensitivity
// 
// PURPOSE: Sets the Joystick sensitivity factor.
//
// INPUT: iJoyNum - Joystick ID (starting from 0)
//        factor - multiplier 0 or >
//
// RETURNS: nothing
//
// Sets the Joystick sensitivity factor.
///////////////////////////////////////////////////////////////////////////////////
void CDXInput::SetJoystickSensitivity(int iJoyNum, float factor)
{
    if(factor < 0 )
        factor = 0;

	m_joystickSensitivity[iJoyNum] = factor;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: GetMouseSensitivity
// 
// PURPOSE: Retrieves the current joystick sensitivity factor.
//
// INPUT: iJoyNum - Joystick ID (starting from 0)
//
// RETURNS: nothing
//
//////////////////////////////////////////////////////////////////////////////////
float CDXInput::GetJoystickSensitivity(int iJoyNum)
{
	return(m_joystickSensitivity[iJoyNum]);
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: GetShiftedKeyState
// 
// PURPOSE: This tells you whether or not the given key is in a shifted state.
//
// INPUT: key - value of the key to be shifted
//
// RETURNS: the shifted key value is returned if the is in a shifted state else
//          the value passed in is returned.
//
//////////////////////////////////////////////////////////////////////////////////
WORD CDXInput::GetShiftedKeyState(WORD key)
{
	if (m_shiftedKeyStates[key])
		return (m_shiftedKeyStates[key]);

	return(key);
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: GetShiftedKeyState
// 
// PURPOSE: Retrieves the keystate of a given key.
//
// INPUT: key - value of the key to be checked
//
// RETURNS: returns the state of the key
//
//////////////////////////////////////////////////////////////////////////////////
WORD CDXInput::GetKeyState(WORD key)
{
    if(key >= CDXKEY_NUMKEYS)
        return 0;

	return m_keyStates[key];
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: GetKeyRaw
// 
// PURPOSE: Gets raw direct draw information about a key.
//
// INPUT: key - value of the key to be checked
//
// RETURNS: returns the raw state of the key
//
/////////////////////////////////////////////////////////////////////////////////////////////
WORD CDXInput::GetKeyRaw(WORD key)
{
    if(key >= NUMDIKEYS)
        return 0;

	return m_keyRaw[key];
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: GetMousePos
// 
// PURPOSE: Retrieves the mouse position and stores the x and y values in the 
//          passed parameters.
//
// INPUT: x - pointer to a long that will be set to the X position
//        y - pointer to a long that will be set to the Y position
//
// RETURNS: nothing
//
//////////////////////////////////////////////////////////////////////////////////
void CDXInput::GetMousePos(LONG* x, LONG* y)
{
	*x = m_mouseX;
	*y = m_mouseY;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: GetMouseFreePos
// 
// PURPOSE: Retrieves the free mouse position and stores the x and y values in 
//          the passed parameters.
//
// INPUT: x - pointer to a long that will be set to the X position
//        y - pointer to a long that will be set to the Y position
//
// RETURNS: nothing
//
///////////////////////////////////////////////////////////////////////////////////
void CDXInput::GetMouseFreePos(LONG* x, LONG* y)
{
	*x = m_mouseFreeX;
	*y = m_mouseFreeY;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: GetMouseDeltas
// 
// PURPOSE: Retrieves the mouse deltas and stores the x and y values in the 
//          passed parameters.
//
// INPUT: x - pointer to a long that will be set to the X position
//        y - pointer to a long that will be set to the Y position
//
// RETURNS: nothing
//
/////////////////////////////////////////////////////////////////////////////////////////////
void CDXInput::GetMouseDeltas(LONG* x, LONG* y)
{
	*x = m_mouseDeltaX;
	*y = m_mouseDeltaY;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: GetMouseScroll
// 
// PURPOSE: Returns true if the CDXKEY passed in was used, only applies to mouse
//			scrolling Key's.  Also returns the number of items the scroll call
//			occured, such as if the user scrolled up 2 times between your updates.
//
// INPUT: key - value of the key to be checked.
//		  num - number of times that key was pressed.
//
// RETURNS: nothing
//
//////////////////////////////////////////////////////////////////////////////////
bool CDXInput::GetMouseScroll(WORD  _CDXKey, UINT * num)
{
	switch(_CDXKey)
	{
	case CDXKEY_MOUSESCROLLUP:
		if(m_keyScroll[0] == true)
		{
			if(num != NULL)
			{
				*num = m_ScrollAmount[0];
			}
			return true;
		}
		else
		{
			return false;
		}
		break;
	case CDXKEY_MOUSESCROLLDOWN:
		if(m_keyScroll[1] == true)
		{
			if(num != NULL)
			{
				*num = m_ScrollAmount[1];
			}
			return true;
		}
		else
		{
			return false;
		}
		break;
	default:
		return false;
		break;
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: GetJoystickPos
// 
// PURPOSE: Retrieves the joystick position and stores the x and y values in the 
//          passed parameters.
//
// INPUT: iJoyNum - Joystick ID (starting from 0)
//        x - pointer to a long that will be set to the X position
//        y - pointer to a long that will be set to the Y position
//
// RETURNS: nothing
//
//////////////////////////////////////////////////////////////////////////////////
void CDXInput::GetJoystickPos(int iJoyNum, LONG* x, LONG* y)
{
	*x = m_joystickX[iJoyNum];
	*y = m_joystickY[iJoyNum];
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: GetJoystickFreePos
// 
// PURPOSE: Retrieves the free joystick position and stores the x and y values in 
//          the passed parameters.
//
// INPUT: iJoyNum - Joystick ID (starting from 0)
//        x - pointer to a long that will be set to the X position
//        y - pointer to a long that will be set to the Y position
//
// RETURNS: nothing
//
///////////////////////////////////////////////////////////////////////////////////
void CDXInput::GetJoystickFreePos(int iJoyNum, LONG* x, LONG* y)
{
	*x = m_joystickFreeX[iJoyNum];
	*y = m_joystickFreeY[iJoyNum];
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: GetJoystickDeltas
// 
// PURPOSE: Retrieves the joystick deltas and stores the x and y values in the 
//          passed parameters.
//
// INPUT: iJoyNum - Joystick ID (starting from 0)
//        x - pointer to a long that will be set to the X position
//        y - pointer to a long that will be set to the Y position
//
// RETURNS: nothing
//
//////////////////////////////////////////////////////////////////////////////////
void CDXInput::GetJoystickDeltas(int iJoyNum, LONG* x, LONG* y)
{
	*x = m_joystickDeltaX[iJoyNum];
	*y = m_joystickDeltaY[iJoyNum];
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: AcquireMouse
// 
// PURPOSE: Obtains access to the mouse.
//
// INPUT: none
//
// RETURNS: 0 or > - success
//          < 0    - failure
//
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXInput::AcquireMouse(void)
{
    HRESULT rval = -1;
    int i;

	if(m_lpDIDMouse)
	{
		m_bMouse = TRUE;
        rval = m_lpDIDMouse->Acquire();

		if ( SUCCEEDED(rval) )
			return rval;

		// didn't get it back, try a few more times
		for (i=0;i<50;i++)
		{
            rval = m_lpDIDMouse->Acquire();
			if ( SUCCEEDED(rval) )
				return rval;
		}

		// still don't have it, something's up
		if( rval!=DIERR_OTHERAPPHASPRIO ) m_bMouse = FALSE;
	}

    return rval;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: UnacquireMouse
// 
// PURPOSE: Releases access to the mouse.
//
// INPUT: none
//
// RETURNS: 0 or > - success
//          < 0    - failure
//
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXInput::UnacquireMouse(void)
{
    HRESULT rval = -1;

	m_bMouse = FALSE;

	if (m_lpDIDMouse)
		rval = m_lpDIDMouse->Unacquire();

    return rval;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: AcquireJoystick
// 
// PURPOSE: Obtains access to a joystick.
//
// INPUT: iJoyNum - Joystick ID (starting from 0)
//
// RETURNS: 0 or > - success
//          < 0    - failure
//
/////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CDXInput::AcquireJoystick(int iJoyNum)
{
    HRESULT rval = -1;

#if DIRECTINPUT_VERSION >= CDX_DIVER
    int i;
	if (m_lpDIDJoystick[iJoyNum])
	{
        rval = m_lpDIDJoystick[iJoyNum]->Acquire();

		if ( SUCCEEDED(rval) ) {
			m_bJoystick = TRUE;
			return rval;
		}

		// didn't get it back, try a few more times
		for (i=0;i<50;i++)
		{
            rval = m_lpDIDJoystick[iJoyNum]->Acquire();

			if ( SUCCEEDED(rval) ) {
				m_bJoystick = TRUE;
				return rval;
			}
		}

		// still don't have it, something's up
		m_bJoystick = FALSE;
	}
#endif

    return rval;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: UnacquireJoystick
// 
// PURPOSE: Releases access to a joystick.
//
// INPUT: iJoyNum - Joystick ID (starting from 0)
//
// RETURNS: 0 or > - success
//          < 0    - failure
//
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXInput::UnacquireJoystick(int iJoyNum)
{
    HRESULT rval = -1;

#if DIRECTINPUT_VERSION >= CDX_DIVER
	m_bJoystick = FALSE;

	if (m_lpDIDJoystick[iJoyNum])
		rval = m_lpDIDJoystick[iJoyNum]->Unacquire();
#endif

    return rval;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: AcquireKeyboard
// 
// PURPOSE: Obtains access to the keyboard.
//
// INPUT: none
//
// RETURNS: 0 or > - success
//          < 0    - failure
//
///////////////////////////////////////////////////////////////////////////////////
HRESULT CDXInput::AcquireKeyboard(void)
{
    HRESULT rval = -1;
    int i;

	if (m_lpDIDKeyboard)
	{
		m_bKeyboard = TRUE;
        rval = m_lpDIDKeyboard->Acquire();

		if ( SUCCEEDED(rval) )
		{
			FlushKeyboardData();
			return rval;
		}

		// didn't get it back, try a few more times
		for (i=0;i<50;i++)
		{
			rval = m_lpDIDKeyboard->Acquire();

		    if ( SUCCEEDED(rval) )
			{
				FlushKeyboardData();
				return rval;
			}
		}

		// still don't have it, something's up
		m_bKeyboard = FALSE;
	}	

    return rval;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: UnacquireKeyboard
// 
// PURPOSE: Releases access to the keyboard.
//
// INPUT: none
//
// RETURNS: 0 or > - success
//          < 0    - failure
//
///////////////////////////////////////////////////////////////////////////////////
HRESULT CDXInput::UnacquireKeyboard(void)
{
    HRESULT rval = -1;

	FlushKeyboardData();
	m_bKeyboard = FALSE;

	if (m_lpDIDKeyboard)
		rval = m_lpDIDKeyboard->Unacquire();

    return rval;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: SetMouseAccelerator
// 
// PURPOSE: Sets the thresholds that accelerate the mouse.  This gives the mouse
//          a natural feel.
//
// INPUT: 
//
// RETURNS: nothing
//
///////////////////////////////////////////////////////////////////////////////////
void CDXInput::SetMouseAccelerator(BOOL onOff, DWORD threshold1, double mult1, DWORD threshold2, double mult2)
{
    int mouseAcclValues[3];

    if(onOff == FALSE)
    {
        // turn accleration off.
        m_mouseThreshold2  = 999;
        m_mouseThreshold1  = 999;
        m_mouseMultiplier2 = 1.0;
        m_mouseMultiplier1 = 1.0;
    }
    else
    {
        if( (threshold1 == 0) && (threshold2 == 0) )
        {
            // Use system variables.
            SystemParametersInfo(SPI_GETMOUSE, 0, mouseAcclValues, 0);

            if(mouseAcclValues[2] > 0)
                m_mouseThreshold1  = mouseAcclValues[0];
            else
                m_mouseThreshold1  = 999;

            if(mouseAcclValues[2] == 2)
                m_mouseThreshold2  = mouseAcclValues[1];
            else
                m_mouseThreshold2  = 999;

            m_mouseMultiplier2 = 4.0;
            m_mouseMultiplier1 = 2.0;
        }
        else
        {
            if(threshold1 > threshold2)
            {
                m_mouseThreshold2  = threshold1;
                m_mouseThreshold1  = threshold2;
                m_mouseMultiplier2 = mult1;
                m_mouseMultiplier1 = mult2;
            }
            else
            {
                m_mouseThreshold2  = threshold2;
                m_mouseThreshold1  = threshold1;
                m_mouseMultiplier2 = mult2;
                m_mouseMultiplier1 = mult1;
            }
        }
    }
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: Update
// 
// PURPOSE: Updates all devices. Call this before you check for input.
//
// INPUT: none
//
// RETURNS: 0 or > - success
//          < 0    - failure
//
///////////////////////////////////////////////////////////////////////////////////
HRESULT CDXInput::Update(void)
{
    HRESULT rval;
	DIMOUSESTATE MouseState;
	LONG oldx, oldy;
	BOOL isPressed;
	WORD i;
	DWORD key;

#if DIRECTINPUT_VERSION >= CDX_DIVER
	DIJOYSTATE JoyState;
#endif

	///////////////////////
	//  KEYBOARD
	///////////////////////
	if(1)
	{
		rval = m_lpDIDKeyboard->GetDeviceState(256, &m_keyRaw);
		if( FAILED(rval) )
		{
			AcquireKeyboard();
			return rval;
		}
		else
		{
			for(i=0; i<NUMDIKEYS; i++)
			{
				if (key = m_DIKToCDXKEY[i])
				{
					// check current key state (bit 8 flipped on means key is pressed)
					if (m_keyRaw[i] & (BYTE)0x80)
						isPressed = TRUE;
					else
						isPressed = FALSE;

					CheckKeyEvents(key, isPressed);
				}
			}
		}
	}

	///////////////////////
	//  MOUSE
	///////////////////////
	if(m_bMouse)
	{
		rval = m_lpDIDMouse->GetDeviceState(sizeof(MouseState), &MouseState);
		if( FAILED(rval) )
		{
			AcquireMouse();
			return rval;
		}
		else
		{
            // Apply acceleration thresholds.
            if( abs(MouseState.lX) > m_mouseThreshold2 )
                MouseState.lX = (LONG)((double)MouseState.lX * m_mouseMultiplier2);
            else if( abs(MouseState.lX) > m_mouseThreshold1 )
                MouseState.lX = (LONG)((double)MouseState.lX * m_mouseMultiplier1);

            if( abs(MouseState.lY) > m_mouseThreshold2 )
                MouseState.lY = (LONG)((double)MouseState.lY * m_mouseMultiplier2);
            else if( abs(MouseState.lY) > m_mouseThreshold1 )
                MouseState.lY = (LONG)((double)MouseState.lY * m_mouseMultiplier1);

			// get new mouse position status
			oldx          = m_mouseFreeX;
			oldy          = m_mouseFreeY;
			m_mouseFreeX += (LONG)((float)MouseState.lX * m_mouseSensitivity);
			m_mouseFreeY += (LONG)((float)MouseState.lY * m_mouseSensitivity);
			m_mouseX     += (LONG)((float)MouseState.lX * m_mouseSensitivity);
			m_mouseY     += (LONG)((float)MouseState.lY * m_mouseSensitivity);

			// clamp non-free mouse values to limits
			if (m_mouseX >= m_mouseMaxX)
				m_mouseX = m_mouseMaxX-1;

			if (m_mouseY >= m_mouseMaxY)
				m_mouseY = m_mouseMaxY-1;

			if (m_mouseX <= m_mouseMinX)
				m_mouseX = m_mouseMinX;

			if (m_mouseY <= m_mouseMinY)
				m_mouseY = m_mouseMinY;

			m_mouseDeltaX = m_mouseFreeX - oldx;
			m_mouseDeltaY = m_mouseFreeY - oldy;
	
			// check the mouse buttons
			for(i=0; i<3; i++)
			{
				key = CDXKEY_MOUSELEFT + i;

				// check current key state (bit 8 flipped on means key is pressed)
				if (MouseState.rgbButtons[i] & (BYTE)0x80)
					isPressed = TRUE;
				else
					isPressed = FALSE;

				CheckKeyEvents(key, isPressed);
			}

			// check if the mouse scrolling button was used.
			if(MouseState.lZ != 0)
			{
				//check if it was a valid mouse scroll effect.
				if(((int)(MouseState.lZ / 120)) == ((double)(MouseState.lZ / 120)))
				{
					if(MouseState.lZ > 0)
					{
						m_ScrollAmount[0] = (BYTE)MouseState.lZ / 120;
						m_keyScroll[0] = true;
						m_keyScroll[1] = false;
					}
					else
					{
						m_ScrollAmount[1] = (BYTE)(MouseState.lZ / 120) * -1;
						m_keyScroll[1] = true;
						m_keyScroll[0] = false;
					}
				}
			}
			else
			{
				m_keyScroll[0] = false;
				m_keyScroll[1] = false;
			}
		}
	}

	///////////////////////
	//  JOYSTICK
	///////////////////////
#if DIRECTINPUT_VERSION >= CDX_DIVER
	if(m_bJoystick)
	{
		for (int iJoyNum=0; iJoyNum<m_iJoystickCount; iJoyNum++){
			m_lpDIDJoystick[iJoyNum]->Poll();

			rval = m_lpDIDJoystick[iJoyNum]->GetDeviceState(sizeof(JoyState), &JoyState);

			if( FAILED(rval) )
			{
				AcquireJoystick(iJoyNum);
				return rval;
			}
			else
			{
				// get new joystick position status
				LONG joyX=(LONG)((float)JoyState.lX * m_joystickSensitivity[iJoyNum]),
					 joyY=(LONG)((float)JoyState.lY * m_joystickSensitivity[iJoyNum]);
				
				oldx			= m_joystickFreeX[iJoyNum];
				oldy			= m_joystickFreeY[iJoyNum];
				m_joystickFreeX[iJoyNum] += joyX;
				m_joystickFreeY[iJoyNum] += joyY;
				m_joystickX[iJoyNum]		+= joyX;
				m_joystickY[iJoyNum]		+= joyY;

				// clamp non-free joystick values to limits
				if (m_joystickX[iJoyNum] >= m_joystickMaxX[iJoyNum])
					m_joystickX[iJoyNum] = m_joystickMaxX[iJoyNum]-1;

				if (m_joystickY[iJoyNum] >= m_joystickMaxY[iJoyNum])
					m_joystickY[iJoyNum] = m_joystickMaxY[iJoyNum]-1;

				if (m_joystickX[iJoyNum] <= m_joystickMinX[iJoyNum])
					m_joystickX[iJoyNum] = m_joystickMinX[iJoyNum];

				if (m_joystickY[iJoyNum] <= m_joystickMinY[iJoyNum])
					m_joystickY[iJoyNum] = m_joystickMinY[iJoyNum];

				m_joystickDeltaX[iJoyNum] = m_joystickFreeX[iJoyNum] - oldx;
				m_joystickDeltaY[iJoyNum] = m_joystickFreeY[iJoyNum] - oldy;
		
				// check the joystick buttons
				for(i=0; i<10; i++)
				{
					key = CDXKEY_JOYBUTN0 + i + iJoyNum*MAX_JOYSTICK_BUTTONS;

					// check current key state (bit 8 flipped on means key is pressed)
					if (JoyState.rgbButtons[i] & (BYTE)0x80)
						isPressed = TRUE;
					else
						isPressed = FALSE;

					CheckKeyEvents(key, isPressed);
				}
			}
		}
	}
#endif

    return 0;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: CheckKeyEvents
// 
// PURPOSE: Checks for keyboard events.
//
// INPUT: key       - key value to check
//        isPressed - is this key pressed or not?
//
// RETURNS: nothing
//
//////////////////////////////////////////////////////////////////////////////////
void CDXInput::CheckKeyEvents(DWORD key, BOOL isPressed)
{
	if (isPressed)
	{
		if (m_keyStates[key] == CDXKEY_NONE || m_keyStates[key] == CDXKEY_RELEASE)
		{
			// press event (key was up before but down now)
			m_keyStates[key]				= CDXKEY_PRESS;
			m_keyPressTimes[key]			= timeGetTime();
			m_keyDragStartPositions[key][0] = m_mouseFreeX;
			m_keyDragStartPositions[key][1] = m_mouseFreeY;
		}
		else if (m_keyStates[key] == CDXKEY_PRESS)
		{
			// drag event (key is still down)
			m_keyStates[key] = CDXKEY_REPEAT;
		}
	}
	else // Key is not currently pressed
	{
		if (m_keyStates[key] == CDXKEY_REPEAT || m_keyStates[key] == CDXKEY_PRESS)
		{
			// release event (key was down before but up now)
			m_keyStates[key] = CDXKEY_RELEASE;
		}
		else if (m_keyStates[key] == CDXKEY_RELEASE)
		{
			m_keyStates[key] = CDXKEY_NONE;
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: UpdateLastKey
// 
// PURPOSE: Updates the m_LastKey variable, and fills it with the LastKey pressed
//
// INPUT: reset - Reset the m_LastKey variable
//
// RETURNS: nothing
//
///////////////////////////////////////////////////////////////////////////////////
void CDXInput::UpdateLastKey( BOOL reset )
{
	if( reset ) m_lastKey=0;

    for( int i=0; i<255; i++ )
    {       
        if( m_keyRaw[i] && !m_oldKeyRaw[i] )
        {       
            m_lastKey = i;
                break;
        }
    }

    memcpy( m_oldKeyRaw, m_keyRaw, 255 );
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: GetKeyDesc
// 
// PURPOSE: Fills the str with the Description of the CeckKey.
//
// INPUT: CheckKey - 
//        str      - 
//
// RETURNS: nothing
//
//////////////////////////////////////////////////////////////////////////////////
void CDXInput::GetKeyDesc( DWORD CheckKey, char *str )
{       
    DWORD key;
    
    key = (CheckKey<<16); //+(1<<30); Extended Keys
    GetKeyNameText( key, str, 50 );
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: ToAscii
// 
// PURPOSE: Converts a rawkey to it's maped windows ASCII key, including the check
//          for ALT and the SHIFT keys.
//
// INPUT: rawkey
//
// RETURNS: the character
//
//////////////////////////////////////////////////////////////////////////////////
char CDXInput::RawToAscii( DWORD rawkey )
{
	BYTE states[256], asc[2];
	if( GetKeyboardState(states) == FALSE )
		return 0;

	if( ::ToAscii( MapVirtualKey( rawkey, 1 ), rawkey, states, (LPWORD)asc, 0 ) > 0 )
		return asc[0];

	return 0;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: GetLastKeyChar
// 
// PURPOSE: Will convert LastKey to Scancode like in dos 
//
// INPUT: none
//
// RETURNS: returns the last key pressed in Scancode format
// 
/////////////////////////////////////////////////////////////////////////////////////////////
DWORD CDXInput::GetLastKeyChar( void )
{       
    return MapVirtualKey( m_lastKey, 1 );    
}


//////////////////////////////////////////////////////////////////////////////////
// NAME: GetNumberOfJoystickButtons
// 
// PURPOSE: returns the number of buttons a joystick has
//
// INPUT: iJoyNum - Joystick ID (starting from 0)
//
// RETURNS: number of buttons
// 
////////////////////////////////////////////////////////////////////////////////////
DWORD CDXInput::GetNumberOfJoystickButtons( int iJoyNum )
{
#if DIRECTINPUT_VERSION >= CDX_DIVER

    DIDEVCAPS Caps;

    // check if joystick exists
	if(m_bJoystick)
	{
        ZeroMemory( &Caps , sizeof( Caps ) );
        Caps.dwSize = sizeof( Caps );    
        if( SUCCEEDED(m_lpDIDJoystick[iJoyNum]->GetCapabilities( &Caps )) )
            return Caps.dwButtons ;
        else
            return 0;
    }
    else
    {
        return 0;
    }
#else
    return 0;
#endif
}


//////////////////////////////////////////////////////////////////////////////////
// NAME: IsGamePad
// 
// PURPOSE: returns TRUE, if the control is a gamepad
// this is useful if you want to know if the joystick is analog or digital
// so you can check for the correct axes values
//
// INPUT: iJoyNum - Joystick ID (starting from 0)
//
// RETURNS: TRUE  - it is a gamepad
//          FALSE - it's not!
// 
/////////////////////////////////////////////////////////////////////////////////////////////
BOOL CDXInput::IsGamePad(int iJoyNum)
{
    if( m_JoystickType[iJoyNum] == DIDEVTYPEJOYSTICK_GAMEPAD )
        return TRUE;
    else
        return FALSE;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: GetKeyName
// 
// PURPOSE: Copies the textual description of the CDXKEY id in 'cdxkey' into 'buf'.
//          It honors the buffer length passed in by 'buflength'.
//
// INPUT: cdxkey -
//        buf    -
//        buflength - 
//
// RETURNS: TRUE  - it is a gamepad
//          FALSE - it's not!
// 
/////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CDXInput::GetKeyName(DWORD cdxkey, char* buf, DWORD buflength)
{
    if(buf == NULL)
        return -1;

    strncpy(buf, &m_keyText[cdxkey][0], buflength);

    return 1;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: InitShiftedKeys
// 
// PURPOSE: Initializes the shifted key states.
//
// INPUT: none
//
// RETURNS: nothing
//
/////////////////////////////////////////////////////////////////////////////////////////////
void CDXInput::InitShiftedKeys(void)
{
	WORD i;

	memset(m_shiftedKeyStates, 0, CDXKEY_NUMKEYS * sizeof(BYTE));

	for (i=0;i<26;i++)
		m_shiftedKeyStates[i+'a'] = i+'A';

	m_shiftedKeyStates['0']  = ')';
	m_shiftedKeyStates['1']  = '!';
	m_shiftedKeyStates['2']  = '@';
	m_shiftedKeyStates['3']  = '#';
	m_shiftedKeyStates['4']  = '$';
	m_shiftedKeyStates['5']  = '%';
	m_shiftedKeyStates['6']  = '^';
	m_shiftedKeyStates['7']  = '&';
	m_shiftedKeyStates['8']  = '*';
	m_shiftedKeyStates['9']  = '(';
	m_shiftedKeyStates['-']  = '_';
	m_shiftedKeyStates['=']  = '+';
	m_shiftedKeyStates['[']  = '{';
	m_shiftedKeyStates[']']  = '}';
	m_shiftedKeyStates['\\'] = '|';
	m_shiftedKeyStates[';']  = ':';
	m_shiftedKeyStates['\''] = '"';
	m_shiftedKeyStates[',']  = '<';
	m_shiftedKeyStates['.']  = '>';
	m_shiftedKeyStates['/']  = '?';
	m_shiftedKeyStates['`']  = '~';
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: InitDIKToCDXKEY
// 
// PURPOSE: Since we can't guarantee that the constants for these DIKs won't
//          change, initialize the remap table here instead of statically doing it.
//
// INPUT: none
//
// RETURNS: nothing
//
////////////////////////////////////////////////////////////////////////////////////
void CDXInput::InitCDXKEYToDIK(void)
{
	// any key mapped to zero means unmapped and should be ignored
	memset(m_CDXKEYToDIK, 0, NUMDIKEYS * sizeof(DWORD));

	m_CDXKEYToDIK[CDXKEY_BACKSPACE]     = DIK_BACK;
	m_CDXKEYToDIK[CDXKEY_TAB]           = DIK_TAB;
	m_CDXKEYToDIK[CDXKEY_ENTER]         = DIK_RETURN;
	m_CDXKEYToDIK[CDXKEY_ESCAPE]        = DIK_ESCAPE;
	m_CDXKEYToDIK[CDXKEY_SPACE]         = DIK_SPACE;
	m_CDXKEYToDIK[CDXKEY_BANG]          = DIK_1;
	m_CDXKEYToDIK[CDXKEY_DQUOTE]        = DIK_APOSTROPHE;
	m_CDXKEYToDIK[CDXKEY_POUND]         = DIK_3;
	m_CDXKEYToDIK[CDXKEY_DOLLAR]        = DIK_4;
	m_CDXKEYToDIK[CDXKEY_PERCENT]       = DIK_5;
	m_CDXKEYToDIK[CDXKEY_AMPERSAND]     = DIK_7;
	m_CDXKEYToDIK[CDXKEY_APOSTROPHE]    = DIK_APOSTROPHE;
	m_CDXKEYToDIK[CDXKEY_LBRACE]        = DIK_9;
	m_CDXKEYToDIK[CDXKEY_RBRACE]        = DIK_0;
	m_CDXKEYToDIK[CDXKEY_ASTERISC]      = DIK_8;
	m_CDXKEYToDIK[CDXKEY_PLUS]          = DIK_EQUALS;
	m_CDXKEYToDIK[CDXKEY_COMMA]         = DIK_COMMA;
	m_CDXKEYToDIK[CDXKEY_DASH]          = DIK_MINUS;
	m_CDXKEYToDIK[CDXKEY_PERIOD]        = DIK_PERIOD;
	m_CDXKEYToDIK[CDXKEY_SLASH]         = DIK_SLASH;
	m_CDXKEYToDIK[CDXKEY_0]             = DIK_0;
	m_CDXKEYToDIK[CDXKEY_1]             = DIK_1;
	m_CDXKEYToDIK[CDXKEY_2]             = DIK_2;
	m_CDXKEYToDIK[CDXKEY_3]             = DIK_3;
	m_CDXKEYToDIK[CDXKEY_4]             = DIK_4;
	m_CDXKEYToDIK[CDXKEY_5]             = DIK_5;
	m_CDXKEYToDIK[CDXKEY_6]             = DIK_6;
	m_CDXKEYToDIK[CDXKEY_7]             = DIK_7;
	m_CDXKEYToDIK[CDXKEY_8]             = DIK_8;
	m_CDXKEYToDIK[CDXKEY_9]             = DIK_9;
	m_CDXKEYToDIK[CDXKEY_COLON]         = DIK_COLON;
	m_CDXKEYToDIK[CDXKEY_SEMICOLON]     = DIK_SEMICOLON;
	m_CDXKEYToDIK[CDXKEY_LESSTHEN]      = DIK_COMMA;
	m_CDXKEYToDIK[CDXKEY_EQUALS]        = DIK_EQUALS;
	m_CDXKEYToDIK[CDXKEY_GREATERTHEN]   = DIK_PERIOD;
	m_CDXKEYToDIK[CDXKEY_QMARK]         = DIK_SLASH;
	m_CDXKEYToDIK[CDXKEY_AT]            = DIK_AT;
	m_CDXKEYToDIK[CDXKEY_CA]            = DIK_A;
	m_CDXKEYToDIK[CDXKEY_CB]            = DIK_B;
	m_CDXKEYToDIK[CDXKEY_CC]            = DIK_C;
	m_CDXKEYToDIK[CDXKEY_CD]            = DIK_D;
	m_CDXKEYToDIK[CDXKEY_CE]            = DIK_E;
	m_CDXKEYToDIK[CDXKEY_CF]            = DIK_F;
	m_CDXKEYToDIK[CDXKEY_CG]            = DIK_G;
	m_CDXKEYToDIK[CDXKEY_CH]            = DIK_H;
	m_CDXKEYToDIK[CDXKEY_CI]            = DIK_I;
	m_CDXKEYToDIK[CDXKEY_CJ]            = DIK_J;
	m_CDXKEYToDIK[CDXKEY_CK]            = DIK_K;
	m_CDXKEYToDIK[CDXKEY_CL]            = DIK_L;
	m_CDXKEYToDIK[CDXKEY_CM]            = DIK_M;
	m_CDXKEYToDIK[CDXKEY_CN]            = DIK_N;
	m_CDXKEYToDIK[CDXKEY_CO]            = DIK_O;
	m_CDXKEYToDIK[CDXKEY_CP]            = DIK_P;
	m_CDXKEYToDIK[CDXKEY_CQ]            = DIK_Q;
	m_CDXKEYToDIK[CDXKEY_CR]            = DIK_R;
	m_CDXKEYToDIK[CDXKEY_CS]            = DIK_S;
	m_CDXKEYToDIK[CDXKEY_CT]            = DIK_T;
	m_CDXKEYToDIK[CDXKEY_CU]            = DIK_U;
	m_CDXKEYToDIK[CDXKEY_CV]            = DIK_V;
	m_CDXKEYToDIK[CDXKEY_CW]            = DIK_W;
	m_CDXKEYToDIK[CDXKEY_CX]            = DIK_X;
	m_CDXKEYToDIK[CDXKEY_CY]            = DIK_Y;
	m_CDXKEYToDIK[CDXKEY_CZ]            = DIK_Z;
	m_CDXKEYToDIK[CDXKEY_LBRACKET]      = DIK_LBRACKET;
	m_CDXKEYToDIK[CDXKEY_BACKSLASH]     = DIK_BACKSLASH;
	m_CDXKEYToDIK[CDXKEY_RBRACKET]      = DIK_RBRACKET;
	m_CDXKEYToDIK[CDXKEY_CARETE]        = DIK_6;
	m_CDXKEYToDIK[CDXKEY_UNDERSCORE]    = DIK_UNDERLINE;
	m_CDXKEYToDIK[CDXKEY_GRAVE]         = DIK_GRAVE;
	m_CDXKEYToDIK[CDXKEY_A]             = DIK_A;
	m_CDXKEYToDIK[CDXKEY_B]             = DIK_B;
	m_CDXKEYToDIK[CDXKEY_C]             = DIK_C;
	m_CDXKEYToDIK[CDXKEY_D]             = DIK_D;
	m_CDXKEYToDIK[CDXKEY_E]             = DIK_E;
	m_CDXKEYToDIK[CDXKEY_F]             = DIK_F;
	m_CDXKEYToDIK[CDXKEY_G]             = DIK_G;
	m_CDXKEYToDIK[CDXKEY_H]             = DIK_H;
	m_CDXKEYToDIK[CDXKEY_I]             = DIK_I;
	m_CDXKEYToDIK[CDXKEY_J]             = DIK_J;
	m_CDXKEYToDIK[CDXKEY_K]             = DIK_K;
	m_CDXKEYToDIK[CDXKEY_L]             = DIK_L;
	m_CDXKEYToDIK[CDXKEY_M]             = DIK_M;
	m_CDXKEYToDIK[CDXKEY_N]             = DIK_N;
	m_CDXKEYToDIK[CDXKEY_O]             = DIK_O;
	m_CDXKEYToDIK[CDXKEY_P]             = DIK_P;
	m_CDXKEYToDIK[CDXKEY_Q]             = DIK_Q;
	m_CDXKEYToDIK[CDXKEY_R]             = DIK_R;
	m_CDXKEYToDIK[CDXKEY_S]             = DIK_S;
	m_CDXKEYToDIK[CDXKEY_T]             = DIK_T;
	m_CDXKEYToDIK[CDXKEY_U]             = DIK_U;
	m_CDXKEYToDIK[CDXKEY_V]             = DIK_V;
	m_CDXKEYToDIK[CDXKEY_W]             = DIK_W;
	m_CDXKEYToDIK[CDXKEY_X]             = DIK_X;
	m_CDXKEYToDIK[CDXKEY_Y]             = DIK_Y;
	m_CDXKEYToDIK[CDXKEY_Z]             = DIK_Z;
	m_CDXKEYToDIK[CDXKEY_LCURLY]        = DIK_LBRACKET;
	m_CDXKEYToDIK[CDXKEY_PIPE]          = DIK_BACKSLASH;
	m_CDXKEYToDIK[CDXKEY_RCURLY]        = DIK_RBRACKET;
	m_CDXKEYToDIK[CDXKEY_TILDA]         = DIK_GRAVE;
	m_CDXKEYToDIK[CDXKEY_DELETE]        = DIK_DELETE;
    m_CDXKEYToDIK[CDXKEY_LEFTSHIFT]     = DIK_LSHIFT;
    m_CDXKEYToDIK[CDXKEY_RIGHTSHIFT]    = DIK_RSHIFT;
    m_CDXKEYToDIK[CDXKEY_LEFTCTRL]      = DIK_LCONTROL;
    m_CDXKEYToDIK[CDXKEY_RIGHTCTRL]     = DIK_RCONTROL;
    m_CDXKEYToDIK[CDXKEY_LEFTALT]       = DIK_LALT;
    m_CDXKEYToDIK[CDXKEY_RIGHTALT]      = DIK_RALT;
    m_CDXKEYToDIK[CDXKEY_LEFTARROW]     = DIK_LEFTARROW;
    m_CDXKEYToDIK[CDXKEY_RIGHTARROW]    = DIK_RIGHTARROW;
    m_CDXKEYToDIK[CDXKEY_UPARROW]       = DIK_UPARROW;
    m_CDXKEYToDIK[CDXKEY_DOWNARROW]     = DIK_DOWNARROW;
    m_CDXKEYToDIK[CDXKEY_F1]            = DIK_F1;
    m_CDXKEYToDIK[CDXKEY_F2]            = DIK_F2;
    m_CDXKEYToDIK[CDXKEY_F3]            = DIK_F3;
    m_CDXKEYToDIK[CDXKEY_F4]            = DIK_F4;
    m_CDXKEYToDIK[CDXKEY_F5]            = DIK_F5;
    m_CDXKEYToDIK[CDXKEY_F6]            = DIK_F6;
    m_CDXKEYToDIK[CDXKEY_F7]            = DIK_F7;
    m_CDXKEYToDIK[CDXKEY_F8]            = DIK_F8;
    m_CDXKEYToDIK[CDXKEY_F9]            = DIK_F9;
    m_CDXKEYToDIK[CDXKEY_F10]           = DIK_F10;
    m_CDXKEYToDIK[CDXKEY_F11]           = DIK_F11;
    m_CDXKEYToDIK[CDXKEY_F12]           = DIK_F12;
    m_CDXKEYToDIK[CDXKEY_INS]           = DIK_INSERT;
    m_CDXKEYToDIK[CDXKEY_DEL]           = DIK_DELETE;
    m_CDXKEYToDIK[CDXKEY_HOME]          = DIK_HOME;
    m_CDXKEYToDIK[CDXKEY_END]           = DIK_END;
    m_CDXKEYToDIK[CDXKEY_PGUP]          = DIK_PRIOR;
    m_CDXKEYToDIK[CDXKEY_PGDN]          = DIK_NEXT;
    m_CDXKEYToDIK[CDXKEY_NUMSLASH]      = DIK_DIVIDE;
    m_CDXKEYToDIK[CDXKEY_NUMSTAR]       = DIK_NUMPADSTAR;
    m_CDXKEYToDIK[CDXKEY_NUMMINUS]      = DIK_NUMPADMINUS;
    m_CDXKEYToDIK[CDXKEY_NUMPLUS]       = DIK_NUMPADPLUS;
    m_CDXKEYToDIK[CDXKEY_NUMENTER]      = DIK_NUMPADENTER;
    m_CDXKEYToDIK[CDXKEY_NUMPERIOD]     = DIK_NUMPADPERIOD;
    m_CDXKEYToDIK[CDXKEY_NUM0]          = DIK_NUMPAD0;
    m_CDXKEYToDIK[CDXKEY_NUM1]          = DIK_NUMPAD1;
    m_CDXKEYToDIK[CDXKEY_NUM2]          = DIK_NUMPAD2;
    m_CDXKEYToDIK[CDXKEY_NUM3]          = DIK_NUMPAD3;
    m_CDXKEYToDIK[CDXKEY_NUM4]          = DIK_NUMPAD4;
    m_CDXKEYToDIK[CDXKEY_NUM5]          = DIK_NUMPAD5;
    m_CDXKEYToDIK[CDXKEY_NUM6]          = DIK_NUMPAD6;
    m_CDXKEYToDIK[CDXKEY_NUM7]          = DIK_NUMPAD7;
    m_CDXKEYToDIK[CDXKEY_NUM8]          = DIK_NUMPAD8;
    m_CDXKEYToDIK[CDXKEY_NUM9]          = DIK_NUMPAD9;
    m_CDXKEYToDIK[CDXKEY_NUMLOCK]       = DIK_NUMLOCK;
    m_CDXKEYToDIK[CDXKEY_CAPSLOCK]      = DIK_CAPITAL;
    m_CDXKEYToDIK[CDXKEY_SCROLLLOCK]    = DIK_SCROLL;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: InitDIKToCDXKEY
// 
// PURPOSE: Since we can't guarantee that the constants for these DIKs won't
//          change, initialize the remap table here instead of statically doing it.
//
// INPUT: none
//
// RETURNS: nothing
//
////////////////////////////////////////////////////////////////////////////////////
void CDXInput::InitDIKToCDXKEY(void)
{
	// any key mapped to zero means unmapped and should be ignored
	memset(m_DIKToCDXKEY, 0, NUMDIKEYS * sizeof(DWORD));

	m_DIKToCDXKEY[DIK_ESCAPE]		= CDXKEY_ESCAPE;
	m_DIKToCDXKEY[DIK_1]			= '1';
	m_DIKToCDXKEY[DIK_2]			= '2';
	m_DIKToCDXKEY[DIK_3]			= '3';
	m_DIKToCDXKEY[DIK_4]			= '4';
	m_DIKToCDXKEY[DIK_5]			= '5';
	m_DIKToCDXKEY[DIK_6]			= '6';
	m_DIKToCDXKEY[DIK_7]			= '7';
	m_DIKToCDXKEY[DIK_8]			= '8';
	m_DIKToCDXKEY[DIK_9]			= '9';
	m_DIKToCDXKEY[DIK_0]			= '0';
	m_DIKToCDXKEY[DIK_MINUS]		= '-';
	m_DIKToCDXKEY[DIK_EQUALS]		= '=';
	m_DIKToCDXKEY[DIK_BACK]			= CDXKEY_BACKSPACE;
	m_DIKToCDXKEY[DIK_TAB]			= CDXKEY_TAB;
	m_DIKToCDXKEY[DIK_Q]			= 'q';
	m_DIKToCDXKEY[DIK_W]			= 'w';
	m_DIKToCDXKEY[DIK_E]			= 'e';
	m_DIKToCDXKEY[DIK_R]			= 'r';
	m_DIKToCDXKEY[DIK_T]			= 't';
	m_DIKToCDXKEY[DIK_Y]			= 'y';
	m_DIKToCDXKEY[DIK_U]			= 'u';
	m_DIKToCDXKEY[DIK_I]			= 'i';
	m_DIKToCDXKEY[DIK_O]			= 'o';
	m_DIKToCDXKEY[DIK_P]			= 'p';
	m_DIKToCDXKEY[DIK_LBRACKET]		= '[';
	m_DIKToCDXKEY[DIK_RBRACKET]		= ']';
	m_DIKToCDXKEY[DIK_RETURN]		= CDXKEY_ENTER;
	m_DIKToCDXKEY[DIK_LCONTROL]		= CDXKEY_LEFTCTRL;
	m_DIKToCDXKEY[DIK_A]			= 'a';
	m_DIKToCDXKEY[DIK_S]			= 's';
	m_DIKToCDXKEY[DIK_D]			= 'd';
	m_DIKToCDXKEY[DIK_F]			= 'f';
	m_DIKToCDXKEY[DIK_G]			= 'g';
	m_DIKToCDXKEY[DIK_H]			= 'h';
	m_DIKToCDXKEY[DIK_J]			= 'j';
	m_DIKToCDXKEY[DIK_K]			= 'k';
	m_DIKToCDXKEY[DIK_L]			= 'l';
	m_DIKToCDXKEY[DIK_SEMICOLON]	= ';';
	m_DIKToCDXKEY[DIK_APOSTROPHE]	= '\'';
	m_DIKToCDXKEY[DIK_GRAVE]		= '`';
	m_DIKToCDXKEY[DIK_LSHIFT]		= CDXKEY_LEFTSHIFT;
	m_DIKToCDXKEY[DIK_BACKSLASH]	= '\\';
	m_DIKToCDXKEY[DIK_Z]			= 'z';
	m_DIKToCDXKEY[DIK_X]			= 'x';
	m_DIKToCDXKEY[DIK_C]			= 'c';
	m_DIKToCDXKEY[DIK_V]			= 'v';
	m_DIKToCDXKEY[DIK_B]			= 'b';
	m_DIKToCDXKEY[DIK_N]			= 'n';
	m_DIKToCDXKEY[DIK_M]			= 'm';
	m_DIKToCDXKEY[DIK_COMMA]		= ',';
	m_DIKToCDXKEY[DIK_PERIOD]		= '.';
	m_DIKToCDXKEY[DIK_SLASH]		= '/';
	m_DIKToCDXKEY[DIK_RSHIFT]		= CDXKEY_RIGHTSHIFT;
	m_DIKToCDXKEY[DIK_MULTIPLY]		= CDXKEY_NUMSTAR;
	m_DIKToCDXKEY[DIK_LMENU]		= CDXKEY_LEFTALT;
	m_DIKToCDXKEY[DIK_SPACE]		= CDXKEY_SPACE;
	m_DIKToCDXKEY[DIK_CAPITAL]		= CDXKEY_CAPSLOCK;
	m_DIKToCDXKEY[DIK_F1]			= CDXKEY_F1;
	m_DIKToCDXKEY[DIK_F2]			= CDXKEY_F2;
	m_DIKToCDXKEY[DIK_F3]			= CDXKEY_F3;
	m_DIKToCDXKEY[DIK_F4]			= CDXKEY_F4;
	m_DIKToCDXKEY[DIK_F5]			= CDXKEY_F5;
	m_DIKToCDXKEY[DIK_F6]			= CDXKEY_F6;
	m_DIKToCDXKEY[DIK_F7]			= CDXKEY_F7;
	m_DIKToCDXKEY[DIK_F8]			= CDXKEY_F8;
	m_DIKToCDXKEY[DIK_F9]			= CDXKEY_F9;
	m_DIKToCDXKEY[DIK_F10]			= CDXKEY_F10;
	m_DIKToCDXKEY[DIK_NUMLOCK]		= CDXKEY_NUMLOCK;
	m_DIKToCDXKEY[DIK_SCROLL]		= CDXKEY_SCROLLLOCK;
	m_DIKToCDXKEY[DIK_NUMPAD7]		= CDXKEY_NUM7;
	m_DIKToCDXKEY[DIK_NUMPAD8]		= CDXKEY_NUM8;
	m_DIKToCDXKEY[DIK_NUMPAD9]		= CDXKEY_NUM9;
	m_DIKToCDXKEY[DIK_SUBTRACT]		= CDXKEY_NUMMINUS;
	m_DIKToCDXKEY[DIK_NUMPAD4]		= CDXKEY_NUM4;
	m_DIKToCDXKEY[DIK_NUMPAD5]		= CDXKEY_NUM5;
	m_DIKToCDXKEY[DIK_NUMPAD6]		= CDXKEY_NUM6;
	m_DIKToCDXKEY[DIK_ADD]			= CDXKEY_NUMPLUS;
	m_DIKToCDXKEY[DIK_NUMPAD1]		= CDXKEY_NUM1;
	m_DIKToCDXKEY[DIK_NUMPAD2]		= CDXKEY_NUM2;
	m_DIKToCDXKEY[DIK_NUMPAD3]		= CDXKEY_NUM3;
	m_DIKToCDXKEY[DIK_NUMPAD0]		= CDXKEY_NUM0;
	m_DIKToCDXKEY[DIK_DECIMAL]		= CDXKEY_NUMPERIOD;
	m_DIKToCDXKEY[DIK_F11]			= CDXKEY_F11;
	m_DIKToCDXKEY[DIK_F12]			= CDXKEY_F12;
	m_DIKToCDXKEY[DIK_NUMPADENTER] = CDXKEY_NUMENTER;
	m_DIKToCDXKEY[DIK_RCONTROL]		= CDXKEY_RIGHTCTRL;
	m_DIKToCDXKEY[DIK_DIVIDE]		= CDXKEY_NUMSLASH;
	m_DIKToCDXKEY[DIK_SYSRQ]		= CDXKEY_PRINTSCRN;
	m_DIKToCDXKEY[DIK_RMENU]		= CDXKEY_RIGHTALT;
	m_DIKToCDXKEY[DIK_HOME]			= CDXKEY_HOME;
	m_DIKToCDXKEY[DIK_UP]			= CDXKEY_UPARROW;
	m_DIKToCDXKEY[DIK_PRIOR]		= CDXKEY_PGUP;
	m_DIKToCDXKEY[DIK_LEFT]			= CDXKEY_LEFTARROW;
	m_DIKToCDXKEY[DIK_RIGHT]		= CDXKEY_RIGHTARROW;
	m_DIKToCDXKEY[DIK_END]			= CDXKEY_END;
	m_DIKToCDXKEY[DIK_DOWN]			= CDXKEY_DOWNARROW;
	m_DIKToCDXKEY[DIK_NEXT]			= CDXKEY_PGDN;
	m_DIKToCDXKEY[DIK_INSERT]		= CDXKEY_INS;
	m_DIKToCDXKEY[DIK_DELETE]		= CDXKEY_DEL;
	m_DIKToCDXKEY[DIK_LWIN]			= 0;
	m_DIKToCDXKEY[DIK_RWIN]			= 0;
	m_DIKToCDXKEY[DIK_APPS]			= 0;
}

//////////////////////////////////////////////////////////////////////////////////
// NAME: InitKeyText
// 
// PURPOSE: Gives each key a textual description.
//
// INPUT: none
//
// RETURNS: nothing
//
// NOTE: DO NOT ADD OR CHANGE A DESCRIPTION SUCH THAT IT EXCEEDS 11
//       CHARACTERS!
// 
/////////////////////////////////////////////////////////////////////////////////////////////
void CDXInput::InitKeyText()
{
    strncpy(&m_keyText[CDXKEY_NULL][0],        "NULL",12);
    strncpy(&m_keyText[CDXKEY_BACKSPACE][0],   "BACKSPACE",12);
    strncpy(&m_keyText[CDXKEY_TAB][0],         "TAB",12);
    strncpy(&m_keyText[CDXKEY_ENTER][0],       "ENTER",12);
    strncpy(&m_keyText[CDXKEY_ESCAPE][0],      "ESC",12);
    strncpy(&m_keyText[CDXKEY_SPACE][0],       "SPACE",12);
    strncpy(&m_keyText[CDXKEY_BANG][0],        "1",12);
    strncpy(&m_keyText[CDXKEY_DQUOTE][0],      "APOSTROPHE",12);
    strncpy(&m_keyText[CDXKEY_POUND][0],       "3",12);
    strncpy(&m_keyText[CDXKEY_DOLLAR][0],      "4",12);
    strncpy(&m_keyText[CDXKEY_PERCENT][0],     "5",12);
    strncpy(&m_keyText[CDXKEY_AMPERSAND][0],   "7",12);
    strncpy(&m_keyText[CDXKEY_APOSTROPHE][0],  "APOSTROPHE",12);
    strncpy(&m_keyText[CDXKEY_LBRACE][0],      "9",12);
    strncpy(&m_keyText[CDXKEY_RBRACE][0],      "0",12);
    strncpy(&m_keyText[CDXKEY_ASTERISC][0],    "8",12);
    strncpy(&m_keyText[CDXKEY_PLUS][0],        "EQUALS",12);
    strncpy(&m_keyText[CDXKEY_COMMA][0],       "COMMA",12);
    strncpy(&m_keyText[CDXKEY_DASH][0],        "UNDERSCORE",12);
    strncpy(&m_keyText[CDXKEY_PERIOD][0],      "PERIOD",12);
    strncpy(&m_keyText[CDXKEY_SLASH][0],       "SLASH",12);
    strncpy(&m_keyText[CDXKEY_0][0],           "0",12);
    strncpy(&m_keyText[CDXKEY_1][0],           "1",12);
    strncpy(&m_keyText[CDXKEY_2][0],           "2",12);
    strncpy(&m_keyText[CDXKEY_3][0],           "3",12);
    strncpy(&m_keyText[CDXKEY_4][0],           "4",12);
    strncpy(&m_keyText[CDXKEY_5][0],           "5",12);
    strncpy(&m_keyText[CDXKEY_6][0],           "6",12);
    strncpy(&m_keyText[CDXKEY_7][0],           "7",12);
    strncpy(&m_keyText[CDXKEY_8][0],           "8",12);
    strncpy(&m_keyText[CDXKEY_9][0],           "9",12);
    strncpy(&m_keyText[CDXKEY_COLON][0],       "SEMICOLON",12);
    strncpy(&m_keyText[CDXKEY_SEMICOLON][0],   "SEMICOLON",12);
    strncpy(&m_keyText[CDXKEY_LESSTHEN][0],    "COMMA",12);
    strncpy(&m_keyText[CDXKEY_EQUALS][0],      "EQUALS",12);
    strncpy(&m_keyText[CDXKEY_GREATERTHEN][0], "PERIOD",12);
    strncpy(&m_keyText[CDXKEY_QMARK][0],       "SLASH",12);
    strncpy(&m_keyText[CDXKEY_AT][0],          "2",12);
    strncpy(&m_keyText[CDXKEY_CA][0],          "A",12);
    strncpy(&m_keyText[CDXKEY_CB][0],          "B",12);
    strncpy(&m_keyText[CDXKEY_CC][0],          "C",12);
    strncpy(&m_keyText[CDXKEY_CD][0],          "D",12);
    strncpy(&m_keyText[CDXKEY_CE][0],          "E",12);
    strncpy(&m_keyText[CDXKEY_CF][0],          "F",12);
    strncpy(&m_keyText[CDXKEY_CG][0],          "G",12);
    strncpy(&m_keyText[CDXKEY_CH][0],          "H",12);
    strncpy(&m_keyText[CDXKEY_CI][0],          "I",12);
    strncpy(&m_keyText[CDXKEY_CJ][0],          "J",12);
    strncpy(&m_keyText[CDXKEY_CK][0],          "K",12);
    strncpy(&m_keyText[CDXKEY_CL][0],          "L",12);
    strncpy(&m_keyText[CDXKEY_CM][0],          "M",12);
    strncpy(&m_keyText[CDXKEY_CN][0],          "N",12);
    strncpy(&m_keyText[CDXKEY_CO][0],          "O",12);
    strncpy(&m_keyText[CDXKEY_CP][0],          "P",12);
    strncpy(&m_keyText[CDXKEY_CQ][0],          "Q",12);
    strncpy(&m_keyText[CDXKEY_CR][0],          "R",12);
    strncpy(&m_keyText[CDXKEY_CS][0],          "S",12);
    strncpy(&m_keyText[CDXKEY_CT][0],          "T",12);
    strncpy(&m_keyText[CDXKEY_CU][0],          "U",12);
    strncpy(&m_keyText[CDXKEY_CV][0],          "V",12);
    strncpy(&m_keyText[CDXKEY_CW][0],          "W",12);
    strncpy(&m_keyText[CDXKEY_CX][0],          "X",12);
    strncpy(&m_keyText[CDXKEY_CY][0],          "Y",12);
    strncpy(&m_keyText[CDXKEY_CZ][0],          "Z",12);
    strncpy(&m_keyText[CDXKEY_LBRACKET][0],    "LEFT BRKT",12);
    strncpy(&m_keyText[CDXKEY_BACKSLASH][0],   "BACKSLASH",12);
    strncpy(&m_keyText[CDXKEY_RBRACKET][0],    "RIGHT BRKT",12);
    strncpy(&m_keyText[CDXKEY_CARETE][0],      "6",12);
    strncpy(&m_keyText[CDXKEY_UNDERSCORE][0],  "UNDERSCORE",12);
    strncpy(&m_keyText[CDXKEY_GRAVE][0],       "GRAVE",12);
    strncpy(&m_keyText[CDXKEY_A][0],           "A",12);
    strncpy(&m_keyText[CDXKEY_B][0],           "B",12);
    strncpy(&m_keyText[CDXKEY_C][0],           "C",12);
    strncpy(&m_keyText[CDXKEY_D][0],           "D",12);
    strncpy(&m_keyText[CDXKEY_E][0],           "E",12);
    strncpy(&m_keyText[CDXKEY_F][0],           "F",12);
    strncpy(&m_keyText[CDXKEY_G][0],           "G",12);
    strncpy(&m_keyText[CDXKEY_H][0],           "H",12);
    strncpy(&m_keyText[CDXKEY_I][0],           "I",12);
    strncpy(&m_keyText[CDXKEY_J][0],           "J",12);
    strncpy(&m_keyText[CDXKEY_K][0],           "K",12);
    strncpy(&m_keyText[CDXKEY_L][0],           "L",12);
    strncpy(&m_keyText[CDXKEY_M][0],           "M",12);
    strncpy(&m_keyText[CDXKEY_N][0],           "N",12);
    strncpy(&m_keyText[CDXKEY_O][0],           "O",12);
    strncpy(&m_keyText[CDXKEY_P][0],           "P",12);
    strncpy(&m_keyText[CDXKEY_Q][0],           "Q",12);
    strncpy(&m_keyText[CDXKEY_R][0],           "R",12);
    strncpy(&m_keyText[CDXKEY_S][0],           "S",12);
    strncpy(&m_keyText[CDXKEY_T][0],           "T",12);
    strncpy(&m_keyText[CDXKEY_U][0],           "U",12);
    strncpy(&m_keyText[CDXKEY_V][0],           "V",12);
    strncpy(&m_keyText[CDXKEY_W][0],           "W",12);
    strncpy(&m_keyText[CDXKEY_X][0],           "X",12);
    strncpy(&m_keyText[CDXKEY_Y][0],           "Y",12);
    strncpy(&m_keyText[CDXKEY_Z][0],           "Z",12);
    strncpy(&m_keyText[CDXKEY_LCURLY][0],      "LEFT BRKT",12);
    strncpy(&m_keyText[CDXKEY_PIPE][0],        "BACKSLASH",12);
    strncpy(&m_keyText[CDXKEY_RCURLY][0],      "RIGHT BRKT",12);
    strncpy(&m_keyText[CDXKEY_TILDA][0],       "GRAVE",12);
    strncpy(&m_keyText[CDXKEY_DELETE][0],      "DELETE",12);
    strncpy(&m_keyText[CDXKEY_LEFTSHIFT][0],   "LEFT SHIFT",12);
    strncpy(&m_keyText[CDXKEY_RIGHTSHIFT][0],  "RIGHT SHIFT",12);
    strncpy(&m_keyText[CDXKEY_LEFTCTRL][0],    "LEFT CTRL",12);
    strncpy(&m_keyText[CDXKEY_RIGHTCTRL][0],   "RIGHT CTRL",12);
    strncpy(&m_keyText[CDXKEY_LEFTALT][0],     "LEFT ALT",12);
    strncpy(&m_keyText[CDXKEY_RIGHTALT][0],    "RIGHT ALT",12);
    strncpy(&m_keyText[CDXKEY_LEFTARROW][0],   "LEFT ARROW",12);
    strncpy(&m_keyText[CDXKEY_RIGHTARROW][0],  "RIGHT ARROW",12);
    strncpy(&m_keyText[CDXKEY_UPARROW][0],     "UP ARROW",12);
    strncpy(&m_keyText[CDXKEY_DOWNARROW][0],   "DOWN ARROW",12);
    strncpy(&m_keyText[CDXKEY_F1][0],          "F1",12);
    strncpy(&m_keyText[CDXKEY_F2][0],          "F2",12);
    strncpy(&m_keyText[CDXKEY_F3][0],          "F3",12);
    strncpy(&m_keyText[CDXKEY_F4][0],          "F4",12);
    strncpy(&m_keyText[CDXKEY_F5][0],          "F5",12);
    strncpy(&m_keyText[CDXKEY_F6][0],          "F6",12);
    strncpy(&m_keyText[CDXKEY_F7][0],          "F7",12);
    strncpy(&m_keyText[CDXKEY_F8][0],          "F8",12);
    strncpy(&m_keyText[CDXKEY_F9][0],          "F9",12);
    strncpy(&m_keyText[CDXKEY_F10][0],         "F10",12);
    strncpy(&m_keyText[CDXKEY_F11][0],         "F11",12);
    strncpy(&m_keyText[CDXKEY_F12][0],         "F12",12);
    strncpy(&m_keyText[CDXKEY_INS][0],         "INSERT",12);
    strncpy(&m_keyText[CDXKEY_DEL][0],         "DEL",12);
    strncpy(&m_keyText[CDXKEY_HOME][0],        "HOME",12);
    strncpy(&m_keyText[CDXKEY_END][0],         "END",12);
    strncpy(&m_keyText[CDXKEY_PGUP][0],        "PAGE UP",12);
    strncpy(&m_keyText[CDXKEY_PGDN][0],        "PAGE DOWN",12);
    strncpy(&m_keyText[CDXKEY_NUMSLASH][0],    "NUM SLASH",12);
    strncpy(&m_keyText[CDXKEY_NUMSTAR][0],     "NUM STAR",12);
    strncpy(&m_keyText[CDXKEY_NUMMINUS][0],    "NUM MINUS",12);
    strncpy(&m_keyText[CDXKEY_NUMPLUS][0],     "NUM PLUS",12);
    strncpy(&m_keyText[CDXKEY_NUMENTER][0],    "NUM ENTER",12);
    strncpy(&m_keyText[CDXKEY_NUMPERIOD][0],   "NUM PERIOD",12);
    strncpy(&m_keyText[CDXKEY_NUM0][0],        "NUM 0",12);
    strncpy(&m_keyText[CDXKEY_NUM1][0],        "NUM 1",12);
    strncpy(&m_keyText[CDXKEY_NUM2][0],        "NUM 2",12);
    strncpy(&m_keyText[CDXKEY_NUM3][0],        "NUM 3",12);
    strncpy(&m_keyText[CDXKEY_NUM4][0],        "NUM 4",12);
    strncpy(&m_keyText[CDXKEY_NUM5][0],        "NUM 5",12);
    strncpy(&m_keyText[CDXKEY_NUM6][0],        "NUM 6",12);
    strncpy(&m_keyText[CDXKEY_NUM7][0],        "NUM 7",12);
    strncpy(&m_keyText[CDXKEY_NUM8][0],        "NUM 8",12);
    strncpy(&m_keyText[CDXKEY_NUM9][0],        "NUM 9",12);
    strncpy(&m_keyText[CDXKEY_NUMLOCK][0],     "NUM LOCK",12);
    strncpy(&m_keyText[CDXKEY_CAPSLOCK][0],    "CAPS LOCK",12);
    strncpy(&m_keyText[CDXKEY_SCROLLLOCK][0],  "SCROLL LOCK",12);
    strncpy(&m_keyText[CDXKEY_PRINTSCRN][0],   "PRINT SCRN",12);
    strncpy(&m_keyText[CDXKEY_PAUSE][0],       "PAUSE",12);
    strncpy(&m_keyText[CDXKEY_MOUSELEFT][0],   "MOUSE LEFT",12);
    strncpy(&m_keyText[CDXKEY_MOUSERIGHT][0],  "MOUSE RIGHT",12);
    strncpy(&m_keyText[CDXKEY_MOUSEMIDDLE][0], "MOUSE MIDLE",12);
    strncpy(&m_keyText[CDXKEY_MOUSEBUTN4][0],  "MOUSE BUTN4",12);
    strncpy(&m_keyText[CDXKEY_JOYBUTN0][0],    "JOY BUTN 0",12);
    strncpy(&m_keyText[CDXKEY_JOYBUTN1][0],    "JOY BUTN 1",12);
    strncpy(&m_keyText[CDXKEY_JOYBUTN2][0],    "JOY BUTN 2",12);
    strncpy(&m_keyText[CDXKEY_JOYBUTN3][0],    "JOY BUTN 3",12);
    strncpy(&m_keyText[CDXKEY_JOYBUTN4][0],    "JOY BUTN 4",12);
    strncpy(&m_keyText[CDXKEY_JOYBUTN5][0],    "JOY BUTN 5",12);
    strncpy(&m_keyText[CDXKEY_JOYBUTN6][0],    "JOY BUTN 6",12);
    strncpy(&m_keyText[CDXKEY_JOYBUTN7][0],    "JOY BUTN 7",12);
    strncpy(&m_keyText[CDXKEY_JOYBUTN8][0],    "JOY BUTN 8",12);
    strncpy(&m_keyText[CDXKEY_JOYBUTN9][0],    "JOY BUTN 9",12);

}