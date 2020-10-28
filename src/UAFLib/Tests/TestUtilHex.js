var log = System.Console.WriteLine;
Globals.ASSERT(UAFUtil.ByteFromHexString("0x010") == 16);
Globals.ASSERT(UAFUtil.ByteFromHexString("0x80000000") == -2147483648);
Globals.ASSERT(UAFUtil.ByteFromHexString("0x80000001") == -2147483647);
