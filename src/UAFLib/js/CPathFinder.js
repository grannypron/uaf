function CPathFinder() {

    this.NONODE = UAFUtil.ByteFromHexString("0xffff");

    this.dX = [ 0,- 1, 1, 0, -1, 1, -1, 1 ];  // Try orthogonal steps first
    this.dY = [-1, 0, 0, 1, -1, -1, 1, 1 ];

    this.m_iCols = 0;
	this.m_iRows = 0;
	this.m_memory = [];  // char *

	// User-defined callback pointers
	this.m_pCBData;			 // Data passed back to following two functions. 
    this.m_fValid;	 // The user-defined validity function
	this.m_fCost;	 // The user-defined cost function
	this.PATH = [];

	/**TODO
	void Clear(void);

	bool InUse();          // true is used, false is avail

	CPoint GetLastStep();
	CPoint GetNextStep();
	int    GetStepCount();
	*/
}

CPathFinder.prototype.Clear = function () {
	if (this.m_memory != null) {
		delete this.m_memory;
	};

}


CPathFinder.prototype.SetData = function (sd) {
	this.m_pCBData = sd;
}

CPathFinder.prototype.SetValid = function (sv) {
    this.m_fValid = sv;
}

CPathFinder.prototype.SetCost = function (sc) {
    this.m_fCost = sc;
}

CPathFinder.prototype.SetSize = function (c, r) {
    this.m_iCols = c;
    this.m_iRows = r;
}

CPathFinder.prototype.InUse = function()
{
	Globals.MsgBoxError("Not Implemented");
	return true;
}

CPathFinder.prototype.GeneratePath10 = function(sx, sy, sw, sh, destLeft, destTop, destRight, destBottom, pCombatantLinger, moveOriginPoint)
{
    // This should operate mostly like the original CPathFinder
    // whenever the x- and y-coordinates have zero offset.
    // In other words .... we expect x to range [0 -> iCols-1]
    // and y to be in the range [0 -> iRows-1].
    // This could be changed by providing x0 and y0 parameters.
    // The original took cpu time proportional to the fourth
    // power of the distance from source to destination.  Our
    // new implementation scales as the second power of distance.
    var        found = false;
    var        totalNodes = 0;
    var        memSize = 0;
    var        numInQueue = 0;     // Number of entries in queue;
    var        numExamined = 0;    // Number of queue entries examined.
    var        minX = 0, maxX = 0;     // First and last valid columns in queueIndex. 
    var        queueIndex = 0;     // Index in nodeQueue of each node;
    // Each entry also indicates whether or not we
    // have called m_fValid for each cell and the
    // result of that call.
    // Values are:
    //    -1 when m_fValid has not been called
    //    -2 if m_fValid returned 'false'
    //   >=0 if m_fValid returned 'true' and node added to nodeQueue
    var nodeQueue;      // List of nodes vistited and to-be-visited.
    //int destID = dx*m_iRows + dy;
    var destXL = 0; //dx-sw+1;
    var destXR = 0;     //dx+dw-1;
    var destYT = 0;  //dy-sh+1;
    var destYB = 0;    //dy+dh-1;
    var sourceID = sx * this.m_iRows + sy;


    if (moveOriginPoint)  // What part of the moving rectangle must enter
    // the destination rectangle.
    {   // Only the origin point of the path rectangle
        destXL = destLeft;
        destXR = destLeft;
        destYT = destTop;
        destYB = destTop;
    }
    else {   // Any part of the path rectangle
        destXL = destLeft - sw + 1; //dx-sw+1;
        destXR = destRight;     //dx+dw-1;
        destYT = destTop - sh + 1;  //dy-sh+1;
        destYB = destBottom;    //dy+dh-1;
    };
    // Allocate memory for
    //   queueIndex[totalNodes]    = -1 or index of node in nodeQueue.
    //   nodeQueue[totalNodes]     = nodesVisited and nodesToVisit
    //   nodeCosts[totalNodes]     = cost of each node in nodeQueue

    totalNodes = this.m_iRows * this.m_iCols;
    /*
    //memSize = 0;
    //memSize += totalNodes * sizeof(* queueIndex);
    //memSize += totalNodes * sizeof(* nodeQueue);
    //m_memory = (char *)My_malloc(memSize);
    if (m_memory == null) {
        Globals.MsgBoxError("Cannot allocate memory for pathfinder");
        return false;
    };
    queueIndex = (int *)m_memory;
    nodeQueue = (PATH_NODE *)((char *)queueIndex + totalNodes * sizeof(* queueIndex));
    */
    // PORT NOTE:  initializing arrays as described in the comments above
    queueIndex = [];
    nodeQueue = [];
    nodeCosts = [];
    for (var idx = 0; idx < totalNodes; idx++) {
        queueIndex[idx] = -1;
        nodeQueue[idx] = new PATH_NODE();
        nodeCosts[idx] = 0;
    }

    // Clear the starting column of queueIndex.
    // Here is what we are doing:
    // We want to avoid clearing the entire queueIndex array because for
    // simple cases where the the path is very short, we want to avoid 
    // the cpu cost of clearing a large array of which we will use only
    // a very small part.  We want this algorithm to be efficient for
    // small problems.  So we only clear columns of the array as needed.
    // memset(queueIndex + m_iRows * sx, 0xff, m_iRows * sizeof(* queueIndex));  // PORT NOTE: Already handled this above
 
    minX = maxX = sx;
    numExamined = 0;
    numInQueue = 1;
    {
        var ID;
        ID = sx * this.m_iRows + sy;
        nodeQueue[0].nodeID = ID;
        nodeQueue[0].parentID = this.NONODE; // Start position has no parent.
        nodeQueue[0].cost = 0;          // After all, we are already there.
        queueIndex[ID] = 0;             // Mark starting node as being in queue
    };

    found = false;
    while ((numInQueue > numExamined) && !found) {
        {
            // We have examined node [numExamined].  It is not a
            // target node.  Add the nodes we can reach from here to
            // the nodeQueue to be examined.
            var i = 0;
            var parentX = 0, parentY = 0;
            var childX = 0, childY = 0;
            parentX = Math.floor(nodeQueue[numExamined].nodeID / this.m_iRows);    // PORT NOTE:  This was a tricky bug!  / in C++ is DIV, not division (a float was being returned and used as an array index!)
            parentY = nodeQueue[numExamined].nodeID % this.m_iRows;
            for (i = 0; i < 8; i++) {
                var childID;
                childX = parentX + this.dX[i];
                childY = parentY + this.dY[i];
                childID = this.m_iRows * childX + childY;
                // childID may be illegal at this point!


                // Avoid calling m_fValid if possible.  It slows us down a lot!
                if ((childX < 0) || (childX >= this.m_iCols)) continue;
                if ((childY < 0) || (childY >= this.m_iRows)) continue;
                if (childX < minX) {
                    //memset(queueIndex + m_iRows * childX, 0xff, m_iRows * sizeof(* queueIndex));   PORT NOTE:  Already handled above
                    minX = childX;
                };
                if (childX > maxX) {
                    //memset(queueIndex + m_iRows * childX, 0xff, m_iRows * sizeof(* queueIndex));   PORT NOTE:  Already handled above
                    maxX = childX;
                };
                // We can look in our table to see if this
                // cell has already been determined to be valid
                if (queueIndex[childID] == -2) continue;  // Already determined invalid.
                if (queueIndex[childID] < 0) {
                    if (this.m_fValid(childX, childY, this.m_pCBData, pCombatantLinger) == 0) {
                        queueIndex[childID] = -2;  // Remember that the cell is invalid.
                        continue;
                    }
                }


                //LinkChild(node, x+i, y+j);

                {  // Add the 'child' node to the queue.
                    var thisID;
                    var childQueueIndex;
                    var childCost;
                    thisID = nodeQueue[numExamined].nodeID;

                    childQueueIndex = queueIndex[childID];
                    childCost = nodeQueue[numExamined].cost;
                    childCost += this.m_fCost(parentX, parentY, childX, childY, this.m_pCBData);
                    if (childQueueIndex >= 0) {
                        // The child node is already in the queue.
                        if (childCost < nodeQueue[childQueueIndex].cost) {
                            if (childQueueIndex >= numExamined) {
                                // We found a cheaper way to an unexamined node.
                                // MsgBoxError("Not Implemented: Code 356");
                                // The child is among the nodes not yet examined.
                                // Its parent must have a lower ID than ours and
                                // therefore we cannot improve its cost (number of steps)
                                // unless the cost function is a bit queer.
                                nodeQueue[childQueueIndex].parentID = thisID;
                                nodeQueue[childQueueIndex].cost = childCost;
                                this.CostSort(nodeQueue, queueIndex, childQueueIndex);
                            }
                            else {
                                Globals.MsgBoxError("NotImplemented: Code 357");
                                // The child node has already been examined.
                                // It has been examined before the current node
                                // and therefore we cannot improve upon it.
                            }
                        }
                        else if (childCost == nodeQueue[childQueueIndex].cost) {
                            // An attempt to get more random-looking walks.
                            // Sometimes we substitute equals.
                            if (childQueueIndex >= numExamined) {
                                if (childQueueIndex & 1) // Occasionally
                                {
                                    // Use the new parent.
                                    nodeQueue[childQueueIndex].parentID = thisID;
                                };
                            }
                            else {
                                Globals.MsgBoxError("NotImplemented: Code 357");
                                // The child node has already been examined.
                                // It has been examined before the current node
                                // and therefore we cannot improve upon it.
                            }
                        }
                    }
                    else {
                        // the child node is not yet in the queue.
                        // We will add it to the end of the queue.

                        queueIndex[childID] = numInQueue;
                        nodeQueue[numInQueue].parentID = thisID;
                        nodeQueue[numInQueue].nodeID = childID;
                        nodeQueue[numInQueue].cost = childCost;
                        numInQueue++;
                        if ((childX <= destXR)
                            && (childY <= destYB)
                            && (childX >= destXL)
                            && (childY >= destYT)
                        ) {
                            {
                                var ID;
                                this.PATH = [];  // PORT NOTE:  was .RemoveAll();
                                for (ID = nodeQueue[numInQueue - 1].nodeID; ID != sourceID; ID = nodeQueue[queueIndex[ID]].parentID) {
                                    var x = 0, y = 0;
                                    x = Math.floor(ID / this.m_iRows);
                                    y = ID % this.m_iRows;
                                    this.PATH.push(new CPoint(x, y));  // PORT NOTE:  was this.PATH.AddHead(CPoint(x, y));
                                }
                            }
                            found = true;
                            break;
                        }
                        // Move lowest cost nodes toward
                        // front of queue
                        this.CostSort(nodeQueue, queueIndex, numInQueue - 1);
                    }
                };
            };  // for ( i )
            numExamined++;
        };
    }
    Globals.My_free(this.m_memory);
    this.m_memory = null;
    return found;
}



CPathFinder.prototype.CostSort = function(nodeQueue, queueIndex, i) {
    var iCost = 0, jCost = 0;
    var j;
    while (i > 0) {
        iCost = nodeQueue[i].cost;
        jCost = nodeQueue[i - 1].cost;
        if (jCost < iCost) break;
        if ((jCost > iCost) || (i & 1))  // Sometimes we swap equals. 
        {
            for (j = i - 1; j > 0; j--) {
                // Find [j] = first instance of jCost
                if (nodeQueue[j - 1].cost != jCost) break;
            };
            { // Swap [index] with [i];
                var t = new PATH_NODE();

                t.nodeID = nodeQueue[i].nodeID;
                t.parentID = nodeQueue[i].parentID;

                nodeQueue[i].nodeID = nodeQueue[j].nodeID;
                nodeQueue[i].parentID = nodeQueue[j].parentID;
                nodeQueue[i].cost = jCost;

                nodeQueue[j].nodeID = t.nodeID;
                nodeQueue[j].parentID = t.parentID;
                nodeQueue[j].cost = iCost;

                queueIndex[nodeQueue[i].nodeID] = i;
                queueIndex[nodeQueue[j].nodeID] = j;
            };
            i = j;
        }
        if (iCost == jCost) break;
    };
    return;
}