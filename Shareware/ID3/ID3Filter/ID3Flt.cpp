// ID3Flt.cpp : Implementation of CID3Flt
#include "stdafx.h"
#include "propspec.h"

#include "ID3Filter.h"
#include "ID3Flt.h"
#include "except.h"
#include "propsetid.h"
#include "id3_fix.h"

/////////////////////////////////////////////////////////////////////////////
// CID3Flt

GUID CLSID_SummaryInformation = {
    0xF29F85E0,
    0x4FF9,
    0x1068,
    { 0xAB, 0x91, 0x08, 0x00, 0x2B, 0x27, 0xB3, 0xD9 }
};
const PID_TITLE = 2;

extern ID3_FrameDef	ID3_FrameDefs[] ;



CID3Flt::CID3Flt() :
	m_pwszFileName(NULL), m_cAttributes(0), m_rgpAttributes(NULL), m_pField(NULL), m_ulChunkID(0)
{

}

CID3Flt::~CID3Flt()
{
    delete[] m_pwszFileName;
	m_ID3Tag.Clear();

	if (m_rgpAttributes)
	{
		while(m_cAttributes)
			delete m_rgpAttributes[--m_cAttributes];

		free(m_rgpAttributes);
		m_rgpAttributes = NULL;
	}
}


HRESULT CID3Flt::ID3ErrToHR(ID3_Err e)
{
	switch(e)
	{
	case ID3E_NoMemory:
		return E_OUTOFMEMORY;

	case ID3E_FieldNotFound:
	case ID3E_InvalidFrameID:
	case ID3E_UnknownFieldType:
	case ID3E_InvalidTagVersion:
	case ID3E_zlibError:
		return FILTER_E_UNKNOWNFORMAT;

	case ID3E_NoFile:
		return FILTER_E_ACCESS;

	case ID3E_NoData:
	case ID3E_NoBuffer:
	case ID3E_TagAlreadyAttached:
	default:
		return E_FAIL;
	}
}
  

STDMETHODIMP CID3Flt::Init( 
            /* [in] */ ULONG grfFlags,
            /* [in] */ ULONG cAttributes,
            /* [size_is][in] */ const FULLPROPSPEC __RPC_FAR *rgAttributes,
            /* [out] */ ULONG __RPC_FAR *pFlags)
{

	HRESULT hr = S_OK;

    *pFlags = IFILTER_FLAGS_OLE_PROPERTIES; // Any NTFS file can have 'em!

	try
    {
		// Each time Init is called we start from scratch...
		//
        if (m_cAttributes > 0 )
        {
			if (m_rgpAttributes)
			{
				while(m_cAttributes)
					delete m_rgpAttributes[--m_cAttributes];

				free(m_rgpAttributes);
				m_rgpAttributes = NULL;
			}
        }

        if (cAttributes > 0 && rgAttributes != NULL)
        {
            // Filter only properties specified in rgAttributes.
            //
            m_rgpAttributes = (CFullPropSpec**)malloc(sizeof(CFullPropSpec*) * cAttributes);
			_ASSERTE(m_rgpAttributes);
			if (m_rgpAttributes = NULL)
				return E_OUTOFMEMORY;

            m_cAttributes = cAttributes;

            CFullPropSpec *pAttrib = (CFullPropSpec *) rgAttributes;
            for (ULONG i=0; i<cAttributes; i++)
			{
                *m_rgpAttributes[i] = pAttrib[i];
			}
        }
        else if (grfFlags & IFILTER_INIT_APPLY_INDEX_ATTRIBUTES)
        {
            // Filter all pseudo-properties.
            //
			// Enumerate through all of the frames in the tag, building a CFullPropSpec for each field and 
			// putting it in our array. CFullPropSpec holds a reference to the related field.
			// 
			// We use our CFieldID mapper class to map the ID3 frame/field ids to propertyset/property ids.
			// 
			CFullPropSpec** rgpTemp = NULL;
			ID3_FrameID frameID = ID3FID_NOFRAME;
			ULONG iFrame = 0;
			ID3_Frame* pFrame = NULL;
			ID3_Field* pField = NULL;
			for (iFrame = 0 ; iFrame < m_ID3Tag.NumFrames() ; iFrame++)
			{
				pFrame = m_ID3Tag.GetFrameNum(iFrame);
				_ASSERTE(pFrame);
				frameID = pFrame->GetID();
				ULONG iField= 0 ;
				ID3_FieldID fieldID = ID3FN_NOFIELD;
				const char* szFrameID;

				// for (iField = 0 ; iField < pFrame->GetNumFields() ; iField++) // GetNumFields() not in ID3Lib 3.05
				for (iField = 0 ; iField < ID3_Frame_GetNumFields(pFrame) ; iField++) 
				{
					// wszFrameID = pFrame->GetStringID(); // Not in ID3Lib 3.05
					szFrameID = ID3_Frame_GetStringID(pFrame); 
					_ASSERTE(szFrameID);
				
					// pField = pFrame->GetFieldNum(iField); // Not in ID3Lib 3.05
					pField = ID3_Frame_GetFieldNum(pFrame, iField); 
					_ASSERTE(pField);

					// fieldID = pField->GetID(); // Not in ID3Lib 3.05
					fieldID = ID3_Field_GetID(pField); 

					// Realloc our array for a new guy
					rgpTemp = (CFullPropSpec**)realloc(m_rgpAttributes, (sizeof(CFullPropSpec*) * (m_cAttributes+1)));
					_ASSERTE(rgpTemp);
					if (rgpTemp == NULL)
						throw CException(E_OUTOFMEMORY);
					m_rgpAttributes = rgpTemp;

					// Set the new guy
					m_rgpAttributes[m_cAttributes] = new CFullPropSpec(CFieldID(szFrameID), fieldID);
					_ASSERTE(m_rgpAttributes[m_cAttributes]);
					if (m_rgpAttributes[m_cAttributes] = NULL)
						throw CException(E_OUTOFMEMORY);

					m_cAttributes++;
				}
			}
		}
		// m_ulChunk is supposed to start of at 1.
        m_ulChunkID = 1;
    }
	catch(ID3_Error e)
	{
		hr = ID3ErrToHR(e.GetErrorID());
 	}
    catch( CException& e)
	{
//        htmlDebugOut(( DEB_ERROR,
//                       "Exception 0x%x caught in CHtmlIFilter::Init\n",
//                       e.GetErrorCode() ));
        hr = GetOleError( e );
    }

	if (FAILED(hr))
	{
		if (m_rgpAttributes)
		{
			while(m_cAttributes)
				delete m_rgpAttributes[--m_cAttributes];
			free(m_rgpAttributes);
			m_rgpAttributes = NULL;
		}
	}
    return hr;
}
        
// IFilter::GetChunk
//
// This gets called until we return FILTER_E_END_OF_CHUNKS.
//
// For each property (aka attribute) we want to return
// 2 chunks. One as a CHUNK_TEXT type and one as a CHUNK_VALUE
// type.
//
// 
STDMETHODIMP CID3Flt::GetChunk( 
            /* [out] */ STAT_CHUNK __RPC_FAR *pStat)
{
	if (m_ulChunkID > m_cAttributes)
        return FILTER_E_END_OF_CHUNKS;

	CFullPropSpec* pAttrib = m_rgpAttributes[m_ulChunkID-1];
	_ASSERTE(pAttrib);

	try
	{
		m_pField = &ID3_Tag_Find(&m_ID3Tag, CFieldID(pAttrib->GetPropSet()))->Field((ID3_FieldID)pAttrib->GetPropertyPropid());
	}
	catch(...)
	{
		return FILTER_E_ACCESS ;
	}

	// Set chunk description
	pStat->idChunk   = m_ulChunkID;
	pStat->breakType = CHUNK_EOP;
	pStat->flags     = CHUNK_VALUE;
	pStat->locale    = GetSystemDefaultLCID();
	pStat->attribute.guidPropSet       = pAttrib->GetPropSet();
	pStat->attribute.psProperty.ulKind = PRSPEC_PROPID;
	pStat->attribute.psProperty.propid = pAttrib->GetPropertyPropid();
	pStat->idChunkSource  = m_ulChunkID;
	pStat->cwcStartSource = 0;
	pStat->cwcLenSource   = 0;

	m_ulChunkID++;

    return S_OK;
}
        
STDMETHODIMP CID3Flt::GetText( 
            /* [out][in] */ ULONG __RPC_FAR *pcwcBuffer,
            /* [size_is][out] */ WCHAR __RPC_FAR *awcBuffer)
{

    return FILTER_E_NO_TEXT ;
}
        
STDMETHODIMP CID3Flt::GetValue( 
            /* [out] */ PROPVARIANT __RPC_FAR *__RPC_FAR *ppPropValue)
{

	_ASSERTE(ppPropValue);
	*ppPropValue = (PROPVARIANT*)CoTaskMemAlloc(sizeof(PROPVARIANT));
	_ASSERTE(*ppPropValue);

	switch (ID3_Field_GetType(m_pField))
	{
		case ID3FTY_INTEGER:
		case ID3FTY_BITFIELD:
			{
				(*ppPropValue)->vt = VT_I4;
				(*ppPropValue)->ulVal = m_pField->Get();
			}
			break;

		case ID3FTY_BINARY:
			return FILTER_E_NO_VALUES ;

		case ID3FTY_ASCIISTRING:
		case ID3FTY_UNICODESTRING:
			{
				ULONG n = m_pField->Size()+1;
				(*ppPropValue)->vt = VT_BSTR;
				(*ppPropValue)->bstrVal = SysAllocStringLen(NULL, n);
				n = m_pField->Get((*ppPropValue)->bstrVal, n);
				if (n == 0)
					*(*ppPropValue)->bstrVal = L'\0';
			}
		break;
	}

    return S_OK;
}
        
STDMETHODIMP CID3Flt::BindRegion( 
            /* [in] */ FILTERREGION origPos,
            /* [in] */ REFIID riid,
            /* [out] */ void __RPC_FAR *__RPC_FAR *ppunk)
{
	return E_NOTIMPL;
}
        

// IPersistFile implementation
HRESULT STDMETHODCALLTYPE CID3Flt::GetClassID(CLSID *pClassID)
{
	if (pClassID == NULL)
		return E_FAIL;
	*pClassID = GetObjectCLSID();
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CID3Flt::IsDirty( void)
{
    return S_FALSE; // Since the filter is read-only, there will never be
                    // changes to the file.
}
      
  
HRESULT STDMETHODCALLTYPE CID3Flt::Load( 
            /* [in] */ LPCOLESTR pszFileName,
            /* [in] */ DWORD dwMode)
{
    if ( m_pwszFileName != 0 )
    {
        delete m_pwszFileName;
        m_pwszFileName = 0;
    }


    HRESULT sc = S_OK;
    try
    {
        unsigned cLen = wcslen( pszFileName ) + 1;
        m_pwszFileName = new WCHAR[cLen];
        wcscpy( m_pwszFileName, pszFileName );

		char szFileName[MAX_PATH];
		WideCharToMultiByte(CP_ACP, 0, m_pwszFileName, -1, szFileName, cLen, NULL, NULL);

		m_ID3Tag.Link(szFileName);
    }
	catch ( ID3_Error err )
    {
		sc = ID3ErrToHR(err.GetErrorID());
    }

    return sc;
}

HRESULT STDMETHODCALLTYPE CID3Flt::Save( 
            /* [unique][in] */ LPCOLESTR pszFileName,
            /* [in] */ BOOL fRemember)
{
	return E_FAIL;
}
        
HRESULT STDMETHODCALLTYPE CID3Flt::SaveCompleted( 
            /* [unique][in] */ LPCOLESTR pszFileName)
{
	return E_FAIL;
}
 
//  Synopsis:   Returns a copy of the current file name
HRESULT STDMETHODCALLTYPE CID3Flt::GetCurFile( 
            /* [out] */ LPOLESTR __RPC_FAR *ppszFileName)
{
     if ( m_pwszFileName == 0 )
        return E_FAIL;

    unsigned cLen = wcslen( m_pwszFileName ) + 1;
    *ppszFileName = (WCHAR *)CoTaskMemAlloc( cLen*sizeof(WCHAR) );

    SCODE sc = S_OK;
    if ( *ppszFileName )
        wcscpy( *ppszFileName, m_pwszFileName );
    else
        sc = E_OUTOFMEMORY;

    return sc;
}