/*---------------------------------------------------------------------------*
  Project:  NitroSDK - tools - buryarg
  File:     buryarg.c

  Copyright 2005.2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: loadrun.c,v $
  Revision 1.14  01/18/2006 02:11:21  kitase_hirotake
  do-indent

  Revision 1.13  09/05/2005 10:54:13  yada
  --type (device type) option was deleted

  Revision 1.12  2005/09/05 01:34:33  yasu
  Made it so version.h is accessed and the latest time stamp for the source file is displayed

  Revision 1.11  09/01/2005 07:21:24  yada
  change exit status

  Revision 1.10  08/31/2005 12:14:27  yada
  applied for OS_Exit()

  Revision 1.9  08/30/2005 01:47:33  yada
  become to be able to load from stdin

  Revision 1.8  08/29/2005 10:00:01  seiki_masashi
  Disabled buffering of standard output

  Revision 1.7  08/25/2005 02:16:11  yada
  able to use Abort string at QuietMode

  Revision 1.6  08/25/2005 00:35:46  yada
  lap time

  Revision 1.5  08/24/2005 13:02:03  yada
  applied for exec timeout

  Revision 1.4  08/24/2005 12:56:53  yada
  change ctrl-c proc

  Revision 1.3  08/24/2005 11:23:00  yada
  lock option of card and cartridge slot are OFF as default

  Revision 1.2  08/24/2005 09:53:35  yada
  applied for new ISD library

  Revision 1.1  08/24/2005 07:44:46  yada
  (none)

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include <sys/stat.h>
#include <getopt.h>
#include <signal.h>
#include <time.h>
#include <version.h>

#include "ISNITRODLL.h"
#include "isd_api.h"

#define  SDK_BOOL_ALREADY_DEFINED_
#include <nitro_win32.h>
#include <nitro/os/common/system.h>

//---- version character string
#define VERSION_STRING     " 1.9  Copyright 2005,2006 Nintendo. All right reserved."
// 1.11 Delete device type
// 1.10 Display latest time stamp of source file
// 1.9 Changed exit status of loadrun itself
// 1.8 Supported terminating character string from OS_EXIT()
// 1.7 Allowed data to be read from standard input
// 1.6 Disable buffering of standard output
// 1.5 Allowed character string termination even in Quiet Mode
// 1.4 Implemented wrap display
// 1.3 Implemented execution timeout
// 1.2 Changed processing when execution is stopped using ctrl-c
// 1.1 Made it so the slot is turned off when execution is stopped using ctrl-c
// 1.0 Open to public

//---- exit number
#define EXIT_NUM_NO_ERROR               207     // Successful exit (however, this will not result next time)
#define EXIT_NUM_USER_SIGNAL            206     // Forcible exit by user (ctrl-C)
#define EXIT_NUM_EXEC_TIME_OUT          205     // Forcible exit due to execution timeout
#define EXIT_NUM_TIME_OUT               204     // Forcible exit due to display timeout
#define EXIT_NUM_SHOW_DEVICES           203     // Exit on device list display
#define EXIT_NUM_SHOW_USAGE             202     // Exit on help display
#define EXIT_NUM_SHOW_VERSION           201     // Exit on version display
#define EXIT_NUM_STRING_ABORT           200     // Forcible exit by character string

#define EXIT_NUM_NO_DEVICE              -1      // There are no available devices
#define EXIT_NUM_UNKNOWN_OPTION         -2      // An unknown option was specified
#define EXIT_NUM_ILLEGAL_OPTION         -3      // Illegal use of option
#define EXIT_NUM_NO_INPUT_FILE          -4      // The specified file does not exist or cannot be opened
#define EXIT_NUM_NOT_CONNECT            -5      // Failed to connect to device
#define EXIT_NUM_CANNOT_USE_CARTRIDGE   -6      // Failed to lock cartridge
#define EXIT_NUM_CANNOT_USE_CARD        -7      // Failed to lock card
#define EXIT_NUM_PRINTF_ERROR           -8      // Error while handling printf data
#define EXIT_NUM_LOADING_ERROR          -9      // Error during load

//---- for device specification
char   *gDeviceName[] = {
    "CGBUSB", "CGBSCSI", "NITROUSB", "NITROUIC", NULL
};
int     gDeviceTypeArray[] = {
    ISNTD_DEVICE_CGB_EMULATOR_USB,
    ISNTD_DEVICE_CGB_EMULATOR_SCSI,
    ISNTD_DEVICE_IS_NITRO_EMULATOR,
    ISNTD_DEVICE_IS_NITRO_UIC
};

//---- operational mode
BOOL    gQuietMode = FALSE;            // quiet mode
BOOL    gVerboseMode = FALSE;          // verbose mode
BOOL    gDebugMode = FALSE;            // debug mode

BOOL    gStdinMode = FALSE;            // stdin mode

BOOL    gIsTypeSpecified = FALSE;      // Is there a device type specification?
int     gSpecifiedType;                // Device type if one is specified

BOOL    gIsSerialSpecified = FALSE;    // Is there a serial number specification?
int     gSpecifiedSerial;              // Serial number if one was specified

BOOL    gIsCartridgeLocked = FALSE;    // Should the cartridge slot be locked?
BOOL    gIsCardLocked = FALSE;         // Should the card slot be locked?

int     gTimeOutTime = 0;              // Timeout interval in sec (0 indicates no timeout)
BOOL    gTimeOutOccured = FALSE;       // Has a timeout occurred?

int     gExecTimeOutTime = 0;          // Execution timeout interval in sec (0 indicates no timeout)
int     gExecTimeOutOccured = FALSE;   // Has an execution timeout occurred?

char   *gAbortString = NULL;           // Abort string
BOOL    gStringAborted = FALSE;        // Has execution terminated due to the abort string?

BOOL    gExitAborted = FALSE;          // Exit from OS_Exit()
int     gExitStatusNum = EXIT_NUM_STRING_ABORT; // Return value when terminated by OS_Exit()
int     gExitStrLength;                // Length of exit string

//---- For NITRO library
HINSTANCE gDllInstance;
NITRODEVICEHANDLE gDeviceHandle;
NITRODEVICEID gDeviceId;

//Connected to the device? (for slot switch)
BOOL    gDeviceConnected = FALSE;


//---- device list
#define DEVICE_MAX_NUM      256
#define DEVICE_SERIAL_NONE  0x7fffffff // means no specified
ISNTDDevice gDeviceList[DEVICE_MAX_NUM];
int     gCurrentDevice = -1;
int     gConnectedDeviceNum = 0;

int     gDeviceTypeSpecified = ISNTD_DEVICE_NONE;
int     gDeviceSerialSpecified = DEVICE_SERIAL_NONE;    // means no specified

//---- Input file
#define FILE_NAME_MAX_SIZE  1024
FILE   *gInputFile;
char    gInputFileNameString[FILE_NAME_MAX_SIZE];
BOOL    gIsInputFileOpened = FALSE;

//---- Time
time_t  gStartTime = 0;                // Start time
BOOL    gIsLineHead = TRUE;            // Start of line?
BOOL    gShowLapTime = FALSE;

//---- Signal
BOOL    gStoppedByUser = FALSE;        // Indicates whether or not execution was stopped by the user.


#define printfIfNotQuiet(...)    do{if(!gQuietMode){printf(__VA_ARGS__);}}while(0)

void    displayErrorAndExit(int exitNum, char *message);
BOOL    outputString(char *buf, int bufSize);

/*---------------------------------------------------------------------------*
  Name:         myExit

  Description:  similar to exit()

  Arguments:    exitNum : exit() number

  Returns:      None
 *---------------------------------------------------------------------------*/
void myExit(int exitNum)
{
    //---- Turns off the slot for the cartridge and card
    if (gDeviceConnected)
    {
        (void)ISNTD_CartridgeSlotPower(gDeviceHandle, FALSE);
        (void)ISNTD_CardSlotPower(gDeviceHandle, FALSE);
    }

    //---- Free DLL memory
    ISNTD_FreeDll();

    if (!gQuietMode)
    {
        if (exitNum == EXIT_NUM_USER_SIGNAL)
        {
            printf("\n*** loadrun: stopped by user.\n");
        }
        else if (exitNum == EXIT_NUM_TIME_OUT)
        {
            printf("\n*** loadrun: stopped by print timeout.\n");
        }
        else if (exitNum == EXIT_NUM_EXEC_TIME_OUT)
        {
            printf("\n*** loadrun: stopped by exec timeout.\n");
        }
        else if (gStringAborted)
        {
            printf("\n*** loadrun: stopped by aborting string.\n");
        }
        else if (gExitAborted)
        {
            exitNum = gExitStatusNum;
        }
    }

    exit(exitNum);
}

/*---------------------------------------------------------------------------*
  Name:         listDevice

  Description:  Displays a list of devices and terminates

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
void listDevice(void)
{
    int     n;

    //Data read
    gConnectedDeviceNum = ISNTD_GetDeviceList(&gDeviceList[0], DEVICE_MAX_NUM);
    if (gConnectedDeviceNum < 0)
    {
        displayErrorAndExit(EXIT_NUM_NO_DEVICE, "Cannot access devices.");
    }

    printf("---- Connected devices:\n");

    for (n = 0; n < gConnectedDeviceNum; n++)
    {
        switch (gDeviceList[n].type)
        {
        case ISNTD_DEVICE_CGB_EMULATOR_USB:
            printf("%3d: [CGBUSB]  IS-CGB-EMU(USB) serial:%8d\n", n, gDeviceList[n].serial);
            break;
        case ISNTD_DEVICE_CGB_EMULATOR_SCSI:
            printf("%3d: [CGBSCSI]  IS-CGB-EMULATOR serial(host-id):%02d%02d\n", n,
                   gDeviceList[n].host, gDeviceList[n].serial);
            break;
        case ISNTD_DEVICE_IS_NITRO_EMULATOR:
            printf("%3d: [NITROUSB]  IS-NITRO-EMULATOR serial:%08d\n", n, gDeviceList[n].serial);
            break;
        case ISNTD_DEVICE_IS_NITRO_UIC:
            printf("%3d: [NITROUIC]  IS-NITRO-UIC serial: %08d\n", n, gDeviceList[n].serial);
            break;
        case ISNTD_DEVICE_UNKNOWN:
            printf("%3d: unknown device %x\n", n, (int)gDeviceList[n].ntdId);
            break;
        default:
            printf("Illegal device\n");
            break;
        }
    }

    //---- Search result
    printf("%d device(s) found.\n", gConnectedDeviceNum);

    myExit(EXIT_NUM_SHOW_DEVICES);
}

/*---------------------------------------------------------------------------*
  Name:         searchDevice

  Description:  search device

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
void searchDevice(void)
{
    //---- If there is no device
    if (gConnectedDeviceNum <= 0)
    {
        displayErrorAndExit(EXIT_NUM_NO_DEVICE, "found no device.");
    }

    //If there was some kind of specification
    if (gDeviceTypeSpecified != ISNTD_DEVICE_NONE || gDeviceSerialSpecified != DEVICE_SERIAL_NONE)
    {
        int     n;
        gCurrentDevice = -1;
        for (n = 0; n < gConnectedDeviceNum; n++)
        {
            //---- Matching with specified device
            if (gDeviceTypeSpecified != ISNTD_DEVICE_NONE
                && gDeviceTypeSpecified != gDeviceList[n].type)
            {
                continue;
            }

            //---- Matching with specified serial line
            if (gDeviceSerialSpecified != DEVICE_SERIAL_NONE
                && gDeviceSerialSpecified != gDeviceList[n].serial)
            {
                continue;
            }

            gCurrentDevice = n;
            break;
        }
    }
    //---- First one if there is no specification
    else
    {
        gCurrentDevice = 0;
    }

    //---- The specified device does not exist or is incorrect
    if (gCurrentDevice < 0
        || gDeviceList[gCurrentDevice].type == ISNTD_DEVICE_NONE
        || gDeviceList[gCurrentDevice].type == ISNTD_DEVICE_UNKNOWN)
    {
        displayErrorAndExit(EXIT_NUM_NO_DEVICE, "illegal device.");
    }
}

/*---------------------------------------------------------------------------*
  Name:         displayUsage

  Description:  Displays the usage

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void displayUsage(void)
{
    fprintf(stderr,
            "NITRO-SDK Development Tool - loadrun - Execute NITRO ROM image\n"
            "Build %lu\n\n"
            "Usage: loadrun [OPTION] <SrlFile>\n"
            "\t\tdownload Nitro srl file to debugger and execute.\n\n"
            "Options:\n"
            "  --version                   : Show version.\n"
            "  -h, --help                  : Show this help.\n"
            "  -q, --quiet                 : Quiet mode.\n"
            "  -v, --verbose               : Verbose mode.\n"
            "  -L, --list                  : List connecting device.\n"
            "  -l, --lap                   : Show lap time at each line.\n"
//            "  -d, --type=DEVICE           : Specify device type.\n"
//            "                                DEVICE=CGBUSB|CGBSCSI|NITROUSB|NITROUIC.\n"
            "  -s, --serial=SERIAL         : Specify serial number.\n"
            "  -t, --timeout=SECOND        : Specify quit time after last print.\n"
            "  -T, --exec-timeout=SECOND   : Specify quit time after execute program.\n"
            "  -a, --abort-string=STRING   : Specify aborting string.\n"
            "  -c, --card-slot=SWITCH      : Cartridge slot SWITCH=ON|OFF, default OFF.\n"
            "  -C, --cartridge-slot=SWITCH : Cartridge slot SWITCH=ON|OFF, default OFF.\n"
            "  --stdin, --standard-input   : Read data from stdin instead of <SrlFile>.\n\n",
            SDK_DATE_OF_LATEST_FILE);
}

/*---------------------------------------------------------------------------*
  Name:         displayVersion

  Description:  Displays the version

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void displayVersion(void)
{
    printf("*** loadrun: %s\n", VERSION_STRING);
}

/*---------------------------------------------------------------------------*
  Name:         displayErrorAndExit

  Description:  Displays errors

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void displayErrorAndExit(int exitNum, char *message)
{
    printf("*** loadrun: Error: %s\n", message);

    //---- Free DLL memory
    ISNTD_FreeDll();

    exit(exitNum);
}

/*---------------------------------------------------------------------------*
  Name:         parseOption

  Description:  parses the option line

  Arguments:    argc : argument count
                argv: argument vector

  Returns:      result. less than 0 if error.
 *---------------------------------------------------------------------------*/
void parseOption(int argc, char *argv[])
{
    int     n;
    int     c;
    BOOL    helpFlag = FALSE;

    struct option optionInfo[] = {
        {"help", no_argument, NULL, 'h'},
        {"quiet", no_argument, NULL, 'q'},
        {"verbose", no_argument, NULL, 'v'},
        {"list", no_argument, NULL, 'L'},
        {"lap", no_argument, NULL, 'l'},
        {"debug", no_argument, NULL, 'D'},      //Hidden options
        {"version", no_argument, NULL, '1'},
        {"stdin", no_argument, NULL, 'I'},
        {"standard-input", no_argument, NULL, 'I'},
        {"type", required_argument, 0, 'd'},    //Hidden options
        {"serial", required_argument, 0, 's'},
        {"timeout", required_argument, 0, 't'},
        {"exec-timeout", required_argument, 0, 'T'},
        {"abort-string", required_argument, 0, 'a'},
        {"card-slot", required_argument, 0, 'c'},
        {"cartridge-slot", required_argument, 0, 'C'},
        {NULL, 0, 0, 0}
    };
    int     optionIndex;

    char   *optionStr = NULL;

    //---- suppress error string of getopt_long()
    opterr = 0;

    while (1)
    {
        c = getopt_long(argc, argv, "+hqvlLDd:s:t:T:a:c:C:", &optionInfo[0], &optionIndex);

        //printf("optind=%d optopt=%d  %x(%c) \n", optind, optopt, c,c );

        if (c == -1)
        {
            break;
        }

        switch (c)
        {
        case 'I':                     //---- standard input
            gStdinMode = TRUE;
            break;
        case 'h':                     //---- help display
            helpFlag = TRUE;
            break;
        case 'q':                     //---- quiet mode
            gQuietMode = TRUE;
            break;
        case 'v':                     //---- verbose mode
            gVerboseMode = TRUE;
            break;
        case 'D':                     //---- debug mode
            gDebugMode = TRUE;
            break;
        case '1':                     //---- version display
            displayVersion();
            myExit(EXIT_NUM_SHOW_VERSION);
            break;
        case 'L':                     //---- device list
            listDevice();
            break;
        case 'l':                     //---- lap time
            gShowLapTime = TRUE;
            break;
        case 'd':                     //---- device
            optionStr = (char *)(optarg + ((*optarg == '=') ? 1 : 0));
            {
                int     n;
                for (n = 0; gDeviceName[n]; n++)
                {
                    if (!strcmp(optionStr, gDeviceName[n]))
                    {
                        gDeviceTypeSpecified = gDeviceTypeArray[n];
                        break;
                    }
                }

                if (gDeviceTypeSpecified == ISNTD_DEVICE_NONE)
                {
                    displayErrorAndExit(EXIT_NUM_ILLEGAL_OPTION, "illegal device type.");
                }
            }
            break;
        case 's':                     //---- serial specification
            optionStr = (char *)(optarg + ((*optarg == '=') ? 1 : 0));
            gDeviceSerialSpecified = atoi(optionStr);
            break;
        case 'c':                     //---- card slot lock
            optionStr = (char *)(optarg + ((*optarg == '=') ? 1 : 0));
            if (!strcmp(optionStr, "ON") || !strcmp(optionStr, "on"))
            {
                gIsCardLocked = TRUE;
            }
            else if (!strcmp(optionStr, "OFF") || !strcmp(optionStr, "off"))
            {
                gIsCardLocked = FALSE;
            }
            else
            {
                displayErrorAndExit(EXIT_NUM_ILLEGAL_OPTION, "illegal value for card slot option.");
            }
            break;
        case 'C':                     //---- cartridge slot lock
            optionStr = (char *)(optarg + ((*optarg == '=') ? 1 : 0));
            if (!strcmp(optionStr, "ON") || !strcmp(optionStr, "on"))
            {
                gIsCartridgeLocked = TRUE;
            }
            else if (!strcmp(optionStr, "OFF") || !strcmp(optionStr, "off"))
            {
                gIsCartridgeLocked = FALSE;
            }
            else
            {
                displayErrorAndExit(EXIT_NUM_ILLEGAL_OPTION,
                                    "illegal value for cartridge slot option.");
            }
            break;
        case 't':                     //---- Timeout interval beginning from final display
            optionStr = (char *)(optarg + ((*optarg == '=') ? 1 : 0));
            gTimeOutTime = atoi(optionStr);
            if (gTimeOutTime <= 0)
            {
                displayErrorAndExit(EXIT_NUM_ILLEGAL_OPTION,
                                    "illegal value for abort timeout option.");
            }
            break;
        case 'T':                     //---- Execution timeout interval
            optionStr = (char *)(optarg + ((*optarg == '=') ? 1 : 0));
            gExecTimeOutTime = atoi(optionStr);
            if (gExecTimeOutTime <= 0)
            {
                displayErrorAndExit(EXIT_NUM_ILLEGAL_OPTION,
                                    "illegal value for abort exec timeout option.");
            }
            break;
        case 'a':                     //---- Abort string
            gAbortString = (char *)(optarg + ((*optarg == '=') ? 1 : 0));
            {
                int     length = strlen(gAbortString);
                if (length <= 0 || length > 256)
                {
                    displayErrorAndExit(EXIT_NUM_ILLEGAL_OPTION,
                                        "illegal value for abort string option.");
                }
            }
            break;
        default:
            displayErrorAndExit(EXIT_NUM_UNKNOWN_OPTION, "unknown option.");
        }
    }

    //---- help display
    {
        BOOL    isDisplayHelp = FALSE;

        if (helpFlag)
        {
            isDisplayHelp = TRUE;
        }
        else if (argc <= optind && !gStdinMode)
        {
            isDisplayHelp = TRUE;
        }
        else if (argc > optind && gStdinMode)
        {
            isDisplayHelp = TRUE;
        }

        if (isDisplayHelp)
        {
            displayUsage();
            exit(EXIT_NUM_SHOW_USAGE);
        }
    }

    //---- Input file name
    if (!gStdinMode)
    {
        strncpy(gInputFileNameString, argv[optind], FILE_NAME_MAX_SIZE);
    }

    if (gVerboseMode)
    {
        if (!gStdinMode)
        {
            printf("Input file is [%s]\n", gInputFileNameString);
        }
        else
        {
            printf("Input file is stdin\n");
        }
    }

    //---- display verbose information
    if (gVerboseMode)
    {
        printf("Print time out : %d sec.\n", gTimeOutTime);
        printf("Execute time out : %d sec.\n", gExecTimeOutTime);
        printf("Card lock : %s.\n", (gIsCardLocked) ? "ON" : "OFF");
        printf("Cartridge lock : %s.\n", (gIsCartridgeLocked) ? "ON" : "OFF");

        if (gAbortString)
        {
            printf("Abort string : [%s]\n", gAbortString);
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         loadFile

  Description:  load file

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
void loadFile(void)
{
    unsigned int address = 0;
    //int fileSize;

    //---- File open
    if (gStdinMode)
    {
        gInputFile = stdin;
        _setmode(_fileno(gInputFile), O_BINARY);
    }
    else
    {
        if ((gInputFile = fopen(gInputFileNameString, "rb")) == NULL)
        {
            displayErrorAndExit(EXIT_NUM_NO_INPUT_FILE, "cannot open input file.");
        }
    }
    gIsInputFileOpened = TRUE;

    //---- connected to device
    if ((gDeviceHandle = ISNTD_DeviceOpen(gDeviceList[gCurrentDevice].ntdId)) == NULL)
    {
        displayErrorAndExit(EXIT_NUM_NOT_CONNECT, "cannot connect device.");
    }
    gDeviceConnected = TRUE;

    //---- issue reset
    ISNTD_Reset(gDeviceHandle, TRUE);
    Sleep(1000);

    //---- Get file size
    //fseek( gInputFile, 0L, SEEK_END );
    //fileSize = ftell( gInputFile );
    //fseek( gInputFile, 0L, SEEK_SET );

    //---- Transfer every 16KB
    while (1)
    {
        char    buf[16384];
        size_t  size = fread(buf, 1, sizeof(buf), gInputFile);
        static int progressCount = 0;

        if (!size)
        {
            break;
        }

        //---- Transfer
        if (!ISNTD_WriteROM(gDeviceHandle, buf, address, size))
        {
            displayErrorAndExit(EXIT_NUM_LOADING_ERROR, "troubled while loading input file.");
        }

        address += size;

        if (gVerboseMode)
        {
            if (!(progressCount++ % 32))
            {
                printf("*");
            }
        }
    }

    //---- File close
    if (gStdinMode)
    {
        _setmode(_fileno(gInputFile), O_TEXT);
    }
    else
    {
        fclose(gInputFile);
    }
    gIsInputFileOpened = FALSE;

    if (gVerboseMode)
    {
        printf("\nInput file size: %d (0x%x) byte\n", address, address);
    }
}

/*---------------------------------------------------------------------------*
  Name:         setSlopPower

  Description:  Slot power process

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void setSlotPower(void)
{
    //---- Should the cartridge slot be locked?
    if (gIsCartridgeLocked)
    {
        if (!ISNTD_CartridgeSlotPower(gDeviceHandle, TRUE))
        {
            displayErrorAndExit(EXIT_NUM_CANNOT_USE_CARTRIDGE, "cannot use cartridge slot.");
        }
    }

    //---- Should the card slot be locked?
    if (gIsCardLocked)
    {
        if (!ISNTD_CardSlotPower(gDeviceHandle, TRUE))
        {
            displayErrorAndExit(EXIT_NUM_CANNOT_USE_CARD, "cannot use card slot.");
        }
    }

    //---- cancel reset
    Sleep(1000);
    ISNTD_Reset(gDeviceHandle, FALSE);
}

/*---------------------------------------------------------------------------*
  Name:         procPrint

  Description:  printf process

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
NITROArch archNum[] = {
    NITROArchARM9,
    NITROArchARM7
};

#define PRINT_ONETIME_SIZE  512
//#define PRINT_ONETIME_SIZE    16

//---- Buffer for joint display
static char gConbineBuf[PRINT_ONETIME_SIZE * 2 + 2] = "\0";
static char *gConbineBufPtr = &gConbineBuf[0];

//---- Buffer for joining and comparing character strings
static char gLineBuf[PRINT_ONETIME_SIZE + 1];


void procPrintf(void)
{
    int     blankTime = 0;

    //Length of exit string
    gExitStrLength = strlen(OS_EXIT_STRING_1);

    while (1)
    {
        BOOL    isOutputString = FALSE;

        //---- When stopped by the user
        if (gStoppedByUser)
        {
            myExit(EXIT_NUM_USER_SIGNAL);
        }

        //printf("[######]\n" );
        {
            int     n;
            int     dataSize;
            char    tmpBuf[PRINT_ONETIME_SIZE + 1];

            for (n = 0; n < 2; n++)
            {
                //---- Get display data
                if (!ISNTD_GetDebugPrint
                    (gDeviceHandle, archNum[n], tmpBuf, &dataSize, PRINT_ONETIME_SIZE))
                {
                    ISNTD_DeviceClose(gDeviceHandle);
                    displayErrorAndExit(EXIT_NUM_PRINTF_ERROR,
                                        "troubled while receiving print data.");
                }
                tmpBuf[dataSize] = '\0';

                //---- If there is data to be displayed, display it
                if (dataSize)
                {
                    //---- record that data was displayed
                    isOutputString = TRUE;

                    //---- standard output
                    if (!outputString(tmpBuf, dataSize))
                    {
                        break;
                    }
                }
            }
        }

        //---- Exit?
        if (gStringAborted || gExitAborted)
        {
            break;
        }

        //---- if text is not being displayed
        if (!isOutputString)
        {
            Sleep(100);
            blankTime += 100;

            //---- timeout decision
            if (gTimeOutTime && blankTime > gTimeOutTime * 1000)
            {
                gTimeOutOccured = TRUE;
                break;
            }
        }
        //---- if text has been displayed
        else
        {
            blankTime = 0;
        }

        //---- timeout check
        if (gExecTimeOutTime > 0)
        {
            time_t  currentTime;
            (void)time(&currentTime);

            if (currentTime - gStartTime >= gExecTimeOutTime)
            {
                gExecTimeOutOccured = TRUE;
                break;
            }
        }
    }
}


/*---------------------------------------------------------------------------*
  Name:         showLapTime

  Description:  display lap time at line head

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
void showLapTime(void)
{
    int     lap;
    time_t  currentTime;

    (void)time(&currentTime);
    lap = currentTime - gStartTime;

    printf("{%d:%02d}", lap / 60, lap % 60);
}

/*---------------------------------------------------------------------------*
  Name:         outputString

  Description:  output string to stdout

  Arguments:    buf : buffer
                bufSize : data size in buffer

  Returns:      FALSE if to do quit.
 *---------------------------------------------------------------------------*/
BOOL outputString(char *buf, int bufSize)
{
    char   *bufEnd = buf + bufSize;
    char   *p = buf;

    int     abortStrLength = gAbortString ? strlen(gAbortString) : 0;

    while (p < bufEnd)
    {
        char   *crPtr = strchr(p, '\n');

        //---- \n missing
        if (!crPtr)
        {
            //---- Save for comparison
            strcat(gConbineBufPtr, p);
            gConbineBufPtr += strlen(p);

            //---- time display
            if (!gQuietMode && gIsLineHead && gShowLapTime)
            {
                showLapTime();
            }
            gIsLineHead = FALSE;

            //---- display
            printfIfNotQuiet(p);

            //---- Destroy if buffer overflows (the responsibility is on the output side to insert \n)
            if (gConbineBufPtr - &gConbineBuf[0] > PRINT_ONETIME_SIZE)
            {
                gConbineBufPtr = &gConbineBuf[0];
                *gConbineBufPtr = '\0';
            }

            break;
        }

        //---- copy up to \n
        {
            int     n = crPtr - p + 1;

            //---- Combine for comparison
            strncpy(gConbineBufPtr, p, n);
            gConbineBufPtr[n] = '\0';

            //---- for display
            strncpy(&gLineBuf[0], p, n);
            gLineBuf[n] = '\0';
        }

        //---- time display
        if (!gQuietMode && gIsLineHead && gShowLapTime)
        {
            showLapTime();
        }
        gIsLineHead = TRUE;

        //---- line display
        printfIfNotQuiet(gLineBuf);

        //---- Compare with abort string
        if (gAbortString && !strncmp(gConbineBuf, gAbortString, abortStrLength))
        {
            gStringAborted = TRUE;
            return FALSE;
        }

        //---- Exit using OS_Exit
        if (!strncmp(gConbineBuf, OS_EXIT_STRING_1, gExitStrLength))
        {
            gExitAborted = TRUE;
            gExitStatusNum = atoi(gConbineBuf + gExitStrLength);
            return FALSE;
        }

        gConbineBufPtr = &gConbineBuf[0];
        *gConbineBufPtr = '\0';

        p = crPtr + 1;
    }

    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         signalHandler

  Description:  signal handler

  Arguments:    sig
                argv: argument vector

  Returns:      ---
 *---------------------------------------------------------------------------*/
void signalHandler(int sig)
{
    gStoppedByUser = TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         main

  Description:  main proc

  Arguments:    argc : argument count
                argv: argument vector

  Returns:      ---
 *---------------------------------------------------------------------------*/
int main(int argc, char *argv[])
{
    //---- initialize DLL
    ISNTD_InitDll();

    //---- Disable buffering of standard output
    setvbuf(stdout, NULL, _IONBF, 0);

    //---- Parse options
    parseOption(argc, argv);

    //Data read
    gConnectedDeviceNum = ISNTD_GetDeviceList(&gDeviceList[0], DEVICE_MAX_NUM);
    if (gConnectedDeviceNum < 0)
    {
        displayErrorAndExit(EXIT_NUM_NO_DEVICE, "Cannot access devices.");
    }

    //---- search for device
    searchDevice();

    //---- data read
    loadFile();

    //---- signal setting
    (void)signal(SIGINT, signalHandler);

    //---- slot
    setSlotPower();

    //---- get start time
    (void)time(&gStartTime);

    //---- printf process
    procPrintf();

    //---- End
    if (gExitAborted)                  //---- Exit from OS_Exit()
    {
        myExit(gExitStatusNum);
    }
    else if (gStringAborted)           //---- Abort string
    {
        myExit(EXIT_NUM_STRING_ABORT);
    }
    else if (gTimeOutOccured)          //---- timeout
    {
        myExit(EXIT_NUM_TIME_OUT);
    }
    else if (gExecTimeOutOccured)
    {
        myExit(EXIT_NUM_EXEC_TIME_OUT);
    }
    else                               //---- Normal end
    {
        myExit(EXIT_NUM_NO_ERROR);
    }
    //---- never reached here

    //---- dummy to avoid warning
    return 0;
}
