var dice = UAFUtil.parseDice("1d6 + 0");
Globals.ASSERT(dice.die == 6 && dice.nbr == 1 && dice.bonus == 0, "TestParseDice.js");

dice = UAFUtil.parseDice("2d8");
Globals.ASSERT(dice.die == 8 && dice.nbr == 2 && dice.bonus == 0, "TestParseDice.js");

dice = UAFUtil.parseDice("2d12 -4");
Globals.ASSERT(dice.die == 12 && dice.nbr == 2 && dice.bonus == -4, "TestParseDice.js");

dice = UAFUtil.parseDice("2d12-3");
Globals.ASSERT(dice.die == 12 && dice.nbr == 2 && dice.bonus == -3, "TestParseDice.js");