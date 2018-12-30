// bkl.cpp : implementation file
//

#include "stdafx.h"
#include "XXX.h"
#include "bkl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// bkl dialog


bkl::bkl(CWnd* pParent /*=NULL*/)
	: CDialog(bkl::IDD, pParent)
{
	//{{AFX_DATA_INIT(bkl)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void bkl::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(bkl)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(bkl, CDialog)
	//{{AFX_MSG_MAP(bkl)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// bkl message handlers

void bkl::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	
	// Do not call CDialog::OnPaint() for painting messages
}
