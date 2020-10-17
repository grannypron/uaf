/** TODO **/
var CHAR_TYPE = 1;
var NPC_TYPE = 2;
var MONSTER_TYPE = 3;
var FAKE_CHARACTER_TYPE = 127;
var IN_PARTY_TYPE = 128;
var NO_READY_ITEM = -1;

// ArchiveString and DeArchiveString
//
// Empty CStrings are stored as a single
// character '*'. When loaded, the '*'
// is replaced with "" (empty string) again.
//
// The actual replacement char can be changed
// by altering ArchiveBlank. Default is '*'.
// 
// Many released versions of DC have
// used '*' so this must be supported
// in DAS regardless of what is used
// in ArchiveBlank.
//
var ArchiveBlank = "*";
var LegacyArchiveBlank = "*";
var AS = function(archive, cstring)
{
    if (cstring == null || cstring.length <= 0)
        archive.writeString(ArchiveBlank);
    else
        archive.writeString(cstring);
}

var DAS = function(archive)
{

    cstring = archive.readString();
    if ((cstring == ArchiveBlank) || (cstring == LegacyArchiveBlank))
        return "";
    return cstring;
}

var IF_KEYWORD_INDEX = {};
/* character keywords */
IF_KEYWORD_INDEX.CHAR_BASE = 0;
IF_KEYWORD_INDEX.CHAR_NAME = 1;
IF_KEYWORD_INDEX.CHAR_AC = 2;
IF_KEYWORD_INDEX.CHAR_HITPOINTS = 3;
IF_KEYWORD_INDEX.CHAR_THAC0 = 4;
IF_KEYWORD_INDEX.CHAR_ADJTHAC0 = 5;
IF_KEYWORD_INDEX.CHAR_RDYTOTRAIN = 6;
IF_KEYWORD_INDEX.CHAR_ADJAC = 7;
IF_KEYWORD_INDEX.CHAR_EFFAC = 8;
IF_KEYWORD_INDEX.CHAR_AGE = 9;
IF_KEYWORD_INDEX.CHAR_MAXAGE = 10;
IF_KEYWORD_INDEX.CHAR_MAXMOVE = 11;
IF_KEYWORD_INDEX.CHAR_ADJUSTED_STR = 12;
IF_KEYWORD_INDEX.CHAR_STR = 12;
IF_KEYWORD_INDEX.CHAR_ADJUSTED_STRMOD = 13;
IF_KEYWORD_INDEX.CHAR_STRMOD = 13;
IF_KEYWORD_INDEX.CHAR_ADJUSTED_INT = 14;
IF_KEYWORD_INDEX.CHAR_INT = 14;
IF_KEYWORD_INDEX.CHAR_ADJUSTED_WIS = 15;
IF_KEYWORD_INDEX.CHAR_WIS = 15;
IF_KEYWORD_INDEX.CHAR_ADJUSTED_DEX = 16;
IF_KEYWORD_INDEX.CHAR_DEX = 16;
IF_KEYWORD_INDEX.CHAR_ADJUSTED_CON = 17;
IF_KEYWORD_INDEX.CHAR_CON = 17;
IF_KEYWORD_INDEX.CHAR_ADJUSTED_CHA = 18;
IF_KEYWORD_INDEX.CHAR_CHA = 18;
IF_KEYWORD_INDEX.CHAR_MAXENC = 19;
IF_KEYWORD_INDEX.CHAR_ENC = 20;
IF_KEYWORD_INDEX.CHAR_GENDER = 21;
IF_KEYWORD_INDEX.CHAR_CLASS = 22;
IF_KEYWORD_INDEX.CHAR_ALIGNMENT = 23;
IF_KEYWORD_INDEX.CHAR_STATUS = 24;
IF_KEYWORD_INDEX.CHAR_UNDEAD = 25;
IF_KEYWORD_INDEX.CHAR_SIZE = 26;
IF_KEYWORD_INDEX.CHAR_MAGICRESIST = 27;
IF_KEYWORD_INDEX.CHAR_NBRHITDICE = 28;
IF_KEYWORD_INDEX.CHAR_NBRATTACKS = 29;
IF_KEYWORD_INDEX.CHAR_MORALE = 30;
IF_KEYWORD_INDEX.CHAR_ALLOWPLAYERCONTROL = 31;
IF_KEYWORD_INDEX.CHAR_DETECTINVISIBLE = 32;
IF_KEYWORD_INDEX.CHAR_DETECTTRAPS = 33;
IF_KEYWORD_INDEX.CHAR_DAMAGEBONUS = 34;
IF_KEYWORD_INDEX.CHAR_CHARTYPE = 35;
IF_KEYWORD_INDEX.CHAR_COINAMOUNT1 = 36;
IF_KEYWORD_INDEX.CHAR_COINAMOUNT2 = 37;
IF_KEYWORD_INDEX.CHAR_COINAMOUNT3 = 38;
IF_KEYWORD_INDEX.CHAR_COINAMOUNT4 = 39;
IF_KEYWORD_INDEX.CHAR_COINAMOUNT5 = 40;
IF_KEYWORD_INDEX.CHAR_COINAMOUNT6 = 41;
IF_KEYWORD_INDEX.CHAR_COINAMOUNT7 = 42;
IF_KEYWORD_INDEX.CHAR_COINAMOUNT8 = 43;
IF_KEYWORD_INDEX.CHAR_COINAMOUNT9 = 44;
IF_KEYWORD_INDEX.CHAR_COINAMOUNT10 = 45;
IF_KEYWORD_INDEX.CHAR_GEMAMOUNT = 46;
IF_KEYWORD_INDEX.CHAR_JEWELRYAMOUNT = 47;
IF_KEYWORD_INDEX.CHAR_ISMAMMAL = 48;
IF_KEYWORD_INDEX.CHAR_ISANIMAL = 49;
IF_KEYWORD_INDEX.CHAR_ISSNAKE = 50;
IF_KEYWORD_INDEX.CHAR_ISGIANT = 51;
IF_KEYWORD_INDEX.CHAR_ISALWAYSLARGE = 52;
IF_KEYWORD_INDEX.CHAR_HASDWARFACPENALTY = 53;
IF_KEYWORD_INDEX.CHAR_HASGNOMEACPENALTY = 54;
IF_KEYWORD_INDEX.CHAR_HASDWARFTHAC0PENALTY = 55;
IF_KEYWORD_INDEX.CHAR_HASGNOMETHAC0PENALTY = 56;
IF_KEYWORD_INDEX.CHAR_HASRANGERDMGPENALTY = 57;
IF_KEYWORD_INDEX.CHAR_HASPOISONIMMUNITY = 58;
IF_KEYWORD_INDEX.CHAR_HASDEATHIMMUNITY = 59;
IF_KEYWORD_INDEX.CHAR_HASCONFUSIONIMMUNITY = 60;
IF_KEYWORD_INDEX.CHAR_HASVORPALIMMUNITY = 61;
IF_KEYWORD_INDEX.CHAR_CANBEHELDORCHARMED = 62;
IF_KEYWORD_INDEX.CHAR_AFFECTEDBYDISPELEVIL = 63;
IF_KEYWORD_INDEX.CHAR_HITBONUS = 64;
IF_KEYWORD_INDEX.CHAR_MIRRORIMAGE = 65;

    /* party values */
IF_KEYWORD_INDEX.PARTY_BASE = 512;
IF_KEYWORD_INDEX.PARTY_FACING = 513;
IF_KEYWORD_INDEX.PARTY_DAYS = 514;
IF_KEYWORD_INDEX.PARTY_HOURS = 515;
IF_KEYWORD_INDEX.PARTY_MINUTES = 516;
IF_KEYWORD_INDEX.PARTY_ACTIVECHAR = 517;
IF_KEYWORD_INDEX.PARTY_MONEYAVAILABLE = 518;
IF_KEYWORD_INDEX.PARTY_TIME = 519;
IF_KEYWORD_INDEX.PARTY_LOCATION = 520;

    /* game values */
IF_KEYWORD_INDEX.GAME_BASE = 1024;
IF_KEYWORD_INDEX.GAME_CURRLEVEL = 1025;
IF_KEYWORD_INDEX.GAME_VERSION = 1026;
IF_KEYWORD_INDEX.GAME_COINNAME1 = 1027;
IF_KEYWORD_INDEX.GAME_COINNAME2 = 1028;
IF_KEYWORD_INDEX.GAME_COINNAME3 = 1029;
IF_KEYWORD_INDEX.GAME_COINNAME4 = 1030;
IF_KEYWORD_INDEX.GAME_COINNAME5 = 1031;
IF_KEYWORD_INDEX.GAME_COINNAME6 = 1032;
IF_KEYWORD_INDEX.GAME_COINNAME7 = 1033;
IF_KEYWORD_INDEX.GAME_COINNAME8 = 1034;
IF_KEYWORD_INDEX.GAME_COINNAME9 = 1035;
IF_KEYWORD_INDEX.GAME_COINNAME10 = 1036;
IF_KEYWORD_INDEX.GAME_GEMNAME = 1037;
IF_KEYWORD_INDEX.GAME_JEWELRYNAME = 1038;
IF_KEYWORD_INDEX.GAME_VAULTMONEYAVAILABLE = 1039;

var DUMP_CHAR_SPELL_ATTS = true;

var _VERSION_0500_ = 0.500;
var _VERSION_0505_ = 0.505;
//var _VERSION_0530_ = 0.530;
var _VERSION_0560_ = 0.560;
//var _VERSION_0562_ = 0.562;
var _VERSION_0563_ = 0.563;
var _VERSION_0564_ = 0.564;
//var _VERSION_0565_ = 0.565;
var _VERSION_0566_ = 0.566;
var _VERSION_05661_ = 0.5661;
var _VERSION_05662_ = 0.5662;
//var _VERSION_05663_ = 0.5663;
//var _VERSION_0567_ = 0.567;
//var _VERSION_05671_ = 0.5671;
//var _VERSION_0568_ = 0.568;
var _VERSION_0569_ = 0.569;
var _VERSION_05691_ = 0.5691
//var _VERSION_0570_ = 0.570;
//var _VERSION_0571_ = 0.571;
var _VERSION_0572_ = 0.572;
var _VERSION_0573_ = 0.573;
//var _VERSION_05731_ = 0.5731;
var _VERSION_0574_ = 0.574;
var _VERSION_0575_ = 0.575;
//var _VERSION_05751_ = 0.5751;
var _VERSION_0576_ = 0.576;
//var _VERSION_0577_ = 0.577;
var _VERSION_05771_ = 0.5771;
var _VERSION_0600_ = 0.6000;
//var _VERSION_0610_ = 0.6100;
var _VERSION_0620_ = 0.6200;
var _VERSION_0630_ = 0.6300;
var _VERSION_0640_ = 0.6400;
//var _VERSION_0641_ = 0.6410;
var _VERSION_0642_ = 0.6420;
//var _VERSION_0650_ = 0.6500;
//var _VERSION_0651_ = 0.6510;
//var _VERSION_0653_ = 0.6530;
//var _VERSION_0654_ = 0.6540;
//var _VERSION_0655_ = 0.6550;
//var _VERSION_0656_ = 0.6560;
var _VERSION_0660_ = 0.6600;
var _VERSION_0661_ = 0.6610;
var _VERSION_0662_ = 0.6620;
var _VERSION_0670_ = 0.6700;
//var _VERSION_0671_ = 0.6710;
//var _VERSION_0672_ = 0.6720;
//var _VERSION_0680_ = 0.6800;
var _VERSION_0681_ = 0.6810;
var _VERSION_0682_ = 0.6820;
var _VERSION_0690_ = 0.6900;
//var _VERSION_0691_ = 0.6910;
var _VERSION_0692_ = 0.6920;
var _VERSION_0693_ = 0.6930;
var _VERSION_0694_ = 0.6940;
//var _VERSION_06941_ = 0.6941;
var _VERSION_0695_ = 0.6950;
var _VERSION_0696_ = 0.6960;
//var _VERSION_06961_ = 0.6961;
var _VERSION_0697_ = 0.6970;
//var _VERSION_06971_ = 0.6971;
var _VERSION_0698_ = 0.6980;
var _VERSION_0699_ = 0.6990;
var _VERSION_06991_ = 0.6991;
var _VERSION_06992_ = 0.6992;
//var _VERSION_0700_ = 0.7000;
var _VERSION_0710_ = 0.7100;
var _VERSION_0720_ = 0.7200;
var _VERSION_0730_ = 0.7300;
var _VERSION_0731_ = 0.7310;
var _VERSION_0740_ = 0.7400;
var _VERSION_0750_ = 0.7500;
//var _VERSION_0760_ = 0.7600;
var _VERSION_0770_ = 0.7700;
//var _VERSION_0771_ = 0.7710;
var _VERSION_0780_ = 0.7800;
var _VERSION_0790_ = 0.7900;
//var _VERSION_0791_ = 0.7910;
//var _VERSION_0792_ = 0.7920;
var _VERSION_0800_ = 0.8000;
var _VERSION_0810_ = 0.8100;
var _VERSION_0820_ = 0.8200;
var _VERSION_0830_ = 0.8300;
//var _VERSION_0831_ = 0.8310;
var _VERSION_0840_ = 0.8400;
var _VERSION_0841_ = 0.8410;
var _VERSION_0842_ = 0.8420;
var _VERSION_0850_ = 0.8500;
var _VERSION_0851_ = 0.8510;
var _VERSION_0852_ = 0.8520;
var _VERSION_0860_ = 0.8600;
var _VERSION_0870_ = 0.8700;
//var _VERSION_0871_ = 0.8710;
var _VERSION_0880_ = 0.8800;
var _VERSION_0881_ = 0.8810;
var _VERSION_0882_ = 0.8820;
var _VERSION_0890_ = 0.8900;
//var _VERSION_0891_ = 0.8910;
//var _VERSION_0892_ = 0.8920;
var _VERSION_0900_ = 0.9000;
//var _VERSION_0902_ = 0.9020;
var _VERSION_0904_ = 0.9040;
var _VERSION_0906_ = 0.9060;
//var _VERSION_0907_ = 0.9070;
var _VERSION_0908_ = 0.9080;
var _VERSION_0909_ = 0.9090;
//var _VERSION_09091_ = 0.9091;
var _VERSION_0910_ = 0.910;
var _VERSION_0911_ = 0.911;
var _VERSION_0912_ = 0.912;
var _VERSION_0913_ = 0.913;
var _VERSION_0914_ = 0.914;
var _VERSION_0914_003 = 0.914003;
var _VERSION_0914_004 = 0.914004;
var _VERSION_0914_005 = 0.914005;
var _VERSION_0914_006 = 0.914006;
var _VERSION_0914_007 = 0.914007;
var _VERSION_0915_ = 0.915;
var _VERSION_0915_015 = 0.915015;
var _VERSION_0915_017 = 0.915017;
var _VERSION_0915_041 = 0.915041;
var _VERSION_0915_045 = 0.915045;
var _VERSION_0916 = 0.916;
var _VERSION_0920_000 = 0.920000;
var _VERSION_0930 = 0.930;

var _ASL_LEVEL_                   = _VERSION_0505_;
var _EXTENDED_ASL_LEVEL_          = _VERSION_0560_;  // Include flags at this version
var _ITEM_DB_VERSION_             = _VERSION_0560_;
var _SPELL_DB_VERSION_            = _VERSION_0560_;
var _MONSTER_DB_VERSION_          = _VERSION_0500_;
var _SPECIAL_ABILITIES_VERSION_   = _VERSION_0930;
var _CELL_CONTENTS_VERSION        = 5.0;