// Header for DLL interface to ID3Lib
//

#ifndef	ID3LIB_ID3LIB_H
#define	ID3LIB_ID3LIB_H

#include "id3_tag.h"

#ifdef  __cplusplus
extern "C" {
#endif

typedef struct tagID3_VerInfo
{
	char	name[30];
	ULONG	version;
	ULONG	revision;
} ID3_VerInfo;


__declspec(dllimport) void ID3_GetVersion(ID3_VerInfo *info);

//lsint       ID3_IsTagHeader ( uchar header[ ID3_TAGHEADERSIZE ] );

__declspec(dllimport) ID3_Tag     *ID3Tag_New     ( void );
__declspec(dllimport) void        ID3Tag_Delete       ( ID3_Tag *tag );
__declspec(dllimport) void        ID3Tag_Clear        ( ID3_Tag *tag );
__declspec(dllimport) bool        ID3Tag_HasChanged   ( ID3_Tag *tag );
__declspec(dllimport) void        ID3Tag_SetUnsync    ( ID3_Tag *tag, bool unsync );
__declspec(dllimport) void        ID3Tag_SetExtendedHeader( ID3_Tag *tag, bool ext );
__declspec(dllimport) void        ID3Tag_SetCompression( ID3_Tag *tag, bool comp );
__declspec(dllimport) void        ID3Tag_SetPadding   ( ID3_Tag *tag, bool pad );
__declspec(dllimport) void        ID3Tag_AddFrame ( ID3_Tag *tag, ID3_Frame *frame );
__declspec(dllimport) void        ID3Tag_AddFrames( ID3_Tag *tag, ID3_Frame *frames, luint num );
__declspec(dllimport) void        ID3Tag_RemoveFrame  ( ID3_Tag *tag, ID3_Frame *frame );
__declspec(dllimport) void        ID3Tag_Parse( ID3_Tag *tag, uchar header[ ID3_TAGHEADERSIZE ], uchar *buffer );
__declspec(dllimport) luint       ID3Tag_Link     ( ID3_Tag *tag, char *fileName );
__declspec(dllimport) void        ID3Tag_Update       ( ID3_Tag *tag );
__declspec(dllimport) void        ID3Tag_Strip        ( ID3_Tag *tag, bool v1Also );
__declspec(dllimport) ID3_Frame   *ID3Tag_FindFrameWithID( ID3_Tag *tag, ID3_FrameID id );
__declspec(dllimport) ID3_Frame   *ID3Tag_FindFrameWithINT( ID3_Tag *tag, ID3_FrameID id, ID3_FieldID fld, luint data );
__declspec(dllimport) ID3_Frame   *ID3Tag_FindFrameWithASCII( ID3_Tag *tag, ID3_FrameID id, ID3_FieldID fld, char *data );
__declspec(dllimport) ID3_Frame   *ID3Tag_FindFrameWithUNICODE( ID3_Tag *tag, ID3_FrameID id, ID3_FieldID fld, wchar_t *data );
__declspec(dllimport) luint       ID3Tag_NumFrames    ( ID3_Tag *tag );
__declspec(dllimport) ID3_Frame   *ID3Tag_GetFrameNum ( ID3_Tag *tag, luint num );
__declspec(dllimport) void        ID3Frame_Clear  ( ID3_Frame *frame );
__declspec(dllimport) void        ID3Frame_SetID  ( ID3_Frame *frame, ID3_FrameID id );
__declspec(dllimport) ID3_FrameID ID3Frame_GetID  ( ID3_Frame *frame );
__declspec(dllimport) ID3_Field   *ID3Frame_GetField  ( ID3_Frame *frame, ID3_FieldID name );
__declspec(dllimport) void        ID3Field_Clear  ( ID3_Field *field );
__declspec(dllimport) luint       ID3Field_Size       ( ID3_Field *field );
__declspec(dllimport) luint       ID3Field_GetNumTextItems( ID3_Field *field );
__declspec(dllimport) void        ID3Field_SetINT ( ID3_Field *field, luint data );
__declspec(dllimport) luint       ID3Field_GetINT ( ID3_Field *field );
__declspec(dllimport) void        ID3Field_SetUNICODE ( ID3_Field *field, wchar_t *string );
__declspec(dllimport) luint       ID3Field_GetUNICODE( ID3_Field *field, wchar_t *buffer, luint maxChars, luint itemNum );
__declspec(dllimport) void        ID3Field_AddUNICODE ( ID3_Field *field, wchar_t *string );
__declspec(dllimport) void        ID3Field_SetASCII   ( ID3_Field *field, char *string );
__declspec(dllimport) luint       ID3Field_GetASCII   ( ID3_Field *field, char *buffer, luint maxChars, luint itemNum );
__declspec(dllimport) void        ID3Field_AddASCII   ( ID3_Field *field, char *string );
__declspec(dllimport) void        ID3Field_SetBINARY( ID3_Field *field, uchar *data, luint size );
__declspec(dllimport) void        ID3Field_GetBINARY( ID3_Field *field, uchar *buffer, luint buffLength );
__declspec(dllimport) void        ID3Field_FromFile   ( ID3_Field *field, char *fileName );
__declspec(dllimport) void        ID3Field_ToFile ( ID3_Field *field, char *fileName );

#ifdef  __cplusplus
}
#endif

#endif
