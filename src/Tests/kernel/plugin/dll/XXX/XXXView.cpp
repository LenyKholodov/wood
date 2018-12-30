// XXXView.cpp : implementation of the CXXXView class
//

#include "stdafx.h"
#include "XXX.h"

#include "XXXDoc.h"
#include "XXXView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CXXXView

IMPLEMENT_DYNCREATE(CXXXView, CView)

BEGIN_MESSAGE_MAP(CXXXView, CView)
	//{{AFX_MSG_MAP(CXXXView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CXXXView construction/destruction

CXXXView::CXXXView()
{
	// TODO: add construction code here

}

CXXXView::~CXXXView()
{
}

BOOL CXXXView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CXXXView drawing

void CXXXView::OnDraw(CDC* pDC)
{
	CXXXDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
}

/////////////////////////////////////////////////////////////////////////////
// CXXXView printing

BOOL CXXXView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CXXXView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CXXXView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CXXXView diagnostics

#ifdef _DEBUG
void CXXXView::AssertValid() const
{
	CView::AssertValid();
}

void CXXXView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CXXXDoc* CXXXView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CXXXDoc)));
	return (CXXXDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CXXXView message handlers
