var loader = new UAFLib.dataLoaders.ItemLoader();
var data = loader.load("C:\\Users\\Shadow\\Desktop\\uaf.git\\uaf-port\\src\\UAFLib\\data\\items.xml");

itemData.LoadFromLoader(data);

Globals.ASSERT(itemData.PeekItem("Long Sword").Wpn_Type == weaponClassType.HandCutting);
Globals.ASSERT(itemData.GetCount() == 540, "TestLoadItems.js - 1");
Globals.ASSERT(itemData.PeekItem("Long Sword") != null && itemData.PeekItem("Long Sword").m_uniqueName != null, "TestLoadItems.js - 2");
Globals.ASSERT(itemData.PeekItem("Long Sword").Dmg_Dice_Sm == 8, "TestLoadItems.js - 3");