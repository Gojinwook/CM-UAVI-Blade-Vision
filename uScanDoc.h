// uScanDoc.h : interface of the CuScanDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_uScanDOC_H__13536236_B603_4CB1_91F4_E6F1ED40A0B5__INCLUDED_)
#define AFX_uScanDOC_H__13536236_B603_4CB1_91F4_E6F1ED40A0B5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CuScanDoc : public CDocument
{
protected: // create from serialization only
	CuScanDoc();
	DECLARE_DYNCREATE(CuScanDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CuScanDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CuScanDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CuScanDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_uScanDOC_H__13536236_B603_4CB1_91F4_E6F1ED40A0B5__INCLUDED_)
