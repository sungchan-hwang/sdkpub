VERSION 4.00
Begin VB.Form Form1
  Caption = "DOsingleBufPatternGenExtClock653x"
  ClientHeight = 3195
  ClientLeft = 1140
  ClientTop = 1515
  ClientWidth = 6360
  Height = 3600
  Left = 1080
  LinkTopic = "Form1"
  ScaleHeight = 3195
  ScaleWidth = 6360
  Top = 1170
  Width = 6480
  Begin VB.CommandButton cmdExit
    Caption = "E&xit"
     Height = 615
     Left = 4680
     TabIndex = 2
     Top = 2520
     Width = 1575
  End
  Begin VB.CommandButton cmdDoOperation
     Caption = "&Do Operation"
     Height = 615
     Left = 2880
     TabIndex = 1
     Top = 2520
     Width = 1575
  End
  Begin VB.TextBox txtStatusBox
     Height = 2295
     Left = 120
     MultiLine = -1          'True
     ScrollBars = 2         'vertical
     TabIndex = 0
     Top = 120
     Width = 6135
  End
End
Attribute VB_Name = "Form1"
Attribute VB_Creatable = False
Attribute VB_Exposed = False
' ********************************************************************
' 
'  Example Program:
'     DOsingleBufPatternGenExtClock653x.FRM
' 
'  Description:
'     Outputs digital data in a buffer from a group of two digital ports
'      using external timing (or pattern generation)
' 
'  Example Category:
'     DO
' 
'  Example Task Types:
'     BUF, NCH, INTTIM, BURST, ASYNC
' 
'  List of key parameters:
'     iGroup, iGroupSize, iDir, iPgConfig, iReqSource, iPgTB, iReqInt,
'      ulCount, ulBufferSize, iResource, ulRemaining, ulAlignIndex
' 
'     [Since variables are hardcoded, there is no guarantee that this
'      program will work for your setup.  This example is simply
'      presented as a code snippet of how you can use NI-DAQ functions
'      to perform a task.]
' 
'  List of NI-DAQ Functions used in this example:
'     NIDAQMakeBuffer, DIG_Grp_Config, NIDAQErrorHandler,
'      DIG_Block_PG_Config, Align_DMA_Buffer, DIG_Block_Out,
'      DIG_Block_Check, NIDAQYield, DIG_Block_Clear
' 
'     [NOTE: For further details on each NI-DAQ function, please refer
'      to the NI-DAQ On-Line Help (NIDAQPC.HLP).]
' 
'  Pin Connection Information: 
'     The digital signals will be on ports 0 and 1. Connect the ground
'      reference to the DIG GND pin.
' 
'     [For further I/O connection details, please refer to your hardware
'      User Manual.]
' 
'     [For further details on how to run this example, please refer to
'      the NI-DAQ Examples On-Line Help (NIDAQEx.HLP).]
' 
' ********************************************************************
Option Explicit
Option Base 0
' 
' Constant for PrintText 
' 
Const LEN_PRINTTEXT = 4096 


' ************************************************************************
' SUBROUTINE:  PrintText
' DESCRIPTION: PrintText to desired TextBox (upto 4096 characters)
' INPUTS:      txtBox - TextBox to print on
'              strText - Text to print
' ************************************************************************
Sub PrintText(txtBox as TextBox, strText as string)

    txtBox.Text = Right$(txtBox.Text + strText$ + Chr$(13) + Chr$(10), LEN_PRINTTEXT)

    txtBox.SelStart = Len(CStr(txtBox.Text))

    DoEvents

End Sub


' ************************************************************************
' SUBROUTINE:  cmdExit_Click
' DESCRIPTION: Clean up and exit
' ************************************************************************
Sub cmdExit_Click()

    End

End Sub

' ************************************************************************
' SUBROUTINE:  Form_Load
' DESCRIPTION: Gets automatically called at startup
' ************************************************************************
Sub Form_Load()


End Sub


' ************************************************************************
' SUBROUTINE:  cmdDoOperation_Click
' DESCRIPTION: The main NI-DAQ operations are here
' ************************************************************************
Sub cmdDoOperation_Click()

    ' 
    '  Local Variable Declarations: 
    

    Dim iStatus As Integer
    Dim iRetVal As Integer
    Dim iDevice As Integer
    Dim iGroup As Integer
    Dim iGroupSize As Integer
    Dim iPort As Integer
    Dim iDir As Integer
    Dim iPgConfig As Integer
    Dim iReqSource As Integer
    Dim iPgTB As Integer
    Dim iReqInt As Integer
    Dim iExtGate As Integer
    Static piBuffer(200) As Integer
    Dim ulCount As Long
    Dim ulBufferSize As Long
    Dim iResource As Integer
    Dim ulRemaining As Long
    Dim ulAlignIndex As Long
    Dim iIgnoreWarning As Integer
    Dim iYieldON As Integer
    Dim i As Long

    iDevice% = 1
    iGroup% = 1
    iGroupSize% = 2
    iDir% = 1
    iPgConfig% = 1
    iReqSource% = 1
    iPgTB% = 3
    iReqInt% = 10
    ulCount& = 100
    ulBufferSize& = 200
    iResource% = 11
    ulRemaining& = 1
    iYieldON% = 1

    ' Temporarily disable buttons for protection from 'DoEvents'
    cmdDoOperation.Enabled = False
    cmdExit.Enabled = False

    iStatus% = NIDAQMakeBuffer(piBuffer%(0), ulCount&, WFM_DATA_I16)

    If (iStatus% =  0) Then

'         Configure group of ports as output, with handshaking.

        iStatus% = DIG_Grp_Config(iDevice%, iGroup%, iGroupSize%, iPort%, iDir%)

        iRetVal% = NIDAQErrorHandler(iStatus%, "DIG_Grp_Config", iIgnoreWarning%)

'         Configure externally timed pattern generation.

        iStatus% = DIG_Block_PG_Config(iDevice%, iGroup%, iPgConfig%, iReqSource%, iPgTB%, iReqInt%, iExtGate%)

        iRetVal% = NIDAQErrorHandler(iStatus%, "DIG_Block_PG_Config", iIgnoreWarning%)

'         Align the DMA buffer so that it does not cross a page boundary
'          for AT bus computers. (It is a good idea to keep your buffers
'          smaller than 4kBytes in size.) NOTE: If you change 'iGroup'
'          or 'iGroupSize', make sure 'iResource' is changed
'          accordingly.

        iStatus% = Align_DMA_Buffer(iDevice%, iResource%, piBuffer%(0), ulCount&, ulBufferSize&, ulAlignIndex&)

        iRetVal% = NIDAQErrorHandler(iStatus%, "Align_DMA_Buffer", iIgnoreWarning%)

'         Start the pattern generation output of 100 "items".

        Call PrintText(txtStatusBox, "Apply your handshaking signals to the appropriate handshaking I/O pins. ")

        iStatus% = DIG_Block_Out(iDevice%, iGroup%, piBuffer%(0), ulCount&)

        iRetVal% = NIDAQErrorHandler(iStatus%, "DIG_Block_Out", iIgnoreWarning%)

        While ((ulRemaining& <>  0) And (iStatus% =  0))

            iStatus% = DIG_Block_Check(iDevice%, iGroup%, ulRemaining&)

            DoEvents

        Wend

        iRetVal% = NIDAQErrorHandler(iStatus%, "DIG_Block_Check", iIgnoreWarning%)

'         CLEANUP - Don't check for errors on purpose.

'         Clear the block operation.

        iStatus% = DIG_Block_Clear(iDevice%, iGroup%)

'         Unconfigure group.

        iStatus% = DIG_Grp_Config(iDevice%, iGroup%, 0, 0, 0)

        Call PrintText(txtStatusBox, "Digital pattern generation output is done! ")

    Else

        Call PrintText(txtStatusBox, "The buffer was not made correctly. " + "Check the parameters for NIDAQMakeBuffer. ")

    End If


    ' Re-enable buttons
    cmdDoOperation.Enabled = True
    cmdExit.Enabled = True

End Sub


