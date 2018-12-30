#if !defined(AFX_BLABLA_H__BE1AE16F_9FD5_46AA_A1A5_DD009F9F90F8__INCLUDED_)
#define AFX_BLABLA_H__BE1AE16F_9FD5_46AA_A1A5_DD009F9F90F8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BlaBla.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// BlaBla dialog

class BlaBla : public CDialog
{
// Construction
public:
	BlaBla(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(BlaBla)
	enum { IDD = IDD_DIALOG1 };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(BlaBla)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(BlaBla)
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BLABLA_H__BE1AE16F_9FD5_46AA_A1A5_DD009F9F90F8__INCLUDED_)
