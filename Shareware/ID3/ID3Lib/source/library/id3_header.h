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


#ifndef	ID3LIB_HEADER_H
#define	ID3LIB_HEADER_H


#include "id3_types.h"


#define	ID3_TAGVERSION							(  3 )
#define	ID3_TAGREVISION							(  0 )


struct ID3_HeaderInfo
{
uchar			version;
uchar			revision;
uchar			frameIDBytes;
uchar			frameSizeBytes;
uchar			frameFlagsBytes;
bool			hasExtHeader;
luint			extHeaderBytes;
bool			setExpBit;
};


extern	ID3_HeaderInfo	ID3_VersionInfo[];


class ID3_Header
{
public:
				ID3_Header						( void );

void			SetVersion						( uchar ver, uchar rev );
void			SetDataSize						( luint newSize );
void			SetFlags						( luint newFlags );
virtual luint	Size							( void ) = 0;
virtual luint	Render							( uchar *buffer ) = 0;

// *** PRIVATE INTERNAL DATA - DO NOT USE *** PRIVATE INTERNAL DATA - DO NOT USE ***

protected:
uchar			version;						// which version?
uchar			revision;						// which revision?
luint			dataSize;						// how big is the data?
luint			flags;							// tag flags
ID3_HeaderInfo	*info;							// the info about this version of the headers
};


ID3_HeaderInfo	*ID3_LookupHeaderInfo			( uchar ver, uchar rev );


#endif


