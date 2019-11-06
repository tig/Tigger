    Set t = CreateObject("Kindel.ID3Tag")
   
    Set fs = CreateObject("Scripting.FileSystemObject")
    'Set folder = fs.GetFolder("C:\MP3")
    'For Each file In folder.Files
        t.Link "tones.mp3"
        ' t.Frames(id3fidENCODEDBY).field(id3fnTEXT) = "Charlie"
		' t.Strip 
        ' t.Update
		For Each frame In t.Frames
			Wscript.Echo frame.ID
			For Each field In frame
				Wscript.Echo field.ID & ":" & field.Value
			Next
		Next
    'Next 
