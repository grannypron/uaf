var log = System.Console.WriteLine;
Globals.ASSERT(Globals.StripFilenamePath("C:\\path\\to\\file.txt") == "file.txt");
Globals.ASSERT(Globals.StripFilenamePath("C:\\path\\to\\file.txt\\") == "file.txt");
log("Complete");
