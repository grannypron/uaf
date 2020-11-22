function GPDL_STATE() { // Also used for event return values.

	// Internal states.  These are the only possible states that
	//  will be returned by State().  
	this.GPDL_UNINITIALIZED = 1;
	this.GPDL_IDLE = 2;
	this.GPDL_WAIT_INPUT = 3;
	this.GPDL_WAIT_ACK = 4;

	// Return status values
	this.GPDL_OK = 5;
	this.GPDL_ACCEPTED = 6;
	this.GPDL_IGNORED = 7;
	this.GPDL_NOSUCHNAME = 8;
	this.GPDL_EVENT_ERROR = 9;
	this.GPDL_READ_ERROR = 10;
	this.GPDL_OVER_RP = 11;
	this.GPDL_UNDER_RP = 12;
	this.GPDL_OVER_SP = 13;
	this.GPDL_UNDER_SP = 14;
	this.GPDL_GREPERROR = 15;
	this.GPDL_ILLPARAM = 16;
	this.GPDL_BADINTEGER = 17;
	this.GPDL_ILLFUNC = 18;     // Illegal cell at entry to function
	this.GPDL_ILLCHARNUM = 19;
	this.GPDL_EXCESSCPU = 20;

}
