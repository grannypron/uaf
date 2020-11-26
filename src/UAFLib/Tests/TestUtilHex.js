var log = System.Console.WriteLine;
Globals.debug(UAFUtil.ByteFromHexString("0x010"));
Globals.debug(UAFUtil.ByteFromHexString("0x80000000"));
Globals.ASSERT(UAFUtil.ByteFromHexString("0x010") == 16, "TestUtilHex.js");
Globals.ASSERT(UAFUtil.ByteFromHexString("0x80000000") == -2147483648, "TestUtilHex.js");
Globals.ASSERT(UAFUtil.ByteFromHexString("0x80000001") == -2147483647, "TestUtilHex.js");
