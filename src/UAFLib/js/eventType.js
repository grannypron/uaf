function eventType() { 
   // Status:
   //        Dlg: Editor dialog has been created to allow user to configure this event
   //       Game: Engine has the code to process this event
   //   ChainIds: Indicates an event which stores references to other events
   //             using event keys, not including the chainEventHappen/chainEventNotHappen
   //             events which all events have. These events need special handling
   //             for situations such as event deletion, since the referenced events
   //             must also be deleted.
   //      
   //             Events with ChainIds must over-ride the following virtual functions
   //             and provide proper handling for each chain event.
   //
   //               IsChained
   //               ChainToEventId
   //               UnchainToEventId
   //               GetEVChainText
   //               GetEVChain
   //               GetChainCount
   //
   //             Special handling must also be added to these functions which
   //             are shared by all events.
   //
   //               DeleteEvent
   //               CopyEvent
   //
   //             And of course each chain event must be processed in 
   //             functions such as Serialize and operator=.
   //
   //             The list of events with ChainIds currently includes:
   //
   //               ChainEventType,
   //               EncounterEvent,
   //               EnterPassword,
   //               QuestStage,
   //               QuestionButton,
   //               QuestionList,
   //               QuestionYesNo,
   //               SmallTown,
   //               TavernEvent,
   //               TempleEvent,
   //               WhoPays,
   //               WhoTries,
   //               LogicBlock,
   //               RandomEvent
   //
                           // Dlg? Game? ChainIds
   NoEvent = 0;            //  -     -     -
   AddNpc = 1;             //  x     x     -
   Camp = 2;               //  x     x     -
   ChainEventType = 3;     //  x     x     x
   Combat = 4;             //  x     x     -
   CombatTreasure = 5;     //  x     x     - <--- CombatTreasure=GiveTreasure
   Damage = 6;             //  x     x     -
   EncounterEvent = 7;     //  x     x     x
   EnterPassword = 8;      //  x     x     x
   GainExperience = 9;     //  x     x     -
   GiveTreasure = 10;      //  x     x     - <--- CombatTreasure=GiveTreasure
   GuidedTour = 11;        //  x     x     -
   InnEvent = 12;          //  -     -     - <--- obsoleted = ; WhoPays+CampEvent
   NPCSays = 13;           //  x     x     -
   PassTime = 14;          //  x     x     -
   PickOneCombat = 15;     //  -     -     - <--- obsoleted = ; use option in Combat event
   QuestStage = 16;        //  x     x     x
   QuestionButton = 17;    //  x     x     x
   QuestionList = 18;      //  x     x     x
   QuestionYesNo = 19;     //  x     x     x
   RemoveNPCEvent = 20;    //  x     x     - 
   ShopEvent = 21;         //  x     x     -
   SmallTown = 22;         //  x     x     x
   Sounds = 23;            //  x     x     -
   SpecialItem = 24;       //  x     x     -
   Stairs = 25;            //  x     x     - <--- Stairs=Teleporter=TransferModule
   TavernEvent = 26;       //  x     x     x
   TavernTales = 27;       //  -     -     - <--- obsoleted = ; included in TavernEvent
   Teleporter = 28;        //  x     x     - <--- Stairs=Teleporter=TransferModule
   TempleEvent = 29;       //  x     x     x
   TextStatement = 30;     //  x     x     - 
   TrainingHallEvent = 31; //  x     x     -
   TransferModule = 32;    //  x     x     - <--- Stairs=Teleporter=TransferModule
   Utilities = 33;         //  x     x     -
   Vault = 34;             //  x     x     -
   WhoPays = 35;           //  x     x     x
   WhoTries = 36;          //  x     x     x
   TakePartyItems = 37;    //  x     x     -
   HealParty = 38;         //  x     x     -
   LogicBlock = 39;        //  x     x     x
   GPDLEvent = 40;         //        x     -
   RandomEvent = 41;       //  x     x     x
   PlayMovieEvent = 42;    //  x     x     -
   JournalEvent = 43;      //  x     x     -
   FlowControl = 44;       //  x     x     -

   // The rest are runtime-generated 'fake' events for control purposes
   // These values are in save files and must not change.
   
   this.CONTROL_Splash=1000; // Must remain as first CONTROL_ event
   this.CONTROL_DoNothing=1001;
   this.CONTROL_Exit=1002;
   this.CONTROL_Start=1003;
   this.CONTROL_Main=1004;
   this.CONTROL_CreateCharacter=1005;
   this.CONTROL_Race=1006;
   this.CONTROL_Gender=1007;
   this.CONTROL_Class=1008;
   this.CONTROL_Alignment=1009;
   this.CONTROL_ChooseStats=1010;
   this.CONTROL_GetCharName=1011;
   this.CONTROL_GetCharIcon=1012;
   this.CONTROL_GetCharSmallPic=1013;
   this.CONTROL_AddCharacter=1014;
   this.CONTROL_Adventure=1015;
   this.CONTROL_Move=1016;
   this.CONTROL_UNUSED=1017;
   this.CONTROL_Save=1018;
   this.CONTROL_Load=1019;
   this.CONTROL_BeginXYEvent=1020;
   this.CONTROL_BeginStepEvent=1021;
   this.CONTROL_PlayStepsound=1022;
   this.CONTROL_TakeTreasure=1023;
   this.CONTROL_ViewCharacter=1024;
   this.CONTROL_ItemsMenu=1025;
   this.CONTROL_GetMoneyQty=1026;
   this.CONTROL_Encamp=1027;
   this.CONTROL_PassingTime=1028;
   this.CONTROL_BuyItems=1029;
   this.CONTROL_TrainCharacters=1030;
   this.CONTROL_BeginRestEvent=1031;
   this.CONTROL_BeginZoneEvent=1032;
   this.CONTROL_CombatAimAuto=1033;
   this.CONTROL_CombatAimManual=1034;
   this.CONTROL_CombatMove=1035;
   this.CONTROL_CombatSpriteAnimate=1036;
   this.CONTROL_TimeDelayMsg=1037;
   this.CONTROL_CombatRoundDelay=1038;
   this.CONTROL_GameSpeed=1039;
   this.CONTROL_HumanChangeClass=1040;
   this.CONTROL_TradeTakerSelect=1041;
   this.CONTROL_AlterGame=1042;
   this.CONTROL_AlterGameLevel=1043;
   this.CONTROL_AlterPartyOrder=1044;
   this.CONTROL_PartyDied=1045;
   this.CONTROL_CombatResults=1046;
   this.CONTROL_MagicMenu=1047;
   this.CONTROL_CastMenu=1048;
   this.CONTROL_CastTargetMenu=1049;
   this.CONTROL_MemorizeMenu=1050;
   this.CONTROL_MemorizeVerifyMenu=1051;
   this.CONTROL_ScribeMenu=1052;
   this.CONTROL_DisplaySpellAffectsMenu=1053;
   this.CONTROL_RestTimeMenu=1054;
   this.CONTROL_AppraiseSelect=1055;
   this.CONTROL_AppraiseEvaluate=1056;
   this.CONTROL_AskYesNo=1057;
   this.CONTROL_DesignSelect=1058;
   this.CONTROL_TargetSelectNonCombat=1059;
   this.CONTROL_NewMuSpellLevelMenu=1069;
   this.CONTROL_BashDoorMenu=1070;
   this.CONTROL_AlterGameSound=1071;
   this.CONTROL_CastNonCombatSpell=1072;
   this.CONTROL_DisplayPartyJournal=1073;
   this.CONTROL_CastCombatSpell=1074;
   this.CONTROL_TargetSelectCombat=1075;
   this.CONTROL_CombatSpellSpriteAnimate=1076;
   this.CONTROL_CombatSpellAimAuto=1077;
   this.CONTROL_CombatSpellAimManual=1078;
   this.CONTROL_CombatUtilityDelay=1079;
   this.CONTROL_InitialMUSpellsMenu=1080;
   this.CONTROL_LevelUpLearnSpellsMenu=1081;
   this.CONTROL_AskCustom = 1082;
   this.CONTROL_BeginTimeEvent = 1083;
   this.CONTROL_ScriptChoice = 1084;
   this.CONTROL_CombatSpecialAction = 1085;
   this.CONTROL_ChooseFromList = 1086;

   this.NUM_EVENT_TYPES = 45; // doesn't include CONTROL_* types
};



function eventTypeObj() {
    entityType.call(this, eventType);
    Object.setPrototypeOf(this, entityType.prototype);
}
