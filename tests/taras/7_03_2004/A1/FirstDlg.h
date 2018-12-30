#pragma once
#include "afxwin.h"
#include <atlimage.h>

// CFirstDlg dialog

class CFirstDlg : public CDialog
{
	DECLARE_DYNAMIC(CFirstDlg)

public:
	CFirstDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CFirstDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG2 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	 CImage bi;
	CButton B1;
	BOOL OnInitDialog();
};
