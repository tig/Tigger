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


#include <iostream.h>
#include "id3_tag.h"


int				main							( int argc, char *argv[] )
{
	cout << "ID3v2 Tag Converter - No copyright 1998 Dirk Mahoney" << endl;
	cout << "Uses " << ID3LIB_NAME << " " << ID3LIB_VERSION << endl << endl;

	cout << "This program converts ID3v1/1.1 and Lyrics3 v2.0" << endl;
	cout << "tags to ID3v2 tags.  This program will also remove" << endl;
	cout << "the old tags." << endl << endl;

	if	( argc > 1 )
	{
		try
		{
			ID3_Tag	myTag;
			luint	argNum	= 1;

			if	( argv[ 1 ][ 0 ] == '-' )
			{
				argNum++;
				myTag.SetVersion ( argv[ 1 ][ 1 ] - '0', 0 );
			}

			myTag.Link ( argv[ argNum ] );
			myTag.Strip();
			myTag.Update();
		}

		catch ( ID3_Error err )
		{
			cout << "Error in ID3Lib call: '" << err.GetErrorDesc() << "'" << endl;
		}
	}
	else
	{
		cout << "Usage: ID3Convert [-version] <file>" << endl << endl;

		cout << "Where: * 'version' specifies which version of the ID3v2 tag" << endl;
		cout << "         standard to which you wish to convert.  Valid numbers" << endl;
		cout << "         and 2 and 3 (default is 3)." << endl;
		cout << "       * 'file' is the file you wish to convert." << endl << endl;
	}

	return 0;
}


