

Function GetFileName (Prompt : string; Ext : string; Row, Col : byte; var escaped : boolean) : string;
{this function gets a filename from the keyboard.  It will get a directory name}
{from the DOS Environment variable defined in the constant ENV_FILES.          }
{The calling parameters are:
             Prompt - A prompt string.
             Ext    - The default extension for the file name.
             Row,Col- The row and column to put the prompt.
                      (note that this funtion will write two lines!)
             Escaped- A boolean flag indicating whether the user hit Escape
                      or not.
}

  var
    path : string;
    name : string;

  begin
    CursorToEnd := False;                                     {this puts the cursor at the end of the edit line}
    ForceUpper := True;                                       {force the input to be uppercase}
    TrimBlanks := True;                                       {trim all excess blanks from the file name}
    name := '';
    path := AddBackslash (GetEnvironmentString (ENV_FILES));  {Get the environment string, and add a backslash to it}
    GotoXY (1,25);
    writeln ('Default Directory = ', path);
    ReadString (Prompt, Row+1,Col, 50, CYAN, LIGHTMAGENTA,
                WHITE, Escaped, name);                        {Read the filename, using the path as a prompt}
    if name <> '' then
      begin
       name := DefaultExtension (name, Ext);                  {add the appropriate extension if one is needed}
       GetFileName := path+name
      end
    else
       GetFileName := '';
  end;

