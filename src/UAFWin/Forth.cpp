#include <stdafx.h>
#include "Externs.h"
#include "CombatSummary.h"

void die(CString msg);
void NotImplemented(int code, bool loop);
void WriteDebugString(const char *ptext, ... );
COMBAT_SUMMARY           *pCS      = NULL;
COMBAT_SUMMARY_ACTION    *pActionA = NULL;
COMBAT_SUMMARY_ACTION    *pActionB = NULL;
const COMBAT_SUMMARY_ACTION    *pCSA     = NULL;
const COMBAT_SUMMARY_COMBATANT *pCSC     = NULL;

#define stringer(x) " "#x
#define string(x) stringer(x)


int nextPrim = 1;
int Exit;




const int MAX_MEM = 10000;
//typedef _int64 i64;
//typedef unsigned _int64 ui64;
typedef _int32 i32;
typedef unsigned _int32 ui32;
typedef _int16 i16;
typedef unsigned _int16 ui16;

ui16 SP;
ui16 PC;
ui16 cfa;
ui16 RP;



#define m_DP          2
#define sm_DP            string(m_DP)  // OR " 2"
#define m_LATEST      4
#define sm_LATEST        string(m_LATEST)
#define m_tickSOURCE  6 //count and address
#define m_2IN        10
#define sm_2IN           string(m_2IN)
#define m_STATE      12
#define sm_STATE         string(m_STATE)
#define m_BASE       14
#define DP0          16

#define SP0 MAX_MEM-1000
#define RP0 MAX_MEM






void Kf_comma(void);
void Kf_HEAD(void);
void Kf_WORD(void);
void Kf_FIND(void);
void Kf_at(void);
void Kf_docolon(void);
void Kf_ABORT();
void Kf_TYPE();
void Kf_EMIT();
void Kf_CR();
void Kf_ABORT();
void Kf_LIT(void);
void Kf_oneMinus(void);
void Kf_Cat(void);
void Kf_tick(void);
void Kf_store(void);
void Kf_Cstore(void);
void Kf_plusStore(void);
void Kf_XOR(void);
void Kf_EXIT(void);
void Kf_QuestionEXIT(void);
void Kf_openBracket(void);
void Kf_closeBracket(void);
void Kf_SETIMMEDIATE(void);
void Kf_0less(void);
void Kf_DUP(void);
void Kf_SWAP(void);
void Kf_OVER(void);
void Kf_ROT(void);
void Kf_NOP(void);
void Kf_CREATE(void);
void Kf_plusP(void);
void Kf_docon(void);
void Kf_2R(void);
void Kf_R2(void);
void Kf_OR(void);
void Kf_AND(void);
void Kf_ABS(void);
void Kf_minus(void);
void Kf_plus(void);
void Kf_UMgear(void);
void Kf_Mgear(void);
void Kf_DROP(void);
void Kf_ABS(void);
void Kf_DNEGATE(void);
void Kf_BRANCH(void);
void Kf_QBRANCH(void);
void Kf_MUslashMOD(void);
void Kf_DEBUG(void);
void Kf_EXECUTE(void);
void Kf_NEGATE(void);
void Kf_less(void);
void Kf_greater(void);
void Kf_ShieldDotNext(void);
void Kf_ShieldDotReadyBang(void);
void Kf_QDUP(void);
void Kf_SPplusMinus(void);
void Kf_FleeingAt(void);
void Kf_CcolonState(void);
void Kf_CcolonDistance(void);
void Kf_CcolonFriendly(void);
void Kf_CcolonAIBaseclass(void);
void Kf_CcolonHasLineOfSight(void);
void Kf_Me(void);
void Kf_He(void);
void Kf_A(void);
void Kf_B(void);
void Kf_AcolonType(void);
void Kf_AcolonDamage(void);
void Kf_WcolonType(void);
void Kf_WcolonRange(void);
void Kf_WcolonProtection(void);
void Kf_WcolonDamage(void);
void Kf_WcolonROF(void);
void Kf_WcolonAttackBonus(void);
void Kf_WcolonPriority(void);


void Kf_equal(void);
void Kf_NOT(void);
void Kf_notEqual(void);

void (*Kf[])(void) = {
  NULL,
  Kf_CREATE,
  Kf_plusP,
  Kf_tick,
  Kf_LIT,     
  Kf_at,
  Kf_NOP,
  Kf_SETIMMEDIATE,
  Kf_openBracket,
  Kf_closeBracket,
  Kf_0less,
  Kf_Cat,
  Kf_Cstore,
  Kf_plusStore,
  Kf_store,
  Kf_oneMinus,
  Kf_DUP,
  Kf_ROT,
  Kf_EXIT,
  Kf_QuestionEXIT,
  Kf_SPplusMinus,
  Kf_docolon,
  Kf_BRANCH,
  Kf_QBRANCH,
  Kf_OVER,
  Kf_HEAD,
  Kf_WORD,
  Kf_FIND,
  Kf_SWAP,
  Kf_2R,
  Kf_R2,
  Kf_OR,
  Kf_XOR,
  Kf_AND,
  Kf_ABS,
  Kf_DROP,
  Kf_plus,
  Kf_minus,
  Kf_NEGATE,
  Kf_DNEGATE,
  Kf_UMgear,
  Kf_Mgear,
  Kf_equal,
  Kf_NOT,
  Kf_notEqual,
  Kf_less,
  Kf_greater,
  Kf_QDUP,
  Kf_MUslashMOD,
  Kf_DEBUG,
  Kf_EXECUTE,
  Kf_docon,
  Kf_Me,
  Kf_He,
  Kf_A,
  Kf_B,
  Kf_AcolonType,
  Kf_AcolonDamage,
  Kf_WcolonType,
  Kf_WcolonRange,
  Kf_WcolonProtection,
  Kf_WcolonDamage,
  Kf_WcolonROF,
  Kf_WcolonAttackBonus,
  Kf_WcolonPriority,
  Kf_ShieldDotNext,
  Kf_ShieldDotReadyBang,
  Kf_FleeingAt,
  Kf_CcolonState,
  Kf_CcolonDistance,
  Kf_CcolonFriendly,
  Kf_CcolonAIBaseclass,
  Kf_CcolonHasLineOfSight,
};


#define t_comma        " " sm_DP " DUP @ 2 ROT +! !"
#define t_stateStore   " " sm_STATE " !"

char m[MAX_MEM] =  
" CREATE"
" +p"
//  The first two primitives are compiled by ExpandKernel()
//  The rest are defined by INTERPRET in the normal manner.
" CREATE ' +p"
" CREATE LIT +p"
" CREATE @ +p"
" CREATE NOP +p"  // Can be used to insert debug break.
" CREATE SETIMMEDIATE +p"
" CREATE [ +p " sm_LATEST " @ SETIMMEDIATE"
" CREATE ] +p" 
" CREATE 0< +p"
" CREATE C@ +p"
" CREATE C! +p"
" CREATE +! +p"
" CREATE ! +p"
" CREATE 1- +p"
" CREATE DUP +p"
" CREATE ROT +p"
" CREATE EXIT +p"
" CREATE ?EXIT +p"  // (false ..) ; (true .. true) EXIT
" CREATE SP+- +p"
" CREATE docolon +p"   
      sm_LATEST  " @ 1- C@" 
      " ]  LIT  ["  t_comma 
      " ]"  sm_LATEST " @ 1- C! EXIT ["
" CREATE -->OP docolon ] 1- EXIT ["
" CREATE OP! docolon ] -->OP C! EXIT [ -2 SP+-"
" CREATE PRIM docolon ] CREATE +p EXIT ["
" PRIM BRANCH"
" PRIM ?BRANCH"
" PRIM OVER"
" PRIM HEAD"
" PRIM WORD"
" PRIM FIND"
" PRIM SWAP"
" PRIM >R"
" PRIM R>"
" PRIM OR"
" PRIM XOR"
" PRIM AND"
" PRIM ABS"
" PRIM DROP"
" PRIM +"
" PRIM -"
" PRIM NEGATE"
" PRIM DNEGATE"
" PRIM UM*"
" PRIM M*"
" PRIM ="
" PRIM NOT"
" PRIM !="
" PRIM <"
" PRIM >"
" PRIM ?DUP"
" PRIM MU/MOD"
" PRIM DEBUG"
" PRIM EXECUTE"
" CREATE , docolon ]" sm_DP " DUP @ 2 ROT +! !  EXIT [ -1 SP+-" 
" PRIM docon"  sm_LATEST " @ -->OP C@ docolon"  // Fetch and replace the docon primitive index
       " ] LIT [ , ]" sm_LATEST " @ OP! EXIT ["
" CREATE CONSTANT docolon ] CREATE docon , EXIT [ -1 SP+-"
" H'20'      CONSTANT BL"
  sm_2IN    " CONSTANT >IN"
  sm_LATEST " CONSTANT LATEST"
  sm_DP     " CONSTANT DP"
  sm_STATE  " CONSTANT  STATE"
" CREATE HIDE   docolon ] H'04' LATEST @ 2 - DUP >R C@ OR  R> C! EXIT ["
" CREATE REVEAL docolon ] H'FB' LATEST @ 2 - DUP >R C@ AND R> C! EXIT ["
" CREATE : docolon ] CREATE  docolon HIDE ] EXIT ["
//_:;     docolon LATEST @ REVEAL LIT EXIT ,XT [ EXIT _IMMEDIATE 
" : IMMEDIATE LATEST @ SETIMMEDIATE EXIT ["
" : ; REVEAL [ ' [ , ] LIT EXIT , EXIT [ IMMEDIATE"  
" : 2DUP OVER OVER ;"
" : ['] LIT LIT , ' , EXIT [ IMMEDIATE"
    

/*
_:BASE         docon  _4       _,
_:DP           docon  _8       _,
_:'SOURCE      docon  _10      _,
_:LATEST       docon  _14      _,
_:HP           docon  _16      _,
_:LP           docon  _18      _,
_:pABORT       docon  _20      _,   
_:LEAVE_LIST   docon  _22      _,
_:SCRADDR      docon  _24      _,  
_:ENV          docon  _26      _, _\ Debug flags and other flags that
                                  _\ affect how the program works.
_:HEXCHAR      docon  _28      _, _\ The first hex character. (usually 'a' or 'A')
_:PAD          docon  _0x128   _,
_:L0           docon  _0x180   _,
_:FALSE        docon  _0       _,
_:TRUE         docon  _0xffff  _,
_:X10PIPE      docon  _23      _,

_:%+           docon  _0x0001 _, _\ Format flag - Force '+' or '-' sign.
_:%u           docon  _0x0002 _, _\ Format flag - Unsigned
_:%0           docon  _0x0004 _, _\ Format flag - zero fill (vs blank fill)
_:%d           docon  _0x0008 _, _\ Format flag - Force decimal conversion
_:%x           docon  _0x0010 _, _\ Format flag - Force hex conversion (lower case)
_:%X           docon  _0x0020 _, _\ Format flag - Force hex conversion (lower case)
_:%u0X         docon  _0x0026 _, 
_:%0d          docon  _0x000c _,
_:%ud          docon  _0x000a _,

_:PRIV-ANY     docon  _0x0001 _,
_:PRIV-DEBUG   docon  _0x0004 _,

_:1-!   docolon
    DUP @ 1- SWAP ! EXIT

_:INITIALIZE   docolon
          0     >IN     !
          10    BASE    !
          0     STATE   !
          0x200 DP      !
          0     LATEST  !
          0     SCRADDR !
          0     ENV     !
          0x41  HEXCHAR !
          EXIT
          
_\ Double operations
_\
_:2@        docolon DUP CELL+ @ SWAP @ EXIT
_:2!        docolon SWAP OVER ! CELL+ ! EXIT
_:2DROP     docolon DROP DROP EXIT
_:2SWAP     docolon ROT >R ROT R> EXIT
_:2OVER     docolon >R >R 2DUP R> R> 2SWAP EXIT
_\
_\
_\ Dictionary Management
_\
_:HERE      docolon     _\  -- addr             current dictionary pointer
    DP @ EXIT
    
_:ALLOT     docolon     _\  n --                allocate n bytes in dictionary
    DP +! EXIT
    
_:ALIGN     docolon     _\ --                   Align DP
    DP @ ALIGNED DP ! EXIT
    
_:,         docolon     _\ x --                 append cell to dictionary
    HERE !   1 CELLS ALLOT  EXIT
    
_:,XT       docolon , EXIT    
    
_:,CF       docolon , EXIT    
    
_:C,        docolon     _\ char --              append char to dictionary
    HERE C!  1 CHARS ALLOT  EXIT
    
_:-->CFA    _\ wordAddr -- CFAAddr              move pointer to CFA field
    docolon EXIT
    
_:COUNT     docolon DUP CHAR+ SWAP C@ EXIT    _\ addr .. addr+1 count

_:(S")      docolon R> COUNT 2DUP + ALIGNED >R EXIT


_:VALIDWORD?        _\ wordAddr -- flag
    docolon
    DUP 0= _IF EXIT _THEN
    LATEST @
    _BEGIN
        DUP -1 XOR
    _WHILE
        2DUP =
        _IF
            2DROP -1 EXIT
        _THEN
        -->NEXT
    _REPEAT
    2DROP 0 EXIT

_:-->NAME   _\ wordAddr -- caddr u              Get a word's name
    docolon
    DUP VALIDWORD?
    _IF
        HERE NAME@ HERE
    _ELSE
        DROP (S") _"???" EXIT
    _THEN
    COUNT EXIT
    
_\ 
_\
_\ Arithmetic
_\
_:0>        docolon 0 > EXIT
*/
" : NIP SWAP DROP ;"
/*
_:<=        docolon > 0= EXIT
_:>=        docolon < 0= EXIT
*/
" : S>D DUP 0< ;"
/*
_:?NEGATE   docolon 0<  _IF NEGATE _THEN EXIT
_:ABS       docolon DUP ?NEGATE EXIT
_:M+        docolon DUP 0< D+ EXIT 
_:DNEGATE   docolon SWAP INVERT SWAP INVERT 1 M+ EXIT
_:?DNEGATE  docolon 0< _IF DNEGATE _THEN EXIT
_:DABS      docolon DUP ?DNEGATE EXIT
_:M*        docolon 2DUP XOR >R SWAP ABS SWAP ABS UM* R> ?DNEGATE EXIT
_:UM/MOD    docolon MU/MOD DROP EXIT
*/
" : HERE DP @ ; 1 SP+-"
" : >MARK HERE 0 , ; 1 SP+-"
" : >RESOLVE HERE SWAP ! ; -1 SP+-"
" : 2+ 2 + ; "
" : COMPILE R> DUP @ , 2+ >R ;"
" : IF COMPILE ?BRANCH  >MARK ; IMMEDIATE 1 SP+-"
" : ELSE COMPILE BRANCH >MARK SWAP >RESOLVE ; IMMEDIATE"
" : THEN >RESOLVE ; IMMEDIATE -1 SP+-"
" : ?NEGATE 0< IF NEGATE THEN ;"
" : ABS DUP 0< IF NEGATE THEN ;"
" : DABS  DUP 0< IF DNEGATE THEN ;"
" : UM/MOD MU/MOD DROP ;"
" : SM/REM"               // d n -- rem quot    ( Symmetrical)
"     2DUP XOR >R"        // sign of quotient
"     OVER >R"            // sign of remainder
"     ABS >R DABS R>" 
"     UM/MOD SWAP"        // quot rem
"     R> ?NEGATE"         // fix remainder sign
"     SWAP R> ?NEGATE ;"   // fix quotient sign
                    
" : FM/MOD"
"      DUP >R"            // Save divisor
"      SM/REM"            // rem quot
"      DUP 0<" 
"      IF"                // if quot negative
"        SWAP R> + SWAP"  // add divisor to remainder
"        1-"              //  and decrement quotient
"      ELSE" 
"        R> DROP" 
"      THEN ;"
" : SM/REM"                 // d n -- rem quot    ( Symmetrical)
     " 2DUP XOR >R"         // sign of quotient
     " OVER >R"             // sign of remainder
     " ABS >R DABS R>" 
     " UM/MOD SWAP"         // quot rem
     " R> ?NEGATE"          // fix remainder sign
     " SWAP R> ?NEGATE"     // fix quotient sign
     " ;"

" : * M* DROP ;"
" : /MOD >R S>D R> FM/MOD ;"
" : / /MOD NIP ;"
" : MOD /MOD DROP ;"
" : */MOD >R M* R> FM/MOD ;"
" : U/MOD 0 SWAP MU/MOD DROP ;"
" : */  */MOD NIP ;"
" : MAX 2DUP < IF SWAP THEN DROP ;"
" : MIN 2DUP > IF SWAP THEN DROP ;"
" : CHAR  BL WORD  1 + C@  ;  1 SP+-"
" : [CHAR] CHAR LIT LIT , , ; IMMEDIATE"
" : ( [CHAR] ) WORD DROP ; IMMEDIATE"
" : \\ 0 WORD DROP ; IMMEDIATE"
/*
_\
_\
_\
_\ Strings
_\
_:CHAR=  _\ ( char delimiter -- flag) _\ If delimiter is a blank then
                                      _\ match any whitespace.
    docolon
    DUP BL =
    _IF
    DROP
    DUP BL =
    SWAP 9 = 
        OR
    _ELSE
    =
    _THEN
    EXIT

_:SKIP      docolon     _\ c-addr u ch -- c-addr' u'     skip leading characters
    >R                  _\ c-addr u                     save character to match
    _BEGIN              _\ c-addr u
        DUP 0=
        _IF
            R> DROP     _\ drop c
            EXIT        _\ c-addr' 0
        _THEN
        OVER C@         _\ c-addr' u' ch
        R@ CHAR= 0=     _\ If looking for blank, match whitespace 
        _IF             _\ c-addr' u'
            R> DROP     _\ drop c
            EXIT
        _THEN
        SWAP CHAR+ SWAP 1- _\ c-addr' u'
    _AGAIN
_\
_\
_:SCAN      docolon     _\ c-addr u ch -- c-addr' u'     Find matching char
    >R
    _BEGIN              _\ c-addr u
        DUP 0=
        _IF
            R> DROP     _\ drop ch
            EXIT        _\ c-addr' 0
        _THEN
        OVER C@         _\ c-addr' u' c
        R@ CHAR=    _\ If looking for blank, match whitespace.
        _IF             _\ c-addr' u'
            R> DROP
            EXIT
        _THEN
        SWAP CHAR+ SWAP 1- _\ c-addr' u'
    _AGAIN
_\    
_\
_\ Input and Output
_\
_:CR        docolon 0x0d EMIT 0x0a EMIT EXIT
_:SPACE     docolon BL EMIT EXIT
_:SPACES    docolon _BEGIN DUP _WHILE SPACE 1- _REPEAT DROP EXIT
_:UMIN      docolon 2DUP U> _IF SWAP _THEN DROP EXIT
_:UMAX      docolon 2DUP U< _IF SWAP _THEN DROP EXIT

_:ACCEPT    docolon     _\ c-addr +n -- +n'     get line from terminal ( -- #chars)
    OVER + 1- OVER      _\ sa ea a
    _BEGIN
        KEY DUP 0x0A <>
    _WHILE
        _\ DUP EMIT        _\ sa ea a c
        _\ DUP 8 =
        _\ _IF
        _\     DROP 1-   >R OVER R> UMAX
        _\ _ELSE
            OVER C! 1+ OVER UMIN
        _\ _THEN           _\ sa ea a
    _REPEAT             _\ sa ea a c
    DROP NIP SWAP - EXIT
    
_:TYPE      docolon     _\ c-addr u -- 
    ?DUP 
    _IF 
        OVER + SWAP
        _DO
            I C@ EMIT
        _LOOP
    _ELSE
        DROP
    _THEN EMIT-FLUSH EXIT
    
_:SOURCE    docolon     _\  -- adr n        current input buffer
    'SOURCE 2@ EXIT     _\                  length is at lower address

_:/STRING   docolon     _\  a u n -- a+n u-n    trim string
    ROT OVER + ROT ROT - EXIT

_:\         docolon
    SOURCE >IN ! DROP EXIT _IMMEDIATE

_:>COUNTED  docolon     _\ src n dst --     copy to counted string
    SWAP 0x3f MIN SWAP
    _\ (src n dst)
    2DUP C! 
    _\ (src n dst)
    CHAR+ SWAP 
    _\ (src dst+1 n)
    CMOVE EXIT

_:COUNTED>  docolon     _\ c-addr -- caddr1 u
    DUP C@ SWAP 1+ SWAP EXIT

_:BLANK docolon 32 FILL EXIT

_:WORD      docolon     _\ ( char -- c-addr)      word delimited by char
    DUP SOURCE >IN @ /STRING    _\  -- c c-adr n
    DUP >R ROT SKIP             _\  -- c-adr' n'
    OVER >R ROT SCAN            _\  -- adr" n"
    DUP
    _IF
        CHAR-
    _THEN                       _\ skip trailing delim.
    R> R> ROT -   >IN +!        _\ update >IN offset
    TUCK -                      _\ -- adr' n
    HERE >COUNTED               _\ --
    HERE                        _\ a
    BL OVER COUNT + C!          _\ append trailing blank
    EXIT
    
_:S"        docolon
    STATE @
    _IF
        LIT (S") ,XT 0x22 WORD
        C@ 1+ ALIGNED ALLOT 
    _ELSE
        0x22 WORD DROP HERE
        HERE 60 + HERE C@ 1+ CMOVE
        HERE 60 + DUP 1+ SWAP C@
    _THEN
    EXIT _IMMEDIATE
                    
_:."        docolon 
    S"  LIT TYPE , EXIT _IMMEDIATE                    
    
_\
_\ Numeric output
_\
_\ Numeric conversion is done least significant digit first, so
_\ the output buffer is built backwards in memory.
_\
_\ Some double-preceision arithmetic operations are
_\ needed to implement ANSI numeric conversion.

_:UD/MOD    docolon     _\ ud1 u2 -- u3 ud4     32/16 -> quote32 + rem16 divide
    >R 0 R@  UM/MOD  -ROT R>  UM/MOD  ROT  EXIT
    
_:UD*       docolon     _\ ud1 d2 -- ud3        32*16 -> 32 multiply
    DUP >R UM* DROP   SWAP R> UM* ROT + EXIT
    
_:HOLD      docolon     _\ char --              add character to output string
    -1  HP +!   HP @ C! EXIT
    
    
_:<#        docolon     _\ --                   begin numeric conversion
    PAD HP ! EXIT

_:CHAR          _\  -- char
    docolon
    BL WORD  1+ C@ EXIT 
    
_:[CHAR]        _\ Compile character literal
    docolon
    CHAR LIT LIT ,XT , EXIT _IMMEDIATE
    


    
_:>DIGIT    docolon     _\ n -- c               convert to 0..9A..Z
    DUP 9 > 7 AND + 0x30 + EXIT
    
_:#         docolon     _\ ud1 -- ud2           convert one digit of output
    BASE @ UD/MOD  ROT >DIGIT  HOLD EXIT
    
_:#S        docolon     _\ ud1 -- ud2           convert remaining digits
    _BEGIN # 2DUP OR 0= _UNTIL EXIT
    
_:#>        docolon     _\ ud1 -- c-addr u      end conversion, get string
    2DROP HP @ PAD OVER - EXIT
    
_:SIGN      docolon     _\ n --                 add minus sign if negative
    0< _IF  0x2d HOLD _THEN EXIT



_:<##>  _\ A more general '#' -  Formatted #  ( ud R flags -- caddr u )
        _\ R is the (minimum) width of the resulting field of ascii characters.
        _\ See the definitions of the % words (eg: "%d") to see the available flags.
        _\ We honor the d, x, X, and 0 flags.
    docolon
    BASE @ >R       _\ Save original base
    HEXCHAR @ >R    _\ Save hex conversion character
    >R              _\ Save the flags
    BASE @
      R@ %d AND 
      _IF 
        DROP 10
      _ELSE  
        R@ %x %X OR AND _IF DROP 16 _THEN
      _THEN
    BASE !
    _\  Parameter:( ud R )   Return:( origBASE origHEXCHAR flags )
    _\
    _\ Now we determine the characters to use for hex digits
    0x41 R@ %x AND _IF 0x20 + _THEN HEXCHAR ! 
    _\
    _\ Now we determine the fill character.
    R> %0 AND   _IF 0x30 _ELSE BL _THEN >R
    >R
    _\  Parameter ( ud )   Return:( origBASE origHEXCHAR fill R )
    _\
    _\ We are ready to begin the conversion to ASCII.
    <# #S           _\ Convert the integer
    _\ Now we may need to add some fill characters.
    PAD HP @ - R> SWAP - _\ ( 0d #FillCharacters )
    DUP 0>
    _IF
        R> SWAP
        0 
        _DO
            DUP HOLD
        _LOOP
        DROP
    _ELSE
        DROP R> DROP
    _THEN
    _\  Parameter:( 0d )   Return:(origBASE origHEXCHAR )
    #> 
    _\  Parameter:( caddr u )  Return:( origBASE origHEXCHAR )
    
    R> HEXCHAR !    _\ Restore original HEXCHAR
    R> BASE !       _\ Restore original BASE.
    EXIT
    
    
_:U.        docolon     _\ u --                 display unsigned u
    0 0 %u    <##> TYPE SPACE EXIT

_:%F04X     docolon     _\ u --                 display 4 digit unsigned hex
    0 4 %u0X <##> EXIT 
    
_:%FD       docolon
    0 0 %d <##> EXIT

_:%F02D     docolon
    0 2 %0d <##> EXIT

_:%F03D     docolon
    0 3 %0d <##> EXIT

_:.         docolon     _\ n --                 display signed n
    <# DUP ABS 0 #S  ROT SIGN #> TYPE SPACE EXIT
    
_:D.        docolon
    <# DUP >R DABS #S R> SIGN #> TYPE SPACE EXIT
    
_:DECIMAL   docolon     _\ --                   set base to decimal
    10 BASE ! EXIT
    
_:HEX       docolon     _\ --                   set base to hex
    16 BASE ! EXIT
    
_:LITERAL   docolon     _\ x --                 append numeric literal
    _\ This tests STATE so that it can also be used
    _\ interpretively.  ANSI does not require this.
    STATE @
    _IF
        LIT LIT ,XT ,
    _THEN EXIT   _IMMEDIATE
  
_:DIGIT?    docolon     _\ c -- n -1  if c is valid digit
    DUP 0x2f >
    _IF
        DUP 0x39 >
        _IF
            0xdf AND DUP 0x40 >    
            _IF
                0x7 -
            _ELSE
                DROP -1
            _THEN
        _THEN
    _ELSE
        DROP -1
    _THEN
    0x30 - DUP BASE @ U< 
    EXIT
    
_:?SIGN     docolon     _\ adr n -- adr' n' f   Get optional sign.  Advance
                        _\                      adr and n if sign.  Return
                        _\                      NZ if negative.
    DUP
    _IF
      OVER C@                 _\ adr n c
      0x2C - DUP ABS 1 = AND  _\ +=-1, -=+1, else 0
      DUP
      _IF
          1+                  _\ +=0, -=+2
          >R 1 /STRING R>     _\ adr' n' f
      _THEN
    _ELSE
      DUP  _\ DUP the zero!
    _THEN
    EXIT

_:TESTBASE _\ ( addr u -- addr u base) _\ is string of form H'nnn'
    docolon
    OVER 1+ C@ 0x27 =
    _IF
        2DUP + 1- C@ 0x27 =
        _IF
            OVER C@
            DUP 0x48 =
            _IF
                0x10
            _ELSE
                DUP 0x44 =
                _IF
                    0x0a
                _ELSE
                    DUP 0x4f =
                    _IF
                        0x08
                    _ELSE
                        0
                    _THEN
                _THEN
            _THEN
            SWAP DROP
        _ELSE
            0
        _THEN
    _ELSE
        0
    _THEN
    EXIT
    
_:>NUMBER   docolon     _\ ud adr u -- ud' adr' u'      convert string to number
    BASE @ >R        _\ In case we change it!
    TESTBASE DUP    _\ ( addr u -- addr u base)   Set BASE if form H'nnn', D'nn', etc.
    _IF
        BASE ! 3 - SWAP 2 + SWAP
    _ELSE
        DROP
    _THEN
    _BEGIN
        DUP
    _WHILE
        OVER C@ DIGIT? 0=
        _IF
            DROP R> BASE ! EXIT
        _THEN
        >R 2SWAP BASE @ UD*
        R> M+ 2SWAP
        1 /STRING
    _REPEAT 
    R> BASE ! EXIT
    
_:?NUMBER   docolon           _\ c-addr -- n -1      string->number
    DUP C@                    _\        -- c-addr 0  if convert error
    _IF
      DUP 0 0 ROT COUNT       _\ ca ud adr n
      ?SIGN >R >NUMBER        _\ ca ud adr' n'
      _IF
          R> 2DROP 2DROP 0    _\ ca 0         error
      _ELSE
          2DROP NIP R>
          _IF NEGATE _THEN -1  _\ n -1         OK
      _THEN 
   _ELSE
     0
   _THEN
   EXIT

_:FIND      docolon     _\ c-addr -- c-addr 0 | xt 1 | xt -1
    LATEST  @
    _\ (c-addr defn)
    _BEGIN
        _FIND_          _\ ( c-addr defn -- c-addr defn
                        _\               -- c-addr -1
        DUP -1 =
        _IF
            DROP 0 EXIT
        _THEN
        _\ ( c-addr defn)
        DUP HIDDEN? 0=
        _IF    
            NIP DUP  IMMEDIATE?
            _IF 1 _ELSE -1 _THEN
            EXIT
        _THEN
        -->NEXT
    _AGAIN _\ EXIT not needed

_\ _:FIND      docolon     _\ c-addr -- c-addr 0 | xt 1 | xt -1
_\     LATEST  @
_\     _BEGIN
_\         DUP -1 XOR
_\     _WHILE
_\         2DUP NAME=   _\ Search user dictionary then system dictionary
_\         _IF
_\             DUP HIDDEN? 0=
_\             _IF
_\                 NIP DUP  IMMEDIATE?
_\                 _IF 1 _ELSE -1 _THEN
_\                 EXIT
_\             _THEN
_\         _THEN
_\         -->NEXT
_\     _REPEAT
_\     DROP 0
_\     EXIT

_:-->BODY     docolon     _\ FIND result to body of definition
    CELL+ EXIT



_:INTERPRET docolon     _\ i*x c-addr u -- j*x      interpret given buffer
        _\ This is a common factor of EVALUATE and QUIT
        _\ ref. dpANS-6 3.4 The Forth Text Interpreter
    'SOURCE 2!   0 >IN !
    _BEGIN
        BL WORD DUP C@
    _WHILE
        FIND ?DUP
        _IF
             1+ STATE @ 0= OR        _\ immed or interp
            _IF  EXECUTE  _ELSE ,XT  _THEN 
        _ELSE
            ?NUMBER
            _IF
                LITERAL
            _ELSE 
                COUNT TYPE 0x3F EMIT CR
                pABORT @ EXECUTE
            _THEN
        _THEN
    _REPEAT DROP EXIT


_:CREATE    docolon         _\ --      create an empty definition
    BL WORD C@ DUP >R       _\ word length
    1 AND
    _IF                      _\ if odd, move up and discard count byte
        HERE DUP 1+ SWAP R@ CMOVE
    _ELSE                   _\ if even use the count byte as a pad
        1 ALLOT
    _THEN
    R@ ALLOT            _\ name field
    R>  C,              _\ name length and flags
    LATEST @ ,          _\ link
    HERE LATEST !       _\ new "latest" link
    LIT dovar ,CF EXIT        _\   code field
_\     head CREATE,6,CREATE,docolon
_\         DW LATEST,FETCH,COMMA,LIT,0,CCOMMA
_\         DW HERE,LATEST,STORE
_\         DW BL,WORD,CFETCH,ONEPLUS,ALLOT
_\         DW LIT,docreate,COMMACF,EXIT

    
_:IMMEDIATE docolon     _\ --
    LATEST @ SETIMMEDIATE EXIT
    
_:DOES>     docolon
    ALIGN
    LIT LIT ,XT HERE 0 ,XT LIT LATEST ,XT LIT @ ,XT LIT -->CFA ,XT LIT ! ,XT LIT EXIT ,XT HERE SWAP !
    EXIT _IMMEDIATE
    
_:CONSTANT      _\ n --
        docolon CREATE LIT docon LATEST @ -->CFA ! , EXIT _IMMEDIATE    
    
_:]     docolon 1 STATE ! EXIT

_:[     docolon 0 STATE ! EXIT _IMMEDIATE
    
 
_:RECURSE   docolon LATEST @ ,XT EXIT _IMMEDIATE 
 
 
_:VARIABLE  docolon CREATE 0 , EXIT 
 
_:DEPTH    _\ -- +n         _\ number of items on stack
    docolon
    SP@ S0 SWAP - 2/ EXIT  _\ 16-BIT VERSION!
 
_:.S  docolon SP@ S0 U<
    _IF
        SP@ S0 1 CELLS - _DO I @ U. 1 CELLS NEGATE _+LOOP 
    _THEN EXIT
 
 
_:QUIT  docolon
    L0 LP !  R0 RP! 0 STATE !
    _BEGIN
        STATE @ 0= 
        _IF
            CR 0x4f EMIT 0x4b EMIT SPACE EMIT-FLUSH
        _THEN
        TIB DUP TIBSIZE ACCEPT 
        INTERPRET 
        1
    _WHILE        
    _REPEAT
    
_:ABORT     docolon
    S0 SP!   QUIT



_:?ABORT        _\ f c-addr u --
    docolon
    ROT 
    _IF
        TYPE ABORT
    _THEN
    2DROP EXIT
    
_:ABORT"        _\ i*x 0  -- i*x     R: j*x -- j*x   x1 = 0    
                _\ i*x x1 --         R: j*x --       x1 <> 0
    docolon
    S" LIT ?ABORT ,XT EXIT _IMMEDIATE


_:POSTPONE      docolon
    BL WORD FIND
    DUP 0= _IF  DROP  COUNT TYPE (S") _"???" TYPE  ABORT _THEN
    0< 
    _IF                 _\ -- xt  non immed: add code to current
                        _\       def'n to compile xt later.
        LIT LIT ,XT  ,  _\   add "LIT,xt,COMMAXT"
        LIT ,XT ,XT     _\       to current definition
    _ELSE  ,XT          _\ immed: compile into cur. def'n
    _THEN 
    EXIT _IMMEDIATE


    
_:'             _\ -- xt        Find word in dictionary
    docolon
    BL WORD FIND 0=
    _IF
        SPACE COUNTED> TYPE 0x3f EMIT ABORT
    _THEN
    EXIT
    
_:[']   docolon ' LIT LIT ,XT , EXIT _IMMEDIATE 
 
_:(             _\ Skip input until ')'
    docolon
    0x29 WORD  DROP EXIT _IMMEDIATE
    

_:BEGIN docolon HERE EXIT _IMMEDIATE

_:IF docolon LIT ?BRANCH ,XT HERE 0 ,XT EXIT _IMMEDIATE 

_:WHILE docolon LIT ?BRANCH ,XT  HERE 0 ,XT EXIT _IMMEDIATE

_:ELSE docolon LIT BRANCH ,XT HERE 0 ,XT SWAP HERE SWAP ! EXIT _IMMEDIATE

_:THEN docolon HERE SWAP ! EXIT _IMMEDIATE

_:REPEAT docolon SWAP LIT BRANCH ,XT ,XT HERE SWAP ! EXIT _IMMEDIATE

_:AGAIN  docolon LIT BRANCH ,XT ,XT EXIT _IMMEDIATE

_:UNTIL  docolon
    LIT ?BRANCH ,XT , EXIT _IMMEDIATE

_:DO            _\ limit n --
    docolon
    LEAVE_LIST @ 0 LEAVE_LIST !
    LIT dodo ,XT
    HERE                _\ loopBack Address = HERE
    EXIT _IMMEDIATE

_:LEAVE         _\  Compile: leaveList loopAddr -- leaveList' loopAddr    
                _\  Runtime   --
    docolon
    LIT R> ,XT LIT R> ,XT LIT 2DROP ,XT
    LIT BRANCH ,XT
    LEAVE_LIST @ HERE SWAP , LEAVE_LIST !
    EXIT _IMMEDIATE

_:FILL_LEAVE_LIST   _\  --
    _\ For each address in list...plant the value HERE
    docolon
    LEAVE_LIST @
    _BEGIN
        DUP 
    _WHILE
        DUP @ SWAP      _\ next this
        HERE SWAP !
    _REPEAT
    DROP
    EXIT
    
_:LOOP
    docolon
    LIT doloop ,XT ,    _\ Place loopBack address
    FILL_LEAVE_LIST
    LEAVE_LIST !
    EXIT   _IMMEDIATE

_:+LOOP
    docolon
    LIT doplusloop ,XT ,    _\ Place loopBack address
    FILL_LEAVE_LIST
    LEAVE_LIST !
    EXIT   _IMMEDIATE

_:UNLOOP docolon R> R> R> 2DROP >R EXIT

    
_:DUMP          _\ addr u --
    docolon
    BASE @ >R HEX
    OVER 1 AND
    _IF
        OVER U. SPACE OVER C@ 8 LSHIFT U. CR
        SWAP 1+ SWAP
    _THEN
    _BEGIN
        DUP
    _WHILE
        OVER U. SPACE OVER @ U. 
        OVER @  -->NAME  TYPE
        CR
        1 - SWAP CELL+ SWAP
    _REPEAT
    2DROP R> BASE ! EXIT

_:NU*   _\ ( <Nwords> u N -- <Nwords>)
    docolon
    0 -ROT DUP 0
    _\ ( <Nwords 0 u N N 0)
    _DO
        _\ ( <Nwords carry u N)
        2DUP >R >R 1+ ROLL 
        _\ (<N-1words carry u lsw )
        UM* ROT 0 D+ 
        _\ (<Nwords carry )
        R> R>
    _LOOP 2DROP DROP EXIT

_:NU/MOD  _\ ( <Nwords> u N .. remainder <Nwords/u> )
    docolon
    0 -ROT DUP
    _BEGIN
      DUP
    _WHILE
      1- >R >R DUP >R MU/MOD DROP R> SWAP R> R> ROT >R
    _REPEAT DROP SWAP DROP
    _BEGIN
      DUP
    _WHILE
      1- R> SWAP
    _REPEAT DROP EXIT
      
      
_:N+    _\ ( <Nwords> <Nwords> N .. <Nwords> )
    docolon
    0 SWAP DUP 0
    _DO
      >R 0 R@ 1+ ROLL 0 D+
      R> DUP DUP + I - SWAP >R ROLL 0 D+ R>
    _LOOP 2DROP EXIT

_:NNEGATE    _\ ( <Nwords> N .. <Nwords> )
    docolon
    1 SWAP DUP 0
    _DO            _\ (nwords carry n)
        >R R@ ROLL
        0 SWAP INVERT 0 D+
        R>
    _LOOP
    2DROP EXIT

_:N-        _\ ( <Nwords N ..  <Nwords>)
    docolon
    DUP >R NNEGATE R> N+ EXIT

_:MALLOC _\ ( #bytes -- )
    1 + 0xfffe AND DUP
    DP +!
    , ;


_:SET-VM-NAME       _\ ( c-string -- )
    docolon
    HERE -1 OVER !  _\ Use active VM
    CELL+ 2 OVER !  _\ Select VM name
    CELL+ 1 OVER !  _\ Function = store
    CELL+ !         _\ pointer to new name
    HERE 10 VM-INFO EXIT
    

_:EVALUATE      _\ caddr u -- ??
    docolon
   'SOURCE 2@ >R >R  >IN @ >R
   INTERPRET
   R> >IN !  R> R> 'SOURCE 2! EXIT
    

_:STRCAT   _\ ( caddr1 u1 caddr2 u2 -- caddr1 u1+u2 )
    docolon
    2OVER 2OVER   _\ ( a1 u1 a2 u2 a1 u1 a2 u2 )
    NIP +         _\ ( a1 u1 a2 u2 a1 u1+u2 )
    >R  >R        _\ ( a1 u1 a2 u2 )    ( u1+u2 a1 )
    ROT           _\ ( a1 a2 u2 u1 )    ( u1+u2 a1 )
    R> +          _\ ( a1 a2 u2 a1+u1 ) ( u1+u2 )
    SWAP CMOVE    _\ ( a1 )             ( u1+u2 )
    R> EXIT

_\ ************************* TIME functions *****************************


_:TWAIT _\ ( deltaTime)  =  ( uuu -- )  u u u = milliseconds - three words
    docolon
    TIME@ 3 N+ WAIT
    EXIT

_:Time->Seconds _\ ( Time .. <3-word time in seconds>)
    docolon
    1000 3 NU/MOD 3 ROLL DROP EXIT

_:Time->HMS _\ ( Time -- hour minute second)
    docolon
    Time->Seconds        _\ ( 3-word-seconds)
    60 3 NU/MOD          _\ ( second 3-word-minute)
    60 3 NU/MOD          _\ ( second minute 3-word-hour)
    24 3 NU/MOD          _\ ( second minute hour 3-word-days)
    2DROP DROP -ROT SWAP EXIT

_:HMS   _\ ( -- hours minutes seconds)
    docolon TIME@ Time->HMS EXIT

_:Time->Minutes _\ ( Time .. <3-word time in minutes>)
    docolon
    Time->Seconds
    60 3 NU/MOD 3 ROLL DROP EXIT


_:Time->Days  _\ ( Time .. u ) ( Time .. days )
    docolon
    Time->Minutes 1440 3 NU/MOD 2DROP SWAP DROP EXIT

_:Days->MDY  _\ ( u .. u1 u2 u3 )  ( Days .. Month Day Year )
    docolon
    1401 +   _\ Make 1 MAR 1996 = Day 0.
    1461 U/MOD   _\  ( <Day within Olympiad> Olympiad)
    >R  365 /MOD DUP 4 =
    _IF 
        1- SWAP 365 + SWAP 
    _THEN  
    >R
    _\ ( Save year )
    DUP 184 < 
    _IF DUP 92 < 
        _IF DUP 31 < 
            _IF 
                1+ 3 
            _ELSE 
                DUP 61 < 
                _IF 30 - 4 
                _ELSE 60 - 5 
                _THEN 
            _THEN 
        _ELSE
            DUP 122 < 
            _IF 
                91 - 6 
            _ELSE 
                DUP 153 < 
                _IF 
                    121 - 7 
                _ELSE
                    152 - 8 
                _THEN 
            _THEN 
        _THEN 
    _ELSE 
        DUP 275 < 
        _IF 
            DUP 214 <
            _IF 
                183 - 9 
            _ELSE 
                DUP 245 < 
                _IF 
                    213 - 10 
                _ELSE 
                    244 - 11
                _THEN
            _THEN 
        _ELSE 
            DUP 306 < 
            _IF 
                274 - 12 
            _ELSE 
                DUP 337 <
                _IF 
                    305 - 13 
                _ELSE 
                    336 - 14 
                _THEN
            _THEN 
        _THEN
    _THEN
    _\ ( day month )
    DUP 12 > 
    _IF 
        12 - SWAP R> 1+   
    _ELSE
        SWAP R>  
    _THEN   
    R> 4 * + 1996 + EXIT



_:Time->MDY _\ (Time .. Month day year)
    docolon
    Time->Days Days->MDY EXIT

_:MDY       _\ ( -- month day year)
    docolon
    TIME@ Time->MDY EXIT

_:HMS->Time  _\ ( Hours Minutes Seconds -- time )
    docolon
    ROT 60 * ROT + 60 UM* ROT 0 D+ 0 1000 3 NU* EXIT

_:MDY->Days  _\ ( Month Day Year -- days)  Jan 1, 2000 = 0
    docolon
    1996 - ROT DUP 3 <
    _IF
    12 + SWAP 1- SWAP
    _THEN
    DUP 30 * SWAP 1+ 3 * 5 / + ROT +
    SWAP DUP 4 / SWAP 365 * + + 1494 - EXIT

_:MDY->Time  _\ ( Month Day Year -- time)
    docolon
    MDY->Days 1440 UM* 0 60000 3 NU* EXIT

_:Set-Time _\ ( month day year hour minute second )
    docolon
    HMS->Time >R >R >R MDY->Time R> R> R> 3 N+  TIME!
    EXIT
_\ *************************** Safe output (No controls or graphics) ****

_:SAFE-ASCII   _\ ( char -- char')
    docolon
    DUP 0x20 U<
    _IF 
        DROP 0x2e
    _ELSE
        DUP 0x7e U>
        _IF
            DROP 0x2e
        _THEN
    _THEN EXIT

_:SAFE-EMIT  _\ ( char --)
    docolon
    SAFE-ASCII EMIT EXIT


_:SAFE-TYPE      docolon     _\ c-addr u -- 
    ?DUP 
    _IF 
        OVER + SWAP
        _DO
            I C@ SAFE-EMIT
        _LOOP
    _ELSE
        DROP
    _THEN EMIT-FLUSH EXIT
    


_\ *************************** X10 Functions ****************************

_:X10_Out   _\ ( function Unit House --)
    docolon
    3       _\ length of message
    -1      _\ From ( will be filled by pipeout
    X10PIPE _\ To
    PipeOut DROP EXIT
_\
_\ X10 controller functions.
_:X10_On docolon     _\ ( Unit House ... ) ( 7)
     0x31 -ROT X10_Out EXIT
_:X10_Off docolon    _\ ( Unit House ... ) ( 7)
     0x30 -ROT X10_Out EXIT
_:X10_bright docolon _\ ( Unit House ... ) ( 7)
     0x42 -ROT X10_Out EXIT
_:X10_Dim docolon    _\ ( Unit House ... ) ( 7)
     0x44 -ROT X10_Out EXIT
\



_\ *************************** SCREEN ***********************************
_:SCR docolon CREATE
    HERE SCRADDR !
    0 C,        _\ x
    0 C,        _\ y
    0 ,         _\ pointer to screen[y][x]
    1024 ALLOT  _\ 16 lines of 64 characters
    EXIT
    
_:SCR.x     docolon SCRADDR @ 0 + EXIT
_:SCR.y     docolon SCRADDR @ 1 + EXIT
_:SCR.pText docolon SCRADDR @ 2 + EXIT _\ Pointer to screen[y][x]
_:SCR.scrn  docolon SCRADDR @ 4 + EXIT _\ The 16 lines of text
    
_:SCR.GotoXY  _\ ( x y -- )
    docolon
    SCR.y C!   SCR.x C! 
    SCR.y C@ 64 * SCR.x C@ + SCR.scrn + SCR.pText ! EXIT
    
_:SCR.CR      _\ ( -- )
    docolon
    0 SCR.y C@ 1+ 15 AND SCR.GotoXY EXIT

_:SCR.TextOut _\ ( c-addr u -- )
    docolon
    DUP >R    _\ Save count to update SCR.pText later
    SCR.pText @ SWAP CMOVE 
    R> DUP SCR.pText +! 
    SCR.x C@ + SCR.x C! EXIT

_:SCR.ClearScreen
    docolon
    SCR.scrn 1024 BLANK EXIT

_:SCR.DisplayScreen _\ Display 16 lines of text
    docolon    
    SCR.scrn
    _BEGIN
        DUP SCR.scrn 1024 + <>
    _WHILE
        DUP 64 TYPE CR
        64 +
    _REPEAT
    DROP EXIT
    
_\ *************************** END SCREEN *******************************

_\ _:MemSize MEMSIZE EXIT _IMMEDIATE


_:INCLUDE  _\ ( caddr u --)
    docolon
    2DUP OPEN-FILE   _\ ( caddr u -- error)
    _IF
        (S") _"Cannot open include file - " TYPE TYPE
    _ELSE
        CR (S") _"File Included - " TYPE TYPE CR
    _THEN
    EXIT

_:BootLoad docolon \ EXIT _IMMEDIATE 

_:Boot1Line docolon
    TIB TIBSIZE 0  READ-LINE DROP
    _IF
        TIB SWAP  0x2e EMIT  EMIT-FLUSH     _\ ( 2DUP TYPE CR ) 
        INTERPRET 1
    _ELSE
        DROP 0
    _THEN EXIT
    
_:BootStrap docolon         _\ Read and interpret the open file;
    INITIALIZE  LIT ABORT pABORT !
   
    _BEGIN
        Boot1Line
    _WHILE
    _REPEAT
    0 CLOSE-FILE DROP
    (S")  _"main" DROP 1-  FIND
    _IF
        EXECUTE
    _ELSE
        (S") _"Cannot Find 'main'"  TYPE
    _THEN
    ABORT
*/



" PRIM Me"
" PRIM He"
" PRIM A"
" PRIM B"
" PRIM A:Type"
" PRIM A:Damage"
" PRIM W:Type"
" PRIM W:Range"
" PRIM W:Protection"
" PRIM W:Damage"
" PRIM W:ROF"
" PRIM W:AttackBonus"
" PRIM W:Priority" 
" PRIM Shield.Next"
" PRIM Shield.Ready!"
" PRIM Fleeing@"
" PRIM C:State"
" PRIM C:Distance"
" PRIM C:Friendly"
" PRIM C:AIBaseclass"
" PRIM C:HasLineOfSight"
"   "
" 0  CONSTANT A:T:Unknnown"
" 1  CONSTANT A:T:SpellCaster"
" 2  CONSTANT A:T:Advance"
" 3  CONSTANT A:T:RangedWeapon"
" 4  CONSTANT A:T:MeleeWeapon"
" 5  CONSTANT A:T:Judo"
" 6  CONSTANT A:T:SpellLikeAbility"
" 0  CONSTANT W:T:NoWeapon"
" 1  CONSTANT W:T:HandBlunt"
" 2  CONSTANT W:T:HandCutting"
" 3  CONSTANT W:T:HandThrow"
" 4  CONSTANT W:T:SlingNoAmmo"
" 5  CONSTANT W:T:Bow"
" 6  CONSTANT W:T:Crossbow"
" 7  CONSTANT W:T:Throw"
" 8  CONSTANT W:T:Ammo"
" 9  CONSTANT W:T:SpellCaster"
" 10 CONSTANT W:T:SpellLikeAbility"
" 0  CONSTANT C:S:None" 
" 1  CONSTANT C:S:Casting" 
" 2  CONSTANT C:S:Attacking" 
" 3  CONSTANT C:S:Guarding"
" 4  CONSTANT C:S:Bandaging" 
" 5  CONSTANT C:S:Using"
" 6  CONSTANT C:S:Moving"
" 7  CONSTANT C:S:Turning"
" 8  CONSTANT C:S:Fleeing"
" 9  CONSTANT C:S:Fled"
" 10 CONSTANT C:S:ContinueGuarding" // ContinueGuarding means that a combatant who
                           // was guarding at the end of a round continues
                           // to guard until his initiative arrives.in
                           // the new round.
" 11 CONSTANT C:S:Petrified"
" 12 CONSTANT C:S:Dying"
" 13 CONSTANT C:S:Unconscious"
" 14 CONSTANT C:S:Dead"
" 15 CONSTANT C:S:Gone"
/*
" : ReadyBestShield   Me 0 Shield.Next Shield.Ready! ;"
" : TooFar? Me W:Range He C:Distance < ; 1 SP+-"
" : Petrified? He C:State C:S:Petrified = ; 1 SP+-"
" : Dead? He C:State C:S:Dead = ; 1 SP+-"
" : Gone? He C:State C:S:Gone = ; 1 SP+-"
" : SpellCasterFilter"  // ( ... removeaction)
"       He C:Friendly ?EXIT"
"       Dead? ?EXIT"
"       TooFar? ?EXIT"
"       Petrified?  ; 1 SP+-"
" : SpellLikeAbilityFilter"  // ( ... removeaction)
"       He C:Friendly ?EXIT"
"       Dead? ?EXIT"
"       TooFar? ?EXIT"
"       Petrified?  ; 1 SP+-"
" : AdvanceFilter"  // ( ... removeaction)
"       He C:Friendly ?EXIT"
"       Dead? ?EXIT"
"       Petrified?  ; 1 SP+-"
" : RangedWeaponFilter"  // ( ... removeaction)
"       He C:Friendly ?EXIT"
"       TooFar? ?EXIT"
"       Dead? ?EXIT"
"       Petrified? ; 1 SP+-"
" : MeleeWeaponFilter"  // ( ... removeaction)
"       He C:Friendly ?EXIT"
"       Dead? ?EXIT"
"       Gone? ?EXIT"
"       TooFar? ?EXIT"
"       Petrified? ; 1 SP+-"
" : JudoFilter" // ( ...removeaction)
"       He C:Friendly ?EXIT"
"       Dead? ( Check for dead target) ?EXIT"
"       Gone? ?EXIT"
"       TooFar? ?EXIT"
"       Petrified? ; 1 SP+-"
" : TestSpellCaster" // ( ... comparisonResult [-1 0 1] 
"      Me B W:Type A W:Type W:T:SpellCaster ="
"      IF"
"        W:T:SpellCaster ="
"        IF 0    ELSE 1  THEN"
"      ELSE"
"        W:T:SpellCaster ="
"        IF -1   ELSE 0 THEN"
"      THEN ;  1 SP+-"
" : TestSpellLikeAbility" // ( ... comparisonResult [-1 0 1] 
"      Me B W:Type A W:Type W:T:SpellLikeAbility ="
"      IF"
"        W:T:SpellLikeAbility ="
"        IF 0    ELSE 1  THEN"
"      ELSE"
"        W:T:SpellLikeAbility ="
"        IF -1   ELSE 0 THEN"
"      THEN ;  1 SP+-"
" : TestCasting"   // ( .. comparison result) A casting target comes first
"     He B C:State   A C:State   C:S:Casting ="
"     IF"
"        C:S:Casting ="
"        IF 0 ELSE 1 THEN"
"     ELSE"
"        C:S:Casting ="
"        IF -1 ELSE 0 THEN" 
"     THEN ; 1 SP+-"
" : TestMelee"    // Melee attacks take precedence
"    Me A A:Type B A:Type   A:T:MeleeWeapon = "
"    IF "
"       A:T:MeleeWeapon ="
"       IF 0 ELSE 1 THEN "
"    ELSE "
"       A:T:MeleeWeapon = "
"       IF -1 ELSE 0 THEN  "
"    THEN ; 1 SP+- "
" : TestAdvanceVersusAdvance"  // Advance towards the closer target
"     He A A:Type A:T:Advance != B A:Type A:T:Advance != OR IF 0 EXIT THEN"
"     B C:Distance A C:Distance - ; 1 SP+-"
" : THINK"  // Return int (<action A> minus <action B>)
//      Any attack using a SpellCaster item takes precedence over any attack
//      not using a SpellCaster item.
"      TestSpellCaster ?EXIT"
"      TestSpellLikeAbility ?EXIT"
//      Neither (or both) attacks is a SpellCaster item.  So we proceed.
//
//      Any action against a character who is casting takes precedence over
//      a non-casting character.
"      TestCasting     ?EXIT"
//      Neither target (or both) were casting.
//
"      TestAdvanceVersusAdvance ?EXIT" // If both actions are 'Advance'
//      No decision yet made.
//      The actions are equal in precedence.
"      0 ; 1 SP+-"
*/
;


//char *bank[256];

//#define cMEM(o) (bank[((ui16)(o)>>8)&0xff][(o)&0xff])


#define iMEM(o) (*(i16 *)(m+(ui16)(o)))
#define dFETCH(o) (((i32)(*(i16 *)(m+(ui16)(o)))<<16) | (*(i16 *)(m+(ui16)(o)+2)))
#define dSTORE(o,v) {*(i16 *)(m+(ui16)(o))=v>>16;*(i16 *)(m+(ui16)(o)+2)=v;};
//WORD& wMEM(int offset){return *(WORD *)(m+offset);};
//#define wMEM(o) (*(i16 *)(m+o))
//char& cMEM(ui16 o){return (bank[((ui16)(o)>>8)&0xff][(o)&0xff]);};
#define cMEM(o)(*(char *)(m+(ui16)(o)))
//i16&  iSTK(int offset){return iMEM(SP+offset);};
#define iSTK(o)(*(i16 *)(m+(ui16)(SP+(o))))
//i64&  dSTK(int offset){return dMEM(SP+offset);};
#define dSTK(o)(dMEM((ui16)(SP+(o))))


#define POPSP()(SP+=2, iMEM(SP-2))
#define POPDOUBLE() (SP+=4, ((((i32)iMEM(SP-4))<<16)|iMEM(SP-2)))
#define PUSHSP(n){SP-=2; iMEM(SP)=n;}
#define PUSHDOUBLE(n) (SP-=4,iMEM(SP+2)=n,iMEM(SP)=n>>16)
#define POPRP()(RP+=2, iMEM(RP-2))
#define PUSHRP(n){RP-=2; iMEM(RP)=n;}


void Kf_SPplusMinus(void){iMEM(iMEM(m_LATEST)-4)=-POPSP()*2;};
void Kf_oneMinus(void){iSTK(0)--;}
void Kf_LIT(void){PUSHSP(iMEM(PC));PC+=2;}
void Kf_at(void){iSTK(0) = iMEM(iSTK(0));};
void Kf_DUP(void){int n=iSTK(0);PUSHSP(n);}
void Kf_QDUP(void){ui16 n=iSTK(0);if (n!=0)PUSHSP(n);};
void Kf_SWAP(void){int n=iSTK(0);iSTK(0)=iSTK(2);iSTK(2)=n;};
void Kf_0less(void){if(iSTK(0)<0)iSTK(0)=-1;else iSTK(0)=0;};
void Kf_OVER(void){int n=iSTK(2);PUSHSP(n);}
void Kf_ROT(void) {int n=iSTK(0);iSTK(0)=iSTK(4);iSTK(4)=iSTK(2);iSTK(2)=n;};
void Kf_Cat(void){iSTK(0) = cMEM(iSTK(0));};
void Kf_Cstore(void) {int addr=POPSP(); cMEM(addr)=POPSP();};
void Kf_store(void){iMEM(iSTK(0)) = iSTK(2); SP+=4;};
void Kf_plusStore(void){int addr=POPSP();int v=POPSP();iMEM(addr)+=v;}
void Kf_LITERAL(void){PUSHSP(iMEM(PC));PC+=2;}
void Kf_2R(void){PUSHRP(POPSP());};
void Kf_R2(void){PUSHSP(POPRP());};
void Kf_OR(void){int t=POPSP();iSTK(0)|=t;};
void Kf_XOR(void){int t=POPSP();iSTK(0)^=t;};
void Kf_AND(void){int t=POPSP();iSTK(0)&=t;};
void Kf_ABS(void){if (iSTK(0)<0)iSTK(0)=-iSTK(0);};
void Kf_minus(void){int t=POPSP();iSTK(0)-=t;};
void Kf_plus(void){int t=POPSP();iSTK(0)+=t;};
void Kf_UMgear(void){ui32 t=(ui16)POPSP()*(ui16)POPSP();PUSHDOUBLE(t);};
void Kf_Mgear(void){ui32 t=POPSP()*POPSP();PUSHDOUBLE(t);};
void Kf_less(void){i16 a=POPSP();i16 b=POPSP();if (b<a)b=-1;else b=0;PUSHSP(b);};
void Kf_greater(void){i16 a=POPSP();i16 b=POPSP();if (b>a)b=-1;else b=0;PUSHSP(b);};
void Kf_equal(void){i16 a=POPSP();i16 b=POPSP();if (b==a)b=-1;else b=0;PUSHSP(b);};
void Kf_NOT(void){if (iMEM(SP)) iMEM(SP)=0;else iMEM(SP)=-1;};
void Kf_notEqual(void){i16 a=POPSP();i16 b=POPSP();if (b!=a)b=-1;else b=0;PUSHSP(b);};
void Kf_MUslashMOD(void){ui32 b=POPDOUBLE();ui32 t=POPDOUBLE();
                         ui32 q=t/b;ui32 r=t%b;PUSHDOUBLE(q);PUSHDOUBLE(r);};
void Kf_DEBUG(void)
{
  int cur, len, addr, offset, sp;
  CString funcName;
  for (cur=iMEM(m_LATEST); cur>0; cur=iMEM(cur))
  {
    if (cur < PC) break;
  };
  len = cMEM(cur-5);
  addr = cur - len - 5;
  funcName = CString(m+addr, len);
  offset = PC - cur - 2;
  WriteDebugString("DEBUG DUMP of AI_Script function \"%s\" at offset %d.  Top of stack first \n", funcName, offset);
  for (sp=SP; sp<SP0; sp+=2)
  {
    WriteDebugString(" [SP+%02d] = 0x%04x\n", sp-SP, iMEM(sp));
  };
}
void Kf_DROP(void){SP+=2;};
void Kf_FIND(void)   // addr ... addr 0 | cfa 1 | cfa -1
{
  int cur, addr, len;
  addr = POPSP();
  len = cMEM(addr);
  //Ke_LATEST Kf_at();
  for (cur=iMEM(m_LATEST); cur>0; cur=iMEM(cur))
  {
    if (len != cMEM(cur-5)) continue;
    if (memcmp(m+addr+1, m+cur-5-len, len) != 0) continue;
    PUSHSP(cur); if (cMEM(cur-2)&1) {PUSHSP(1)} else PUSHSP(-1) return; 
  };
  PUSHSP(addr); PUSHSP(0);
}
void Kf_HEAD(void) // addr ..  // Creates NOP header
{
  //We must position the word so that its link will end 
  // at a word boundary.
  int origAddr, len, offset, here, prev, i;
  origAddr = POPSP();
  here=iMEM(m_DP);
  len = cMEM(origAddr);
  offset = (22222-(len+1))%2;
  memmove(m+here+offset, m+origAddr+1, len);
  for (i=0; i<offset; i++) m[here+i] = 0;
  m[here+offset+len] = len;
  iMEM(here+offset+len+1) = 0; // Assume no change to data stack
  m[here+offset+len+3] = 0; // flags
  m[here+offset+len+4] = 0; // ":$"
  prev = iMEM(m_LATEST); iMEM(here+offset+len+5) = prev; // link
  iMEM(m_LATEST) = here+offset+len+5;
  iMEM(m_DP) += offset+len+7;
};
void Kf_plusP(void){cMEM(iMEM(m_LATEST)-1)=nextPrim++;}
void Kf_WORD(void)      // c .. addr  
                        // reads >IN and puts counted string at HERE
                        // Leaves address of HERE on stack
                        // Note: If the delimiter c is a blank then we
                        //       match it with either a blank or a <tab>
{
  char c = POPSP();
  char c1 = c;
  if (c == 0)
  {
    ASSERT(TRUE);
  };
  if (c == ' ')
  { 
    c1 = '\t';
  };
  int srcAddr;
  int n = iMEM(m_tickSOURCE+2) - iMEM(m_2IN);
  srcAddr = iMEM(m_tickSOURCE) + iMEM(m_2IN);
  cMEM(iMEM(m_DP)) = 0;
  while (n > 0)
  {
    if ((cMEM(srcAddr) != c) && (cMEM(srcAddr) != c1)) break;
    n--;
    srcAddr++;
  };
  while (n > 0)
  {
    if ((cMEM(srcAddr) == c) || (cMEM(srcAddr) == c1)) break;
    cMEM(iMEM(m_DP))++;
    cMEM(iMEM(m_DP)+cMEM(iMEM(m_DP))) = cMEM(srcAddr);
    n--;
    srcAddr++;
  };
  cMEM(iMEM(m_DP)+cMEM(iMEM(m_DP))+1) = ' ';
  if (cMEM(iMEM(m_DP)) != 0)  
  {
    srcAddr++;
  }
  iMEM(m_2IN) = srcAddr - iMEM(m_tickSOURCE);
  PUSHSP(iMEM(m_DP));
}
void Kf_CREATE(void)
{
  PUSHSP(' '); Kf_WORD(); Kf_HEAD();
}
void Kf_COUNT(void)
{
  int addr=iSTK(0);
  iSTK(0)++;
  PUSHSP(cMEM(addr));
}
void Kf_tick(void)
{
  PUSHSP(' ');Kf_WORD();Kf_FIND();
  if (POPSP()==0)
  {
    Kf_COUNT(); Kf_TYPE();
    PUSHSP(0x3f); Kf_EMIT(); Kf_CR(); 
    Kf_ABORT();
  };
}

void Kf_NEGATE(void){iSTK(0)=-iSTK(0);};
void Kf_DNEGATE(void){i32 d=POPDOUBLE();PUSHDOUBLE(-d);}
void Kf_BRANCH(void){PC=iMEM(PC);};
void Kf_QBRANCH(void){if(POPSP()!=0)PC+=2;else PC=iMEM(PC);};
void Kf_CONVERT(void) // d1 addr1 -- d2 addr2
{
  int addr;
  char c;
  i32 result;
  addr=POPSP()+1;
  result = POPDOUBLE();
  for(;;addr++)
  {
    c = m[addr];
    if (c >= 'a') c = c - 'a' + 'A';
    if (c >= 'A') c = c - 'A' + 10 + '0';
    c -= '0';
    if ((c < 0) || (c >= iMEM(m_BASE))) break;
    result = result * iMEM(m_BASE) + c;
  };
  PUSHDOUBLE(result);
  PUSHSP(addr);
};
void Kf_QNUMBER(void)  //  counted-addr ... d -1  |  c-addr 0
{
  int addr, len, col=0, origBase, newBase, sign=1, remainder;
  addr = POPSP(); len=m[addr];
  if (len==0){PUSHSP(addr);PUSHSP(0);return;};
  origBase = iMEM(m_BASE);
  if ((len>3) && ((m[addr+2]=='\'')&&(m[addr+len]=='\'')))
  {
         if (m[addr+1] == 'H') newBase = 16;
    else if (m[addr+1] == 'D') newBase = 10;
    else if (m[addr+1] == 'O') newBase = 8;
    else {PUSHSP(addr); PUSHSP(0); return;}
    iMEM(m_BASE) = newBase;
    col+=2;    // skip leading H'
    len-=3; // remove trailing '
  }
  col++;  // Skip count byte
  if (len > 0)
  {
       if (m[addr+col] == '+'){col++; len--;}
       else if (m[addr+col] == '-'){col++;len--;sign=-1;};
  };
  PUSHSP(0);PUSHSP(0);PUSHSP(addr+col-1);Kf_CONVERT();
  iMEM(m_BASE) = origBase;
  remainder = col+len+addr - POPSP();  // 
  if (remainder != 0)
  {
    iSTK(2)=addr;iSTK(0)=0;return;
  };

//      _IF
//          R> 2DROP 2DROP 0    _\ ca 0         error
//      _ELSE
//          2DROP NIP R>
//          _IF NEGATE _THEN -1  _\ n -1         OK
//      _THEN 
  if (sign < 0) Kf_DNEGATE();
  PUSHSP(-1);
}
void Kf_NOP(void){};
void Kf_EMIT(void)
{
  char c = POPSP();
#ifdef UAFEngine
  WriteDebugString("%c", c);
#else
  printf("%c", c);
#endif
}
void Kf_TYPE(void)
{
  int n=POPSP();
  int addr=POPSP();
#ifdef UAFEngine
  {
    CString msg;
    msg = CString(m+addr,n);
    WriteDebugString("%s", msg);
  }
#else
  {
  int i;
  for(i=0; i<n; i++)
  {
    printf("%c", m[addr+i]);
  };
#endif
}
void Kf_CR(void)
{
  printf("\n");
}
void Kf_EXIT(void){Exit=1;}
void Kf_QuestionEXIT(void){if (iMEM(SP)) Exit=1; else POPSP();};
void Kf_ABORT(void)
{
  Exit=999999;
}
void Kf_docon(void){PUSHSP(iMEM(cfa+2));};
void Kf_SETIMMEDIATE(void){int addr=POPSP();m[addr-2]|=1;}
void Kf_openBracket(void){iMEM(m_STATE)=0;}
void Kf_closeBracket(void){iMEM(m_STATE)=1;}
void DataStackError(ui16 p, int n)
{
  CString name,msg;
  char *txt;
  ui16 addr, len;
  addr = iMEM(m_LATEST);
  while (addr > p) addr = iMEM(addr);
  len = cMEM(addr-5);
  txt = m+addr-5-len;
  name = CString(txt, len);
  if (n > 0)
  {
    msg.Format("The Forth word '%s' added %d too many entries to the data stack", name, n);
  }
  else
  {
    msg.Format("The Forth word '%s' removed %d too many entries from the data stack", name, -n);
  };
  die(msg);
}

void Kf_docolon(void)
{
  PUSHRP(SP+iMEM(cfa-4))
  PUSHRP(PC);PC=cfa+2;
  for (;;)
  {
    int op;
    cfa = iMEM(PC); PC+=2;
    op = cMEM(cfa-1);
    Kf[op]();
    if (Exit) break;
  };
  Exit--;
  if (SP != iMEM(RP+2))
  {
    DataStackError(PC, (iMEM(RP+2)-SP)/2);
  };
  PC = POPRP();
  RP+=2;
}

void Kf_EXECUTE(void)
{
  int op;
  cfa = POPSP();
  op = cMEM(cfa-1);
  Kf[op]();
}


void Kf_INTERPRET(void) // i*x c-addr u -- j*x      interpret given buffer
     //   This is a common factor of EVALUATE and QUIT
     //   ref. dpANS-6 3.4 The Forth Text Interpreter
{
  iMEM(m_tickSOURCE+2) = POPSP();
  iMEM(m_tickSOURCE  ) = POPSP(); // Save source and count
  iMEM(m_2IN) = 0;               // Current input column
  for (;;)
  {
    int wordAddr;
    PUSHSP(' '); Kf_WORD(); 
    wordAddr = iSTK(0);
    if (cMEM(wordAddr) == 0) break;
    Kf_FIND();
    if (iSTK(0) != 0)
    {
      if ((POPSP() > 0) || (iMEM(m_STATE) == 0)) //Immediate or intepret
      {
        cfa = POPSP();
        int op=cMEM(cfa-1);
        Kf[op]();
        if (Exit) return;
      //  _IF
      //       1+ STATE @ 0= OR        _\ immed or interp
      //      _IF  EXECUTE 
      //      }
      //  _ELSE
      }
      else
      {
      //      _IF
      //          LITERAL
      //      _ELSE 
      //          COUNT TYPE 0x3F EMIT CR
      //          pABORT @ EXECUTE
      //      _THEN
       //      _ELSE ,XT  _THEN 
            // Push a word address
        iMEM(iMEM(m_DP)) = POPSP();
        iMEM(m_DP) += 2;
      };
    }
    else
    {
      POPSP();
      Kf_QNUMBER();
      if (POPSP() == 0)
      {
        Kf_COUNT(); 
#ifdef UAFEngine
        {
          CString msg;
          int addr, n;
          n = POPSP(); addr = POPSP();
          msg = CString(m+addr, n);
          msg += "?\n";
          WriteDebugString(msg);
        }
#else
        Kf_TYPE();
        PUSHSP(0x3f); Kf_EMIT(); Kf_CR(); 
#endif
        Kf_ABORT(); return;
      }
      else
      {
        SP+=2; // For now....only single precision
        if (iMEM(m_STATE) != 0)
        {
          int dp;
          char lit[] = {3,'L','I','T'};
          PUSHSP(*(i16 *)(lit+2));
          PUSHSP(*(i16 *)(lit+0));
          PUSHSP(SP+2);
          Kf_FIND(); POPSP();
          dp=iMEM(m_DP);
          iMEM(dp)=POPSP();
          POPSP();
          POPSP();
          iMEM(dp+2) = POPSP();
          iMEM(m_DP)+=4;
        };
      };
    };
  };
  POPSP();
}
/*
void Kf_NUMBER(void) // n addr .. n addr
{ // Definition is from 'Starting Forth'
  int i, n;
  int addr = POPSP();
  int result = addr;
  int flag=0;
  int sign = 1;
  char c;
  if (m[addr] != 0)
  {
    result = 0;
    flag = -1;
    n = m[addr++];
    for (i=0; i<n; i++)
    {
      c = m[addr++];
      if (c=='-') {sign = -sign; continue;};
      if (c=='+') continue;
      if ((c < '0') || (c >= iMEM(m_BASE))) {result=addr; flag=0; break;};
    };
    result = sign*result;
  };
  PUSHSP(flag);
  PUSHSP(result);
};
*/


void Kf_PrintSource(void)
{
  int toin, n;
  toin = iMEM(m_2IN) - 30;
  if (toin < 0) toin = 0;
  n = iMEM(m_tickSOURCE+2) - toin;
  if (n > 80) n = 80;
  PUSHSP(iMEM(m_tickSOURCE) + toin);
  PUSHSP(n);
#ifdef UAFEngine
  {
    int addr;
    CString msg;
    n = POPSP();
    addr = POPSP();
    WriteDebugString("*** Error in Forth Script ****\n");
    msg = CString(m+addr, n);
    WriteDebugString(msg+"\n");
  }
#else
  Kf_TYPE();
  Kf_CR();
#endif
}

void COMBAT_SUMMARY_COMBATANT::operator = (const COMBAT_SUMMARY_COMBATANT& csc)
{
  if (csc.GetShieldCount() > 0) die("Internal Error.  Shield count non-zero");
  if (csc.GetWeaponCount() > 0) die("Internal Error.  Weapon count non-zero");
  if (csc.GetAttackCount() > 0) die("Internal Error.  Attack count non-zero");
  shieldSummary.SetSize(0);
  index = csc.index;
  shieldToReady = csc.shieldToReady;
  fleeing = csc.fleeing;
  state = csc.state;
  friendly = csc.friendly;
  state = csc.state;
  availAttacks = csc.availAttacks;
  x = csc.x;
  y = csc.y;
  width = csc.width;
  height = csc.height;
}


#define pCC (pCombatSummary->GetCombatant(pCombatSummary->m_curCombatant))

void Kf_ShieldDotNext(void)
{
  ui16 i,n;
  n = pCSC->shieldSummary.GetSize();
  i = iSTK(0);
  if (i >= n) i=0;else i=i+1;
  iSTK(0)=i;
}
void Kf_ShieldDotReadyBang(void)
{
  ui16 i;
  i = POPSP();
  pCS->GetCombatant(0)->shieldToReady = i;
}

void Kf_FleeingAt(void){PUSHSP(pCSC->fleeing);};
void Kf_CcolonState(void){PUSHSP(pCSC->state);};
void Kf_CcolonDistance(void){PUSHSP(pCSA->distance22);};
void Kf_CcolonFriendly(void){PUSHSP(pCSC->friendly);};

void Kf_CcolonAIBaseclass(void)
{
  PUSHSP(pCSC->AIBaseclass);
};
void Kf_CcolonHasLineOfSight(void) { PUSHSP(pCSA->hasLineOfSight); };

void Kf_Me(void)
{
  pActionA->pCSC = pActionA->pMe;
  pActionB->pCSC = pActionB->pMe;
  pCSC = pCSA->pCSC;
}
void Kf_A(void){pCSA=pCS->pActionA; pCSC=pCSA->pCSC;};
void Kf_B(void){pCSA=pCS->pActionB; pCSC=pCSA->pCSC;};
void Kf_AcolonType(void){PUSHSP(pCSA->actionType);};
void Kf_AcolonDamage(void){PUSHSP(pCSA->damage);};
void Kf_WcolonType(void)
{
  if ((pCSA->weaponOrd == 0) || (pCSA->weaponOrd > pCSC->GetWeaponCount()))
  {
    PUSHSP(0);  // NotWeapon
  }
  else
  {
    PUSHSP(pCSC->PeekWeapon(pCSA->weaponOrd-1)->Wpn_Type);
  };
}
void Kf_WcolonRange(void)
{
  if ((pCSA->weaponOrd == 0) || (pCSA->weaponOrd > pCSC->GetWeaponCount()))
  {
    PUSHSP(0);  // NotWeapon
  }
  else
  {
    PUSHSP(pCSC->PeekWeapon(pCSA->weaponOrd-1)->range22);
  };
}
void Kf_WcolonPriority(void)
{
  if ((pCSA->weaponOrd == 0) || (pCSA->weaponOrd > pCSC->GetWeaponCount()))
  {
    PUSHSP(0);  // NotWeapon
  }
  else
  {
    PUSHSP(pCSC->PeekWeapon(pCSA->weaponOrd-1)->priority);
  };
}
void Kf_WcolonAttackBonus(void)
{
  if ((pCSA->weaponOrd == 0) || (pCSA->weaponOrd > pCSC->GetWeaponCount()))
  {
    PUSHSP(0);  // NotWeapon
  }
  else
  {
    PUSHSP(pCSC->PeekWeapon(pCSA->weaponOrd-1)->attackBonus);
  };
}
void Kf_WcolonROF(void)
{
  if ((pCSA->weaponOrd == 0) || (pCSA->weaponOrd > pCSC->GetWeaponCount()))
  {
    PUSHSP(0);  // NotWeapon
  }
  else
  {
    PUSHSP(pCSC->PeekWeapon(pCSA->weaponOrd-1)->ROF_Per_Round);
  };
}
void Kf_WcolonDamage(void)
{
  if ((pCSA->weaponOrd == 0) || (pCSA->weaponOrd > pCSC->GetWeaponCount()))
  {
    PUSHSP(0);  // NotWeapon
  }
  else
  {
    int damage;
    const WEAPON_SUMMARY *pWeapon;
    pWeapon = pCSC->PeekWeapon(pCSA->weaponOrd-1);
    damage =  pCSA->pHe->isLarge
             ?(pWeapon->largeDamageDice+pWeapon->largeDamageBonus)
             :(pWeapon->smallDamageDice+pWeapon->smallDamageBonus);
    PUSHSP(damage);
  };
}
void Kf_WcolonProtection(void)
{
  if ((pCSA->weaponOrd == 0) || (pCSA->weaponOrd > pCSC->GetWeaponCount()))
  {
    PUSHSP(0);  // NotWeapon
  }
  else
  {
    PUSHSP(pCSC->PeekWeapon(pCSA->weaponOrd-1)->protection);
  };
}

void Kf_He(void)
{
  pActionA->pCSC = pActionA->pHe;
  pActionB->pCSC = pActionB->pHe;
  pCSC = pCSA->pCSC;
};


bool ExpandKernel(void)
{ 
  static bool finished=false;
  static bool success=true;
  int len;
  if (finished) return success;
  finished = true;
  len = strlen(m);
  SP = SP0;
  RP = RP0;
  // Move the definitions to the end of mem
  memmove(m + SP - len - 100, m, len+1);
  memset(m, 0, SP - len - 100);
  iMEM(m_DP) = DP0;
  iMEM(m_tickSOURCE) = SP - len - 100;
  iMEM(m_tickSOURCE+2) = len;
  iMEM(m_LATEST) = 0;
  iMEM(m_2IN) = 0;
  nextPrim = 1;
  Kf_CREATE(); Kf_plusP();
  Kf_CREATE(); Kf_plusP();
  PUSHSP(iMEM(m_tickSOURCE) += iMEM(m_2IN));
  PUSHSP(iMEM(m_tickSOURCE+2) -= iMEM(m_2IN));
  iMEM(m_STATE) = 0;
  iMEM(m_BASE) = 10;
  Exit=0;
  Kf_INTERPRET();
  if (Exit)
  {
    MsgBoxInfo("AI_Script Error.  See log for details", "Warning");
    Kf_PrintSource();
    return false;
  };
  {
    FILE *f;
    CString fname;
    fname.Format("%sAI_Script.BLK", rte.DataDir());
    f = fopen(fname, "r");
    if (f != NULL)
    {
      bool printVersion = true;
      {
        //MsgBoxInfo("An AI Script exists!","Warning");
        //debugSeverity = 7;
        //WriteDebugString("An AI Script exists in the data folder\n");
      };
      while (fgets(m + SP - 200, 120, f) != NULL)
      {
        char *str = m+SP-200;
        while ((str=strchr(str, 10))!= NULL)
        {
          *str = ' ';
        };
        if (printVersion)
        {
          printVersion = false;
          WriteDebugString("AI_Script first line = %s\n", m+SP-200);
        };
        iMEM(m_tickSOURCE) = SP - 200;
        iMEM(m_tickSOURCE+2) = (i16)strlen(m+SP-200);
        PUSHSP(iMEM(m_tickSOURCE));
        PUSHSP(iMEM(m_tickSOURCE+2));
        Kf_INTERPRET();
        if (Exit)
        {
          MsgBoxInfo("AI_Script Error.  See log for details", "Warning");
          Kf_PrintSource();
          success = false;
          break;;
        };
      };
      fclose(f);
    }
    else
    {
      die("AI_Script.BLK file does not exist in data folder");
    };
  };
  return success;
};

void FetchKernel(unsigned char *program)
{
  int len;
  len = iMEM(m_DP);
  memcpy(program, m, len);
}

int cfaFIND(char *name)
{
  int cur, len;
  len = strlen(name);
  for (cur=iMEM(m_LATEST); cur>0; cur=iMEM(cur))
  {
    if (len != cMEM(cur-5)) continue;
    if (memcmp(name, m+cur-5-len, len) == 0) return cur;
  };
  return -1;
}

int RunSpellCasterFilter(COMBAT_SUMMARY *pCombatSummary, COMBAT_SUMMARY_ACTION *pcsa)
{
  static int cfaFILTER = 0;
  pCS = pCombatSummary;
  pActionA = pcsa;
  pActionB = pcsa;
  pCSA = pActionA;
  pActionA->pCSC = pActionA->pMe;
  pActionB->pCSC = pActionB->pMe;
  pCSC = pCSA->pCSC;
  if (cfaFILTER == 0)
  {
    cfaFILTER = cfaFIND("SpellCasterFilter");
  };
  if (cfaFILTER > 0)
  {
    cfa = cfaFILTER;
    SP = SP0;
    RP = RP0;
    Kf_docolon();
    return POPSP();
  };
  return 0;
}

int RunSpellLikeAbilityFilter(COMBAT_SUMMARY *pCombatSummary, COMBAT_SUMMARY_ACTION *pcsa)
{
  static int cfaFILTER = 0;
  pCS = pCombatSummary;
  pActionA = pcsa;
  pActionB = pcsa;
  pCSA = pActionA;
  pActionA->pCSC = pActionA->pMe;
  pActionB->pCSC = pActionB->pMe;
  pCSC = pCSA->pCSC;
  if (cfaFILTER == 0)
  {
    cfaFILTER = cfaFIND("SpellLikeAbilityFilter");
  };
  if (cfaFILTER > 0)
  {
    cfa = cfaFILTER;
    SP = SP0;
    RP = RP0;
    Kf_docolon();
    return POPSP();
  };
  return 0;
}

int RunAdvanceFilter(COMBAT_SUMMARY *pCombatSummary, COMBAT_SUMMARY_ACTION *pcsa)
{
  static int cfaFILTER = 0;
  pCS = pCombatSummary;
  pActionA = pcsa;
  pActionB = pcsa;
  pCSA = pActionA;
  pActionA->pCSC = pActionA->pMe;
  pActionB->pCSC = pActionB->pMe;
  pCSC = pCSA->pCSC;
  if (cfaFILTER == 0)
  {
    cfaFILTER = cfaFIND("AdvanceFilter");
  };
  if (cfaFILTER > 0)
  {
    cfa = cfaFILTER;
    SP = SP0;
    RP = RP0;
    Kf_docolon();
    return POPSP();
  };
  return 0;
}

int RunJudoFilter(COMBAT_SUMMARY *pCombatSummary, COMBAT_SUMMARY_ACTION *pcsa)
{
  static int cfaFILTER = 0;
  pCS = pCombatSummary;
  pActionA = pcsa;
  pActionB = pcsa;
  pCSA = pActionA;
  pActionA->pCSC = pActionA->pMe;
  pActionB->pCSC = pActionB->pMe;
  pCSC = pCSA->pCSC;
  if (cfaFILTER == 0)
  {
    cfaFILTER = cfaFIND("JudoFilter");
  };
  if (cfaFILTER > 0)
  {
    cfa = cfaFILTER;
    SP = SP0;
    RP = RP0;
    Kf_docolon();
    return POPSP();
  };
  return 0;
}

int RunMeleeWeaponFilter(COMBAT_SUMMARY *pCombatSummary, COMBAT_SUMMARY_ACTION *pcsa)
{
  static int cfaFILTER = 0;
  pCS = pCombatSummary;
  pActionA = pcsa;
  pActionB = pcsa;
  pCSA = pActionA;
  pActionA->pCSC = pActionA->pMe;
  pActionB->pCSC = pActionB->pMe;
  pCSC = pCSA->pCSC;
  if (cfaFILTER == 0)
  {
    cfaFILTER = cfaFIND("MeleeWeaponFilter");
  };
  if (cfaFILTER > 0)
  {
    cfa = cfaFILTER;
    SP = SP0;
    RP = RP0;
    Kf_docolon();
    return POPSP();
  };
  return 0;
}

int RunRangedWeaponFilter(COMBAT_SUMMARY *pCombatSummary, COMBAT_SUMMARY_ACTION *pcsa)
{
  static int cfaFILTER = 0;
  pCS = pCombatSummary;
  pActionA = pcsa;
  pActionB = pcsa;
  pCSA = pActionA;
  pActionA->pCSC = pActionA->pMe;
  pActionB->pCSC = pActionB->pMe;
  pCSC = pCSA->pCSC;
  if (cfaFILTER == 0)
  {
    cfaFILTER = cfaFIND("RangedWeaponFilter");
  };
  if (cfaFILTER > 0)
  {
    cfa = cfaFILTER;
    SP = SP0;
    RP = RP0;
    Kf_docolon();
    return POPSP();
  };
  return 0;
}

int RunTHINK(unsigned char *program, COMBAT_SUMMARY *pCombatSummary)
{
  static int cfaTHINK = 0;
  pCS = pCombatSummary;
  pActionA = pCS->pActionA;
  pActionB = pCS->pActionB;
  pCSA = pActionA;
  pActionA->pCSC = pActionA->pMe;
  pActionB->pCSC = pActionB->pMe;
  pCSC = pCSA->pCSC;
  if (cfaTHINK == 0)
  {
    cfaTHINK = cfaFIND("THINK");
  };
  if (cfaTHINK > 0)
  {
    cfa = cfaTHINK;
    SP = SP0;
    RP = RP0;
    Kf_docolon();
    return POPSP();
  };
  return 0;
}

