#pragma once

#include "A1Dlg.h"
#include "afxwin.h"

class CA1Dlg;

// CPartDlg dialog

class CPartDlg : public CDialog
{
	DECLARE_DYNAMIC(CPartDlg)
public:
	CA1Dlg *pd;
	int n;
	CImage PImages[6];
	char CfgFiles[6][256];
	BOOL OnInitDialog();

public:
	CPartDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPartDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	CButton PartButtons[6];
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton5();
	afx_msg void OnBnClickedButton6();
};
