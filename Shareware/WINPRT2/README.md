# WinPrint 2.0 - An aborted effort to rewrite WinPrint

Copyright © 1994 Charles E. Kindel, Jr.

WinPrint is a Windows application that facilitates the easy printing of standard text files . WinPrint 2.0 is a completely rewritten version of the original WinPrint 1.x. Unless otherwise noted, WinPrint refers to WinPrint 2.0 throughout this documentation.
WinPrint 1.x was very sucessful because it clearly addressed a problem many people had: “How do I print this text file quickly and easily?” Windows itself provides basically two ways users can do this. 

The first is to load the file into Notepad and print it from there. This has several limitations:

* Minimal formatting abilities. Headers and footers are available, but are very limited. In particular there is no choice of fonts and no way to set margins.
* Limited file length support. Under Win16 Notepad could only print files less than about 30K in size. Win32 fixes this.
* No advanced user interface such as drag-n-drop, multiple file selection, etc...

The other method is to use Windows Write. Using Write gets around the file length and formatting problems, but it simply takes too much effort to use.

Most users have an advanced word processor such as Word for Windows or WordPerfect. Trying to use a word processor to print standard ASCII text files is really no different than trying to use Windows Write.

The WinPrint 2.0 design provides a mechanism whereby other types of files besides text can be supported. The WinPrint Printing Engine supports the notion of File Type Drivers. These DLLs know how to process an input stream and output that stream to a output medium that Printing Engine provides. The initial release of WinPrint will support several standard text file type drivers (DOS Text, ANSI Text, Source Code). File type drivers for bitmaps, metafiles, and other file types will be made available as resources permit.

WinPrint 2.0  will be sold and distributed as shareware. The shareware license will be per-user. Because WinPrint can be used as part of other applications as well as standalone, this must be taken into account. The basic premise is that every user of WinPrint must have a valid license. A valid license allows that user to use the product standalone or as part of some custom solution. If someone wants to distribute a solution that uses WinPrint as a component, the creator of the solution must purchase a license for each user. 
