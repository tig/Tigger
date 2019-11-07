//CXTract.java
import java.io.*;
import java.net.*;
import java.util.*;

public class CXTract
{
	Hashtable m_hash = null;
	
	
	/**
	 * Process a single file
	 * 
	 * @param sInFile Input file
	 */
	void ProcessFile(String sInFile)
	{
		try
		{
			BufferedReader in = new BufferedReader(new InputStreamReader(new FileInputStream(sInFile)));

			String s = in.readLine();
			while (s != null)
			{
				if (s.startsWith("  <META NAME=\"MsgPropFrom\"") || s.startsWith("From:"))
				{
					//System.out.println(sInFile + ": " + s);
					// find the @
					int nStart = 0;
					int nAt = s.indexOf('@'); 
					int nEnd = 0;
					
					if (nAt != -1)
					{
						// find the start
						char[] rgc = s.toCharArray();
						
						for (nStart = nAt - 1; nStart > 0 ; nStart--)
						{
							if (rgc[nStart] == '\"' ||
								rgc[nStart] == '\'' ||
								rgc[nStart] == ' ' ||
								rgc[nStart] == '>' ||
								rgc[nStart] == '<' ||
								rgc[nStart] == '(' ||
								rgc[nStart] == ')' ||
								rgc[nStart] == '/' ||
								rgc[nStart] == '\\')
							{
								nStart++;
								break;
							}
						}

						// find end
						for (nEnd = nAt + 1; nEnd < s.length() -1 ; nEnd++)
						{
							if (rgc[nEnd] == '\"' ||
								rgc[nEnd] == '\'' ||
								rgc[nEnd] == ' ' ||
								rgc[nEnd] == '>' ||
								rgc[nEnd] == '<' ||
								rgc[nEnd] == '(' ||
								rgc[nEnd] == ')' ||
								rgc[nEnd] == '/' ||
								rgc[nEnd] == '\\')
							{
								nEnd--;
								break;
							}
						}

						if (nStart < nAt && nAt < nEnd)
						{
							String sEmail = s.substring(nStart, nEnd+1);
							String sEmailLower = sEmail.toLowerCase();
							if (!m_hash.containsKey(sEmailLower))
							{
								m_hash.put(sEmailLower, sEmail);
								// System.out.println("Put " + sEmail);
							}
						}
						else
						{
							System.out.println("Error parsing " + s);
						}
						
						break;
					}
				}
				
				s = in.readLine();
			}
			
			in.close();
		}
		catch (FileNotFoundException e)
		{
			System.err.println("Exception opening file: " + e);
			System.exit(0);
		}
		catch (IOException e)
		{
		    System.err.println("Exception:  " + e);
			System.exit(0);
		}

	}			
	
	/**
	* Processes a sub-directory recursively
	* 
	* @param sDir Directory
	 */
	void ProcessDir(String sDir)
	{
		System.out.println("Enumerating " + sDir);
		File fileRoot = new File(sDir);
		String rgsDir[] = fileRoot.list();

		if (rgsDir == null)
		{
			System.out.println("return");
			return;
		}
		
		for (int i = 0 ; i < rgsDir.length ; i++)
		{
			// Is it a file?
			if (rgsDir[i].endsWith(".txt") == true || rgsDir[i].endsWith(".htm") == true)
			{
				ProcessFile(sDir + File.separator + rgsDir[i]);
			}
			else
			{
				// Recurse
				ProcessDir(sDir + File.separator + rgsDir[i]);
			}
		}
	}
	
	
	public boolean ExtractEmailNames(String sFrom, String sTo)
	{
		System.out.println("Extracting email names from " + sFrom + " and writing them to " + sTo);
		
		try
		{
			m_hash = new Hashtable(8000);
			// Recurse directories, starting here. Read every file found and look for the "From:" line
			// 
			ProcessDir(sFrom);

			PrintWriter out = new PrintWriter(new BufferedWriter(new FileWriter(sTo)));

			for (Enumeration e = m_hash.elements() ; e.hasMoreElements() ;) {
				out.println(e.nextElement());
			}
	
			out.close();
		}
		catch (IOException e)
		{
		    System.err.println("Exception:  " + e);
		    return false;
		}
		
		return true;
	}
	
}
