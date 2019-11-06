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


#include <string.h>
#include <memory.h>
#include "id3_tag.h"
#include "id3_misc_support.h"


luint			ID3_Tag::Render					( uchar *buffer )
{
	luint	bytesUsed	= 0;

	if	( buffer )
	{
		ID3_Elem		*cur	= frameList;
		ID3_TagHeader	header;

		SetVersion ( ID3_TAGVERSION, ID3_TAGREVISION );

		header.SetVersion ( version, revision );
		bytesUsed += header.Size();

		// set up the encryption and grouping IDs

		// ...

		while	( cur )
		{
			if	( cur->frame )
			{
				cur->frame->compression = compression;
				cur->frame->SetVersion ( version, revision );
				bytesUsed += cur->frame->Render ( &buffer[ bytesUsed ] );
			}

			cur = cur->next;
		}

		if	( syncOn )
		{
			uchar	*tempz;
			luint	newTagSize;

			newTagSize = GetUnSyncSize ( &buffer[ header.Size() ], bytesUsed - header.Size() );

			if	( newTagSize > 0 && ( newTagSize + header.Size() ) > bytesUsed )
			{
				if	( tempz = new uchar[ newTagSize ] )
				{
					UnSync ( tempz, newTagSize, &buffer[ header.Size() ], bytesUsed - header.Size() );
					header.SetFlags ( ID3HF_UNSYNC );

					memcpy ( &buffer[ header.Size() ], tempz, newTagSize );
					bytesUsed = newTagSize + header.Size();
					delete[] tempz;
				}
				else
					ID3_THROW ( ID3E_NoMemory );
			}
		}

		// zero the remainder of the buffer so that our
		// padding bytes are zero
		for	( luint i = 0; i < PaddingSize ( bytesUsed ); i++ )
			buffer[ bytesUsed + i ] = 0;

		bytesUsed += PaddingSize ( bytesUsed );

		header.SetDataSize ( bytesUsed - header.Size() );
		header.Render ( buffer );
	}
	else
		ID3_THROW ( ID3E_NoBuffer );

	// set the flag which says that the tag hasn't changed
	hasChanged = false;

	return bytesUsed;
}


luint			ID3_Tag::Size					( void )
{
	luint			bytesUsed	= 0;
	ID3_Elem		*cur		= frameList;
	ID3_TagHeader	header;

	header.SetVersion ( version, revision );
	bytesUsed += header.Size();

	while	( cur )
	{
		if	( cur->frame )
		{
			cur->frame->SetVersion ( version, revision );
			bytesUsed += cur->frame->Size();
		}

		cur = cur->next;
	}

	// add 30% for sync
	if	( syncOn )
		bytesUsed += bytesUsed / 3;

	bytesUsed += PaddingSize ( bytesUsed );

	return bytesUsed;
}


void			ID3_Tag::RenderExtHeader		( uchar *buffer )
{
	luint	bytesUsed	= 0;

	if	( version == 3 && revision == 0 )
	{
	}

	return;
}


void			ID3_Tag::GenerateTempName		( void )
{
	tmpnam ( tempName );

	return;
}


void			ID3_Tag::RenderToHandle			( void )
{
	uchar	*buffer;
	luint	size;

	if	( fileHandle )
	{
		if	( size = Size() )
		{
			if	( buffer = new uchar[ size ] )
			{
				if	( size = Render ( buffer ) )
				{
					// if the new tag fits perfectly within the old
					// and the old one actually existed (ie this isn't the first tag
					// this file has had)
					if	( ( oldTagSize == 0 && fileSize == 0 ) || ( size == oldTagSize && size != 0 ) )
					{
						fseek ( fileHandle, 0, SEEK_SET );
						fwrite ( buffer, 1, size, fileHandle );
						oldTagSize = size;
					}
					else
					{
						// else we gotta make a temp file,
						// copy the tag into it, copy the
						// rest of the old file after the tag,
						// delete the old file, rename this
						// new file to the old file's name
						// and update the fileHandle
						FILE	*tempOut;

						if	( strlen ( tempName ) > 0 )
						{
							if	( tempOut = fopen ( tempName, "wb" ) )
							{
								uchar	*buffer2;

								fwrite ( buffer, 1, size, tempOut );

								fseek ( fileHandle, oldTagSize, SEEK_SET );

								if	( buffer2 = new uchar [ BUFF_SIZE ] )
								{
									int	i;

									while	( ! feof ( fileHandle ) )
									{
										i = fread ( buffer2, 1, BUFF_SIZE, fileHandle );
										fwrite ( buffer2, 1, i, tempOut );
									}

									delete[] buffer2;
								}

								fclose ( tempOut );
								fclose ( fileHandle );
								remove ( fileName );
								rename ( tempName, fileName );
								OpenLinkedFile();

								oldTagSize = size;
							}
						}
					}
				}

				delete[] buffer;
			}
			else
				ID3_THROW ( ID3E_NoMemory );
		}
	}
	else
		ID3_THROW ( ID3E_NoData );

	return;
}


#define	ID3_PADMULTIPLE		( 2048 )
#define	ID3_PADMAX			( 4096 )


luint			ID3_Tag::PaddingSize			( luint curSize )
{
	luint	newSize	= 0;

	// if padding is switched off or there is no attached file
	if	( ! padding || fileSize == 0 )
		return 0;

	// if the old tag was large enough to hold the new tag, then
	// we will simply pad out the difference - that way the new
	// tag can be written without shuffling the rest of the song
	// file around
	if	( oldTagSize && ( oldTagSize > curSize ) && ( curSize - oldTagSize ) < ID3_PADMAX )
		newSize = oldTagSize;
	else
	{
		luint	tempSize	= curSize + fileSize;

		// this method of automatic padding
		// rounds the COMPLETE FILE up to the
		// nearest 2K.  If the file will already be an
		// even multiple of 2K (with the tag included)
		// then we just add another 2K of padding
		tempSize = ( ( tempSize / ID3_PADMULTIPLE ) + 1 ) * ID3_PADMULTIPLE;

		// the size of the new tag is the new filesize
		// minus the song size
		newSize = tempSize - fileSize;
	}

	return newSize - curSize;
}


