Globals.ASSERT(gpdlStack.activeGPDL() != null, "TestGPDL.js - 1");
Globals.ASSERT(gpdlStack.m_stack.length == gpdlStack.GPDL_STACKSIZE, "TestGPDL.js - 2");


var scripts = [];
scripts[0] = [256, 100663297, 393216, 1536, 6, 134217728, 524288, 2048, 8, 16777216, 65536, 256, 1, 16777216, 65536, 256, 100663297, -670695424, 14157312, 55302, 100663512, 403046400, 1574400];   // This the compiled version of '$RETURN "You ded";'


binaryCode = scripts[0];
Globals.debug(binaryCode[1] & UAFUtil.ByteFromHexString("0xffffff"));
Globals.debug(binaryCode[4]);
gpdlStack.activeGPDL().ExecuteScript(scripts[0], 1, null, 0);

Globals.debug("gpdlStack.activeGPDL().m_H:" + gpdlStack.activeGPDL().m_H);
Globals.debug("gpdlStack.activeGPDL().m_PC:" + gpdlStack.activeGPDL().m_PC);