//////////////////////////////////////////////////////////////////////////////////
// $Source: /cvsroot/uaf/UAF/src/cdx/cdxinput.h,v $
// $Author: cocoaspud $
//
// $Log: cdxinput.h,v $
// Revision 1.5  2002/03/02 13:37:08  cocoaspud
// Completed menu updates for char creation menus
//
// Revision 1.11  2002/01/01 23:42:25  wasteland
// Added support for multiple joysticks
//
// Revision 1.10  2001/01/26 19:55:15  mindcry
// Finally CDX got a way(RawToAscii) for real scancode to character converting.
//
// Revision 1.9  2000/12/04 19:58:44  wasteland
// Added scroll mouse functionality, created by Jacek Ringwelski
//
// Revision 1.8  2000/08/18 14:16:02  mindcry
// Added back in the old Exculsiv... stuff, for backwards compatibility..
// But new users should still use Exclusive now!
//
// Revision 1.7  2000/08/18 13:43:34  mindcry
// All 'Exculsive' typos have been fixed to 'Exclusive'
// Thanks to all reporters of this
//
// Revision 1.6  2000/05/10 23:29:26  hebertjo
// Cleaned up all the old code that was commented out.  Version 3.0 is getting
// very close to release.
//
// Revision 1.5  2000/05/09 01:05:34  hebertjo
// Changed the way SetMouseAccelerator() works.
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
// Revision 2.7  2000/01/19 23:14:30  jhebert
// First checkin for v3.0
// - non-exclusive mouse access is the default now.
//
// 
//
// $Revision: 1.5 $
//////////////////////////////////////////////////////////////////////////////////
#ifndef CDXINPUT_H
#define CDXINPUT_H

// CDX Input Types. Applies to keyboard keys, mouse & joystick buttons.
#define CDXKEY_NONE				0
#define CDXKEY_RELEASE			1
#define CDXKEY_PRESS			2
#define CDXKEY_REPEAT			3

// CDXInput Key constants
enum
{
	CDXKEY_NULL				= 0x00000000,
    CDXKEY_BACKSPACE		= 0x00000008,
    CDXKEY_TAB				= 0x00000009,
    CDXKEY_ENTER			= 0x0000000D,
    CDXKEY_ESCAPE			= 0x0000001B,
    CDXKEY_SPACE			= 0x00000020,

	// Printable characters
	CDXKEY_BANG,
	CDXKEY_DQUOTE,
	CDXKEY_POUND,
	CDXKEY_DOLLAR,
	CDXKEY_PERCENT,
	CDXKEY_AMPERSAND,
	CDXKEY_APOSTROPHE,
	CDXKEY_LBRACE,
	CDXKEY_RBRACE,
	CDXKEY_ASTERISC,
	CDXKEY_PLUS,
	CDXKEY_COMMA,
	CDXKEY_DASH,
	CDXKEY_PERIOD,
	CDXKEY_SLASH,
    CDXKEY_0,
	CDXKEY_1,
	CDXKEY_2,
	CDXKEY_3,
	CDXKEY_4,
	CDXKEY_5,
	CDXKEY_6,
	CDXKEY_7,
	CDXKEY_8,
	CDXKEY_9,
	CDXKEY_COLON,
	CDXKEY_SEMICOLON,
	CDXKEY_LESSTHEN,
	CDXKEY_EQUALS,
	CDXKEY_GREATERTHEN,
	CDXKEY_QMARK,
	CDXKEY_AT,
	CDXKEY_CA,
	CDXKEY_CB,
	CDXKEY_CC,
	CDXKEY_CD,
	CDXKEY_CE,
	CDXKEY_CF,
	CDXKEY_CG,
	CDXKEY_CH,
	CDXKEY_CI,
	CDXKEY_CJ,
	CDXKEY_CK,
	CDXKEY_CL,
	CDXKEY_CM,
	CDXKEY_CN,
	CDXKEY_CO,
	CDXKEY_CP,
	CDXKEY_CQ,
	CDXKEY_CR,
	CDXKEY_CS,
	CDXKEY_CT,
	CDXKEY_CU,
	CDXKEY_CV,
	CDXKEY_CW,
	CDXKEY_CX,
	CDXKEY_CY,
	CDXKEY_CZ,
	CDXKEY_LBRACKET,
	CDXKEY_BACKSLASH,
	CDXKEY_RBRACKET,
	CDXKEY_CARETE,
	CDXKEY_UNDERSCORE,
	CDXKEY_GRAVE,
	CDXKEY_A,
	CDXKEY_B,
	CDXKEY_C,
	CDXKEY_D,
	CDXKEY_E,
	CDXKEY_F,
	CDXKEY_G,
	CDXKEY_H,
	CDXKEY_I,
	CDXKEY_J,
	CDXKEY_K,
	CDXKEY_L,
	CDXKEY_M,
	CDXKEY_N,
	CDXKEY_O,
	CDXKEY_P,
	CDXKEY_Q,
	CDXKEY_R,
	CDXKEY_S,
	CDXKEY_T,
	CDXKEY_U,
	CDXKEY_V,
	CDXKEY_W,
	CDXKEY_X,
	CDXKEY_Y,
	CDXKEY_Z,
	CDXKEY_LCURLY,
	CDXKEY_PIPE,
	CDXKEY_RCURLY,
	CDXKEY_TILDA,
	CDXKEY_DELETE,

	// modifier keys
    CDXKEY_LEFTSHIFT		= 0x00000080,
    CDXKEY_RIGHTSHIFT,
	CDXKEY_LEFTCTRL,
	CDXKEY_RIGHTCTRL,
    CDXKEY_LEFTALT,
    CDXKEY_RIGHTALT,
	
	// arrow keys
	CDXKEY_LEFTARROW,
    CDXKEY_RIGHTARROW,
    CDXKEY_UPARROW,
    CDXKEY_DOWNARROW,
    
	// function keys
	CDXKEY_F1,
    CDXKEY_F2,
    CDXKEY_F3,
    CDXKEY_F4,
    CDXKEY_F5,
    CDXKEY_F6,
    CDXKEY_F7,
    CDXKEY_F8,
    CDXKEY_F9,
    CDXKEY_F10,
    CDXKEY_F11,
    CDXKEY_F12,

	// cursor control keys
    CDXKEY_INS,
    CDXKEY_DEL,
    CDXKEY_HOME,
    CDXKEY_END,
    CDXKEY_PGUP,
    CDXKEY_PGDN,
	
	// numeric keypad
    CDXKEY_NUMSLASH,
    CDXKEY_NUMSTAR,
    CDXKEY_NUMMINUS,
    CDXKEY_NUMPLUS,
    CDXKEY_NUMENTER,
    CDXKEY_NUMPERIOD,
    CDXKEY_NUM0,
    CDXKEY_NUM1,
    CDXKEY_NUM2,
    CDXKEY_NUM3,
    CDXKEY_NUM4,
    CDXKEY_NUM5,
    CDXKEY_NUM6,
    CDXKEY_NUM7,
    CDXKEY_NUM8,
    CDXKEY_NUM9,
    
	// locks and misc keys
	CDXKEY_NUMLOCK,
    CDXKEY_CAPSLOCK,
    CDXKEY_SCROLLLOCK,
    CDXKEY_PRINTSCRN,
    CDXKEY_PAUSE,
	
	// windows keys deliberately not listed

	// characters 256 and up used for mouse and joystick buttons etc.
	CDXKEY_MOUSELEFT		= 0x00000100,
	CDXKEY_MOUSERIGHT,
	CDXKEY_MOUSEMIDDLE,
	CDXKEY_MOUSEBUTN4,

	// Joystick buttons for each joystick, joy0 is left as just "joy" for backwards compatibility
#define MAX_JOYSTICK_BUTTONS 10
	CDXKEY_JOYBUTN0,
	CDXKEY_JOYBUTN1,
	CDXKEY_JOYBUTN2,
	CDXKEY_JOYBUTN3,
	CDXKEY_JOYBUTN4,
	CDXKEY_JOYBUTN5,
	CDXKEY_JOYBUTN6,
	CDXKEY_JOYBUTN7,
	CDXKEY_JOYBUTN8,
	CDXKEY_JOYBUTN9,

	CDXKEY_JOY1BUTN0 = (CDXKEY_JOYBUTN0 + MAX_JOYSTICK_BUTTONS),
	CDXKEY_JOY1BUTN1,CDXKEY_JOY1BUTN2,CDXKEY_JOY1BUTN3,CDXKEY_JOY1BUTN4,CDXKEY_JOY1BUTN5,
	CDXKEY_JOY1BUTN6,CDXKEY_JOY1BUTN7,CDXKEY_JOY1BUTN8,CDXKEY_JOY1BUTN9,

	CDXKEY_JOY2BUTN0 = (CDXKEY_JOYBUTN0 + 2*MAX_JOYSTICK_BUTTONS),
	CDXKEY_JOY2BUTN1,CDXKEY_JOY2BUTN2,CDXKEY_JOY2BUTN3,CDXKEY_JOY2BUTN4,CDXKEY_JOY2BUTN5,
	CDXKEY_JOY2BUTN6,CDXKEY_JOY2BUTN7,CDXKEY_JOY2BUTN8,CDXKEY_JOY2BUTN9,

	CDXKEY_JOY3BUTN0 = (CDXKEY_JOYBUTN0 + 3*MAX_JOYSTICK_BUTTONS),
	CDXKEY_JOY3BUTN1,CDXKEY_JOY3BUTN2,CDXKEY_JOY3BUTN3,CDXKEY_JOY3BUTN4,CDXKEY_JOY3BUTN5,
	CDXKEY_JOY3BUTN6,CDXKEY_JOY3BUTN7,CDXKEY_JOY3BUTN8,CDXKEY_JOY3BUTN9,

	CDXKEY_JOY4BUTN0 = (CDXKEY_JOYBUTN0 + 4*MAX_JOYSTICK_BUTTONS),
	CDXKEY_JOY4BUTN1,CDXKEY_JOY4BUTN2,CDXKEY_JOY4BUTN3,CDXKEY_JOY4BUTN4,CDXKEY_JOY4BUTN5,
	CDXKEY_JOY4BUTN6,CDXKEY_JOY4BUTN7,CDXKEY_JOY4BUTN8,CDXKEY_JOY4BUTN9,

	CDXKEY_JOY5BUTN0 = (CDXKEY_JOYBUTN0 + 5*MAX_JOYSTICK_BUTTONS),
	CDXKEY_JOY5BUTN1,CDXKEY_JOY5BUTN2,CDXKEY_JOY5BUTN3,CDXKEY_JOY5BUTN4,CDXKEY_JOY5BUTN5,
	CDXKEY_JOY5BUTN6,CDXKEY_JOY5BUTN7,CDXKEY_JOY5BUTN8,CDXKEY_JOY5BUTN9,

	CDXKEY_JOY6BUTN0 = (CDXKEY_JOYBUTN0 + 6*MAX_JOYSTICK_BUTTONS),
	CDXKEY_JOY6BUTN1,CDXKEY_JOY6BUTN2,CDXKEY_JOY6BUTN3,CDXKEY_JOY6BUTN4,CDXKEY_JOY6BUTN5,
	CDXKEY_JOY6BUTN6,CDXKEY_JOY6BUTN7,CDXKEY_JOY6BUTN8,CDXKEY_JOY6BUTN9,

	CDXKEY_JOY7BUTN0 = (CDXKEY_JOYBUTN0 + 7*MAX_JOYSTICK_BUTTONS),
	CDXKEY_JOY7BUTN1,CDXKEY_JOY7BUTN2,CDXKEY_JOY7BUTN3,CDXKEY_JOY7BUTN4,CDXKEY_JOY7BUTN5,
	CDXKEY_JOY7BUTN6,CDXKEY_JOY7BUTN7,CDXKEY_JOY7BUTN8,CDXKEY_JOY7BUTN9,

	// last keytype defined
	CDXKEY_NUMKEYS
};

#define NUMDIKEYS 256
#define CDXKEY_MOUSESCROLLUP	0
#define CDXKEY_MOUSESCROLLDOWN	1


// In order to support more joysticks, the enumeration above must be expanded, and
// this defined changed.
#define MAX_JOYSTICKS 8

//////////////////////////////////////////////////////////////////////////////////
// CDXInput is a class wrapper for DirectInput and contains functions to receive 
// data from the mouse, keyboard and joystick(s).
//////////////////////////////////////////////////////////////////////////////////
class CDXAPI CDXInput
{
public:
	CDXInput(void);
	virtual ~CDXInput(void);

	HRESULT Create(void *hInst, void *hWnd);
	HRESULT Update(void);
	void    UpdateLastKey( BOOL reset );
	void    FlushKeyboardData();
	void    SetMouseLimits(LONG x1, LONG y1, LONG x2, LONG y2);
	void    SetMouseSensitivity(float factor);
    void    SetMousePos(LONG x, LONG y);
	void    SetMouseFreePos(LONG x, LONG y);
	void    GetMousePos(LONG* x, LONG* y);
	void    GetMouseFreePos(LONG* x, LONG* y);
	void    GetMouseDeltas(LONG* x, LONG* y);
	bool	GetMouseScroll(WORD  _CDXKey, UINT * num = NULL); // for scroll mouse
	HRESULT RunMouseControlPanel(void *hWnd = NULL);
    HRESULT SetActiveDevices(BOOL bMouse, BOOL bKeyboard, BOOL bJoystick);
	HRESULT SetMouseAbs(void);

	// Those three are only in for backwards compatibility (Mindcry: which one?)
    HRESULT SetMouseExculsive(BOOL bExclusive) { return CDXInput::SetMouseExclusive(bExclusive); };
    HRESULT SetKeyboardExculsive(BOOL bExclusive) { return CDXInput::SetKeyboardExclusive(bExclusive); };
    HRESULT SetJoystickExculsive(BOOL bExclusive) { return CDXInput::SetJoystickExclusive(0, bExclusive); };
    HRESULT SetMouseExclusive(BOOL bExclusive);
    HRESULT SetKeyboardExclusive(BOOL bExclusive);
    BYTE    GetLastKey( void ) { return m_lastKey; };
	WORD    GetShiftedKeyState(WORD key);
	WORD    GetKeyState(WORD key);
	WORD    GetKeyRaw(WORD key);
    void    GetKeyDesc( DWORD Checkkey, char *str );
	char	RawToAscii( DWORD rawkey );
	float   GetMouseSensitivity();
    BOOL    HasMouse( void )    { return m_bMouse; }
    DWORD   GetLastKeyChar( void );
    DWORD   CDXKEYToDIK(DWORD cdxkey) {if(cdxkey > NUMDIKEYS) return 0; return m_CDXKEYToDIK[cdxkey];}
    HRESULT GetKeyName(DWORD cdxkey, char* buf, DWORD buflength);
    void    SetMouseAccelerator(BOOL onOff, DWORD threshold1 = 0, double mult1 = 1.0, DWORD threshold2 = 0, double mult2 = 1.0);  

	// JoySticks
    BOOL    HasJoystick( void ) { return m_bJoystick; }
	int		GetNumJoysticks() { return m_iJoystickCount; }
	HRESULT RunJoystickControlPanel(int iJoyNum, void *hWnd = NULL);
    HRESULT SetJoystickExclusive(int iJoyNum, BOOL bExclusive);
    void    SetJoystickLimits(int iJoyNum, LONG x1, LONG y1, LONG x2, LONG y2);
	void    SetJoystickSensitivity(int iJoyNum, float factor);
	void    SetJoystickPos(int iJoyNum, LONG x, LONG y);
	void    SetJoystickFreePos(int iJoyNum, LONG x, LONG y);
	void    GetJoystickPos(int iJoyNum, LONG* x, LONG* y);
	void    GetJoystickFreePos(int iJoyNum, LONG* x, LONG* y);
	void    GetJoystickDeltas(int iJoyNum, LONG* x, LONG* y);
	float   GetJoystickSensitivity( int iJoyNum );
	HRESULT SetJoystickAbs( int iJoyNum );
    BOOL    IsGamePad( int iJoyNum );
    DWORD   GetNumberOfJoystickButtons(int iJoyNum );
    DWORD   GetJoystickType( int iJoyNum ) { return m_JoystickType[iJoyNum]; }

	//Methods for backwards compatibility - accesses the first joystick only
	HRESULT RunJoystickControlPanel(void *hWnd = NULL)	{ return CDXInput::RunJoystickControlPanel(0, hWnd); }
    HRESULT SetJoystickExclusive(BOOL bExclusive)		{ return CDXInput::SetJoystickExclusive(0, bExclusive); }
    void    SetJoystickLimits(LONG x1, LONG y1, LONG x2, LONG y2) { CDXInput::SetJoystickLimits(0, x1, y1, x2, y2); }
	void    SetJoystickSensitivity(float factor)		{ CDXInput::SetJoystickSensitivity(0, factor); }
	void    SetJoystickPos(LONG x, LONG y)				{ CDXInput::SetJoystickPos(0, x, y); }
	void    SetJoystickFreePos(LONG x, LONG y)			{ CDXInput::SetJoystickFreePos(0, x, y); }
	void    GetJoystickPos(LONG* x, LONG* y)			{ CDXInput::GetJoystickPos(0, x, y); }
	void    GetJoystickFreePos(LONG* x, LONG* y)		{ CDXInput::GetJoystickFreePos(0, x, y); }
	void    GetJoystickDeltas(LONG* x, LONG* y)			{ CDXInput::GetJoystickDeltas(0, x, y); }
	float   GetJoystickSensitivity()					{ return CDXInput::GetJoystickSensitivity(0); }
	HRESULT SetJoystickAbs()							{ return CDXInput::SetJoystickAbs(0); }
    BOOL    IsGamePad()									{ return CDXInput::IsGamePad(0); }
    DWORD   GetNumberOfJoystickButtons()				{ return CDXInput::GetNumberOfJoystickButtons(0); }
    DWORD   GetJoystickType()							{ return CDXInput::GetJoystickType(0); }

private:
	void    InitDIKToCDXKEY();
    void    InitCDXKEYToDIK();
    void    InitShiftedKeys();
    void    InitKeyText();
    void    CheckKeyEvents(DWORD key, BOOL isPressed);
	HRESULT AcquireMouse();
	HRESULT UnacquireMouse();
	HRESULT AcquireKeyboard();
	HRESULT UnacquireKeyboard();
	HRESULT AcquireJoystick(int iJoyNum);
	HRESULT UnacquireJoystick(int iJoyNum);

private:
	LPDIRECTINPUT        m_lpDI;
	LPDIRECTINPUTDEVICE  m_lpDIDKeyboard;
	LPDIRECTINPUTDEVICE  m_lpDIDMouse;

#if DIRECTINPUT_VERSION >= CDX_DIVER
	LPDIRECTINPUTDEVICE2 m_lpDIDJoystick[MAX_JOYSTICKS];
#endif

	int    m_iJoystickCount;
	BOOL   m_bMouse;
	BOOL   m_bKeyboard;
	BOOL   m_bJoystick;
    DWORD  m_JoystickType[MAX_JOYSTICKS];
	float  m_mouseSensitivity;
	float  m_joystickSensitivity[MAX_JOYSTICKS];
	LONG   m_mouseMinX;
	LONG   m_mouseMinY;
	LONG   m_mouseMaxX;
	LONG   m_mouseMaxY;
	LONG   m_joystickMinX[MAX_JOYSTICKS];
	LONG   m_joystickMinY[MAX_JOYSTICKS];
	LONG   m_joystickMaxX[MAX_JOYSTICKS];
	LONG   m_joystickMaxY[MAX_JOYSTICKS];
	LONG   m_mouseX, m_mouseY;
	LONG   m_mouseFreeX, m_mouseFreeY;
	LONG   m_mouseDeltaX, m_mouseDeltaY;
	LONG   m_joystickX[MAX_JOYSTICKS], m_joystickY[MAX_JOYSTICKS];
	LONG   m_joystickFreeX[MAX_JOYSTICKS], m_joystickFreeY[MAX_JOYSTICKS];
	LONG   m_joystickDeltaX[MAX_JOYSTICKS], m_joystickDeltaY[MAX_JOYSTICKS];
	BYTE   m_keyStates[CDXKEY_NUMKEYS];
	BYTE   m_keyRaw[NUMDIKEYS];
	BYTE   m_oldKeyRaw[NUMDIKEYS];
	bool   m_keyScroll[2];	// for scroll mouse
	BYTE   m_ScrollAmount[2];	// for scroll mouse
	DWORD  m_keyPressTimes[CDXKEY_NUMKEYS];
	DWORD  m_keyDragStartPositions[CDXKEY_NUMKEYS][2];
    BYTE   m_lastKey;
	BYTE   m_shiftedKeyStates[CDXKEY_NUMKEYS];
	DWORD  m_DIKToCDXKEY[NUMDIKEYS];
    HWND   m_hWnd;
    DWORD  m_CDXKEYToDIK[NUMDIKEYS];
    char   m_keyText[CDXKEY_NUMKEYS][12];
    LONG   m_mouseThreshold1;
    LONG   m_mouseThreshold2;
    double m_mouseMultiplier1;
    double m_mouseMultiplier2;
};

#endif
