#ifndef __REGEXP_H__
#define __REGEXP_H__

// I copied this pretty much verbatim from an existing program.  The words
// at the top of that program are duplicated below:  Paul Stevens

/* Extended regular expression matching and search library,
   version 0.12.
   (Implements POSIX draft P1003.2/D11.2, except for some of the
   internationalization features.)
   Copyright (C) 1993, 94, 95, 96, 97, 98 Free Software Foundation, Inc.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with the GNU C Library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */




#define RE_TRANSLATE_TYPE char *


// The following bits are used to determine the regexp syntax we
// recognize.  The set/not-set meanings are chosen so that Emacs syntax
// remains the value 0.  The bits are given in alphabetical order, and
// the definitions shifted by one from the previous bit; thus, when we
// add or remove a bit, only one other definition need change.
typedef unsigned long int reg_syntax_t;

// If this bit is not set, then \ inside a bracket expression is literal.
// If set, then such a \ quotes the following character. 
#define RE_BACKSLASH_ESCAPE_IN_LISTS ((unsigned long int) 1)

// If this bit is not set, then + and ? are operators, and \+ and \? are
// literals.
// If set, then \+ and \? are operators and + and ? are literals.
#define RE_BK_PLUS_QM (RE_BACKSLASH_ESCAPE_IN_LISTS << 1)

// If this bit is set, then character classes are supported.  They are:
//   [:alpha:], [:upper:], [:lower:],  [:digit:], [:alnum:], [:xdigit:],
//   [:space:], [:print:], [:punct:], [:graph:], and [:cntrl:].
// If not set, then character classes are not supported.
#define RE_CHAR_CLASSES (RE_BK_PLUS_QM << 1)

// If this bit is set, then ^ and $ are always anchors (outside bracket
//   expressions, of course).
// If this bit is not set, then it depends:
//      ^  is an anchor if it is at the beginning of a regular
//         expression or after an open-group or an alternation operator;
//      $  is an anchor if it is at the end of a regular expression, or
//         before a close-group or an alternation operator.
//
// This bit could be (re)combined with RE_CONTEXT_INDEP_OPS, because
// POSIX draft 11.2 says that * etc. in leading positions is undefined.
// We already implemented a previous draft which made those constructs
// invalid, though, so we haven't changed the code back.
#define RE_CONTEXT_INDEP_ANCHORS (RE_CHAR_CLASSES << 1)

// If this bit is set, then special characters are always special
//   regardless of where they are in the pattern.
// If this bit is not set, then special characters are special only in
//   some contexts; otherwise they are ordinary.  Specifically,
//   * + ? and intervals are only special when not after the beginning,
//   open-group, or alternation operator. 
#define RE_CONTEXT_INDEP_OPS (RE_CONTEXT_INDEP_ANCHORS << 1)

// If this bit is set, then *, +, ?, and { cannot be first in an re or
//   immediately after an alternation or begin-group operator.
#define RE_CONTEXT_INVALID_OPS (RE_CONTEXT_INDEP_OPS << 1)

// If this bit is set, then . matches newline.
// If not set, then it doesn't. 
#define RE_DOT_NEWLINE (RE_CONTEXT_INVALID_OPS << 1)

// If this bit is set, then . doesn't match NUL.
// If not set, then it does. 
#define RE_DOT_NOT_NULL (RE_DOT_NEWLINE << 1)

// If this bit is set, nonmatching lists [^...] do not match newline.
// If not set, they do. 
#define RE_HAT_LISTS_NOT_NEWLINE (RE_DOT_NOT_NULL << 1)

// If this bit is set, either \{...\} or {...} defines an
//   interval, depending on RE_NO_BK_BRACES.
// If not set, \{, \}, {, and } are literals.
#define RE_INTERVALS (RE_HAT_LISTS_NOT_NEWLINE << 1)

// If this bit is set, +, ? and | aren't recognized as operators.
// If not set, they are. 
#define RE_LIMITED_OPS (RE_INTERVALS << 1)

// If this bit is set, newline is an alternation operator.
// If not set, newline is literal. 
#define RE_NEWLINE_ALT (RE_LIMITED_OPS << 1)

// If this bit is set, then `{...}' defines an interval, and \{ and \}
//   are literals.
//If not set, then `\{...\}' defines an interval.
#define RE_NO_BK_BRACES (RE_NEWLINE_ALT << 1)

// If this bit is set, (...) defines a group, and \( and \) are literals.
// If not set, \(...\) defines a group, and ( and ) are literals. 
#define RE_NO_BK_PARENS (RE_NO_BK_BRACES << 1)

// If this bit is set, then \<digit> matches <digit>.
// If not set, then \<digit> is a back-reference.
#define RE_NO_BK_REFS (RE_NO_BK_PARENS << 1)

// If this bit is set, then | is an alternation operator, and \| is literal.
// If not set, then \| is an alternation operator, and | is literal.
#define RE_NO_BK_VBAR (RE_NO_BK_REFS << 1)

// If this bit is set, then an ending range point collating higher
//   than the starting range point, as in [z-a], is invalid.
// If not set, then when ending range point collates higher than the
//   starting range point, the range is ignored. 
#define RE_NO_EMPTY_RANGES (RE_NO_BK_VBAR << 1)

// If this bit is set, then an unmatched ) is ordinary.
// If not set, then an unmatched ) is invalid. 
#define RE_UNMATCHED_RIGHT_PAREN_ORD (RE_NO_EMPTY_RANGES << 1)

// If this bit is set, succeed as soon as we match the whole pattern,
// without further backtracking.
#define RE_NO_POSIX_BACKTRACKING (RE_UNMATCHED_RIGHT_PAREN_ORD << 1)

// If this bit is set, do not process the GNU regex operators.
// If not set, then the GNU regex operators are recognized.
#define RE_NO_GNU_OPS (RE_NO_POSIX_BACKTRACKING << 1)

// If this bit is set, turn on internal regex debugging.
// If not set, and debugging was on, turn it off.
// This only works if regex.c is compiled -DDEBUG.
// We define this bit always, so that all that's needed to turn on
// debugging is to recompile regex.c; the calling code can always have
// this bit set, and it won't affect anything in the normal case.
#define RE_DEBUG (RE_NO_GNU_OPS << 1)

// This global variable defines the particular regexp syntax to use (for
// some interfaces).  When a regexp is compiled, the syntax used is
// stored in the pattern buffer, so changing this does not affect
// already-compiled regexps.  Located in regexp.cpp.
extern reg_syntax_t re_syntax_options; 

// Define combinations of the above bits for the standard possibilities.
// (The [[[ comments delimit what gets put into the Texinfo file, so
// don't delete them!) 
// [[[begin syntaxes]]] 
#define RE_SYNTAX_EMACS 0

#define RE_SYNTAX_AWK									\
  (RE_BACKSLASH_ESCAPE_IN_LISTS   | RE_DOT_NOT_NULL		\
   | RE_NO_BK_PARENS              | RE_NO_BK_REFS		\
   | RE_NO_BK_VBAR                | RE_NO_EMPTY_RANGES	\
   | RE_DOT_NEWLINE		  | RE_CONTEXT_INDEP_ANCHORS	\
   | RE_UNMATCHED_RIGHT_PAREN_ORD | RE_NO_GNU_OPS)

#define RE_SYNTAX_GNU_AWK												\
  ((RE_SYNTAX_POSIX_EXTENDED | RE_BACKSLASH_ESCAPE_IN_LISTS | RE_DEBUG)	\
   & ~(RE_DOT_NOT_NULL | RE_INTERVALS | RE_CONTEXT_INDEP_OPS))

#define RE_SYNTAX_POSIX_AWK 								\
  (RE_SYNTAX_POSIX_EXTENDED | RE_BACKSLASH_ESCAPE_IN_LISTS	\
   | RE_INTERVALS	    | RE_NO_GNU_OPS)

#define RE_SYNTAX_GREP							\
  (RE_BK_PLUS_QM              | RE_CHAR_CLASSES	\
   | RE_HAT_LISTS_NOT_NEWLINE | RE_INTERVALS	\
   | RE_NEWLINE_ALT)

#define RE_SYNTAX_EGREP									\
  (RE_CHAR_CLASSES        | RE_CONTEXT_INDEP_ANCHORS	\
   | RE_CONTEXT_INDEP_OPS | RE_HAT_LISTS_NOT_NEWLINE	\
   | RE_NEWLINE_ALT       | RE_NO_BK_PARENS				\
   | RE_NO_BK_VBAR)

#define RE_SYNTAX_POSIX_EGREP							\
  (RE_SYNTAX_EGREP | RE_INTERVALS | RE_NO_BK_BRACES)

// P1003.2/D11.2, section 4.20.7.1, lines 5078ff.
#define RE_SYNTAX_ED RE_SYNTAX_POSIX_BASIC

#define RE_SYNTAX_SED RE_SYNTAX_POSIX_BASIC

// Syntax bits common to both basic and extended POSIX regex syntax.
#define _RE_SYNTAX_POSIX_COMMON								\
  (RE_CHAR_CLASSES | RE_DOT_NEWLINE      | RE_DOT_NOT_NULL	\
   | RE_INTERVALS  | RE_NO_EMPTY_RANGES)

#define RE_SYNTAX_POSIX_BASIC						\
  (_RE_SYNTAX_POSIX_COMMON | RE_BK_PLUS_QM)

// Differs from ..._POSIX_BASIC only in that RE_BK_PLUS_QM becomes
// RE_LIMITED_OPS, i.e., \? \+ \| are not recognized.  Actually, this
// isn't minimal, since other operators, such as \`, aren't disabled.
#define RE_SYNTAX_POSIX_MINIMAL_BASIC		\
  (_RE_SYNTAX_POSIX_COMMON | RE_LIMITED_OPS)

#define RE_SYNTAX_POSIX_EXTENDED						\
  (_RE_SYNTAX_POSIX_COMMON | RE_CONTEXT_INDEP_ANCHORS	\
   | RE_CONTEXT_INDEP_OPS  | RE_NO_BK_BRACES			\
   | RE_NO_BK_PARENS       | RE_NO_BK_VBAR				\
   | RE_UNMATCHED_RIGHT_PAREN_ORD)

// Differs from ..._POSIX_EXTENDED in that RE_CONTEXT_INVALID_OPS
// replaces RE_CONTEXT_INDEP_OPS and RE_NO_BK_REFS is added.
#define RE_SYNTAX_POSIX_MINIMAL_EXTENDED				\
  (_RE_SYNTAX_POSIX_COMMON  | RE_CONTEXT_INDEP_ANCHORS	\
   | RE_CONTEXT_INVALID_OPS | RE_NO_BK_BRACES			\
   | RE_NO_BK_PARENS        | RE_NO_BK_REFS				\
   | RE_NO_BK_VBAR	    | RE_UNMATCHED_RIGHT_PAREN_ORD)
// [[[end syntaxes]]] 

// Maximum number of duplicates an interval can allow.  Some systems
// (erroneously) define this in other header files, but we want our
// value, so remove any previous define.
// If sizeof(int) == 2, then ((1 << 15) - 1) overflows.
#define RE_DUP_MAX (0x7fff)


// POSIX `cflags' bits (i.e., information for `regcomp').

// If this bit is set, then use extended regular expression syntax.
// If not set, then use basic regular expression syntax.
#define REG_EXTENDED 1

// If this bit is set, then ignore case when matching.
// If not set, then case is significant.
#define REG_ICASE (REG_EXTENDED << 1)

// If this bit is set, then anchors do not match at newline
//   characters in the string.
// If not set, then anchors do match at newlines. 
#define REG_NEWLINE (REG_ICASE << 1)

// If this bit is set, then report only success or fail in regexec.
// If not set, then returns differ between not matching and errors. 
#define REG_NOSUB (REG_NEWLINE << 1)


// POSIX `eflags' bits (i.e., information for regexec).

// If this bit is set, then the beginning-of-line operator doesn't match
//   the beginning of the string (presumably because it's not the
//   beginning of a line).
// If not set, then the beginning-of-line operator does match the
//   beginning of the string. 
#define REG_NOTBOL 1

// Like REG_NOTBOL, except for the end-of-line. 
#define REG_NOTEOL (1 << 1)



class re_pattern_buffer {
public:
	re_pattern_buffer(void);
	~re_pattern_buffer(void);

// [[[begin pattern_buffer]]]
	// Space that holds the compiled pattern.  It is declared as
	// `unsigned char *' because its elements are
	// sometimes used as array indexes.
	unsigned char *buffer;

	// Number of bytes to which `buffer' points.
	unsigned long int allocated;

	// Number of bytes actually used in `buffer'.
	unsigned long int used;

	// Syntax setting with which the pattern was compiled.
	reg_syntax_t syntax;

	// Pointer to a fastmap, if any, otherwise NULL.  re_search uses
	// the fastmap, if there is one, to skip over impossible
	// starting points for matches.
	char *fastmap;

	// Either a translate table to apply to all characters before
	// comparing them, or zero for no translation.  The translation
	// is applied to a pattern when it is compiled and to a string
	// when it is matched.
	RE_TRANSLATE_TYPE translate;

	// Number of subexpressions found by the compiler.
	size_t re_nsub;

	// Zero if this pattern cannot match the empty string, one else.
	// Well, in truth it's used only in `re_search_2', to see
	// whether or not we should use the fastmap, so we don't set
	// this absolutely perfectly; see `re_compile_fastmap' (the
	// `duplicate' case).
	unsigned can_be_null : 1;

	// If REGS_UNALLOCATED, allocate space in the `regs' structure
	// for `max (RE_NREGS, re_nsub + 1)' groups.
	// If REGS_REALLOCATE, reallocate space if necessary.
	// If REGS_FIXED, use what's there.
#define REGS_UNALLOCATED 0
#define REGS_REALLOCATE 1
#define REGS_FIXED 2
	unsigned regs_allocated : 2;

	// Set to zero when `regex_compile' compiles a pattern; set to one
	// by `re_compile_fastmap' if it updates the fastmap.
	unsigned fastmap_accurate : 1;

	// If set, `re_match_2' does not return information about
	// subexpressions.
	unsigned no_sub : 1;

	// If set, a beginning-of-line anchor doesn't match at the
	// beginning of the string.
	unsigned not_bol : 1;

	// Similarly for an end-of-line anchor.
	unsigned not_eol : 1;

	// If true, an anchor at a newline matches.
	unsigned newline_anchor : 1;

// [[[end pattern_buffer]]]
};


// Type for byte offsets within the string.  POSIX mandates this.
typedef int regoff_t;


// This is the structure we store register match data in.  See
// regex.texinfo for a full description of what registers match.
class re_registers
{
public:
	re_registers(void);
	~re_registers(void);
	void clear(void);
	unsigned num_regs;
	regoff_t *start;
	regoff_t *end;
};



//******************************************************************************
//
// User-callable function prototypes
// 
//******************************************************************************

void re_Set_EGrep(void);  // Set egrep syntax options
const char *re_compile(const char *pattern, re_pattern_buffer *bufp);
void re_print(re_pattern_buffer *bufp);

// Returns length of string matched.
int re_match (re_pattern_buffer *bufp, 
			  const char *string, 
			  int size, 
			  int pos, 
			  re_registers *regs);



// Use re_search to do a grep-like search of a string.
// Search is first attemted at string+startpos.  If that fails the search is 
//   attempted at string+startpos+1.  And so on up to and including
//   string+range.
// Return value is position in string at which match was found.
//   -1 if no find.  -2 if error.
// bufp     = the pattern previously complied by re_compile.
// string   = pointer to the string to be searched.
// size     = length of string
// startpos = offset of first character to search in string
// range    = offset of last byte of string at which search can 
//              start.  You probably want this to be large so that the
//              entire string gets searched.
// regs     = NULL or re_registers struct where matching strings
//              and substrings are recorded.
int re_search (re_pattern_buffer *bufp, 
			   const char *string, 
			   int size, 
			   int startpos, 
			   int range, 
			   re_registers *regs);






// Define MATCH_MAY_ALLOCATE unless we need to make sure that the
// searching and matching functions should not call alloca.  On some
// systems, alloca is implemented in terms of malloc, and if we're
// using the relocating allocator routines, then malloc could cause a
// relocation, which might (if the strings being searched are in the
// ralloc heap) shift the data out from underneath the regexp
// routines.
//
// Here's another reason to avoid allocation: Emacs
// processes input from X in a signal handler; processing X input may
// call malloc; if input arrives while a matching routine is calling
// malloc, then we're scrod.  But Emacs can't just block input while
// calling matching routines; then we don't notice interrupts when
// they come in.  So, Emacs blocks input around all regexp calls
// except the matching calls, which it leaves unprotected, in the
// faith that they will not malloc.

// Normally, this is fine.
#define MATCH_MAY_ALLOCATE


// If we are not linking with Emacs proper,
// we can't use the relocating allocator
// even if config.h says that we can.
#undef REL_ALLOC
#define BYTEWIDTH 8 // In bits. 
typedef char boolChr;
#define UNSIGN_boolChr unsigned char  // To avoid Lint warnings


// These are the command codes that appear in compiled regular
// expressions.  Some opcodes are followed by argument bytes.  A
// command code can specify any interpretation whatsoever for its
// arguments.  Zero bytes may appear in the compiled regular expression. 

enum re_opcode_t
{
	no_op = 0,

	// Succeed right away--no more backtracking. 
	succeed,

	// Followed by one byte giving n, then by n literal bytes.
	exactn,

	// Matches any (more or less) character.
	anychar,

	// Matches any one char belonging to specified set.  First
	// following byte is number of bitmap bytes.  Then come bytes
	// for a bitmap saying which chars are in.  Bits in each byte
	// are ordered low-bit-first.  A character is in the set if its
	// bit is 1.  A character too large to have a bit in the map is
	// automatically not in the set.
	charset,

	// Same parameters as charset, but match any character that is
    // not one of those specified.
	charset_not,

	// Start remembering the text that is matched, for storing in a
	// register.  Followed by one byte with the register number, in
	// the range 0 to one less than the pattern buffer's re_nsub
	// field.  Then followed by one byte with the number of groups
	// inner to this one.  (This last has to be part of the
	// start_memory only because we need it in the on_failure_jump
	// of re_match_2.) 
	start_memory,

	// Stop remembering the text that is matched and store it in a
	// memory register.  Followed by one byte with the register
	// number, in the range 0 to one less than `re_nsub' in the
	// pattern buffer, and one byte with the number of inner groups,
	// just like `start_memory'.  (We need the number of inner
	// groups here because we don't have any easy way of finding the
	// corresponding start_memory when we're at a stop_memory.) 
	stop_memory,

	// Match a duplicate of something remembered. Followed by one
	// byte containing the register number.
	duplicate,

	// Fail unless at beginning of line.
	begline,

	// Fail unless at end of line.
	endline,

	// Succeeds if at beginning of buffer (if emacs) or at beginning
	// of string to be matched (if not). 
	begbuf,

	// Analogously, for end of buffer/string.
	endbuf,

	// Followed by two byte relative address to which to jump.
	jump,

	// Same as jump, but marks the end of an alternative.
	jump_past_alt,

	// Followed by two-byte relative address of place to resume at
	// in case of failure.
	on_failure_jump,

	// Like on_failure_jump, but pushes a placeholder instead of the
	// current string position when executed.
	on_failure_keep_string_jump,

	// Throw away latest failure point and then jump to following
	// two-byte relative address. 
	pop_failure_jump,

	// Change to pop_failure_jump if know won't have to backtrack to
	// match; otherwise change to jump.  This is used to jump
	// back to the beginning of a repeat.  If what follows this jump
	// clearly won't match what the repeat does, such that we can be
	// sure that there is no use backtracking out of repetitions
	// already matched, then we change it to a pop_failure_jump.
	// Followed by two-byte address. 
	maybe_pop_jump,

	// Jump to following two-byte address, and push a dummy failure
	// point. This failure point will be thrown away if an attempt
	// is made to use it for a failure.  A `+' construct makes this
	// before the first repeat.  Also used as an intermediary kind
	// of jump when compiling an alternative.
	dummy_failure_jump,

	// Push a dummy failure point and continue.  Used at the end of
	// alternatives.
	push_dummy_failure,

	// Followed by two-byte relative address and two-byte number n.
	// After matching N times, jump to the address upon failure.
	succeed_n,

	// Followed by two-byte relative address, and two-byte number n.
	// Jump to the address N times, then fail.
	jump_n,

	// Set the following two-byte relative address to the
	// subsequent two-byte number.  The address *includes* the two
	// bytes of number.
	set_number_at,

	wordchar,	 // Matches any word-constituent character.
	notwordchar, // Matches any char that is not a word-constituent.

	wordbeg,	 // Succeeds if at word beginning.
	wordend,	 // Succeeds if at word end.  

	wordbound,	 // Succeeds if at a word boundary. 
	notwordbound // Succeeds if not at a word boundary. 

} ;


#define SIGN_EXTEND_CHAR(c) ((signed char) (c))

#endif