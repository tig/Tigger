#include "stdafx.h"
#include "propset.h"
#include "StgEdit.h"
#include "StgEditItem.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CStgEditItem
IMPLEMENT_DYNAMIC(CStgEditItem, CObject)

CStgEditItem::CStgEditItem(CTreeCtrl* pTree)
{
	m_pTree = pTree ;
}

CStgEditItem::~CStgEditItem()
{
}

BOOL CStgEditItem::Expand(HTREEITEM hitem )
{
	return FALSE ;
}
/////////////////////////////////////////////////////////////////////////////
// CStgEditItem message handlers
 
 /////////////////////////////////////////////////////////////////////////////
// CStgEditStgItem
IMPLEMENT_DYNAMIC(CStgEditStgItem, CStgEditItem)


CStgEditStgItem::CStgEditStgItem(CTreeCtrl* pTree, IStorage* pStorage) :
	CStgEditItem(pTree)
{
	pStorage->AddRef() ;
	m_pStorage = pStorage ;
}

CStgEditStgItem::~CStgEditStgItem()
{
	if (m_pStorage)
	{
		m_pStorage->Release() ;
		m_pStorage = NULL ;
	}
}

BOOL CStgEditStgItem::Expand(HTREEITEM hitem )
{
	if (m_pStorage == NULL)
		return FALSE ;
	
    TV_INSERTSTRUCT tvis ;
    tvis.hParent = hitem ;
    tvis.hInsertAfter = TVI_LAST ;
    tvis.item.mask = TVIF_TEXT | TVIF_CHILDREN | TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE ;
    tvis.item.cChildren = 1 ;

	IEnumSTATSTG* penum = NULL ;
	HRESULT hr ;
	STATSTG	statstg ;
	CStgEditItem* pItem = NULL ;
	
	if (SUCCEEDED(hr = m_pStorage->EnumElements(0,NULL,0,&penum)))
	{
		while(S_OK == penum->Next(1, &statstg, 0))
		{
			if (statstg.type == STGTY_STORAGE)
			{
				IStorage* pstg = NULL ;
				if (SUCCEEDED(hr = m_pStorage->OpenStorage(statstg.pwcsName, NULL, STGM_READ|STGM_SHARE_EXCLUSIVE, NULL, 0, &pstg)))
				{
					pItem = new CStgEditStgItem(m_pTree, pstg) ;
					pstg->Release() ;
				    tvis.item.cChildren = 1 ;
				}
				else
				{
					TRACE0("m_pStorage->OpenStorage failed!\n");
				}
			}
			else if (statstg.type == STGTY_STREAM)
			{
				IStream* pstm= NULL ;
				if (SUCCEEDED(hr = m_pStorage->OpenStream(statstg.pwcsName, NULL, STGM_READ|STGM_SHARE_EXCLUSIVE, NULL, &pstm)))
				{
					if (*statstg.pwcsName == 0x05)
					{
						pItem = new CStgEditPsetItem(m_pTree, pstm) ;
						statstg.type = STGTY_PROPERTY ;
					    tvis.item.cChildren = 1 ;
					}
					else
					{
						pItem = new CStgEditStmItem(m_pTree, pstm) ;
					    tvis.item.cChildren = 0 ;
					}
					pstm->Release() ;
				}
				else
				{
					TRACE0("m_pStorage->OpenStream failed!\n");
				}
			}
			else
			{
				tvis.item.cChildren = 0 ;
			}

			tvis.item.lParam = (LPARAM)pItem ;
#ifdef _UNICODE
			WCHAR szName[_MAX_PATH] ;
			lstrcpy(szName, statstg.pwcsName) ;
#else
			char szName[_MAX_PATH] ;
			WideCharToMultiByte(CP_ACP, 0, statstg.pwcsName, wcslen(statstg.pwcsName)+1, szName, _MAX_PATH, NULL, NULL);
#endif
			
			if (*szName <= 0x1F)
			{
				int len = lstrlen(szName) + 1;
				TCHAR* pS = szName+len;
				TCHAR  c = *szName ;
				
				while(pS > szName)
				{
					*(pS+4) = *pS ;
					--pS ;
				}
				
				TCHAR sz[6] ;
				wsprintf(sz, _T("\\0x%02X"), (UINT)c) ;
				memcpy(szName, sz, 5) ;
			}
			
			tvis.item.pszText = szName;
			tvis.item.iImage = TYPE_TO_ID(statstg.type) ;
			tvis.item.iSelectedImage = tvis.item.iImage + 1 ;
			m_pTree->InsertItem(&tvis) ;

			CoTaskMemFree(statstg.pwcsName) ;
			pItem = NULL ;
		}
		penum->Release() ;
		return TRUE ;
	}

	return FALSE ;
}



 /////////////////////////////////////////////////////////////////////////////
// CStgEditStmItem
IMPLEMENT_DYNAMIC(CStgEditStmItem, CStgEditItem)

CStgEditStmItem::CStgEditStmItem(CTreeCtrl* pTree, IStream* pStream) :
	CStgEditItem(pTree)
{
	pStream->AddRef() ;
	m_pStream = pStream ;
}

CStgEditStmItem::~CStgEditStmItem()
{
	if (m_pStream)
	{
		m_pStream->Release() ;
		m_pStream = NULL ;
	}
}

BOOL CStgEditStmItem::Expand(HTREEITEM hitem )
{
	if (m_pStream == NULL)
		return FALSE ;
	return FALSE ;
}


 /////////////////////////////////////////////////////////////////////////////
// CStgEditPsetItem
IMPLEMENT_DYNAMIC(CStgEditPsetItem, CStgEditStmItem)

CStgEditPsetItem::CStgEditPsetItem(CTreeCtrl* pTree, IStream* pStream) :
	CStgEditStmItem(pTree, pStream)
{
}

CStgEditPsetItem::~CStgEditPsetItem()
{
}

BOOL CStgEditPsetItem::Expand(HTREEITEM hitem )
{
	if (m_pStream == NULL)
		return FALSE ;

	LARGE_INTEGER li ;
	LISet32( li, 0 );
	m_pStream->Seek( li, STREAM_SEEK_SET, NULL ) ;
	
	if (!m_propset.ReadFromStream(m_pStream))
		return FALSE ;

	TV_INSERTSTRUCT tvis ;
	tvis.hParent = hitem ;
	tvis.hInsertAfter = TVI_LAST ;
	tvis.item.mask = TVIF_TEXT | TVIF_CHILDREN | TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE ;
	tvis.item.cChildren = 0 ;

	BOOL f= FALSE ;
	CPropertySection* pSec = (CPropertySection*)(m_propset.GetList()->GetHead()) ;
	if (pSec)
	{
		CProperty* pProp ;
		POSITION pos ;
		for( pos = pSec->GetList()->GetHeadPosition(); pos != NULL; )
		{
			pProp = (CProperty*)pSec->GetList()->GetNext( pos ) ;
			if (pProp)
			{
				VARTYPE vt = (VARTYPE)pProp->GetType() ;
				CString strName ;
				if (!pSec->GetName( pProp->GetID(), strName ))
					strName = _T("<no name in dictionary>") ;

				TCHAR sz[1024] ;
				wsprintf(sz, "PropID %d: %s", pProp->GetID(), strName ) ;
				
				tvis.item.pszText = sz;
				tvis.item.iImage = TYPE_TO_ID(STGTY_PROPERTY+1) ;
				tvis.item.iSelectedImage = tvis.item.iImage + 1 ;
				
				CStgEditPropItem *pPropItem = new CStgEditPropItem(m_pTree, pSec, pProp->GetID() ) ;
				tvis.item.lParam = (LPARAM)pPropItem ;
				m_pTree->InsertItem(&tvis) ;
				f = TRUE ;

				
				/*
				DWORD cb ;
				LPVOID pv = pProp->Get(&cb);
				// TODO: Handle unicode v. mbcs correctly.
				if (vt == VT_BSTR || vt == VT_LPWSTR || vt == VT_LPSTR)
				{
					TCHAR sz[1024] ;
					wsprintf(sz, "PropID %d: %s", pProp->GetID(), pv ) ;
					
					tvis.item.pszText = sz;
					tvis.item.iImage = TYPE_TO_ID(STGTY_PROPERTY+1) ;
					tvis.item.iSelectedImage = tvis.item.iImage + 1 ;
					tvis.item.lParam = NULL ;
					m_pTree->InsertItem(&tvis) ;
					f = TRUE ;
				}
				*/
			}
		}
	}
	
	return f;
}

 /////////////////////////////////////////////////////////////////////////////
// CStgEditPropItem
IMPLEMENT_DYNAMIC(CStgEditPropItem, CStgEditItem)

CStgEditPropItem::CStgEditPropItem(CTreeCtrl* pTree, CPropertySection* pSec, ULONG ulPropID) :
	CStgEditItem(pTree)
{
	m_pSec = pSec ;
	m_ulPropID = ulPropID ;
	
	
}

CStgEditPropItem::~CStgEditPropItem()
{
}

BOOL CStgEditPropItem::Expand(HTREEITEM hitem )
{
	return FALSE ;
}
