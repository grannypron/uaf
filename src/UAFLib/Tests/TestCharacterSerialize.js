var character = new CHARACTER();

// Serialize
/*var ar = new CAR(null, 6.0);
var log = System.Console.WriteLine;
character.SerializeCAR(ar, 6.0);*/

// De-serialize
//var path = "C:\\Users\\Shadow\\Desktop\\uaf.git\\uaf-port\\src\\UAFLib\\dude_noheader.chr";
var path = "C:\\Users\\Shadow\\Desktop\\uaf.git\\uaf-port\\src\\UAFLib\\Druid.chr";
var fs = System.IO.File.OpenRead(path);
var ar = new CAR(fs, 6.0);
var log = System.Console.WriteLine;
character.SerializeCAR(ar, 6.0);
fs.Close();


//var log = System.Console.WriteLine; log("here");
