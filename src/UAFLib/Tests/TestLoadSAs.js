/*
function Deserialize(filename, debug) {
    var character = new CHARACTER();
    var path = "C:\\Users\\Shadow\\Desktop\\uaf.git\\uaf-port\\src\\UAFLib\\Tests\\" + filename;
    var fs = System.IO.File.OpenRead(path);
    var ar = new CAR(fs, 6.0);

    //Read the first 16 header bytes because this isn't covered in the CHARACTER's serialize method
    ar.readInt(); ar.readInt(); ar.readInt(); ar.readInt();

    if (debug) {
        ar.MFCSerializer.debug = true;
    }
    character.SerializeCAR(ar, 6.0);
    fs.Close();
    return character;
}

SPECAB.loadData(specialAbilitiesData, "C:\\Users\\Shadow\\Downloads\\Full_Release_191031\\TutorialDesign.dsn\\Data\\specialAbilities.dat");

*/


var loader = new UAFLib.SpecabilityLoader();
var data = loader.load("C:\\Users\\Shadow\\Desktop\\uaf.git\\uaf-port\\src\\UAFLib\\SpecialAbilities.xml");

var enumerator = data.GetEnumerator();
while (enumerator.MoveNext()) {
    var key = enumerator.Current.Key;
    var value = enumerator.Current.Value;
    for (var idxAsl = 0; idxAsl < value.length; idxAsl++) {
//        try {
            specialAbilitiesData.InsertString(key, value[idxAsl][0], value[idxAsl][1], 2);
//        } catch (ex) {
//            Globals.debug("SA load failed on " + key);
//        }
    }
}

//specialAbilitiesData.InsertString()
Globals.ASSERT(data.Keys.length == 432, "data.Keys.length == 432");
Globals.ASSERT(specialAbilitiesData.FindAbility("Global_Display") != null, "specialAbilitiesData.FindAbility(\"Global_Display\") != null");