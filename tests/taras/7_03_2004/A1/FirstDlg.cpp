// FirstDlg.cpp : implementation file
//

#include "stdafx.h"
#include "A1.h"
#include "FirstDlg.h"


// CFirstDlg dialog

IMPLEMENT_DYNAMIC(CFirstDlg, CDialog)
CFirstDlg::CFirstDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFirstDlg::IDD, pParent)
{
}

CFirstDlg::~CFirstDlg()
{
}

void CFirstDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON1, B1);
}


BEGIN_MESSAGE_MAP(CFirstDlg, CDialog)
END_MESSAGE_MAP()


// CFirstDlg message handlers

BOOL CFirstDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	HRESULT res=bi.Load("first.jpg");
	B1.SetBitmap((HBITMAP)bi);
	RedrawWindow();
	return true;
}