function CODES() {
    this.m_hashTable = [];  //9973 unsigned __int16 
    this.m_codes = []; // 8192 unsigned int 
    this.m_nextCode = 0; // unsigned __int16 
    this.m_numCodes = 0;
    this.m_numMisses = 0;

    /**TODO
    CODES(void);
    ~CODES(void);
    void Clear(void);
    unsigned __int16 AddCode(unsigned int code);
    void PrintStats(void);
    void HashDump(FILE * f);
    bool Full(void) { return m_nextCode == 8190; };
    unsigned __int16 FindCode(unsigned int code); // zero if no find
    */
}

