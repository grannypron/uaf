function INDEX() {
	this.m_name = [];
	this.m_value = [];
	this.m_allocatedSize = 0;

	/**TODO**
	int read(CArchive & ar);
	int read(RAM_FILE & ar);
	unsigned int lookup(const CString& funcName);
	*/
}

INDEX.prototype.numEntry = function () {
	return this.m_allocatedSize;
}

INDEX.prototype.name = function (index) {
	return this.m_name[index];
}

