//////////////////////////////////////////////////////////////////////////////////
// $Source: /cvsroot/uaf/UAF/src/cdx/cdxlink.h,v $
// $Author: cocoaspud $
//
// $Log: cdxlink.h,v $
// Revision 1.4  2001/10/13 15:19:59  cocoaspud
// Updates for MSVC ver 7 and other misc
//
// Revision 1.1.1.1  2000/04/22 16:10:25  hebertjo
// Initial checkin of v3.0 to SourceForge CVS.
//
// Revision 2.2  2000/01/13 23:51:04  jhebert
// First checkin for v3.0
// Made the destructor virtual.
//
// 
//
// $Revision: 1.4 $
//////////////////////////////////////////////////////////////////////////////////
#ifndef CDXLINK_H
#define CDXLINK_H

//////////////////////////////////////////////////////////////////////////////////
// CDXLinkSession Structure
//////////////////////////////////////////////////////////////////////////////////
struct CDXLinkSession
{
	char *m_Name;
	GUID  m_Guid;
	GUID  m_AppGuid;
	DWORD m_Flags;
	DWORD m_MaxPlayers;
	DWORD m_CurrentPlayers;
};

//////////////////////////////////////////////////////////////////////////////////
// CDXLinkPlayer Structure
//////////////////////////////////////////////////////////////////////////////////
struct CDXLinkPlayer
{
	DPID m_ID;
	char *m_Name;
	DWORD m_Flags;
	HANDLE m_Event;
};

//////////////////////////////////////////////////////////////////////////////////
// CDXLink Class
//////////////////////////////////////////////////////////////////////////////////
class CDXAPI CDXLink
{
public:
	CDXLink(void);
	virtual ~CDXLink(void);

	BOOL CreateIPX(void* hWnd);
	BOOL CreateTCPIP(void* hWnd, char* IPAddress);
	BOOL CreateModem(void* hWnd, char* PhoneNo);
	BOOL CreateSerial(void* hWnd);

	BOOL EnumSessions(CDXLinkSession* Sessions, GUID APP_GUID);
	BOOL HostSession(CDXLinkSession* Session);
	BOOL JoinSession(CDXLinkSession* Session);
	BOOL CloseSession(void);

	BOOL EnumPlayers(CDXLinkPlayer* Players, DWORD Flags = 0);
	BOOL CreatePlayer(CDXLinkPlayer* Player);
	BOOL DestroyPlayer(CDXLinkPlayer* Player);

	HRESULT Send(DPID idFrom, DPID idTo, DWORD dwFlags,
							 LPVOID lpData, DWORD dwDataSize);
	HRESULT Receive(LPDPID lpidFrom, LPDPID lpidTo,	DWORD dwFlags,
									LPVOID lpData, LPDWORD lpdwDataSize);

public:
	HRESULT rval;
	LPDIRECTPLAY3A m_DP;
	LPDIRECTPLAYLOBBY2A m_DPLobby;
};

#endif
