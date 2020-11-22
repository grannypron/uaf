
function GLOBAL_SOUND_DATA() {

    this.hCharHit = 0;
    this.hCharMiss = 0;
    this.hPartyBump = 0;
    this.hPartyStep = 0;
    this.hDeathMusic = 0;
    this.CharHit = "";
    this.CharMiss = "";
    this.PartyBump = "";
    this.PartyStep = "";
    this.DeathMusic = "";

/**TODO
    public:
        //  DECLARE_SERIAL( GLOBAL_SOUND_DATA )
        GLOBAL_SOUND_DATA();
virtual ~GLOBAL_SOUND_DATA() { ClearSounds(); }
void Clear(BOOL ctor = FALSE);
void ClearSounds();
void Serialize(CArchive & ar);
void Serialize(CAR & ar);

#ifdef UAFEDITOR
void CrossReference(CR_LIST * pCRList);
void Export(JWriter & jw);
void Import(JReader & jr);
#endif

GLOBAL_SOUND_DATA & operator =(const GLOBAL_SOUND_DATA& src);
GLOBAL_SOUND_DATA(const GLOBAL_SOUND_DATA & src) { * this = src; }

BACKGROUND_SOUNDS IntroMusic;
BACKGROUND_SOUNDS CampMusic;
*/

};