/*---------------------------------------------------------------------------*
  Project:  NitroSDK - libraries - OS
  File:     os_printf.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: os_printf.c,v $
  Revision 1.48  01/18/2006 02:11:30  kitase_hirotake
  do-indent

  Revision 1.47  10/12/2005 08:32:38  yada
  OS_PutString() is deadstripped in case of SDK_FINALROM

  Revision 1.46  2005/06/23 12:08:37  yasu
  Changed to not use SDK_LITTLE_ENDIAN

  Revision 1.45  2005/06/07 09:49:34  yasu
  As a UnitTest measure, made changeable the calling of OS_Terminate in OSi_Panic

  Revision 1.44  2005/03/04 11:09:27  yasu
  Changed __declspec(weak) to SDK_WEAK_SYMBOL

  Revision 1.43  02/28/2005 05:26:29  yosizaki
  do-indent.

  Revision 1.42  02/28/2005 04:26:25  yosizaki
  Minor revisions related to SDK_NO_MESSAGE

  Revision 1.41  2005/02/18 06:22:52  yasu
  Signed/Unsigned conversion warning control

  Revision 1.40  2005/02/04 11:02:15  yasu
  Revised the copyright date

  Revision 1.39  01/31/2005 03:16:15  seiki_masashi
  For division elimination, specified that speed is always the priority for optimization

  Revision 1.38  01/28/2005 12:58:25  seiki_masashi
  Changed things so OS_VSNPrintf uses division as little as possible

  Revision 1.37  11/24/2004 11:07:37  yada
  Let a few functions be weak symbol.

  Revision 1.36  11/24/2004 08:50:22  yada
  Let a few functions be weak symbol.

  Revision 1.35  11/12/2004 10:45:30  yada
  just fix comment

  Revision 1.34  10/08/2004 07:49:13  yada
  consider for the situation of not calling OS_InitPrintServer()

  Revision 1.33  2004/10/04 10:06:30  yasu
  support SDK_NO_MESSAGE

  Revision 1.32  2004/09/11 06:00:34  yasu
  Add OS_TPrintf

  Revision 1.31  06/08/2004 01:22:47  yada
  add stub OS_Printf() for FINALROM

  Revision 1.30  06/08/2004 00:31:31  yada
  invalidate debug functions(OS_Printf, etc.) when FINALROM

  Revision 1.29  05/06/2004 02:54:39  yada
  stop by OS_Terminate() in OSi_Panic(), not by OS_Halt()

  Revision 1.28  04/08/2004 02:47:05  takano_makoto
  Change to use OS_VSNPrintf() in ARM7.

  Revision 1.27  03/26/2004 06:52:15  yosizaki
  add OS_SPrintf, OS_VSPrintf, OS_SNPrintf, and OS_VSNPrintf.

  Revision 1.26  2004/03/05 09:41:48  yasu
  add SDK_NO_VSNPRINTF to disable vsnprintf

  Revision 1.25  2004/03/04 12:41:05  yasu
  change function typedef name

  Revision 1.24  2004/02/28 12:07:55  yasu
  fix buffer handling

  Revision 1.23  2004/02/28 09:47:10  yasu
  fix comment

  Revision 1.22  2004/02/18 01:09:26  yasu
  force to turn byte access warning off

  Revision 1.21  2004/02/16 11:43:42  yasu
  fix OS_PrintServer

  Revision 1.20  2004/02/14 09:40:59  yasu
  workaround for unused warning message

  Revision 1.19  2004/02/14 09:26:08  yasu
  move printserver into os_printf

  Revision 1.18  2004/02/14 07:07:24  yasu
  support printserver component

  Revision 1.17  2004/02/13 08:58:52  yasu
  nitro_sp.h -> nitro.h

  Revision 1.16  02/13/2004 01:50:10  yada
  Sorting include files accordingly for ARM9 and AMR7

  Revision 1.15  02/05/2004 08:38:44  yada
  only modify comment

  Revision 1.14  2004/02/05 07:09:02  yasu
  change SDK prefix iris -> nitro

  Revision 1.13  2004/01/26 09:05:05  yasu
  Link stub of ISDPrintf if no IS_IRIS_DIR

  Revision 1.12  2003/12/29 04:27:24  yasu
  workaround for OS_Warning, OS_Panic

  Revision 1.11  12/25/2003 07:31:26  yada
  Changes as per unified type rules

  Revision 1.10  2003/12/17 08:21:56  yasu
  Made the code to be shared with ARM7

  Revision 1.9  2003/12/16 10:46:16  yasu
  Made the code to be shared with ARM9/7

  Revision 1.8  2003/12/12 05:03:48  yasu
  Added a workaround to use until version that supports vsprintf strb is released.

  Revision 1.7  2003/12/10 12:20:50  yasu
  Minor IS library related changes

  Revision 1.6  2003/12/10 10:57:08  yasu
  Embedding ISDPrint
  Enable output to TEG board

  Revision 1.5  2003/12/02 07:28:59  yasu
  warning avoidance

  Revision 1.4  2003/12/02 06:51:51  yasu
  Changed blank lines

  Revision 1.3  2003/12/02 03:44:50  yasu
  Rev. 2 for IS-IRIS-EMULATOR library

  Revision 1.2  2003/12/02 02:38:49  yasu
  First step of embedding 'printout' into ISDebugger

  Revision 1.1  11/29/2003 01:25:46  yada
  File name changed

  Revision 1.8  2003/11/25 11:17:37  yasu
  Add emulator automatic discriminate

  Revision 1.7  2003/11/25 00:03:44  yasu
  Release of OS_PutChar/OS_PutString/OS_VPrintf

  Revision 1.6  11/21/2003 12:20:31  yada
  reg_EMU_CONSOLE_OUT changed to reg_OS_EMU_CONSOLE_OUT

  Revision 1.5  2003/11/14 08:07:46  yasu
  Stop taking string buffer from stack. Make it static. 

  Revision 1.4  2003/11/14 07:46:00  yasu
  Minor revisions

  Revision 1.3  2003/11/14 06:14:03  yasu
  Added OS_Warning() OS_Panic()

  Revision 1.2  2003/11/13 11:59:01  yasu
  Created initial version

  Revision 1.1  2003/11/13 11:24:08  yasu
  Created Initial version. Probably won't work because emulator implementation not finished.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include        <nitro.h>

#ifdef          SDK_LINK_ISD
# pragma warn_extracomma off
# include       <isdbglib.h>           // has extracomma in enum
# pragma warn_extracomma reset
#else
void    ISDPrint(const char *);
#endif

#ifndef SDK_FINALROM
static char common_buffer[256];        // thread unsafe, but less use of stack
#endif

#if !defined(SDK_FINALROM) && defined(SDK_NO_MESSAGE)
#undef OSi_Warning
#undef OSi_TWarning
#undef OSi_Panic
#undef OSi_TPanic
void    OSi_Warning(const char *file, int line, const char *fmt, ...);
void    OSi_TWarning(const char *file, int line, const char *fmt, ...);
void    OSi_Panic(const char *file, int line, const char *fmt, ...);
void    OSi_TPanic(const char *file, int line, const char *fmt, ...);
#endif /* SDK_NO_MESSAGE*/


/*---------------------------------------------------------------------------*
  Name:         OS_PutChar

  Description:  put a letter for debug console

  Arguments:    c : char code , shuild be 0x01-0xff

  Returns:      None.
 *---------------------------------------------------------------------------*/
#ifndef SDK_FINALROM
SDK_WEAK_SYMBOL void OS_PutChar(char c)
{
    char    str[2];

    str[0] = c;
    str[1] = '\0';

    // Because of no putchar-type function on ISDebugger library,
    // use 'putstring' for a single letter outputting
    OS_PutString(str);
}
#endif

/*---------------------------------------------------------------------------*
  Name:         OS_PutString

  Description:  put a string to debug console.
                console port are automatically switched depends on emu/hw

  Arguments:    str     : string

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void OS_PutStringInit(const char *str);

#ifndef SDK_FINALROM
static void OS_PutStringAris(const char *str);
static void OS_PutStringISD(const char *str);
#ifdef  SDK_ARM7
static void OS_PutStringPrnSrv(const char *str);
#endif
#endif

#ifndef SDK_FINALROM
void    (*OS_PutString) (const char *str) = OS_PutStringInit;
#endif


/*---------------------------------------------------------------------------*
  Name:         OS_PutStringInit
  Description:  OS_PutString initializer
 *---------------------------------------------------------------------------*/
static void OS_PutStringInit(const char *str)
{
#ifndef SDK_FINALROM
    OSIntrMode intr = OS_DisableInterrupts();

#ifdef  SDK_ARM9
    if (OS_IsRunOnEmulator())
    {
        OS_PutString = OS_PutStringAris;
    }
    else
#endif
    {
#ifdef  SDK_ARM7
        OS_PutString = OS_PutStringPrnSrv;
#else
        OS_PutString = OS_PutStringISD;
#endif
    }

    OS_PutString(str);

    (void)OS_RestoreInterrupts(intr);

#else
    (void)str;                         // avoiding to unused warning
#endif
}


/*---------------------------------------------------------------------------*
  Name:         OS_PutStringAris
  Description:  OS_PutString for software simulator called ARIS
 *---------------------------------------------------------------------------*/
#ifndef SDK_FINALROM
#ifdef  SDK_CW_WARNOFF_SAFESTRB
#include <nitro/code32.h>
#endif
static void OS_PutStringAris(const char *str)
{
    char    c;

    while ('\0' != (c = *str))
    {
        reg_OS_EMU_CONSOLE_OUT = (u8)c; // Console out
        str++;
    }
}

#ifdef  SDK_CW_WARNOFF_SAFESTRB
#include <nitro/codereset.h>
#endif
#endif


/*---------------------------------------------------------------------------*
  Name:         OS_PutStringISD
  Description:  OS_PutString for hardware emulator called IS Debugger
 *---------------------------------------------------------------------------*/
#ifndef SDK_FINALROM
static void OS_PutStringISD(const char *str)
{
    // ISDPrint initialize process
    OS_InitLock();

    OS_PutString = ISDPrint;
    OS_PutString(str);
}
#endif


/*---------------------------------------------------------------------------*
  Name:         OS_VPrintf/OS_TVPrintf

  Description:  print formatted strings (vprintf ver.)

  Arguments:    fmt   : format string
                vlist : parameters

  Returns:      None.
 *---------------------------------------------------------------------------*/
// If you want to use "vsnprintf()" in ARM7, define SDK_USE_VSNPRINTF.
// "vsnprintf()" is able to print float format but the code size become hugely bigger.
// #define SDK_USE_VSNPRINTF

#ifndef SDK_FINALROM
#if     defined(SDK_ARM9) || defined(SDK_USE_VSNPRINTF)
SDK_WEAK_SYMBOL void OS_VPrintf(const char *fmt, va_list vlist)
{
    (void)vsnprintf(common_buffer, sizeof(common_buffer), fmt, vlist);
    OS_PutString(common_buffer);
}
#else
SDK_WEAK_SYMBOL void OS_VPrintf(const char *fmt, va_list vlist)
{
    OS_TVPrintf(fmt, vlist);
}
#endif
SDK_WEAK_SYMBOL void OS_TVPrintf(const char *fmt, va_list vlist)
{
    (void)OS_VSNPrintf(common_buffer, sizeof(common_buffer), fmt, vlist);
    OS_PutString(common_buffer);
}
#endif

/*---------------------------------------------------------------------------*
  Name:         OS_Printf/OS_TPrintf

  Description:  print formatted strings.

  Arguments:    fmt   : format string
                ...   and parameters

  Returns:      None.
 *---------------------------------------------------------------------------*/
#ifndef SDK_FINALROM
SDK_WEAK_SYMBOL void OS_Printf(const char *fmt, ...)
{
    va_list vlist;

    va_start(vlist, fmt);
    OS_VPrintf(fmt, vlist);
    va_end(vlist);
}

SDK_WEAK_SYMBOL void OS_TPrintf(const char *fmt, ...)
{
    va_list vlist;

    va_start(vlist, fmt);
    OS_TVPrintf(fmt, vlist);
    va_end(vlist);
}
#endif

/*---------------------------------------------------------------------------*
  Name:         OSi_Warning / OSi_TWarning

  Description:  print warning message.

  Arguments:    file : filename    of warning location
                line : line number of warning location
                fmt   : format string
                ...   and parameters

  Returns:      None.
 *---------------------------------------------------------------------------*/
#ifndef SDK_FINALROM
SDK_WEAK_SYMBOL void OSi_Warning(const char *file, int line, const char *fmt, ...)
{
    va_list vlist;

    va_start(vlist, fmt);
    OS_Printf("%s:%d Warning:", file, line);
    OS_VPrintf(fmt, vlist);
    OS_Printf("\n");
    va_end(vlist);
}


SDK_WEAK_SYMBOL void OSi_TWarning(const char *file, int line, const char *fmt, ...)
{
    va_list vlist;

    va_start(vlist, fmt);
    OS_TPrintf("%s:%d Warning:", file, line);
    OS_TVPrintf(fmt, vlist);
    OS_TPrintf("\n");
    va_end(vlist);
}
#endif

/*---------------------------------------------------------------------------*
  Name:         OSi_Panic/OSi_TPanic

  Description:  print panic message and halt cpu.

  Arguments:    file : filename    of panic location
                line : line number of panic location
                fmt   : format string
                ...   and parameters

  Returns:      None.
 *---------------------------------------------------------------------------*/
#ifndef SDK_FINALROM
void    (*OSi_FuncTerminate) (void) = OS_Terminate;

SDK_WEAK_SYMBOL void OSi_Panic(const char *file, int line, const char *fmt, ...)
{
    va_list vlist;

    va_start(vlist, fmt);
    (void)OS_DisableInterrupts();
    OS_Printf("%s:%d Panic:", file, line);
    OS_VPrintf(fmt, vlist);
    OS_Printf("\n");
    OSi_FuncTerminate();               // Never Returns
}

SDK_WEAK_SYMBOL void OSi_TPanic(const char *file, int line, const char *fmt, ...)
{
    va_list vlist;

    va_start(vlist, fmt);
    (void)OS_DisableInterrupts();
    OS_TPrintf("%s:%d Panic:", file, line);
    OS_TVPrintf(fmt, vlist);
    OS_TPrintf("\n");
    OSi_FuncTerminate();               // Never Returns
}
#endif

/*---------------------------------------------------------------------------*
  Name:         OS_InitPrintServer

  Description:  initialize print server
                - this code should be called before OS_InitLock()
                - OS_InitLock() are also called from OS_Init()

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
#ifdef   SDK_ARM9
#ifndef  SDK_FINALROM
void OS_InitPrintServer(void)
{
    OSPrintServerBuffer *p;

    // first, allocate buffer and register it.
    OS_InitArena();
    p = OS_AllocFromSharedArenaHi(sizeof(OSPrintServerBuffer), 4);
    p->in = p->out = 0UL;
    PXI_SetComponentParam((u32)p);
}
#endif //SDK_FINALROM
#endif //SDK_ARM9


/*---------------------------------------------------------------------------*
  Name:         OS_PrintServer

  Description:  print a string comes from ARM7 via shared memory

  Arguments:    str     string

  Returns:      None.
 *---------------------------------------------------------------------------*/
#ifdef   SDK_ARM9
#ifndef  SDK_FINALROM
#include <nitro/code32.h>              // to access 'common_buffer' via cache
void OS_PrintServer(void)
{
    OSPrintServerBuffer *p;
    register OSPrintWChar word;

    u32     in, out;
    int     i;

    p = (OSPrintServerBuffer *)PXI_GetComponentParam();

    //---- If print buffer isn't set up, do nothing.
    if (!p)
    {
        return;
    }

    out = p->out;
    in = p->in;

    while (in != out)
    {
        i = 0;

        while (in != out && i < sizeof(common_buffer) - 3)
        {
            word.s = p->buffer[out].s;
            if (word.c[0])
            {
                common_buffer[i++] = word.c[0]; // store via cache

                if (word.c[1])
                {
                    common_buffer[i++] = word.c[1];     // store via cache
                }
            }
            out++;
            if (out >= OS_PRINTSRV_WCHARSIZE)
                out = 0;
        }
        common_buffer[i] = '\0';       // store via cache
        OS_PutString(common_buffer);
    }

    // tell finished
    p->out = out;
}

#include <nitro/codereset.h>
#endif //SDK_FINALROM
#endif //SDK_ARM9


/*---------------------------------------------------------------------------*
  Name:         OS_PutStringPrnSrv

  Description:  sends string to ARM9 via shared memory

  Arguments:    str     string to be sent

  Returns:      None
 *---------------------------------------------------------------------------*/
#ifdef  SDK_ARM7
static void OS_PutStringPrnSrv(const char *str)
{
#ifndef  SDK_FINALROM
    OSPrintServerBuffer *p;
    register OSPrintWChar word;

    u32     in, in_tmp, out;
    u32     isOdd;

    p = (OSPrintServerBuffer *)PXI_GetComponentParam();
    in = p->in;
    out = p->out;
    isOdd = ((u32)str) & 1;

    while (1)
    {
        in_tmp = in + 1;
        if (in_tmp >= OS_PRINTSRV_WCHARSIZE)
            in_tmp = 0;
        if (out == in_tmp)
            break;                     // Buffer full, then exit

        if (isOdd)
        {
            p->buffer[in].s = word.s = (u16)((*(u16 *)(str - 1)) & 0xff00);
            str++;
            isOdd = 0UL;
            if (!word.c[1])
                break;
            in = in_tmp;
        }
        else
        {
            p->buffer[in].s = word.s = *(u16 *)str;
            str += 2;
            if (!word.c[0])
                break;
            in = in_tmp;
            if (!word.c[1])
                break;
        }
    }
    p->in = in;
#else  //SDK_FINALROM
    (void)str;
#endif //SDK_FINALROM
}
#endif //SDK_ARM7


#if defined(SDK_CW_WARNOFF_SAFESTRB)
#include <nitro/code32.h>
#endif

/* inner function for sized-buffer output*/
typedef struct dst_string_tag
{
    size_t  len;
    char   *cur;
    char   *base;
}
dst_string;

static void string_put_char(dst_string * p, char c)
{
    if (p->len > 0)
        *p->cur = c, --p->len;
    ++p->cur;
}

static void string_fill_char(dst_string * p, char c, int n)
{
    if (n > 0)
    {
        size_t  i, k = p->len;
        if (k > (size_t) n)
            k = (size_t) n;
        for (i = 0; i < k; ++i)
            p->cur[i] = c;
        p->len -= k;
        p->cur += n;
    }
}

static void string_put_string(dst_string * p, const char *s, int n)
{
    if (n > 0)
    {
        size_t  i, k = p->len;
        if (k > (size_t) n)
            k = (size_t) n;
        for (i = 0; i < k; ++i)
            p->cur[i] = s[i];
        p->len -= k;
        p->cur += n;
    }
}

/*---------------------------------------------------------------------------*
  Name:         OS_SPrintf

  Description:  equal to 'OS_VSPrintf' except argument style.

  Arguments:    dst   : destination buffer.
                fmt   : format string.

  Returns:      length of the generated string.
 *---------------------------------------------------------------------------*/
SDK_WEAK_SYMBOL int OS_SPrintf(char *dst, const char *fmt, ...)
{
    int     ret;
    va_list va;
    va_start(va, fmt);
    ret = OS_VSPrintf(dst, fmt, va);
    va_end(va);
    return ret;
}

/*---------------------------------------------------------------------------*
  Name:         OS_VSPrintf

  Description:  equal to 'OS_VSNPrintf' except buffer size argument.

  Arguments:    dst   : destination buffer.
                fmt   : format string.
                vlist : parameters.

  Returns:      length of the generated string.
 *---------------------------------------------------------------------------*/
SDK_WEAK_SYMBOL int OS_VSPrintf(char *dst, const char *fmt, va_list vlist)
{
    return OS_VSNPrintf(dst, 0x7FFFFFFF, fmt, vlist);
}

/*---------------------------------------------------------------------------*
  Name:         OS_SNPrintf

  Description:  equal to 'OS_VSNPrintf' except argument style.

  Arguments:    dst   : destination buffer.
                len    : destination buffer size.
                fmt   : format string.
 
  Returns:      length of the generated string. (except '\0')
                if(result < len),
                  put NUL in dst[result].
                else if(len > 0),
                  put NUL in dst[len - 1].
                else,
                  do nothing.
 *---------------------------------------------------------------------------*/
SDK_WEAK_SYMBOL int OS_SNPrintf(char *dst, size_t len, const char *fmt, ...)
{
    int     ret;
    va_list va;
    va_start(va, fmt);
    ret = OS_VSNPrintf(dst, len, fmt, va);
    va_end(va);
    return ret;
}

/*---------------------------------------------------------------------------*
  Name:         OS_VSNPrintf

  Description:  small-size vsnprintf which is similar to 'vsnprintf'
                without following supports.
                * CodeWarrior Extensions (#s)
                * MSL AltiVec Extensions (v, vh, vl, hv, lv, @)
                * indexed arguments (%m$, *m$)
                * floating-point
                * wchar_t

  Note:         '+' and '#' do not work, MSL's sprintf().
                to keep same result, they are no implement.
                { // exsample
                  char buf[5];
                  sprintf(buf, "%-i\n", 45);  // "45"  (OK)
                  sprintf(buf, "%0i\n", 45);  // "45"  (OK)
                  sprintf(buf, "% i\n", 45);  // " 45" (OK)
                  sprintf(buf, "%+i\n", 45);  // "%+i" ("+45" expected)
                  sprintf(buf, "%#x\n", 45);  // "%#x" ("0x2d" expected)
                  // but, this works correctly!
                  sprintf(buf, "% +i\n", 45); // "+45" (OK)
                }

  Arguments:    dst   : destination buffer.
                len    : destination buffer size.
                fmt   : format string.
                vlist : parameters.

  Returns:      length of the generated string. (except '\0')
                if(result < len),
                  put NUL in dst[result].
                else if(len > 0),
                  put NUL in dst[len - 1].
                else,
                  do nothing.
 *---------------------------------------------------------------------------*/
SDK_WEAK_SYMBOL int OS_VSNPrintf(char *dst, size_t len, const char *fmt, va_list vlist)
{
    char    buf[24];
    int     n_buf;
    char    prefix[2];
    int     n_prefix;

    const char *s = fmt;

    dst_string str;
    str.len = len, str.cur = str.base = dst;

    while (*s)
    {
        if ((unsigned int)(((unsigned char)*s ^ 0x20) - 0xA1) < 0x3C)
        {
            /* Shift JIS character*/
            string_put_char(&str, *s++);
            if (*s)
                string_put_char(&str, *s++);
        }
        else if (*s != '%')
        {
            /* normal ASCII character*/
            string_put_char(&str, *s++);
        }
        else
        {
            /* output with format*/
            enum
            {
                flag_blank = 000001,   /* ' ' */
                flag_plus = 000002,    /* '+' */
                flag_sharp = 000004,   /* '#' */
                flag_minus = 000010,   /* '-' */
                flag_zero = 000020,    /* '0' */
                flag_l1 = 000040,      /* "l"*/
                flag_h1 = 000100,      /* "h"*/
                flag_l2 = 000200,      /* "ll"*/
                flag_h2 = 000400,      /* "hh"*/
                flag_unsigned = 010000, /* 'o', 'u', ...*/
                flag_end
            };
            int     flag = 0, width = 0, precision = -1, radix = 10;
            char    hex_char = 'a' - 10;
            const char *p_start = s;
            /* flags*/
            for (;;)
            {
                switch (*++s)
                {
                case '+':
                    if (s[-1] != ' ')
                        break;
                    flag |= flag_plus;
                    continue;
                case ' ':
                    flag |= flag_blank;
                    continue;
                case '-':
                    flag |= flag_minus;
                    continue;
                case '0':
                    flag |= flag_zero;
                    continue;
                }
                break;
            }
            /* width*/
            if (*s == '*')
            {
                ++s, width = va_arg(vlist, int);
                if (width < 0)
                    width = -width, flag |= flag_minus;
            }
            else
            {
                while ((*s >= '0') && (*s <= '9'))
                    width = (width * 10) + *s++ - '0';
            }
            /* precision*/
            if (*s == '.')
            {
                ++s, precision = 0;
                if (*s == '*')
                {
                    ++s, precision = va_arg(vlist, int);
                    if (precision < 0)
                        precision = -1;
                }
                else
                {
                    while ((*s >= '0') && (*s <= '9'))
                        precision = (precision * 10) + *s++ - '0';
                }
            }
            /* option*/
            switch (*s)
            {
            case 'h':
                if (*++s != 'h')
                    flag |= flag_h1;
                else
                    ++s, flag |= flag_h2;
                break;
            case 'l':
                if (*++s != 'l')
                    flag |= flag_l1;
                else
                    ++s, flag |= flag_l2;
                break;
            }

            /* type*/
            switch (*s)
            {
            case 'd':                 /* signed decimal*/
            case 'i':                 /* signed decimal*/
                goto put_integer;
            case 'o':                 /* unsigned octal*/
                radix = 8;
                flag |= flag_unsigned;
                goto put_integer;
            case 'u':                 /* unsigned decimal*/
                flag |= flag_unsigned;
                goto put_integer;
            case 'X':                 /* unsigned hexadecimal*/
                hex_char = 'A' - 10;
                goto put_hexadecimal;
            case 'x':                 /* unsigned hexadecimal*/
                goto put_hexadecimal;
            case 'p':                 /* pointer*/
                /* equal to code warrior*/
                flag |= flag_sharp;
                precision = 8;
                goto put_hexadecimal;

            case 'c':                 /* character*/
                if (precision >= 0)
                    goto put_invalid;
                {
                    int     c = va_arg(vlist, int);
                    width -= 1;
                    if (flag & flag_minus)
                    {
                        string_put_char(&str, (char)c);
                        string_fill_char(&str, ' ', width);
                    }
                    else
                    {
                        char    pad = (char)((flag & flag_zero) ? '0' : ' ');
                        string_fill_char(&str, pad, width);
                        string_put_char(&str, (char)c);
                    }
                    ++s;
                }
                break;

            case 's':                 /* string*/
                {
                    int     n_buf = 0;
                    const char *p_buf = va_arg(vlist, const char *);
                    if (precision < 0)
                    {
                        while (p_buf[n_buf])
                            ++n_buf;
                    }
                    else
                    {
                        while ((n_buf < precision) && p_buf[n_buf])
                            ++n_buf;
                    }
                    width -= n_buf;
                    if (flag & flag_minus)
                    {
                        string_put_string(&str, p_buf, n_buf);
                        string_fill_char(&str, ' ', width);
                    }
                    else
                    {
                        char    pad = (char)((flag & flag_zero) ? '0' : ' ');
                        string_fill_char(&str, pad, width);
                        string_put_string(&str, p_buf, n_buf);
                    }
                    ++s;
                }
                break;

            case 'n':                 /* store the number of output*/
                {
                    int     pos = str.cur - str.base;
                    if (flag & flag_h2)
                        ;
                    else if (flag & flag_h1)
                        *va_arg(vlist, signed short *) = (signed short)pos;
                    else if (flag & flag_l2)
                        *va_arg(vlist, u64 *) = (u64)pos;
                    else
                        *va_arg(vlist, signed int *) = (signed int)pos;
                }
                ++s;
                break;

            case '%':                 /* output '%'*/
                if (p_start + 1 != s)
                    goto put_invalid;
                string_put_char(&str, *s++);
                break;

            default:                  /* invalid type*/
                goto put_invalid;

              put_invalid:
                string_put_string(&str, p_start, s - p_start);
                break;

              put_hexadecimal:
                radix = 16;
                flag |= flag_unsigned;
              put_integer:
                {
                    u64     val = 0;
                    n_prefix = 0;

                    if (flag & flag_minus)
                        flag &= ~flag_zero;
                    if (precision < 0)
                        precision = 1;
                    else
                        flag &= ~flag_zero;

                    if (flag & flag_unsigned)
                    {
                        if (flag & flag_h2)
                            val = va_arg(vlist, unsigned char);
                        else if (flag & flag_h1)
                            val = va_arg(vlist, unsigned short);
                        else if (flag & flag_l2)
                            val = va_arg(vlist, u64);
                        else
                            val = va_arg(vlist, unsigned long);
                        flag &= ~(flag_plus | flag_blank);
                        if (flag & flag_sharp)
                        {
                            if (radix == 16)
                            {
                                if (val != 0)
                                {
                                    prefix[0] = (char)(hex_char + (10 + 'x' - 'a'));
                                    prefix[1] = '0';
                                    n_prefix = 2;
                                }
                            }
                            else if (radix == 8)
                            {
                                prefix[0] = '0';
                                n_prefix = 1;
                            }
                        }
                    }
                    else
                    {
                        if (flag & flag_h2)
                            val = va_arg(vlist, char);
                        else if (flag & flag_h1)
                            val = va_arg(vlist, short);
                        else if (flag & flag_l2)
                            val = va_arg(vlist, u64);
                        else
                            val = va_arg(vlist, long);
                        if ((val >> 32) & 0x80000000)
                        {
                            val = ~val + 1;
                            prefix[0] = '-';
                            n_prefix = 1;
                        }
                        else
                        {
                            if (val || precision)
                            {
                                if (flag & flag_plus)
                                {
                                    prefix[0] = '+';
                                    n_prefix = 1;
                                }
                                else if (flag & flag_blank)
                                {
                                    prefix[0] = ' ';
                                    n_prefix = 1;
                                }
                            }
                        }
                    }
                    n_buf = 0;
                    switch (radix)
                    {
                    case 8:
                        while (val != 0)
                        {
                            int     d = (int)(val & 0x07);
                            val >>= 3;
                            buf[n_buf++] = (char)(d + '0');
                        }
                        break;
                    case 10:
                        if ((val >> 32) == 0)
                        {
#if defined(SDK_CW) || defined(__MWERKS__)
#pragma optimize_for_size off
#endif
                            u32     v = (u32)val;
                            while (v != 0)
                            {
                                // If division involves u32 and constant, the compiler automatically
                                // coverts to addition using magic number
                                u32     r = v / 10;
                                int     d = (int)(v - (r * 10));
                                v = r;
                                buf[n_buf++] = (char)(d + '0');
                            }
                        }
                        else
                        {
                            while (val != 0)
                            {
                                u64     r = val / 10;
                                int     d = (int)(val - (r * 10));
                                val = r;
                                buf[n_buf++] = (char)(d + '0');
                            }
                        }
                        break;
                    case 16:
                        while (val != 0)
                        {
                            int     d = (int)(val & 0x0f);
                            val >>= 4;
                            buf[n_buf++] = (char)((d < 10) ? (d + '0') : (d + hex_char));
                        }
                        break;
                    }
                    if ((n_prefix > 0) && (prefix[0] == '0'))
                    {
                        n_prefix = 0;
                        buf[n_buf++] = '0';
                    }
                }
                goto put_to_stream;

              put_to_stream:
                {
                    int     n_pad = precision - n_buf;
                    if (flag & flag_zero)
                    {
                        if (n_pad < width - n_buf - n_prefix)
                            n_pad = width - n_buf - n_prefix;
                    }
                    if (n_pad > 0)
                        width -= n_pad;
                    width -= n_prefix + n_buf;
                    if (!(flag & flag_minus))
                        string_fill_char(&str, ' ', width);
                    while (n_prefix > 0)
                        string_put_char(&str, prefix[--n_prefix]);
                    string_fill_char(&str, '0', n_pad);
                    while (n_buf > 0)
                        string_put_char(&str, buf[--n_buf]);
                    if (flag & flag_minus)
                        string_fill_char(&str, ' ', width);
                    ++s;
                }
                break;
            }
        }
    }

    if (str.len > 0)
        *str.cur = '\0';
    else if (len > 0)
        str.base[len - 1] = '\0';
    return str.cur - str.base;
}

#if defined(SDK_CW_WARNOFF_SAFESTRB)
#include <nitro/codereset.h>
#endif



//================================================================================
//              DUMMY PRINT (stub for FINALROM)
//================================================================================
#ifdef SDK_FINALROM
#ifdef OS_Printf
#undef OS_Printf
#endif
void    OS_Printf(const char *fmt, ...);
void OS_Printf(const char *fmt, ...)
{
#pragma unused( fmt )
}
#endif
