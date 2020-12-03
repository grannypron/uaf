function BINOPS() {
	// Opcodes that require an address or number
	this.BINOP_ILLEGAL = 0;
	this.BINOP_JUMP;
	this.BINOP_ReferenceGLOBAL; // Bit 23 of address means store
	this.BINOP_JUMPFALSE;
	this.BINOP_STORE_FP;
	this.BINOP_FETCH_FP;
	this.BINOP_SUBOP;       // Opcode with sub-opcode
	this.BINOP_CALL;        // Call user-defined function
	this.BINOP_RETURN; //0x08   // subop = #locals<<12 + #parameters to pop
	this.BINOP_LOCALS;
	this.BINOP_FETCHTEXT;  //c-style string from the code segment
}