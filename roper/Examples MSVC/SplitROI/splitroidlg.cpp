// ImageDlg.cpp : implementation file
//

#include "stdafx.h"
#include "splitroi.h"
#include "splitroidlg.h"

/* PI Globals  */
HANDLE bhandle;
void   *lpvBuff;
int		buffer_flag = 0;
HWND   gwnd;
long   buf_size;
int NumberRois = 1;

/* PI Includes */
#include "platform.h"
extern "C" 
{
#include "splitwin.h"
#include "pigenfcn.h"
#include "pimltfcn.h"
#include "piimgfcn.h"
#include "pigendef.h"
#include "pistpdef.h"
#include "pistpfcn.h"
#include "pifcsfcn.h"
}
static Window *Displays[5] = {0,0,0,0,0};

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CImageDlg dialog

CImageDlg::CImageDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CImageDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CImageDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CImageDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CImageDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CImageDlg, CDialog)
	//{{AFX_MSG_MAP(CImageDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, OnCreate)
	ON_BN_CLICKED(IDC_BUTTON2, OnAcquire)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CImageDlg message handlers

BOOL CImageDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	CString strAboutMenu;
	strAboutMenu.LoadString(IDS_ABOUTBOX);
	if (!strAboutMenu.IsEmpty())
	{
		pSysMenu->AppendMenu(MF_SEPARATOR);
		pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CImageDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CImageDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CImageDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CImageDlg::OnCreate() 
{
	// TODO: Add your control notification handler code here
	/* Call Libraries */
	int ok;
	unsigned int error;
	int status;
	char controllerstring[] = "Princeton Instruments Acquistion";	
    int i;
	char Name[40];

	/* Create Controller Object */
	ok = PICM_CreateController( 	ST133,																
									EEV_1024x1024_CCD47_10,							
									ROM_FULL,	
 									APP_NORMAL_IMAGING,     /* Imaging    	*/
									&error );               /* Error      	*/

	/* Setup the interface card	*/ 

    if ( ok )
    {
         PICM_SetInterfaceCard( 	PCI_TIMER_Interface,			  /* Interface   */							
        							0xa00,					  /* BaseAddress */
        							CHANNEL_10,				  /* Interrupt   */
	        						&error ); 				  /* error code  */
	}
	
	/* Set Adc Speed fast or Slow */
	PICM_Set_Fast_ADC();
    
    
    /* Clear All Rois */
    PICM_Clear_MultStrip();

    /* Roi Number 1 */
    PICM_SetROI_MultiStrip(1,1,203,203, 1, 1, 1, &error);

    /* Roi Number 2 */
    PICM_SetROI_MultiStrip(251,251,400,400, 1, 1, 2, &error);

    /* Roi Number 3 */
    PICM_SetROI_MultiStrip(501,401,767,511, 1, 1, 3, &error);

    NumberRois = 3;

    /* Generate The Side Effects */
	PICM_Generate_sideeffects();   
    
    /* Set Exposure Time */
    PICM_SetExposure( 0.15 , &error );          /* Time , Error */ 
	PICM_Set_AutoStop(1);
	
    /* Allocate Memory and Initialize System */
    if ( ok ) 
    {   /* Obtain Size Based on Current ROI in Controller Object */
     	buf_size = PICM_SizeNeedToAllocate(); 
     	/* Allocate And Lock Memory */
     	bhandle  = GlobalAlloc( GPTR, buf_size );
     	lpvBuff  = (void huge*) GlobalLock( bhandle );  
		buffer_flag = 1;
		PICM_Set_controller_version( 4 );
     	status = PICM_Initialize_System( lpvBuff, &error ); 
        
		for ( i=0; i<NumberRois; i++)		
		{			   			
			/* Set up window name */
			sprintf( Name, "Roi Number %i ",i+1 );

			/* Create a new Display Class for current controller */
			Displays[i] = new Window( PICM_Get_pixeldimension_x(),
						 			  PICM_Get_pixeldimension_y(),
									  Name, 
                                      i+1);
		}
					
    }  


	
}

void CImageDlg::OnAcquire() 
{
	unsigned int status;
    int i;

	// TODO: Add your control notification handler code here
	// Start Controller 
	PICM_Start_controller();

	 //Wait For Data 
	 while ( !PICM_ChkData(&status) )
		;
	
    for ( i=0;i<NumberRois;i++)
	    // Show Data 
	    Displays[i]->UpdateDeviceIndependentBitmap((unsigned short *)lpvBuff );
    
    
    // Reset Buffer 
	PICM_ResetUserBuffer();
}

void CImageDlg::OnOK() 
{
	// TODO: Add extra validation here
	if ( buffer_flag )
	{
		GlobalUnlock( bhandle );
		GlobalFree(	  bhandle );
	}
	PICM_CleanUp();	
	CDialog::OnOK();
}