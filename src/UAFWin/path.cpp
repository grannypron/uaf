/******************************************************************************
* Filename: Path.cpp
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

#include "..\Shared\stdafx.h"

#include "externs.h"
#include "Dungeon.h"
#include <string.h>
//#include <math.h>
#include "combatants.h"
#include "DrawTile.h"
#include "Path.h"

void *My_malloc(int size);
void My_free(void *addr);


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

int MAX_STEPS = 100;

PATH_MANAGER pathMgr;

int GetCost(int sx, int sy, int dx, int dy, void *ud)
{
#ifdef OLDPATH
  return 1;//GetDist(GetDir(sx, sy, dx, dy));
#else
  int d2;
//  if (abs(sx-dx) > 1)
//    return 100;
//  if (abs(sy-dy) > 1)
//    return 100;
  d2 = (sx-dx)*(sx-dx) + (sy-dy)*(sy-dy);
  return 5*d2 + 5;
#endif
}

int GetValid(int x, int y, void *ud, COMBATANT *pCombatantLinger)
{
  BOOL checkIfOccupied = pathMgr.GetOccBlock();

	if (!ValidCoords(y,x))
    return FALSE;
 
/* ****************************** *****************************
 * This made no sense to me.  Perhaps it was meant to not check blocking
 * at the destination itself.  But this code seems incomplete and backward!
 *
 *
 * if ((x < pathMgr.DestLeft) || (y < pathMgr.DestTop))
 * { 
 *   checkIfOccupied = FALSE;
 * }
 * *******************************************************************
 */
  BOOL valid = (OBSTICAL_none == ObsticalType(x, y, 
                      pathMgr.GetPathWidth(), pathMgr.GetPathHeight(), 
                      checkIfOccupied,
                      TRUE,
                      pCombatantLinger));
  return valid;
}

#ifdef OLDPATH

CPathFinder::CPathFinder() 
{
	m_pOpen = m_pClosed = NULL;
	m_pStack = NULL;
  m_pBest = NULL;
  m_iRows=0;
  m_iSX=0;
  m_iSY=0;
  m_iDX=0;
  m_iDY=0;
  m_iDNum=0;
  m_pCBData=NULL;
  udCost=NULL;
  udValid=NULL;
}

CPathFinder::~CPathFinder() {
	Clear();
}

void CPathFinder::Clear()
{
  ClearNodes();
  
  m_iSX=0;
  m_iSY=0;
  m_iDX=0;
  m_iDY=0;
  m_iDNum=0;
  m_pCBData=NULL;
  udCost=0;
  udValid=0;
  m_pBest = NULL;

  if (m_pStack != NULL)
  {
    _asStack *temp = m_pStack, *temp2 = NULL;

    while (temp != NULL)
    {
      temp2 = temp;
      temp = temp->next;
      delete temp2;
    }
  }
  m_pStack = NULL;
  PATH.RemoveAll();
}

void CPathFinder::ClearNodes() {
	_asNode *temp = NULL;

	if (m_pOpen) {
		while (m_pOpen) {
			temp = m_pOpen->next;

			delete m_pOpen;

			m_pOpen = temp;
		}
	}

	if (m_pClosed) {
		while (m_pClosed) {
			temp = m_pClosed->next;

			delete m_pClosed;

			m_pClosed = temp;
		}
	}
}

bool CPathFinder::GeneratePath(int sx, int sy, int dx, int dy) 
{
	m_iSX = sx; m_iSY = sy; m_iDX = dx; m_iDY = dy;
	m_iDNum = Coord2Num(dx,dy);

	_asNode *temp = new _asNode(sx, sy);
	_asNode *best=NULL;

	temp->g = 0;
	temp->dist2 = (dx-sx)*(dx-sx) + (dy-sy)*(dy-sy);
	temp->f = temp->g + temp->dist2;
	temp->number = Coord2Num(sx, sy);

	m_pOpen = temp;
	while (true) 
  {
    best = GetBest();
		if (best==NULL) return false;
		if (best->number == m_iDNum) break;

		CreateChildren(best);
	}
	
	m_pBest = best;

  temp = m_pBest;

  // create step list in reverse order
  while (temp != NULL)
  {
    if (!((temp->x == sx) && (temp->y == sy)))
      PATH.AddHead(CPoint(temp->x, temp->y));
    temp = temp->parent;
  }
  
  return true;
}

_asNode *CPathFinder::GetBest() {
	if (!m_pOpen) return NULL;

	_asNode *temp = m_pOpen, *temp2 = m_pClosed;
	m_pOpen = temp->next;

	if (temp2) {
		m_pClosed = temp;
		m_pClosed->next = temp2;
	} else {
		m_pClosed = temp;
		m_pClosed->next = NULL;
	}

	return temp;
}

void CPathFinder::CreateChildren(_asNode *node) {
	int x = node->x, y = node->y;

	for (int i=-1;i<2;i++) {
		for (int j=-1;j<2;j++) {
			if (i == 0 && j == 0 || !udValid(x+i, y+j, m_pCBData)) continue;

			LinkChild(node, x+i, y+j);
		}
	}
}

void CPathFinder::LinkChild(_asNode *node, int x, int y) 
{
	int g = node->g + udCost(node->x, node->y, x,y,m_pCBData);
	int num = Coord2Num(x,y);

	_asNode *check = NULL;
  	
  check = CheckList(m_pOpen, num);
  if (check != NULL) 
  {
		node->children[node->numchildren++] = check;
		
		// A better route found, so update
		// the node and variables accordingly.
		if (g < check->g) 
    {
			check->parent = node;
			check->g = g;
			check->f = g + check->dist2;
		}
	} 
  else if ((check=CheckList(m_pClosed, num))!=NULL) 
  {
		node->children[node->numchildren++] = check;

		if (g < check->g) 
    {
			check->parent = node;
			check->g = g;
			check->f = g + check->dist2;

			// The fun part...
			UpdateParents(check);
		}
	} 
  else 
  {
		_asNode *newnode = new _asNode(x,y);
		newnode->parent = node;
		newnode->g = g;
		newnode->dist2 = (x-m_iDX)*(x-m_iDX) + (y-m_iDY)*(y-m_iDY);
		newnode->f = newnode->g + newnode->dist2;
		newnode->number = Coord2Num(x,y);

		AddToOpen(newnode);

		node->children[node->numchildren++] = newnode;
	}
}

_asNode *CPathFinder::CheckList(_asNode *node, int num) 
{
	while (node) 
  {
		if (node->number == num) 
      return node;
		node = node->next;
	}

	return NULL;
}

// Sort with respect to 'f'
void CPathFinder::AddToOpen(_asNode *addnode) 
{
	_asNode *node = m_pOpen;
	_asNode *prev = NULL;

	if (!m_pOpen) 
  {
		m_pOpen = addnode;
		return;
	}

	while(node) 
  {
		if (addnode->f > node->f) 
    {
			prev = node;
			node = node->next;
		} 
    else 
    {
			if (prev) 
      {
				prev->next = addnode;
				addnode->next = node;
			} 
      else 
      {
				_asNode *temp = m_pOpen;

				m_pOpen = addnode;
				m_pOpen->next = temp;
			}

			return;
		}
	}

	prev->next = addnode;
}

// Have to propagate the changes down.
void CPathFinder::UpdateParents(_asNode *node) 
{
	int g = node->g, c = node->numchildren;
  
	_asNode *kid = NULL;
  int i;
	for (i=0;i<c;i++) 
  {
		kid = node->children[i];
		if (g+1 < kid->g) 
    {
			kid->g = g+1;
			kid->f = kid->g + kid->dist2;
			kid->parent = node;
		
			Push(kid);
		}
	}

	_asNode *parent;

	while (m_pStack) 
  {
		parent = Pop();
		c = parent->numchildren;
		for (i=0;i<c;i++) 
    {
			kid = parent->children[i];
			
			if (parent->g+1 < kid->g) 
      {
				kid->g = parent->g + udCost(parent->x, parent->y, kid->x, kid->y,m_pCBData);
				kid->f = kid->g + kid->dist2;
				kid->parent = parent;

				Push(kid);
			}
		}
	}
}

void CPathFinder::Push(_asNode *node) 
{

	if (!m_pStack) 
  {
		m_pStack = new _asStack;
		m_pStack->data = node;
		m_pStack->next = NULL;
	} 
  else 
  {
		_asStack *temp = new _asStack;

		temp->data = node;
		temp->next = m_pStack;
		m_pStack = temp;
	}
}

_asNode *CPathFinder::Pop() 
{
	_asNode *data = m_pStack->data;
	_asStack *temp = m_pStack;

	m_pStack = temp->next;
	
	delete temp;

	return data;
}

CPoint CPathFinder::GetFirstStep()// always the start location
{
  return GetNextStep();
}

CPoint CPathFinder::GetNextStep()
{
  CPoint tmp;

  if (PATH.GetCount() > 0)
  {
    tmp = PATH.GetHead();
    PATH.RemoveHead();
  }
  else
  {
    tmp.x = -1;
    tmp.y = -1;
  }
  return tmp;
}

CPoint CPathFinder::GetLastStep()
{
  if (PATH.GetCount() > 0)
    return PATH.GetTail();
  else
	  return CPoint(-1,-1);
}

bool CPathFinder::InUse()
{
	return (PATH.GetCount() > 0);
}

#else  // if !OLDPATH

CPathFinder::CPathFinder(void)
{
  m_memory = NULL;
}

CPathFinder::~CPathFinder(void)
{
  Clear();
}


void CPathFinder::Clear(void)
{
  if (m_memory != NULL)
  {
    My_free(m_memory);
    m_memory = NULL;
  };
}

void	CPathFinder::SetData(void *sd)
{
  m_pCBData = sd;
}

bool CPathFinder::InUse(void)
{
  MsgBoxError("Not Implemented");
  return true;
}



CPoint CPathFinder::GetNextStep(void)
{
  CPoint tmp;

  if (PATH.GetCount() > 0)
  {
    tmp = PATH.GetHead();
    PATH.RemoveHead();
  }
  else
  {
    tmp.x = -1;
    tmp.y = -1;
  }
  return tmp;
}

CPoint CPathFinder::GetLastStep(void)
{
  if (PATH.GetCount() > 0)
    return PATH.GetTail();
  else
	  return CPoint(-1,-1);
}

int CPathFinder::GetStepCount(void)
{
  return PATH.GetCount();
}


char dX[8] = {0,-1,1,0,-1,1,-1,1};  // Try orthogonal steps first
char dY[8] = {-1,0,0,1,-1,-1,1,1};

//char dX[8] = {-1,-1,-1,0,0,1,1,1};  // Old method compatibility
//char dY[8] = {-1,0,1,-1,1,-1,0,1};


typedef WORD IDTYPE;  // I want this to be unsigned 2 bytes

struct PATH_NODE
{ // Intentionally made this 8 bytes to take advantage of hardware addressing capability
  IDTYPE nodeID;   // x*rows+y
  IDTYPE parentID; 
  int    cost;     // Cost to get here from starting position.
};

#define NONODE 0xffff


void CostSort(PATH_NODE *nodeQueue, int *queueIndex, int i)
{
  int iCost, jCost;
  int j;
  while (i > 0)
  {
    iCost = nodeQueue[i].cost;
    jCost = nodeQueue[i-1].cost;
    if (jCost < iCost) break;
    if ((jCost > iCost) || (i & 1) )  // Sometimes we swap equals. 
    {
      for (j=i-1; j>0; j--)
      {
        // Find [j] = first instance of jCost
        if (nodeQueue[j-1].cost != jCost) break;
      };
      { // Swap [index] with [i];
        PATH_NODE t;
  
        t.nodeID   = nodeQueue[i].nodeID;
        t.parentID = nodeQueue[i].parentID;

        nodeQueue[i].nodeID   = nodeQueue[j].nodeID;
        nodeQueue[i].parentID = nodeQueue[j].parentID;
        nodeQueue[i].cost     = jCost;

        nodeQueue[j].nodeID   = t.nodeID;
        nodeQueue[j].parentID = t.parentID;
        nodeQueue[j].cost     = iCost;

        queueIndex[nodeQueue[i].nodeID] = i;
        queueIndex[nodeQueue[j].nodeID] = j;
      };
      i = j;
    }
    if (iCost == jCost) break;
  };
  return;
}

bool CPathFinder::GeneratePath(int sx, int sy, 
                               int sw, int sh,
                               int destLeft,  int destTop,
                               int destRight, int destBottom,
                               COMBATANT *pCombatantLinger,
                               BOOL moveOriginPoint)
{
  // This should operate mostly like the original CPathFinder
  // whenever the x- and y-coordinates have zero offset.
  // In other words .... we expect x to range [0 -> iCols-1]
  // and y to be in the range [0 -> iRows-1].
  // This could be changed by providing x0 and y0 parameters.
  // The original took cpu time proportional to the fourth
  // power of the distance from source to destination.  Our
  // new implementation scales as the second power of distance.
  bool       found;
  int        totalNodes;
  int        memSize;
  int        numInQueue;     // Number of entries in queue;
  int        numExamined;    // Number of queue entries examined.
  int        minX, maxX;     // First and last valid columns in queueIndex. 
  int       *queueIndex;     // Index in nodeQueue of each node;
                             // Each entry also indicates whether or not we
                             // have called m_fValid for each cell and the
                             // result of that call.
                             // Values are:
                             //    -1 when m_fValid has not been called
                             //    -2 if m_fValid returned 'false'
                             //   >=0 if m_fValid returned 'true' and node added to nodeQueue
  PATH_NODE *nodeQueue;      // List of nodes vistited and to-be-visited.
  //int destID = dx*m_iRows + dy;
  int destXL; //dx-sw+1;
  int destXR;     //dx+dw-1;
  int destYT;  //dy-sh+1;
  int destYB;    //dy+dh-1;
  int sourceID = sx*m_iRows + sy;


  if (moveOriginPoint)  // What part of the moving rectangle must enter
                        // the destination rectangle.
  {   // Only the origin point of the path rectangle
    destXL = destLeft;
    destXR = destLeft;
    destYT = destTop;
    destYB = destTop;
  }
  else
  {   // Any part of the path rectangle
    destXL = destLeft-sw+1; //dx-sw+1;
    destXR = destRight;     //dx+dw-1;
    destYT = destTop-sh+1;  //dy-sh+1;
    destYB = destBottom;    //dy+dh-1;
  };
  // Allocate memory for
  //   queueIndex[totalNodes]    = -1 or index of node in nodeQueue.
  //   nodeQueue[totalNodes]     = nodesVisited and nodesToVisit
  //   nodeCosts[totalNodes]     = cost of each node in nodeQueue
  totalNodes = m_iRows * m_iCols;
  memSize = 0;
  memSize += totalNodes * sizeof (*queueIndex);
  memSize += totalNodes * sizeof (*nodeQueue);
  m_memory = (char *)My_malloc(memSize);
  if (m_memory == NULL)
  {
    MsgBoxError("Cannot allocate memory for pathfinder");
    return false;
  };
  queueIndex   = (int *)m_memory;
  nodeQueue    = (PATH_NODE *)((char *)queueIndex   + totalNodes*sizeof(*queueIndex));

  // Clear the starting column of queueIndex.
  // Here is what we are doing:
  // We want to avoid clearing the entire queueIndex array because for
  // simple cases where the the path is very short, we want to avoid 
  // the cpu cost of clearing a large array of which we will use only
  // a very small part.  We want this algorithm to be efficient for
  // small problems.  So we only clear columns of the array as needed.
  memset(queueIndex + m_iRows*sx, 0xff, m_iRows*sizeof(*queueIndex));
  minX = maxX = sx;
  numExamined = 0;
  numInQueue = 1;
  {
    IDTYPE ID;
    ID = sx*m_iRows + sy;
    nodeQueue[0].nodeID = ID;
    nodeQueue[0].parentID = NONODE; // Start position has no parent.
    nodeQueue[0].cost = 0;          // After all, we are already there.
    queueIndex[ID] = 0;             // Mark starting node as being in queue
  };

  found = false;
	while ((numInQueue > numExamined) && !found) 
  {
    //best = GetBest();
		//if (best==NULL) return false;
    // If next node to examine is the destination node then we are done.
		//if (nodeQueue[numExamined].nodeID == destID)
    /*
    int curx = nodeQueue[numExamined].nodeID / m_iRows;
    int cury = nodeQueue[numExamined].nodeID % m_iRows;
    if (    (curx+sw > destXL) 
         && (cury+sh > destYT) 
        && (curx    <= destXR)
         && (cury    <= destYB)
       )
    {  
      IDTYPE ID;
      PATH.RemoveAll();
      for (ID=nodeQueue[numExamined].nodeID; ID != sourceID; ID = nodeQueue[queueIndex[ID]].parentID)
      {
        int x, y;
        x = ID / m_iRows;
        y = ID % m_iRows;
        PATH.AddHead(CPoint(x, y));
      };
      My_free (m_memory);
      m_memory = NULL;
      return true;
    };
    */
    {
      // We have examined node [numExamined].  It is not a
      // target node.  Add the nodes we can reach from here to
      // the nodeQueue to be examined.
      int i;
      int parentX, parentY;
      int childX, childY;
      parentX = nodeQueue[numExamined].nodeID / m_iRows;
      parentY = nodeQueue[numExamined].nodeID % m_iRows;
  	  for (i=0;i<8;i++) 
      {
        IDTYPE childID;
        childX = parentX + dX[i];
        childY = parentY + dY[i];
        childID = m_iRows*childX + childY;
        // childID may be illegal at this point!


     	  //if (!m_fValid(childX, childY, m_pCBData)) continue;
        // Avoid calling m_fValid if possible.  It slows us down a lot!
        if ( (childX < 0) || (childX >= m_iCols) ) continue;
        if ( (childY < 0) || (childY >= m_iRows) ) continue;
        if (childX < minX)
        {
          memset(queueIndex + m_iRows*childX, 0xff, m_iRows*sizeof(*queueIndex));
          minX = childX;
        };
        if (childX > maxX)
        {
          memset(queueIndex + m_iRows*childX, 0xff, m_iRows*sizeof(*queueIndex));
          maxX = childX;
        };
        // We can look in our table to see if this
        // cell has already been determined to be valid
        if (queueIndex[childID] == -2) continue;  // Already determined invalid.
        if (queueIndex[childID] < 0)
        {
          if (m_fValid(childX, childY, m_pCBData, pCombatantLinger) == 0)
          {
            queueIndex[childID] = -2;  // Remember that the cell is invalid.
            continue;
          }
        };


        //LinkChild(node, x+i, y+j);
         
        {  // Add the 'child' node to the queue.
          IDTYPE thisID;
          int childQueueIndex;
          int childCost;
          thisID = nodeQueue[numExamined].nodeID;
                  
          childQueueIndex = queueIndex[childID];
          childCost =   nodeQueue[numExamined].cost;
          childCost +=  m_fCost(parentX, parentY, childX, childY, m_pCBData);
	        if (childQueueIndex >= 0)
          {
            // The child node is already in the queue.
            if (childCost < nodeQueue[childQueueIndex].cost)
            {
              if (childQueueIndex >= numExamined)
              {
                // We found a cheaper way to an unexamined node.
                // MsgBoxError("Not Implemented: Code 356");
                // The child is among the nodes not yet examined.
                // Its parent must have a lower ID than ours and
                // therefore we cannot improve its cost (number of steps)
                // unless the cost function is a bit queer.
                nodeQueue[childQueueIndex].parentID = thisID;
                nodeQueue[childQueueIndex].cost = childCost;
                CostSort(nodeQueue, queueIndex, childQueueIndex);
              }
              else 
              {
                MsgBoxError("NotImplemented: Code 357");
                // The child node has already been examined.
                // It has been examined before the current node
                // and therefore we cannot improve upon it.
              };
            }
            else if (childCost == nodeQueue[childQueueIndex].cost)
            {
              // An attempt to get more random-looking walks.
              // Sometimes we substitute equals.
              if (childQueueIndex >= numExamined)
              {
                if (childQueueIndex & 1) // Occasionally
                {
                  // Use the new parent.
                  nodeQueue[childQueueIndex].parentID = thisID;
                };
              }
              else 
              {
                MsgBoxError("NotImplemented: Code 357");
                // The child node has already been examined.
                // It has been examined before the current node
                // and therefore we cannot improve upon it.
              };
            };
          }
          else 
          {
            // the child node is not yet in the queue.
            // We will add it to the end of the queue.

            queueIndex[childID] = numInQueue;
            nodeQueue[numInQueue].parentID = thisID;
	          nodeQueue[numInQueue].nodeID = childID;
            nodeQueue[numInQueue].cost = childCost;
            numInQueue++;
            //if (nodeQueue[numInQueue++].nodeID == destID) 
            if (    (childX <= destXR)
                 && (childY <= destYB)
                 && (childX >= destXL)
                 && (childY >= destYT)
               )
            {
              {  
                IDTYPE ID;
                PATH.RemoveAll();
                //for (ID=nodeQueue[numExamined].nodeID; ID != sourceID; ID = nodeQueue[queueIndex[ID]].parentID)
                for (ID=nodeQueue[numInQueue-1].nodeID; ID != sourceID; ID = nodeQueue[queueIndex[ID]].parentID)
                {
                  int x, y;
                  x = ID / m_iRows;
                  y = ID % m_iRows;
                  PATH.AddHead(CPoint(x, y));
                };
              };
              found = true;
              break;
            };
            // Move lowest cost nodes toward
            // front of queue
            CostSort(nodeQueue, queueIndex, numInQueue-1); 
	         }
        }; 
	    };  // for ( i )
      numExamined++;
    };
	}
  My_free (m_memory);
  m_memory = NULL;
  return found;
}

#endif

//****************************************************
//****************************************************
//****************************************************

PATH_MANAGER::PATH_MANAGER(int num )
{
	MaxPaths = num;
	Tags = new TAG_LIST(num);
	Path = new CPathFinder[num];

  OccupantsBlock = TRUE;	
	CurrPath = 0;
	StartX = 0, StartY = 0, DestLeft = 0, DestTop = 0; DestRight = 0; DestBottom = 0;
  pathWidth=1;pathHeight=1;
}


PATH_MANAGER::~PATH_MANAGER()
{
	if (Tags) delete Tags;
	if (Path) delete[] Path;
}

void PATH_MANAGER::Clear()
{
  pathWidth=1;pathHeight=1;
  OccupantsBlock = TRUE;
	for (int i=0; i<MaxPaths; i++)
		Path[i].Clear();
	Tags->Clear ();
	CurrPath = 0;
	StartX = 0, StartY = 0, DestLeft = 0, DestTop = 0; DestRight = 0; DestBottom = 0;
}

int	PATH_MANAGER::GetPath(int startx,    int starty, 
                          int destLeft,  int destTop,
                          int destRight, int destBottom,
                          BOOL occupantsBlock,
                          COMBATANT *pCombatantLinger,
                          BOOL moveOriginPoint)     // 'moveOriginPoint' means that the point (startX,startY)
                                                    // must be moved into the destinatiomn rectangle.
                                                    // Otherwise, any part of the path rectangle being moved into
                                                    // destination rectangle will suffice. 
{
 // WriteDebugString("Starting GetPath(startxy=(%i, %i) destLeftTop=(%i, %i) destRightBottom=(%i,%i) )\n", 
 //                  startx, starty, destLeft, destTop, destRight, destBottom);

  OccupantsBlock = occupantsBlock;

  if (   (!moveOriginPoint && 
            (     (startx + pathWidth  - 1 < destLeft)
               || (starty + pathHeight - 1 < destTop)
               || (startx > destRight)
               || (starty > destBottom)
            )
         )
      ||
         (moveOriginPoint &&
           (    (startx < destLeft)
             || (starty < destTop)
             || (startx > destRight)
             || (starty > destBottom)
           )
         )
     )
  {
  	StartX     = startx;
    StartY     = starty;
    DestLeft   = destLeft;
    DestTop    = destTop;
    DestRight  = destRight;
    DestBottom = destBottom;
	
    if (FindPath(pCombatantLinger, moveOriginPoint))
    {
      //WriteDebugString("GetPath() returning path %u\n", CurrPath);
      return CurrPath;
    }
    else
    {
      //WriteDebugString("GetPath() returning path -1\n");
	    return -1;
    }
  }
  else
  {
    return -1;// we are already there
  };
}


CPathFinder* PATH_MANAGER::GetPath(int which)
{
	if (which<0 || which>=MaxPaths) return NULL;
	Tags->Set(which);
	return &Path[which];
}


void	PATH_MANAGER::FreePath(int which)
{
	if (which<0 || which>=MaxPaths) return;
	Tags->Clear(which);
	Path[which].Clear();
}

int PATH_MANAGER::GetStepsRemaining(int which)
{
	if (which<0 || which>=MaxPaths) return 0;
	return Path[which].GetStepCount();
}

bool PATH_MANAGER::FindPath(COMBATANT *pCombatantLinger, BOOL moveOriginPoint)
{
	CurrPath = GetAvailablePath ();
	if (CurrPath == -1) return false;
  Path[CurrPath].Clear();

  Path[CurrPath].SetData(NULL);
  Path[CurrPath].SetValid(GetValid);
  Path[CurrPath].SetCost(GetCost);
  Path[CurrPath].SetSize(MAX_TERRAIN_WIDTH, MAX_TERRAIN_HEIGHT);



  bool Found = Path[CurrPath].GeneratePath(StartX, StartY, pathWidth, pathHeight, 
                                           DestLeft, DestTop,
                                           DestRight, DestBottom, 
                                           pCombatantLinger,
                                           moveOriginPoint);


  if (!Found)
  {
    Tags->Clear(CurrPath);
    Path[CurrPath].Clear();
  }
  else
    Tags->Set(CurrPath);

	return Found;
}

int  PATH_MANAGER::GetAvailablePath()
{
	int val = Tags->FindOpenTag();
	if (val > -1) return val;

	for (int i=0; i<MaxPaths; i++)
  {
		if (!Path[i].InUse())
    { 
      Tags->Clear(i);
      return i;
    }
  }
	return -1;
}
