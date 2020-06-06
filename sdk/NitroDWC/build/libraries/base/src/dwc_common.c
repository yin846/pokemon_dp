#include <string.h>
#include <nitro.h>

#include <base/dwc_common.h>


//----------------------------------------------------------------------------
// define
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
// variable
//----------------------------------------------------------------------------
static MATHRandContext32 stRandContext32 = { 0, 0, 0 };  // Random number context


//----------------------------------------------------------------------------
// prototype
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
// function - external
//----------------------------------------------------------------------------
/*---------------------------------------------------------------------------*
  Function for creating common key/value type character strings
  Arguments: key    key character string to be set
          value    value character string to be set
          string    Pointer to head of storage for key/value type character string
          separator    Character used for separating character strings
  Return value: Length of the key/value character string that is set (not including NULL terminator)
  Use: Designates key/value character string, and creates DWC common key/value character string.
 *---------------------------------------------------------------------------*/
int  DWC_SetCommonKeyValueString(const char* key, const char* value, char* string, char separator)
{

    SDK_ASSERT(key && value);

    OS_SNPrintf(string, DWC_COMMONSTR_MAX_KEY_VALUE_LEN,
                "%c%s%c%s",
                separator, key, separator, value);

    return (int)strlen(string);
}


/*---------------------------------------------------------------------------*
  Function for adding common key / value type character string
  Arguments: key    key character string to be set
          value    value character string to be set
          string    Pointer to head of storage for key/value type character string
          separator    Character used for separating character strings
  Return value: Length of the key/value character string after a key/value character string has been added (not including NULL terminator)
  Use: Adds a key/value character string to an existing character string.
 *---------------------------------------------------------------------------*/
int  DWC_AddCommonKeyValueString(const char* key, const char* value, char* string, char separator)
{

    (void)DWC_SetCommonKeyValueString(key, value, strchr(string, '\0'), separator);

    return (int)strlen(string);
}


/*---------------------------------------------------------------------------*
  Function for acquiring the value of a common key/value type character string
  Arguments: key  key character string to be extracted
          value  Pointer to the head of storage for the extracted value character string
          string  key/value type character string
          separator    Character used for separating character strings
  Return value: Length of the value character string (not including NULL terminator) Returns -1 if non existent key is designated
  Use: From DWC common key/value type character string that is separated using the designated separator,
          obtains the value character string that corresponds to the designated key character string.
 *---------------------------------------------------------------------------*/
int  DWC_GetCommonValueString(const char* key, char* value, const char* string, char separator)
{
    const char* pSrcBegin;
    char* pSrcEnd;
    int len;

    SDK_ASSERT(key);

    if (!value) return -1;

    pSrcBegin = strchr(string, separator);  // Gets the start pointer to the key/value string
    if (!pSrcBegin) return -1;

    while (1){
        if (!strncmp(pSrcBegin+1, key, strlen(key)) &&
            (pSrcBegin[strlen(key)+1] == separator)){
            break;  // key discovered
        }

        // Search for next key.
        pSrcBegin = strchr(pSrcBegin+1, separator);
        if (!pSrcBegin) return -1;
        pSrcBegin = strchr(pSrcBegin+1, separator);
        if (!pSrcBegin) return -1;
    }

    // Gets the start pointer to value
    pSrcBegin = strchr(pSrcBegin+1, separator);
    if (!pSrcBegin) return -1;
    pSrcBegin++;

    // Gets the string length of value
    pSrcEnd = strchr(pSrcBegin, separator);
    if (pSrcEnd) len = (int)(pSrcEnd-pSrcBegin);
    else len = (int)strlen(pSrcBegin);

    strncpy(value, pSrcBegin, (u32)len);  // Copies the value string
    value[len] = '\0';

    return len;
}


//----------------------------------------------------------------------------
// function - internal
//----------------------------------------------------------------------------
/*---------------------------------------------------------------------------*
  Function for getting a 32-bit random number
  Argument : Maximum value for the random number to be gotten Here, the maximum value for the random number to be obtained is max-1.
              When 0 has been specified, a number from any range can be obtained.
  Return value: 32-bit random number
  Use : Gets a 32-bit random number generated by the function MATH_Rand32()
 *---------------------------------------------------------------------------*/
u32  DWCi_GetMathRand32(u32 max)
{
    u64 seed;

    if (!stRandContext32.x && !stRandContext32.mul && !stRandContext32.add){
        // Initializes when the random context is not initialized
        OS_GetMacAddress((u8 *)&seed);
        seed = ((seed >> 24) & 0xffffff) | (OS_GetTick() << 24);
        MATH_InitRand32(&stRandContext32, seed);
    }

    return MATH_Rand32(&stRandContext32, max);
}


/*---------------------------------------------------------------------------*
  Gets the string length of a wide character string.
  argument: str wide character string
  Return value: character count
 *---------------------------------------------------------------------------*/
u32 DWCi_WStrLen( const u16* str )
{
    u32 length = 0;

    while ( str[length] != 0 )
    {
        length++;
    }

    return length;
}


//----------------------------------------------------------------------------
// function - static
//----------------------------------------------------------------------------
