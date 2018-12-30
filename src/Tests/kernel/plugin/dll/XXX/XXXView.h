// XXXView.h : interface of the CXXXView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_XXXVIEW_H__A78F89D3_46AC_4F67_9F20_C57A42094530__INCLUDED_)
#define AFX_XXXVIEW_H__A78F89D3_46AC_4F67_9F20_C57A42094530__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CXXXView : public CView
{
protected: // create from serialization only
	CXXXView();
	DECLARE_DYNCREATE(CXXXView)

// Attributes
public:
	CXXXDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CXXXView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CXXXView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CXXXView)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in XXXView.cpp
inline CXXXDoc* CXXXView::GetDocument()
   { return (CXXXDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_XXXVIEW_H__A78F89D3_46AC_4F67_9F20_C57A42094530__INCLUDED_)
