/*********************************************************************
*
* Example program: 
*    DIsingleBufPatternGenExtClock653x.c
*
* Description: 
*    Inputs digital data into a buffer from a group of two digital
*     ports using external timing (or pattern generation, for DIO-32
*     series devices)
*
* Example Category: 
*    DI
*
* Example Task Types: 
*    BUF, NCH, EXTTIM, BURST, ASYNC
*
* List of key parameters: 
*    iGroup, iGroupSize, iDir, iPgConfig, iReqSource, iPgTB, iReqInt,
*     ulCount, ulAlignIndex, iResource, ulRemaining
*
*    [Since variables are hardcoded, there is no guarantee that this
*     program will work for your setup.  This example is simply
*     presented as a code snippet of how you can use NI-DAQ functions
*     to perform a task.]
*
* List of NI-DAQ Functions used in this example: 
*    DIG_Grp_Config, NIDAQErrorHandler, DIG_Block_PG_Config,
*     Align_DMA_Buffer, DIG_Block_In, DIG_Block_Check, NIDAQYield,
*     DIG_Block_Clear, NIDAQPlotWaveform
*
*    [NOTE: For further details on each NI-DAQ function, please refer
*     to the NI-DAQ On-Line Help (NIDAQPC.HLP).]
*
* Pin Connection Information: 
*    Connect your digital signals to ports 0 and 1. Connect the ground
*     reference to the DIG GND pin.
*
*    [For further I/O connection details, please refer to your hardware
*     User Manual.]
*
*    [For further details on how to run this example, please refer to
*     the NI-DAQ Examples On-Line Help (NIDAQEx.HLP).]
*
*********************************************************************/
/*
 * Includes: 
 */

#include "nidaqex.h"


/*
 * Main: 
 */

void main(void)
{
    /*
     * Local Variable Declarations: 
     */

    i16 iStatus = 0;
    i16 iRetVal = 0;
    i16 iDevice = 1;
    i16 iGroup = 1;
    i16 iGroupSize = 2;
    i16 iPort = 0;
    i16 iDir = 0;
    i16 iPgConfig = 1;
    i16 iReqSource = 1;
    i16 iPgTB = 3;
    i16 iReqInt = 10;
    i16 iExtGate = 0;
    static i16 piBuffer[200] = {0};
    u32 ulCount = 100;
    u32 ulBufferSize = 200;
    u32 ulAlignIndex = 0;
    i16 iResource = 11;
    u32 ulRemaining = 1;
    i16 iIgnoreWarning = 0;
    i16 iYieldON = 1;

    /* Configure group of ports as input, with handshaking. */

    iStatus = DIG_Grp_Config(iDevice, iGroup, iGroupSize, iPort,
     iDir);

    iRetVal = NIDAQErrorHandler(iStatus, "DIG_Grp_Config",
     iIgnoreWarning);

    /* Configure internally timed pattern generation with timebase 3,
     interval 10, and no external gating. */

    iStatus = DIG_Block_PG_Config(iDevice, iGroup, iPgConfig,
     iReqSource, iPgTB, iReqInt, iExtGate);

    iRetVal = NIDAQErrorHandler(iStatus, "DIG_Block_PG_Config",
     iIgnoreWarning);

    /* Align the DMA buffer so that it does not cross a page boundary
     for AT bus computers. (It is a good idea to keep your buffers smaller
     than 4kBytes in size.) NOTE: If you change 'iGroup' or 'iGroupSize',
     make sure 'iResource' is changed accordingly. */
    

    iStatus = Align_DMA_Buffer(iDevice, iResource, piBuffer, ulCount,
     ulBufferSize, &ulAlignIndex);

    iRetVal = NIDAQErrorHandler(iStatus, "Align_DMA_Buffer",
     iIgnoreWarning);

    /* Start the pattern generation input of 100 "items". */

    iStatus = DIG_Block_In(iDevice, iGroup, piBuffer, ulCount);

    iRetVal = NIDAQErrorHandler(iStatus, "DIG_Block_In",
     iIgnoreWarning);

    printf(" Apply the external clock to the REQ pin on I/O connector .\n");

    while ((ulRemaining != 0) && (iStatus == 0)) {


        iStatus = DIG_Block_Check(iDevice, iGroup, &ulRemaining);
        

        iRetVal = NIDAQYield(iYieldON);

    }


    iRetVal = NIDAQErrorHandler(iStatus, "DIG_Block_Check",
     iIgnoreWarning);

    /* CLEANUP - Don't check for errors on purpose. */

    /* Clear the block operation. */

    iStatus = DIG_Block_Clear(iDevice, iGroup);

    /* Unconfigure group. */

    iStatus = DIG_Grp_Config(iDevice, iGroup, 0, 0, 0);

    iStatus = NIDAQPlotWaveform(piBuffer, ulCount, WFM_DATA_I16);

    printf(" The data is available in 'piBuffer'.\n");

    if (iStatus == 0) {


        printf(" Digital pattern generation input is done!\n");

    }



}

/* End of program */
