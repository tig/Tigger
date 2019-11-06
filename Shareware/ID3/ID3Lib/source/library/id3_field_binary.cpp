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
#include <memory.h>
#include "id3_field.h"


void			ID3_Field::Set					( uchar *newData, luint newSize )
{
	Clear();

	if	( newSize )
	{
		if	( ! ( data = new uchar[ newSize ] ) )
			ID3_THROW ( ID3E_NoMemory );

		memcpy ( data, newData, newSize );
		size = newSize;

		type = ID3FTY_BINARY;
		hasChanged = true;
	}

	return;
}


void			ID3_Field::Get					( uchar *buffer, luint buffLength )
{
	if	( ! buffer )
		ID3_THROW ( ID3E_NoBuffer );

	if	( data && size )
	{
		luint	actualBytes	= MIN ( buffLength, size );

		memcpy ( buffer, data, actualBytes );
	}

	return;
}


void			ID3_Field::FromFile				( char *info )
{
	FILE	*temp;
	luint	fileSize;
	uchar	*buffer;

	if	( ! info )
		ID3_THROW ( ID3E_NoData );

	if	( temp = fopen ( info, "rb" ) )
	{
		fseek ( temp, 0, SEEK_END );
		fileSize = ftell ( temp );
		fseek ( temp, 0, SEEK_SET );

		if	( buffer = new uchar[ fileSize ] )
		{
			fread ( buffer, 1, fileSize, temp );

			Set ( buffer, fileSize );

			delete[] buffer;
		}

		fclose ( temp );
	}

	return;
}


void			ID3_Field::ToFile				( char *info )
{
	if	( ! info )
		ID3_THROW ( ID3E_NoData );

	if	( ( data != NULL ) && ( size > 0 ) )
	{
		FILE	*temp;

		if	( temp = fopen ( info, "wb" ) )
		{
			fwrite ( data, 1, size, temp );
			fclose ( temp );
		}
	}

	return;
}


luint			ID3_Field::ParseBinary			( uchar *buffer, luint posn, luint buffSize )
{
	luint	bytesUsed	= 0;

	bytesUsed = buffSize - posn;

	if	( fixedLength != -1 )
		bytesUsed = MIN ( fixedLength, bytesUsed );

	Set ( &buffer[ posn ], bytesUsed );

	hasChanged = false;

	return bytesUsed;
}


luint			ID3_Field::RenderBinary			( uchar *buffer )
{
	luint	bytesUsed	= 0;

	bytesUsed = BinSize();
	memcpy ( buffer, (uchar *) data, bytesUsed );

	hasChanged = false;

	return bytesUsed;
}


