/******************************************************************************
* Filename: Path.h
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
*
* The CPathfinder code is based on source downloaded
* from www.generation5.org/astar_d.shtml
******************************************************************************/
#ifndef __PATHS_H_
#define __PATHS_H_

#include "taglist.h"

enum PATH_DIR { PathNorth, PathNE, PathEast, PathSE, PathSouth, PathSW, PathWest, PathNW, PathBAD=-1 };

inline BOOL IsNorthDir(PATH_DIR dir) { return ((dir==PathNorth)||(dir==PathNE)||(dir==PathNW)); }
inline BOOL IsSouthDir(PATH_DIR dir) { return ((dir==PathSouth)||(dir==PathSE)||(dir==PathSW)); }
inline BOOL IsEastDir(PATH_DIR dir) { return ((dir==PathEast)||(dir==PathNE)||(dir==PathSE)); }
inline BOOL IsWestDir(PATH_DIR dir) { return ((dir==PathWest)||(dir==PathNW)||(dir==PathSW)); }

inline PATH_DIR GetDir (int cx, int cy, int dx, int dy)
{
  if (cx > dx) // nw, w, sw
  {
    if (cy > dy) return PathNW;
    else if (cy < dy) return PathSW;
    return PathWest;
  }
  else if (cx < dx) // ne, e, se
  {
    if (cy > dy) return PathNE;
    else if (cy < dy) return PathSE;
    return PathEast;
  }
  else // cx == dx
  {
    if (cy > dy) return PathNorth;
    else if (cy < dy) return PathSouth;
    return PathBAD; // curr == dest
  }
}

inline int GetDist (PATH_DIR d)
{
  if (d==PathNorth || d==PathEast || d==PathSouth || d==PathWest)
    return 1;
  else
    return 2;
}

//****************************************************
//****************************************************
//****************************************************

class _asNode {
	public:
		_asNode(int a = -1,int b = -1) : x(a), y(b), number(0), numchildren(0) 
    {
			parent = next = NULL;
			memset(children, 0, sizeof(children));
      f=g=0;
      dist2=0;
		}

		int			f,g;
    int     dist2;    // distance squared
		int			x,y;			// Coordinate position
		int			numchildren;
		int			number;			// x*30+y
		_asNode		*parent;
		_asNode		*next;
		_asNode		*children[8];	// Assumes square tiles
};

// Stack for propagation.
struct _asStack {
	_asNode	 *data;
	_asStack *next;
};

typedef int(*_asFunc)(int, int, void *, COMBATANT *);
typedef int(*_asCostFunc)(int, int, int, int, void *);

//#define OLDPATH

#ifdef OLDPATH
class CPathFinder 
{
	public:
		CPathFinder();
		~CPathFinder();

		bool	GeneratePath(int, int, int, int);
		void	SetValid(_asFunc sv) { udValid = sv;   }
		void	SetCost(_asCostFunc sc)  { udCost = sc;    }
		void	SetData(void *sd)	 { m_pCBData = sd; }
		void	SetSize(int /*c*/, int r)		 { m_iRows = r;    }
    void  Clear();

		_asNode	*GetBestNode() { return m_pBest; }

  
  // these are for the entity when he is following the path
  CPoint  GetFirstStep ();  // always the start location
  CPoint  GetNextStep ();
  CPoint  GetLastStep ();
  int     GetStepCount() { return PATH.GetCount(); }

  // maintainence
  bool InUse ();          // true is used, false is avail

	protected:
		int		m_iRows;			// Used to calculate node->number
		int		m_iSX, m_iSY, m_iDX, m_iDY, m_iDNum;
		void	*m_pCBData;			// Data passed back.

		_asNode	*m_pOpen;			// The open list
		_asNode	*m_pClosed;			// The closed list
		_asNode *m_pBest;			// The best node
		_asStack*m_pStack;			// Propagation stack

		_asCostFunc	udCost;				// The user-defined cost function
		_asFunc udValid;			// The user-defined validity function

		// Functions.
		void	AddToOpen(_asNode *);
		void	ClearNodes();
		void	CreateChildren(_asNode *);
		void	LinkChild(_asNode *, int, int);
		void	UpdateParents(_asNode *);

		// Stack Functions.
		void	Push(_asNode *);
		_asNode *Pop();
		
		_asNode *CheckList(_asNode *, int);
		_asNode	*GetBest();
		
		// Inline functions.
		inline int Coord2Num(int x, int y) { return x * m_iRows + y; }

    CList<CPoint, CPoint> PATH;
};


#else    // New CPathFinder
class CPathFinder
{
  int   m_iCols;
  int   m_iRows;
  char *m_memory;

  // User-defined callback pointers
	void *m_pCBData;			 // Data passed back to following two functions. 
	_asFunc     m_fValid;	 // The user-defined validity function
  _asCostFunc	m_fCost;	 // The user-defined cost function
  CList<CPoint, CPoint> PATH;
  //void CostSort(PATH_NODE *nodeQueue, int index);
public:
  CPathFinder();
  ~CPathFinder();
  void Clear(void);

  void	SetValid(_asFunc sv)    { m_fValid = sv;   };
	void	SetCost(_asCostFunc sc) { m_fCost = sc;    }
	void	SetData(void *sd);
	void	SetSize(int c, int r)   { m_iCols = c; m_iRows = r;    }

  bool InUse ();          // true is used, false is avail

  bool	GeneratePath(int sx, int sy, int sw, int sh, 
                     int destLeftx, int destTop, int destRight, int destBottom,
                     COMBATANT *pCombatantLinger,
                     BOOL moveOriginPoint); // as opposed to 'moveOriginRectangle'


  CPoint GetLastStep();
  CPoint GetNextStep();
  int    GetStepCount();
};
#endif

//****************************************************
//****************************************************
//****************************************************

class PATH_MANAGER
{
public:
  enum { MAXPATHS = 100 };
  int DestLeft, DestTop, DestRight, DestBottom, StartX, StartY;

private:// variables

  CPathFinder*    Path;
  TAGptr    Tags;// for determining which paths are already taken
  int     MaxPaths;
  int     CurrPath;
  BOOL OccupantsBlock;
  BYTE pathWidth;
  BYTE pathHeight; 
  
private:// functions
  
  // usage functions
  int   GetAvailablePath ();// returns -1 if none avail, 0+ for index
  bool  FindPath (COMBATANT *pCombatantLinger,
                  BOOL moveOriginPoint);   // as opposed to 'moveOriginRectangle'
  PATH_MANAGER& operator=(const PATH_MANAGER &src); // no implementation
  
public:
  
  PATH_MANAGER (int num = MAXPATHS);// max number of paths
  ~PATH_MANAGER ();
  
  int  GetStepsRemaining(int which);
  BOOL GetOccBlock() { return OccupantsBlock; }
  BYTE GetPathWidth() { return pathWidth; }
  BYTE GetPathHeight() { return pathHeight; }
  void SetPathSize(int w, int h) { pathWidth = w; pathHeight = h; }

  void Clear ();  


  /*  20160612  PRS
   *  We discovered that the 5th and 6th parameters are not treated as 'width' and 'height'
   * but rather as the lower right corner of a rectangle.
   * So we changed the definition and had to change many calls to this function
   * which had "1,1" as the 5th and 6th parameters.
   * I worry......this is a major difference.  How did we survive this long?  Am I confused?
   * Well, here goes.  I'll try to leave a clear 'undo' trail behind.  Just in case.
   *
   * int  GetPath (int startx,    int starty, 
   *               int destx,     int desty,
   *               int destWidth, int destHeight,
   *               BOOL occupantsBlock,
   *               COMBATANT *pCombatantLinger);
   */
  int  GetPath (int startx,    int starty, 
                int destLeft,  int destTop,
                int destRight, int destBottom,
                BOOL occupantsBlock,
                COMBATANT *pCombatantLinger,
                BOOL moveOriginPoint);



  void  FreePath (int which);
  
  CPathFinder* GetPath (int which);  
};

extern PATH_MANAGER pathMgr;

#endif