// XTract.java

import java.io.*;
import java.net.*;
import java.util.*;

/**
 * This class can take a variable number of parameters on the command
 * line. Program execution begins with the main() method. The class
 * constructor is not invoked unless an object of type 'Class1'
 * created in the main() method.
 */
public class XTract
{
	/**
	 * The main entry point for the application. 
	 *
	 * @param args Array of parameters passed to the application
	 * via the command line.
	 */
	public static void main (String[] args)
	{
		String strSourceDir = new String(".");
		String strDestFile = null;

		for (int a = 0 ; a < args.length ; a++)
		{
			if (args[a].startsWith("/s:"))
			{
				strSourceDir = args[a].substring(3);	
			}
			else
			{
				if (strDestFile != null)
					ArgError();
				
				strDestFile = args[a];
			}
		}

		if (strDestFile == null) ArgError();
		
		CXTract cx = new CXTract();
		cx.ExtractEmailNames(strSourceDir, strDestFile);
	}
	
	static void ArgError()
	{
		System.err.println("Usage: XTract [/s:source-dir] dest-file");
		System.err.println("  Where");
		System.err.println("     /s = Source directory");
		System.err.println("     dest-dir = Desitination file");
		System.exit(1);
	}
}
