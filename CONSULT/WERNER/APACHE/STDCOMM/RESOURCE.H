#define SETTINGSDLGBOX              100

#define STDCOMMICON                 200

#define RESTABLE_BAUDRATES          400
#define RESTABLE_PARITY             401
#define RESTABLE_STOPBITS           402
#define RESTABLE_BREAKLENGTHS       403

#define IDD_DLGFRAME                0x100
#define IDD_PORTCB                  0x101
#define IDD_BAUDRATECB              0x102
#define IDD_DATABITSCB              0x103
#define IDD_PARITYCB                0x104
#define IDD_STOPBITSCB              0x105
#define IDD_BREAKLENGTHCB           0x106
#define IDD_DTRDSR                  0x107
#define IDD_RTSCTS                  0x108
#define IDD_XONXOFF                 0x109

#define IDS_COMPREFIX               0x310

#define IDS_BAUD110                 0x320
#define IDS_BAUD300                 0x321
#define IDS_BAUD600                 0x322
#define IDS_BAUD1200                0x323
#define IDS_BAUD2400                0x324
#define IDS_BAUD4800                0x325
#define IDS_BAUD9600                0x326
#define IDS_BAUD14400               0x327
#define IDS_BAUD19200               0x328
#define IDS_BAUD38400               0x329
#define IDS_BAUD56000               0x32A
#define IDS_BAUD128000              0x32B
#define IDS_BAUD256000              0x32C

#define IDS_PARITYNONE              0x330
#define IDS_PARITYEVEN              0x331
#define IDS_PARITYODD               0x332
#define IDS_PARITYMARK              0x333
#define IDS_PARITYSPACE             0x334

#define IDS_ONESTOPBIT              0x340
#define IDS_ONE5STOPBITS            0x341
#define IDS_TWOSTOPBITS             0x342

#define IDS_BREAK100                0x350
#define IDS_BREAK250                0x351
#define IDS_BREAK500                0x352
#define IDS_BREAK750                0x353
#define IDS_BREAK1000               0x354
#define IDS_BREAK1500               0x355

#define IDS_SETTINGS_PORT           0x360
#define IDS_SETTINGS_BAUDRATE       0x361
#define IDS_SETTINGS_DATABITS       0x362
#define IDS_SETTINGS_PARITY         0x363
#define IDS_SETTINGS_STOPBITS       0x364
#define IDS_SETTINGS_BREAKLENGTH    0x365
#define IDS_SETTINGS_FLOWCONTROL    0x366

#define IDS_DEFAULT_PORT            0x370
#define IDS_DEFAULT_BAUDRATE        0x371
#define IDS_DEFAULT_DATABITS        0x372
#define IDS_DEFAULT_PARITY          0x373
#define IDS_DEFAULT_STOPBITS        0x374
#define IDS_DEFAULT_BREAKLENGTH     0x375
#define IDS_DEFAULT_FLOWCONTROL     0x376

#define IDS_CE_RXOVER               0x381
#define IDS_CE_OVERRUN              0x382
#define IDS_CE_RXPARITY             0x383
#define IDS_CE_FRAME                0x384
#define IDS_CE_BREAK                0x385
#define IDS_CE_CTSTO                0x386
#define IDS_CE_DSRTO                0x387
#define IDS_CE_RLSDTO               0x388
#define IDS_CE_TXFULL               0x389
#define IDS_CE_PTO                  0x38A
#define IDS_CE_IOE                  0x38B
#define IDS_CE_DNS                  0x38C
#define IDS_CE_OOP                  0x38D
#define IDS_CE_MODE                 0x38E

// Communication driver defines... (grabbed from WINDOWS.H)
// NOTE: this is VERSION dependant

#define NOPARITY	       0
#define ODDPARITY	       1
#define EVENPARITY	    2
#define MARKPARITY	    3
#define SPACEPARITY	    4

#define ONESTOPBIT	    0
#define ONE5STOPBITS	    1
#define TWOSTOPBITS	    2

#define CBR_110	       0xFF10
#define CBR_300          0xFF11
#define CBR_600          0xFF12
#define CBR_1200         0xFF13
#define CBR_2400         0xFF14
#define CBR_4800         0xFF15
#define CBR_9600         0xFF16
#define CBR_14400        0xFF17
#define CBR_19200        0xFF18
#define CBR_38400        0xFF1B
#define CBR_56000        0xFF1F
#define CBR_128000       0xFF23
#define CBR_256000       0xFF27
