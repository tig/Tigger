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


#ifndef	ID3LIB_HEADER_FRAME_H
#define	ID3LIB_HEADER_FRAME_H


#include "id3_types.h"
#include "id3_header.h"
#include "id3_header_tag.h"
#include "id3_field.h"


#define	ID3FL_TAGALTER							( 1 << 15 )
#define	ID3FL_FILEALTER							( 1 << 14 )
#define	ID3FL_SIGNED							( 1 << 13 )

#define	ID3FL_COMPRESSION						( 1 <<  7 )
#define	ID3FL_ENCRYPTION						( 1 <<  6 )
#define	ID3FL_GROUPING							( 1 <<  5 )


struct ID3_FrameAttr
{
char	textID	[ 5 ];
luint	size;
luint	flags;
};


class ID3_FrameHeader : public ID3_Header
{
public:
virtual luint	Size							( void );
void			SetFrameID						( ID3_FrameID id );
luint			GetFrameInfo					( ID3_FrameAttr &attr, uchar *buffer );
virtual luint	Render							( uchar *buffer );

// *** PRIVATE INTERNAL DATA - DO NOT USE *** PRIVATE INTERNAL DATA - DO NOT USE ***

protected:
ID3_FrameID		frameID;						// which frame are we the header for?
};


#endif


