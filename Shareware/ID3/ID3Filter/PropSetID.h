// Copyright (c) 1998 Charles E. Kindel, Jr.
// 
//
// ID3v2 PropertySet/Property ID Definitions
//
// ID3v2 has been reserved the following UUID range for
// representing ID3v2 frame/fields as COM docfile properties.
//
//	xxxxxxxx-0001-0000-C000-000000000046 
//
// This reservation was made by Charlie Kindel of Microsoft.
//
// We could have used generated UUIDs for this (using the
// uuidgen tool), but reserved ones are easier on the eyes and
// since Charlie's on the "inside" we can do it :-).
//
// ID3v2 uses 4 character string names to identify frames. Only
// the upper case characters A..Z and 0..9 may be used. This 
// means that a frame ID can be represented in 24 bits (because
// there are 36 possibilities (6 bits) for each of the 4 character
// positions). We could encode these 24 bits in the UUID, but instead
// I choose to reserve a chunk of UUIDs with 32 bits available, 
// allowing for encoding the frameID using the ASCII codes for
// the characters. This will make a given PropertyID UUID much
// more recongnizable as a given FrameID (to a human).
//
// This file defines a C++ class for converting
// between ID3 FieldIDs and COM docfile PROPIDs.
//
// Example usage:
//
//	int wmain(int argc, WCHAR* argv[])
//  {
//  	CFieldID id;
//  	UUID PropID;
//  	WCHAR szUUID[50];
//
//  	CoInitialize(NULL);
//  	if (argc > 1)
//  	{
//			// Convert FieldID on command line to 
//			// A propID
//  		id = argv[1];
//  		PropID = id;
//
//			// Or, just cast:
//			PropID = (CField)argv[1];
//
//			// The other way
//			const WCHAR* pwszID = (const WCHAR*)(CField)PropID;
//
//			// print out PropID (uuid)
//			StringFromGUID2(PropID, szUUID, 50);
//			printf("Encoded %S as %S\n", argv[1], szUUID);
//
//			// print out reverse
//			printf("Decoded %S From: %S\n", pwszID, szUUID);
//
//		}
//		return 0;
//	}
//  
// This program's output, given "DEF3" on the command line would be:
//
//		Encoded DEF3 as {33464544-0001-0000-C000-000000000046}
//		Decoded DEF3 From: {33464544-0001-0000-C000-000000000046}
//
// {01000000-0000-0000-C000-000000000046}
const UUID UUID_ID3v2PropSetBase = 
{
    0x00000000,
    0x0001,
    0x0000,
    { 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46 }
};

class CFieldID
{
public:
	CFieldID();
	CFieldID(const WCHAR* pwszFieldID);
	CFieldID(const CHAR* pszFieldID);
	CFieldID(const UUID& refPropSetID);
	~CFieldID();

	BOOL operator==(const CFieldID& Other) const;
	const CFieldID& operator=(const WCHAR* pwszFieldID);
	const CFieldID& operator=(const CHAR* pszFieldID);
	const CFieldID& operator=(const UUID& uuid);

	operator UUID() const;
	operator const WCHAR*() const;

private:
	ULONG	m_ulEncodedID;
	WCHAR   m_wszFieldID[5] ;

	void EncodeFieldID(const WCHAR* pwszFieldID);
	void EncodePropID(const UUID& refPropSetID);

	UUID GetPropSetID() const;
	WCHAR* GetFieldID() const;
};


