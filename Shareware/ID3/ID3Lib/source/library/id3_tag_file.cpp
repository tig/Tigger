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
#include "id3_tag.h"


bool			exists							( char *name )
{
	bool	doesExist	= false;
	FILE	*in;

	if	( name )
	{
		if	( in = fopen ( name, "rb" ) )
		{
			doesExist = true;
			fclose ( in );
		}
	}
	else
		ID3_THROW ( ID3E_NoData );

	return doesExist;
}


void			ID3_Tag::OpenLinkedFile			( void )
{
	char	*mode		= "rb+";

	if	( ! exists ( fileName ) )
		mode = "wb+";

	if	( fileHandle = fopen ( fileName, mode ) )
	{
		fseek ( fileHandle, 0, SEEK_END );
		fileSize = ftell ( fileHandle );
		fseek ( fileHandle, 0, SEEK_SET );
	}

	return;
}


luint			ID3_Tag::Link					( char *fileInfo )
{
	luint	posn	= 0;

	if	( fileInfo )
	{
		strncpy ( fileName, fileInfo, 256 );

		// if we were attached to some other file
		// file then abort
		if	( fileHandle )
			ID3_THROW ( ID3E_TagAlreadyAttached );

		GenerateTempName();
		OpenLinkedFile();
		oldTagSize = ParseFromHandle();

		if	( oldTagSize )
			oldTagSize += ID3_TAGHEADERSIZE;

		fileSize -= oldTagSize;
		posn = oldTagSize;
	}
	else
		ID3_THROW ( ID3E_NoData );

	return posn;
}


void			ID3_Tag::Update					( void )
{
	if	( HasChanged() )
		RenderToHandle();

	return;
}


void			ID3_Tag::Strip					( bool v1Also )
{
	FILE	*tempOut;

	if	( strlen ( tempName ) > 0 )
	{
		if	( tempOut = fopen ( tempName, "wb" ) )
		{
			uchar	*buffer2;

			fseek ( fileHandle, oldTagSize, SEEK_SET );

			if	( buffer2 = new uchar[ BUFF_SIZE ] )
			{
				bool	done		= false;
				luint	bytesCopied	= 0;
				luint	bytesToCopy	= fileSize;
				int		i;

				if	( hasV1Tag && v1Also )
					bytesToCopy -= extraBytes;

				while	( ! feof ( fileHandle ) && ! done )
				{
					luint	size	= BUFF_SIZE;

					if	( ( bytesToCopy - bytesCopied ) < BUFF_SIZE )
					{
						size = bytesToCopy - bytesCopied;
						done = true;
					}

					if	( i = fread ( buffer2, 1, size, fileHandle ) )
						fwrite ( buffer2, 1, i, tempOut );

					bytesCopied += i;
				}

				delete[] buffer2;
			}

			fclose ( tempOut );
			fclose ( fileHandle );
			remove ( fileName );
			rename ( tempName, fileName );
			OpenLinkedFile();

			oldTagSize = 0;
			extraBytes = 0;

			if	( v1Also )
				hasV1Tag = false;

			hasChanged = true;
		}
	}

	return;
}


