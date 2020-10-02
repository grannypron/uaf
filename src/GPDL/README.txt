The GPDL compiler compiles your GPDL program into a binary
file that can be executed by UAFWin.

Usage:

   GPDLcomp TextFile  BinaryFile  [optional listFile]

Errors during compilation will be printed in the 'DOS'
window that evoked the compiler.  They generally give
a line number and more information than you want.

For UAFWin to find the binary file it must be named:
         talk.bin
An unfortunate lack of foresight on my part.  I originally
meant it to be used for a small purpose.  Hence the name.
The optional listFile is a human-readable 'assembly-language'
listing.  Very helpful when your program dies and a
diagnostic gives an execution address.  

There is not much documentation for how to write a program.
There is a file named 'functions.txt' and a file named 
'language.txt' that were written mostly to remind myself 
of what I had done.

There is a file named talk.txt that is an example of a 
program written using GPDL that provides an 'Ultima-like' 
ability to talk to people in a game design.

The GPDL language was built to provide a foundation for
an ability to write user-defined programs to access and
manipulate any and all of the game state.  Currently, there
are no function available to do much but accept keyboard 
input and produce printed text on the screen.  But it should
be trivial to add functions to accomplish many of the things
that I have seen people ask for on the FRUA mailing list.

For example....

  I want to see if bit 3 of quest variable 19 is set or bit 1
     of quest variable 4 is set.
          *AND*
  A naked, three legged elf cleric is leading the party.
          *AND*
  No thief with experience greater than 2378 is in the party
          *AND*
  No party member is carrying the scroll of thunder.
 
  If these things are true I want to remove the scroll
  of lightning (if present) and set quest variable
  5 to 167 and give the cleric an additional leg (because
  even numbers are better), transport the party to 
  the back room of the temple, clear the 'been-done' 
  flag of the 'do-once' event at cell 5,9, disable
  event chaining, and cause the sun to set.

Now it can be done easily. At least it will be easy once you
tell me what functions are needed so that they can be incorporated
into the compiler and UAFWin.

This example obviously requires a function that returns the
value of quest variables, a logical AND function (to extract 
the individual bits of a number), a function 
that returns attributes of party members, a function that
returns names of items the party is carrying, a function
that changes the party's location, etc.  I hope that the
collective wisdom of the FRUA clan can help develop some
nicely general and useful functions.  That will be the hard
part....not to create a slew of very special-purpose functions.

The language allows local function definitons so that you can
easily break conplex tasks into simple, impossible-to-screw-up
statements. (Ha. Ha. Joke.)  The above example might be coded


// Short name to save typing long system function name.
$FUNC IPMA (pos, attr) {
	$RETURN $IFPARTYMEMBERATTRIBUTE (pos,attr);
	// True if attribute is defined.
} IPMA;

$PUBLIC $FUNC fountainOfYouth() {
    $FUNC condition1() {
	    $IF($BIT($GETQUEST(19),3)) {$RETURN 1;}; // Quest 19 bit 3
		$IF($BIT($GETQUEST(4) ,1)) {$RETURN 1;}; // Quest 4 bit 1
		$RETURN;  // Default value is "".
	} condition1;

	$FUNC condition2() {
	    $IF(IPMA(0,"ThreeLegged")) { // Party leader
	    ....etc
	} condition2;

    $FUNC condition3() {
	    ....etc
	} condition3

	$FUNC condition4) {
	    ....etc
	} condition4

	$FUNC doit() {
	    $SETQUEST(5,167);
		....etc....
	} doit;

    $IF(condition1()) {
    $IF(condition2()) {
    $IF(condition3()) {
    $IF(condition4()) {
       doit();
    };};};};
} fountainOfYouth;

The functions 'condition1()' and so on are local to
function 'fountainOfYouth()' so that the names can be
used elsewhere without any problem.  The word $PUBLIC
makes fountainOfYouth() available from UAFWin's text
event.

So how do you get the fountailOfYouth() function to
execute during game-play?  Elemetary.  You plant a
text event with very special text!  A text event with
the following text:

      **SHAZAM**fountainOfYouth()

No parameters.  But if it turns out that parameters
would be nice then we will add them.  Then the function
could do different things depending on which text event
evoked it.

Note that you can turn on function tracing (with $DEBUG())
to help you figure out where you have made that foolish
mistake.

If anyone actually reads this and wants to know
more you will have made my day.  Feel free to contact
me at   prsteven@facstaff.wisc,edu
Suggestions are particularly welcome since I have never
played a UA game or designed a UA game and have very 
little feel for what would be useful.  CocoaSpud has
indicated that his interest lies in making UAFWin
useful even if beauty must be sacrificed in the process.
You are the person who can judge what 'useful' means, we 
will put up a mild struggle for beauty, and I wager 
that with a bit of discussion we can accomodate both.

PAul
