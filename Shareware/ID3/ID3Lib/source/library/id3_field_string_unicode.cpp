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


#include <wchar.h>
#include <string.h>
#include <stdlib.h>
#include "id3_field.h"


// this function is another way of using Set()

ID3_Field&		ID3_Field::operator=			( wchar_t *string )
{
	Set ( string );

	return *this;
}


// this is Set()

void			ID3_Field::Set					( wchar_t *string )
{
	luint	bytesUsed	= wcslen ( string );

	// we can simply increment the
	// bytesUsed count here because
	// we just pilfer the NULL which is
	// present in the string which was
	// passed to us
	if	( flags & ID3FF_NULL )
		bytesUsed++;

	// doubling the bytesUsed because
	// Unicode is twice the size of ASCII
	bytesUsed *= sizeof ( wchar_t );

	Set ( (uchar *) string, bytesUsed );

	type = ID3FTY_UNICODESTRING;
	hasChanged = true;

	return;
}


void			ID3_Field::Add					( wchar_t *string )
{
	if	( ! data )
		Set ( string );
	else
	{
		wchar_t	*temp;
		luint	newLen;
		lsint	nullOffset	= 0;

		// if there is a NULL in this string, set this offset
		// so that we ignore it in string size calculations
		if	( flags & ID3FF_NULL )
			nullOffset = -1;

		// +1 is for the NULL at the end and the
		// other +1 is for the list divider
		newLen = 1 + ( size / sizeof ( wchar_t ) ) + wcslen ( string ) + 1 + nullOffset;

		// I use the value 1 as a divider because then I
		// can change it to either a '/' or a NULL at render
		// time.  This allows easy use of these functions
		// for text lists or in the IPLS frame

		if	( temp = new wchar_t[ newLen ] )
		{
			wcscpy ( temp, (wchar_t *) data );
			temp[ ( size / sizeof ( wchar_t ) ) + nullOffset ] = L'\001';
			wcscpy ( &temp[ ( size / sizeof ( wchar_t ) ) + 1 + nullOffset ], string );

			Set ( temp );

			delete[] temp;
		}
		else
			ID3_THROW ( ID3E_NoMemory );
	}

	return;
}


// this is Get()

luint			ID3_Field::Get					( wchar_t *buffer, luint maxChars, luint itemNum )
{
	luint	charsUsed	= 0;

	// check to see if there is a string in the frame
	// to copy before we even try
	if	( data )
	{
		lsint	nullOffset	= 0;

		if	( flags & ID3FF_NULL )
			nullOffset = -1;

		// first we must find which element 
		// is being sought to make sure it
		// exists before we try to get it
		if	( itemNum <= GetNumTextItems() && itemNum > 0 )
		{
			wchar_t	*source		= (wchar_t *) data;
			luint	posn		= 0;
			luint	sourceLen	= 0;
			luint	curItemNum	= 1;

			// now we find that element and set the souvre pointer
			while	( curItemNum < itemNum )
			{
				while	( *source != L'\001' && *source != L'\0' && posn < ( ( size / sizeof ( wchar_t ) ) + nullOffset ) )
					source++, posn++;

				source++;
				curItemNum++;
			}

			// now that we are positioned at the first character
			// of the string we want, find the end of it
			while	( source[ sourceLen ] != L'\001' && source[ sourceLen ] != L'\0' && posn < ( ( size / sizeof ( wchar_t ) + nullOffset ) ) )
				sourceLen++, posn++;

			// we subtract 1 here so we have
			// room for the NULL terminator
			maxChars--;

			if	( buffer )
			{
				luint	actualChars		= MIN ( maxChars, sourceLen );

				wcsncpy ( buffer, source, actualChars );
				buffer[ actualChars ] = L'\0';
				charsUsed = actualChars;
			}
			else
				ID3_THROW ( ID3E_NoBuffer );
		}
	}

	return charsUsed;
}


luint			ID3_Field::GetNumTextItems		( void )
{
	luint	numItems	= 0;

	if	( data )
	{
		luint	posn	= 0;

		numItems++;

		while	( posn < size )
			if	( data[ posn++ ] == L'\001' )
				numItems++;
	}

	return numItems;
}


luint			ID3_Field::ParseUnicodeString	( uchar *buffer, luint posn, luint buffSize )
{
	luint	bytesUsed	= 0;
	wchar_t	*temp		= NULL;

	if	( fixedLength != -1 )
		bytesUsed = fixedLength;
	else
	{
		if	( flags & ID3FF_NULL )
			while	( ( posn + bytesUsed ) < buffSize &&
					  ! ( buffer[ posn + bytesUsed ] == 0 && buffer[ posn + bytesUsed + 1 ] == 0 ) )
				bytesUsed += 2;
		else
			bytesUsed = buffSize - posn;
	}

	if	( bytesUsed )
	{
		if	( temp = new wchar_t[ ( bytesUsed / sizeof ( wchar_t ) ) + 1 ] )
		{
			luint	loc	= 0;

			memcpy ( temp, &buffer[ posn ], bytesUsed );
			temp[ bytesUsed / sizeof ( wchar_t ) ] = 0;

			// if there is a BOM, skip past it and check to see if we
			// need to swap the byte order around
			if	( temp[ 0 ] == 0xFEFF || temp[ 0 ] == 0xFFFE )
			{
				loc++;

				// if we need to swap the byte order
				if	( temp[ 0 ] != 0xFEFF )
					for	( int i = loc; i < (int) wcslen ( temp ); i++ )
						temp[ i ] = ( ( temp[ i ] >> 8 ) & 0xFF ) | ( ( ( temp[ i ] ) & 0xFF ) << 8 );
			}

			Set ( &temp[ loc ] );

			delete[] temp;
		}
		else
			ID3_THROW ( ID3E_NoMemory );
	}

	if	( flags & ID3FF_NULL )
		bytesUsed += 2;

	hasChanged = false;

	return bytesUsed;
}


luint			ID3_Field::RenderUnicodeString	( uchar *buffer )
{
	luint	bytesUsed	= 0;

	bytesUsed = BinSize();

	if	( data && size && bytesUsed )
	{
		luint	i;
		wchar_t	*ourString	= (wchar_t *) &buffer[ sizeof ( wchar_t ) ];

		// we render at sizeof ( wchar_t ) bytes into the buffer
		// because we make room for the Unicode BOM
		memcpy ( &buffer[ sizeof ( wchar_t ) ], (uchar *) data, bytesUsed - sizeof ( wchar_t ) );

		// now we convert the internal dividers to what they
		// are supposed to be
		for	( i = 0; i < bytesUsed; i++ )
			if	( ourString[ i ] == 1 )
			{
				wchar_t	sub	=	L'/';

				if	( flags & ID3FF_NULLDIVIDE )
					sub = L'\0';

				ourString[ i ] = sub;
			}
	}

	if	( bytesUsed )
	{
		// render the BOM
		wchar_t	*BOM = (wchar_t *) buffer;
		BOM[ 0 ] = 0xFEFF;
	}

	if	( bytesUsed == 2 && ( flags & ID3FF_NULL ) )
		buffer[ 0 ] = buffer[ 1 ] = 0;

	hasChanged = false;

	return bytesUsed;
}


