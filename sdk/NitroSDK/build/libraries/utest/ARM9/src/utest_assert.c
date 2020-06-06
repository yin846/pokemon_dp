/*---------------------------------------------------------------------------*
# Project:  NitroSDK - libraries - utest
  File:     utest_assert.c

  Copyright 2005.2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: utest_assert.c,v $
  Revision 1.2  01/18/2006 02:12:38  kitase_hirotake
  do-indent

  Revision 1.1  2005/06/24 02:19:25  yasu
  Added unit test

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nitro.h>
#include <nitro/utest.h>

/*---------------------------------------------------------------------------*
  Normal ASSERT
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
  Name:         UTi_Assert
  Description:  If exp_val value is non-zero, output NG message
  Arguments:    file :    __FILE__ Source file name
                line :    __LINE__ No. of source file lines
                exp_str: Message format to display
                exp_val: Value to display
  Returns:      None.
 *---------------------------------------------------------------------------*/
void UTi_Assert(const char *file, int line, const char *exp_str, int exp_val)
{
    // If val is 0, treat as NG
    if (!exp_val)
    {
        UTi_SetResult(UT_RESULT_NG);
        UTi_SetMessage(file, line, "UT_Assert: %s[=%d] is not TRUE\n", exp_str, exp_val);
    }

    // If val is non-zero, and it has not failed any tests so far, treat as OK.
    else if (UTi_GetResult() != UT_RESULT_NG)
    {
        UTi_SetResult(UT_RESULT_OK);
    }
    return;
}

/*---------------------------------------------------------------------------*
  Name:         UTi_AssertEq
  Description:  If the values of exp_val1and exp_val2 are not equal, output NG message
  Arguments:    file :    __FILE__ Source file name
                line :    __LINE__ No. of source file lines
                exp_str1: Text of value 1 to compare
                exp_val1: Value 1 to compare
                exp_str2: Text of value 2 to compare
                exp_val2: Value 2 to compare
  Returns:      None.
 *---------------------------------------------------------------------------*/
void UTi_AssertEq(const char *file, int line, const char *exp_str1, int exp_val1,
                  const char *exp_str2, int exp_val2)
{
    // If val1 and val2 are not equal, treat as NG
    if (exp_val1 != exp_val2)
    {
        UTi_SetResult(UT_RESULT_NG);
        UTi_SetMessage(file, line, "UT_AssertEq: %s[=%d] is not equal to %s[=%d]\n", exp_str1,
                       exp_val1, exp_str2, exp_val2);
    }

    // If val1 and val2 are equal, and they have not failed any tests so far, treat as OK
    else if (UTi_GetResult() != UT_RESULT_NG)
    {
        UTi_SetResult(UT_RESULT_OK);
    }
    return;
}


/*---------------------------------------------------------------------------*
  Name:         UTi_AssertNe
  Description:  If the values of exp_val1and exp_val2 are not equal, output NG message
  Arguments:    file :    __FILE__ Source file name
                line :    __LINE__ No. of source file lines
                exp_str1: Text of value 1 to compare
                exp_val1: Value 1 to compare
                exp_str2: Text of value 2 to compare
                exp_val2: Value 2 to compare
  Returns:      None.
 *---------------------------------------------------------------------------*/
void UTi_AssertNe(const char *file, int line, const char *exp_str1, int exp_val1,
                  const char *exp_str2, int exp_val2)
{
    // If val1 and val2 are equal, treat as NG
    if (exp_val1 == exp_val2)
    {
        UTi_SetResult(UT_RESULT_NG);
        UTi_SetMessage(file, line, "UT_AssertNe: %s[=%d] is equal to %s[=%d]\n", exp_str1, exp_val1,
                       exp_str2, exp_val2);
    }

    // If val1 and val2 are not equal, and they have not failed any tests so far, treat as OK
    else if (UTi_GetResult() != UT_RESULT_NG)
    {
        UTi_SetResult(UT_RESULT_OK);
    }
    return;
}


/*---------------------------------------------------------------------------*
  Name:         UTi_AssertMemEq
  
  Description:  If memory blocks exp_ptr1 and exp_ptr2, having length exp_len bytes,
                are not equal, outputs NG message,
  
  Arguments:    file :    __FILE__ Source file name
                line :    __LINE__ No. of source file lines
                exp_str1: Text expression to compare in block 1
                exp_ptr1: Block 1 to compare
                exp_str2: Text expression to compare in block 2
                exp_ptr2: Block 2 to compare
                exp_len : Size of blocks to compare
  Returns:      None.
 *---------------------------------------------------------------------------*/
void UTi_AssertMemEq(const char *file, int line,
                     const char *exp_str1, const void *exp_ptr1,
                     const char *exp_str2, const void *exp_ptr2, int exp_len)
{
    // If exp_ptr1 and exp_ptr2 are not equal for exp_len bytes of length, treat as NG
    if (UTi_MemCmp(exp_ptr1, exp_ptr2, exp_len))
    {
        UTi_SetResult(UT_RESULT_NG);
        UTi_SetMessage(file, line, "UT_AssertMemEq: %s is not equal to %s (size=%d)\n", exp_str1,
                       exp_str2, exp_len);
    }

    // If val1 and val2 are equal, and they have not failed any tests so far, treat as OK
    else if (UTi_GetResult() != UT_RESULT_NG)
    {
        UTi_SetResult(UT_RESULT_OK);
    }
    return;
}


/*---------------------------------------------------------------------------*
  Name:         UTi_AssertMemNe
  
  Description:  If memory blocks exp_ptr1 and exp_ptr2, having length exp_len bytes,
                are equal, outputs NG message,
  
  Arguments:    file :    __FILE__ Source file name
                line :    __LINE__ No. of source file lines
                exp_str1: Text expression to compare in block 1
                exp_ptr1: Block 1 to compare
                exp_str2: Text expression to compare in block 2
                exp_ptr2: Block 2 to compare
                exp_len : Size of blocks to compare
  Returns:      None.
 *---------------------------------------------------------------------------*/
void UTi_AssertMemNe(const char *file, int line,
                     const char *exp_str1, const void *exp_ptr1,
                     const char *exp_str2, const void *exp_ptr2, int exp_len)
{
    // If exp_ptr1 and exp_ptr2 are equal for exp_len bytes of length, treat as NG
    if (!UTi_MemCmp(exp_ptr1, exp_ptr2, exp_len))
    {
        UTi_SetResult(UT_RESULT_NG);
        UTi_SetMessage(file, line, "UT_AssertMemNe: %s is equal to %s (size=%d)\n", exp_str1,
                       exp_str2, exp_len);
    }

    // If val1 and val2 are not equal, and they have not failed any tests so far, treat as OK
    else if (UTi_GetResult() != UT_RESULT_NG)
    {
        UTi_SetResult(UT_RESULT_OK);
    }
    return;
}


/*---------------------------------------------------------------------------*
  ASSERT for assert functionality
 *---------------------------------------------------------------------------*/

OSContext UTi_AssertContext;

/*---------------------------------------------------------------------------*
  Name:         UTi_Terminate
  Description:  Replaces termination process. Returns to original state after ASSERT.
  Arguments:    None.
  Returns:      None.
 *---------------------------------------------------------------------------*/
#ifdef SDK_DEBUG
void UTi_Terminate(void)
{
    OS_LoadContext(&UTi_AssertContext);
}
#endif


/*---------------------------------------------------------------------------*
  Name:         UTi_AssertAsserted
  Description:  Outputs error message when Assert does not occur.
  Arguments:    file :    __FILE__ Source file name
                line :    __LINE__ No. of source file lines
                exp_str:  Message format to display
                asserted: If assert occurs, 1. Otherwise 0.
  Returns:      None.
 *---------------------------------------------------------------------------*/
void UTi_AssertAsserted(const char *file, int line, const char *exp_str, int asserted)
{
    // If assert does not occur, treat as NG
    if (!asserted)
    {
        UTi_SetResult(UT_RESULT_NG);
        UTi_SetMessage(file, line, "UT_AssertAsserted: %s not asserted\n", exp_str);
    }

    // If assert occurs, and it does not fail tests so far, treat as OK
    else if (UTi_GetResult() != UT_RESULT_NG)
    {
        UTi_SetResult(UT_RESULT_OK);
    }
    return;
}


/*---------------------------------------------------------------------------*
  Name:         UTi_AssertNotAsserted
  Description:  Outputs error message when Assert occurs.
  Arguments:    file :    __FILE__ Source file name
                line :    __LINE__ No. of source file lines
                exp_str:  Message format to display
                asserted: If assert occurs, 1. Otherwise 0.
  Returns:      None.
 *---------------------------------------------------------------------------*/
void UTi_AssertNotAsserted(const char *file, int line, const char *exp_str, int asserted)
{
    // If assert occurred, treat as NG
    if (asserted)
    {
        UTi_SetResult(UT_RESULT_NG);
        UTi_SetMessage(file, line, "UT_AssertNotAsserted: %s asserted\n", exp_str);
    }

    // If assert does not occur, and it does not fail tests so far, treat as OK
    else if (UTi_GetResult() != UT_RESULT_NG)
    {
        UTi_SetResult(UT_RESULT_OK);
    }
    return;
}


/*---------------------------------------------------------------------------*
  Name:         UTi_MemCmp
  
  Description:  Compares memory blocks ptr1 and ptr2 for len bytes
  
  Arguments:    ptr1: Block 1 to compare
                ptr2: Block 2 to compare
                len    : Size of blocks to compare
  
  Returns:      0    : They are equal
                non-zero: They are not equal
 *---------------------------------------------------------------------------*/
int UTi_MemCmp(const void *ptr1, const void *ptr2, int len)
{
    int     i;

    for (i = 0; i < len; i++)
    {
        if (((unsigned char *)ptr1)[i] != ((unsigned char *)ptr2)[i])
        {
            return 1;
        }
    }
    return 0;
}
