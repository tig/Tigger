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
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include "id3_tag.h"
#include "id3_misc_support.h"


luint			ID3_CRLFtoLF					( char *buffer, luint size )
{
	luint	newSize	= 0;
	char	*dest	= buffer;
	char	*source	= buffer;

	if	( buffer && size )
	{
		while	( source < ( buffer + size ) )
		{
			if	( *source == 0x0D )
				source++;
			else
				*dest++ = *source++;
		}

		newSize = dest - buffer;
	}
	else
		ID3_THROW ( ID3E_NoData );

	return newSize;
}


luint			ID3_StripTimeStamps				( char *buffer, luint size )
{
	luint	newSize	= 0;
	char	*dest	= buffer;
	char	*source	= buffer;

	if	( buffer && size )
	{
		while	( source < ( buffer + size ) )
		{
			if	( *source == '[' )
				source += 7;
			else
				*dest++ = *source++;
		}

		newSize = dest - buffer;
	}
	else
		ID3_THROW ( ID3E_NoData );

	return newSize;
}


void			ID3_Tag::ParseLyrics3			( void )
{
	if	( fileHandle )
	{
		uchar	buffer[ 18 ];

		fseek ( fileHandle, -143, SEEK_END );
		fread ( buffer, 1, 18, fileHandle );

		// first check for an ID3v1 tag
		if	( memcmp ( &buffer[ 15 ], "TAG", 3 ) == 0 )
		{
			if	( memcmp ( &buffer[ 6 ], "LYRICSEND", 9 ) == 0 )
			{
				// we have a Lyrics3 v1.00 tag
			}
			else if ( memcmp ( &buffer[ 6 ], "LYRICS200", 9 ) == 0 )
			{
				// we have a Lyrics3 v2.00 tag
				luint	lyricsSize;

				buffer[ 6 ] = 0;
				lyricsSize = atoi ( (char *) buffer );

				fseek ( fileHandle, -18 - lyricsSize, SEEK_CUR );
				fread ( buffer, 1, 11, fileHandle );

				if	( memcmp ( buffer, "LYRICSBEGIN", 11 ) == 0 )
				{
					luint	bytesToRead	= lyricsSize - 11;
					uchar	*buff2;

					extraBytes += lyricsSize + 9 + 6;

					if	( buff2 = new uchar[ bytesToRead ] )
					{
						luint	posn		= 0;
						bool	stampsUsed	= false;

						fread ( buff2, 1, bytesToRead, fileHandle );

						while	( posn < bytesToRead )
						{
							uchar	fid[ 4 ];
							uchar	sizeT[ 6 ];
							luint	size;

							fid[ 3 ] = 0;
							sizeT[ 5 ] = 0;

							memcpy ( fid, &buff2[ posn ], 3 );
							memcpy ( sizeT, &buff2[ posn + 3 ], 5 );
							size = atoi ( (char *) sizeT );

							// the IND field
							if	( strcmp ( (char *) fid, "IND" ) == 0 )
							{
								if	( buff2[ posn + 8 + 1 ] == '1' )
									stampsUsed = true;
							}

							// the TITLE field
							if	( strcmp ( (char *) fid, "ETT" ) == 0 )
							{
								char	*text;

								if	( text = new char[ size + 1 ] )
								{
									text[ size ] = 0;
									memcpy ( text, &buff2[ posn + 8 ], size );

									ID3_AddTitle ( this, text );

									delete[] text;
								}
								else
									ID3_THROW ( ID3E_NoMemory );
							}

							// the ARTIST field
							if	( strcmp ( (char *) fid, "EAR" ) == 0 )
							{
								char	*text;

								if	( text = new char[ size + 1 ] )
								{
									text[ size ] = 0;
									memcpy ( text, &buff2[ posn + 8 ], size );

									ID3_AddArtist ( this, text );

									delete[] text;
								}
								else
									ID3_THROW ( ID3E_NoMemory );
							}

							// the ALBUM field
							if	( strcmp ( (char *) fid, "EAL" ) == 0 )
							{
								char	*text;

								if	( text = new char[ size + 1 ] )
								{
									text[ size ] = 0;
									memcpy ( text, &buff2[ posn + 8 ], size );

									ID3_AddAlbum ( this, text );

									delete[] text;
								}
								else
									ID3_THROW ( ID3E_NoMemory );
							}

							// the LYRICS field
							if	( strcmp ( (char *) fid, "LYR" ) == 0 )
							{
								char	*text;
								luint	newSize;

								newSize = ID3_CRLFtoLF ( (char *) &buff2[ posn + 8 ], size );

								if	( stampsUsed )
									newSize = ID3_StripTimeStamps ( (char *) &buff2[ posn + 8 ], newSize );

								if	( text = new char[ newSize + 1 ] )
								{
									text[ newSize ] = 0;

									memcpy ( text, &buff2[ posn + 8 ], newSize );

									ID3_AddLyrics ( this, text );

									delete[] text;
								}
								else
									ID3_THROW ( ID3E_NoMemory );
							}

							posn += size + 8;
						}

						delete[] buff2;
					}
				}
			}
		}
	}
	else
		ID3_THROW ( ID3E_NoData );

	return;
}


