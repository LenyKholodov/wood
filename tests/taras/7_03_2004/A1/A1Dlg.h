// A1Dlg.h : header file
//

#pragma once

#include <atlimage.h>
#include "afxwin.h"
#include "afxcmn.h"
#include "PartDlg.h"
#include "FirstDlg.h"

void ReadString(FILE *f,char *String);

// CA1Dlg dialog
class CA1Dlg : public CDialog
{
// Construction
public:
	CA1Dlg(CWnd* pParent = NULL);	// standard constructor
	CImage Image;
	CImage ColorImage;
	bool il;
	CPoint p;
	COLORREF Color;
	COLORREF ColorRefs[12];
	int w;
	int h;
	char PictFile[256];

	bool fl;

	int n;
	char Names[1024][255];
	char ColorFiles[1024][255];
	char NoncolorFiles[1024][255];
	
	// Dialog Data
	enum { IDD = IDD_A1_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTtnGetDispInfoCustom1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnLButtonDown(UINT nFlags,CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedButton2();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	CButton ColorButton;
	CButton ExitButton;
	CListBox PicturesList;
	CButton Colors[12];
	CBitmap ColorBitmaps[12];
	afx_msg void OnLbnSelchangeList1();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedButton5();
	afx_msg void OnBnClickedButton6();
	afx_msg void OnBnClickedButton7();
	afx_msg void OnBnClickedButton8();
	afx_msg void OnBnClickedButton9();
	afx_msg void OnBnClickedButton10();
	afx_msg void OnBnClickedButton11();
	afx_msg void OnBnClickedButton12();
	afx_msg void OnBnClickedButton13();
	afx_msg void OnBnClickedButton14();
	afx_msg void OnLbnDblclkList1();
};
