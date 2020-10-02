
#ifndef __GPDLOPCODES_H__
#define __GPDLOPCODES_H__

enum BINOPS {
	// Opcodes that require an address or number
	BINOP_ILLEGAL				=0,
	BINOP_JUMP,			
	BINOP_ReferenceGLOBAL, // Bit 23 of address means store
	BINOP_JUMPFALSE,
	BINOP_STORE_FP,
	BINOP_FETCH_FP,
	BINOP_SUBOP,       // Opcode with sub-opcode
	BINOP_CALL,        // Call user-defined function
	BINOP_RETURN, //0x08   // subop = #locals<<12 + #parameters to pop
  BINOP_LOCALS,
  BINOP_FETCHTEXT,  //c-style string from the code segment
};
// commonly used expression
const unsigned int SHIFTED_BINOP = (BINOP_SUBOP<<24);

enum SUBOPS {
	SUBOP_ILLEGAL			            =0x000,
	SUBOP_NOOP                    =0x001,
	SUBOP_POP                     =0x002,
	SUBOP_DUP                     =0x003,			
	SUBOP_LISTEN                  =0x004,
	SUBOP_LISTENTEXT              =0x005,
	SUBOP_TESTKEY                 =0x006,
	SUBOP_SAY                     =0x007,
	SUBOP_NUMERIC                 =0x008,
	SUBOP_nLESS                   =0x009,
	SUBOP_nTIMES                  =0x00a,
	SUBOP_nMINUS                  =0x00b,
	SUBOP_PLUS                    =0x00c, 
	SUBOP_ISEQUAL                 =0x00d,
	SUBOP_nISEQUAL                =0x00e,
	SUBOP_GREP                    =0x00f, 
	SUBOP_RANDOM                  =0x010,
	SUBOP_SWAP                    =0x011,
	SUBOP_OVER                    =0x012,
	SUBOP_DEBUG                   =0x013,
	SUBOP_iTIMES                  =0x014,
	SUBOP_SET_GLOBAL_ASL          =0x015,
	SUBOP_GET_GLOBAL_ASL          =0x016,
	SUBOP_NOT                     =0x017,
	SUBOP_LENGTH                  =0x018,
	SUBOP_SET_CHAR_ASL            =0x019,
	SUBOP_PARTYSIZE               =0x01a,
	SUBOP_GET_CHAR_SEX            =0x01b,
	SUBOP_GET_CHAR_ASL            =0x01c,
	SUBOP_GET_CHAR_NAME           =0x01d,
	SUBOP_IndexToActor            =0x01e,
	SUBOP_nGREATER                =0x01f,
	SUBOP_DELETE_LEVEL_ASL        =0x020,
	SUBOP_DEPRECATED_SET_LEVEL_STATS_ASL    = 0x21, //SUBOP_SET_LEVEL_STATS_ASL     =0x021,
	SUBOP_DEPRECATED_DELETE_LEVEL_STATS_ASL = 0x22, //DELETE_LEVEL_STATS_ASL  =0x022,
  SUBOP_SET_PARTY_ASL           =0x023,
  SUBOP_GET_PARTY_ASL           =0x024,
  SUBOP_IF_PARTY_ASL            =0x025,
  SUBOP_DELETE_PARTY_ASL        =0x026,
  SUBOP_SMALL_PICTURE           =0x027,
  SUBOP_SET_QUEST               =0x028,
  SUBOP_GET_CHAR_AC             =0x029,
  SUBOP_SET_CHAR_AC             =0x02a,
  SUBOP_GET_CHAR_HITPOINTS      =0x02b,
  SUBOP_SET_CHAR_HITPOINTS      =0x02c,
  SUBOP_GET_CHAR_THAC0          =0x02d,
  SUBOP_SET_CHAR_THAC0          =0x02e,
  SUBOP_GET_CHAR_RDYTOTRAIN     =0x02f,
  SUBOP_SET_CHAR_RDYTOTRAIN     =0x030,
  SUBOP_GetFriendly             =0x031,
  SUBOP_RollHitPointDice        =0x032,
  SUBOP_AURA_Destroy            =0x033,
  SUBOP_AURA_Create             =0x034,
  SUBOP_AURA_AddSA              =0x035,
  SUBOP_AURA_GetSA              =0x036,
  SUBOP_AURA_RemoveSA           =0x037,
  SUBOP_AURA_Size               =0x038,             
  SUBOP_AURA_Shape              =0x039,            
  SUBOP_AURA_Attach             =0x03a,
  SUBOP_AURA_Location           =0x03b,
  SUBOP_AURA_Spell              =0x03c,
  SUBOP_AURA_Combatant          =0x03d,
  SUBOP_AURA_Wavelength         =0x03e,
  SUBOP_GET_CHAR_AGE            =0x03f,
  SUBOP_SET_CHAR_AGE            =0x040,
  SUBOP_GET_CHAR_MAXAGE         =0x041,
  SUBOP_SET_CHAR_MAXAGE         =0x042,
  SUBOP_GET_CHAR_MAXMOVE        =0x043,
  SUBOP_SET_CHAR_MAXMOVE        =0x044,
  SUBOP_GET_CHAR_LIMITED_STR    =0x045,
  SUBOP_SET_CHAR_PERM_STR       =0x046,
  SUBOP_GET_CHAR_LIMITED_STRMOD =0x047,
  SUBOP_SET_CHAR_PERM_STRMOD    =0x048,
  SUBOP_GET_CHAR_LIMITED_INT    =0x049,
  SUBOP_SET_CHAR_PERM_INT       =0x04a,
  SUBOP_GET_CHAR_LIMITED_WIS    =0x04b,
  SUBOP_SET_CHAR_PERM_WIS       =0x04c,
  SUBOP_GET_CHAR_LIMITED_DEX    =0x04d,
  SUBOP_SET_CHAR_PERM_DEX       =0x04e,
  SUBOP_GET_CHAR_LIMITED_CON    =0x04f,
  SUBOP_SET_CHAR_PERM_CON       =0x050,
  SUBOP_GET_CHAR_LIMITED_CHA    =0x051,
  SUBOP_SET_CHAR_PERM_CHA       =0x052,
  SUBOP_GET_CHAR_MAXENC         =0x053,
  SUBOP_SET_CHAR_MAXENC         =0x054,
  SUBOP_GET_CHAR_ENC            =0x055,
  SUBOP_GET_CHAR_GENDER         =0x056,
  SUBOP_SET_CHAR_GENDER         =0x057,
  SUBOP_GET_CHAR_CLASS          =0x058,
  SUBOP_SET_CHAR_CLASS          =0x059,
  SUBOP_GET_CHAR_ALIGNMENT      =0x05a,
  SUBOP_SET_CHAR_ALIGNMENT      =0x05b,
  SUBOP_GET_CHAR_STATUS         =0x05c,
  SUBOP_SET_CHAR_STATUS         =0x05d,
  SUBOP_GET_CHAR_UNDEAD         =0x05e,
  SUBOP_SET_CHAR_UNDEAD         =0x05f,
  SUBOP_GET_CHAR_SIZE           =0x060,
  SUBOP_SET_CHAR_SIZE           =0x061,
  SUBOP_GET_CHAR_MAGICRESIST    =0x062,
  SUBOP_SET_CHAR_MAGICRESIST    =0x063,
  SUBOP_GrSet                   =0x064,
  SUBOP_GrSetLinefeed           =0x065,
  SUBOP_GrMoveTo                =0x066,
  SUBOP_GrFormat                =0x067,
  SUBOP_GrColor                 =0x068,
  SUBOP_GrPrint                 =0x069,
  SUBOP_GrPrtLF                 =0x06a,
  SUBOP_GrMark                  =0x06b,
  SUBOP_GrMove                  =0x06c,
  SUBOP_GrTab                   =0x06d,
  SUBOP_GrPic                   =0x06e,
  SUBOP_GET_CHAR_ADJAC          =0x06f,
  SUBOP_GET_CHAR_EFFAC          =0x070,
  SUBOP_GET_CHAR_PERM_STR       =0x071,
  SUBOP_GET_CHAR_PERM_STRMOD    =0x072,
  SUBOP_GET_CHAR_PERM_INT       =0x073,
  SUBOP_GET_CHAR_PERM_WIS       =0x074,
  SUBOP_GET_CHAR_PERM_DEX       =0x075,
  SUBOP_GET_CHAR_PERM_CON       =0x076,
  SUBOP_GET_CHAR_PERM_CHA       =0x077,
  SUBOP_MODIFY_CHAR_ATTRIBUTE   =0x078,
  SUBOP_REMOVE_CHAR_MODIFICATION=0x079,
  SUBOP_GET_CHAR_NBRHITDICE     =0x083,
  SUBOP_GET_CHAR_NBRATTACKS     =0x084,
  SUBOP_GET_CHAR_MORALE         =0x085,
  SUBOP_SET_CHAR_MORALE         =0x086,
  SUBOP_ToHitComputation_Roll   =0x087,
  SUBOP_nPLUS                   =0x09d,
  SUBOP_GET_PARTY_FACING        =0x09e,
  SUBOP_SET_PARTY_FACING        =0x09f,
  SUBOP_GET_PARTY_DAYS          =0x0a0,
  SUBOP_SET_PARTY_DAYS          =0x0a1,
  SUBOP_GET_PARTY_HOURS         =0x0a2,
  SUBOP_SET_PARTY_HOURS         =0x0a3,
  SUBOP_GET_PARTY_MINUTES       =0x0a4,
  SUBOP_SET_PARTY_MINUTES       =0x0a5,
  SUBOP_GET_PARTY_ACTIVECHAR    =0x0a6,
  SUBOP_SET_PARTY_ACTIVECHAR    =0x0a7,
  SUBOP_GET_GAME_CURRLEVEL      =0x0a8,
  SUBOP_GET_GAME_VERSION        =0x0a9,
  SUBOP_Myself                  =0x0aa,
  SUBOP_Gender                  =0x0ab,
  SUBOP_LOR                     =0x0ac,
  SUBOP_LAND                    =0x0ad,
  SUBOP_nOR                     =0x0ae,
  SUBOP_nXOR                    =0x0af,
  SUBOP_nAND                    =0x0b0,
  SUBOP_GET_CHAR_MAXHITPOINTS   =0x0b1,
  SUBOP_SET_CHAR_MAXHITPOINTS   =0x0b2,
  SUBOP_LESS                    =0x0b3,
  SUBOP_LESSEQUAL               =0x0b4,
  SUBOP_nLESSEQUAL              =0x0b5,
  SUBOP_GREATER                 =0x0b6,
  SUBOP_GREATEREQUAL            =0x0b7,
  SUBOP_nGREATEREQUAL           =0x0b8,
  SUBOP_nSLASH                  =0x0b9,
  SUBOP_nPERCENT                =0x0ba,
  SUBOP_nNOTEQUAL               =0x0bb,
  SUBOP_NOTEQUAL                =0x0bc,
  SUBOP_iPLUS                   =0x0bd,
  SUBOP_iMINUS                  =0x0be,
  SUBOP_iDIV                    =0x0bf,
  SUBOP_iMOD                    =0x0c0,
  SUBOP_iEQUAL                  =0x0c1,
  SUBOP_iLESS                   =0x0c2,
  SUBOP_iGREATER                =0x0c3,
  SUBOP_nNEGATE                 =0x0c4,
  SUBOP_Name                    =0x0c5,
  SUBOP_TargetContext           =0x0c6,
  SUBOP_ClassContext            =0x0c7,
  SUBOP_RaceContext             =0x0c8,
  SUBOP_Status                  =0x0c9,
  SUBOP_Alignment               =0x0ca,
  SUBOP_AlignmentGood           =0x0cb,
  SUBOP_AlignmentEvil           =0x0cc,
  SUBOP_AlignmentLawful         =0x0cd,
  SUBOP_AlignmentNeutral        =0x0ce,
  SUBOP_AlignmentChaotic        =0x0cf,
  SUBOP_HitPoints               =0x0d0,
  SUBOP_InParty                 =0x0d1,
  SUBOP_IsUndead                =0x0d7,
  SUBOP_FALSE                   =0x0d8,
  SUBOP_ONE                     =0x0d9,
  SUBOP_ZERO                    =0x0da,
  SUBOP_MIDDLE                  =0x0db,
  SUBOP_DAT_Race_Weight         =0x0dc,
  SUBOP_DAT_Race_Height         =0x0dd,
  SUBOP_GET_CONFIG              =0x0de,
  SUBOP_DAT_Item_CommonName     =0x0df,
  SUBOP_DAT_Item_IDName         =0x0e0,
  SUBOP_DAT_Class_Baseclasses   =0x0e1,
  SUBOP_DAT_Item_Priority       =0x0e2,
  SUBOP_DAT_Item_MaxRange       =0x0e3,
  SUBOP_DAT_Item_MediumRange    =0x0e4,
  SUBOP_DAT_Item_ShortRange     =0x0e5,
  SUBOP_DAT_Item_DamageSmall    =0x0e6,
  SUBOP_DAT_Item_DamageLarge    =0x0e7,
  SUBOP_DAT_Item_AttackBonus    =0x0e8,
  SUBOP_SET_CHAR_ICON_INDEX     =0x0e9,
  SUBOP_GET_CHAR_ICON_INDEX     =0x0ea,
  SUBOP_SLEEP                   =0x0eb,
  SUBOP_DRAWADVENTURESCREEN     =0x0ec,
  SUBOP_GET_CHAR_ADJ_STR        =0x0ed, 
  SUBOP_GET_CHAR_ADJ_STRMOD     =0x0ee,
  SUBOP_GET_CHAR_ADJ_INT        =0x0ef,
  SUBOP_GET_CHAR_ADJ_WIS        =0x0f0,
  SUBOP_GET_CHAR_ADJ_DEX        =0x0f1,
  SUBOP_GET_CHAR_ADJ_CON        =0x0f2,
  SUBOP_GET_CHAR_ADJ_CHA        =0x0f3,
                /*       Numbers available here      */
  SUBOP_IndexOf                     =0x104,
  SUBOP_GET_CHAR_TYPE               =0x107,
  SUBOP_COINNAME                    =0x10a,
  SUBOP_COINCOUNT                   =0x10b,
  SUBOP_COINRATE                    =0x10c,
  SUBOP_GET_PARTY_MONEYAVAILABLE    =0x10d,
  SUBOP_GET_VAULT_MONEYAVAILABLE    =0x10e,
  SUBOP_LAST_ATTACKER_OF            =0x10f,
  SUBOP_NOT_USED_FOR_ANYTHING1      =0x110,
  SUBOP_NOT_USED_FOR_ANYTHING2      =0x111,
  SUBOP_LEAST_DAMAGED_FRIENDLY      =0x112,
  SUBOP_MOST_DAMAGED_FRIENDLY       =0x113,
  SUBOP_NEAREST_TO                  =0x114,
  SUBOP_NEAREST_ENEMY_TO            =0x115,
  SUBOP_LEAST_DAMAGED_ENEMY         =0x116,
  SUBOP_MOST_DAMAGED_ENEMY          =0x117,
  SUBOP_IS_AFFECTED_BY_SPELL        =0x118,
  SUBOP_IS_AFFECTED_BY_SPELL_ATTR   =0x119,
  SUBOP_CURR_CHANGE_BY_VAL          =0x11A,
  SUBOP_GET_ISMAMMAL                =0x11F,
  SUBOP_GET_ISANIMAL                =0x120,
  SUBOP_GET_ISSNAKE                 =0x121,
  SUBOP_GET_ISGIANT                 =0x122,
  SUBOP_GET_ISALWAYSLARGE           =0x123,
  SUBOP_GET_HASDWARFACPENALTY       =0x124,
  SUBOP_GET_HASGNOMEACPENALTY       =0x125,
  SUBOP_GET_HASDWARFTHAC0PENALTY    =0x126,
  SUBOP_GET_HASGNOMETHAC0PENALTY    =0x127,
  SUBOP_GET_HASRANGERDMGPENALTY     =0x128,
  SUBOP_GET_HASPOISONIMMUNITY       =0x129,
  SUBOP_GET_HASDEATHIMMUNITY        =0x12A,
  SUBOP_GET_HASCONFUSIONIMMUNITY    =0x12B,
  SUBOP_GET_HASVORPALIMMUNITY       =0x12C,
  SUBOP_GET_CANBEHELDORCHARMED      =0x12D,
  SUBOP_GET_AFFECTEDBYDISPELEVIL    =0x12E,
  SUBOP_GIVE_CHAR_ITEM              =0x133,
  SUBOP_TAKE_CHAR_ITEM              =0x134,
  SUBOP_LOGIC_BLOCK_VALUE           =0x13B,
  SUBOP_GET_CHAR_DAMAGEBONUS        =0x13C,
  SUBOP_SET_CHAR_DAMAGEBONUS        =0x13D,
  SUBOP_GET_CHAR_HITBONUS           =0x13E,
  SUBOP_SET_CHAR_HITBONUS           =0x13F,
  SUBOP_CHAR_REMOVEALLSPELLS        =0x144,
  SUBOP_MYINDEX                     =0x146,
  SUBOP_CASTSPELLONTARGET           =0x147,
  SUBOP_CASTSPELLONTARGETAS         =0x148,
  SUBOP_CHAR_DISPELMAGIC            =0x14A,
  SUBOP_CHAR_REMOVEALLITEMCURSE     =0x14B,
  SUBOP_SET_LEVEL_STATS_ASL         =0x14C,
  SUBOP_DELETE_LEVEL_STATS_ASL      =0x14D,
  SUBOP_FORCENUMERIC                =0x14E,
  SUBOP_SET_CHAR_SA                 =0x14F,
  SUBOP_ItemContext                 =0x150,
  SUBOP_GET_ITEM_SA                 =0x151,
  SUBOP_GET_HOOK_PARAM              =0x152,
  SUBOP_GET_CHAR_Ready              =0x153,
  SUBOP_IF_CHAR_ASL                 =0x154,
  SUBOP_SET_CHAR_Ready              =0x155,
  SUBOP_AddCombatant                =0x156,
  SUBOP_DelimitedStringCount        =0x157,
  SUBOP_DelimitedStringSubstring    =0x158,
  SUBOP_SA_ITEM_GET                 =0x159,
  SUBOP_SET_CHAR_CLERICLVL          =0x15a,
  SUBOP_SET_CHAR_FIGHTERLVL         =0x15b,
  SUBOP_SET_CHAR_RANGERLVL          =0x15c,
  SUBOP_SET_CHAR_DRUIDLVL           =0x15d,
  SUBOP_SET_CHAR_PALADINLVL         =0x15e,
  SUBOP_SET_CHAR_THIEFLVL           =0x15f,
  SUBOP_SA_CLASS_GET                =0x162,
  SUBOP_SA_SPELL_GET                =0x163,
  SUBOP_SA_CHARACTER_GET            =0x164,
  SUBOP_SA_PARAM_GET                =0x165,
  SUBOP_SA_PARAM_SET                =0x166,
  SUBOP_SET_COMBATANT_SA            =0x167,
  SUBOP_SA_NAME                     =0x168,
  SUBOP_SA_REMOVE                   =0x169,
  SUBOP_SET_CHAR_MAGICUSERLVL       =0x16a,
  SUBOP_GET_CHAR_Exp                =0x16b,
  SUBOP_SET_CHAR_Exp                =0x16c,
  SUBOP_GET_CHAR_Lvl                =0x16d,
  SUBOP_SET_CHAR_Lvl                =0x16e,
  SUBOP_ListAdjacentCombatants      =0x16f,
  SUBOP_SA_COMBATANT_GET            =0x170,
  SUBOP_GetSpellbook                =0x171,  // Deprecated then readded 20160907 PRS
  SUBOP_GET_CHARACTER_SA            =0x172,
  SUBOP_SpellContext                =0x173,
  SUBOP_GET_SPELL_Level             =0x174,
  SUBOP_NextCreatureIndex           =0x175,
  SUBOP_SET_HOOK_PARAM              =0x176,
  SUBOP_RUN_CHAR_SCRIPTS            =0x177,
  SUBOP_VisualDistance              =0x178,
  SUBOP_DebugWrite                  =0x179,
  SUBOP_ComputeAttackDamage         =0x17a,
  SUBOP_GET_SPELL_CanBeDispelled    =0x17b,
  SUBOP_GET_EVENT_Attribute         =0x17c,
  SUBOP_DELETE_CHARACTER_SA         =0x17d,
  SUBOP_DELETE_COMBATANT_SA         =0x17e,
  SUBOP_SET_CHAR_SEX                =0x17f,
  SUBOP_GET_COMBATANT_SA            =0x180,
  SUBOP_GET_CHAR_SA                 =0x181,
  SUBOP_TeleportCombatant           =0x182,
  SUBOP_AttackerContext             =0x183,
  SUBOP_CombatantContext            =0x184,
  SUBOP_CombatantLocation           =0x185,
  SUBOP_IsLineOfSight               =0x186,
  SUBOP_DelimitedStringFilter       =0x187,
  SUBOP_DelimitedStringAdd          =0x188,
  SUBOP_DUMP_CHARACTER_SAS          =0x189,
  SUBOP_Wiggle                      =0x18a,
  SUBOP_GET_CHAR_RACE               =0x18b,
  SUBOP_SET_CHAR_RACE               =0x18c,
  SUBOP_UpCase                      =0x18d,
  SUBOP_DownCase                    =0x18e,
  SUBOP_Capitalize                  =0x18f,
  SUBOP_AURA_SetData                =0x190,
  SUBOP_AURA_GetData                =0x191,
  SUBOP_KNOW_SPELL                  =0x192,
  SUBOP_SetFriendly                 =0x193,
  SUBOP_GetCombatRound              =0x194,
  SUBOP_DAT_Baseclass_Experience    =0x195,
  SUBOP_DAT_Baseclass_Level         =0x196,
  SUBOP_SpellgroupContext           =0x197,
  SUBOP_MonsterTypeContext          =0x198,
  SUBOP_AbilityContext              =0x199,
  SUBOP_TraitContext                =0x19a,
  SUBOP_CharacterContext            =0x19b,
  SUBOP_GET_SPELL_SA                =0x19c,
  SUBOP_GET_MONSTERTYPE_SA          =0x19e,
  SUBOP_GET_RACE_SA                 =0x19f,
  SUBOP_GET_ABILITY_SA              =0x1a0,
  SUBOP_GET_CLASS_SA                =0x1a2,
  SUBOP_SET_CHARACTER_SA            =0x1a4,
  SUBOP_SA_BASECLASS_GET            =0x1a5,
  SUBOP_GET_BASECLASS_SA            =0x1a6,
  SUBOP_SA_MONSTERTYPE_GET          =0x1ad,
  SUBOP_SA_RACE_GET                 =0x1ae,
  SUBOP_SA_ABILITY_GET              =0x1af,
  SUBOP_DELETE_CLASS_SA             =0x1b6,
  SUBOP_SelectSpell                 =0x1b8,
  SUBOP_Memorize                    =0x1b9,
  SUBOP_DelimitedStringHead         =0x1ba,
  SUBOP_DelimitedStringTail         =0x1bb,
  SUBOP_GET_CHAR_ADJTHAC0           =0x1bc,
  SUBOP_MonsterPlacement            =0x1bd,
  SUBOP_GET_PARTY_TIME              =0x1be,
  SUBOP_SET_PARTY_TIME              =0x1bf,
  SUBOP_SET_AFFECTEDBYDISPELEVIL    =0x1c0,
  SUBOP_SetWall                     =0x1c1,
  SUBOP_SetBackground               =0x1c2,
  SUBOP_SetOverlay                  =0x1c3,
  SUBOP_SetDoor                     =0x1c4,
  SUBOP_SetBlockage                 =0x1c5,
  SUBOP_GetWall                     =0x1c6,
  SUBOP_GetBackground               =0x1c7,
  SUBOP_GetOverlay                  =0x1c8,
  SUBOP_GetDoor                     =0x1c9,
  SUBOP_GetBlockage                 =0x1ca,
  SUBOP_COMBATANT_AVAILATTACKS      =0x1cb,
  SUBOP_CALL_GLOBAL_SCRIPT          =0x1cc,
  SUBOP_RUN_CHAR_SE_SCRIPTS         =0x1cd,
  SUBOP_REMOVE_SPELL_EFFECT         =0x1ce,
  SUBOP_RUN_AREA_SE_SCRIPTS         =0x1cf,
  SUBOP_RUN_CHAR_PS_SCRIPTS         =0x1d0,
  SUBOP_SA_SOURCE_TYPE              =0x1d1,
  SUBOP_SA_SOURCE_NAME              =0x1d2,
  SUBOP_IntegerTable                =0x1d3,
  SUBOP_GET_PARTY_LOCATION          =0x1d4,
  SUBOP_SET_PARTY_XY                =0x1d5,
  SUBOP_ForEachPartyMember          =0x1d6,
  SUBOP_ForEachPossession           =0x1d7,
  SUBOP_GetCombatantState           =0x1d8,
  SUBOP_IsIdentified                =0x1d9,
  SUBOP_SkillAdj                    =0x1da,
  SUBOP_SpellAdj                    =0x1db,
  SUBOP_SetMemorizeCount            =0x1dc,
  SUBOP_GetHighestLevelBaseclass    =0x1dd,
  SUBOP_GetBaseclassLevel           =0x1de,

};

//**********************************************************************//RDR
//                                                                      //RDR
//**********************************************************************//RDR
enum COPS {  //Character-sized opcodes                                  //RDR
	COP_ILLEGAL				=0,                                                 //RDR
  COP_LOR           =1,                                                 //RDR
  COP_LAND          =2,                                                 //RDR
  COP_ISEQUAL       =3,                                                 //RDR
  COP_ISNOTEQUAL    =4,                                                 //RDR
  COP_LESS          =5,                                                 //RDR
  COP_LESSEQUAL     =6,                                                 //RDR
  COP_GREATER       =7,                                                 //RDR
  COP_GREATEREQUAL  =8,                                                 //RDR
  COP_PLUS          =9,                                                 //RDR
  COP_TIMES         =10,                                                //RDR
  COP_SLASH         =11,                                                //RDR
  COP_PERCENT       =12,                                                //RDR
  COP_NEGATE        =13,                                                //RDR
  COP_NOT           =14,                                                //RDR
  COP_RDR           =15,                                                //RDR
  COP_INTEGER1      =16,                                                //RDR
  COP_INTEGER2      =17,                                                //RDR
  COP_INTEGER3      =18,                                                //RDR
  COP_INTEGER4      =19,                                                //RDR
  COP_LXOR          =20,                                                //RDR
  COP_BITOR         =21,                                                //RDR
  COP_BITXOR        =22,                                                //RDR
  COP_BITAND        =23,                                                //RDR
  COP_MINUS         =24,                                                //RDR
  COP_MIN           =25,                                                //RDR
  COP_MAX           =26,                                                //RDR
  COP_NONE          =27,  //No operation needed.  Like unary Plus.      //RDR
  COP_RDR1          =28,  // Byte-coded COP_RDR                         //RDR
};                                                                      //RDR
                                                                        //RDR
enum CTOKENTYPE {CTKN_NONE=0,                                           //RDR
        CTKN_NAME,                                                      //RDR
        CTKN_PLUS,                                                      //RDR
        CTKN_MINUS,                                                     //RDR
        CTKN_INTEGER,                                                   //RDR
        CTKN_OPENPAREN,                                                 //RDR
        CTKN_CLOSEPAREN,                                                //RDR
        CTKN_COLON,                                                     //RDR
        CTKN_GEAR,                                                      //RDR
        CTKN_SLASH,                                                     //RDR
        CTKN_EQUAL,                                                     //RDR
        CTKN_LESS,                                                      //RDR
        CTKN_GREATER,                                                   //RDR
        CTKN_ISEQUAL,                                                   //RDR
        CTKN_NOTEQUAL,                                                  //RDR
        CTKN_LOR,                                                       //RDR
        CTKN_LXOR,                                                      //RDR
        CTKN_LAND,                                                      //RDR
        CTKN_BITOR,                                                     //RDR
        CTKN_BITXOR,                                                    //RDR
        CTKN_BITAND,                                                    //RDR
        CTKN_LESSEQUAL,                                                 //RDR
        CTKN_GREATEREQUAL,                                              //RDR
        CTKN_PERCENT,                                                   //RDR
        CTKN_NOT,                                                       //RDR
        CTKN_DICE,                                                      //RDR
        CTKN_MIN,                                                       //RDR
        CTKN_MAX,                                                       //RDR
        CTKN_STRING                                                     //RDR
       };                                                               //RDR
                                                                        //RDR
#endif

#define CTX_Combatant   0x00000001
#define CTX_Trait       0x00000002
#define CTX_Target      0x00000004
#define CTX_Character   0x00000008
#define CTX_Attacker    0x00000010
#define CTX_Item        0x00000020
#define CTX_Class       0x00000040
#define CTX_Race        0x00000080
#define CTX_Spell       0x00000100
#define CTX_BaseClass   0x00000200
#define CTX_Myself      0x00000400
#define CTX_SpellGroup  0x00000800
#define CTX_MonsterType 0x00001000
#define CTX_Ability     0x00002000

