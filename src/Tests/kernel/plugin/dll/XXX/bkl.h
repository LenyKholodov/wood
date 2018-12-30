#if !defined(AFX_BKL_H__467C31F4_35DA_41F6_819F_7B68BDD9E93F__INCLUDED_)
#define AFX_BKL_H__467C31F4_35DA_41F6_819F_7B68BDD9E93F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// bkl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// bkl dialog

class bkl : public CDialog
{
// Construction
public:
	bkl(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(bkl)
	enum { IDD = IDD_DIALOG2 };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(bkl)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(bkl)
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BKL_H__467C31F4_35DA_41F6_819F_7B68BDD9E93F__INCLUDED_)
