function SPELL_TARGETING_DATA() {
    this.m_MaxTargets = 0;
    this.m_MaxRange = 0;
    this.m_Width = 0;
    this.m_Height = 0;
    this.m_maxRangeX = 0;
    this.m_maxRangeY = 0;
    this.m_targType = TargetingType.getByNumber(0);
    this.m_SelectingUnits = false; // vs selecting map location
    this.m_canTargetFriend = false; // can party member be targeted?
    this.m_canTargetEnemy = false;
    this.m_IsPartyAligned = false;
    this.m_area = false;
    this.m_MaxHitDice = 0.0;    // double
    this.m_HDTotal = 0.0;       // double
    this.m_MapTargetX = 0;
    this.m_MapTargetY = 0;
    this.m_targets = [];  // In combat, this is a COMBAT_ID;  else a Unique party ID   //CArray < int, int > 
    this.m_maptarglist = new CList();   // PORT NOTE:  This is a MAPLOCATIONLIST - NOT a MAPTARGETLIST - this may be a bug idk.  Either way, they are just generic CLists in this port



    /** TODO


    int  MaxTargets(void) { return m_MaxTargets; };
    int  MaxRange(void) { return m_MaxRange; };
    int  Width(void) { return m_Width; };
    int  Height(void) { return m_Height; };
  void MaxTargets(int v){ m_MaxTargets = v; };
void MaxRange(int v){ m_MaxRange = v; };
void Width(int v){ m_Width = v; };
void Height(int v){ m_Height = v; };

public:
SPELL_TARGETING_DATA();
SPELL_TARGETING_DATA(const SPELL_TARGETING_DATA & src);
SPELL_TARGETING_DATA & operator=(const SPELL_TARGETING_DATA &src);
bool operator == (const SPELL_TARGETING_DATA& src) const ;
void Clear();

int NumTargets() const ;
int NumMapTargets(void) const ;
BOOL STD_AddTarget(int target, double hd, int range, int mapX, int mapY);
BOOL IsAlreadyTargeted(int target) const ;
BOOL AddMapTarget(int mapx, int mapy);

BOOL STD_CanAddTarget(int hd = 0, int range = 0);
BOOL AllTargetsChosen();
BOOL ValidNumTargets();
BOOL HDLimitReached();
CString FormatRemainingTargsText();

};

*/

}

SPELL_TARGETING_DATA.prototype.Clear = function () {
    this.m_targType = TargetingType.NowSelf;
    this.m_SelectingUnits = true;
    this.m_canTargetFriend = false;
    this.m_canTargetEnemy = true;
    this.m_IsPartyAligned = true;
    this.m_MaxHitDice = 0;
    this.m_MaxTargets = 0;
    this.m_MaxRange = 0;
    this.m_maxRangeX = 99999;
    this.m_maxRangeY = 99999;
    this.m_MapTargetX = -1;
    this.m_MapTargetY = -1;
    this.m_HDTotal = 0.0;
    this.m_targets = [];
    this.m_maptarglist.RemoveAll();

}
