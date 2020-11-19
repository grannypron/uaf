function Forth() {
    this.m = 
        " CREATE" +
        " +p" +
        //  The first two primitives are compiled by ExpandKernel()
        //  The rest are defined by INTERPRET in the normal manner.
        " CREATE ' +p" +
        " CREATE LIT +p" +
        " CREATE @ +p" +
        " CREATE NOP +p" +  // Can be used to insert debug break.
        " CREATE SETIMMEDIATE +p" +
        " CREATE [ +p " + sm_LATEST +  " @ SETIMMEDIATE" +
        " CREATE ] +p" +
        " CREATE 0< +p" +
        " CREATE C@ +p" +
        " CREATE C! +p" +
        " CREATE +! +p" +
        " CREATE ! +p" +
        " CREATE 1- +p" +
        " CREATE DUP +p" +
        " CREATE ROT +p" +
        " CREATE EXIT +p" +
        " CREATE ?EXIT +p" +  // (false ..) ; (true .. true) EXIT
        " CREATE SP+- +p" +
        " CREATE docolon +p" +
        sm_LATEST  + " @ 1- C@" +
        " ]  LIT  [" + t_comma +
        " ]"  + sm_LATEST + " @ 1- C! EXIT [" +
        " CREATE -->OP docolon ] 1- EXIT [" +
        " CREATE OP! docolon ] -->OP C! EXIT [ -2 SP+-" +
        " CREATE PRIM docolon ] CREATE +p EXIT [" +
        " PRIM BRANCH" +
        " PRIM ?BRANCH" +
        " PRIM OVER" +
        " PRIM HEAD" +
        " PRIM WORD" +
        " PRIM FIND" +
        " PRIM SWAP" +
        " PRIM >R" +
        " PRIM R>" +
        " PRIM OR" +
        " PRIM XOR" +
        " PRIM AND" +
        " PRIM ABS" +
        " PRIM DROP" +
        " PRIM +" +
        " PRIM -" +
        " PRIM NEGATE" +
        " PRIM DNEGATE" +
        " PRIM UM*" +
        " PRIM M*" +
        " PRIM =" +
        " PRIM NOT" +
        " PRIM !=" +
        " PRIM <" +
        " PRIM >" +
        " PRIM ?DUP" +
        " PRIM MU/MOD" +
        " PRIM DEBUG" +
        " PRIM EXECUTE" +
        " CREATE , docolon ]"  + sm_DP + " DUP @ 2 ROT +! !  EXIT [ -1 SP+-" +
        " PRIM docon"  + sm_LATEST + " @ -->OP C@ docolon" +  // Fetch and replace the docon primitive index
        " ] LIT [ , ]"  + sm_LATEST + " @ OP! EXIT [" +
        " CREATE CONSTANT docolon ] CREATE docon , EXIT [ -1 SP+-" +
        " H'20'      CONSTANT BL" +
        sm_2IN    + " CONSTANT >IN" +
        sm_LATEST + " CONSTANT LATEST" +
        sm_DP     + " CONSTANT DP" +
        sm_STATE  + " CONSTANT  STATE" +
        " CREATE HIDE   docolon ] H'04' LATEST @ 2 - DUP >R C@ OR  R> C! EXIT [" +
        " CREATE REVEAL docolon ] H'FB' LATEST @ 2 - DUP >R C@ AND R> C! EXIT [" + 
        " CREATE : docolon ] CREATE  docolon HIDE ] EXIT [" + 
        //_:;     docolon LATEST @ REVEAL LIT EXIT ,XT [ EXIT _IMMEDIATE 
        " : IMMEDIATE LATEST @ SETIMMEDIATE EXIT [" + 
        " : ; REVEAL [ ' [ , ] LIT EXIT , EXIT [ IMMEDIATE" + 
        " : 2DUP OVER OVER ;" + 
        " : ['] LIT LIT , ' , EXIT [ IMMEDIATE";

    this.MAX_MEM = 10000;
    this.SP = 0;
    this.RP = 0;
    this.SP0 = MAX_MEM - 1000;
    this.RP0 = MAX_MEM;
}

var finished = false;
var success = true;
Forth.prototype.ExpandKernel = function () {
    var len = 0;
    if (finished) return success;
    finished = true;
    len = m.length;
    this.SP = this.SP0;
    this.RP = this.RP0;
/**  This is where it starts to get nasty.  I think I am going to skip this and turn off LoadAI_Script
    // Move the definitions to the end of mem
    memmove(this.m + this.SP - len - 100, m, len + 1);
    memset(this.m, 0, this.SP - len - 100);
    iMEM(m_DP) = DP0;
    iMEM(m_tickSOURCE) = SP - len - 100;
    iMEM(m_tickSOURCE + 2) = len;
    iMEM(m_LATEST) = 0;
    iMEM(m_2IN) = 0;
    nextPrim = 1;
    Kf_CREATE(); Kf_plusP();
    Kf_CREATE(); Kf_plusP();
    PUSHSP(iMEM(m_tickSOURCE) += iMEM(m_2IN));
    PUSHSP(iMEM(m_tickSOURCE + 2) -= iMEM(m_2IN));
    iMEM(m_STATE) = 0;
    iMEM(m_BASE) = 10;
    Exit = 0;
    Kf_INTERPRET();
    if (Exit) {
        MsgBoxInfo("AI_Script Error.  See log for details", "Warning");
        Kf_PrintSource();
        return false;
    };
    {
        FILE * f;
        CString fname;
        fname.Format("%sAI_Script.BLK", rte.DataDir());
        f = fopen(fname, "r");
        if (f != NULL) {
            bool printVersion = true;
            {
                //MsgBoxInfo("An AI Script exists!","Warning");
                //debugSeverity = 7;
                //WriteDebugString("An AI Script exists in the data folder\n");
            };
            while (fgets(m + SP - 200, 120, f) != NULL) {
                char * str = m + SP - 200;
                while ((str = strchr(str, 10)) != NULL) {
            * str = ' ';
                };
                if (printVersion) {
                    printVersion = false;
                    WriteDebugString("AI_Script first line = %s\n", m + SP - 200);
                };
                iMEM(m_tickSOURCE) = SP - 200;
                iMEM(m_tickSOURCE + 2) = (i16)strlen(m + SP - 200);
                PUSHSP(iMEM(m_tickSOURCE));
                PUSHSP(iMEM(m_tickSOURCE + 2));
                Kf_INTERPRET();
                if (Exit) {
                    MsgBoxInfo("AI_Script Error.  See log for details", "Warning");
                    Kf_PrintSource();
                    success = false;
                    break;;
                };
            };
            fclose(f);
        }
        else {
            Globals.die("AI_Script.BLK file does not exist in data folder");
        };
    };
    return success;

*/
}

Forth.prototype.FetchKernel = function () {
    Globals.NotImplemented("Forth.prototype.FetchKernel");
}