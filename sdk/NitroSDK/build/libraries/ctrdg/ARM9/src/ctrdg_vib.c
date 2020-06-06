/*---------------------------------------------------------------------------*
  Project:  NitroSDK - build - libraries - ctrdg
  File:     ctrdg_vib.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include <nitro.h>

/*-----------------------------------------------------------------------*
                    Type, Constant
 *-----------------------------------------------------------------------*/

#define VIBi_INTR_DELAY_TICK    (19)

#define VIBi_BITID              (2)

typedef struct
{
    u32     current_pos;                /* Current position in pulse set*/
    u32     rest_pos;                   /* Position that is halt time*/
    u32     rest_tick;                  /* Length of halt time. 1 = 1 Tick*/
    u32     on_tick[VIB_PULSE_NUM_MAX]; /* Length of start time. 1 = 1 Tick*/
    u32     off_tick[VIB_PULSE_NUM_MAX];/* Length of stop time. 1 = 1 Tick*/
    BOOL    is_enable;                  /* If oscillating, TRUE.*/
    u32     repeat_num;                 /* Number of times to repeat pulse set. When 0, repeats endlessly. */
    u32     current_count;              /* Indicates the number of times that pulse set was repeated. */
    VIBCartridgePulloutCallback cartridge_pullout_callback;     /* Cartridge removal callback*/
}
VIBiPulseInfo;

/*-----------------------------------------------------------------------*
                    Function prototypes
 *-----------------------------------------------------------------------*/

//--- Auto Function Prototype --- Don't comment here.
BOOL    VIB_Init(void);
void    VIB_End(void);
void    VIB_StartPulse(const VIBPulseState * state);
void    VIB_StopPulse(void);
BOOL    VIB_IsExecuting(void);
void    VIB_SetCartridgePulloutCallback(VIBCartridgePulloutCallback func);
BOOL    VIB_IsCartridgeEnabled(void);
static inline u32 VIBi_PulseTimeToTicks(u32 pulse_time);
static BOOL VIBi_PulledOutCallbackCartridge(void);
static void VIBi_MotorOnOff(VIBiPulseInfo * pulse_vib);
static void VIBi_SleepCallback(void *);
//--- End of Auto Function Prototype

/*-----------------------------------------------------------------------*
                    Variables
 *-----------------------------------------------------------------------*/

/* Cache is accessed in 32-byte units, so align the top.*/
static VIBiPulseInfo pulse_vib ATTRIBUTE_ALIGN(32);
static PMSleepCallbackInfo sc_info;

/*-----------------------------------------------------------------------*
                    Global Function Definitions
 *-----------------------------------------------------------------------*/

/*!
    Initializes pulse rumble. \n
    When called in duplicate, it is equivalent to VIB_IsCartridgeEnabled.
    
    Inside this function, before entering sleep mode using NitroSDK's PM_AppendPreSleepCallback function,
    the callback that stops the rumble is registered.
    
    @retval TRUE    Initialization was successful. 
    @retval TRUE   Initialization failed.
*/
BOOL VIB_Init(void)
{

    static BOOL is_initialized;

    if (is_initialized)
    {
        return VIB_IsCartridgeEnabled();
    }
    is_initialized = TRUE;

    if (CTRDGi_IsBitIDAtInit(VIBi_BITID))
    {
        MI_CpuClearFast(&pulse_vib, sizeof(pulse_vib));
        CTRDG_SetPulledOutCallback(VIBi_PulledOutCallbackCartridge);

        /* Register the callback used before entering sleep.*/
        PM_SetSleepCallbackInfo(&sc_info, VIBi_SleepCallback, NULL);
        PM_AppendPreSleepCallback(&sc_info);

        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*!
    Stop usage of the Rumble Pak library.
    
    Stop the pulse rumble,
    delete the pre-sleep callback registered with the VIB_Init function,
    and delete the cartridge-removal callback.
*/
void VIB_End(void)
{

    VIB_StopPulse();
    PM_DeletePreSleepCallback(&sc_info);
    CTRDG_SetPulledOutCallback(NULL);
}

/*!
    Start pulse rumble. \n
    If the previous pulse rumble has not been completed, stop it and then start. \n
    Because the status is copied on the library side, there is no need to allocate memory.
    
    Check the hardware restrictions before starting rumble.
    If it violates the restrictions and this is a debug or release build, display a message using the OS_Panic function,
    and halt the program. If this is a final ROM build, pulse rumble is not started.
    
    @sa VIBPulseState
    
    @param state   Pulse rumble status
*/
void VIB_StartPulse(const VIBPulseState * state)
{

    if (!VIB_IsCartridgeEnabled())
        return;

    {
        int     i;

        /* Does the ON time exceed VIB_ON_TIME_MAX?*/
        for (i = 0; i < state->pulse_num; i++)
        {
            if (state->on_time[i] > VIB_ON_TIME_MAX)
            {
#ifdef SDK_FINALROM
                return;
#else
                VIB_StopPulse();
                OS_Panic("pulse_vib: on_time[%d] is over VIB_ON_TIME_MAX.\n", i);
#endif
            }
        }
        /* Does the OFF time exceed the previous ON time?*/
        for (i = 0; i < state->pulse_num - 1; i++)
        {
            if (state->on_time[i] > state->off_time[i])
            {
#ifdef SDK_FINALROM
                return;
#else
                VIB_StopPulse();
                OS_Panic("pulse_vib: on_time[%d] is over off_time[%d].\n", i, i);
#endif
            }
        }
        /* Is the REST time less than VIB_REST_TIME_MIN?*/
        if (state->rest_time < VIB_REST_TIME_MIN)
        {
#ifdef SDK_FINALROM
            return;
#else
            VIB_StopPulse();
            OS_Panic("pulse_vib: rest_time is less than VIB_REST_TIME_MIN.\n", i);
#endif
        }
    }

    VIB_StopPulse();

    pulse_vib.rest_tick = VIBi_PulseTimeToTicks(state->rest_time) - VIBi_INTR_DELAY_TICK;
    pulse_vib.repeat_num = state->repeat_num;
    pulse_vib.current_count = 0;

    pulse_vib.current_pos = 0;

    {
        u32     i;

        for (i = 0; i < VIB_PULSE_NUM_MAX; i++)
        {
            pulse_vib.on_tick[i] = VIBi_PulseTimeToTicks(state->on_time[i]) - VIBi_INTR_DELAY_TICK;
            pulse_vib.off_tick[i] =
                VIBi_PulseTimeToTicks(state->off_time[i]) - VIBi_INTR_DELAY_TICK;
        }
    }
    pulse_vib.rest_pos = state->pulse_num * 2 - 1;

    pulse_vib.is_enable = TRUE;
    /* Send a pulse_vib structure pointer*/
    VIBi_MotorOnOff(&pulse_vib);
}

/*!
    Stops pulse rumble.
*/
void VIB_StopPulse(void)
{

    if (pulse_vib.is_enable)
    {

        pulse_vib.is_enable = FALSE;
        /* Send a pulse_vib structure pointer*/
        VIBi_MotorOnOff(&pulse_vib);
    }
}

/*!
    Returns whether pulse rumble has completed. The pulse rumble is considered completed at the point when the last rest_time has finished.
    
    @retval TRUE    Pulse rumble has not completed.
    @retval FALSE    Pulse rumble has completed.
*/
BOOL VIB_IsExecuting(void)
{

    return pulse_vib.is_enable;
}

/*!
    Register the Game Pak removal callback.
    
    When Game Pak removal occurs, the library immediately stops the pulse rumble. @n
    If the callback is registered using this function, the callback is called afterwards.
    
    @param func Game Pak removal callback
*/
void VIB_SetCartridgePulloutCallback(VIBCartridgePulloutCallback func)
{

    pulse_vib.cartridge_pullout_callback = func;
}

/*!
    Returns whether the Rumble Pak is enabled.
    (If Pak has been removed once, the function does not return TRUE.
    
    @retval TRUE    The Rumble Pak is enabled.
    @retval TRUE    The Rumble Pak is not enabled.
*/
BOOL VIB_IsCartridgeEnabled(void)
{

    return CTRDG_IsBitID(VIBi_BITID);
}

/*-----------------------------------------------------------------------*
                    Local Function Definition
 *-----------------------------------------------------------------------*/

static inline u32 VIBi_PulseTimeToTicks(u32 pulse_time)
{

    return ((OS_SYSTEM_CLOCK / 1000) * (pulse_time)) / 64 / 10;
}

static BOOL VIBi_PulledOutCallbackCartridge(void)
{

    VIB_StopPulse();
    if (pulse_vib.cartridge_pullout_callback)
    {
        pulse_vib.cartridge_pullout_callback();
    }

    return FALSE;                      /* Do not stop the software right away.*/
}

/*!
    Turn vibration on and off.
    
    pulse_vib->is_enable
        @li TRUE     Turn on vibration.
        @li FALSE    Turn off vibration.
*/
static void VIBi_MotorOnOff(VIBiPulseInfo * pulse_vib)
{
    /* Flush the shared memory that has been set*/
    DC_FlushRange(pulse_vib, sizeof(VIBiPulseInfo));
    if (pulse_vib->is_enable == TRUE)
    {
        CTRDG_SendToARM7(pulse_vib);
    }
    else
    {
        CTRDG_SendToARM7(NULL);
    }
}

static void VIBi_SleepCallback(void *)
{

    VIB_StopPulse();
}
