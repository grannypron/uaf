var log = System.Console.WriteLine;
Globals.ASSERT(Globals.StripFilenamePath("C:\\path\\to\\file.txt") == "file.txt", "TestStripFilenamePath.js");
Globals.ASSERT(Globals.StripFilenamePath("C:\\path\\to\\file.txt\\") == "file.txt", "TestStripFilenamePath.js");
log("Complete");
