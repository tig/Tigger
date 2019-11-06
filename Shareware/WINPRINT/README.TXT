WinPrint 1.5 Release Notes

Release 1.52  June 16, 1994

Copyright (c) 1990-1994 Charles E. Kindel, Jr.
Portions Copyright (c) 1985-1994 Microsoft Corporation.

     Charles E. Kindel, Jr.
     22925 N.E. 12th Place
     Redmond, WA 98053
     
     CompuServe ID:  71551,1455
     FAX/Voice:     (206) 868-9591

*** NOTE ****
In previous releases my CompuServe ID was given incorrectly.
If you had trouble contacting me please try again
*************

NEW WAY TO REGISTER:  Type "GO SWREG" at any "!" prompt on
CompuServe and you will be able to register WinPrint and
many other shareware applications automatically!  Your
CompuServe account will be charged the shareware
registration fee and your registration number will be sent
to you via CompuServe Mail.

Please do not be shy in reporting bugs or suggestions!

For information on using WinPrint or for registering
WinPrint use WinHelp to read the WINPRINT.HLP file.

WHAT IS WINPRINT?

     WinPrint is a Windows shareware application that allows
     you to easily print ASCII text files from within
     Windows.  It allows you to specify headers and footers
     with page numbers, fonts, margins, landscape v.
     portrait mode, and "multiple pages up" printing.  See
     the WINPRINT.HLP file for a more complete description.
     
     WinPrint was designed from the very beginning to be a
     tool for printing plain text files.  Version 1.3 and
     Version 1.4 do not support the printing of graphics,
     word processing documents, or spreadsheets.  The goal
     from has been to keep WinPrint as small, efficient, and
     simple as possible.
     
     If you want to print graphics use PaintBrush it works
     just fine.  If you want to print your word processing
     documents, use your word processor, and if you want to
     print spreadsheets use your spreadsheet package; they
     all print nicely.  But if you want to print plain ASCII
     text files use WinPrint!

     The WINPRINT.HLP file contains all the documentation
     you will need.  It also contains information on how to
     register WinPrint.
     
GETTING STARTED
     
     A few quick notes on what is needed to run WinPrint:
     
     Under Windows 3.0 and 3.0a you need the following
     Dynamic Link Libraries (DLLs) in your path:
          
          TDUTIL.DLL     - WinPrint Support DLL
          COMMDLG.DLL    - Microsoft Common Dialogs DLL.
                    You only need    this file if you are
                    running on Windows 3.0.
     
     You will also need the main WINPRINT.EXE executable
     file and the help file (WINPRINT.HLP).
     
     I recommend that you create a "WINAPPS" directory off
     of your root directory in which you place all of your
     "small" Windows applications such as WinPrint.  Copy
     the above files into this directory and make sure that
     the directory is in your path statement.
     
     You can now run WinPrint just as you would any Windows
     application.  See the section in the WINPRINT.HLP file
     titled STARTING WINPRINT for more details.
     
NOTES TO USERS OF PREVIOUS VERSIONS OF WINPRINT 1.x

     WinPrint 1.32 was designed Windows 2.x application.  It
     worked correctly under Windows 3.0 because it adhered
     to Microsoft's programming guidelines for Windows.
     
     WinPrint 1.4 has been designed as a Windows 3.x
     application.  It takes advantage of features found only
     in Windows 3.0 and above.  As such, it will not run on
     Windows 2.x, and will not operate correctly with
     printer drivers that have not been updated to the
     Windows 3.0 specification.
     
     Versions of Windows previous to Windows 3.0 provided no
     way for applications to store printer settings
     "locally."  For example any change made to printer
     settings from within an application (such as landscape
     mode) were made system wide.  The Windows 3.0 printer
     driver specification allows for applications to change
     printer settings locally.  WinPrint 1.4 takes advantage
     of this, and adheres to the policy that no application
     except Control Panel can make system wide printer
     setting changes.
     
     One of the biggest bugs in WinPrint 1.32 was that it
     incorrectly selected fonts.  For example you would tell
     it to print using a 10 point font, but it would
     actually use a font much smaller.  This has been
     corrected in WinPrint 1.4.
     
     Because of the changes to WinPrint 1.4 are fundamental
     in nature, saved configurations from earlier versions
     will not be converted.  It might have been possible to
     do a partial conversion of your settings to the new
     format, but there are so many new features and bug
     fixes in 1.4 that the conversion would have been
     inaccurate at best.  The decision was made that it
     would be easier for the user to re-create your settings
     than to try to figure out what was converted correctly
     and what was not.
     
     WinPrint 1.32 saved all configuration information in
     the WIN.INI file.  WinPrint 1.4 saves all of it's
     configuration information in the WINPRINT.INI file.
     The format of the information has changed, but when
     WinPrint 1.4 is run on a machine for the first time, it
     copies your registration information to the
     WINPRINT.INI file.  Once you run WinPrint 1.4 for the
     first time you can delete all WinPrint related sections
     from your WIN.INI file if you wish.
     
     The format of WinPrint registration numbers changed
     between versions 1.3 and 1.4.  WinPrint 1.4 and up will
     correctly decode your 1.32 registration number and
     convert it to the new format.  This has been done for
     several reasons:
     
     1) The old WinPrint registration number was up to 12
     characters long and was too difficult to enter.
     
     2) The new number format allows me to more easily track
     what version of WinPrint a user is using.
     
     Version 1.5 of WinPrint was designed to specifically
     support Windows 3.1 and to address many small bugs.  It
     has not been extensively tested on Windows 3.0.  If you
     are still using Windows 3.0, it is recommeded that you
     DO NOT upgrade to WinPrint 1.5 until you also upgrade
     to Windows 3.1.

KNOWN PROBLEMS
     Version 1.5 of WinPrint has been extensively tested.
     However there are a few problems that are known and are
     not worth the time it would take to fix at this point:
     
     UNC FILENAMES ARE NOT SUPPORTED
          WinPrint 1.5 does not correctly support "UNC"
     style filenames.  UNC filenames are of the form
     "\\SERVER\SHARE\DIRECTORY\FILENAME.EXT".  Future
     versions of WinPrint will support these file
     specifications correctly.
     
     SOME WINDOWS 3.0 PRINTER DRIVERS ARE BUGGY
          Many of the Dot Matrix printer drivers shipped
     with Windows 3.0 are buggy, and most do not in fact
     conform the the Windows 3.0 printer driver
     specification.  Because of this you will not be able to
     change printer settings from WinPrint with these
     drivers.  See the WINPRINT.HLP file for more
     information on this.  It is highly recommended that you
     upgrade to Windows 3.1.
     
     POSTSCRIPT EMULATORS REPORT INCORRECT PAGE SIZE
          Several PostScript addon products and emulators
     incorrectly report page size to Windows.  The result is
     that when WinPrint is set up so all margins are set to
     0.00", some text is printed "off page."  If you
     encounter this problem simply increase all four margins
     until the problem goes away (usually 0.10" works).
     This problem has been noted on the HP LaserJet IIIsi in
     PostScript mode and the PacificPage Postscript
     cartridge for the HP LaserJet II.
     
     DOT MATRIX DRIVERS SELECT FONTS INCORRECTLY
          Some Windows 3.0 dot matrix printer drivers allow
     you to set the resoultion that they print graphics at.
     When these drivers are set at their lowest resolution
     settings they print some fonts too big.  The easy work
     around is to set the driver up to use higher resolution
     or print only with device based fonts (see
     WINPRINT.HLP for more details).
     
VERSION HISTORY
     Version 1.52 June 16, 1994
     Windows NT allows printer device names to be
     arbitrarily long (e.g. when connecting to a network
     printer).  Previous versions of WinPrint would fail to
     load configurations when the printer device name was
     too long.   Version 1.52 fixes this problem.
     The helpfile has also had minor updates.
     
     Version 1.51 September 26, 1993
     Command line expansion of fully qualified wildcards on
     the command line was not working in previous versions.
     For example, WinPrint would expand C:\FOO\BAR\*.* to
     C:\FOO\*.*.  This has been fixed.
     
     Version 1.50 December 26, 1992
     Minor bug fixes over 1.50.001.  Fixed help file and
     added support for "GO SWREG" on compuserve!
     
     Version 1.50.001 May 20, 1992
     Fixed bugs regarding printing with tab expansion on.
     Fixed some problems with headers/footers and page
     numbers.  Changed file select listbox back to a single
     column due to popular demand.  New 3D dialog effects
     (requires updated TDUTIL.DLL).  Recompiled with
     Microsoft C7.00 (reduced executable size by over
     10K!!!).

     Version 1.44 February 5, 1992
     Same as version 1.43 except distributed via standard
     shareware channels such as CompuServe.
     
     Version 1.43 June 16, 1994
     This version is a maintenece release.  It fixes some
     minor cosmetic problems.  This version will be
     distributed with a book called Windows 3.1 Power Tools.
     
     Version 1.41 June 16, 1994
     This version is a maintenence release.  Verion 1.40 had
     a major bug in the command line handling (it was
     essentailly compeletely broken).  Version 1.42 also
     removes the need for SHELL.DLL.
     
     Version 1.40 November 1, 1991
     After over a year and a half, there is finally a new
     release of WinPrint!  This version is completely re-
     vamped and updated for Windows 3.0 and 3.1.  New
     features include multiple column printing, Windows
     help, and drag drop from the Windows 3.1 file manager.
     
     Version 1.32 July 22, 1990
     The original wide distribution version.

LICENSE INFORMATION

     WinPrint is not and has never been public domain
     software, nor is it free software.  WinPrint is a
     commercial application distributed as shareware.

     Non-licensed users are granted a limited license
     (registration) to use WinPrint on a 21-day trial basis
     for the purpose of determining whether WinPrint is
     suitable for their needs.  The use of WinPrint, except
     for the initial 21-day trial, requires registration.
     The use of unlicensed (un-registered) copies of
     WinPrint by any person, business, corporation,
     government agency or any other entity is strictly
     prohibited.

     A single user license permits a user to use WinPrint
     only on a single computer.  Licensed users may use the
     program on different computers, but may not use the
     program on more than one computer at the same time.

     No one may modify or patch the WinPrint executable and
     documentation files in any way, including but not
     limited to decompiling, disassembling, or otherwise
     reverse engineering the program.

     A limited license is granted to copy and distribute
     WinPrint only for the trial use of others, subject to
     the above limitations, and also the following:

     1)   WinPrint must be copied in unmodified form,
          complete with the file containing this license
          information.

     2)   The WinPrint product consists of the following
          files:
          WINPRINT.EXE, TDUTIL.DLL, README.TXT, and
          WINPRINT.HLP.  All of these files must be copied
          in unmodified form.

     3)   WinPrint may not be distributed in conjunction
          with any other product with out a specific license
          to do so from Charles E. Kindel, Jr.

     4)   No fee, charge, or other compensation may be
          requested or accepted, except as authorized below:

          A)   Operators of electronic bulletin board
               systems (sysops) may make WinPrint available
               for downloading only as long as the above
               conditions are met.  An overall or time-
               dependent charge for the use of the bulletin
               board system is permitted as long as there is
               not a specific charge for the download of
               WinPrint.
     
          B)   Vendors may charge a disk duplication and
               handling fee, which may not exceed eight
               dollars.

LIMITED WARRANTY

     Charles E. Kindel, Jr. guarantees your satisfaction
     with this product for a period of 90 days from the date
     of original purchase.  If you are unsatisfied with
     WinPrint within that time period contact Charles E.
     Kindel, Jr. via normal or electronic mail for a full
     refund.

     Charles E. Kindel, Jr. warrants that the program will
     perform in substantial compliance with the
     documentation supplied with the software product.  If a
     significant defect in the product is found, the
     Purchaser may obtain a refund.  In no event will such a
     refund exceed the purchase price of the product.

     EXCEPT AS PROVIDED ABOVE, CHARLES E. KINDEL, JR.
     DISCLAIMS ALL WARRANTIES, EITHER EXPRESS OR IMPLIED,
     INCLUDING, BUT NOT LIMITED TO IMPLIED WARRANTIES OF
     MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE,
     WITH RESPECT TO THE PRODUCT.  SHOULD THE PROGRAM PROVE
     DEFECTIVE, THE PURCHASER ASSUMES THE RISK OF PAYING THE
     ENTIRE COST OF ALL NECESSARY SERVICING, REPAIR, OR
     CORRECTION AND ANY INCIDENTAL OR CONSEQUENTIAL DAMAGES.
     IN NO EVENT WILL CHARLES E. KINDEL, JR. BE LIABLE FOR
     ANY DAMAGES WHATSOEVER (INCLUDING WITHOUT LIMITATION
     DAMAGES FOR LOSS OF BUSINESS PROFITS, BUSINESS
     INTERRUPTION, LOSS OF BUSINESS INFORMATION AND THE
     LIKE) ARISING OUT OF THE USE OR THE INABILITY TO USE
     THIS PRODUCT EVEN IF CHARLES E. KINDEL, JR. HAS BEEN
     ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.

     Use of this product for any period of time constitutes
     your acceptance of this agreement and subjects you to
     its contents.

U.S. GOVERNMENT RESTRICTED RIGHTS

     Use, duplication, or disclosure by the Government is
     subject to restrictions as set forth in subdivision
     (b)(3)(ii) of the Rights in Technical Data and Computer
     Software clause at 252.227-7013.
     Contractor/manufacturer is Charles E. Kindel, Jr.,
     22925 N.E. 12th Place, Redmond, WA 98053.

