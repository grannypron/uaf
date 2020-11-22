function DICEPLUS() {// To record things like:
    //     2d9 + 12 + 3*level
    this.m_Text = "";
    this.m_Bin = "";
}

DICEPLUS.prototype.DICEPLUSText = function(text) {
    this.m_Text = text;
}

/**TODO
void Clear(void);
void PreSerialize(BOOL IsStoring);
int Serialize(CAR & car);
int Serialize(CArchive & car);
void CrossReference(class CR_LIST * pCRList, class CR_REFERENCE * pCRReference)const ;
//void AddAdjust(int value, const CString& name);
//void AddAdjust(ADJUSTMENT& adjust);
void AddAdjust(CString & name);
int GetDataSize(void) const ;
CString EncodeText();
BOOL DecodeText(CString data, CString & err);
BOOL DecodeText(CString data);
BOOL Roll(double & result) const ;
BOOL MaxRoll(double & result) const ; // return max possible result

BOOL Compile() const ;
*/