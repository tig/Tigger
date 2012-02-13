/*  ez.c

                  ez.c    (c) KindelCo Software Systems, 1987

                          EZ - The tlw Screen Editor


Author:   Charles E. Kindel, Jr. (tigger)

Started:  July 17, 1987
Version:  2.03


NAME
    EZ - A screen editor.

SYNOPSIS
    ez filename

DESCRIPTION
    EZ is a screen editor intended for generating and modifying text
    files in an interative mode.  A window portraying 23 lines of the
    text shows the changes or additions to the text as they are made.
    A command line at the bottom is reserved for advisory messages
    during the edit session.

    A cursor in the window shows where the text modification of the file
    will occur.  The window moves automatically as the cursor is moved
    beyond the edge of the window.  If the cursor moves below the bottom
    margin of the screen, the window moves down to show a lower portion
    of the text (assuming more text exists below the current window).
    Similarly, if the cursor moves above the top of the screen, the
    window moves up to reveal the text above the current window unless
    the cursor is already at the top of the screen.

    The commands, described below, include moving the cursor, inserting
    text, and deleting the text.

    On starting up, the text is transferred from the backing store to
    the processor memory.  A newline is added at the end if one is not
    there.  At the end of the edit session, the modified text is trans-
    ferred from memory to the backing store.  Every attempt is made to
    avoid the loss of modifications.

    The roots of this small editor are found in Cambridge, Massechusettes
    where TECO and real-time TECO were developed at MIT.

EXAMPLES
    ez filename         Edit filename. File may or may not already exist.
                        (NOTE: This is the ONLY proper usage).

MESSAGES
    Usage synopsis given when illegal command form is entered.

    The following messages may appear on the command line:
    "AT TOP"
    "AT END"
    "BUFFER MODIFIED.  DO YOU WISH TO SAVE MODIFICATIONS? (y or n)."
    "EZ version 1.0 - 29-Jul-87."
    "New file."
    "Read fails!"
    "Writing ### bytes to filename."
    "Write fails!"
    "Move to what line:"

DIAGNOSTICS
    Exit status is 0 if the file was found or if a new file is being
    edited.

    Exit status is 1 if the file exists but could not be accessed.

SEE ALSO
    Information regarding TECO and real-time TECO developed at MIT
    in Cambridge, Massechusettes.

LIMITATIONS
    Text size is limited to 65,000 characters.  No repeats, no macros,
    no cut and paste, no search, no windows.  Tabstops are set to every
    4 spaces.

FACILITY
    UNIX BSD 4.2 System - ez.c.
    MS-DOS PC System - ez.c.

BUILD
    On UNIX BSD 4.2 System: make
    On MS-DOS PC System (using TurboC): make
    (you need Terminal and Xsys routines for the PC.)

BUGS
     The text size is limited to 65535 characters.  No repeats, no macros, no
     cut and paste, no search, no windows.  Tabstops are set to every 4 spaces.

REVISIONS
     DATE           VER     NAME      CHANGE
     July 18, 1987  1.00    Kindel    Original
     July 19, 1987  1.01    Kindel    Attempted compile on some modules.
     July 19, 1987  1.10    Kindel    Read a file and display the screen.
     July 20, 1987  1.20    Kindel    Insert characters, Delete characters
                                      (no scrolling)
     July 21, 1987  1.30    Kindel    Move right, left, up, down.
     July 22, 1987  1.35    Kindel    Paint screen if you move off of it.
     July 23, 1987  1.40    Kindel    Buffer delete routines.
     July 25, 1987  1.45    Kindel    Primitive display deleting.
     July 26, 1987  1.50    Kindel    Scrolling is attempted.
     July 27, 1987  1.70    Kindel    Scrolling, Most commands, Move by page,
                                      Move to top, end, etc...
     July 29, 1987  1.90    Kindel    Beta version.  Re-wrote DpDel and cleaned
                                      up display routines.
     July 30, 1987  2.00    Kindel    Fixed most major bugs.  Added go to line
                                      number function.  Added beeps.
     July 31, 1987  2.02    Kindel    More bug fixes.  Changed buffer size to
                                      65000 bytes to try to avoid segmentation
                                      faults.


COMMANDS
    Commands are implemented by binding each command to a control
    character entered from the keyboard.  Control characters are entered
    by depressing the control key and then one other key simultaneously
    (just as with the shift key).  The binding of commands to control
    keys is shown in the summary below.

    Any key input that is not assigned to one of the commands listed
    below becomes an insert-character command which causes that character
    to be inserted into the text.  Control characters inserted into the
    text are shown in inverse video, except that the carriage return and
    the tab character are displayed as newlines or spaces to the next
    tabstop, respectively.

    If at the top (bottom) of the text, a command that attempts to move
    or delete beyond the top (bottom) of the text generates an appropriate
    error message.

    A word is interpretted as any sequence of non-whitespace characters.

    The following abbreviations are used below:
    bob - beginning of text buffer.
    bol - beginning of line.
    bow - beginning of word.
    chars - characters.
    eol - end of line.
    eow - end of word (first non-whitespace char after word).
    eob - end of text buffer.

    ABRT    Abort the edit session.
        If the text has been modified since the beginning of the edit
        session or the last write to a disk, a question appears on the
        command line:
            BUFFER MODIFIED.  DO YOU WISH TO SAVE MODIFICATIONS? (y or n).
        If the response is 'Y', 'y', or <CR>, the edit session is terminated
        without saving the current text; otherwise, the session continues.

    DBOL    Delete to the beginning of the line.
        The chars to the left of the cursor up to the bol are deleted.  If
        the cursor is at the bol, the previous line is deleted.  An error
        message is generated if the cursor is at the beginning of the text.

    DBOW    Delete to the beginning of the previous word.
        The whitespace, if any, and then the word to the left of the cursor
        is deleted.  If the cursor is in the middle of a word, all of the
        characters to the left of the cursor up to the beginning of the word
        are deleted.  An error message is generated if the cursor is at the
        beginning of the text.

    DCHR    Delete char under the cursor.
        The character under the cursor is deleted.  If the cursor is at the
        end of the line, the newline is deleted.  An error message is
        displayed if the cursor is at the end of the text.

    DELP    Delete previous character.
        The character to the left of the cursor is deleted.  If the cursor
        is at the bol, the previous newline is deleted and the two lines are
        joined.  An error message is displayed if the cursor is at the
        beginning of the text.

    DEOL    Delete to the end of the line.
        The characters from the cursor to the eol are deleted, including the
        newline.  An error message is displayed if the cursor is at the end
        of the text.

    DEOW    Delete to the end of the next word.
        The whitespace, if any, and the word following the cursor are deleted,
        including the character above the cursor.  If the cursor is in the
        middle of a word, the character above the cursor and the rest of the
        word are deleted.  An error message is displayed if the cursor is at
        the end of the text.

    MBOB    Move to the beginning of the text buffer.
        Moves the cursor to the first character in the text.  An error message
        is displayed if the cursor is already at the beginning of the text.

    MBOL    Move to the beginning of the line.
        Moves the cursor to the first character of the line or to the first
        characterof the previous line if it is already at the bol.  An error
        message is displayed if the cursor is at the beginning of the text.

    MBOW    Move to the beginning of the previous word.
        Moves the cursor to the first character of the word to the left of
        the cursor.  If the cursor is at the first character of a word, the
        cursor is moved to the first character of the previous word.  An error
        message is displayed if the cursor is at the beginning of the text.

    MEOB    Move to the end of the text buffer.
        Moves the cursor to the last character in the text buffer.  The last
        character in the text buffer is always a newline character.  An error
        message is displayed if the cursor is already at the end of the text.

    MEOL    Move to the end of the line.
        Moves the cursor to the newline character at the end of the line.  If
        the cursor is already at the end of the line, the cursor is moved to
        the end of the next line.  An error message is displayed if the cursor
        is at the end of the text.

    MEOW    Move to the end of the word.
        Moves the cursor to the position just past the last character of the
        next word in the text.  If the cursor is already at the end of a word,
        the cursor is moved to the next position just past the end of the same
        word.  An error message is displayed if the cursor is at the end of
        the text.

    MVDN    Move down one line.
        Moves the cursor straight down to the same position on the next line.
        If the next line does not extend out as far as the current cursor
        position, the cursor moves to the end of the next line.  If the same
        positon on the next line is in the middle of a tab, the cursor is
        moved to the next tab stop.  If the cursor is on the last line of the
        text, the cursor moves to the end of the text.  If the cursor is on
        the last line of the dislay, the next line is displayed on the bottom
        of the screen (the top line on the display is erased).  An error
        message is displayed if the cursor is at the end of the text.

    MVLF    Move left one character.
        Moves the cursor left one position.  If the cursor is at the beginning
        of a line, the cursor moves up to the end of the previous line.  An
        error message is displayed if the cursor is at the beginning of text.

    MVNP    Move to next page.
        Moves the cursor down 22 lines in the same fashion as MVDN.  Move to
        the end of text if less than 22 lines are left.  An error message is
        displayed if the cursor is at the end of the text.

    MVPP    Move to previous page.
        Moves the cursor up 22 lines in the same fashion as MVUP.  Moves to
        the beginning of the buffer if the cursor is less than 22 lines
        from the top of the buffer.  An error message is displayed if the
        cursor is at the top of the text.

    MVRT    Move right one character.
        Moves the cursor right one position.  If the cursor is at the end of
        the line, the cursor moves to the beginning of the next line.  An
        error message is displayed if the cursor is at the end of the text.

    MVUP    Move up one line.
        Moves the cursor up to the same position on the line above the cursor.
        If the destination line does not extend out to the current cursor
        position, then the cursor is moved to the end of the destination line.
        An error message is displayed if the cursor is at the beginning of
        the text.

    MTLN    Move to a specific line number.
        Prompts the user for a line number and moves to that line.  If a line
        number is specified that is greater than the end of the buffer then
        go to the end of the buffer.

    QUIT    Write, then quit.
        If the text has been modified, the text buffer is written to the
        backing store and then the edit session is terminated.

    VERS    Print version number, refresh display.
        The version number and date of last change is printed on the command
        line and the display is completely refreshed in case of some
        contamination.

    WRIT    Write text buffer to text.
        Save the text buffer to the backing store.  The entire text is
        written regardless of the position of the cursor.

QUICK REFERENCE

    CONTROL
     KEY   COMMAND        DEFINITION
    ----------------------------------------------------------------
      A     MBOB        Move to bob.
      B     MBOL        Move to bol.
      C     QUIT        Write, then quit.
      D     DCHR        Delete char under cursor.
      E     DEOW        Delete to eow.
      F     MEOL        Move to eol.
      G     DEOL        Delete to eol.
      H     MVLF        Move left one space.
      I      tab        Insert spaces up to next tab stop.
      J     MVDN        Move down one line.
      K     MVUP        Move up one line.
      L     MVRT        Move right one position.
      M                 Start text on a new line.
      N     MVNP        Move cursor up one page.
      O     MBOW        Move to bow.
      P     MEOW        Move to eow.
      Q                     reserved for command macros.
      R                     reserved for repeat command.
      S                     reserved for search.
      T                     reserved for take a line.
      U     DBOL        Delete to bol.
      V     VERS        Print EZ version, refresh display.
      W     DBOW        Delete to bow.
      X                     reserved for recall repeat count.
      Y     MVPP        Move to previous page.
      Z     MEOB        Move to eob.
            QUOT            reserved for "take next char literally".

      Del   DELP        Delete previous char.
     ESC-C  ABRT        Abort the edit, no write.
     ESC-W  WRIT        Write text buffer to backing store, no quit.
     ESC-G  MTLN        Move to a line.

*/

/*==========================================================================*/
/*       ez.c     Charles E. Kindel   (C) KindelCo Software Systems, 1987   */
/*==========================================================================*/

/* IMPORTS -----------------------------------------------------------------*/

#include <stdio.h>
#include "ez.h"
#include "xsys.h"                      /* Serial io routines                */
#include "disp.h"                      /* Display routines                  */
#include "buf.h"                       /* Buffer routines                   */
#include "cmnd.h"                      /* Command loop routines             */

/* LOCAL SYMBOLIC DEFINITIONS ----------------------------------------------*/
#define ERROR       2                  /* If there isn't a file name no the command line */



/*==========================================================================*/
/*                          main program                                    */
/*                                                                          */
/*--------------------------------------------------------------------------*/
int main (argc, argv)
int argc;
char *argv[];
{
     /* process command line */
     if (argc == 2)
     {     BfFileNamep = argv [1];
           Xinit ();                            /* initialize serial io     */
           DpInit ();                           /* initialize display       */
           BfInit (BfFileNamep);                /* initialize buffer, and attempt to read the input file */
           Cmnd ();                             /* the command loop         */
     }
     else
        fprintf (stderr, "ez: must include file name.\n");
     return (ERROR);
} /* end of main */



/*==========================================================================*/
/*    END OF  ez.c     (C) KindelCo Software Systems, 1987                  */
/*==========================================================================*/
