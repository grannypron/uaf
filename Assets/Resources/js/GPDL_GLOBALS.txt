function GPDL_GLOBALS() {
	this.m_value = [];
	this.m_allocatedSize = 0;

	/**TODO**
	int read(CArchive & ar);
	int read(RAM_FILE & ar);
	*/
}

GPDL_GLOBALS.prototype.peek = function (index) {
	return this.m_value[index];
}

GPDL_GLOBALS.prototype.peek = function (index, value) {
	this.m_value[index] = value;
}
