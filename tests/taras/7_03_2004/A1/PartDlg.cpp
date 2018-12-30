// PartDlg.cpp : implementation file
//

#include "stdafx.h"
#include "A1.h"
#include "PartDlg.h"
#include ".\partdlg.h"


// CPartDlg dialog

IMPLEMENT_DYNAMIC(CPartDlg, CDialog)
CPartDlg::CPartDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPartDlg::IDD, pParent)
{
}

CPartDlg::~CPartDlg()
{
}

void CPartDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON1, PartButtons[0]);
	DDX_Control(pDX, IDC_BUTTON2, PartButtons[1]);
	DDX_Control(pDX, IDC_BUTTON3, PartButtons[2]);
	DDX_Control(pDX, IDC_BUTTON4, PartButtons[3]);
	DDX_Control(pDX, IDC_BUTTON5, PartButtons[4]);
	DDX_Control(pDX, IDC_BUTTON6, PartButtons[5]);
}


BEGIN_MESSAGE_MAP(CPartDlg, CDialog)
	ON_WM_ACTIVATE()
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON4, OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON2, OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON5, OnBnClickedButton5)
	ON_BN_CLICKED(IDC_BUTTON6, OnBnClickedButton6)
END_MESSAGE_MAP()


// CPartDlg message handlers

void CPartDlg::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	CDialog::OnActivate(nState, pWndOther, bMinimized);
}

BOOL CPartDlg::OnInitDialog()
{
	CDialog::OnInitDialog();


	char fn[256];
	FILE *f=fopen("Parts.cfg","r");
	if(f!=NULL)
	{
		n=0;
		while(!feof(f))
		{
			ReadString(f,fn);
			ReadString(f,CfgFiles[n]);
			PImages[n].Load(fn);
			PartButtons[n].EnableWindow();
			PartButtons[n].SetBitmap((HBITMAP)PImages[n]);
			n++;
		}
		fclose(f);
	}
	else
		return false;
	return true;
}

void CPartDlg::OnBnClickedButton1()
{
	strcpy(pd->PictFile,CfgFiles[0]);
	SendMessage(WM_CLOSE);
}

void CPartDlg::OnBnClickedButton2()
{
	strcpy(pd->PictFile,CfgFiles[1]);
	SendMessage(WM_CLOSE);
}

void CPartDlg::OnBnClickedButton3()
{
	strcpy(pd->PictFile,CfgFiles[2]);
	SendMessage(WM_CLOSE);
}

void CPartDlg::OnBnClickedButton4()
{
	strcpy(pd->PictFile,CfgFiles[3]);
	SendMessage(WM_CLOSE);
}

void CPartDlg::OnBnClickedButton5()
{
	strcpy(pd->PictFile,CfgFiles[4]);
	SendMessage(WM_CLOSE);
}

void CPartDlg::OnBnClickedButton6()
{
	strcpy(pd->PictFile,CfgFiles[5]);
	SendMessage(WM_CLOSE);
}
