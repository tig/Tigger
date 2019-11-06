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


#include "id3_misc_support.h"


// converts an ASCII string into a Unicode one

void			ID3_ASCIItoUnicode				( wchar_t *unicode, char *ascii, luint len )
{
	if	( ascii && unicode )
		for	( luint i = 0; i < len; i++ )
			unicode[ i ] = (wchar_t) ascii[ i ] & 0xFF;

	return;
}


// converts a Unicode string into ASCII

void			ID3_UnicodeToASCII				( char *ascii, wchar_t *unicode, luint len )
{
	if	( unicode && ascii )
		for	( luint i = 0; i < len; i++ )
			ascii[ i ] = unicode[ i ] & 0xFF;

	return;
}


void			ID3_AddArtist					( ID3_Tag *tag, char *text )
{
	if	( tag->Find ( ID3FID_LEADARTIST ) == NULL &&
		  tag->Find ( ID3FID_BAND ) == NULL &&
		  tag->Find ( ID3FID_CONDUCTOR ) == NULL &&
		  tag->Find ( ID3FID_COMPOSER ) == NULL &&
		  strlen ( text ) > 0 )
		{
			ID3_Frame	*artistFrame;

			if	( artistFrame = new ID3_Frame )
			{
				artistFrame->SetID ( ID3FID_LEADARTIST );
				artistFrame->Field ( ID3FN_TEXT ) = text;
				tag->AddFrame ( artistFrame, true );
			}
			else
				ID3_THROW ( ID3E_NoMemory );
		}

	return;
}


void			ID3_AddAlbum					( ID3_Tag *tag, char *text )
{
	if	( tag->Find ( ID3FID_ALBUM ) == NULL && strlen ( text ) > 0 )
	{
		ID3_Frame	*albumFrame;

		if	( albumFrame = new ID3_Frame )
		{
			albumFrame->SetID ( ID3FID_ALBUM );
			albumFrame->Field ( ID3FN_TEXT ) = text;
			tag->AddFrame ( albumFrame, true );
		}
		else
			ID3_THROW ( ID3E_NoMemory );
	}

	return;
}


void			ID3_AddTitle					( ID3_Tag *tag, char *text )
{
	if	( tag->Find ( ID3FID_TITLE ) == NULL && strlen ( text ) > 0 )
	{
		ID3_Frame	*titleFrame;

		if	( titleFrame = new ID3_Frame )
		{
			titleFrame->SetID ( ID3FID_TITLE );
			titleFrame->Field ( ID3FN_TEXT ) = text;
			tag->AddFrame ( titleFrame, true );
		}
		else
			ID3_THROW ( ID3E_NoMemory );
	}

	return;
}


void			ID3_AddLyrics					( ID3_Tag *tag, char *text )
{
	if	( tag->Find ( ID3FID_UNSYNCEDLYRICS ) == NULL && strlen ( text ) > 0 )
	{
		ID3_Frame	*lyricsFrame;

		if	( lyricsFrame = new ID3_Frame )
		{
			lyricsFrame->SetID ( ID3FID_UNSYNCEDLYRICS );
			lyricsFrame->Field ( ID3FN_LANGUAGE ) = "eng";
			lyricsFrame->Field ( ID3FN_TEXT ) = text;
			tag->AddFrame ( lyricsFrame, true );
		}
		else
			ID3_THROW ( ID3E_NoMemory );
	}

	return;
}


