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


#ifndef	ID3LIB_TYPES_28BITINT_H
#define	ID3LIB_TYPES_28BITINT_H


#include <iostream.h>
#include "id3_types.h"


class int28
{
public:
				int28							( luint val = 0 );
				int28							( uchar *val );

uchar			operator[]						( luint posn );
friend ostream&	operator<<						( ostream& out, int28& val );
luint			get								( void );

// *** PRIVATE INTERNAL DATA - DO NOT USE *** PRIVATE INTERNAL DATA - DO NOT USE ***

protected:
void			set								( luint val );
uchar			value[ sizeof ( luint ) ];		// the integer stored as a uchar array
};


ostream&		operator<<						( ostream& out, int28& val );
istream&		operator>>						( istream& in, int28& val );


#endif


