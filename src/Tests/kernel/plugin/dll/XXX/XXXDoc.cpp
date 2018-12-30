// XXXDoc.cpp : implementation of the CXXXDoc class
//

#include "stdafx.h"
#include "XXX.h"

#include "XXXDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CXXXDoc

IMPLEMENT_DYNCREATE(CXXXDoc, CDocument)

BEGIN_MESSAGE_MAP(CXXXDoc, CDocument)
	//{{AFX_MSG_MAP(CXXXDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CXXXDoc construction/destruction

CXXXDoc::CXXXDoc()
{
	// TODO: add one-time construction code here

}

CXXXDoc::~CXXXDoc()
{
}

BOOL CXXXDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CXXXDoc serialization

void CXXXDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CXXXDoc diagnostics

#ifdef _DEBUG
void CXXXDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CXXXDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CXXXDoc commands
