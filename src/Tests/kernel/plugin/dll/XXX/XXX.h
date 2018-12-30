// XXX.h : main header file for the XXX application
//

#if !defined(AFX_XXX_H__A44A79CA_4127_413B_88A3_3B98C4B35AA5__INCLUDED_)
#define AFX_XXX_H__A44A79CA_4127_413B_88A3_3B98C4B35AA5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CXXXApp:
// See XXX.cpp for the implementation of this class
//

class CXXXApp : public CWinApp
{
public:
	CXXXApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CXXXApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CXXXApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_XXX_H__A44A79CA_4127_413B_88A3_3B98C4B35AA5__INCLUDED_)
