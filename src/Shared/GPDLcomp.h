
#ifndef __GPDLCOMP_H__
#define __GPDLCOMP_H__

#include "GPDLOpCodes.h"

#define MAXSTACK 20


// Attributes that can preceed function and variable declarations.
#define ATTRIBUTE_PUBLIC 1

// script function usages
#define ALL_USAGE              255
#define TALK_USAGE               1
#define SPELL_USAGE              2
#define LOGICBLK_USAGE           4
#define EVENT_USAGE              8
#define INTERNAL_USAGE          16
#define SPECIALABILITIES_USAGE  32
#define COMBAT_USAGE            64
#define GRAPHICS_USAGE         128

typedef CList<CString, CString> TEXTLIST;

class DICTIONARY;
class DEFINITION;
class GLOBALS;
enum TOKENTYPE;
class RAM_FILE;

class GPDLCOMP
{
  friend class GPDL;
public:
  unsigned int indexSize(void);
  CString indexName(int i);
  int indexValue(int i);
  unsigned int constantsSize(void);
  CString constantValue(int i);
private:

  // A class to make sure that current gets put back
  // as it was when you exit your function.
  class SAVECURRENT 
  {
  private:
    DICTIONARY *m_saveOldCurrent;
    DICTIONARY **m_oldCurrent;
  public:
    SAVECURRENT (DICTIONARY **oldCurrent) 
    {
      m_oldCurrent = oldCurrent;
      m_saveOldCurrent=*m_oldCurrent;
    };
    ~SAVECURRENT(void) 
    {
      *m_oldCurrent=m_saveOldCurrent;
    };
  };

  // A class to make sure that context gets put back
  // as it was when you exit your function.
  class SAVECONTEXT 
  {
  private:
    DICTIONARY *m_saveOldContext;
    DICTIONARY **m_oldContext;
  public:
    SAVECONTEXT (DICTIONARY **oldContext) 
    {
      m_oldContext = oldContext;
      m_saveOldContext=*m_oldContext;
    };
    ~SAVECONTEXT(void) 
    {
      *m_oldContext=m_saveOldContext;
    };
  };
  // Here we have a class of object that you can allocate on
  // the stack if you want to save the current activeFunction
  // and be confidant that it will be restoreed when you
  // are finished with it.  Avoids the common mistake of
  // putting in a return and forgetting to restore activeFunction.
  class SAVEACTIVEFUNCTION {
  private:
    DEFINITION *m_saveActiveFunction;
    DEFINITION **m_activeFunction;
  public:
    SAVEACTIVEFUNCTION (DEFINITION *newFunction, DEFINITION **activeFunction) {
      m_activeFunction = activeFunction;
      m_saveActiveFunction=*m_activeFunction;
      *m_activeFunction=newFunction;
    };
    ~SAVEACTIVEFUNCTION (void) {
      *m_activeFunction=m_saveActiveFunction;
    };
  };
  // Here we have a class of object that you can allocate on
  // the stack if you want to save the current breaklist when
  // you enter a function and restore the breaklist when you
  // leave the function.  Avoids the common mistake of
  // putting in a return and forgetting to restore breaklist.
  class SAVEBREAKLIST {
  private:
    unsigned int  m_saveBreak;
    unsigned int *m_oldBreak;
  public:
    SAVEBREAKLIST (unsigned int *oldBreak) {
      m_oldBreak = oldBreak;
      m_saveBreak=*m_oldBreak;
      *m_oldBreak=0;
    };
    ~SAVEBREAKLIST (void) {
      *m_oldBreak=m_saveBreak;
    };
  };
  // Here we have a class of object that you can allocate on
  // the stack if you want to save the current continuelist
  // and be confidant that it will be restoreed when you
  // are finished with it.  Avoids the common mistake of
  // putting in a return and forgetting to restore continuelist.
  class SAVECONTINUELIST {
  private:
    unsigned int m_saveContinueList;
    unsigned int *m_oldContinue;
  public:
    SAVECONTINUELIST (unsigned int *oldContinue) {
      m_oldContinue = oldContinue;
      m_saveContinueList=*m_oldContinue;
      *m_oldContinue=0;
    };
    ~SAVECONTINUELIST (void) {
      *m_oldContinue = m_saveContinueList;
    };
  };




  DICTIONARY *m_root;
  DICTIONARY *m_current;
  DICTIONARY *m_context;
  DEFINITION *m_activeFunction;
  GLOBALS    *m_globals;
  int         m_programIndex;
  //unsigned int m_numSysFunc;
  unsigned int m_breaklist; //=0xffffffff
  unsigned int m_continuelist; //=0xffffffff;
  bool         m_compilingScript;
public:
  DWORD        m_availableContexts;
private:

  void discardCurrent(void);
           // Pop the current dictionary and throw it away.
  int compileVariableDecl(void);
  int compileAtomicElement(void);
  int compileExpression(void);
  int compileVarAssignment(void);
  int compileTypedSystemFunctionCall(int type);
  int compileActualParameters(DEFINITION *def );
  int compileFunctionCall(DEFINITION *func);
  int compileStatement();
  int compileFunctionDecl(unsigned int attributes=0);
  int compilePublicDecl(void);
  int compileWhile(void);
  int compileIf(void);
  int compileSwitch(void);
  int compileReturn(void);
  int compileBlock(CString& blockname);
  int switchCompileExpression(void);
  int respondGetPattern(void);
  int respondSayText(void);
  int compileRespond(void);
  int compileBreak(void);
  int compileContinue(void);
  int compileVariableReference(DEFINITION *def);
  int compileVariableStore(DEFINITION *def);
  int compileForceNumeric(void);
  int switchCheckNoDefault(bool& Default);
  int switchCheckForColon (void);
  int respondCheckComma(void);
  int respondCloseParen(void);
  void switchFillPrevCase(int& prevJumpCase);
  void compileFalse(void);
  void addToBreakList (void);
  void resolveBreaks(unsigned int addr);
  void addToContinueList (void);
  void switchFillCodeJump (int& prevCodeJump);
  void resolveJump(unsigned int address);
  void switchCompileDUP(void);
  void resolveContinues(unsigned int addr);
  void compilePOP(void);
  void defineFormalParameters(DEFINITION *func, DICTIONARY *dict);
  void compileJump(unsigned int destination);
  void InitializeGPDLcompiler(void);
  unsigned int compileJumpFalse(void);
  unsigned int switchCompileJumpGREP  (void);
  unsigned int switchCompileJNE (void);
  unsigned int compileJump(void);
  unsigned int switchCompileJumpOver();
  TOKENTYPE GetToken(void);
  unsigned int GetCodeSize(void);
  unsigned int *GetCodeAddress(void);
public:
  GPDLCOMP(void);
  ~GPDLCOMP(void);
  int CompileProgram(
                  const char *GetOneLine(int n), 
                  int ProcessErrorMessage(const CString& errMsg, bool wait),
                  bool compilingScript=false);
  CString CompileProgram(const char *text);
  CString CompileProgram(FILE *f);
  CString CompileScript(const CString& script, const char *entryPoints[]);
  void WriteDictionary(CArchive& outarchive);
  void WriteCode(CArchive& outarchive);
  void WriteCode(RAM_FILE& outarchive);
  void WriteConstants(CArchive& outarchive);
  void WriteDictionary(RAM_FILE& result);
  void WriteConstants(RAM_FILE& result);
  void list(FILE *file);

  void GetSpellFunctionNames(TEXTLIST &list);
  void GetEventFunctionNames(TEXTLIST &list);
  void GetLogicBlockFunctionNames(TEXTLIST &list);
  void GetSpecialAbilityFunctionNames(TEXTLIST &list);
};


class RDRCOMP                                                           //RDR
{  //RuntimeDataReference Compiler                                      //RDR 
  char *m_code;                                                         //RDR
  int m_inputIndex;                                                     //RDR
  int m_codeSize;                                                       //RDR
  int m_codeIndex;                                                      //RDR
  int m_integer; // computed for CTKN_INTEGER.                          //RDR
  void m_Initialize(void);                                              //RDR
  char m_nextChar(void);                                                //RDR
  bool m_initialChar(char c);                                           //RDR
  bool m_moreChar(char c);                                              //RDR
  bool m_numeric(char c);                                               //RDR
  bool m_whitespace(char c);                                            //RDR
  CTOKENTYPE m_backspaceTkn;                                            //RDR
  CTOKENTYPE m_latestTkn;                                               //RDR
  CString m_token;                                                      //RDR
  CTOKENTYPE m_getRawToken(void);                                       //RDR  
  CTOKENTYPE m_GetToken(void);                                          //RDR
  CTOKENTYPE m_NextToken(void);                                         //RDR  
  void m_backspace(void);                                               //RDR
  void m_backspaceToken(void);                                          //RDR
  void m_commaBYTE(char c);                                             //RDR
  void m_commaINT(unsigned int binCode);                                //RDR
  void m_commaOP(COPS binCode);                                         //RDR
  void m_error(char *errorMsg, bool print=false);                       //RDR
  int  m_EvaluateExpression(void);                                      //RDR
  int  m_EvaluateAtomicElement(void);                                   //RDR
  CString m_expr; // The expression to compile.                         //RDR
  CString m_Result(void);                                               //RDR
  CString m_errorMsg;                                                   //RDR
  int  (*m_compileRDR)(CString&, char *, int arraySize);                //RDR
public:                                                                 //RDR
  RDRCOMP(void);                                                        //RDR
  ~RDRCOMP(void);                                                       //RDR
  CString CompileExpression(const CString& expr,                        //RDR
                            int (*RDR)(CString&, char *, int size));    //RDR
  CString ErrorMessage(void) {return m_errorMsg;};                      //RDR
};                                                                      //RDR
                                                                        //RDR
#endif // ifndef __GPDLCOMP_H__