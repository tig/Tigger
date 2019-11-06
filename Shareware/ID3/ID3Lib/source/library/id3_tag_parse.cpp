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


#include <stdio.h>
#include <string.h>
#include <memory.h>
#include "id3_tag.h"
#include "zlib.h"


ID3_Elem		*ID3_Tag::GetLastElem			( ID3_Elem *list )
{
	ID3_Elem	*last;

	last = list;

	while	( last && last->next )
		last = last->next;

	return last;
}



void			ID3_Tag::AddBinary				( uchar *buffer, luint size )
{
	uchar	*newBin;

	if	( size > 0 )
	{
		if	( newBin = new uchar[ size ] )
		{
			ID3_Elem	*elem, *lastElem;

			memcpy ( newBin, buffer, size );

			if	( elem = new ID3_Elem )
			{
				elem->next		= NULL;
				elem->frame		= NULL;
				elem->binary	= newBin;
				elem->tagOwns	= true;

				lastElem = GetLastElem ( binaryList );

				if	( lastElem )
					lastElem->next = elem;
				else
					binaryList = elem;
			}
			else
				ID3_THROW ( ID3E_NoMemory );
		}
		else
			ID3_THROW ( ID3E_NoMemory );
	}
	else
		ID3_THROW ( ID3E_NoData );

	return;
}


void			ID3_Tag::ExpandBinaries			( uchar *buffer, luint size )
{
	ID3_FrameAttr	attr;
	ID3_FrameHeader	frHeader;
	luint			posn	= 0;

	while	( posn < ( size - 6 ) && buffer[ posn ] != 0 )
	{
		luint	newBinSize;

		frHeader.SetVersion ( version, revision );

		posn += frHeader.GetFrameInfo ( attr, &buffer[ posn ] );
		newBinSize = frHeader.Size() + attr.size;

		// firstly, let's check to see if we are parsing a CDM.  If so,
		// let's expanded it out now.
		if	( strcmp ( attr.textID, "CDM" ) == 0 )
		{
			// if the method was zlib
			if	( buffer[ posn ] == 'z' )
			{
				luint	expandedSize	= 0;
				uchar	*expBin;

				expandedSize |= buffer[ posn + 1 ] << 24;
				expandedSize |= buffer[ posn + 2 ] << 16;
				expandedSize |= buffer[ posn + 3 ] << 8;
				expandedSize |= buffer[ posn + 4 ];

				if	( expBin = new uchar[ expandedSize ] )
				{
					uncompress ( expBin, &expandedSize, &buffer[ posn + 1 + sizeof ( luint ) ], attr.size - sizeof ( luint ) - 1 );

					ExpandBinaries ( expBin, expandedSize );

					posn += attr.size;

					delete[] expBin;
				}
			}
		}
		else
		{
			AddBinary ( &buffer[ posn - frHeader.Size() ], attr.size + frHeader.Size() );
			posn += attr.size;
		}
	}

	return;
}


void			ID3_Tag::ProcessBinaries		( ID3_FrameID whichFrame, bool attach )
{
	ID3_FrameAttr	attr;
	ID3_FrameHeader	frHeader;
	ID3_Elem		*cur	= binaryList;

	frHeader.SetVersion ( version, revision );

	while	( cur && cur->binary )
	{
		ID3_FrameID		id;
		ID3_Frame		*frame;
		luint			posn;
		luint			extras			= 0;
		luint			expandedSize	= 0;
		uchar			groupingID		= 0;
		uchar			encryptionID	= 0;

		posn = frHeader.GetFrameInfo ( attr, cur->binary );

		if	( attr.flags & ID3FL_COMPRESSION )
		{
			expandedSize |= cur->binary[ posn + 0 ] << 24;
			expandedSize |= cur->binary[ posn + 1 ] << 16;
			expandedSize |= cur->binary[ posn + 2 ] << 8;
			expandedSize |= cur->binary[ posn + 3 ];

			extras += sizeof ( luint );
		}

		if	( attr.flags & ID3FL_ENCRYPTION )
		{
			encryptionID = cur->binary[ posn ];
			posn++, extras++;
		}

		if	( attr.flags & ID3FL_GROUPING )
		{
			groupingID = cur->binary[ posn ];
			posn++, extras++;
		}

		id = ID3_FindFrameID ( attr.textID );

		if	( ( id == whichFrame || whichFrame == ID3FID_NOFRAME ) && id != ID3FID_NOFRAME )
		{
			uchar	*expBin;
			bool	useExpBin		= false;

			if	( attr.flags & ID3FL_COMPRESSION )
			{
				if	( ! ( expBin = new uchar[ expandedSize ] ) )
					ID3_THROW ( ID3E_NoMemory );

				uncompress ( expBin, &expandedSize, &cur->binary[ posn + sizeof ( luint ) ], attr.size - extras );
				useExpBin = true;
			}

			if	( ! ( frame = new ID3_Frame ) )
				ID3_THROW ( ID3E_NoMemory );

			ID3_Elem	*elem, *lastElem;

			frame->SetID ( id );

			if	( useExpBin )
			{
				frame->Parse ( expBin, expandedSize );
				delete[] expBin;
			}
			else
				frame->Parse ( &cur->binary[ posn ], attr.size - extras );

			// here is where we call a special handler
			// for this frame type if one is specified
			// in the frame definition
			{
				ID3_FrameDef	*frameInfo;

				frameInfo = ID3_FindFrameDef ( id );

				if	( frameInfo && frameInfo->parseHandler )
					attach = frameInfo->parseHandler ( frame );
			}

			// if, after all is said and done, we
			// are still supposed to attach our
			// newly parsed frame to the tag, do so
			if	( attach )
			{
				if	( ! ( elem = new ID3_Elem ) )
					ID3_THROW ( ID3E_NoMemory );

				elem->next		= NULL;
				elem->frame		= frame;
				elem->binary	= NULL;
				elem->tagOwns	= true;

				lastElem = GetLastElem ( frameList );

				if	( lastElem )
					lastElem->next = elem;
				else
					frameList = elem;
			}
			else
			{
				// if not, delete it
				delete frame;
			}

			ID3_Elem	*temp	= cur;
			cur = cur->next;

			RemoveFromList ( temp, &binaryList );
		}
		else
		{
			cur = cur->next;
		}
	}

	return;
}


void			ID3_Tag::Parse					( uchar header[ ID3_TAGHEADERSIZE ], uchar *buffer )
{
	luint			tagSize	= 0;
	bool			done	= false;
	int28			temp	= &header[ 6 ];
	luint			posn	= 0;
	uchar			prevVer	= version;
	uchar			prevRev	= revision;

	Clear();

	tagSize = temp.get();

	SetVersion ( header[ 3 ], header[ 4 ] );

	if	( header[ 5 ] & ID3HF_UNSYNC )
		tagSize = ReSync ( buffer, tagSize );

	// okay, if we are 2.01, then let's skip over the
	// extended header for now because I am lazy
	if	( version == 2 && revision == 1 )
		if	( header[ 5 ] & ID3HF_EXTENDEDHEADER )
		{
			luint	extSize	= 0;

			extSize |= buffer[ 0 ] << 24;
			extSize |= buffer[ 1 ] << 16;
			extSize |= buffer[ 2 ] <<  8;
			extSize |= buffer[ 3 ] <<  0;

			posn = extSize + sizeof ( luint );
		}

	// okay, if we are 3.00, then let's actually
	// parse the extended header (for now, we skip
	// it because we are lazy)
	if	( version == 3 && revision == 0 )
		if	( header[ 5 ] & ID3HF_EXTENDEDHEADER )
		{
			luint	extSize	= 0;

			extSize |= buffer[ 0 ] << 24;
			extSize |= buffer[ 1 ] << 16;
			extSize |= buffer[ 2 ] <<  8;
			extSize |= buffer[ 3 ] <<  0;

			posn = extSize + sizeof ( luint );
		}

	// this call will convert the binary data block (tag)
	// into a linked list of binary frames
	ExpandBinaries ( &buffer[ posn ], tagSize );

	// let's parse the CRYPTO frames
	// the 'false' parameter means "don't
	// attach the frame to the tag when
	// processed".  This is because we have
	// installed a parsing handler for the
	// crypto reg frame.  This is a default
	// parameter - if the frame type has a
	// custom parsing handler, that handler
	// will tell ID3Lib whether to attach
	// or not.
	ProcessBinaries ( ID3FID_CRYPTOREG, false );

	// let's parse the GROUPING frames
	// the 'false' parameter means "don't
	// attach the frame to the tag when
	// processed".  This is because we have
	// installed a parsing handler for the
	// crypto reg frame.  This is a default
	// parameter - if the frame type has a
	// custom parsing handler, that handler
	// will tell ID3Lib whether to attach
	// or not.
	ProcessBinaries ( ID3FID_GROUPINGREG, false );

	// let's parse the rest of the binaries
	ProcessBinaries();

	// reset the version parameters which
	// were in effect before the parse
	SetVersion ( prevVer, prevRev );

	// set the flag which says that the tag hasn't changed
	hasChanged = false;

	return;
}


luint			ID3_Tag::ParseFromHandle		( void )
{
	luint	size	= 0;

	if	( fileHandle )
	{
		uchar	header	[ ID3_TAGHEADERSIZE ];
		lsint	tagSize;

		fseek ( fileHandle, 0, SEEK_SET );

		if	( fread ( header, 1, sizeof ( header ), fileHandle ) )
		{
			if	( ( tagSize = ID3_IsTagHeader ( header ) ) > 0 )
			{
				uchar	*bin;

				if	( bin = new uchar[ tagSize ] )
				{
					fread ( bin, 1, tagSize, fileHandle );

					Parse ( header, bin );
					size = tagSize;

					delete[] bin;
				}
			}
		}

		ParseLyrics3();
		ParseID3v1();
	}
	else
		ID3_THROW ( ID3E_NoData );

	return size;
}


