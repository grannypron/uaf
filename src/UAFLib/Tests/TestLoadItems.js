var loader = new UAFLib.dataLoaders.ItemLoader();
var data = loader.load("C:\\Users\\Shadow\\Desktop\\uaf.git\\uaf-port\\src\\UAFLib\\data\\items.xml");

itemData.LoadFromLoader(data);

Globals.ASSERT(itemData.GetCount() == 540, "TestLoadItems.js");
Globals.ASSERT(itemData.PeekItem("Long Sword") != null && itemData.PeekItem("Long Sword").m_uniqueName != null, "TestLoadItems.js");
Globals.ASSERT(itemData.PeekItem("Long Sword").Dmg_Dice_Sm == 8);