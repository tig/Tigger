#include "stdafx.h"
#include "propsetid.h"



CFieldID::CFieldID() :
		m_ulEncodedID(0)
{
	*m_wszFieldID = L'\0' ;
}

CFieldID::CFieldID(const WCHAR* pwszFieldID)
{
	EncodeFieldID(pwszFieldID);
}

CFieldID::CFieldID(const CHAR* pszFieldID)
{
	WCHAR wszFieldID[5];
	MultiByteToWideChar(CP_ACP, 0, pszFieldID, 4, wszFieldID, 4);
	EncodeFieldID(wszFieldID);
}

CFieldID::CFieldID(const UUID& refPropSetID)
{
	EncodePropID(refPropSetID);
}

CFieldID::~CFieldID()
{}

void CFieldID::EncodeFieldID(const WCHAR* pwszFieldID)
{
	ULONG n;

	m_ulEncodedID = 0;

	for (long i = 0 ; i <= 3 ;i++)
	{
		n = *(m_wszFieldID+i) = *(pwszFieldID+i);
		m_ulEncodedID |= (n << (8*i));
	}
}

UUID CFieldID::GetPropSetID() const
{
	UUID refPropSetID = UUID_ID3v2PropSetBase;
	refPropSetID.Data1 |= m_ulEncodedID;
	return refPropSetID;
}

void CFieldID::EncodePropID(const UUID& refPropSetID)
{
	ULONG n;
	m_ulEncodedID = refPropSetID.Data1;

	for (long i = 0 ; i <= 3 ;i++)
	{
		// shift right 8 bits and & with 0xFF 
		n = (m_ulEncodedID >> (8*i)) & 0xFF;
		*(m_wszFieldID+i) = (WCHAR)n;
	}
	*(m_wszFieldID+4) = L'\0';
}

BOOL CFieldID::operator==(const CFieldID& Other) const
{ 
	return (Other.m_ulEncodedID == m_ulEncodedID);
}

const CFieldID& CFieldID::operator=(const WCHAR* pwszFieldID)
{
	EncodeFieldID(pwszFieldID);
	return *this;
}

const CFieldID& CFieldID::operator=(const CHAR* pszFieldID)
{
	WCHAR wszFieldID[5];
	MultiByteToWideChar(CP_ACP, 0, pszFieldID, 4, wszFieldID, 4);
	EncodeFieldID(wszFieldID);
	return *this;
}

const CFieldID& CFieldID::operator=(const UUID& uuid)
{
	EncodePropID(uuid);
	return *this;
}

CFieldID::operator UUID() const
{
	return GetPropSetID();
}

CFieldID::operator const WCHAR*() const
{ 
	return m_wszFieldID ; 
}
