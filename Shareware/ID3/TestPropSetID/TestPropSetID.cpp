// TestPropSetID.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "../ID3Filter/PropSetID.h"

WCHAR IntToChar(int i)
{
	WCHAR c = 0;

	if (i < 26)
		c = i + L'A';
	else
		c = i - 26 + L'0';

	return c;
}


int wmain(int argc, WCHAR* argv[])
{
	CoInitialize(NULL);
	CFieldID id;
	UUID uuid;
	WCHAR szUUID[50];

	if (argc > 1)
	{
		id = argv[1];
		
		uuid = (CFieldID)argv[1];

		StringFromGUID2(uuid, szUUID, 50);
		printf("Encoded %S as %S\n", argv[1], szUUID);

		const WCHAR* pwszID = (const WCHAR*)(CFieldID)uuid;
		printf("Decoded %S From: %S\n", pwszID, szUUID);
	}
	else
	{
		WCHAR FieldID[5];
		const WCHAR* pcsz = NULL;

		FieldID[4] = L'\0';

		for (int i1 = 0 ; i1 < 36 ; i1++)
		{
			FieldID[0] = IntToChar(i1);
			
			for (int i2 = 0 ; i2 < 36 ; i2++)
			{
				FieldID[1] = IntToChar(i2);

				for (int i3 = 0 ; i3 < 36 ; i3++)
				{
					FieldID[2] = IntToChar(i3);

					for (int i4 = 0 ; i4 < 36 ; i4++)
					{
						FieldID[3] = IntToChar(i4);
						uuid = (CFieldID)FieldID;
						pcsz = (const WCHAR*)(CFieldID)uuid;

#if 0
						StringFromGUID2(uuid, szUUID, 50);
						printf("%S == %S (%S)\n", FieldID, pcsz, szUUID);
#endif
						if (wcscmp(FieldID, pcsz) != 0)
						{
							printf("Failure to compare: %S\n", FieldID);
							return 0;
						}
					}
				}
			}

		}
	}

	return 0;
}
