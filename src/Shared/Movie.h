/********************************************************************
 *
 *   Author:			Robert Karlsson
 *
 *   Module:			Movie.h
 *
 *   $Workfile: $
 *   $Revision: 1.3 $
 *   $Date: 2002/02/07 13:09:54 $
 *
 *   Description:		Interface for the Movie class.
 *
 *   Dependencies:	
 *
 ********************************************************************/

#if !defined(AFX_MOVIE_H__EC559780_FB30_11D2_BDF5_0000E86752EE__INCLUDED_)
#define AFX_MOVIE_H__EC559780_FB30_11D2_BDF5_0000E86752EE__INCLUDED_

/*** Include files **************************************************/

/*** Defines ********************************************************/

/*** Macros *********************************************************/

/*** Data types definitions and typedefs ****************************/

/*** External class declarations ************************************/
//class CDXSurface;
//class CDXScreen;

/*** External interface declarations ********************************/
//struct IMultiMediaStream; 
//struct IMediaStream;
//struct IDirectDrawMediaStream;
//struct IDirectDrawStreamSample;
//struct IDirectDrawSurface;

/********************************************************************/
/*** Class definitions **********************************************/
/********************************************************************/
class Movie  
{
/*** Public Member Functions ****************************************/
public:
	Movie(CDXScreen* pScreen);
	virtual ~Movie();

	bool Create(const char* szFilename, CDXSurface *pSurface);
	bool Play();
	bool Stop();
	bool Destroy();
	bool Update(RECT *pRect=NULL);
  RECT GetSrcRect() const { return m_srcRect; }

/*** Protected Member Functions *************************************/
protected:

/*** Protected Member Variables *************************************/
protected:

/*** Private Member Functions ***************************************/
private:
	bool OpenMMStream(const char* szFilename);

/*** Private Member Variables ***************************************/
private:
	CDXScreen*						    m_pScreen;
	CDXSurface*						    m_pSurface;
	IMultiMediaStream*			  m_pMMStream;
	IMediaStream*					    m_pPrimaryVidStream;
	IDirectDrawMediaStream*		m_pDDStream;
  IDirectDrawStreamSample*	m_pDDStreamSample;
  IDirectDrawSurface*			  m_pDDSurface;
	RECT								      m_rectClipping;
  RECT                      m_srcRect;
};

#endif // !defined(AFX_MOVIE_H__EC559780_FB30_11D2_BDF5_0000E86752EE__INCLUDED_)

/* EOF */


