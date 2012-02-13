/*

                                 CHECK

         An interactive checkbook program designed to make balancing
          your checking, savings, and credit card accounts simple.

                  (C) 1987 KindelCo SoftWare Systems
                          All Rights Reserved


Notes on the design:

   Prompts on 25th line.
   Status on top line.
   Edit:  3 windows
     - One for any data before current entry
     - Current entry window (Where editing is done)
     - One for any data after current entry.

SCREEN WILL LOOK SOMETHING LIKE THIS:

-----------------------------------------------------------------------------
FILENAME.EXT    - CHECK (C) 1987 KindelCo SoftWare Systems -  DATE  SAVE  INS
+===========================================================================+
| DATE | DESCRIPTION                     | C#  |û| DEBIT | CREDIT | BALANCE |
+---------------------------------------------------------------------------+
|Jan  1| Mamma Mia's Pizza               |  123| |   7.98|        |   234.32|
|Feb  3| Deposit - Paycheck 234393       |     |û|       |  232.11|   466.43|
|  .   |                                 |     | |       |        |         |
|  .   |                                 |     | |       |        |         |
|  .   |                                 |     | |       |        |         |
|  .   |                                 |     | |       |        |         |
|  .   |                                 |     | |       |        |         |
|  .   |                                 |     | |       |        |         |
+---------------------------- Edit Window ----------------------------------+
|Feb 17| Price Club (Computer Paper)     |  124| |  24.23|        |   490.66|
+- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -+
| DATE: Feburary 17, 1987    DESCRIPTION: Price Club (Computer Paper)       |
| CHECK NUMBER: 124          RECONCILED: _                                  |
| DEBIT/CREDIT: $24.23       BALANCE: $490.66  RECONCILED BALANCE: $223.32  |
+---------------------------------------------------------------------------+
|Feb 22| ATM Withdrawal                  |     |û|  20.00|        |   470.66|
|  .   |                                 |     | |       |        |         |
|  .   |                                 |     | |       |        |         |
+===========================================================================+
   F1-Help | F2-Print | F3-File | F4-Learn
-----------------------------------------------------------------------------
    - Editing/Movement keys:
       - Scroll through records : Up/Dn arrows, PgUp/Dn, Home/End
         Scrolling moves the records behind the "EDIT WINDOW",
         "magnifying" each record as it passes under the edit window.
       - Move from field to field : Ctrl-Right/Left, Tab/ShiftTab, ENTER
       - Edit Keys: Del, Ins, Right/Left, BackSpace, Ctrl-Del, Ctrl-Home
         Ctrl-End.

    - Fields in the edit window have "HOT" selections:
            - DATE: Enter number, 1st 3 letters, for month
                    Enter number for day
                    Comma is automatic
                    Enter last number or any part for year
                    Space bar will return value from previous record
                    Grey +,- will add or subtract days
                    Default will be current date unless adding a record
                     (and then it will be the previous value)
                    Day, Month, and Year are acutally separate fields
                    Return or arrow or tab will move to next field

             - Description:
                    "HOT" (LEARNable) descriptions are available.
                       - Enter first one or two letters and the rest will
                         be filled in.  Programmable flag will determine
                         debit or credit. Hot key could be an ALT-Key
                       - Maybe it could scan the field as inputted for
                         key words and decide debit/credit.
                       - Menu could pop up listing available
                         descriptions.  Just scroll to the one you want
                         and hit return!
                       - Could use Alt-Keys?

             - CHECK NUMBER:
                    - Defaults to the next check unless you are
                      adding a record.
                    - Space zaps.
                    - Return, Arrow, or Tab accepts and goes on.
                    - Grey +,- add,subtract
                    - Does not have to be a number.

             - RECONCILED:
                    - Any alpha key and space bar toggle.

             - DEBIT/CREDIT:
                    - Selected automatically, but can be changed by hitting
                      the normal (-) key.
                    - Normal input (use strings)
                    - When on DEBIT/CREDIT and return is hit, do same thing
                      as though it was a down arrow.

DISPLAY:
    - Treat top and bottom as two internal windows
    - Externally (to the user) they will be one.
    - Edit window is another window.

CALCULATION:
    - At startup calculate all balances
    - whenever a credit/debit is added or changed
      call a routine to recalculate all values that come after.
      This makes screen writing more smooth.

DATA STRUCTURES:
    - Linked list of records:
                  Previous : pointer
                  Month : integer
                  Day : integer
                  Year : integer
                  Disc : string
                  ChkNum : string      (checking only)
                  Reconsiled : boolean (checking only)
                  Debit/Credit : float
                  balance : float (not saved to disk)
                  Next : pointer




-------------------------------------------------------------------------*/
