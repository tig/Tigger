// splitter.cpp : implementation file
//

#include "stdafx.h"
#include "stgedit.h"
#include "splitter.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSplitter

IMPLEMENT_DYNCREATE(CSplitter, CSplitterWnd)

CSplitter::CSplitter()
{ 
    
}

CSplitter::~CSplitter()
{                 
}

BOOL CSplitter::ReplaceView(int row, int col,CRuntimeClass * pViewClass,SIZE size)
{
  CCreateContext context;
  BOOL bSetActive;
	       
   
  if ((GetPane(row,col)->IsKindOf(pViewClass))==TRUE)
       return FALSE;
				    
   //CDocument * pDoc=GetParentFrame()->GetActiveDocument();
   
   // Get pointer to CDocument object so that it can be used in the creation 
   // process of the new view
   CDocument * pDoc= ((CView *)GetPane(row,col))->GetDocument();
   CView * pActiveView=GetParentFrame()->GetActiveView();
   if (pActiveView==NULL || pActiveView==GetPane(row,col))
      bSetActive=TRUE;
   else
      bSetActive=FALSE;

	// set flag so that document will not be deleted when view is destroyed
	BOOL bAutoDelete = pDoc->m_bAutoDelete ;
	pDoc->m_bAutoDelete=FALSE;    
	// Delete existing view 
    ((CView *) GetPane(row,col))->DestroyWindow();
	// set flag back to default 
	pDoc->m_bAutoDelete=bAutoDelete;
 
    // Create new view                      
   
   context.m_pNewViewClass=pViewClass;
   context.m_pCurrentDoc=pDoc;
   context.m_pNewDocTemplate=NULL;
   context.m_pLastView=NULL;
   context.m_pCurrentFrame=NULL;
   
   CreateView(row,col,pViewClass,size, &context);
   
   CView * pNewView= (CView *)GetPane(row,col);
   
   if (bSetActive==TRUE)
      GetParentFrame()->SetActiveView(pNewView);
   
   RecalcLayout(); 
   GetPane(row,col)->SendMessage(WM_PAINT);
   
   return TRUE;
}
    

BEGIN_MESSAGE_MAP(CSplitter, CSplitterWnd)
	//{{AFX_MSG_MAP(CSplitter)
	ON_WM_LBUTTONDOWN()
	ON_WM_SETCURSOR()
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSplitter message handlers


