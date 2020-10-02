#ifndef __COMBATANT_SUMMARY_H__
#define __COMBATANT_SUMMARY_H__

#ifdef UAFEngine

struct SHIELD_SUMMARY
{
  int index;  // Index in combatant's inventory (myItems)
  int protection;
};

class ITEM_DATA;
enum weaponClassType;

struct WEAPON_SUMMARY
{
  int              index; // Index in combatants inventory (myItems)
  const ITEM_DATA  *pWeapon;
  const SPELL_DATA *pSpell;
  weaponClassType	 Wpn_Type;
  CString          ammoType;
  int              range22;  // Squared (2*range)
  int              protection;
  int              largeDamageDice;
  int              smallDamageDice;
  int              largeDamageBonus;
  int              smallDamageBonus;
  int              ROF_Per_Round;
  int              attackBonus;
  int              priority;
};

struct AMMO_SUMMARY
{
  int              index; // Index in combatants inventory (myItems)
  const ITEM_DATA  *pAmmo;
  CString          ammoType;
  //int              range22;  // Squared (2*range)
  int              protection;
  int              qty;
  int              largeDamageDice;
  int              smallDamageDice;
  int              largeDamageBonus;
  int              smallDamageBonus;
  int              ROF_Per_Round;
  int              attackBonus;
  int              priority;
};

struct ATTACK_SUMMARY
{
  int              index; // Index in combatants action list
  const SPELL_DATA *pSpell;
  int              damage;
};

struct COMBAT_SUMMARY_COMBATANT
{
  COMBAT_SUMMARY_COMBATANT(void)
  {
    Clear();
  };
  void operator =(const COMBAT_SUMMARY_COMBATANT& csc);
  void Clear(void)
  {
    shieldSummary.SetSize(0);
    weaponSummary.SetSize(0);
    attackSummary.SetSize(0);
    shieldToReady = 0;
    index = 0;
    fleeing = 0;
    state = 0;
    friendly = 0;
    availAttacks = 0;
    x = y = -1;
    width = height = 1;
    isLarge = 0;
    canMove = 0;
  };
  int index;
  CHARACTER *pCharacter;
  int x, y, width, height;
  int fleeing;
  int friendly;  // Friendly to COMBAT_SUMMARY_COMBATANT[0]
  int state;  // from COMBATANT::individualCombatantState and various flags
  int availAttacks;
  int isLarge;
  int canMove;
  int AIBaseclass; // -1 means not computed.  Value from combatant's SA named AIBaseclass
                   // bit 15 means no such SAs.

  mCArray<SHIELD_SUMMARY, SHIELD_SUMMARY &> shieldSummary;
  unsigned int shieldToReady;
  DEFINE_mCARRAY_ACCESS_FUNCTIONS(Shield,                     /* array data name      */ \
                                  int *,                      /* Search variable type */ \
                                  idx,                        /* search variable name */ \
                                  shieldSummary,              /* array name           */ \
                                  SHIELD_SUMMARY,             /* array data type      */ \
                                  COMBAT_SUMMARY_COMBATANT)   /* type of *this        */
  mCArray<WEAPON_SUMMARY, WEAPON_SUMMARY &> weaponSummary;
  //unsigned int WeaponToReady;
  DEFINE_mCARRAY_ACCESS_FUNCTIONS(Weapon,                     /* array data name      */ \
                                  int *,                      /* Search variable type */ \
                                  idx,                        /* search variable name */ \
                                  weaponSummary,              /* array name           */ \
                                  WEAPON_SUMMARY,             /* array data type      */ \
                                  COMBAT_SUMMARY_COMBATANT)   /* type of *this        */
  mCArray<CString, CString &> debugSummary;
  DEFINE_mCARRAY_ACCESS_FUNCTIONS(DebugItem,                  /* array data name      */ \
                                  int *,                      /* Search variable type */ \
                                  idx,                        /* search variable name */ \
                                  debugSummary,               /* array name           */ \
                                  CString,                    /* array data type      */ \
                                  COMBAT_SUMMARY_COMBATANT)   /* type of *this        */
  mCArray<AMMO_SUMMARY, AMMO_SUMMARY &> ammoSummary;
  DEFINE_mCARRAY_ACCESS_FUNCTIONS(Ammo,                       /* array data name      */ \
                                  int *,                      /* Search variable type */ \
                                  idx,                        /* search variable name */ \
                                  ammoSummary,                /* array name           */ \
                                  AMMO_SUMMARY,               /* array data type      */ \
                                  COMBAT_SUMMARY_COMBATANT)   /* type of *this        */
  mCArray<ATTACK_SUMMARY, ATTACK_SUMMARY &> attackSummary;
  //unsigned int WeaponToReady;
  DEFINE_mCARRAY_ACCESS_FUNCTIONS(Attack,                     /* array data name      */ \
                                  int *,                      /* Search variable type */ \
                                  idx,                        /* search variable name */ \
                                  attackSummary,              /* array name           */ \
                                  ATTACK_SUMMARY,             /* array data type      */ \
                                  COMBAT_SUMMARY_COMBATANT)   /* type of *this        */

};


enum ACTION_TYPE
{
  AT_Unknown            = 0,
  AT_SpellCaster        = 1,
  AT_Advance            = 2,
  AT_RangedWeapon       = 3,
  AT_MeleeWeapon        = 4,
  AT_Judo               = 5, //Hands and feet....no weapon
  AT_SpellLikeAbility   = 6,
};

struct COMBAT_SUMMARY_ACTION
{
  COMBAT_SUMMARY_ACTION(void)
  {
    actionType  = AT_Unknown;
    targetOrd   = 0;           // COMBAT_SUMMARY ordinal
    weaponOrd   = 0;           // COMBAT_SUMMARY_COMBATANT ordinal
    ammoOrd     = 0;           // COMBAT_SUMMARY_COMBATANT ordinal
    attackOrd   = 0;           // COMBAT_SUMMARY_COMBATANT ordinal
    advance     = 0;           // Move closer to target.
    distance22  = 0;
    damage      = 0;
    pCSC=pMe=pHe=NULL;
  };
  ACTION_TYPE actionType;
  int targetOrd; // The COMBAT_SUMMARY combatant number (0=active combatant)
  int weaponOrd; // The COMBAT_SUMMARY_COMBATANT weapon number
  int ammoOrd;   // The COMBAT_SUMMARY_COMBATANT ammo number
  int attackOrd; // The COMABT_SUMMARY_COMABTANT attack number
  int distance22;// (distance * 2) squared
  int damage;
  int advance;
  COMBAT_SUMMARY_COMBATANT *pCSC;
  COMBAT_SUMMARY_COMBATANT *pMe;
  COMBAT_SUMMARY_COMBATANT *pHe;
};


struct COMBAT_SUMMARY
{
  COMBAT_SUMMARY(void)
  {
    m_thinkCount = 0;
    m_filenum = 0;
    reachableCells = (int *)malloc(MAX_TERRAIN_WIDTH*MAX_TERRAIN_HEIGHT*sizeof(reachableCells[0]));
    Clear();
  };
  ~COMBAT_SUMMARY(void)
  {
    free(reachableCells);
  };

  void Clear(void)
  {
    m_combatants.SetSize(0);
    m_availableActions.SetSize(0);
    pActionA=pActionB=NULL;
    memset(reachableCells,0xff,MAX_TERRAIN_WIDTH*MAX_TERRAIN_HEIGHT*sizeof(reachableCells[0]));
  };
  int m_thinkCount;
  int m_filenum;
  int m_he; // The current non-me combatant.
  COMBAT_SUMMARY_ACTION *pActionA;
  COMBAT_SUMMARY_ACTION *pActionB;
  mCArray<COMBAT_SUMMARY_COMBATANT, COMBAT_SUMMARY_COMBATANT&>m_combatants;
  DEFINE_mCARRAY_ACCESS_FUNCTIONS(Combatant,                /* array data name      */ \
                                  int *,                    /* Search variable type */ \
                                  index,                    /* search variable name */ \
                                  m_combatants,             /* array name           */ \
                                  COMBAT_SUMMARY_COMBATANT, /* array data type      */ \
                                  COMBAT_SUMMARY)           /* type of *this        */
  mCArray<COMBAT_SUMMARY_ACTION, COMBAT_SUMMARY_ACTION &> m_availableActions;
  DEFINE_mCARRAY_ACCESS_FUNCTIONS(Action,                   /* array data name      */ \
                                  int *,                    /* Search variable type */ \
                                  index,                    /* search variable name */ \
                                  m_availableActions,       /* array name           */ \
                                  COMBAT_SUMMARY_ACTION,    /* array data type      */ \
                                  COMBAT_SUMMARY)           /* type of *this        */
  int *reachableCells;

};

#endif
#endif