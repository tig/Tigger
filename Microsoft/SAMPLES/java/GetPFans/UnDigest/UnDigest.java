// Class1.java
import java.io.*;
import java.net.*;
import java.util.*;

import DigestSplitter.*;

/**
 * This class can take a variable number of parameters on the command
 * line. Program execution begins with the main() method. The class
 * constructor is not invoked unless an object of type 'Class1'
 * created in the main() method.
 */
public class UnDigest
{
	public static void ShowHelp()
	{
		System.err.println("Usage: UnDigest src-dir [/h][/d:dest-dir]");
		System.err.println("  Where");
		System.err.println("     src-dir = Source directory or file.");
		System.err.println("     /h = Help");
		System.err.println("     /d = Desitination directory");
		System.exit(1);
	}

	/**
	 * Process a single file
	 * 
	 * @param sInFile Input file
	 * @param sOutRoot Output directory
	 */
	static void ProcessFile(String sInFile, String sOutRoot)
	{
		try
		{
			// Strip .txt from sInFile
			int nStart = sInFile.lastIndexOf(File.separator);
			if (nStart > 0) 
				nStart++;
			String sNoTxt = sInFile.substring(nStart, sInFile.lastIndexOf(".txt")) + "-";

			// Append year to output directory
			String sTmp = new String(sNoTxt.substring(sNoTxt.length() - 7, sNoTxt.length()-5));
			if (Integer.parseInt(sTmp) < 50) 
				sTmp = "20" + sTmp;
			else
				sTmp = "19" + sTmp;
			sOutRoot = sOutRoot + File.separator + sTmp;

			// Append month to output directory
			sTmp = new String(sNoTxt.substring(sNoTxt.length() - 5, sNoTxt.length()-3));
			sOutRoot = sOutRoot + File.separator + sTmp;

			// Create the directory
			File f = new File(sOutRoot);
			f.mkdirs();
			
			InputStream stm = new FileInputStream(sInFile);
			DigestSplitter splitter = new DigestSplitter(stm, sOutRoot, sNoTxt);
			splitter.Process();
		}
		catch (FileNotFoundException e)
		{
			System.err.println("Exception opening file: " + e);
			System.exit(0);
		}
	}			
		
	/**
	* Processes a sub-directory recursively
	* 
	* @param sDir Directory
	* @param sOutDir Output directory
	 */
	public static void ProcessDir(String sDir, String sOutDir)
	{
		System.out.println("Enumerating " + sDir + " to " + sOutDir);
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
			if (rgsDir[i].endsWith(".txt") == true)
			{
				ProcessFile(sDir + File.separator + rgsDir[i], sOutDir);
			}
			else
			{
				// Recurse
				ProcessDir(sDir + File.separator + rgsDir[i], sOutDir + File.separator + rgsDir[i]);
			}
		}
	}

	/**
	 * The main entry point for the application. 
	 *
	 * @param args Array of parameters passed to the application
	 * via the command line.
	 */
	public static void main (String[] args)
	{
		String sInFile = null;
		String sInDir = null;
		File fileCurrentDir = new File(".");
		String sOutRoot = fileCurrentDir.getAbsolutePath();
		
		for (int a = 0 ; a < args.length ; a++)
		{
			StringTokenizer st = new StringTokenizer(args[a], ":");
			String cmd;
			if (st.hasMoreTokens()) 
			{         
				cmd = st.nextToken();
				if (cmd.compareTo("/h") == 0)
				{
					ShowHelp();
					continue;
				}
				else if (cmd.compareTo("/d") == 0)
				{
					sOutRoot = args[a].substring(3);
					//if (!sOutRoot.endsWith(File.separator))
					//	sOutRoot = sOutRoot + File.separator;
					continue;
				}
				else
				{
					sInFile = cmd;
				}
			}			
		}
		if (sInFile == null)
			ShowHelp(); // will exit
		
		System.out.println("Src = " + sInFile + ", Dest = " + sOutRoot);

		// If only one file was specified, simply process it and exit
		File f = new File(sInFile);
		if (f.isFile())
		{
			ProcessFile(sInFile, sOutRoot);
		}
		else
		{		
			// Enumerate through all *.txt files in all sub-directories of current directory
			// and Process each.
			// 
			ProcessDir(sInFile, sOutRoot);
		}

		System.exit(1);
	}
}

