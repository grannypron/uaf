function GPDL() {
    this.DBG_messageBox	   = 1;
    this.DBG_functionTrace = 2;

    this.m_program = null;
    this.m_globals = new GPDL_GLOBALS();
    this.m_index = new INDEX();
    this.m_PC = 0;
    this.m_FP = 0;
    this.m_SP = 0;
    this.m_RP = 0;
    this.m_RP0 = 1000;
    this.m_SP0 = 1000;
    this.m_H = 0;
    this.m_programAllocated = 0; // Allocated for program
    this.m_executionAddress = 0; // latest Opcode fetch address
    this.m_state = GPDL_STATE.GPDL_UNINITIALIZED;
    this.m_status = GPDL_STATE.GPDL_UNINITIALIZED;
    this.m_debugLevel = 0;
    this.m_interpretCount = 0;
    this.m_dataStack = null;
    this.m_returnStack = null;
    this.m_pGPDLevent = new GPDL_EVENT();
    this.m_errorLog = new ERRORLOG();
    this.m_false = "";
    this.m_listenText = this.m_false;
    this.m_true = "1";
    this.SetIntermediateResult(0.0);
    this.pTimedSA = null;
}

GPDL.prototype.ExecuteScript = function (code, entryPointOrdinal) {

    var RETURN_VAL = null;
    eval(code);
    return RETURN_VAL;   // This is set in the special ability code (or not)
    /*
    var status;
    var numEntry = 0;
    var binaryCode;
    binaryCode = code;
    if (this.m_program != null) Globals.My_free(this.m_program);
    numEntry = binaryCode[1] & UAFUtil.ByteFromHexString("0xffffff");
    this.m_H = binaryCode[4];
    if (entryPointOrdinal > numEntry) return m_false;
    if (entryPointOrdinal <= 0) return m_false;
    this.m_PC = binaryCode[12 + entryPointOrdinal - 1];
    if (this.m_PC == 0) return m_false;
    this.m_program = binaryCode[12 + (4 * numEntry)];
    if (this.m_dataStack == null)
        { this.m_dataStack = []; for (var idx = 0; idx < this.m_SP0; idx++) { this.m_dataStack[this.m_SP0] = ""; } }
    if (this.m_returnStack == null)
        { this.m_returnStack = []; for (var idx = 0; idx < this.m_RP0; idx++) { this.m_returnStack[this.m_RP0] = ""; } }
    this.m_pGPDLevent = null;
    this.m_RP = this.m_RP0;
    this.m_SP = this.m_SP0;
    this.m_pushRP(UAFUtil.ByteFromHexString("0xffffffff"));          // So we will know when we are done.
    this.m_pushRP(UAFUtil.ByteFromHexString("0xffffffff"));          // Old Frame Pointer
    this.m_string1 = this.m_false;
    this.m_pushSP(this.m_string1); // The empty string as default result.
    this.m_FP = this.m_SP;
    this.m_interpretCount = 0;
    this.status = this.m_interpret();
    this.m_program = null;
    if (this.status != GPDL_STATE.GPDL_IDLE) return this.m_false;
    this.m_popString1();
    return this.m_string1;
    */
}

GPDL.prototype.m_pushRP = function(n) {
    if (this.m_RP < 2) {
        if (this.m_interpStatus == GPDL_STATE.GPDL_OK) this.m_interpStatus = GPDL_STATE.GPDL_OVER_RP;
        return;
    }
    this.m_returnStack[--this.m_RP] = n;
}

GPDL.prototype.m_pushSP = function(val) {
    if (this.m_SP < 2) {
        if (this.m_interpStatus == GPDL_STATE.GPDL_OK) this.m_interpStatus = GPDL_STATE.GPDL_OVER_SP;
        return;
    };
    this.m_dataStack[--this.m_SP] = val;
}

GPDL.prototype.SetIntermediateResult = function (val) {
    this.m_IntermediateResult = val;
}



GPDL.prototype.m_interpret = function() {
    var i = 0;
    this.m_interpStatus = GPDL_STATE.GPDL_OK;

    while (this.m_interpStatus == GPDL_STATE.GPDL_OK) {
        this.m_interpretCount++;
        if (this.m_interpretCount > 1000000) {
            this.m_interpStatus = GPDL_STATE.GPDL_EXCESSCPU;
            return this.m_interpStatus;
        };
        this.m_executionAddress = this.m_PC; // for error printout
        // 19 Apr PRS m_bincode=m_code.peek(m_PC++);
        /*
        this.m_bincode = GPDL_GLOBALS.Peek(this.m_PC++);
        this.m_opcode = (this.m_bincode >> 24) & 0xff;
        this.m_subop = m_bincode & 0xffffff; // Also might be an address
        switch (this.m_opcode) {
            case BINOPS.BINOP_LOCALS:
                for (i = 0; i < m_subop; i++)
                    this.m_pushSP(m_false);
                break;
            case BINOPS.BINOP_JUMP:
                this.m_PC = m_subop;
                break;
            case BINOPS.BINOP_ReferenceGLOBAL:
                {
                    var index = 0;
                    index = this.m_subop;
                    if (index & 0x800000) {
                        this.m_globals.Set(index & 0x7fffff, this.m_popSP());
                    }
                    else {
                        this.m_pushSP(this.m_globals.peek(index));
                    };
                };
                break;
            case BINOPS.BINOP_FETCHTEXT:
                this.m_string1 = (""+this.m_program) + (this.m_bincode & 0xffffff);
                this.m_pushString1();
                break;
            case BINOPS.BINOP_CALL:
                this.m_pushPC();
                this.m_pushFP();
                this.m_FP = this.m_SP;
                this.m_PC = this.m_subop;
                if (this.m_debugLevel & this.DBG_functionTrace) {
                    var openParenLoc = 0, LSP = 0;
                    var numPar = 0;
                    this.m_string1 = this.m_globals.peek(GPDL_GLOBALS.Peek(this.m_PC) & 0xffffff);
                    openParenLoc = this.m_string1.Find('(');
                    if (openParenLoc < 0) {
                        this.m_errorLog.log("Function has illegal entry");
                        this.INTERP_RETURN(GPDL_STATE.GPDL_ILLFUNC);
                    };
                    this.m_string2.Format("0x%06x call ", m_returnStack[m_RP + 1] - 1);
                    this.m_string2 += m_string1.Left(openParenLoc + 1);
                    numPar = 0xffffff;
                    var parseValue = parseInt(m_string1 + openParenLoc + 1); if (!isNaN(parseValue)) { numPar = parseValue; }
                    if (numPar > 10) numPar = 10;
                    LSP = this.m_SP;
                    for (var j = 0; j < numPar; LSP++, j++)
                    {
                        this.m_string1 = this.m_dataStack[LSP];
                        if (this.m_string1.GetLength() > 20) this.m_string1 = this.m_string1.Left(20);
                        this.m_string2 += "\"" + this.m_string1 + "\"";
                        if (j != numPar - 1) this.m_string2 += ',';
                    };
                    this.m_string2 += ")";
                    this.m_errorLog.log(m_string2);
                };
                this.m_PC++; // skip over entry information.
                break;
            case BINOPS.BINOP_FETCH_FP:
                this.m_subop -= (this.m_subop & 0x800000) << 1;
                this.m_pushSP(this.m_dataStack[this.m_FP + this.m_subop]);
                break;
            case BINOPS.BINOP_JUMPFALSE:
                this.m_popString1();
                if ((this.m_string1 == m_false) || (this.m_string1 == "0")) this.m_PC = this.m_subop;
                break;
            case BINOPS.BINOP_RETURN:
                this.m_SP = this.m_FP;
                this.m_popFP();
                this.m_popPC();
                for (; this.m_subop & 0xfff; this.m_subop--) this.m_popString1();
                // Leave the parameters behind.
                if (this.m_PC == 0xffffffff) {
                    this.m_state = GPDL_STATE.GPDL_IDLE;
                    this.m_discourseText.Clear();
                    this.INTERP_RETURN(GPDL_STATE.GPDL_IDLE);
                };
                if (this.m_debugLevel & this.DBG_functionTrace) {
                    this.m_string1 = "          return ";
                    this.m_string1 += "\"" + this.m_dataStack[this.m_SP] + "\"";
                    this.m_errorLog.log(this.m_string1);
                };
                break;
            case BINOPS.BINOP_STORE_FP:
                this.m_subop -= (this.m_subop & 0x800000) << 1;
                this.m_dataStack[this.m_FP + this.m_subop] = this.m_popSP();
                break;
            case BINOPS.BINOP_SUBOP:
                switch (this.m_subop) {
                    case SUBOPS.SUBOP_AddCombatant:
                        this.m_popInteger1();  // Friendly flag
                        this.m_popString1();  // monster name
                        this.m_pushSP(m_false);
                        break;
                    case SUBOPS.SUBOP_ToHitComputation_Roll:
                        this.m_Integer1 = 10;
                        this.m_pushInteger1();
                        break;
                    case SUBOPS.SUBOP_Alignment:
                        this.m_popString1();
                        this.m_string2 = this.m_Alignment(this.m_string1);
                        this.m_pushString2();
                        break;
                    case SUBOPS.SUBOP_AlignmentGood:
                        this.m_popString1();
                        this.m_string2 = this.m_AlignmentGood(this.m_string1);
                        this.m_pushString2();
                        break;
                    case SUBOPS.SUBOP_AlignmentEvil:
                        this.m_popString1();
                        this.m_string2 = this.m_AlignmentEvil(this.m_string1);
                        this.m_pushString2();
                        break;
                    case SUBOPS.SUBOP_AlignmentLawful:
                        this.m_popString1();
                        this.m_string2 = this.m_AlignmentLawful(this.m_string1);
                        this.m_pushString2();
                        break;
/*                    case SUBOPS.SUBOP_AlignmentNeutral:
                        m_popString1();
                        m_string2 = m_AlignmentNeutral(m_string1);
                        m_pushString2();
                        break;
                    case SUBOP_AlignmentChaotic:
                        m_popString1();
                        m_string2 = m_AlignmentChaotic(m_string1);
                        m_pushString2();
                        break;
                    case SUBOPS.SUBOP_GET_CHARACTER_SA:
                        {
                            m_popString2();  // SA name
                            m_pushString3();
                        };
                        break;
                    case SUBOPS.SUBOP_SET_CHARACTER_SA:
                        {
                            m_popString1();  // Parameter
                            m_popString2();  // name
                            m_popString3();
                        };
                        m_pushString1();
                        break;
                    case SUBOPS.SUBOP_DELETE_CHARACTER_SA:
                        {
                            m_popString2();  // SA name
                            m_pushString3();
                        };
                        break;
                    case SUBOP_GET_COMBATANT_SA:
                        {
                            m_popString2();  // name
#ifdef UAFEngine
                            COMBATANT * pCombatant = m_popCombatantActor(); // Index
                            if (pCombatant != NULL) {
                                m_string3 = pCombatant -> combatantSA.GetString(m_string2);
                            };
#endif
                            m_pushString3();
                        };
                        break;
                    case SUBOP_SET_COMBATANT_SA:
                        {
                            m_popString1(); // Parameter
                            m_popString2(); // name
#ifdef UAFEngine
                            COMBATANT * pCombatant = m_popCombatantActor();
                            if ((pCombatant != NULL) && (pCombatant != & NULL_COMBATANT)) {
                                if (IsCombatActive()) {
                                    pCombatant -> combatantSA.InsertAbility(m_string2, m_string1, "Script Add SA to combatant ", pCombatant -> GetName());
                                }
                                else {
                                    MsgBoxError("$SET_COMBATANT_SA() called when combat not active", "Error", 2);
                                };
                            }
#else
                            m_popString3();
#endif
                        };
                        m_pushString2();
                        break;
                    case SUBOP_DELETE_COMBATANT_SA:
                        {
                            m_popString2();  // name
#ifdef UAFEngine
                            COMBATANT * pCombatant = m_popCombatantActor(); // Index
                            if (pCombatant != NULL) {
                                m_string3 = pCombatant -> combatantSA.DeleteAbility(m_string2);
                            };
#endif
                            m_pushString3();
                        };
                        break;
                    case SUBOP_DUMP_CHARACTER_SAS:
                        {
#ifdef UAFEngine
                            CHARACTER * pCharacter = m_popCharacterActor();
                            if (pCharacter != NULL) {
                                POSITION pos;
                                pos = pCharacter -> specAbs.GetHeadPosition();




                                while (pos != NULL) {
                                    const CStringPAIR * pSpecAb;
                                    CString line;
                                    pSpecAb = pCharacter -> specAbs.GetNextData(pos);
                                    line.Format(" Character Special Ability = %s; value = %s",
                                        pSpecAb -> Key(), pSpecAb -> Value());
                                    m_errorLog.log(line);
                                };
                            };





#endif
                            m_string3 = "Dump of Character SAs";
                            m_pushString3();
                        };
                        break;
                    case SUBOP_DelimitedStringCount:
                        {
                            char delimiter;
                            m_Integer1 = 0;
                            m_popString1();
                            if (!m_string1.IsEmpty()) {
                                int col;
                                int len;
                                len = m_string1.GetLength();
                                delimiter = m_string1[0];
                                col = 1;
                                while ((col > 0) && (col < len)) {
                                    col = m_string1.Find(delimiter, col);
                                    m_Integer1++;
                                    col++;
                                };
                            };
                            m_pushInteger1();
                        };
                        break;
                    case SUBOP_DelimitedStringSubstring:
                        {
                            int start, len;
                            char delimiter;
                            m_string2 = emptyString;
                            m_popInteger1();
                            m_popString1();
                            len = m_string1.GetLength();
                            start = 0;
                            if ((len > 0) && (m_Integer1 >= 0)) {
                                delimiter = m_string1[0];
                                while ((start >= 0) && (start < len)) {
                                    if (m_Integer1 == 0) {
                                        int end;
                                        end = m_string1.Find(delimiter, start + 1);
                                        if (end < 0) end = len;
                                        m_string2 = m_string1.Mid(start + 1, end - start - 1);
                                        break;
                                    };
                                    m_Integer1--;
                                    start = m_string1.Find(delimiter, start + 1);
                                };
                            };
                        };
                        m_pushString2();
                        break;
                    case SUBOP_DelimitedStringHead:
                        {
                            int len;
                            char delimiter;
                            m_string2 = emptyString;
                            m_popString1();
                            len = m_string1.GetLength();
                            if (len > 0) {
                                delimiter = m_string1[0];
                                {
                                    int end;
                                    end = m_string1.Find(delimiter, 1);
                                    if (end < 0) end = len;
                                    m_string2 = m_string1.Mid(1, end - 1);
                                };
                            };
                        };
                        m_pushString2();
                        break;
                    case SUBOP_DelimitedStringTail:
                        {
                            int len;
                            char delimiter;
                            m_string2 = emptyString;
                            m_popString1();
                            len = m_string1.GetLength();
                            if (len > 0) {
                                delimiter = m_string1[0];
                                {
                                    int end;
                                    end = m_string1.Find(delimiter, 1);
                                    if (end < 0) end = len;
                                    m_string2 = m_string1.Right(len - end);
                                };
                            };
                        };
                        m_pushString2();
                        break;
                    case SUBOP_DelimitedStringFilter:
                        {
                            m_popString3();  // Filter NAme
                            m_popString2();  // Filter string
                            m_popString1();  // source string
#ifdef UAFEngine
                            m_string4 = DelimitedStringFilter(m_string1, m_string2, m_string3);
#else
                            m_string4.Empty();
#endif
                            m_pushString4();
                        };
                        break;
                    case SUBOP_DelimitedStringAdd:
                        {
                            m_popString3();  // Possible Delimiter
                            m_popString2(); // New head
                            m_popString1(); // original string
                            if (m_string1.IsEmpty()) {
                                if (m_string3.IsEmpty()) {
                                    m_string4 = '#';
                                }
                                else {
                                    m_string4 = m_string3[0];
                                };
                            }
                            else {
                                m_string4 = m_string1[0];
                            }
                            m_string4 += m_string2;
                            m_string4 += m_string1;
                            m_pushString4();
                        };
                        break;
                    case SUBOP_GET_ITEM_SA:
                        {
                            ITEM_DATA * pItemData;
                            m_popString2(); // SA name
                            m_popString1(); // item name
                            ITEM_ID itemID;
                            itemID = m_string1;
                            pItemData = itemData.GetItem(itemID);
                            if (pItemData == NULL) {
                                m_string1 = NO_SUCH_SA;
                            }
                            else {
                                CStringPAIR * csp;
                                csp = pItemData -> specAbs.FindAbility(m_string2);
                                if (csp == NULL) m_string1 = NO_SUCH_SA;
                                else m_string1 = csp -> Value();
                            };
                            m_pushString1();
                        };
                        break;
                    //case SUBOP_SET_ITEM_SA:
                    //  {
                    //    ITEM_DATA *pItemData;
                    //    m_popString3(); // SA value
                    //    m_popString2(); // SA name
                    //    m_popString1(); // item name
                    //    ITEM_ID itemID;
                    //    itemID = m_string1;
                    //    pItemData = itemData.GetItem(itemID);
                    //    if (pItemData == NULL)
                    //    {
                    //      m_string1 = NO_SUCH_SA;
                    //    }
                    //    else
                    //    {
                    //      pItemData->specAbs.InsertAbility(m_string2, m_string3, m_string1, m_string2);
                    //    };
                    //    m_pushString1();
                    //  };
                    //  break;
                    //case SUBOP_DELETE_ITEM_SA:
                    //  {
                    //    ITEM_DATA *pItemData;
                    //    m_popString2(); // SA name
                    //    m_popString1(); // item name
                    //    ITEM_ID itemID;
                    //    itemID = m_string1;
                    //    pItemData = itemData.GetItem(itemID);
                    //    if (pItemData == NULL)
                    //    {
                    //      m_string1 = NO_SUCH_SA;
                    //    }
                    //    else
                    //    {
                    //      m_string1 = pItemData->specAbs.DeleteAbility(m_string2);
                    //    };
                    //    m_pushString1();
                    //  };
                    case SUBOP_GET_SPELL_SA:
                        {
                            SPELL_DATA * pSpellData;
                            m_popString2(); // SA name
                            m_popString1(); // spell name
                            SPELL_ID spellID;
                            spellID = m_string1;
                            pSpellData = spellData.GetSpell(spellID);
                            if (pSpellData == NULL) {
                                m_string1 = NO_SUCH_SA;
                            }
                            else {
                                CStringPAIR * csp;
                                csp = pSpellData -> specAbs.FindAbility(m_string2);
                                if (csp == NULL) m_string1 = NO_SUCH_SA;
                                else m_string1 = csp -> Value();
                            };
                            m_pushString1();
                        };
                        break;
                    /*  Spell SAs are read-only
                          case SUBOP_SET_SPELL_SA:
                            {
                              SPELL_DATA *pSpellData;
                              m_popString3(); // SA value
                              m_popString2(); // SA name
                              m_popString1(); // spell name
                              SPELL_ID spellID;
                              spellID = m_string1;
                              pSpellData = spellData.GetSpell(spellID);
                              if (pSpellData == NULL)
                              {
                                m_string1 = NO_SUCH_SA;
                              }
                              else
                              {
                                pSpellData->specAbs.InsertAbility(m_string2, m_string3, m_string1, m_string2);
                              };
                              m_pushString1();
                            };
                            break;
                          case SUBOP_DELETE_SPELL_SA:
                            {
                              SPELL_DATA *pSpellData;
                              m_popString2(); // SA name
                              m_popString1(); // spell name
                              SPELL_ID spellID;
                              spellID = m_string1;
                              pSpellData = spellData.GetSpell(spellID);
                              if (pSpellData == NULL)
                              {
                                m_string1 = NO_SUCH_SA;
                              }
                              else
                              {
                                m_string1 = pSpellData->specAbs.DeleteAbility(m_string2);
                              };
                              m_pushString1();
                            };
                            break;
                    
                     * /
                    /*
                       case SUBOP_GET_SPELLGROUP_SA:
                           {
                             const SPELLGROUP_DATA *pSpellgroupData;
                             SPELLGROUP_ID spellgroupID;
                             m_popString2(); // SA name
                             m_popString1(); // spellgroup name
                             //pSpellgroupData = spellgroupData.GetSpellgroupData(m_string1);
                             spellgroupID = m_string1;
                             pSpellgroupData = spellgroupData.PeekSpellgroup(spellgroupID);
                             if (pSpellgroupData == NULL)
                             {
                               m_string1 = NO_SUCH_SA;
                             }
                             else
                             {
                               CStringPAIR *csp;
                               csp = pSpellgroupData->m_specAbs.FindAbility(m_string2);
                               if (csp == NULL) m_string1 = NO_SUCH_SA;
                               else m_string1 = csp->Value();
                             };
                             m_pushString1();
                           };
                           break;
                         //case SUBOP_SET_SPELLGROUP_SA:
                         //  {
                         //    SPELLGROUP_DATA *pSpellgroupData;
                         //    SPELLGROUP_ID spellgroupID;
                         //    m_popString3(); // SA value
                         //    m_popString2(); // SA name
                         //    m_popString1(); // spellgroup name
                         //    spellgroupID = m_string1;
                         //    //pSpellgroupData = spellgroupData.GetSpellgroupData(m_string1);
                         //    pSpellgroupData = spellgroupData.GetSpellgroup(spellgroupID);
                         //    if (pSpellgroupData == NULL)
                         //    {
                         //      m_string1 = NO_SUCH_SA;
                         //    }
                         //    else
                         //    {
                         //      pSpellgroupData->m_specAbs.InsertAbility(m_string2, m_string3, m_string1, m_string2);
                         //    };
                         //    m_pushString1();
                         //  };
                         //  break;
                         //case SUBOP_DELETE_SPELLGROUP_SA:
                         //  {
                         //    SPELLGROUP_DATA *pSpellgroupData;
                         //    SPELLGROUP_ID spellgroupID;
                         //    m_popString2(); // SA name
                         //    m_popString1(); // spellgroup name
                         //    spellgroupID = m_string1;
                         //    pSpellgroupData = spellgroupData.GetSpellgroup(spellgroupID);
                         //    if (pSpellgroupData == NULL)
                         //    {
                         //      m_string1 = NO_SUCH_SA;
                         //    }
                         //    else
                         //    {
                         //      m_string1 = pSpellgroupData->m_specAbs.DeleteAbility(m_string2);
                         //    };
                         //    m_pushString1();
                         //  };
                         //  break;
                   * /
                    case SUBOP_GET_MONSTERTYPE_SA:
                        {
                            MONSTER_DATA * pMonsterData;
                            MONSTER_ID monsterID;
                            m_popString2(); // SA name
                            m_popString1(); // monster name
                            monsterID = m_string1;
                            pMonsterData = monsterData.GetMonster(monsterID);
                            if (pMonsterData == NULL) {
                                m_string1 = NO_SUCH_SA;
                            }
                            else {
                                CStringPAIR * csp;
                                csp = pMonsterData -> specAbs.FindAbility(m_string2);
                                if (csp == NULL) m_string1 = NO_SUCH_SA;
                                else m_string1 = csp -> Value();
                            };
                            m_pushString1();
                        };
                        break;
                    //case SUBOP_SET_MONSTERTYPE_SA:
                    //  {
                    //    MONSTER_DATA *pMonsterData;
                    //    MONSTER_ID monsterID;
                    //    m_popString3(); // SA value
                    //    m_popString2(); // SA name
                    //    m_popString1(); // monster name
                    //    monsterID = m_string1;
                    //    pMonsterData = monsterData.GetMonster(monsterID);
                    //    if (pMonsterData == NULL)
                    //    {
                    //      m_string1 = NO_SUCH_SA;
                    //    }
                    //    else
                    //    {
                    //      pMonsterData->specAbs.InsertAbility(m_string2, m_string3, m_string1, m_string2);
                    //    };
                    //    m_pushString1();
                    //  };
                    //  break;
                    //case SUBOP_DELETE_MONSTERTYPE_SA:
                    //  {
                    //    MONSTER_DATA *pMonsterData;
                    //    MONSTER_ID monsterID;
                    //    m_popString2(); // SA name
                    //    m_popString1(); // monster name
                    //    monsterID = m_string1;
                    //    pMonsterData = monsterData.GetMonster(monsterID);
                    //    if (pMonsterData == NULL)
                    //    {
                    //      m_string1 = NO_SUCH_SA;
                    //    }
                    //    else
                    //    {
                    //      m_string1 = pMonsterData->specAbs.DeleteAbility(m_string2);
                    //    };
                    //    m_pushString1();
                    //  };
                    //  break;
                    case SUBOP_GET_RACE_SA:
                        {
                            RACE_DATA * pRaceData;
                            RACE_ID raceID;
                            m_popString2(); // SA name
                            m_popString1(); // race name
                            raceID = m_string1;
                            pRaceData = raceData.GetRace(raceID);
                            if (pRaceData == NULL) {
                                m_string1 = NO_SUCH_SA;
                            }
                            else {
                                CStringPAIR * csp;
                                csp = pRaceData -> m_specAbs.FindAbility(m_string2);
                                if (csp == NULL) m_string1 = NO_SUCH_SA;
                                else m_string1 = csp -> Value();
                            };
                            m_pushString1();
                        };
                        break;
                    //case SUBOP_DELETE_RACE_SA:
                    //  {
                    //    RACE_DATA *pRaceData;
                    //    RACE_ID raceID;
                    //    m_popString2(); // SA name
                    //    m_popString1(); // race name
                    //    //pRaceData = raceData.GetRaceData(m_string1);
                    //    raceID = m_string1;
                    //    pRaceData = raceData.GetRace(raceID);
                    //    if (pRaceData == NULL)
                    //    {
                    //      m_string1 = NO_SUCH_SA;
                    //    }
                    //    else
                    //    {
                    //      m_string1 = pRaceData->m_specAbs.DeleteAbility(m_string2);
                    //    };
                    //    m_pushString1();
                    //  };
                    //  break;
                    case SUBOP_GET_CLASS_SA:
                        {
                            CLASS_DATA * pClassData;
                            CLASS_ID classID;
                            m_popString2(); // SA name
                            m_popString1(); // class name
                            //pClassData = classData.GetClassData(m_string1);
                            classID = m_string1;
                            pClassData = classData.GetClass(classID);
                            if (pClassData == NULL) {
                                m_string1 = NO_SUCH_SA;
                            }
                            else {
                                CStringPAIR * csp;
                                csp = pClassData -> m_specialAbilities.FindAbility(m_string2);
                                if (csp == NULL) m_string1 = NO_SUCH_SA;
                                else m_string1 = csp -> Value();
                            };
                            m_pushString1();
                        };
                        break;
                    case SUBOP_GET_BASECLASS_SA:
                        {
                            BASE_CLASS_DATA * pBaseclassData;
                            BASECLASS_ID baseclassID;
                            m_popString2(); // SA name
                            m_popString1(); // baseclass name
                            //pClassData = classData.GetClassData(m_string1);
                            baseclassID = m_string1;
                            pBaseclassData = baseclassData.GetBaseclass(baseclassID);
                            if (pBaseclassData == NULL) {
                                m_string1 = NO_SUCH_SA;
                            }
                            else {
                                CStringPAIR * csp;
                                csp = pBaseclassData -> m_specAbs.FindAbility(m_string2);
                                if (csp == NULL) m_string1 = NO_SUCH_SA;
                                else m_string1 = csp -> Value();
                            };
                            m_pushString1();
                        };
                        break;
      //case SUBOP_SET_CLASS_SA:
      //  {
      //    CLASS_DATA *pClassData;
      //    CLASS_ID classID;
      //    m_popString3(); // SA value
      //    m_popString2(); // SA name
      //    m_popString1(); // class name
      //    classID = m_string1;
      //    //pClassData = classData.GetClassData(m_string1);
      //    pClassData = classData.GetClass(classID);
      //    if (pClassData == NULL)
      //    {
      //      m_string1 = NO_SUCH_SA;
      //    }
      //    else
      //    {
      //      pClassData->m_specialAbilities.InsertAbility(m_string2, m_string3, m_string1, m_string2);
      //    };
      //    m_pushString1();
      //  };
      //  break;
      //case SUBOP_DELETE_CLASS_SA:
      //  {
      //    CLASS_DATA *pClassData;
      //    CLASS_ID classID;
      //    m_popString2(); // SA name
      //    m_popString1(); // class name
      //    classID = m_string1;
      //    //pClassData = classData.GetClassData(m_string1);
      //    pClassData = classData.GetClass(classID);
      //    if (pClassData == NULL)
      //    {
      //      m_string1 = NO_SUCH_SA;
      //    }
      //    else
      //    {
      //      m_string1 = pClassData->m_specialAbilities.DeleteAbility(m_string2);
      //    };
      //    m_pushString1();
      //  };
      //  break;

#ifdef USE_TRAITS
      //case SUBOP_GET_TRAIT_SA:
      //  {
      //    TRAIT_DATA *pTraitData;
      //    m_popString2(); // SA name
      //    m_popString1(); // trait name
      //    pTraitData = traitData.GetTraitData(m_string1);
      //    if (pTraitData == NULL)
      //    {
      //      m_string1 = NO_SUCH_SA;
      //    }
      //    else
      //    {
      //      CStringPAIR *csp;
      //      csp = pTraitData->m_specAbs.FindAbility(m_string2);
      //      if (csp == NULL) m_string1 = NO_SUCH_SA;
      //      else m_string1 = csp->Value();
      //    };
      //    m_pushString1();
      //  };
      //  break;
      //case SUBOP_SET_TRAIT_SA:
      //  {
      //    TRAIT_DATA *pTraitData;
      //    m_popString3(); // SA value
      //    m_popString2(); // SA name
      //    m_popString1(); // trait name
      //    pTraitData = traitData.GetTraitData(m_string1);
      //    if (pTraitData == NULL)
      //    {
      //      m_string1 = NO_SUCH_SA;
      //    }
      //    else
      //    {
      //      pTraitData->m_specAbs.InsertAbility(m_string2, m_string3, m_string1, m_string2);
      //    };
      //    m_pushString1();
      //  };
      //  break;
      //case SUBOP_DELETE_TRAIT_SA:
      //  {
      //    TRAIT_DATA *pTraitData;
      //    m_popString2(); // SA name
      //    m_popString1(); // trait name
      //   pTraitData = traitData.GetTraitData(m_string1);
      //    if (pTraitData == NULL)
      //    {
      //      m_string1 = NO_SUCH_SA;
      //    }
      //    else
      //    {
      //      m_string1 = pTraitData->m_specAbs.DeleteAbility(m_string2);
      //    };
      //    m_pushString1();
      //  };
      //  break;
#endif

                    case SUBOP_GET_ABILITY_SA:
                        {
                            const ABILITY_DATA * pAbilityData;
                            m_popString2(); // SA name
                            m_popString1(); // ability name
                            pAbilityData = abilityData.PeekAbilityData(abilityData.LocateAbilityData(m_string1));
                            if (pAbilityData == NULL) {
                                m_string1 = NO_SUCH_SA;
                            }
                            else {
                                CStringPAIR * csp;
                                csp = pAbilityData -> m_specAbs.FindAbility(m_string2);
                                if (csp == NULL) m_string1 = NO_SUCH_SA;
                                else m_string1 = csp -> Value();
                            };
                            m_pushString1();
                        };
                        break;
                    //case SUBOP_SET_ABILITY_SA:
                    //  {
                    //    ABILITY_DATA *pAbilityData;
                    //    m_popString3(); // SA value
                    //    m_popString2(); // SA name
                    //    m_popString1(); // ability name
                    //    pAbilityData = abilityData.GetAbilityData(abilityData.LocateAbilityData(m_string1));
                    //    if (pAbilityData == NULL)
                    //    {
                    //      m_string1 = NO_SUCH_SA;
                    //    }
                    //    else
                    //    {
                    //      pAbilityData->m_specAbs.InsertAbility(m_string2, m_string3, m_string1, m_string2);
                    //    };
                    //    m_pushString1();
                    //  };
                    //  break;
                    //case SUBOP_DELETE_ABILITY_SA:
                    //  {
                    //    ABILITY_DATA *pAbilityData;
                    //    m_popString2(); // SA name
                    //    m_popString1(); // ability name
                    //    pAbilityData = abilityData.GetAbilityData(abilityData.LocateAbilityData(m_string1));
                    //    if (pAbilityData == NULL)
                    //    {
                    //      m_string1 = NO_SUCH_SA;
                    //    }
                    //    else
                    //    {
                    //      m_string1 = pAbilityData->m_specAbs.DeleteAbility(m_string2);
                    //    };
                    //    m_pushString1();
                    //  };
                    //  break;
                    case SUBOP_GET_HOOK_PARAM:
                        m_popInteger1();
                        m_string1 = emptyString;
                        if (m_Integer1 < NUMHOOKPARAM) {
                            m_string1 = (* p_hook_parameters)[m_Integer1];
                        };
                        m_pushString1();
                        break;
                    case SUBOP_SET_HOOK_PARAM:
                        m_popString1();  // The new value
                        m_popInteger1(); // the index
                        m_string2 = emptyString;
                        if ((m_Integer1 < NUMHOOKPARAM) && (m_Integer1 >= 0)) {
                            m_string2 = (* p_hook_parameters)[m_Integer1];
                            (* p_hook_parameters)[m_Integer1] = m_string1;
                        };
                        m_pushString2();
                        break;
                    case SUBOP_SA_NAME:
#ifdef UAFEngine
                        SA_Name();
#else
                        m_string1.Empty();
                        m_pushString1();
#endif
                        break;
                    case SUBOP_SA_SOURCE_TYPE:
#ifdef UAFEngine
                        m_string1 = pScriptContext -> GetSourceTypeName();
#else
                        m_string1.Empty();
#endif
                        m_pushString1();
                        break;
                    case SUBOP_SA_SOURCE_NAME:
#ifdef UAFEngine
                        m_string1 = pScriptContext -> GetSourceName();
#else
                        m_string1.Empty();
#endif
                        m_pushString1();
                        break;
                    case SUBOP_SA_PARAM_GET:
                        SA_Param();
                        break;
                    case SUBOP_SA_PARAM_SET:
                        m_popString1();  // parameter value
                        pScriptContext -> SetParam(m_string1);
                        m_pushString1();
                        break;
                    case SUBOP_SA_REMOVE:
#ifdef UAFEngine
                        m_string1 = pScriptContext -> RemoveAbility();
#endif
                        m_pushString1();
                        break;
                    case SUBOP_SA_ITEM_GET:
                        SA_Param(pScriptContext -> GetItemSAContext("$SA_ITEM_GET()"));
                        break;
                    case SUBOP_SA_CHARACTER_GET:
                        SA_Param(pScriptContext -> GetCharacterSAContext("$SA_CHARACTER_GET()"));
                        break;
                    case SUBOP_SA_COMBATANT_GET:
#ifdef UAFEngine
                        SA_Param(pScriptContext -> GetCombatantSAContext("$SA_COMBATANT_GET()"));
#endif
                        break;
                    case SUBOP_SA_CLASS_GET:
                        SA_Param(pScriptContext -> GetClassSAContext("$SA_CLASS_GET()"));
                        break;
                    case SUBOP_SA_BASECLASS_GET:
                        SA_Param(pScriptContext -> GetBaseclassSAContext("$SA_BASECLASS_GET()"));
                        break;
                    case SUBOP_SA_SPELL_GET:
                        SA_Param(pScriptContext -> GetSpellSAContext("$SA_SPELL_GET()"));
                        break;
                    case SUBOP_SA_MONSTERTYPE_GET:
                        SA_Param(pScriptContext -> GetMonstertypeSAContext("$SA_MONSTERTYPE_GET()"));
                        break;
                    case SUBOP_SA_RACE_GET:
                        SA_Param(pScriptContext -> GetRaceSAContext("$SA_RACE_GET()"));
                        break;
                    case SUBOP_SA_ABILITY_GET:
                        SA_Param(pScriptContext -> GetAbilitySAContext("$SA_ABILITY_GET()"));
                        break;

#ifdef USE_TRAITS
      //case SUBOP_SA_TRAIT_GET:
      //  SA_Param(pScriptContext->GetTraitSAContext());
      //  break;
#endif





                    case SUBOP_COINCOUNT:
                        if (IsCombatActive()) {
                            m_interpretError("Combat active in $COINCOUNT");
                            m_Integer3 = 0;
                            m_pushInteger3();
                            break;
                        };
                        /*  **** Replaced with code that uses character uniqueID instead of party index.
                        m_popInteger1();   // The coin ordinal
                        if (m_Integer1 < 1) m_Integer1 = 1;
                        if (m_Integer1 > MONEY_DATA_TYPE::MAX_COIN_TYPES) m_Integer1 = 1;
                        
                        m_popInteger2();   // The character ordinal
                        if ((m_Integer2 < 1) || (m_Integer2 > GetPartySize(party)))
                        {
                          // assume active char 
                          m_Integer2 = party.activeCharacter+1;
                        };   
                        
                        m_Integer3 = party.characters[m_Integer2-1].money.Coins[m_Integer1-1];
                        m_pushInteger3();
                        * /
                        {
                            CHARACTER * pChar;
                            m_popInteger1(); // Coin Ordinal
                            if (m_Integer1 > MONEY_DATA_TYPE:: MAX_COIN_TYPES) m_Integer1 = 1;
                            pChar = Dude("$COINCOUNT()");
                            m_Integer3 = pChar -> money.Coins[m_Integer1 - 1];
                            m_pushInteger3();
                        };
                        break;
                    case SUBOP_COINNAME:
                        m_popInteger1(); // The coin ordinal;
                        m_string1 = globalData.moneyData.GetCoinName(m_Integer1 - 1);
                        m_pushString1();
                        break;
                    case SUBOP_COINRATE:
                        m_popInteger1(); // The coin ordinal;
                        m_Integer2 = globalData.moneyData.GetCoinRate(m_Integer1 - 1);
                        m_pushInteger2();
                        break;
                    case SUBOP_CURR_CHANGE_BY_VAL:
                        {
                            // GPDL doesn't handle floats, convert to integer
                            double ir = GetIntermediateResult();
                            int temp = 0;
                            if (ir < 0.0)
                                temp = ceil(ir + -0.5); // round downo
                            else
                                temp = floor(ir + 0.5); // round up

                            m_string1.Format("%i", temp);
                            m_pushString1();
                        }
                        break;
                    case SUBOP_DEBUG:
                        m_popString1();
                        m_string2.Format("$DEBUG(%s)", m_string1);
                        m_errorLog.log(m_string2);
                        m_pushString1();  // Our 'result'
                        break;
                    case SUBOP_DebugWrite:
                        WriteDebugString(m_dataStack[m_SP] + '\n');
                        break;
                    case SUBOP_DEPRECATED_DELETE_LEVEL_STATS_ASL:
                        m_popString1();  // key
                        DeleteLevelASL(globalData,
                            GetCurrentLevel(),
                            m_string1);
                        //        globalData.levelInfo.stats[globalData.currLevel]
                        //          .level_attributes.Delete(m_string1);
                        m_pushSP(m_false);  // Must supply a result
                        break;
                    case SUBOP_DELETE_LEVEL_STATS_ASL:
                        m_popString1();  // key
                        m_popString3();  //Level
                        if (m_string3 == "") m_Integer1 = GetCurrentLevel();
                        else m_Integer1 = atoi(m_string3);
                        DeleteLevelASL(globalData,
                            m_Integer1,
                            m_string1);
                        //        globalData.levelInfo.stats[globalData.currLevel]
                        //          .level_attributes.Delete(m_string1);
                        m_pushSP(m_false);  // Must supply a result
                        break;
                    case SUBOP_DELETE_PARTY_ASL:
                        m_popString1();  // key
                        DeletePartyASL(party, m_string1);
                        //        party.party_attributes.Delete(m_string1);
                        m_pushSP(m_false
                        );  // Must supply a result
                        break;
                    case SUBOP_DUP:
                        m_popString1();
                        m_pushString1();
                        m_pushString1();
                        break;
                    case SUBOP_FALSE:
                        m_pushSP(m_false);
                        break;
                    case SUBOP_FORCENUMERIC:
                        m_popInteger1();
                        m_pushInteger1();
                        break;
                    case SUBOP_ISEQUAL:
                        m_popString1();
                        m_popString2();
                        m_pushSP(m_string1 == m_string2 ? m_true : m_false);
                        break;
                    case SUBOP_Gender:
                        m_popString1();
                        ASSERT(m_string1.GetLength() == 24);
                        m_string2 = m_Gender(m_string1);
                        m_pushString2();
                        break;
                    case SUBOP_GET_CHAR_ASL:




                        {
                            CHARACTER * pChar;
                            m_popString1();  // key
                            pChar = m_popCharacter("$GET_CHAR_ASL()"); // character number
#ifdef UAFEngine
                            m_string2 = LookupCharacterASL(
                                pChar,
                                m_string1);
#else
                            m_string2 = "Asl Value";
#endif
                        };
                        m_pushSP(m_string2);  // Must supply a result
                        break;



                    //if (IsCombatActive())
                    //{
                    //  m_interpretError("Combat active in $GET_CHAR_ASL");
                    //  m_string2="";
                    //  m_pushString2();
                    //  break;
                    //}
                    /* Unreachable
                    m_popString1();
                    m_popInteger1();
                    if ((m_Integer1<0) 
                        || (m_Integer1>=GetPartySize(party))) 
                    {
                      m_interpretError("Illegal party member # in $GET_CHAR_ASL");
                      break;
                    };
                    m_pushString2();
                    break;
                    * /
                    case SUBOP_GET_CHAR_RACE:
                        {
                            CHARACTER * pChar;
                            pChar = m_popCharacter("$GET_CHAR_RACE()");
                            if (pChar != NULL) {
                                m_string1 = raceData.GetRaceName(pChar -> RaceID());
                            }
                            else {
                                m_string1 = "NoSuchCharacter";
                            };
                            m_pushString1();
                        };
                        break;
                    case SUBOP_SET_CHAR_RACE:
                        {
                            CHARACTER * pChar;
                            m_popString1();
                            pChar = m_popCharacter("$SET_CHAR_RACE()");
                            if (pChar != NULL) {
                                //raceType newRace;
                                RACE_ID newRaceID;
                                newRaceID = m_string1;
                                if (raceData.LocateRace(newRaceID) >= 0)
                                //if (newRace == raceUnknown)
                                {
                                    pChar -> RaceID(newRaceID);
                                }
                                else {
                                    m_string1 = "";
                                };
                            }
                            else {
                                m_string1 = "";
                            };
                            m_pushString1();
                        };
                        break;

                    case SUBOP_GET_ISMAMMAL:
                        m_GetActorBOOL(& CHARACTER:: IsMammal, "$GET_ISMAMMAL()"); break;
                        {
                            /*
                            m_popString1(); // ActorType
                            ActorType actor = m_StringToActor((LPCSTR)m_string1);
                            m_getCharacterValueWithActor(CHAR_ISMAMMAL, actor);      
                            break;
                            * /
                        }
                    case SUBOP_GET_ISANIMAL:
                        GET_ACTOR_BOOL(IsAnimal, "$GET_ISANIMAL()"); break;
                        {
                            /*
                            m_popString1(); // ActorType
                            ActorType actor = m_StringToActor((LPCSTR)m_string1);
                            m_getCharacterValueWithActor(CHAR_ISANIMAL, actor);      
                            break;
                            * /
                        }
                    case SUBOP_GET_ISSNAKE:
                        GET_ACTOR_BOOL(IsSnake, "$GET_ISSNAKE()"); break;
                    /*
                    {
                      m_popString1(); // ActorType
                      ActorType actor = m_StringToActor((LPCSTR)m_string1);
                      m_getCharacterValueWithActor(CHAR_ISSNAKE, actor);      
                    }
                    break;
                    * /
                    case SUBOP_GET_ISGIANT:
                        GET_ACTOR_BOOL(IsGiant, "$GET_ISGIANT()"); break;
                    /*
                    {
                      m_popString1(); // ActorType
                      ActorType actor = m_StringToActor((LPCSTR)m_string1);
                      m_getCharacterValueWithActor(CHAR_ISGIANT, actor);      
                    }
                    break;        
                    * /
                    case SUBOP_GET_ISALWAYSLARGE:
                        GET_ACTOR_BOOL(IsAlwaysLarge, "$GET_ISALWAYSLARGE()"); break;
                    /*
                    {
                      m_popString1(); // ActorType
                      ActorType actor = m_StringToActor((LPCSTR)m_string1);
                      m_getCharacterValueWithActor(CHAR_ISALWAYSLARGE, actor);      
                    }
                    break;
                    * /
                    case SUBOP_GET_HASDWARFACPENALTY:
                        GET_ACTOR_BOOL(HasDwarfACPenalty, "$GET_HASDWARFACPENALTY()"); break;
                    /*
                    {
                      m_popString1(); // ActorType
                      ActorType actor = m_StringToActor((LPCSTR)m_string1);
                      m_getCharacterValueWithActor(CHAR_HASDWARFACPENALTY, actor);      
                    }
                    break;
                    * /
                    case SUBOP_GET_HASGNOMEACPENALTY:
                        GET_ACTOR_BOOL(HasGnomeACPenalty, "$GET_HASGNOMEACPENALTY()"); break;
                    /*
                    {
                      m_popString1(); // ActorType
                      ActorType actor = m_StringToActor((LPCSTR)m_string1);
                      m_getCharacterValueWithActor(CHAR_HASGNOMEACPENALTY, actor);      
                    }
                    break;
                    * /
                    case SUBOP_GET_HASDWARFTHAC0PENALTY:
                        GET_ACTOR_BOOL(HasDwarfTHAC0Penalty, "$GET_HASDWARFTHAC0PENALTY()"); break;
                    /*
                    {
                      m_popString1(); // ActorType
                      ActorType actor = m_StringToActor((LPCSTR)m_string1);
                      m_getCharacterValueWithActor(CHAR_HASDWARFTHAC0PENALTY, actor);      
                    }
                    break;
                    * /
                    case SUBOP_GET_HASGNOMETHAC0PENALTY:
                        GET_ACTOR_BOOL(HasGnomeTHAC0Penalty, "$GET_HASGNOMETHAC0PENALTY()"); break;
                    /*
                    {
                      m_popString1(); // ActorType
                      ActorType actor = m_StringToActor((LPCSTR)m_string1);
                      m_getCharacterValueWithActor(CHAR_HASGNOMETHAC0PENALTY, actor);      
                    }
                    break;
                    * /
                    case SUBOP_GET_HASRANGERDMGPENALTY:
                        GET_ACTOR_BOOL(HasRangerDmgPenalty, "$GET_HASRANGERDMGPENALTY()"); break;
                    /*
                    {
                      m_popString1(); // ActorType
                      ActorType actor = m_StringToActor((LPCSTR)m_string1);
                      m_getCharacterValueWithActor(CHAR_HASRANGERDMGPENALTY, actor);      
                    }
                    break;
                    * /
                    case SUBOP_GET_HASPOISONIMMUNITY:
                        GET_ACTOR_BOOL(HasPoisonImmunity, "$GET_HASPOISONACIMMUNITY()"); break;
                    /*
                    {
                      m_popString1(); // ActorType
                      ActorType actor = m_StringToActor((LPCSTR)m_string1);
                      m_getCharacterValueWithActor(CHAR_HASPOISONIMMUNITY, actor);      
                    }
                    break;
                    * /
                    case SUBOP_GET_HASDEATHIMMUNITY:
                        GET_ACTOR_BOOL(HasDeathImmunity, "$GET_HASDEATHIMMUNITY()"); break;
                    /*
                    {
                      m_popString1(); // ActorType
                      ActorType actor = m_StringToActor((LPCSTR)m_string1);
                      m_getCharacterValueWithActor(CHAR_HASDEATHIMMUNITY, actor);      
                    }
                    break;
                    * /
                    case SUBOP_GET_HASCONFUSIONIMMUNITY:
                        GET_ACTOR_BOOL(HasConfusionImmunity, "$GET_HASCONFUSIONIMMUNITY()"); break;
                    /*
                    {
                      m_popString1(); // ActorType
                      ActorType actor = m_StringToActor((LPCSTR)m_string1);
                      m_getCharacterValueWithActor(CHAR_HASCONFUSIONIMMUNITY, actor);      
                    }
                    break;
                    * /
                    case SUBOP_GET_HASVORPALIMMUNITY:
                        GET_ACTOR_BOOL(HasVorpalImmunity, "$GET_HASVORPALIMMUNITY()"); break;
                    /*
                    {
                      m_popString1(); // ActorType
                      ActorType actor = m_StringToActor((LPCSTR)m_string1);
                      m_getCharacterValueWithActor(CHAR_HASVORPALIMMUNITY, actor);      
                    }
                    break;
                    * /
                    case SUBOP_GET_CANBEHELDORCHARMED:
                        GET_ACTOR_BOOL(CanBeHeldOrCharmed, "$GET_CANBEHELDORCHARMED()"); break;
                    /*
                    {
                      m_popString1(); // ActorType
                      ActorType actor = m_StringToActor((LPCSTR)m_string1);
                      m_getCharacterValueWithActor(CHAR_CANBEHELDORCHARMED, actor);      
                    }
                    break;
                    * /
                    case SUBOP_GET_AFFECTEDBYDISPELEVIL:
                        GET_ACTOR_BOOL(AffectedByDispelEvil, "$GET_AFFECTEDBYDISPELLEVIL()"); break;
                    /*
                    {
                      m_popString1(); // ActorType
                      ActorType actor = m_StringToActor((LPCSTR)m_string1);
                      m_getCharacterValueWithActor(CHAR_AFFECTEDBYDISPELEVIL, actor);      
                    }
                    break;
                    * /
                    //case SUBOP_SET_AFFECTEDBYDISPELEVIL:      SET_ACTOR_BOOL(AffectedByDispelEvil); break;
                    /*
                    {
                      m_popString2();
                      m_popString1(); // ActorType
                      ActorType actor = m_StringToActor((LPCSTR)m_string1);
                      m_setCharacterValueWithActor(CHAR_AFFECTEDBYDISPELEVIL, actor, m_string2);      
                    }
                    break;
                    * /
                    case SUBOP_GET_CHAR_NAME: GET_CHAR_STRING(GetName, "$GET_CHAR_NAME()"); break;
                    case SUBOP_GET_CHAR_AC: GET_CHAR_INT(GetBaseAC, "$GET_CHAR_AC()"); break;
                    case SUBOP_GET_CHAR_ADJAC: GET_CHAR_INT(GetAdjAC, "$GET_CHAR_ADJAC()"); break;
                    case SUBOP_GET_CHAR_EFFAC: GET_CHAR_INT(GetEffectiveAC, "$GET_CHAR_EFFAC()"); break;
                    case SUBOP_GET_CHAR_HITPOINTS: GET_CHAR_INT(GetAdjHitPoints, "$GET_CHAR_HITPOINTS()"); break;
                    case SUBOP_GET_CHAR_ADJTHAC0: GET_CHAR_INT(GetAdjTHAC0, "$GET_CHAR_ADJTHAC0()"); break;
                    case SUBOP_GET_CHAR_THAC0: GET_CHAR_INT(GetTHAC0, "$GET_CHAR_THAC0()"); break;
                    case SUBOP_GET_CHAR_RDYTOTRAIN: GET_CHAR_INT(GetAdjReadyToTrain, "$GET_CHAR_RDYTOTRAIN()"); break;
                    case SUBOP_GET_CHAR_Exp:
                        {
                            m_popString1(); // class
#ifdef UAFEngine
                            {
                                CHARACTER * pChar;
                                BASECLASS_ID baseclassID;
                                baseclassID = m_string1;
                                pChar = m_popCharacter("$GET_CHAR_Exp()");
                                //m_Integer1 = pChar->GetCurrExp(m_string1);
                                m_Integer1 = pChar -> GetCurrExp(baseclassID);
                            };
#else
                            {
                                m_popInteger1();
                                m_Integer1 = 20000;
                            };
#endif
                            m_pushInteger1();
                        };
                        break;
                    case SUBOP_GET_CHAR_ICON_INDEX: GET_CHAR_INT(GetIconIndex, "$GET_CHAR_ICON_INDEX()"); break;
                    case SUBOP_GET_CHAR_AGE: GET_CHAR_INT(GetAdjAge, "$GET_CHAR_AGE()"); break;
                    case SUBOP_GET_CHAR_MAXAGE: GET_CHAR_INT(GetAdjMaxAge, "$GET_CHAR_MAXAGE()"); break;
                    case SUBOP_GET_CHAR_MAXMOVE: GET_CHAR_INT(GetAdjMaxMovement_GPDL, "$GET_CHAR_MAXMOVE()"); break;
                    case SUBOP_GET_CHAR_LIMITED_STR: GET_CHAR_INT(GetLimitedStr, "$GET_CHAR_LIMITED_STR()"); break;
                    case SUBOP_GET_CHAR_LIMITED_STRMOD: GET_CHAR_INT(GetLimitedStrMod, "$GET_CHAR_LIMITED_STRMOD()"); break;
                    case SUBOP_GET_CHAR_LIMITED_INT: GET_CHAR_INT(GetLimitedInt, "$GET_CHAR_LIMITED_INT()"); break;
                    case SUBOP_GET_CHAR_LIMITED_WIS: GET_CHAR_INT(GetLimitedWis, "$GET_CHAR_LIMITED_WIS()"); break;
                    case SUBOP_GET_CHAR_LIMITED_DEX: GET_CHAR_INT(GetLimitedDex, "$GET_CHAR_LIMITED_DEX()"); break;
                    case SUBOP_GET_CHAR_LIMITED_CON: GET_CHAR_INT(GetLimitedCon, "$GET_CHAR_LIMITED_CON()"); break;
                    case SUBOP_GET_CHAR_LIMITED_CHA: GET_CHAR_INT(GetLimitedCha, "$GET_CHAR_LIMITED_CHA()"); break;
                    case SUBOP_GET_CHAR_ADJ_STR: GET_CHAR_INT(GetAdjStr, "$GET_CHAR_ADJ_STR()"); break;
                    case SUBOP_GET_CHAR_ADJ_STRMOD: GET_CHAR_INT(GetAdjStrMod, "$GET_CHAR_ADJ_STRMOD()"); break;
                    case SUBOP_GET_CHAR_ADJ_INT: GET_CHAR_INT(GetAdjInt, "$GET_CHAR_ADJ_INT()"); break;
                    case SUBOP_GET_CHAR_ADJ_WIS: GET_CHAR_INT(GetAdjWis, "$GET_CHAR_ADJ_WIS()"); break;
                    case SUBOP_GET_CHAR_ADJ_DEX: GET_CHAR_INT(GetAdjDex, "$GET_CHAR_ADJ_DEX()"); break;
                    case SUBOP_GET_CHAR_ADJ_CON: GET_CHAR_INT(GetAdjCon, "$GET_CHAR_ADJ_CON()"); break;
                    case SUBOP_GET_CHAR_ADJ_CHA: GET_CHAR_INT(GetAdjCha, "$GET_CHAR_ADJ_CHA()"); break;
                    case SUBOP_GET_CHAR_PERM_STR: GET_CHAR_INT(GetPermStr, "$GET_CHAR_PERM_STR()"); break;
                    case SUBOP_GET_CHAR_PERM_STRMOD: GET_CHAR_INT(GetPermStrMod, "$GET_CHAR_PERM_STRMOD()"); break;
                    case SUBOP_GET_CHAR_PERM_INT: GET_CHAR_INT(GetPermInt, "$GET_CHAR_PERM_INT()"); break;
                    case SUBOP_GET_CHAR_PERM_WIS: GET_CHAR_INT(GetPermWis, "$GET_CHAR_PERM_WIS()"); break;
                    case SUBOP_GET_CHAR_PERM_DEX: GET_CHAR_INT(GetPermDex, "$GET_CHAR_PERM_DEX()"); break;
                    case SUBOP_GET_CHAR_PERM_CON: GET_CHAR_INT(GetPermCon, "$GET_CHAR_PERM_CON()"); break;
                    case SUBOP_GET_CHAR_PERM_CHA: GET_CHAR_INT(GetPermCha, "$GET_CHAR_PERM_CHA()"); break;
                    case SUBOP_GET_CHAR_MAXENC: GET_CHAR_INT(GetAdjMaxEncumbrance, "$GET_CHAR_MAXENC()"); break;
                    case SUBOP_GET_CHAR_ENC: GET_CHAR_INT(GetEncumbrance, "$GET_CHAR_ENC()"); break;
                    case SUBOP_GET_CHAR_GENDER: GET_CHAR_ENUM(GetAdjGender, "$GET_CHAR_GENDER()"); break;
                    case SUBOP_GET_CHAR_CLASS: GET_CHAR_STRING(GetClass, "$GET_CHAR_CLASS()"); break;
                    case SUBOP_GET_CHAR_ALIGNMENT: GET_CHAR_ENUM(GetAdjAlignment, "$GET_CHAR_ALIGNMENT()"); break;
                    case SUBOP_GET_CHAR_STATUS: GET_CHAR_ENUM(GetAdjStatus, "$GET_CHAR_STATUS()"); break;
                    case SUBOP_GET_CHAR_UNDEAD: GET_CHAR_STRING(GetUndeadType, "$GET_CHAR_UNDEAD()"); break;
                    case SUBOP_GET_CHAR_SIZE: GET_CHAR_ENUM(GetAdjSize, "$GET_CHAR_SIZE()"); break;
                    case SUBOP_GET_CHAR_MAGICRESIST: GET_CHAR_INT(GetAdjMagicResistance, "$GET_CHAR_MAGICRESIST()"); break;
                    case SUBOP_GET_CHAR_Lvl:
                        {
                            m_popString1(); // class
#ifdef UAFEngine
                            {
                                CHARACTER * pChar;
                                BASECLASS_ID baseclassID;
                                baseclassID = m_string1;
                                pChar = m_popCharacter("$GET_CHAR_Lvl()");
                                //m_Integer1 = pChar->GetLevel(m_string1);
                                m_Integer1 = pChar -> GetCurrLevel(baseclassID);
                            }
#else
                            {
                                m_popInteger1();
                                m_Integer1 = 3;
                            };
#endif
                            m_pushInteger1();
                        };
                        break;


                    case SUBOP_GET_CHAR_NBRHITDICE: GET_CHAR_FLOAT(GetNbrHD, "$GET_CHAR_NBRHITDICE()"); break;
                    case SUBOP_GET_CHAR_NBRATTACKS: GET_CHAR_FLOAT(GetNbrAttacks, "$GET_CHAR_NBRATTACKS()"); break;
                    case SUBOP_GET_CHAR_MORALE: GET_CHAR_INT(GetAdjMorale, "$GET_CHAR_MORALE()"); break;
                    case SUBOP_GET_CHAR_DAMAGEBONUS: GET_CHAR_INT(GetAdjDmgBonus, "$GET_CHAR_DAMAGEBONUS()"); break;
                    case SUBOP_GET_CHAR_HITBONUS: GET_CHAR_INT(GetAdjHitBonus, "$GET_CHAR_HITBONUS()"); break;

                    case SUBOP_GET_CHAR_Ready:
                        {
                            DWORD rdyLoc;
                            m_popInteger2();  //The position in list
                            m_popString1();   //The location on body
                            m_popString2();   //The actor string
                            ActorType actor = m_StringToActor((LPCSTR)m_string2);
                            if (m_string1.IsEmpty()) rdyLoc = Cannot;
                            else rdyLoc = CString2Base38(m_string1);
                            CHARACTER * tdude = /* (CHARACTER*) * /GetCurrentlyActiveContext(& actor, "$GET_CHAR_Ready");
                            if (tdude == NULL) {
                                m_interpretError("Invalid target actor data in $GET_CHAR_Ready");
                                m_pushSP(m_false);
                                break;
                            };

                            //int index = tdude->myItems.GetReady((itemLocationType)m_Integer1);
                            int index = tdude -> myItems.GetReadiedItem(rdyLoc, m_Integer2);
                            if (index >= 0) {
                                //ITEM *pItem;
                                //GLOBAL_ITEM_ID giID = tdude->myItems.GetItem(index);
                                //m_string2 = itemData.GetItemUniqueName(giID);
                                ITEM_ID itemID = tdude -> myItems.GetItem(index);
                                m_string2 = itemData.GetItemUniqueName(itemID);
                            }
                            else {
                                m_string2 = "";
                            };
                            m_pushString2();
                        };
                        break;

                    case SUBOP_SET_CHAR_Ready:
                        {
                            m_popString1();  // item name
                            //m_popInteger1(); // body location index
                            m_popString3(); // body location index
                            m_Integer1 = CString2Base38(m_string3);
                            m_popString2();  // actor

                            ActorType actor = m_StringToActor((LPCSTR)m_string2);
                            CHARACTER * tdude = /* (CHARACTER*) * /GetCurrentlyActiveContext(& actor, "$SET_CHAR_Ready");
                            if (tdude == NULL) {
                                m_interpretError("Invalid target actor data in $SUBOP_SET_CHAR_Ready");
                                m_pushSP(m_false);
                                break;
                            };
                            ITEM_ID itemID;
                            itemID = m_string1;
                            tdude -> myItems.Ready(itemID, m_Integer1,/* tdude->GetItemMask(), * / tdude);
                        };
                        m_pushSP(m_false);
                        break;

                    case SUBOP_CASTSPELLONTARGETAS:
                        {
                            m_popString1(); // caster actor string
                            m_popString2(); // spell name
                            m_popString3(); // target actor string

                            ActorType actor = m_StringToActor((LPCSTR)m_string3);

                            CHARACTER * tdude = /* (CHARACTER*) * /GetCurrentlyActiveContext(& actor, "$CastSpellOnTargetAs()");
                            if (tdude == NULL) {
                                m_interpretError("Invalid target actor data in $CastSpellOnTargetAs()");
                                m_pushSP(m_false);
                                break;
                            }

                            actor = m_StringToActor((LPCSTR)m_string1);

                            CHARACTER * cdude = /*(CHARACTER*)* /GetCurrentlyActiveContext(& actor, "$CastSpellOnTargetAs");
                            if (cdude == NULL) {
                                m_interpretError("Invalid caster actor data in $CastSpellOnTargetAs()");
                                m_pushSP(m_false);
                                break;
                            }

                            //GLOBAL_SPELL_ID spelldbkey = spellData.FindName(m_string2);
                            SPELL_ID spellID;
                            spellID = m_string2;
                            //if (!spelldbkey.IsValidSpell())
                            if (!spellID.IsValidSpell()) {
                                CString err;
                                err.Format("Invalid spell name \'%s\' in $CastSpellOnTargetAs", m_string2);
                                m_interpretError(err);
                                m_pushSP(m_false);
                                break;
                            }

                            //SPELL_DATA *pSpell = spellData.GetSpellData(spelldbkey);
                            SPELL_DATA * pSpell = spellData.GetSpell(spellID);
                            if (pSpell == NULL) {
                                CString err;
                                err.Format("Failed spell db lookup for spell \'%s\' in $CastSpellOnTargetAs", m_string2);
                                m_interpretError(err);
                                m_pushSP(m_false);
                                break;
                            }

#ifdef UAFEngine
                            //cdude->InvokeSpellOnTarget(*tdude, -1, spelldbkey, NULL, NULL, NULL, false);
                            cdude -> InvokeSpellOnTarget(* tdude, -1, spellID, NULL, NULL, NULL, false, NULL);
#endif
                            m_pushSP(m_true);
                        }
                        break;

                    // cast spell on target using imaginary 30th level Cleric/MU
                    case SUBOP_CASTSPELLONTARGET:
                        {
                            m_popString1(); // spell name
                            m_popString2(); // target actor string

                            ActorType actor = m_StringToActor((LPCSTR)m_string2);
          

#ifdef UAFEngine
                            CHARACTER * tdude = /*(CHARACTER*)* /GetCurrentlyActiveContext(& actor, "$CastSpellOnTarget()");
                            if ((tdude == NULL) || (tdude == & FakeCharacter)) {
                                m_interpretError("Invalid target actor data in $CastSpellOnTarget()");
                                m_pushSP(m_false);
                                break;
                            }
#endif

                            //GLOBAL_SPELL_ID spelldbkey = spellData.FindName(m_string1);
                            SPELL_ID spellID;
                            spellID = m_string1;
                            //if (!spelldbkey.IsValidSpell())
                            if (!spellID.IsValidSpell()) {
                                CString err;
                                err.Format("Invalid spell name \'%s\' in $CastSpellOnTarget()", m_string1);
                                m_interpretError(err);
                                m_pushSP(m_false);
                                break;
                            }

                            //SPELL_DATA *pSpell = spellData.GetSpellData(spelldbkey);
                            SPELL_DATA * pSpell = spellData.GetSpell(spellID);
                            if (pSpell == NULL) {
                                CString err;
                                err.Format("Failed spell db lookup for spell \'%s\' in $CastSpellOnTarget()", m_string1);
                                m_interpretError(err);
                                m_pushSP(m_false);
                                break;
                            }

#ifdef UAFEngine
                            //CHARACTER FakeCharacter;
                            // create high-level spell caster
                            //
                            // create Cleric/MU so either spell class can be cast
                            //
                            // when other spell classes are added, this will need to be 
                            // changed to create specific char classes (can't have cl/mu/dr/rang/etc
                            // as valid char class.
                            //


                            DWORD StartExp;
/*
          StartExp = 2 + 
                     GetMinClericExpForLevel(29) +
                     GetMinMagicUserExpForLevel(29);
* / StartExp = 0; // Not Implemented(0x9e5a235, false);

                            FakeCharacter.SetAlignment(ChaoticNeutral);
                            //FakeCharacter.race(Human);
                            RACE_ID raceID;
                            raceID = "Human";
                            FakeCharacter.RaceID(raceID);
                            FakeCharacter.SetGender(Male);
                            CLASS_ID classID;
                            classID = "Fighter";
                            FakeCharacter.SetClass(classID); // 
                            //FakeCharacter.SetClass(C_MU); // 
                            FakeCharacter.SetType(FAKE_CHARACTER_TYPE);
                            FakeCharacter.SetName("TempGPDLClericMU");
                            FakeCharacter.generateNewCharacter(StartExp, START_EXP_VALUE);
                            FakeCharacter.SetPermStr(18);
                            FakeCharacter.SetPermInt(18);
                            FakeCharacter.SetPermWis(18);
                            FakeCharacter.SetPermDex(18);
                            FakeCharacter.SetPermCon(18);
                            FakeCharacter.SetPermCha(18);
                            FakeCharacter.InvokeSpellOnTarget(* tdude, -1, spellID, NULL, NULL, NULL, false, NULL);
#else
                            m_pushSP(m_true); // editor only
#endif
                        }
                        break;

                    case SUBOP_CHAR_REMOVEALLSPELLS:
                        {
                            m_popString1(); // spell level
                            m_popString2(); // actor string

                            ActorType actor = m_StringToActor((LPCSTR)m_string2);

                            CHARACTER * dude = /*(CHARACTER*)* /GetCurrentlyActiveContext(& actor, "$CHAR_REMOVEALLSPELLS()");
                            if (dude == NULL) {
                                m_interpretError("Invalid actor data in $CHAR_REMOVEALLSPELLS()");
                                m_pushSP(m_false);
                                break;
                            }

                            int count = 0;
#ifdef UAFEngine
                            int spelllvl = atoi(m_string1);

                            for (int j = spelllvl; j > 0; j--)
                            {
                                count += dude -> RemoveAllSpellEffectsByLevel(j);
                            };
#endif
                            m_string3.Format("%i", count);
                            m_pushSP(m_string3);
                        }
                        break;
                    /*
                          case SUBOP_CHAR_REMOVESPELL:
                            {
                              m_popString1(); // spell name
                              m_popString2(); // target actor string
                    
                              ActorType actor = m_StringToActor((LPCSTR)m_string2);
                              
                              CHARACTER *dude = (CHARACTER*)GetCurrentlyActiveContext(&actor);          
                              if (dude==NULL)
                              {
                                m_interpretError("Invalid actor data in $SUBOP_CHAR_REMOVESPELL");
                                m_pushSP(m_false);
                                break;
                              }
                    
                              BOOL wasremoved=FALSE;
                    #ifdef UAFEngine          
                              wasremoved = dude->RemoveSpellByName(m_string1);
                    #endif
                              m_string3.Format("%i", (wasremoved?TRUE:FALSE));
                              m_pushSP(m_string3);
                            }
                            break;
                    * /
                    case SUBOP_CHAR_DISPELMAGIC:
                        {
                            m_popString1(); // spell level
                            m_popString2(); // actor string

                            ActorType actor = m_StringToActor((LPCSTR)m_string2);

                            CHARACTER * dude = /*(CHARACTER*)* /GetCurrentlyActiveContext(& actor, "$CHAR_DISPELMAGIC()");
                            if (dude == NULL) {
                                m_interpretError("Invalid actor data in $CHAR_DISPELMAGIC()");
                                m_pushSP(m_false);
                                break;
                            }

                            int count = 0;
#ifdef UAFEngine
                            int spelllvl = atoi(m_string1);

                            count += dude -> DispelSpellEffectsByLevel(spelllvl);
#endif
                            m_string3.Format("%i", count);
                            m_pushSP(m_string3);
                        }
                        break;

                    case SUBOP_CHAR_REMOVEALLITEMCURSE:
                        {
                            m_popString1(); // actor string

                            ActorType actor = m_StringToActor((LPCSTR)m_string1);

                            CHARACTER * dude = /*(CHARACTER*)* /GetCurrentlyActiveContext(& actor, "$CHAR_REMOVEALLITEMCURSE()");
                            if (dude == NULL) {
                                m_interpretError("Invalid actor data in $CHAR_REMOVEALLITEMCURSE()");
                                m_pushSP(m_false);
                                break;
                            }

#ifdef UAFEngine
                            dude -> ClearAllItemCursedFlags();
#endif
                            m_pushSP(m_true);
                        }
                        break;
                    case SUBOP_DAT_Class_Baseclasses:
                        {
                            m_popString1();
                            m_string2.Empty();
                            CLASS_ID classID;
                            classID = m_string1;
                            const CLASS_DATA * pClass;
                            pClass = classData.PeekClass(classID);
                            if (pClass != NULL) {
                                int j, n;
                                n = pClass -> GetCount();
                                for (j = 0; j < n; j++) {
                                    const BASECLASS_ID * pBaseclassID;
                                    pBaseclassID = pClass -> PeekBaseclassID(j);
                                    m_string2 += '$';
                                    m_string2 += * pBaseclassID;
                                };
                            };
                            m_pushString2();
                        };
                        break;
                    case SUBOP_DAT_Baseclass_Level:
                        {
                            m_popInteger1();
                            m_popString1();
                            BASECLASS_ID baseclassID;
                            baseclassID = m_string1;
                            //m_Integer2 = baseclassData.GetLevel(m_string1, m_Integer1);
                            m_Integer2 = baseclassData.GetLevel(baseclassID, m_Integer1);
                            m_pushInteger2();
                        };
                        break;
                    case SUBOP_DAT_Baseclass_Experience:
                        {
                            m_popInteger1();
                            m_popString1();
                            BASECLASS_ID baseclassID;
                            baseclassID = m_string1;
                            //m_Integer2 = baseclassData.GetMinExpForLevel(m_string1, m_Integer1);
                            m_Integer2 = baseclassData.GetMinExpForLevel(baseclassID, m_Integer1);
                            m_pushInteger2();
                        };
                        break;
                    case SUBOP_GET_CHAR_SEX:
                        {
                            CHARACTER * pChar;
                            pChar = Dude("$GET_CHAR_SEX");
                            m_string1 = GenderNames[pChar -> GetAdjGender()];
                            m_pushString1();
                        };
                        break;
                    /*   ******* Replaced with definition that uses uniqueID of character rather than party index
                          case SUBOP_SET_CHAR_SEX:
                            m_popString1();  // new sex
                            m_popInteger1(); // id
                            
                    #ifdef UAFEngine
                            if (m_Integer1 == -2)
                            {
                              CHARACTER *pChar;
                              pChar = pScriptContext->GetCreatedCharacterContext("$GET_CHAR_SEX()");
                              if (pChar == NULL)
                              {
                                m_interpretError("Illegal characer Index in $GET_CHAR_SEX");
                              }
                              else
                              {
                                pChar->SetGender(m_string1);
                              };
                              break;
                            }
                            else if (IsCombatActive())
                            {
                              if (   (m_Integer1<0) 
                                  || (m_Integer1>=GetNumCombatants())) 
                              {
                                m_interpretError("Illegal party member # in $SET_CHAR_SEX");
                                break;
                              };
                              SetCombatantGender(m_Integer1, m_string1);
                            }
                            else
                            {          
                              if (   (m_Integer1<0) 
                                  || (m_Integer1>=GetPartySize(party))) 
                              {
                                m_interpretError("Illegal party member # in $SET_CHAR_SEX");
                                break;
                              };
                              SetCharacterGender(party, m_Integer1, m_string1);
                            }
                    #endif
                            m_pushString1();
                            break;
                    * /
                    case SUBOP_GET_CHAR_TYPE:
                        {
                            int type;
                            CHARACTER * pDude = Dude("$GET_CHAR_TYPE()");
                            type = pDude -> GetType();
                            switch (type) {
                                case 1: m_string1 = "@PC@"; break;
                                case 2: m_string1 = "@NPC@"; break;
                                case 3:
                                    {
                                        ////const MONSTER_DATA *pMonsterData;
                                        //if (pDude==NULL) return "";
                                        ////pMonsterData = monsterData.PeekMonster(pDude->monsterID);
                                        ////if (pMonsterData == NULL) return "";
                                        ////return pMonsterData->Name;
                                        m_string1 = pDude -> monsterID; break;
                                    };
                                default: m_string1 = "";
                            };
                        };
                        m_pushString1();
                        break;
                    case SUBOP_GET_GLOBAL_ASL:
                        m_popString1(); // Get key.
                        m_string2 = LookupGlobalASL(globalData, m_string1);
                        m_pushSP(m_string2);
                        break;
                    case SUBOP_GET_PARTY_ASL:
                        m_popString1(); // Get key.
                        m_string2 = LookupPartyASL(party, m_string1);
                        m_pushSP(m_string2);
                        break;

                    case SUBOP_GET_VAULT_MONEYAVAILABLE:
                        {
                            m_popInteger1();

                            double total = globalData.VaultTotal();

                            if (m_Integer1 == 0) // no conversion
                            {
                                m_Integer2 = (int)total;
                            }
                            else {
                                if ((m_Integer1 >= 1) && (m_Integer1 <= 10)) {
                                    m_Integer2 =
                                        (int)globalData.moneyData.Convert(total,
                                            globalData.moneyData.GetBaseType(),
                                            globalData.moneyData.GetItemClass(m_Integer1 - 1));
                                }
                                else
                                    m_Integer2 = 0;
                            }
                            m_pushInteger2();
                        }
                        break;

                    case SUBOP_GET_PARTY_MONEYAVAILABLE:
                        {
                            m_popInteger1();

                            double total = 0;
                            for (int j = 0; j < party.numCharacters; j++)
                            total += party.characters[j].money.Total();

                            if (m_Integer1 == 0) // no conversion
                            {
                                m_Integer2 = (int)total;
                            }
                            else {
                                if ((m_Integer1 >= 1) && (m_Integer1 <= 10)) {
                                    m_Integer2 =
                                        (int)globalData.moneyData.Convert(total,
                                            globalData.moneyData.GetBaseType(),
                                            globalData.moneyData.GetItemClass(m_Integer1 - 1));
                                }
                                else
                                    m_Integer2 = 0;
                            }
                            m_pushInteger2();
                        }
                        break;

                    case SUBOP_GIVE_CHAR_ITEM:
                        {
                            m_popString1(); // item name
                            m_popString2(); // actor string
                            ActorType actor = m_StringToActor((LPCSTR)m_string2);
                            CHARACTER * tdude = /*(CHARACTER*)* /GetCurrentlyActiveContext(& actor, "$GIVE_CHAR_ITEM()");
                            if (tdude == NULL) {
                                m_string2 = m_ActorToString(& NULL_ACTOR);
                                m_pushString2();
                                break;
                            }
                            {
                                //GLOBAL_ITEM_ID giID = itemData.FindName(m_string1);
                                ITEM_ID itemID;
                                itemID = m_string1;
                                //itemData.FindItemID(itemID);
                                itemData.LocateItem(itemID);
                                //BOOL success = dude->addCharacterItem(giID, 
                                BOOL success = tdude -> addCharacterItem(itemID,
                                    //itemData.GetItemBundleQty(giID), 
                                    itemData.GetItemBundleQty(itemID),
                                    -1, // default charges
                                    TRUE, // is identified
                                    -1); // no cost
                                m_pushSP(success ? m_true : m_false);
                            }

                        }
                        break;

                    case SUBOP_TAKE_CHAR_ITEM:
                        {
                            m_popString1(); // item name
                            m_popString2(); // actor string
                            ActorType actor = m_StringToActor((LPCSTR)m_string2);
                            CHARACTER * tdude = /*(CHARACTER*)* /GetCurrentlyActiveContext(& actor, "$TAKE_CHAR_ITEM()");
                            if (tdude == NULL) {
                                m_string2 = m_ActorToString(& NULL_ACTOR);
                                m_pushString2();
                                break;
                            }

                            //GLOBAL_ITEM_ID giID = itemData.FindName(m_string1);
                            ITEM_ID itemID;
                            itemID = m_string1;
                            //itemData.FindItemID(itemID);
                            itemData.LocateItem(itemID);
                            BOOL success = FALSE;

                            POSITION pos = tdude -> myItems.GetHeadPosition();
                            POSITION itemPos = NULL;
                            while (pos != NULL)  // First we go through the items.  The item with the given
                            // name that is also the ItemContext() will take precedence.
                            {
                                const ITEM * pItem;
                                pItem = & tdude -> myItems.PeekAtPos(pos);
                                //if (pItem->m_giID == giID)
                                if (pItem -> itemID == itemID) {
                                    if (itemPos == NULL) {
                                        itemPos = pos;
                                    };
                                    if (pItem -> key == pScriptContext -> GetItemContextKey()) {
                                        itemPos = pos;  // Override any others
                                        break;  // No sense searching further
                                    };
                                };
                                tdude -> myItems.GetNext(pos);
                            };
                            if (itemPos != NULL) {
                                success = tdude -> delCharacterItem(tdude -> myItems.GetKeyAt(itemPos),
                                    //itemData.GetItemBundleQty(giID));
                                    itemData.GetItemBundleQty(itemID));
                            }
                            m_pushSP(success ? m_true : m_false);
                        }
                        break;

                    case SUBOP_GREATER:
                        m_popString1();
                        m_popString2();
                        m_pushSP(m_string2 > m_string1 ? m_true : m_false);
                        break;
                    case SUBOP_GREATEREQUAL:
                        m_popString1();
                        m_popString2();
                        m_pushSP(m_string2 >= m_string1 ? m_true : m_false);
                        break;
                    case SUBOP_SpellAdj:
                        {
                            CHARACTER * pChar;
                            m_popInteger4(); // The bonus
                            m_popInteger3(); // The percent
                            m_popInteger2(); // The last level
                            m_popInteger1(); // The first level
                            m_popString2();  // The Identification
                            m_popString1();  // The SCHOOL_ID
                            pChar = m_popCharacterActor();
#ifdef UAFEngine
                            if (pChar != NULL) {
                                m_string1 = pChar -> SpellAdjustment(
                                    m_string1,  // schoolID
                                    m_string2,  // identification
                                    m_Integer1, // first level
                                    m_Integer2, // last level
                                    m_Integer3, // percent
                                    m_Integer4  // bonus
                                );
                            };
#endif
                            m_pushString1();
                        };
                        break;
                    case SUBOP_SkillAdj:
                        {
                            CHARACTER * pChar;
                            m_Integer2 = 0;
                            m_popInteger1();
                            m_popString3();
                            m_popString2();
                            m_popString1();
                            pChar = m_popCharacterActor();
#ifdef UAFEngine
                            m_string1 = m_SkillAdjustment(
                                pChar,
                                m_string1,
                                m_string2,
                                m_string3,
                                m_Integer1);
#endif
                            m_pushString1();
                        };
                        break;
                    case SUBOP_GREP:
                        // bool GREP (pattern, string)
                        re_Set_EGrep(); // Set egrep mode
                        re_searchString = m_popSP();   // The string to search.
                        m_popString2();   // The pattern
                        re_searchString.MakeUpper();
                        m_string2 = upperPattern(m_string2);
                        m_grepCResult = re_compile(LPCTSTR(m_string2), & m_grepPat);
                        if (m_grepCResult != NULL) {
                            if (m_debugLevel && DBG_messageBox) {
                                CString msg;
                                msg = "Error compiling Grep pattern\n";
                                msg += m_string2;
                                msg += "\n";
                                msg += CString(m_grepCResult);
                                MsgBoxError(msg);
                            };
                            INTERP_RETURN(GPDL_GREPERROR);
                        };
                        m_Integer1 = re_search(& m_grepPat,
                            LPCTSTR(re_searchString),
                            re_searchString.GetLength(),
                            0,
                            re_searchString.GetLength(),
                   & re_grepReg);
                        m_pushSP(m_Integer1 < 0 ? m_false : m_true);
                        break;
                    case SUBOP_Wiggle:
                        m_popInteger1();
                        if (((unsigned)m_Integer1 > re_grepReg.num_regs)
              || (re_grepReg.start[(unsigned)m_Integer1] < 0))
                        {
                            m_string1 = "";
                        }
        else
                        {
                            m_string1 = re_searchString.Mid(re_grepReg.start[m_Integer1], re_grepReg.end[m_Integer1] - re_grepReg.start[m_Integer1]);
                        }
                        m_pushString1();
                        break;
                    case SUBOP_HitPoints:
                        m_popString1();
                        m_string2 = m_HitPoints(m_string1);
                        m_pushString2();
                        break;
                    case SUBOP_IF_PARTY_ASL:
                        m_popString1(); // Get key.
                        if (ExistsPartyASL(party, m_string1))
                            m_pushSP(m_true);
                        else
                            m_pushSP(m_false);
                        break;
                    case SUBOP_IF_CHAR_ASL:
                        {
                            CHARACTER * pChar;
                            m_popString1();   //  ASL Key
                            // Replaced with code that uses UniqueID rather than party index
                            //m_popInteger1();
                            //if (   (m_Integer1<0) 
                            //    || (m_Integer1>=GetPartySize(party))) 
                            //{
                            //m_interpretError("Illegal party member # in $GET_CHAR_ASL");
                            //break;
                            //};
                            //m_string2 = LookupCharacterASL(
                            //party,
                            //m_Integer1,
                            //m_string1);
                            //m_pushString2();
                            pChar = Dude("$IF_CHAR_ASL()");
                            m_string2 = LookupCharacterASL(pChar, m_string1);
                            m_pushString2();
                        };
                        break;
                    case SUBOP_IS_AFFECTED_BY_SPELL:
                        {
                            SPELL_ID spellID;
                            m_popString1(); // SpellName
                            m_popString2(); // ActorType
                            spellID = m_string1;
                            m_string3 = m_IsCharAffectedBySpell(m_string2, spellID);
                            m_pushString3();
                        };
                        break;

                    case SUBOP_IS_AFFECTED_BY_SPELL_ATTR:
                        m_popString1(); // SpellName
                        m_popString2(); // ActorType
                        m_string3 = m_IsCharAffectedBySpellAttribute(m_string2, m_string1);
                        m_pushString3();
                        break;

                    case SUBOP_IndexOf:
                        m_popString1();
                        m_string2 = m_IndexOf(m_string1);
                        m_pushString2();
                        break;
                    case SUBOP_IndexToActor:
                        m_string2 = m_IndexToActor();
                        m_pushString2();
                        break;
                    case SUBOP_InParty:
                        m_popString1();
                        m_string2 = m_InParty(m_string1);
                        m_pushString2();
                        break;
                    case SUBOP_LAND:
                        m_popString1();
                        m_popString2();
                        m_pushSP(((m_string1.GetLength() != 0) && m_string2.GetLength() != 0) ?
                            m_true : m_false);
                        break;

                    case SUBOP_LAST_ATTACKER_OF:
                        {
#ifdef UAFEngine
                            if (!IsCombatActive()) {
                                m_string2 = m_ActorToString(& NULL_ACTOR);
                                m_pushString2();
                                break;
                            }

                            m_popString1();
                            ActorType actor = m_StringToActor((LPCSTR)m_string1);
#ifdef newCombatant
                            COMBATANT * dude = GetCurrentlyActiveContext(& actor, "$LAST_ATTACKER_OF()") -> m_pCombatant;
#else
                            COMBATANT * dude = (COMBATANT *)GetCurrentlyActiveContext(& actor);
#endif

                            if (dude == NULL) {
                                m_string2 = m_ActorToString(& NULL_ACTOR);
                                m_pushString2();
                                break;
                            }

                            COMBATANT * pLastAttacker = GetLastAttackerOf(dude);
                            if (pLastAttacker != NULL) {
                                pLastAttacker -> GetContext(& actor);
                                m_string2 = m_ActorToString(& actor);
                            }
                            else
                                m_string2 = m_ActorToString(& NULL_ACTOR);
                            m_pushString2();         
#else
                            m_string2 = m_ActorToString(& NULL_ACTOR);
                            m_pushString2();                      
#endif
                        }
                        break;

                    case SUBOP_LENGTH:
                        m_popString1();
                        m_uInteger1 = m_string1.GetLength();
                        m_pushUInteger1();
                        break;
                    case SUBOP_LESS:
                        m_popString1();
                        m_popString2();
                        m_pushSP(m_string2 < m_string1 ? m_true : m_false);
                        break;
                    case SUBOP_LESSEQUAL:
                        m_popString1();
                        m_popString2();
                        m_pushSP(m_string2 <= m_string1 ? m_true : m_false);
                        break;

                    case SUBOP_LEAST_DAMAGED_FRIENDLY:
                        {
#ifdef UAFEngine
                            if (!IsCombatActive()) {
                                m_string2 = m_ActorToString(& NULL_ACTOR);
                                m_pushString2();
                                break;
                            }

                            COMBATANT * pLastAttacker = GetLeastDamagedFriendly();
                            if (pLastAttacker != NULL) {
                                ActorType actor;
                                pLastAttacker -> GetContext(& actor);
                                m_string2 = m_ActorToString(& actor);
                            }
                            else
                                m_string2 = m_ActorToString(& NULL_ACTOR);
                            m_pushString2();         
#else
                            m_string2 = m_ActorToString(& NULL_ACTOR);
                            m_pushString2();                      
#endif
                        }
                        break;
                    case SUBOP_LEAST_DAMAGED_ENEMY:
                        {
#ifdef UAFEngine
                            if (!IsCombatActive()) {
                                m_string2 = m_ActorToString(& NULL_ACTOR);
                                m_pushString2();
                                break;
                            }

                            COMBATANT * pLastAttacker = GetLeastDamagedEnemy();
                            if (pLastAttacker != NULL) {
                                ActorType actor;
                                pLastAttacker -> GetContext(& actor);
                                m_string2 = m_ActorToString(& actor);
                            }
                            else
                                m_string2 = m_ActorToString(& NULL_ACTOR);
                            m_pushString2();         
#else
                            m_string2 = m_ActorToString(& NULL_ACTOR);
                            m_pushString2();                      
#endif
                        }
                        break;

                    case SUBOP_LISTEN:
                        if (m_pGPDLevent == NULL) {
                            m_pushEmptyString();
                            break;
                        };
                        m_discourseText.StartInput();
                        m_state = GPDL_WAIT_INPUT;
                        INTERP_RETURN(GPDL_WAIT_INPUT);
                    case SUBOP_LOGIC_BLOCK_VALUE:
                        m_popString1();
                        m_string2 = LookupGlobalASL(globalData, "LOGICBLOCKVALUES");
                        if ((m_string2 == "") || (m_string1 == "")) {
                            m_string1 = "";
                        }
                        else {
                            int index = 0;
                            unsigned int k;
                            unsigned int len = m_string2.GetLength();
                            k = m_string1[0] - 'A';
                            if ((k < 0) || (k > 11)) {
                                m_string1 = "";
                                m_pushString1();
                                break;
                            };
                            for (i = 0; i <= k; i++) {
                                if (len < sizeof(int)) {
                                    m_string1 = "";
                                    break;
                                };
                                k = * (int *)((LPCTSTR)(m_string2) + index);
                                index += sizeof(int);
                                len -= sizeof(int);
                                if (len < k) {
                                    m_string1 = "";
                                    break;
                                };
                                if (i != k) {
                                    index += k;
                                    len -= k;
                                    continue;
                                };
                                m_string1 = CString((LPCTSTR)m_string2.Mid(index, k));
                            };
                        };
                        m_pushString1();
                        break;
                    case SUBOP_Myself:
                        m_string1 = m_Myself();
                        m_pushString1();
                        break;
                    case SUBOP_NextCreatureIndex:
                        m_popInteger1();  // The filter
#ifdef UAFEngine
                        if (IsCombatActive()) {
                            if (m_dataStack[m_SP].IsEmpty()) {
                                m_popInteger2();
                            }
                            else {
                                m_popInteger2();
                                m_Integer2++;
                            };
                            for (; m_Integer2 < combatData.NumCombatants(); m_Integer2++) {
                                COMBATANT * pCOMBATANT;
                                pCOMBATANT = combatData.GetCombatant(m_Integer2);
                                if (m_Integer1 & 1) {
                                    if (!pCOMBATANT -> IsAlive()) continue;
                                };
                                if (m_Integer1 & 2) {
                                    if (pCOMBATANT -> friendly) continue;
                                };
                                if (m_Integer1 & 4) {
                                    if (!pCOMBATANT -> friendly) continue;
                                };
                                if (m_Integer1 & 8) {
                                    if (!pCOMBATANT -> charOnCombatMap(false, true)) continue;
                                };
                                break;
                            };
                            if (m_Integer2 >= combatData.NumCombatants()) {
                                m_string1 = "";
                                m_pushString1();
                            }
                            else {
                                m_pushInteger2();
                            };
                        }
                        else {
          /* Really * / NotImplemented(0xc43109, FALSE);
                        };
#else
                        m_string1 = "";
                        m_pushString1();
#endif
                        break;
                    case SUBOP_CharacterContext:
                        m_string1 = pScriptContext -> GetCharacterContext("$CharacterContext() called when no character context exists");
                        m_pushString1();
                        break;
                    case SUBOP_ItemContext:
                        m_string1 = pScriptContext -> GetItemContext("$ItemContext() called when no item context exists") -> UniqueName();
                        m_pushString1();
                        break;
                    case SUBOP_ClassContext:
                        m_string2 = pScriptContext -> GetClassContext("$ClassContext() called when no class context exists") -> m_name;
                        m_pushString2();
                        break;
                    case SUBOP_RaceContext:
                        m_string2 = pScriptContext -> GetRaceContext("$RaceContext() called when no race context exists") -> m_name;
                        m_pushString2();
                        break;
                    case SUBOP_SpellContext:
                        {
#ifdef UAFEngine
                            m_string1 = pScriptContext -> GetSpellContext("$SpellContext() called when no spell context exists") -> uniqueName;
#else
                            m_string1 = pScriptContext -> GetSpellContext(NULL) -> Name;
#endif
                            m_pushString1();
                        };
                        break;
                    case SUBOP_MonsterTypeContext:
                        {
#ifdef UAFEngine
                            m_string1 = pScriptContext -> GetMonsterTypeContext("$MonsterTypeContext() called when no monster type context exists") -> monsterID;
#else
                            m_string1 = pScriptContext -> GetMonsterTypeContext(NULL) -> MonsterID();
#endif
                            m_pushString1();
                        };
                        break;
                    //case SUBOP_GetSpellName:
                    //  {
                    //    GLOBAL_SPELL_ID gsID;
                    //    m_popString1();
                    //    gsID = m_Spell(m_string1);
                    //    m_string2 = spellData.GetSpellName(gsID);
                    //    m_pushString2();
                    //  };
                    //  break;
                    case SUBOP_GET_SPELL_Level:
                        {
                            SPELL_ID spellID;
                            const SPELL_DATA * pSpell;
                            m_popString1();
                            spellID = m_string1;
                            m_Integer1 = 0;
                            pSpell = spellData.PeekSpell(spellID);
                            if (pSpell != NULL) {
                                m_Integer1 = pSpell -> Level;
                            };
                            m_pushInteger1();
                        };
                        break;
                    case SUBOP_GET_SPELL_CanBeDispelled:
                        {
                            SPELL_ID spellID;
                            const SPELL_DATA * pSpell;
                            m_popString1();
                            spellID = m_string1;
                            m_string2.Empty();
                            pSpell = spellData.PeekSpell(spellID);
                            if (pSpell != NULL) {
                                m_string2 = pSpell -> CanBeDispelled ? "1" : "";
                            };
                            m_pushString2();
                        };
                        break;
                    case SUBOP_nAND:
                        m_popInteger1();
                        m_popInteger2();
                        m_Integer2 &= m_Integer1;
                        m_pushInteger2();
                        break;
                    case SUBOP_LOR:
                        m_popString1();
                        m_popString2();
                        m_pushSP(((m_string1.GetLength() != 0) || (m_string2.GetLength() != 0)) ?
                            m_true : m_false);
                        break;
                    case SUBOP_nISEQUAL:
                        m_popInteger1();
                        m_popInteger2();
                        m_pushSP(m_Integer1 == m_Integer2 ? m_true : m_false);
                        break;
                    case SUBOP_IsUndead:
                        m_popString1();
                        m_string2 = m_IsUndead(m_string1);
                        m_pushString2();
                        break;
                    case SUBOP_MIDDLE:
                        m_popInteger2(); //count
                        m_popInteger1(); //first index
                        m_popString1();
                        m_string2 = m_string1.Mid(m_Integer1, m_Integer2);
                        m_pushString2();
                        break;

                    case SUBOP_MOST_DAMAGED_FRIENDLY:
                        {
#ifdef UAFEngine
                            if (!IsCombatActive()) {
                                m_string2 = m_ActorToString(& NULL_ACTOR);
                                m_pushString2();
                                break;
                            }

                            COMBATANT * pLastAttacker = GetMostDamagedFriendly();
                            if (pLastAttacker != NULL) {
                                ActorType actor;
                                pLastAttacker -> GetContext(& actor);
                                m_string2 = m_ActorToString(& actor);
                            }
                            else
                                m_string2 = m_ActorToString(& NULL_ACTOR);
                            m_pushString2();         
#else
                            m_string2 = m_ActorToString(& NULL_ACTOR);
                            m_pushString2();                      
#endif
                        }
                        break;
                    case SUBOP_MOST_DAMAGED_ENEMY:
                        {
#ifdef UAFEngine
                            if (!IsCombatActive()) {
                                m_string2 = m_ActorToString(& NULL_ACTOR);
                                m_pushString2();
                                break;
                            }

                            COMBATANT * pLastAttacker = GetMostDamagedEnemy();
                            if (pLastAttacker != NULL) {
                                ActorType actor;
                                pLastAttacker -> GetContext(& actor);
                                m_string2 = m_ActorToString(& actor);
                            }
                            else
                                m_string2 = m_ActorToString(& NULL_ACTOR);

                            m_pushString2();         
#else
                            m_string2 = m_ActorToString(& NULL_ACTOR);
                            m_pushString2();                      
#endif
                        }
                        break;

                    case SUBOP_nGREATER:
                        m_popInteger1();
                        m_popInteger2();
                        m_pushSP(m_Integer2 > m_Integer1 ? m_true : m_false);
                        break;
                    case SUBOP_nGREATEREQUAL:
                        m_popInteger1();
                        m_popInteger2();
                        m_pushSP(m_Integer2 >= m_Integer1 ? m_true : m_false);
                        break;
                    case SUBOP_nLESS:
                        m_popInteger1();
                        m_popInteger2();
                        m_pushSP(m_Integer2 < m_Integer1 ? m_true : m_false);
                        break;
                    case SUBOP_nMINUS:
                        m_popInteger1();
                        m_popInteger2();
                        m_Integer2 -= m_Integer1;
                        m_pushInteger2();
                        break;
                    case SUBOP_Name:
                        m_popString1();
                        m_string2 = m_Name(m_string1);
                        m_pushString2();
                        break;

                    case SUBOP_NEAREST_TO:
                        {
#ifdef UAFEngine
                            if (!IsCombatActive()) {
                                m_string2 = m_ActorToString(& NULL_ACTOR);
                                m_pushString2();
                                break;
                            }

                            m_popString1();
                            ActorType actor = m_StringToActor((LPCSTR)m_string1);
#ifdef newCombatant
                            COMBATANT * dude = GetCurrentlyActiveContext(& actor, "$NEAREST_TO()") -> m_pCombatant;
#else
                            COMBATANT * dude = (COMBATANT *)GetCurrentlyActiveContext(& actor);
#endif

                            if (dude == NULL) {
                                m_string2 = m_ActorToString(& NULL_ACTOR);
                                m_pushString2();
                                break;
                            }

                            COMBATANT * pLastAttacker = GetNearestTo(dude);
                            if (pLastAttacker != NULL) {
                                pLastAttacker -> GetContext(& actor);
                                m_string2 = m_ActorToString(& actor);
                            }
                            else
                                m_string2 = m_ActorToString(& NULL_ACTOR);
                            m_pushString2();         
#else
                            m_string2 = m_ActorToString(& NULL_ACTOR);
                            m_pushString2();                      
#endif
                        }
                        break;
                    case SUBOP_NEAREST_ENEMY_TO:
                        {
#ifdef UAFEngine
                            if (!IsCombatActive()) {
                                m_string2 = m_ActorToString(& NULL_ACTOR);
                                m_pushString2();
                                break;
                            }

                            m_popString1();
                            ActorType actor = m_StringToActor((LPCSTR)m_string1);
#ifdef newCombatant
                            COMBATANT * dude = GetCurrentlyActiveContext(& actor, "$NEAREST_ENEMY_TO()") -> m_pCombatant;
#else
                            COMBATANT * dude = (COMBATANT *)GetCurrentlyActiveContext(& actor);
#endif

                            if (dude == NULL) {
                                m_string2 = m_ActorToString(& NULL_ACTOR);
                                m_pushString2();
                                break;
                            }

                            COMBATANT * pLastAttacker = GetNearestEnemyTo(dude);
                            if (pLastAttacker != NULL) {
                                pLastAttacker -> GetContext(& actor);
                                m_string2 = m_ActorToString(& actor);
                            }
                            else
                                m_string2 = m_ActorToString(& NULL_ACTOR);
                            m_pushString2();         
#else
                            m_string2 = m_ActorToString(& NULL_ACTOR);
                            m_pushString2();                      
#endif
                        }
                        break;

                    case SUBOP_nLESSEQUAL:
                        m_popInteger1();
                        m_popInteger2();
                        m_pushSP(m_Integer2 <= m_Integer1 ? m_true : m_false);
                        break;
                    case SUBOP_NOOP:
                        break;
                    case SUBOP_nNEGATE:
                        m_popInteger1();
                        m_Integer2 = -m_Integer1;
                        m_pushInteger2();
                        break;
                    case SUBOP_nNOTEQUAL:
                        m_popInteger1();
                        m_popInteger2();
                        m_pushSP(m_Integer1 != m_Integer2 ? m_true : m_false);
                        break;
                    case SUBOP_nOR:
                        m_popInteger1();
                        m_popInteger2();
                        m_Integer2 |= m_Integer1;
                        m_pushInteger2();
                        break;
                    case SUBOP_NOT:
                        if (m_dataStack[m_SP] == m_false) m_dataStack[m_SP] = m_true;
                        else m_dataStack[m_SP] = m_false;
                        break;
                    case SUBOP_NOTEQUAL:
                        m_popString1();
                        m_popString2();
                        m_pushSP(m_string1 == m_string2 ? m_false : m_true);
                        break;
                    case SUBOP_nPERCENT:
                        m_popInteger1();
                        m_popInteger2();
                        m_Integer2 %= m_Integer1;
                        m_pushInteger2();
                        break;
                    case SUBOP_nPLUS:
                        m_popInteger1();
                        m_popInteger2();
                        m_Integer2 += m_Integer1;
                        m_pushInteger2();
                        break;
                    case SUBOP_nSLASH:
                        m_popInteger1();
                        m_popInteger2();
                        if (m_Integer1 == 0) {
                            WriteDebugString("Attempt to divide (/#) by zero ");
                            m_Integer1 = 1;
                        };
                        m_Integer2 /= m_Integer1;
                        m_pushInteger2();
                        break;
                    case SUBOP_nTIMES:
                        m_popInteger1();
                        m_popInteger2();
                        m_Integer2 *= m_Integer1;
                        m_pushInteger2();
                        break;
                    case SUBOP_NUMERIC:
                        if (m_popInteger(m_bool1, m_uInteger1) == GPDL_OK)
                            m_pushSP(m_true);
                        else m_pushSP(m_false);
                        break;
                    case SUBOP_nXOR:
                        m_popInteger1();
                        m_popInteger2();
                        m_Integer2 ^= m_Integer1;
                        m_pushInteger2();
                        break;
                    case SUBOP_OVER:
                        m_popString1();
                        m_popString2();
                        m_pushString2();
                        m_pushString1();
                        m_pushString2();
                        break;
                    case SUBOP_LISTENTEXT:
                        if (m_pGPDLevent == NULL) {
                            m_listenText.Empty();
                        };
                        m_pushSP(m_listenText);
                        break;
                    case SUBOP_ONE:
                        m_pushSP(m_true);
                        break;
                    case SUBOP_PARTYSIZE:
                        m_Integer1 = GetPartySize(party);
                        m_pushInteger1();
                        break;
                    case SUBOP_PLUS: // string concatenation
                        m_popString1();
                        m_popString2();
                        m_string2 += m_string1;
                        m_pushString2();
                        break;
                    case SUBOP_POP:
                        m_popString1();
                        break;
                    case SUBOP_RANDOM:
                        m_popInteger1();
                        if (m_Integer1 <= 0) {
                            m_interpStatus = GPDL_ILLPARAM;
                            break;
                        };
                        //m_uInteger1=rand()%m_Integer1;
                        m_uInteger1 = RollDice(m_Integer1, 1, 0.0) - 1;
                        m_pushUInteger1();
                        break;

                    case SUBOP_RUN_CHAR_SCRIPTS:
                        {
                            CString result;
                            m_popString1();   // Script name
#ifdef UAFEngine
                            COMBATANT * pCOMBATANT;
                            if (IsCombatActive()) {
                                m_popInteger1();  // character index
                                pCOMBATANT = combatData.GetCombatant(m_Integer1);
                                if (pCOMBATANT != NULL) {
                                    result = pCOMBATANT -> RunCombatantScripts(
                                        m_string1,
                                        ScriptCallback_LookForChar,
                                        "N",
                                        "Script function $RUN_CHAR_SCRIPTS()");
                                    if (result.IsEmpty() || (result[0] != 'N'))
#ifdef newCombatant
                                    result = pCOMBATANT -> m_pCharacter -> RunCharacterScripts(
#else
              result = pCOMBATANT -> RunCharacterScripts(
#endif
                              m_string1,
                                        ScriptCallback_LookForChar,
                                        "N",
                                        "Script function $RUN_CHAR_SCRIPTS()");
                                };
                            }
                            else {
                                CHARACTER * pCharacter;
                                pCharacter = m_popCharacter("$RUN_CHAR_SCRIPTS()");
                                if (pCharacter != NULL) {
                                    result = pCharacter -> RunCharacterScripts(
                                        m_string1,
                                        ScriptCallback_LookForChar,
                                        "N",
                                        "Script function $RUN_CHAR_SCRIPTS()");
                                };
                            };
#endif
                            m_pushString1();
                        };
                        break;
                    case SUBOP_RUN_CHAR_SE_SCRIPTS:
                        {
                            CString result;
                            m_popString1();   // Script name
                            m_popString2();
#ifdef UAFEngine
                            {
                                CHARACTER * pCharacter;
                                ActorType actor = m_StringToActor((LPCSTR)m_string2);

                                pCharacter = /*(CHARACTER*)* /GetCurrentlyActiveContext(& actor, "$RUN_CHAR_SE_SCRIPTS()");
                                if (pCharacter == NULL) {
                                    m_interpretError("Invalid target actor data in $RUN_CHAR_SE_SCRIPTS");
                                    m_pushSP(m_false);
                                    break;
                                };
                                m_string1 = pCharacter -> RunSEScripts(
                                    m_string1,
                                    "Script function $RUN_CHAR_SE_SCRIPTS()");
                            };
#endif
                            m_pushString1();
                        };
                        break;
                    case SUBOP_RUN_CHAR_PS_SCRIPTS:
                        {
                            CString result;
                            m_popString1();   // Script name
                            m_popString2();
#ifdef UAFEngine
                            {
                                CHARACTER * pCharacter;
                                COMBATANT * pCombatant;
                                ActorType actor = m_StringToActor((LPCSTR)m_string2);

                                pCharacter = /*(CHARACTER*)* /GetCurrentlyActiveContext(& actor, "$RUN_CHAR_PS_SCRIPTS()");          
#ifdef newCombatant
                                if ((pCharacter == NULL) || ((pCombatant = pCharacter -> m_pCombatant) == NULL))
#else
                                if ((pCharacter == NULL) || ((pCombatant = pCharacter -> GetCombatant()) == NULL))
#endif
                                {
                                    m_interpretError("Invalid target actor data in $RUN_CHAR_PS_SCRIPTS");
                                    m_pushSP(m_false);
                                    break;
                                };
                                m_string1 = pCombatant -> RunPSScripts(m_string1, "Script function $RUN_CHAR_PS_SCRIPTS()");
                            };
#endif
                            m_pushString1();
                        };
                        break;
                    case SUBOP_RUN_AREA_SE_SCRIPTS:
                        {
                            CString result;
                            m_popInteger1();  // Combatant #
                            m_popString2();   // Script Name
#ifdef UAFEngine
                            {
                                COMBATANT * pCombatant;

                                pCombatant = combatData.GetCombatant(m_Integer1);
                                if (pCombatant == NULL) {
                                    m_interpretError("Invalid target actor data in $RUN_AREA_SE_SCRIPTS");
                                    m_pushSP(m_false);
                                    break;
                                };
                                m_string1 = activeSpellList.RunSEScripts(
                                    pCombatant -> x,
                                    pCombatant -> y,
                                    m_string2,
                                    "Script function $RUN_AREA_SE_SCRIPTS()");
                            };
#endif
                            m_pushString1();
                        };
                        break;
                    case SUBOP_ForEachPartyMember:
                        {
                            m_popString1(); // The script name
#ifdef UAFEngine
                            m_popString2(); // The Special Ability name
                            m_string3 = party.ForEachPartyMember(m_string2, m_string1);
                            m_pushString3();
#endif
                        };
                        break;
                    case SUBOP_ForEachPossession:
                        m_popString1();  // The script name
                        m_popString2();  // The actor
                        {
#ifdef UAFEngine
                            CHARACTER * pCharacter;
                            ActorType actor;
                            actor = m_StringToActor((LPCSTR)m_string2);
                            pCharacter = /*(CHARACTER*)* /GetCurrentlyActiveContext(& actor, "$ForEachPossession()");
                            if (pCharacter == NULL) {
                                m_interpretError("Invalid target actor data in $ForEachPossession()");
                                m_pushSP(m_false);
                                break;
                            };
                            m_pushSP(pCharacter -> ForEachPossession(m_string1));
#else
                            m_pushString1();
#endif
                        };
                        break;
                    case SUBOP_IsIdentified:
                        m_popInteger1(); // The 'num'
                        m_popInteger2(); // The 'key'
                        m_popString1();  // The 'actor'
                        {
#ifdef UAFEngine
                            CHARACTER * pCharacter;
                            ActorType actor;
                            actor = m_StringToActor((LPCSTR)m_string1);
                            pCharacter = /*(CHARACTER*)* /GetCurrentlyActiveContext(& actor, "$IsIdentified()");
                            if (pCharacter == NULL) {
                                m_interpretError("Invalid target actor data in $IsIdentifed");
                                m_pushSP(m_false);
                                break;
                            };
                            m_Integer1 = pCharacter -> IsIdentified(m_Integer2, m_Integer1);
                            m_pushInteger1();
#else
                            m_pushSP(m_true);
#endif
                        };
                        break;
                    case SUBOP_GetCombatantState:
#ifdef UAFEngine
                        {
                            COMBATANT * pCombatant;
                            pCombatant = m_popCombatantActor();
                            m_string1 = CombatantStateText[pCombatant -> State()];
                            m_pushString1();
                        };
#endif
                        break;
                    case SUBOP_SET_PARTY_XY:
                        m_popInteger1();
                        m_popInteger2();
#ifdef UAFEngine
                        InitiateTaskMessage_SetPartyXY(m_Integer2, m_Integer1);
#endif
                        m_string1.Empty();
                        m_pushString1();
                        break;
                    case SUBOP_REMOVE_SPELL_EFFECT:
                        m_popString1();  // The script name
                        m_popString2();  // The actor
#ifdef UAFEngine
                        {
                            CHARACTER * pCharacter;
                            ActorType actor = m_StringToActor((LPCSTR)m_string2);

                            pCharacter = /*(CHARACTER*)* /GetCurrentlyActiveContext(& actor, "$Remove_SPELL_EFFECT()");
                            if (pCharacter == NULL) {
                                m_interpretError("Invalid target actor data in $REMOVE_SPELL_EFFECT()");
                                m_pushSP(m_false);
                                break;
                            };
                            m_string1 = pCharacter -> RemoveSpellEffect(m_string1);
                        };
#endif
                        m_pushString1();
                        break;
                    case SUBOP_CALL_GLOBAL_SCRIPT:
                        {
                            CString result;
                            m_popString1();  // Script name
                            m_popString2();  // Special Ability name
#ifdef UAFEngine
                            m_string1 = RunGlobalScript(m_string2, m_string1, false);
#endif
                            m_pushString1();
                        };
                        break;
                    case SUBOP_SAY:
                        if (m_pGPDLevent == NULL) {
                            break;
                        };
                        m_popString1();
#ifdef UAFEngine
                        m_discourseText.SetNewTop();
                        m_discourseText.SetText(m_string1, GetTextBoxCharWidth());
                        m_discourseText.Display();
                        m_state = GPDL_WAIT_ACK;
#endif
                        m_pushSP(m_false); // All functions must return a value!
                        INTERP_RETURN(GPDL_WAIT_ACK);
                    case SUBOP_SET_CHAR_AC: SET_CHAR_INT(SetAC, "$SET_CHAR_AC()"); break;
                    case SUBOP_SET_CHAR_HITPOINTS: SET_CHAR_INT(SetHitPoints, "$SET_CHAR_HITPOINTS()"); break;
                    case SUBOP_SET_CHAR_THAC0: SET_CHAR_INT(SetTHAC0, "$SET_CHAR_THAC0()"); break;
                    case SUBOP_SET_CHAR_RDYTOTRAIN: SET_CHAR_BOOL(SetReadyToTrain, "$SET_CHAR_RDYTOTRAIN()"); break;
                    case SUBOP_SET_CHAR_Exp:
                        {
                            m_popInteger1();  // New Experience
                            m_popString1(); // class
#ifdef UAFEngine
                            {
                                CHARACTER * pChar;
                                pChar = m_popCharacter("$SET_CHAR_Exp()");
                                BASECLASS_ID baseclassID;
                                baseclassID = m_string1;
                                //pChar->SetCurrExp(m_string1, m_Integer1); break;
                                pChar -> SetCurrExp(baseclassID, m_Integer1);
                                break;
                            };
#else
                            m_popInteger1();
#endif
                            // Unreachable m_pushInteger1();  // Have to provide an answer!
                        };
                        break;


                    case SUBOP_SET_CHAR_Lvl:
                        {
                            m_popInteger1(); //new level
                            m_popString1(); // class
#ifdef UAFEngine
                            {
                                CHARACTER * pChar;
                                pChar = m_popCharacter("$SET_CHAR_:v;()");
                                BASECLASS_ID baseclassID;
                                baseclassID = m_string1;
                                //pChar->SetLevel(m_string1, m_Integer1);
                                pChar -> SetLevel(baseclassID, m_Integer1);
                            };
#else
                            m_popInteger1();
#endif
                            m_pushInteger1();
                        };
                        break;
                    case SUBOP_GET_CHAR_MAXHITPOINTS:
                        {
#ifdef UAFEngine
                            {
                                CHARACTER * pChar;
                                pChar = m_popCharacter("$GET_CHAR_MAXHITPOINTS()");
                                m_Integer1 = pChar -> GetMaxHitPoints();
                            };
#else
                            m_popInteger1();
#endif
                            m_pushInteger1();
                        };
                        break;

                    case SUBOP_SET_CHAR_MAXHITPOINTS:
                        {
                            m_popInteger1(); // new max
#ifdef UAFEngine
                            {
                                CHARACTER * pChar;
                                pChar = m_popCharacter("$SET_CHAR_MAXHITPOINTS()");
                                pChar -> SetMaxHitPoints(m_Integer1);
                            };
#else
                            m_popString1();
#endif
                            m_pushInteger1();
                        };
                        break;

                    case SUBOP_SET_CHAR_ICON_INDEX: SET_CHAR_INT(SetIconIndex, "$SET_CHAR_ICON_INDEX()"); break;
                    case SUBOP_SET_CHAR_AGE: SET_CHAR_INT(SetAge, "$SET_CHAR_AGE()"); break;
                    case SUBOP_SET_CHAR_MAXAGE: SET_CHAR_INT(SetMaxAge, "$SET_CHAR_MAXAGE()"); break;
                    case SUBOP_SET_CHAR_MAXMOVE: SET_CHAR_INT(SetMaxMovement, "$SET_CHAR_MAXMOVE()"); break;
                    case SUBOP_SET_CHAR_PERM_STR: SET_CHAR_INT(SetPermStr, "$SET_CHAR_PERM_STR()"); break;
                    case SUBOP_SET_CHAR_PERM_STRMOD: SET_CHAR_INT(SetPermStrMod, "$SET_CHAR_PERM_STRMOD()"); break;
                    case SUBOP_SET_CHAR_PERM_INT: SET_CHAR_INT(SetPermInt, "$SET_CHAR_PERM_INT()"); break;
                    case SUBOP_SET_CHAR_PERM_WIS: SET_CHAR_INT(SetPermWis, "$SET_CHAR_PERM_WIS()"); break;
                    case SUBOP_SET_CHAR_PERM_DEX: SET_CHAR_INT(SetPermDex, "$SET_CHAR_PERM_DEX()"); break;
                    case SUBOP_SET_CHAR_PERM_CON: SET_CHAR_INT(SetPermCon, "$SET_CHAR_PERM_CON()"); break;
                    case SUBOP_SET_CHAR_PERM_CHA: SET_CHAR_INT(SetPermCha, "$SET_CHAR_PERM_CHA()"); break;
                    case SUBOP_SET_CHAR_MAXENC: SET_CHAR_INT(SetMaxEncumbrance, "$SET_CHAR_MAXENC()"); break;
                    case SUBOP_SET_CHAR_SEX: SET_CHAR_STRING(SetGender, "$SET_CHAR_SEX()"); break;
                    case SUBOP_SET_CHAR_GENDER: SET_CHAR_STRING(SetGender, "$SET_CHAR_GENDER()"); break;
                    case SUBOP_SET_CHAR_ALIGNMENT: SET_CHAR_ENUM(SetAlignment, alignmentType, "$SET_CHAR_ALIGNMENT()"); break;
                    case SUBOP_SET_CHAR_STATUS: SET_CHAR_ENUM(SetStatus, charStatusType, "$SET_CHAR_STATUS()"); break;
                    case SUBOP_SET_CHAR_UNDEAD: SET_CHAR_STRING(SetUndead, "$SET_CHAR_UNDEAD()"); break;
                    case SUBOP_SET_CHAR_SIZE: SET_CHAR_ENUM(SetSize, creatureSizeType, "$SET_CHAR_SIZE()"); break;
                    case SUBOP_SET_CHAR_MAGICRESIST: SET_CHAR_INT(SetMagicResistance, "$SET_CHAR_<AGICRESIST()"); break;
                    case SUBOP_SET_CHAR_MORALE: SET_CHAR_INT(SetMorale, "$SET_CHAR_MORALE()"); break;
                    case SUBOP_SET_CHAR_DAMAGEBONUS: SET_CHAR_INT(SetDmgBonus, "$SET_CHAR_DAMAGEBONUS()"); break;
                    case SUBOP_SET_CHAR_HITBONUS: SET_CHAR_INT(SetHitBonus, "$SET_CHAR_HITBONUS()"); break;
                    case SUBOP_SetMemorizeCount: m_SetMemorizeCount(); break;
                    case SUBOP_GetHighestLevelBaseclass:
                        m_GetHighestLevelBaseclass(); break;
                    case SUBOP_GetBaseclassLevel: m_GetBaseclassLevel(); break;

                    case SUBOP_SET_CHAR_ASL:
                        {
                            CHARACTER * pChar;
                            m_popString2();  // value
                            m_popString1();  // key
                            pChar = m_popCharacter("$GET_CHAR_ASL()"); // character number
#ifdef UAFEngine
                            InsertCharacterASL(
                                pChar,
                                m_string1,
                                m_string2);
#endif
                        };
                        m_pushSP(m_string2);  // Must supply a result
                        break;
                    case SUBOP_MODIFY_CHAR_ATTRIBUTE:
                        m_popString4();   // Source of adjustment (for identification)
                        m_popString3();   // Text to display in character's stats
                        m_popInteger3();  // duration
                        m_popString2();   // time units
                        m_popInteger2();  // amount to be added
                        m_popString1();   // The attribute to be modified
        // Dude() gets this m_popString1();   // Ye Olde character
#ifdef UAFEngine
                        Dude("$MODIFY_CHAR_ATTRIBUTE()") -> AddTemporaryEffect(
                            m_string1,
                            m_string2,
                            m_Integer3,
                            m_Integer2,
                            m_string3,
                            m_string4,
                            "Script function $MODIFY_CHAR_ATTRIBUTE()");
#else
                        m_popString1(); // The dude
#endif
                        m_pushEmptyString();
                        break;
                    case SUBOP_REMOVE_CHAR_MODIFICATION:
#ifdef UAFEngine
                        m_popString1(); // The search pattern
                        m_string2 = (Dude("$REMOVE_CHAR_MODIFICTION()") -> RemoveTemporaryEffect(m_string1)) ? "1" : "";
                        m_pushString2();
#else
                        m_popString1();  // the pattern
                        m_popString2();  // the dude
                        m_pushEmptyString();
#endif
                        break;
                    case SUBOP_GET_GAME_CURRLEVEL: GET_LITERAL_INT(globalData.currLevel + 1); break;
                    //case SUBOP_GET_GAME_VERSION:     m_getGameValue(GAME_VERSION);   break;
                    case SUBOP_GET_GAME_VERSION:
                        m_string1.Format("%1.8f", globalData.version);
                        m_pushString1();
                        break;
                    case SUBOP_SET_GLOBAL_ASL:
                        m_popString1();  // The value
                        m_popString2();  // The key
                        InsertGlobalASL(
                            globalData,
                            m_string2,
                            m_string1);
                        m_pushSP(m_string1);
                        //        m_dataStack[m_SP]=m_variables.Set(m_string2, m_string1);
                        break;
                    case SUBOP_DEPRECATED_SET_LEVEL_STATS_ASL:
                        m_popString2();  // value
                        m_popString1();  // key
                        InsertLevelASL(
                            globalData,
                            GetCurrentLevel(),
                            m_string1,
                            m_string2);
                        //        m_attributeInsert(
                        //          globalData.levelInfo.stats[globalData.currLevel].level_attributes,
                        //          m_string1, 
                        //          m_string2);
                        m_pushSP(m_false);  // Must supply a result
                        break;
                    case SUBOP_SET_LEVEL_STATS_ASL:
                        m_popString2();  // value
                        m_popString1();  // key
                        m_popString3();  //Level
                        if (m_string3 == "") m_Integer1 = GetCurrentLevel();
                        else m_Integer1 = atoi(m_string3);
                        InsertLevelASL(
                            globalData,
                            m_Integer1,
                            m_string1,
                            m_string2);
                        //        m_attributeInsert(
                        //          globalData.levelInfo.stats[globalData.currLevel].level_attributes,
                        //          m_string1, 
                        //          m_string2);
                        m_pushSP(m_false);  // Must supply a result
                        break;
                    case SUBOP_GET_EVENT_Attribute:
                        m_popString1();     // The Attribute name
                        m_popInteger1();
#ifdef UAFEngine
                        m_string2 = FindEventAttribute(m_Integer1, m_string1);
#else
                        m_string2 = "!Found!";
#endif
                        m_pushString2();
                        break;
                    //case SUBOP_GET_PARTY_FACING:     m_getPartyValue(PARTY_FACING);     break;
                    case SUBOP_GET_PARTY_FACING: GET_LITERAL_INT(party.facing); break;
                    case SUBOP_GET_PARTY_LOCATION:
                        m_string1.Format("/%d/%d/%d",
                            party.level + 1, party.Posx, party.Posy);
                        m_pushString1();
                        break;
                    //case SUBOP_SET_PARTY_FACING:     m_setPartyValue(PARTY_FACING);     break;
                    case SUBOP_SET_PARTY_FACING:
                        m_popInteger1();
                        party.facing = min(max(m_Integer1, FACE_NORTH), FACE_SE);
                        break;
                    //case SUBOP_GET_PARTY_DAYS:       m_getPartyValue(PARTY_DAYS);       break;
                    case SUBOP_GET_PARTY_DAYS: GET_LITERAL_INT(party.days); break;
                    //case SUBOP_SET_PARTY_DAYS:       m_setPartyValue(PARTY_DAYS);       break;
                    case SUBOP_SET_PARTY_DAYS: SET_LITERAL_INT(party.days); break;
                    //case SUBOP_GET_PARTY_HOURS:      m_getPartyValue(PARTY_HOURS);      break;
                    case SUBOP_GET_PARTY_HOURS: GET_LITERAL_INT(party.hours); break;
                    //case SUBOP_SET_PARTY_HOURS:      m_setPartyValue(PARTY_HOURS);      break;
                    case SUBOP_SET_PARTY_HOURS: SET_LITERAL_INT(party.hours); break;
                    //case SUBOP_GET_PARTY_MINUTES:    m_getPartyValue(PARTY_MINUTES);    break;
                    case SUBOP_GET_PARTY_MINUTES: GET_LITERAL_INT(party.minutes); break;
                    //case SUBOP_SET_PARTY_MINUTES:    m_setPartyValue(PARTY_MINUTES);    break;
                    case SUBOP_SET_PARTY_MINUTES: SET_LITERAL_INT(party.minutes); break;
                    //case SUBOP_GET_PARTY_TIME:       m_getPartyValue(PARTY_TIME);       break;
                    case SUBOP_GET_PARTY_TIME: GET_LITERAL_INT((party.days * 24 + party.hours) * 60 + party.minutes);
                        break;
                    //case SUBOP_SET_PARTY_TIME:       m_setPartyValue(PARTY_TIME);       break;
                    case SUBOP_SET_PARTY_TIME:
                        m_popInteger1();
                        party.days = m_Integer1 / (24 * 60);
                        party.hours = m_Integer1 / 60 % 24;
                        party.minutes = m_Integer1 % 60;
                        m_pushEmptyString();
                        break;
                    //case SUBOP_GET_PARTY_ACTIVECHAR: m_getPartyValue(PARTY_ACTIVECHAR); break;
                    case SUBOP_GET_PARTY_ACTIVECHAR:
                        GET_LITERAL_INT(party.characters[party.activeCharacter].uniquePartyID); break;
                    //case SUBOP_SET_PARTY_ACTIVECHAR: m_setPartyValue(PARTY_ACTIVECHAR); break;
                    case SUBOP_SET_PARTY_ACTIVECHAR:
                        m_popInteger1();
                        if (party.numCharacters > 0) {
                            party.activeCharacter = m_Integer1 % party.numCharacters;
                        }
                        else {
                            party.activeCharacter = 0;
                        };
                        m_pushEmptyString();
                        break;
                    case SUBOP_SET_PARTY_ASL:
                        m_popString2();  // value
                        m_popString1();  // key
                        //        m_attributeInsert(
                        //          party.party_attributes,
                        //          m_string1, 
                        //          m_string2);
                        InsertPartyASL(
                            party,
                            m_string1,
                            m_string2);
                        m_pushSP(m_string2);  // Must supply a result
                        break;
                    case SUBOP_SET_QUEST:
                        {
                            int j;
                            char sign = ' ', c;
                            int value = 0, oldValue;
                            int valLen;
                            m_string1 = m_popSP(); // Get value or increment
                            m_string2 = m_popSP(); // Get quest name
                            valLen = m_string1.GetLength();
                            for (j = 0; j < valLen; j++) {
                                c = m_string1[j];
                                if (c == '-') sign = '-';
                                if (c == '+') sign = '+';
                                if ((c >= '0') && (c <= '9')) value = value * 10 + c - '0';
                            };
                            if (sign != ' ') {
                                oldValue = globalData.questData.GetStage(m_string2);
                                if (sign == '+') value = oldValue + value;
                                else value = oldValue - value;
                            };
                            globalData.questData.SetStage(m_string2, value);
                            value = globalData.questData.GetStage(m_string2);
                            m_string1.Format("%d", value);
                            m_pushSP(m_string1);
                        };
                        break;
                    case SUBOP_SMALL_PICTURE:
                        m_popString1();
                        m_pushString1(); // Must supply result.
                        if (m_pGPDLevent == NULL) break;
                        //if (m_pGPDLevent->pic.picType!=SmallPicDib) break;
                        m_pGPDLevent -> pic.FreePic();
                        m_pGPDLevent -> pic.picType = SmallPicDib;
                        m_pGPDLevent -> pic.filename=m_string1;
                        m_pGPDLevent -> pic.LoadPicSurfaces("");
                        break;
                    case SUBOP_Status:
                        m_popString1();
                        m_string2 = m_Status(m_string1);
                        m_pushString2();
                        break;
                    case SUBOP_SWAP:
                        m_popString1();
                        m_popString2();
                        m_pushString1();
                        m_pushString2();
                        break;

                    /*      case SUBOP_TARGETINDEX:
                            {
                              m_string1 = pScriptContext->GetTargetContext();
                              if (m_string1.IsEmpty())
                              {
                                MsgBoxError("$TargetIndex() called when no target context exists","Error",2);
                              }
                              else
                              {
                                m_string1 = m_IndexOf(m_string1);
                              };
                              m_pushString1();
                            };
                            break;
                    * /
                    case SUBOP_MYINDEX:
                        m_string1 = m_IndexOf(m_Myself());
                        m_pushString1();
                        break;

                    case SUBOP_TargetContext:
                        m_string1 = pScriptContext -> GetTargetContext("$TargetContext() called when no target context exists");
                        m_pushString1();
                        break;
                    case SUBOP_AttackerContext:
                        m_string1 = pScriptContext -> GetAttackerContext("$AttackerContext() called when no attacker context exists");
                        m_pushString1();
                        break;
                    case SUBOP_CombatantContext:
                        m_string1 = pScriptContext -> GetCombatantContext("$CombatantContext() called when no combatant context exists");
                        m_pushString1();
                        break;
                    case SUBOP_iEQUAL:
                        m_popString1();
                        m_popString2();
                        if (LongCompare(m_string1, m_string2) == 0)
                            m_pushSP(m_true); else m_pushSP(m_false);
                        break;
                    case SUBOP_iGREATER:
                        m_popString1();
                        m_popString2();
                        if (LongCompare(m_string1, m_string2) < 0)
                            m_pushSP(m_true); else m_pushSP(m_false);
                        break;
                    case SUBOP_iLESS:
                        m_popString1();
                        m_popString2();
                        if (LongCompare(m_string1, m_string2) > 0)
                            m_pushSP(m_true); else m_pushSP(m_false);
                        break;
                    case SUBOP_iPLUS:
                        m_popString1();
                        m_popString2();
                        m_string3 = LongAdd(m_string1, m_string2);
                        m_pushString3();
                        break;
                    case SUBOP_iMINUS:
                        m_popString1();
                        m_popString2();
                        m_string3 = LongSubtract(m_string2, m_string1);
                        m_pushString3();
                        break;
                    case SUBOP_iDIV:
                        m_popString1();
                        m_popString2();
                        LongDivide(m_string2, m_string1, m_string3, m_string4);
                        m_pushString3();
                        break;
                    case SUBOP_iMOD:
                        m_popString1();
                        m_popString2();
                        LongDivide(m_string2, m_string1, m_string3, m_string4);
                        m_pushString4();
                        break;
                    case SUBOP_iTIMES:
                        // n-digit multiplication.
                        m_popString1();
                        m_popString2();
                        m_string3 = LongMultiply(m_string1, m_string2);
                        m_pushString3();
                        /*
                        {
                          int k, k1, carry;
                          int i,j;
                          unsigned char *t;  // Temporary space for all three strings.
                          unsigned char *m, *n, *r;
                          int len1, len2;
                          bool error=false;
                          m_string1.TrimLeft();
                          m_string2.TrimLeft();
                          m_string1.TrimRight();
                          m_string2.TrimRight();
                          len1=m_string1.GetLength();
                          len2=m_string2.GetLength();
                          t=(unsigned char *)alloca(len1+len2+(len1+len2));
                          if (t==NULL) {
                            MsgBoxError("$TIMES alloca failure");
                            break;
                          };
                          m=t;
                          n=t+len1;
                          r=n+len2;
                          for (i=0; i<len1; i++) {
                            m[i]=(unsigned char)(m_string1.GetAt(i)-'0');
                            if (m[i]>9) error=true;
                          };
                          for (i=0; i<len2; i++) {
                            n[i]=(unsigned char)(m_string2.GetAt(i)-'0');
                            if (n[i]>9) error=true;
                          };
                          if (error) {
                            m_error_message("argument to $TIMES not decimal digits");
                            break;
                          };
                          for (i=0; i<len1+len2; i++) r[i]=0;
                          for (i=len1-1; i>=0; i--) { // each digit of multiplier
                            for (j=len2-1; j>=0; j--) { // each digit of multiplicand
                              k=i+j+1; // where answer goes.
                              r[k]=(unsigned char)(r[k]+(m[i]*n[j]));
                              k1=k;
                              carry=0;
                              while (r[k1]>=10) {
                                carry=r[k1]/10;
                                r[k1]=(unsigned char)(r[k1]%10);
                                k1--;
                                r[k1]=(unsigned char)(r[k1]+carry);
                              };
                            };
                          };
                          k=-1;
                          for (i=0; i<len1+len2; i++) {
                            r[i]+='0';
                            if ((r[i]!='0')&&(k==-1))k=i; // first non-zero digit
                          };
                          if (k==-1) k=0;
                          m_dataStack[m_SP]=CString((char *)r+k,len1+len2-k);
                        };
                        * /
                        break;
                    case SUBOP_SetFriendly:
                        {
                            m_popInteger2();  // friendly adjustment
                            m_popInteger1();  // combatant id
#ifdef UAFEngine
                            COMBATANT * pCombatant;
                            pCombatant = combatData.GetCombatant(m_Integer1);
                            if (pCombatant == NULL) {
                                m_Integer3 = 0;
                            }
                            else {
                                m_Integer3 = pCombatant -> m_adjFriendly;
                                if ((m_Integer2 >= 0) && (m_Integer2 <= 3)) {
                                    pCombatant -> m_adjFriendly = m_Integer2;
                                };
                            };
#else
                            m_Integer3 = 0;
#endif
                            m_pushInteger3();
                        };
                        break;
                    case SUBOP_GetFriendly:
                        {
                            m_popString2();  // data type (basic, adjustment, or adjusted)
                            m_popInteger1();  // combatant id
#ifdef UAFEngine
                            COMBATANT * pCombatant;
                            pCombatant = combatData.GetCombatant(m_Integer1);
                            if (pCombatant == NULL) {
                                m_string3 = "Huh?";
                                m_pushString3();
                            }
                            else {
                                if (m_string2 == "A") m_Integer3 = pCombatant -> m_adjFriendly;
                                else if (m_string2 == "B") m_Integer3 = pCombatant -> friendly ? 1 : 0;
                                else if (m_string2 == "F") m_Integer3 = pCombatant -> GetIsFriendly() ? 1 : 0;
                                else {
                                    m_string3 = "Huh?";
                                    m_pushString3();
                                    break;
                                };
                                m_pushInteger3();
                            };
#else
                            m_Integer3 = 0;
                            m_pushInteger3();
#endif
                        };
                        break;
                    case SUBOP_GetCombatRound:
                        {
#ifdef UAFEngine
                            m_Integer1 = GetCurrentRound();
#else
                            m_Integer1 = 3;
#endif
                            m_pushInteger1();
                        };
                        break;
                    case SUBOP_TeleportCombatant:
                        {
                            m_popInteger3();              // y
                            m_popInteger2();              // x
                            m_popInteger1();              // id
#ifdef UAFEngine
                            if (combatData.GetCombatant(m_Integer1) == NULL) {
                                m_interpretError("Attempt to teleport illegal combatant");
                            }
                            else {
                                combatData.GetCombatant(m_Integer1) -> TeleportCombatant(m_Integer2, m_Integer3);
                            };
#endif
                        };
                        m_string1 = emptyString;
                        m_pushString1();
                        break;
                    case SUBOP_CombatantLocation:
                        m_popString1();  // Axis
                        m_popInteger1(); // Combatant ID
#ifdef UAFEngine
                        if (IsCombatActive()
                            && (m_Integer1 < combatData.NumCombatants())) {
                            if (m_string1 == "X") {
                                m_Integer2 = combatData.GetCombatant(m_Integer1) -> x;
                            }
                            else {
                                m_Integer2 = combatData.GetCombatant(m_Integer1) -> y;
                            };
                        }
                        else {
                            m_Integer2 = -1;
                        };
#else
                        m_Integer2 = 0;
#endif
                        m_pushInteger2();
                        break;
                    case SUBOP_VisualDistance:
                        m_popInteger1();
                        m_popInteger2();
                        m_Integer3 = 999999;
#ifdef UAFEngine
                        if (IsCombatActive()) {
                            COMBATANT * pC1, * pC2;
                            pC1 = combatData.GetCombatant(m_Integer1);
                            pC2 = combatData.GetCombatant(m_Integer2);
                            if ((pC1 != NULL) && (pC2 != NULL)) {
                                int x1, x2, y1, y2;
                                x1 = pC1 -> x;
                                x2 = pC2 -> x;
                                y1 = pC1 -> y;
                                y2 = pC2 -> y;
                                if (HaveLineOfSight(x1, y1, x2, y2, NULL)) {
                                    m_Integer3 = (x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2);
                                    m_Integer3 = sqrt((float)m_Integer3);
                                };
                            };
                        };
#endif
                        m_pushInteger3();
                        break;
                    case SUBOP_IsLineOfSight:
                        m_popInteger5();
                        m_popInteger4();
                        m_popInteger3();
                        m_popInteger2();
                        m_popInteger1();
#ifdef UAFEngine
                        if (IsLineOfSight(m_Integer1, m_Integer2, m_Integer3, m_Integer4)) {
                            m_string1 = "1";
                        }
                        else {
                            m_string1 = "";
                        };
#else
                        m_string1 = "1";
#endif
                        m_pushString1();
                        break;
                    /*
                          case SUBOP_DialogSelect:
                            {
                              for (i=0; i<MAX_DLG_CONTROLS; i++)
                              {
                                dialogControls[i] = "";
                              };
                              m_popInteger1();
                              switch (m_Integer1)
                              {
                              case 0:
                                gpdlDlg = 0;
                                if (activeDlg != NULL) delete activeDlg;
                                activeDlg = new GPDLDlg(IDD_GPDL, NULL);            
                                break;
                              };
                            };
                            m_pushInteger1();
                            break;
                          case SUBOP_DialogSet:
                            {
                              m_popString1();
                              m_popUInteger1();
                              switch(gpdlDlg)
                              {
                              default:
                                {
                                  if (m_uInteger1 < MAX_DLG_CONTROLS)
                                  {
                                    dialogControls[m_uInteger1] = m_string1;
                                  };
                                };
                                break;
                              case 999: break; // to shut up the compiler
                              };
                            };
                            m_pushString1();
                            break;
                          case SUBOP_DialogDisplay:
                            {
                              ShowCursor(true);
                              activeDlg->DoModal();
                              ShowCursor(false);
                            };
                            m_string1 = "";
                            m_pushString1();
                            break;
                          case SUBOP_DialogGet:
                            {
                              m_popUInteger1();
                              if (m_uInteger1 <MAX_DLG_CONTROLS)
                              {
                                m_pushSP(dialogResults[m_uInteger1]);
                              }
                              else
                              {
                                m_pushSP(emptyString);
                              };
                            };
                            break;
                    * /
                    case SUBOP_ListAdjacentCombatants:
                        m_popInteger1();  // The combatant in question
#ifdef UAFEngine
                        m_string1 = combatData.ListAdjacentCombatants(m_Integer1, FALSE);
#else
                        m_string1 = "|0";
#endif
                        m_pushString1();
                        break;
                    case SUBOP_ComputeAttackDamage:
                        {
                            const COMBATANT * pAttacker;
                            const COMBATANT * pTarget;
                            int damage = 0;
                            pTarget = m_popCombatant("$ComputeAttackDamage()");
                            pAttacker = m_popCombatant("$ComputeAttackDamage()");
                            if ((pTarget != NULL) && (pAttacker != NULL)) {
                                damage = ComputeAttackDamage(pAttacker, pTarget);
                            };
                            m_Integer1 = damage;
                            m_pushInteger1();
                        };
                        break;
                    case SUBOP_ZERO:
                        m_string1 = "0";
                        m_pushString1();
                        break;
                    case SUBOP_UpCase:
                        m_popString1();
                        m_string1.MakeUpper();
                        m_pushString1();
                        break;
                    case SUBOP_DownCase:
                        m_popString1();
                        m_string1.MakeLower();
                        m_pushString1();
                        break;
                    case SUBOP_Capitalize:
                        m_popString1();
                        {
                            bool ws = true;
                            int len = m_string1.GetLength();
                            m_string2 = m_string1;
                            m_string2.MakeUpper();
                            m_string1.MakeLower();
                            for (int j = 0; j < len; j++)
                            {
                                if (ws) {
                                    m_string1.SetAt(j, m_string2.GetAt(j));
                                    ws = false;
                                }
                                if (m_string1.GetAt(j) == ' ') ws = true;
                            };
                        };
                        m_pushString1();
                        break;
                    //   Readded 20160907 PRS /* Deprecated
                    case SUBOP_GetSpellbook:
                        {
#ifdef UAFEngine
                            CHARACTER * pCharacter;
                            m_popString1();
                            pCharacter = m_popCharacterActor();
                            m_string1 = GetSpellbook(pCharacter, m_string1);
                            m_pushString1();
#else
                            m_popString1();
#endif
                        };
                        break;
                    //        * /
                    case SUBOP_SelectSpell:
                        {
#ifdef UAFEngine
                            CHARACTER * pCharacter;
                            SPELL_ID spellID;
                            m_popString1();  // the spell name
                            pCharacter = m_popCharacterActor();
                            spellID = m_string1;
                            m_string2 = SelectSpell(pCharacter, spellID);
                            m_pushString2();
#else
                            m_popString1();
#endif
                        };
                        break;
                    case SUBOP_Memorize:
                        {
#ifdef UAFEngine
                            CHARACTER * pCharacter;
                            pCharacter = m_popCharacterActor();
                            Memorize(pCharacter);
#else
                            m_popString1();
#endif
                            m_string2 = "OK";
                            m_pushString2();
                        };
                        break;
                    /*  Replaced with version that uses uniqueID instead of party index
                          case SUBOP_SetMemorizeCount:
                            {
                    #ifdef UAFEngine
                              m_popString1();
                              m_popString3();
                              m_popInteger1();
                              m_Integer1 = SetMemorizeCount(m_Integer1, m_string3, m_string1, "$SetMemorizeCount()");
                              m_pushInteger1();
                    #else
                              m_popString1();
                              m_popString2();
                              m_popString3();
                              m_string2 = "OK";
                              m_pushString2();
                    #endif
                            };
                            break;
                            * /

                    case SUBOP_KNOW_SPELL:
                        {
#ifdef UAFEngine
                            CHARACTER * pCharacter;
                            SPELL_ID spellid;
                            const SPELL_DATA * pSpell;
                            m_popInteger1(); // Know or un-know
                            m_popString1(); // the spellID
                            pCharacter = m_popCharacterActor();
                            spellid = m_string1;
                            pSpell = spellData.PeekSpell(spellid);
                            m_string2 = "1";
                            if (pSpell == NULL) {
                                m_string2.Empty();
                            }
                            else {
                                if (!pCharacter -> KnowSpellyyy(pSpell, m_Integer1 != 0)) {
                                    m_string2.Empty();
                                };
                            };
#else
                            m_popString1(); // 'know' flag
                            m_popString1(); // spell name
                            m_popString1(); // actor
                            m_string2 = "1"; 
#endif
                        };
                        m_pushString2();
                        break;

                    case SUBOP_MonsterPlacement:
#ifdef UAFEngine
                        m_popString1();
                        m_string2 = MonsterPlacementCallback(m_string1);
                        m_pushString2();
#endif
                        break;


                    case SUBOP_SetWall:
                        m_popInts(5);
#ifdef UAFEngine
                        SetMapOverride(WALL_OVERRIDE_USER, m_intArray);
#endif
                        m_pushEmptyString();
                        break;
                    case SUBOP_SetBackground:
                        m_popInts(5);
#ifdef UAFEngine
                        SetMapOverride(BACKGROUND_OVERRIDE_USER, m_intArray);
#endif
                        m_pushEmptyString();
                        break;
                    case SUBOP_SetOverlay:
                        m_popInts(5);
#ifdef UAFEngine
                        SetMapOverride(OVERLAY_OVERRIDE_USER, m_intArray);
#endif
                        m_pushEmptyString();
                        break;
                    case SUBOP_SetBlockage:
                        m_popInts(5);
#ifdef UAFEngine
                        SetMapOverride(BLOCKAGE_OVERRIDE, m_intArray);
#endif
                        m_pushEmptyString();
                        break;
                    case SUBOP_SetDoor:
                        m_popInts(5);
#ifdef UAFEngine
                        SetMapOverride(DOOR_OVERRIDE_USER, m_intArray);
#endif
                        m_pushEmptyString();
                        break;
                    case SUBOP_GetWall:
                        m_popInts(4);
#ifdef UAFEngine
                        m_Integer1 = GetMapOverride(WALL_OVERRIDE_USER, m_intArray);
#endif
                        m_pushInteger1();
                        break;
                    case SUBOP_GetBackground:
                        m_popInts(4);
#ifdef UAFEngine
                        m_Integer1 = GetMapOverride(BACKGROUND_OVERRIDE_USER, m_intArray);
#endif
                        m_pushInteger1();
                        break;
                    case SUBOP_GetOverlay:
                        m_popInts(4);
#ifdef UAFEngine
                        m_Integer1 = GetMapOverride(OVERLAY_OVERRIDE_USER, m_intArray);
#endif
                        m_pushInteger1();
                        break;
                    case SUBOP_GetBlockage:
                        m_popInts(4);
#ifdef UAFEngine
                        m_Integer1 = GetMapOverride(BLOCKAGE_OVERRIDE, m_intArray);
#endif
                        m_pushInteger1();
                        break;
                    case SUBOP_GetDoor:
                        m_popInts(5);
#ifdef UAFEngine
                        m_Integer1 = GetMapOverride(DOOR_OVERRIDE_USER, m_intArray);
#endif
                        m_pushInteger1();
                        break;
                    case SUBOP_COMBATANT_AVAILATTACKS:
#ifdef UAFEngine
                        {
                            COMBATANT * pCombatant;
                            m_popInteger1(); // Function
                            m_popInteger2(); // Value
                            pCombatant = m_popCombatantActor();
                            if (pCombatant != NULL) {
                                switch (m_Integer1) {
                                    case 0:
                                        pCombatant -> availAttacks =  m_Integer2; break;
                                    case 1:
                                        pCombatant -> availAttacks += m_Integer2; break;
                                };
                                m_Integer1 = pCombatant -> availAttacks;
                                m_pushInteger1();
                                break;
                            }
                            else {
                                m_string1 = "";
                            };
                        };
#else
                        m_string1 = "";
#endif
                        m_pushString1();
                        break;

                    case SUBOP_IntegerTable:
                        {
                            m_popString1();  // The function
                            m_popInteger2(); // The value
                            m_popString3();  // The table name
                            m_popString4();  // The Special Ability name
#ifdef UAFEngine
                            m_Integer1 = IntegerTableLookup(m_string4, m_string3, m_Integer2, m_string1);
#else
                            m_Integer1 = 1;
#endif
                            m_pushInteger1();
                            break;
                        };
                    case SUBOP_RollHitPointDice:
                        {
                            m_popInteger3(); // upper limit
                            m_popInteger2(); // lower limit
                            m_popString1(); // basclass name
#ifdef UAFEngine
                            m_Integer1 = RollHitPointDice(m_string1, m_Integer2, m_Integer3);
#else
                            m_Integer1 = 7;
#endif
                            m_pushInteger1();
                        };
                        break;
                    case SUBOP_AURA_Create:
                        {
#ifdef UAFEngine
                            m_popString5();
                            m_popString4();
                            m_popString3();
                            m_popString2();
                            m_popString1();
                            combatData.CreateAura(m_string1, m_string2, m_string3, m_string4, m_string5);
                            break;
#endif
#ifdef UAFEDITOR
                            m_popString1();
                            m_popString1();
                            m_popString1();
                            m_popString1();
#endif
                        };
                        break;
                    case SUBOP_AURA_Destroy: AURA_FUNCTION($AURA_Destroy); break;
                    case SUBOP_AURA_AddSA: AURA_FUNCTION($AURA_AddSA); break;
                    case SUBOP_AURA_GetSA: AURA_FUNCTION($AURA_GetSA); break;
                    case SUBOP_AURA_RemoveSA: AURA_FUNCTION($AURA_RemoveSA); break;
                    case SUBOP_AURA_Attach: AURA_FUNCTION($AURA_Attach); break;
                    case SUBOP_AURA_Combatant: AURA_FUNCTION($AURA_Combatant); break;
                    case SUBOP_AURA_Wavelength: AURA_FUNCTION($AURA_Wavelength); break;
                    case SUBOP_AURA_Size: AURA_FUNCTION($AURA_Size); break;
                    case SUBOP_AURA_Shape: AURA_FUNCTION($AURA_Shape); break;
                    case SUBOP_AURA_Spell: AURA_FUNCTION($AURA_Spell); break;
                    case SUBOP_AURA_Location: AURA_FUNCTION($AURA_Location); break;
                    case SUBOP_AURA_GetData: AURA_FUNCTION($AURA_GetData); break;
                    case SUBOP_AURA_SetData: AURA_FUNCTION($AURA_SetData); break;

                    case SUBOP_GrSet:
                        {
                            m_popString3();
                            m_popString2();
#ifdef UAFEngine
                            m_popString1();
                            m_string2 = GrSet(m_string1, m_string2, m_string3);
                            m_pushString2();
#endif
                        };
                        break;
                    case SUBOP_GrSetLinefeed:
                        {
#ifdef UAFEngine
                            m_popString1();
                            m_string2 = GrSetLinefeed(m_string1);
                            m_pushString2();
#endif
                        };
                        break;
                    case SUBOP_GrMoveTo:
                        {
#ifdef UAFEngine
                            m_popString1();
                            m_string2 = GrMoveTo(m_string1);
                            m_pushString2();
#endif
                        };
                        break;
                    case SUBOP_GrFormat:
                        {
#ifdef UAFEngine
                            m_popString1();
                            m_string2 = GrFormat(m_string1);
                            m_pushString2();
#endif
                        };
                        break;
                    case SUBOP_GrPrint:
                        {
#ifdef UAFEngine
                            m_popString1();
                            m_string2 = GrPrint(m_string1, 0, FALSE);
                            m_pushString2();
#endif
                        };
                        break;
                    case SUBOP_GrPrtLF:
                        {
#ifdef UAFEngine
                            m_popString1();
                            m_string2 = GrPrtLF(m_string1, 0, FALSE);
                            m_pushString2();
#endif
                        };
                        break;
                    case SUBOP_GrColor:
                        {
                            m_popString1();
                            m_pushString1();
#ifdef UAFEngine
                            m_popInteger1();
                            m_string2 = GrColor(m_string1, m_Integer1);
                            m_pushString2();
#endif
                        };
                        break;
                    case SUBOP_GrMark:
                        {
#ifdef UAFEngine
                            m_popString1();
                            m_string2 = GrMark(m_string1);
                            m_pushString2();
#endif
                        };
                        break;
                    case SUBOP_GrMove:
                        {
#ifdef UAFEngine
                            m_popString1();
                            m_string2 = GrMove(m_string1);
                            m_pushString2();
#endif
                        };
                        break;
                    case SUBOP_GrTab:
                        {
#ifdef UAFEngine
                            m_popString1();
                            m_string2 = GrTab(m_string1);
                            m_pushString2();
#endif
                        };
                        break;
                    case SUBOP_GrPic:
                        {
#ifdef UAFEngine
                            m_popString1();
                            m_string2 = GrPic(m_string1);
                            m_pushString2();
#endif
                        };
                        break;
                    case SUBOP_DAT_Race_Weight:
                        {
                            CHARACTER * pChar;
                            pChar = m_popCharacterActor();
#ifdef UAFEngine
                            m_Integer1 = DAT_Race_Weight(pChar);
#else
                            m_Integer1 = 0;
#endif
                            m_pushInteger1();
                        };
                        break;
                    case SUBOP_DAT_Race_Height:
                        {
                            CHARACTER * pChar;
                            pChar = m_popCharacterActor();
#ifdef UAFEngine
                            m_Integer1 = DAT_Race_Height(pChar);
#else
                            m_Integer1 = 0;
#endif
                            m_pushInteger1();
                        };
                        break;
                    case SUBOP_DAT_Item_CommonName:
                        {
                            const ITEM_DATA * pItem;
                            pItem = m_GetItemData();
                            if (pItem != NULL) {
                                m_string2 = pItem -> CommonName();
                            };
                            m_pushString2();
                        };
                        break;
                    case SUBOP_DAT_Item_IDName:
                        {
                            const ITEM_DATA * pItem;
                            pItem = m_GetItemData();
                            if (pItem != NULL) {
                                m_string2 = pItem -> IdName();
                            };
                            m_pushString2();
                        };
                        break;
                    case SUBOP_DAT_Item_Priority:
                        {
                            const ITEM_DATA * pItem;
                            pItem = m_GetItemData();
                            if (pItem != NULL) {
                                m_string2.Format("%d", pItem -> m_priorityAI);
                            };
                            m_pushString2();
                        };
                        break;
                    case SUBOP_DAT_Item_MaxRange:
                        {
                            const ITEM_DATA * pItem;
                            pItem = m_GetItemData();
                            if (pItem == NULL) {
                                m_string2.Empty();
                            }
                            else {
                                m_string2.Format("%d", pItem -> RangeMax);
                            };
                            m_pushString2();
                        };
                        break;
                    case SUBOP_DAT_Item_MediumRange:
                        {
                            const ITEM_DATA * pItem;
                            pItem = m_GetItemData();
#ifdef UAFEngine
                            if (pItem != NULL) {
                                m_string2.Format("%d", pItem -> RangeMedium);
                            };
#endif
                            m_pushString2();
                        };
                        break;
                    case SUBOP_DAT_Item_ShortRange:
                        {
                            const ITEM_DATA * pItem;
                            pItem = m_GetItemData();
#ifdef UAFEngine
                            if (pItem != NULL) {
                                m_string2.Format("%d", pItem -> RangeShort);
                            };
#endif
                            m_pushString2();
                        };
                        break;
                    case SUBOP_DAT_Item_DamageSmall:
                        {
                            const ITEM_DATA * pItem;
                            pItem = m_GetItemData();
                            if (pItem != NULL) {
                                m_string2.Format("$%d$%d$%d", pItem -> Nbr_Dice_Sm, pItem -> Dmg_Dice_Sm, pItem -> Dmg_Bonus_Sm);
                            };
                            m_pushString2();
                        };
                        break;
                    case SUBOP_DAT_Item_DamageLarge:
                        {
                            const ITEM_DATA * pItem;
                            pItem = m_GetItemData();
                            if (pItem != NULL) {
                                m_string2.Format("$%d$%d$%d", pItem -> Nbr_Dice_Lg, pItem -> Dmg_Dice_Lg, pItem -> Dmg_Bonus_Lg);
                            };
                            m_pushString2();
                        };
                        break;
                    case SUBOP_DAT_Item_AttackBonus:
                        {
                            const ITEM_DATA * pItem;
                            pItem = m_GetItemData();
                            if (pItem != NULL) {
                                m_string2.Format("%d", pItem -> Attack_Bonus);
                            };
                            m_pushString2();
                        };
                        break;
                    case SUBOP_GET_CONFIG:
                        {
                            m_popString1();
#ifdef UAFEngine
                            FindConfigToken(m_string1, m_string2);
#else
                            FindConfigToken(m_string1, m_string2);
#endif
                            m_pushString2();
                        };
                        break;
                    case SUBOP_SLEEP:
#ifdef UAFEngine
                        Sleep(m_popInteger());
#endif
                        m_pushEmptyString();
                        break;
                    case SUBOP_DRAWADVENTURESCREEN:
#ifdef UAFEngine
                        UpdateAdventureScreen();
#endif
                        m_pushEmptyString();
                        break;
                    default:
                        {
                            CString msg;
                            msg.Format("Illegal subop code\n0x%08x", m_subop);
                            m_interpretError(msg);
                        };
                        return BreakExecute();
                };
                break;
            default:
                {
                    var msg = "";
                    msg = "Illegal opcode\n0x%02x";
                    this.m_interpretError(msg);
                }
                return this.BreakExecute();
        }
*/
    }
    // Some sort of error must have occurred.
    this.INTERP_RETURN(m_interpStatus);

}
