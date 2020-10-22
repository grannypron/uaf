var log = System.Console.WriteLine;
globalData.SetMaxPCs(10);
globalData.SetMinPCs(2);
globalData.SetMaxPartySize(12);

Globals.ASSERT(globalData.GetMaxPCs() == 10, "TestGlobalStats.js");
Globals.ASSERT(globalData.GetMinPCs() == 2, "TestGlobalStats.js");
Globals.ASSERT(globalData.GetMaxPartySize() == 12, "TestGlobalStats.js");

