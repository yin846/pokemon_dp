/*---------------------------------------------------------------------------*
# Project:  NitroSDK - libraries - utest
  File:     utest_main.c

  Copyright 2005.2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: utest_main.c,v $
  Revision 1.2  01/18/2006 02:12:38  kitase_hirotake
  do-indent

  Revision 1.1  2005/06/24 02:19:25  yasu
  Added unit test

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nitro.h>
#include <nitro/utest.h>

UTiResult UTi_Result;

/*---------------------------------------------------------------------------*
  Name:         UTi_Main

  Description:  Executes UnitTest module in order and outputs result.

  Arguments:    num_module Number of modules
                modules    Module

  Returns:      None.
 *---------------------------------------------------------------------------*/
void UTi_Main(int num_module, UTiModule * modules)
{
    int     i;
    int     result = 0;
    UTiModule *t = modules;

    for (i = 0; i < num_module; i++)
    {
        UTi_Printf("%s\n", t->testname);

        // Clear setting values
        UTi_SetResult(UT_RESULT_UNTESTED);

        // Clear message buffer
        UTi_ClearMessage();

        // Common processing during start
        if (t->callback_begin)
        {
            t->callback_begin();
        }

        // Call test
        if (t->callback)
        {
            t->callback();
        }

        // Common processing during termination
        if (t->callback_end)
        {
            t->callback_end();
        }

        // Determine result
        switch (UTi_GetResult())
        {
        case UT_RESULT_OK:
            UTi_PutString("--->OK\n\n");
            break;

        case UT_RESULT_NG:
            UTi_PutString(UTi_GetMessage());
            UTi_PutString("--->NG\n\n");
            result = 1;
            break;

        case UT_RESULT_UNTESTED:
            UTi_PutString("--->??\n\n");
            break;

        default:
            OS_Panic("System Error!!");
            break;
        }

        t++;
    }
    UTi_Printf("%c%c", -1, result + '0');
}
