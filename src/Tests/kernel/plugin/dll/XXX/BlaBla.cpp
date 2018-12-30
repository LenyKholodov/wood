// BlaBla.cpp : implementation file
//

#include "stdafx.h"
#include "XXX.h"
#include "BlaBla.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// BlaBla dialog


BlaBla::BlaBla(CWnd* pParent /*=NULL*/)
	: CDialog(BlaBla::IDD, pParent)
{
	//{{AFX_DATA_INIT(BlaBla)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void BlaBla::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(BlaBla)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(BlaBla, CDialog)
	//{{AFX_MSG_MAP(BlaBla)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// BlaBla message handlers

void BlaBla::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here

	MessageBox ("here!");
	
	// Do not call CDialog::OnPaint() for painting messages
}
