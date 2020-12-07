var tagList = new TAG_LIST(100);

tagList.Set(10);

Globals.ASSERT(tagList.FindOpenTag() == 0, "TestTagList.js - 1");

for (var idxTag = 0; idxTag < 11; idxTag++) {
    tagList.Set(idxTag);
}

Globals.debug(tagList.FindOpenTag());
Globals.ASSERT(tagList.FindOpenTag() == 11, "TestTagList.js - 2");


tagList.ClearItem(5);
Globals.ASSERT(tagList.FindOpenTag() == 5, "TestTagList.js - 3");
