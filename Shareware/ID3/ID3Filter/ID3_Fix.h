#include "id3_tag.h"

extern ID3_FrameDef	ID3_FrameDefs[] ;

ID3_Frame* ID3_Tag_Find(ID3_Tag* pTag, const WCHAR *wszFrameID);


ULONG ID3_Frame_GetNumFields(ID3_Frame* pFrame);
ID3_Field* ID3_Frame_GetFieldNum(ID3_Frame* pFrame, ULONG iField);
const char* ID3_Frame_GetStringID(ID3_Frame* pFrame);
void ID3_Frame_SetStringID(ID3_Frame* pFrame, const char* sz);
ID3_FieldID ID3_Field_GetID(ID3_Field* pField);
ID3_FieldType ID3_Field_GetType(ID3_Field *pField);
