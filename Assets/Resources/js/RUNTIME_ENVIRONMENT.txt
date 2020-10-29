/** TODO **/
function RUNTIME_ENVIRONMENT() {
    this.m_executableDir = "";  // Where the program executable is.
    this.m_rootDir = "";        // A folder containg designs and perhaps an editor folder
    this.m_designDir = "";      // A folder containing single design; usually "something.dsn"
    this.m_dataDir = "";        // The folder containint the levels, etc of the dungeon
    this.m_soundDir = "";       // The sounds folder of the design; usually "designDir\Resources"
    this.m_configDir = "";      // The folder in which we will find 'config.txt
    this.m_saveDir = "";        // The folder in which we store party and character data.
    this.m_artDir = "";         // The art folder of the design; usually "designDir\Resources"
    this.m_backupDir = "";
    this.m_programFilesChecked = false;
}

RUNTIME_ENVIRONMENT.prototype.IconArtDir = function () { return this.m_artDir; }
RUNTIME_ENVIRONMENT.prototype.PicArtDir = function () { return this.m_artDir; }
RUNTIME_ENVIRONMENT.prototype.RootDir = function ()           { return this.m_rootDir; };
RUNTIME_ENVIRONMENT.prototype.DataDir = function ()           { return this.m_dataDir; };
RUNTIME_ENVIRONMENT.prototype.DesignDir = function ()         { return this.m_designDir; };
RUNTIME_ENVIRONMENT.prototype.MovieDir = function ()          { return this.m_artDir; };
RUNTIME_ENVIRONMENT.prototype.WindowArtDir = function ()      { return this.m_artDir; };
RUNTIME_ENVIRONMENT.prototype.CursorArtDir = function ()      { return this.m_artDir; };
RUNTIME_ENVIRONMENT.prototype.CombatArtDir = function ()      { return this.m_artDir; };
RUNTIME_ENVIRONMENT.prototype.MapArtDir = function ()         { return this.m_artDir; };
RUNTIME_ENVIRONMENT.prototype.IconArtDir = function ()        { return this.m_artDir; };
RUNTIME_ENVIRONMENT.prototype.BackgroundArtDir = function ()  { return this.m_artDir; };
RUNTIME_ENVIRONMENT.prototype.TitleArtDir = function ()       { return this.m_artDir; };
RUNTIME_ENVIRONMENT.prototype.SoundDir = function ()          { return this.m_soundDir; };
RUNTIME_ENVIRONMENT.prototype.AreaViewArtDir = function ()    { return this.m_artDir; };
RUNTIME_ENVIRONMENT.prototype.ExecutableDir = function ()     { return this.m_designDir; };
RUNTIME_ENVIRONMENT.prototype.SaveDir = function ()           { return this.m_saveDir; };
RUNTIME_ENVIRONMENT.prototype.SmallPicDir = function ()       { return this.m_artDir; };
RUNTIME_ENVIRONMENT.prototype.ScreenCapDir = function ()      { return this.m_designDir; };
RUNTIME_ENVIRONMENT.prototype.WallArtDir = function ()        { return this.m_artDir; };
//RUNTIME_ENVIRONMENT.prototype.LogDir = function ();  **TODO**
RUNTIME_ENVIRONMENT.prototype.DoorArtDir = function ()        { return this.m_artDir; };
RUNTIME_ENVIRONMENT.prototype.OverlayArtDir = function ()     { return this.m_artDir; };
RUNTIME_ENVIRONMENT.prototype.SpellArtDir = function ()       { return this.m_artDir; };
RUNTIME_ENVIRONMENT.prototype.ConfigDir = function ()         { return this.m_dataDir; };
RUNTIME_ENVIRONMENT.prototype.PicArtDir = function ()         { return this.m_artDir; };
RUNTIME_ENVIRONMENT.prototype.CreditsArtDir = function ()     { return this.m_artDir; };
RUNTIME_ENVIRONMENT.prototype.BackupDir = function ()         { return this.m_backupDir; };
RUNTIME_ENVIRONMENT.prototype.OverlandArtDir = function ()    { return this.m_artDir; };
RUNTIME_ENVIRONMENT.prototype.SplashArtDir = function ()      { return this.m_artDir; };
RUNTIME_ENVIRONMENT.prototype.SpriteArtDir = function ()      { return this.m_artDir; };

//void DesignDir(const CString& s) { m_designDir = s; };  **TODO**
var RUNTIME_ENVIRONMENT = new RUNTIME_ENVIRONMENT();
