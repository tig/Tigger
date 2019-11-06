//  The authors have released ID3Lib as Public Domain (PD) and claim no copyright,
//  patent or other intellectual property protection in this work.  This means that
//  it may be modified, redistributed and used in commercial and non-commercial
//  software and hardware without restrictions.  ID3Lib is distributed on an "AS IS"
//  basis, WITHOUT WARRANTY OF ANY KIND, either express or implied.
//  
//  The ID3Lib authors encourage improvements and optimisations to be sent to the
//  ID3Lib coordinator, currently Dirk Mahoney (dirk@id3.org).  Approved
//  submissions may be altered, and will be included and released under these terms.
//  
//  Sun Nov 22 14:53:14 1998


#ifndef	ID3LIB_TAG_H
#define	ID3LIB_TAG_H


#include <wchar.h>
#include <stdio.h>
#include "id3_types.h"
#include "id3_frame.h"
#include "id3_header_frame.h"
#include "id3_header_tag.h"
#include "id3_version.h"


// for file buffers etc
#define	BUFF_SIZE	( 65536 )


struct ID3_Elem
{
ID3_Elem		*next;
ID3_Frame		*frame;
uchar			*binary;
bool			tagOwns;
};


struct ID3V1_Tag
{
char			ID			[  3 ],
				title		[ 30 ],
				artist		[ 30 ],
				album		[ 30 ],
				year		[  4 ],
				comment		[ 30 ];
uchar			genre;
};


class ID3_Tag
{
public:
				ID3_Tag							( char *name = NULL );
				~ID3_Tag						( void );

void			Clear							( void );
bool			HasChanged						( void );
void			SetUnsync						( bool newSync );
void			SetExtendedHeader				( bool ext );
void			SetCompression					( bool comp );
void			SetPadding						( bool pad );
void			AddFrame						( ID3_Frame *newFrame, bool freeWhenDone = false );
void			AddFrames						( ID3_Frame *newFrames, luint numFrames, bool freeWhenDone = false );
void			RemoveFrame						( ID3_Frame *oldFrame );
luint			Render							( uchar *buffer );
luint			Size							( void );
void			Parse							( uchar header[ ID3_TAGHEADERSIZE ], uchar *buffer );
luint			Link							( char *fileInfo );
void			Update							( void );
void			Strip							( bool v1Also = true );
ID3_Frame		*Find							( ID3_FrameID id );
ID3_Frame		*Find							( ID3_FrameID id, ID3_FieldID fld, luint data );
ID3_Frame		*Find							( ID3_FrameID id, ID3_FieldID fld, char *data );
ID3_Frame		*Find							( ID3_FrameID id, ID3_FieldID fld, wchar_t *data );
luint			NumFrames						( void );
ID3_Frame		*GetFrameNum						( luint num );
ID3_Frame		*operator[]						( luint num );

// *** PRIVATE INTERNAL DATA - DO NOT USE *** PRIVATE INTERNAL DATA - DO NOT USE ***

void			SetupTag						( char *fileInfo );
void			SetVersion						( uchar ver, uchar rev );
void			ClearList						( ID3_Elem *list );
void			DeleteElem						( ID3_Elem *cur );
void			AddBinary						( uchar *buffer, luint size );
void			ExpandBinaries					( uchar *buffer, luint size );
void			ProcessBinaries					( ID3_FrameID whichFrame = ID3FID_NOFRAME, bool attach = true ); // this default means all frames
void			RemoveFromList					( ID3_Elem *which, ID3_Elem **list );
ID3_Elem		*GetLastElem					( ID3_Elem *list );
ID3_Elem		*Find							( ID3_Frame *frame );
luint			PaddingSize						( luint curSize );
void			GenerateTempName				( void );
void			RenderExtHeader					( uchar *buffer );
void			OpenLinkedFile					( void );
void			RenderToHandle					( void );
luint			ParseFromHandle					( void );
void			ParseID3v1						( void );
void			ParseLyrics3					( void );
luint			GetUnSyncSize					( uchar *buffer, luint size );
void			UnSync							( uchar *destData, luint destSize, uchar *sourceData, luint sourceSize );
luint			ReSync							( uchar *binarySourceData, luint sourceSize );
uchar			version;						// what version tag?
uchar			revision;						// what revision tag?
ID3_Elem		*frameList;						// the list of known frames currently attached to this tag
ID3_Elem		*binaryList;					// the list of yet-to-be-parsed frames currently attached to this tag
ID3_Elem		*findCursor;					// on which element in the frameList are we currently positioned?
bool			syncOn;							// should we unsync this tag when rendering?
bool			compression;					// should we compress frames when rendering?
bool			padding;						// add padding to tags?
bool			extendedHeader;					// create an extended header when rendering?
bool			hasChanged;						// has the tag changed since the last parse or render?
FILE			*fileHandle;					// a handle to the file we are linked to
char			fileName[ 256 ];				// the name of the file we are linked to
char			tempName[ 256 ];				// a temp name in case we need a temp file for copying stuff
luint			fileSize;						// the size of the file (without any tag)
luint			oldTagSize;						// the size of the old tag (if any)
luint			extraBytes;						// extra bytes to strip from end of file (ID3v1 and Lyrics3 tags)
bool			hasV1Tag;						// does the file have an ID3v1 tag attached?
static luint	instances;						// how many ID3_Tag objects are floating around in this app?
};


ID3_Tag&		operator<<						( ID3_Tag& tag, ID3_Frame& frame );
ID3_Tag&		operator<<						( ID3_Tag& tag, ID3_Frame *frame );


#endif


