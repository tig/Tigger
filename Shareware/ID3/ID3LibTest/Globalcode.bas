Attribute VB_Name = "Globalcode"
' VisID3 - A Visual ID3 Editor
'
' Copyright (c) 1999 Charles E. Kindel, Jr.
' All Rights Reserved
' See http://www.kindel.com/mp3/id3com for more information
'
Public rgFrameID
Public rgFrameIDDesc

Public Sub InitFrameIDArrays()
    rgFrameID = Array("AENC", "APIC", "COMM", "COMR", "ENCR", "EQUA", "ETCO", "GEOB", "GRID", "IPLS", "LINK", "MCDI", "MLLT", "OWNE", "PRIV", "PCNT", "POPM", "POSS", "RBUF", "RVAD", "RVRB", "SYLT", "SYTC", "TALB", "TBPM", "TCOM", "TCON", "TCOP", "TDAT", "TDLY", "TENC", "TEXT", "TFLT", "TIME", "TIT1", "TIT2", "TIT3", "TKEY", "TLAN", "TLEN", "TMED", "TOAL", "TOFN", "TOLY", "TOPE", "TORY", "TOWN", "TPE1", "TPE2", "TPE3", "TPE4", "TPOS", "TPUB", "TRCK", "TRDA", "TRSN", "TRSO", "TSIZ", "TSRC", "TSSE", "TYER", "UFID", "USER", "USLT", "WCOM", "WCOP", "WOAF", "WOAR", "WOAS", "WORS", "WPAY", "WPUB", "TXXX", "WXXX")
    rgFrameIDDesc = Array("Audio encryption", "Attached picture", "Comments", "Commercial frame", "Encryption method registration", "Equalization", "Event timing codes", "General encapsulated object", "Group identification registration", "Involved people list", "Linked information", "Music CD identifier", "MPEG location lookup table", "Ownership frame", "Private frame", "Play counter", "Popularimeter", "Position synchronisation frame", "Recommended buffer size", "Relative volume adjustment", "Reverb", "Synchronized lyric/text", "Synchronized tempo codes", "Album/Movie/Show title", "BPM (beats per minute)", _
                        "Composer", "Content type", "Copyright message", "Date", "Playlist delay", "Encoded by", "Lyricist/Text writer", "File type", "Time", "Content group description", "Title/songname/content description", "Subtitle/Description refinement", "Initial key", "Language(s)", "Length", "Media type", "Original album/movie/show title", "Original filename", "Original lyricist(s)/text writer(s)", "Original artist(s)/performer(s)", "Original release year", "File owner/licensee", "Lead performer(s)/Soloist(s)", "Band/orchestra/accompaniment", "Conductor/performer refinement", _
                        "Interpreted, remixed, or otherwise modified by", "Part of a set", "Publisher", "Track number/Position in set", "Recording dates", "Internet radio station name", "Internet radio station owner", "Size", "ISRC (international standard recording code)", "Software/Hardware and settings used for encoding", "Year", "Unique file identifier", "Terms of use", "Unsychronized lyric/text transcription", "Commercial information", "Copyright/Legal information", "Official audio file webpage", "Official artist/performer webpage", "Official audio source webpage", "Official internet radio station homepage", _
                        "Payment", "Publishers official webpage", "User defined text information frame", "User defined URL link frame")

End Sub
Public Function GetFrameIDDesc(frameid As String) As String
    Dim Desc As String
    
    Select Case frameid
        Case ""
            Desc = "[No Frame Id!]"

        Case "AENC"
            Desc = "Audio encryption"

        Case "APIC"
            Desc = "Attached picture"

        Case "COMM"
            Desc = "Comments"

        Case "COMR"
            Desc = "Commercial frame"

        Case "ENCR"
            Desc = "Encryption method registration"

        Case "EQUA"
            Desc = "Equalization"

        Case "ETCO"
            Desc = "Event timing codes"

        Case "GEOB"
            Desc = "General encapsulated object"

        Case "GRID"
            Desc = "Group identification registration"

        Case "IPLS"
            Desc = "Involved people list"

        Case "LINK"
            Desc = "Linked information"

        Case "MCDI"
            Desc = "Music CD identifier"

        Case "MLLT"
            Desc = "MPEG location lookup table"

        Case "OWNE"
            Desc = "Ownership frame"

        Case "PRIV"
            Desc = "Private frame"

        Case "PCNT"
            Desc = "Play counter"

        Case "POPM"
            Desc = "Popularimeter"

        Case "POSS"
            Desc = "Position synchronisation frame"

        Case "RBUF"
            Desc = "Recommended buffer size"

        Case "RVAD"
            Desc = "Relative volume adjustment"

        Case "RVRB"
            Desc = "Reverb"

        Case "SYLT"
            Desc = "Synchronized lyric/text"

        Case "SYTC"
            Desc = "Synchronized tempo codes"

        Case "TALB"
            Desc = "Album/Movie/Show title"

        Case "TBPM"
            Desc = "BPM (beats per minute)"

        Case "TCOM"
            Desc = "Composer"

        Case "TCON"
            Desc = "Content type"

        Case "TCOP"
            Desc = "Copyright message"

        Case "TDAT"
            Desc = "Date"

        Case "TDLY"
            Desc = "Playlist delay"

        Case "TENC"
            Desc = "Encoded by"

        Case "TEXT"
            Desc = "Lyricist/Text writer"

        Case "TFLT"
            Desc = "File type"

        Case "TIME"
            Desc = "Time"

        Case "TIT1"
            Desc = "Content group description"

        Case "TIT2"
            Desc = "Title/songname/content description"

        Case "TIT3"
            Desc = "Subtitle/Description refinement"

        Case "TKEY"
            Desc = "Initial key"

        Case "TLAN"
            Desc = "Language(s)"

        Case "TLEN"
            Desc = "Length"

        Case "TMED"
            Desc = "Media type"

        Case "TOAL"
            Desc = "Original album/movie/show title"

        Case "TOFN"
            Desc = "Original filename"

        Case "TOLY"
            Desc = "Original lyricist(s)/text writer(s)"

        Case "TOPE"
            Desc = "Original artist(s)/performer(s)"

        Case "TORY"
            Desc = "Original release year"

        Case "TOWN"
            Desc = "File owner/licensee"

        Case "TPE1"
            Desc = "Lead performer(s)/Soloist(s)"

        Case "TPE2"
            Desc = "Band/orchestra/accompaniment"

        Case "TPE3"
            Desc = "Conductor/performer refinement"

        Case "TPE4"
            Desc = "Interpreted, remixed, or otherwise modified by"

        Case "TPOS"
            Desc = "Part of a set"

        Case "TPUB"
            Desc = "Publisher"

        Case "TRCK"
            Desc = "Track number/Position in set"

        Case "TRDA"
            Desc = "Recording dates"

        Case "TRSN"
            Desc = "Internet radio station name"

        Case "TRSO"
            Desc = "Internet radio station owner"

        Case "TSIZ"
            Desc = "Size"

        Case "TSRC"
            Desc = "ISRC (international standard recording code)"

        Case "TSSE"
            Desc = "Software/Hardware and settings used for encoding"

        Case "TYER"
            Desc = "Year"

        Case "UFID"
            Desc = "Unique file identifier"

        Case "USER"
            Desc = "Terms of use"

        Case "USLT"
            Desc = "Unsychronized lyric/text transcription"

        Case "WCOM"
            Desc = "Commercial information"

        Case "WCOP"
            Desc = "Copyright/Legal information"

        Case "WOAF"
            Desc = "Official audio file webpage"

        Case "WOAR"
            Desc = "Official artist/performer webpage"

        Case "WOAS"
            Desc = "Official audio source webpage"

        Case "WORS"
            Desc = "Official internet radio station homepage"

        Case "WPAY"
            Desc = "Payment"

        Case "WPUB"
            Desc = "Publishers official webpage"
        
        
        Case Else
            If Left(frameid, 1) = "T" Then
                Desc = "User defined text information frame"
            Else
                If Left(frameid, 1) = "W" Then
                    Desc = "User defined URL link frame"
                Else
                    Desc = "Unrecognized frame"
                End If
            End If
    End Select
    
    GetFrameIDDesc = Desc & " (" & frameid & ")"
End Function

Public Function GetFieldIDDesc(fieldid As ID3LibFieldID) As String
    Dim Desc As String
    Select Case fieldid
        Case id3NOFIELD
            Desc = "Invalid Field"
        Case id3TEXTENC
            Desc = "Text encoding"
        Case id3TEXT
            Desc = "Text"
        Case id3URL
            Desc = "URL"
        Case id3DATA
            Desc = "Data"
        Case id3DESCRIPTION
            Desc = "Description"
        Case id3OWNER
            Desc = "Owner"
        Case id3EMAIL
            Desc = "Email"
        Case id3RATING
            Desc = "Rating"
        Case id3FILENAME
            Desc = "Filename"
        Case id3LANGUAGE
            Desc = "Language"
        Case id3PICTURETYPE
            Desc = "Picture type"
        Case id3IMAGEFORMAT
            Desc = "Image format"
        Case id3MIMETYPE
            Desc = "MIME type"
        Case id3COUNTER
            Desc = "Counter"
        Case id3SYMBOL
            Desc = "Symbol"
        Case id3VOLUMEADJ
            Desc = "Volume adjustment"
        Case id3NUMBITS
            Desc = "Volume adjustment bits"
        Case id3VOLCHGRIGHT
            Desc = "Volume change, right"
        Case id3VOLCHGLEFT
            Desc = "Volume change, left"
        Case id3PEAKVOLRIGHT
            Desc = "Volume peak, right"
        Case id3PEAKVOLLEFT
            Desc = "Volume peak, right"
        Case Else
            Desc = "Invalid Field"
    End Select
    
    GetFieldIDDesc = Desc & " (" & Format(fieldid) & ")"
End Function


Public Function ChooseFrameType() As String
    ChooseFrameType = ""
    dlgChooseFrameID.Show vbModal
    If dlgChooseFrameID.fOKPressed Then
        ChooseFrameType = dlgChooseFrameID.list.SelectedItem.Text
    End If
    Unload dlgChooseFrameID
End Function

