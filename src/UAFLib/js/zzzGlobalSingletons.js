var TargetingType = new TargetingTypeObj();  // PORT NOTE:  This must be above the Globals() initalizer because of (at least) FakeCharacter

var Globals = new Globals();

var miscErrorType = new miscErrorTypeObj();
var debugStrings = new DEBUG_STRINGS();
var alignmentType = new alignmentTypeObj();
var surfaceType = new surfaceTypeObj();
var charStatusType = new charStatusTypeObj(charStatusType);
var combatVictorType = new combatVictorTypeObj();
var creatureSizeType = new creatureSizeTypeObj(creatureSizeType);
var individualCombatantState = new individualCombatantState();
var eventDirType = new eventDirTypeObj();
var eventDistType = new eventDistTypeObj();
var ActorInstanceType = new ActorInstanceTypeObj();
var EventSourceType = new EventSourceTypeObj();
var genderType = new genderTypeObj(genderType);
var eventType = new eventTypeObj();
var eventTurnUndeadModType = new eventTurnUndeadModTypeObj();
var eventSurpriseType = new eventSurpriseTypeObj();
var chainTriggerType = new chainTriggerTypeObj();
var eventTriggerType = new eventTriggerTypeObj();
var weaponClassType = new weaponClassTypeObj();
var MonsterPenaltyType = new MonsterPenaltyTypeObj();
var MonsterFormType = new MonsterFormTypeObj();
var itemData = new ITEM_DATA_TYPE();
var spellData = new SPELL_DATA_TYPE();
var monsterData = new MONSTER_DATA_TYPE();
var moneyData = new MONEY_DATA_TYPE();
var SCRIPT_SOURCE_TYPE = new SCRIPT_SOURCE_TYPEObj();
var SPECAB_STRING_TYPE = new SPECAB_STRING_TYPEObj();
var SPELL_EFFECTS_DATA = new SPELL_EFFECTS_DATA();
var CBFUNC = new CBFUNCObj();
var CBRESULT = new CBRESULTObj();
var globalData = new GLOBAL_STATS();
var combatData = new COMBAT_DATA();
var AURA_WAVELENGTH = new AURA_WAVELENGTHObj();
var AURA_SHAPE = new AURA_SHAPEObj();
var AURA_ATTACHMENT = new AURA_ATTACHMENTObj();
var OBSTICAL_TYPE = new OBSTICAL_TYPEObj();
var abilityData = new ABILITY_DATA_TYPE();
var Drawtile = new Drawtile();
var SoundMgr = new SoundMgrObj();
var party = new PARTY();
var GameRules = new GameRules();
var FormattedText = new FormattedText();
var DispText = new DispText();

var combatSummary = new COMBAT_SUMMARY();
var PATH_DIR = new PATH_DIRObj();
var monsterArrangement = new MonsterArrangement();

var raceData = new RACE_DATA_TYPE();
var baseclassData = new BASE_CLASS_DATA_TYPE();
var classData = new CLASS_DATA_TYPE();
var Items = new Items();
var specialAbilitiesData = new A_SPECABILITY_DEF_L();
var pScriptContext = new SCRIPT_CONTEXT();
var activeSpellList = new ACTIVE_SPELL_LIST();
var FLEEING_FLAGS = new FLEEING_FLAGS();

var pathMgr = new PATH_MANAGER(1000);

var RunTimeIF = new RunTimeIF();

var RNG = new DefaultRNG();

var DEFAULT_SPELL_EFFECT_FLAGS = SPELL_EFFECTS_DATA.ALL_TARG_TYPES;


var UAFLib = importNamespace("UAFLib");  // For Jint to access C# library
