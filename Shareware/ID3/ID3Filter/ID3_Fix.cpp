#include "stdafx.h"
#include "id3_fix.h"
// ========================================================================
// ID3_FrameDefs et. al. are NOT public and I should not be using them.
//
//			The functions below encapsulate all of my undocumented access to
//			ID3Lib 3.05 and can easily be re-written or removed once ID3Lib 
//			supports this type of function directly.
//
// ID3_Tag_Find is a wrapper around ID3_Tag to provide finding of a frame using
// the 4 character frame ID.
//
ID3_Frame* ID3_Tag_Find(ID3_Tag* pTag, const WCHAR *wszFrameID)
{
	char	szFrameID[5];
	if (WideCharToMultiByte(CP_ACP, 0, wszFrameID, 4, szFrameID, 4, 0 ,0) == 0)
		ID3_THROW(ID3E_NoMemory);

	ULONG iFrame = 0;
	while (ID3_FrameDefs[iFrame].id != ID3FID_NOFRAME)
	{
		if (lstrcmpA(szFrameID, ID3_FrameDefs[iFrame].longTextID) == 0)
			return pTag->Find(ID3_FrameDefs[iFrame].id);
		iFrame++;
	}

	_ASSERTE(NULL);
	return NULL;
}

// ID3_Frame_GetNumFields is a wrapper around ID3_Frame to provide the
// ability to enumerate the fields in a frame.
//
ULONG ID3_Frame_GetNumFields(ID3_Frame* pFrame)
{
	ULONG iFrame = 0;
	while (ID3_FrameDefs[iFrame].id != ID3FID_NOFRAME && ID3_FrameDefs[iFrame].id != pFrame->GetID())
		iFrame++;

	_ASSERTE(ID3_FrameDefs[iFrame].id  != ID3FID_NOFRAME);
	if (ID3_FrameDefs[iFrame].id  == ID3FID_NOFRAME)
		return 0;

	ULONG numFields = 0 ;
	while(ID3_FrameDefs[iFrame].fieldDefs[numFields].id != ID3FN_NOFIELD )
		numFields++;
	
	return numFields;
}

// ID3_Frame_GetFieldNum gets a frame by index. It is a wrapper around
// ID3_Frame and allows for enumeration of fields within a frame.
//
ID3_Field* ID3_Frame_GetFieldNum(ID3_Frame* pFrame, ULONG iField)
{
	ID3_Field* pField=NULL;

	ULONG iFrame = 0;
	while (ID3_FrameDefs[iFrame].id != ID3FID_NOFRAME && ID3_FrameDefs[iFrame].id != pFrame->GetID())
		iFrame++;

	_ASSERTE(ID3_FrameDefs[iFrame].id  != ID3FID_NOFRAME);
	if (ID3_FrameDefs[iFrame].id  == ID3FID_NOFRAME)
		return 0;

	pField = &pFrame->Field(ID3_FrameDefs[iFrame].fieldDefs[iField].id); // ouch! what a hack
	_ASSERTE(pField);
	return pField;
}

// ID3_Frame_GetStringID returns the 4 character string ID for a frame.
//
const char* ID3_Frame_GetStringID(ID3_Frame* pFrame)
{
	ULONG iFrame = 0;
	while (ID3_FrameDefs[iFrame].id != ID3FID_NOFRAME && ID3_FrameDefs[iFrame].id != pFrame->GetID())
		iFrame++;

	_ASSERTE(ID3_FrameDefs[iFrame].id  != ID3FID_NOFRAME);
	if (ID3_FrameDefs[iFrame].id  == ID3FID_NOFRAME)
		return 0;

	return ID3_FrameDefs[iFrame].longTextID;

}

// ID3_Frame_SetStringID sets the 4 character string ID for a frame.
//
void ID3_Frame_SetStringID(ID3_Frame* pFrame, const char* sz)
{

	ULONG iFrame = 0;
	while (ID3_FrameDefs[iFrame].id != ID3FID_NOFRAME && (lstrcmpA(ID3_FrameDefs[iFrame].longTextID, sz) != 0))
		iFrame++;

	_ASSERTE(ID3_FrameDefs[iFrame].id  != ID3FID_NOFRAME);
	if (ID3_FrameDefs[iFrame].id  == ID3FID_NOFRAME)
		ID3_THROW(ID3E_InvalidFrameID) ;

	pFrame->SetID(ID3_FrameDefs[iFrame].id);

	return;

}

// ID3_Field_GetID returns the ID of the field
//
ID3_FieldID ID3_Field_GetID(ID3_Field* pField)
{
	return pField->name;
}

// ID3_Field_GetID returns the ID of the field
//
ID3_FieldType ID3_Field_GetType(ID3_Field *pField)
{
	return pField->type;
}
