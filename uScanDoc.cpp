// uScanDoc.cpp : implementation of the CuScanDoc class
//

#include "stdafx.h"
#include "uScan.h"

#include "uScanDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CuScanDoc

IMPLEMENT_DYNCREATE(CuScanDoc, CDocument)

BEGIN_MESSAGE_MAP(CuScanDoc, CDocument)
	//{{AFX_MSG_MAP(CuScanDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CuScanDoc construction/destruction

CuScanDoc::CuScanDoc()
{
	// TODO: add one-time construction code here

}

CuScanDoc::~CuScanDoc()
{
}

BOOL CuScanDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CuScanDoc serialization

void CuScanDoc::Serialize(CArchive& ar)
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
// CuScanDoc diagnostics

#ifdef _DEBUG
void CuScanDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CuScanDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CuScanDoc commands
