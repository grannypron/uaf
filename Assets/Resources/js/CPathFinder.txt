function CPathFinder() {


	this.m_iCols = 0;
	this.m_iRows = 0;
	this.m_memory = [];  // char *

	// User-defined callback pointers
	var m_pCBData;			 // Data passed back to following two functions. 
	var  m_fValid;	 // The user-defined validity function
	var m_fCost;	 // The user-defined cost function
	var PATH = [];

	/**TODO
	void Clear(void);

	void SetValid(_asFunc sv)    { m_fValid = sv; };
	void SetCost(_asCostFunc sc) { m_fCost = sc; }
	void SetData(void * sd);
	void SetSize(int c, int r)   { m_iCols = c; m_iRows = r; }

	bool InUse();          // true is used, false is avail

	bool	GeneratePath(int sx, int sy, int sw, int sh,
		int destLeftx, int destTop, int destRight, int destBottom,
		COMBATANT * pCombatantLinger,
		BOOL moveOriginPoint); // as opposed to 'moveOriginRectangle'


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
	this.udValid = sv;
}

CPathFinder.prototype.SetCost = function (sc) {
	this.udCost = sc;
}

CPathFinder.prototype.SetSize = function (r) {
	this.m_iRows = r;
}