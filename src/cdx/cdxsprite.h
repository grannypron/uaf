//////////////////////////////////////////////////////////////////////////////////
// $Source: /cvsroot/uaf/UAF/src/cdx/cdxsprite.h,v $
// $Author: cocoaspud $
//
// $Log: cdxsprite.h,v $
// Revision 1.5  2001/10/13 15:19:59  cocoaspud
// Updates for MSVC ver 7 and other misc
//
// Revision 1.11  2001/01/11 14:38:49  mindcry
// Added CountFrames, which tells you the number of frames in a CDXSprite.
//
// Revision 1.10  2000/11/29 21:37:47  wasteland
// Changed m_angle from an int to a double to match up with what is passed to CDXSurface::RotoZoom
//
// Revision 1.9  2000/09/30 19:30:08  istan
// Moved private vars to be protected instead for subclasses.
//
// Revision 1.8  2000/09/07 05:06:52  istan
// Added default value for MemoryType in Create()
//
// Revision 1.7  2000/05/31 17:47:55  hebertjo
// Added some Accessor functions
//
// Revision 1.6  2000/05/12 22:48:02  hebertjo
// I added RotoZoomimg to CDXSurface and CDXSprite.  I ripped this code from NukeDX.
// IT works and as been debuged by them but it is slow.  CDX definitely needed this
// feature and it did not take long to CDX'ize their code so I added it.  I plan
// on changing this to a faster implementation once I finish it.
//
// Revision 1.5  2000/05/10 23:29:26  hebertjo
// Cleaned up all the old code that was commented out.  Version 3.0 is getting
// very close to release.
//
// Revision 1.4  2000/05/09 01:22:06  hebertjo
// Moved the CDXBLT_XXXX #defines to cdxsurface.h.  This is where they belong, thanks
// to who ever pointed that out.
//
// Revision 1.3  2000/05/01 17:06:26  hebertjo
// Many small changes.  Fixed a few problems identified by BoundsChecker.
// Removed some old code that was commented out.  Cleaned up the look of
// some of the code.
//
// Revision 1.2  2000/04/26 18:45:43  hebertjo
// Made many small changes to fix numerous bugs and some enhancements.
//
// Revision 1.1.1.1  2000/04/22 16:12:01  hebertjo
// Initial checkin of v3.0 to SourceForge CVS.
//
// Revision 2.10  2000/03/20 21:11:23  wasteland
// Added methods to get the Position and Velocity
//
// Revision 2.9  2000/01/21 01:34:25  jhebert
// Changed m_Tile to GetTile().
//
// Revision 2.8  2000/01/13 23:43:17  jhebert
// First checkin for v3.0.  No major changes, added some safety chacks and all
// the functions have headers.
//
// 
//
// $Revision: 1.5 $
//////////////////////////////////////////////////////////////////////////////////
#ifndef CDXSPRITE_H
#define CDXSPRITE_H

#include "cdxtile.h"
#include "cdxmap.h"

//////////////////////////////////////////////////////////////////////////////////
// This class contains the data and functions required to display animated sprites.
// A CDXSprite object is created from a pointer to a CDXTile object which is used 
// to store the bitmap data for the sprite. All the sprite's frames must be the 
// same width and height and stored in the same bitmap file.
//////////////////////////////////////////////////////////////////////////////////
class CDXAPI CDXSprite
{
public:
	CDXSprite(void);
	virtual ~CDXSprite(void);

	HRESULT Create(CDXTile* pTile);
	HRESULT Create(CDXScreen *pScreen, const char* Filename, int w, int h, int num, BYTE memoryType = CDXMEM_VIDTHENSYS);

	void SetPos(int pX, int pY) { m_PosX = pX; m_PosY = pY; }
	void SetPosX(int pX) { m_PosX = pX; }
	void SetPosY(int pY) { m_PosY = pY; }
	void GetPos(int &pX, int &pY) const { pX = m_PosX; pY = m_PosY; }
    int  GetPosX(void) const { return m_PosX; }
    int  GetPosY(void) const { return m_PosY; }
	void SetVel(int vX, int vY) { m_VelX = vX; m_VelY = vY; }
	void SetVelX(int vX) { m_VelX = vX; }
	void SetVelY(int vY) { m_VelY = vY; }
	void GetVel(int &vX, int &vY) const { vX = m_VelX; vY = m_VelY; }
    int  GetVelX(void) const { return m_VelX; }
    int  GetVelY(void) const { return m_VelY; }
	void SetFrame(int frame) { m_Frame = frame; }
    int  GetFrame(void) { return m_Frame; }
	int  CountFrames(void) { return m_Tile->GetBlockCount(); }
	void SetDelay(int delay) { m_Delay = delay; }
    int  GetDelay(void) {return m_Delay;}
	void SetState(int state) { m_State = state; }
    int  GetState(void) {return m_State;}
	void SetType(int type) { m_Type = type; }
    int  GetType(void) {return m_Type;}
    void SetAngle(double angle) { m_Angle = angle; }
    double GetAngle(void) {return m_Angle;}
    void SetScale(float scale) { m_ScaleFactor = scale; }
    float GetScale(void) {return m_ScaleFactor;}
    void SetShadowOffset(LONG dx, LONG dy);
	void SetShadowValue(WORD Shade) { m_ShadowValue = Shade; }
    WORD GetShadowValue(void) { return m_ShadowValue; }
	void SetAlphaValue(WORD Shade) { m_AlphaValue = Shade; }
    WORD GetAlphaValue(void) { return m_AlphaValue; }
	void SetColorKey(DWORD Col) { GetTile()->SetColorKey(Col); }
	void SetColorKey(void) { m_Tile->SetColorKey(); }
    DWORD GetColorKey(void) { return m_Tile->GetColorKey(); }
    void SetStretchWidth(WORD width) { m_StretchedWidth = width; }
    WORD GetStretchWidth(void) { return m_StretchedWidth; }
    void SetStretchHeight(WORD height) { m_StretchedHeight = height; }
    WORD GetStretchHeight(void) { return m_StretchedHeight; }
    void SetNext(CDXSprite* lpCDXS) { m_Next = lpCDXS; }
    void SetPrev(CDXSprite* lpCDXS) { m_Prev = lpCDXS; }
    CDXSprite* GetNext(void) { return m_Next; }
    CDXSprite* GetPrev(void) { return m_Prev; }
    void       SetTile(CDXTile* lpTile);
    CDXTile*   GetTile(void) { return m_Tile; }    
	HRESULT Lock(void) { return m_Tile->Lock(); }
	HRESULT UnLock(void) { return m_Tile->UnLock(); }
	BOOL    SpriteHit(CDXSprite* pSprite);
	BOOL    TileHit(CDXMap* pMap, int Tile);
	BOOL    SpriteHitPixel(CDXSprite* pSprite);
    BOOL    IsFlipped(void) { return m_Flipped; }
	HRESULT Draw(CDXSurface* lpCDXS, DWORD ScrnWorldX, DWORD ScrnWorldY, WORD BltType);
  HRESULT Blit(CDXSurface* lpCDXS, const RECT *srcRect, const RECT *dstRect, WORD BltType);

protected:
	void Init(void);
    inline HRESULT BlkBlt(CDXSurface* lpDDSDest, RECT SrcRect, RECT DestRect);
	inline HRESULT TransBlt(CDXSurface* lpDDS, RECT SrcRect, RECT DestRect);
	inline HRESULT TransHFlip(CDXSurface* lpCDXSDest, RECT SrcRect, RECT DestRect);
	inline HRESULT BlkHFlip(CDXSurface* lpCDXSDest, RECT SrcRect, RECT DestRect);
	inline HRESULT TransVFlip(CDXSurface* lpCDXSDest, RECT SrcRect, RECT DestRect);
	inline HRESULT BlkVFlip(CDXSurface* lpCDXSDest, RECT SrcRect, RECT DestRect);
	inline HRESULT BlkAlpha(CDXSurface* dest, RECT srcRect, RECT destRect);
	inline HRESULT BlkAlphaFast(CDXSurface* dest, RECT srcRect, RECT destRect);
	inline HRESULT BlkShadow(CDXSurface* dest, RECT srcRect, RECT destRect);
	inline HRESULT BlkShadowFast(CDXSurface* dest, RECT srcRect, RECT destRect);
	inline HRESULT TransAlpha(CDXSurface* dest, RECT srcRect, RECT destRect);
	inline HRESULT TransAlphaFast(CDXSurface* dest, RECT srcRect, RECT destRect);
	inline HRESULT TransShadow(CDXSurface* dest, RECT srcRect, RECT destRect);
	inline HRESULT TransShadowFast(CDXSurface* dest, RECT srcRect, RECT destRect);
	inline HRESULT TransAlphaMask(CDXSurface* dest, RECT srcRect, RECT destRect);
	inline HRESULT TransScaled(CDXSurface* lpCDXSDest, RECT SrcRect, RECT DestRect);
	inline HRESULT BlkScaled(CDXSurface* lpCDXSDest, RECT SrcRect, RECT DestRect);
	inline HRESULT TransStretched(CDXSurface* lpCDXSDest, RECT SrcRect, RECT DestRect);
	inline HRESULT BlkStretched(CDXSurface* lpCDXSDest, RECT SrcRect, RECT DestRect);
    inline HRESULT BlkRotoZoom(CDXSurface* lpCDXSDest, RECT SrcRect, RECT DestRect);
    inline HRESULT TransRotoZoom(CDXSurface* lpCDXSDest, RECT SrcRect, RECT DestRect);


// MichaelR: Did on request so subclasses would be able to access these variables
// instead of having to use the slower accessor function, but of course at their own
// risk of them changing in later revs.
protected:
	int m_PosX;				// The sprite's X position
	int m_PosY;				// The sprite's Y position
	int m_PosZ;             // The sprite's Z position
	int m_VelX;				// The sprite's X velocity
	int m_VelY;				// The sprite's Y velocity
	int m_Frame;			// The current frame
	int m_Delay;			// Used for game timing, the time till the next frame
	int m_State;			// User defined state.  Walking, jumping, etc
	int m_Type;				// User defined type.  Health, weapon, etc
	double m_Angle;			// The sprite's angle of rotation
	float m_ScaleFactor;	// The amount by which to scale the sprite if Drawing with CDXBLT_XXXSCALED
	BOOL m_Flipped;			// Is the sprite flipped?
	LONG m_ShadowOffsetX;   // Amount to offset the shadow in the X direction
	LONG m_ShadowOffsetY;   // Amount to offset the shadow in the Y direction
	WORD m_ShadowValue;     // Alpha value used in drawing the shadow 0-256
	WORD m_AlphaValue;      // Alpha value used when alpha blending 0-256
	BOOL m_TileCreated;     // Used internal to determine if the class created the CDXTile object or not
	WORD m_StretchedHeight; // Used to define the height of the stretched sprite 
	WORD m_StretchedWidth;  // Used to define the width of the stretched sprite
	CDXTile*   m_Tile;		// A CDXTile pointer to the sprite's bitmap data
	CDXSprite* m_Next;		// A CDXSprite pointer to the next sprite in a CDXSpriteList
	CDXSprite* m_Prev;		// A CDXSprite pointer to the previous sprite in a CDXSpriteList
};




//////////////////////////////////////////////////////////////////////////////////
// Sprite lists are used to store sprites in a dynamic fashion. A CDXSpriteList 
// is a linked list of CDXSprite objects, which can be added and removed at run-time.
//
// LOOPING THE LIST: When using a sprite list you will often need to loop through 
// the list and perform operations on each of the sprites in the list, such as 
// updating their positions.  The code below shows how to loop through a CDXSpriteList:
//
// CDXSpriteList SpriteList;
// CDXSprite* Node;
//
// CDXSprite* Save;
//
// for(Node = SpriteList.GetFirst(); Node != NULL; Node = SpriteList.GetNext(Node))
// {
//     Node->Draw(lpCDXS, ScrnWorldX, ScrnWorldY, BltType);
// }
// 
// Notice that if you intend to remove sprites from the list as you update them you 
// must save a pointer to the next sprite in the list to prevent the loop from 
// becoming lost.
//
//////////////////////////////////////////////////////////////////////////////////
class CDXAPI CDXSpriteList
{
public:
	CDXSpriteList(void);
	virtual ~CDXSpriteList(void);

    void AddSprite(CDXSprite* pSprite) { m_spriteList.Add(pSprite); }
    void DelSprite(CDXSprite* pSprite) { m_spriteList.Remove(pSprite, TRUE); }
    LONG GetCount(void) { return m_spriteList.GetCount(); }
    CDXSprite* GetFirst(void) { return m_spriteList.GetFirst(); }
    CDXSprite* GetNext(CDXSprite* pSprite) { return m_spriteList.Next(pSprite); }
	virtual HRESULT Draw(CDXSurface* lpCDXS, DWORD ScrnWorldX, DWORD ScrnWorldY, WORD BltType);

private:
	CDXCList<CDXSprite> m_spriteList;  // Linked List template
};

#endif
