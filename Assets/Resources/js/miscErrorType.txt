function miscErrorType() { 
    this.NoError = 0;
    this.UnknownError = 1;
    this.DeadChar = 2;
    this.UnconsciousChar = 3;
    this.FledChar = 4;
    this.PetrifiedChar = 5;
    this.GoneChar = 6;
    this.AnimatedChar = 7;
    this.TempGoneChar = 8;
    this.RunningChar = 9;
    this.DyingChar = 10;
    this.NoMoney = 11;
    this.NotEnoughMoney = 12;
    this.MaxItemsReached = 13;
    this.TooMuchWeight = 14;
    this.TakesTwoHands = 15;
    this.WrongClass = 16;
    this.ItemAlreadyReadied = 17;
    this.NoIcons = 18;
    this.NoSmallPics = 19;
    this.NoDesigns = 20;
    this.LevelLoadError = 21;
    this.InvalidGameState = 22;
    this.InvalidEventType = 23;
    this.InvalidMenuState = 24;
    this.InvalidTaskState = 25;
    this.PartyIsFull = 26;
    this.NPCPartyLimitReached = 27;
    this.ItemIsReadied = 28;
    this.MissingMonsterCombatIcons = 29;
    this.GraphicEngineError = 30;
    this.DesignVersionError = 31;
    this.SaveGameVersionError = 32;
    this.SaveCharVersionError = 33;
    this.FileLoadError = 34;
    this.FileSaveError = 35;
    this.DupCharName = 36;
    this.NoFreeHands = 37;
    this.CharacterCreationError = 38;
    this.ErrorText = 39;
    this.LastMiscError = 40;

};

function miscErrorTypeObj() {
    miscErrorType.call(this);
    Object.setPrototypeOf(this, entityType.prototype);
}

var MAX_MISC_ERROR_MSGS = new miscErrorType().LastMiscError + 1;