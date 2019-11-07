//DigestSplitter.java
package DigestSplitter;

import java.io.*;
import java.net.*;
import java.util.*;
import java.text.*;

public class DigestSplitter
{
	public String m_sOutFileExtension = ".htm";
	public String m_sStyleSheet = "/porsche/pfans2/archive/msg.css";
	
	public InputStream m_Stream = null;
	public String m_sOutDir = null;
	public String m_sRootFileName = null;
	public BufferedReader m_Reader = null;

	public DigestSplitter()
	{
	}
	
	public DigestSplitter(InputStream stm, String sOutDir, String sRootFileName)
	{
		m_Stream = stm;
		m_sOutDir = sOutDir;
		m_sRootFileName = sRootFileName;
	}
	
	/**
	 *  Read a header entry
	 * @param in Stream to read from
	 * @param start First line 
	 * @param key Key to check for 
	*/
	String ReadHeaderItem(String s, String key)
	{
		try
		{
			String u = s.toUpperCase();
			if (u.startsWith(key))
			{
				String out = new String(s);
				while (out.endsWith(","))
				{
					s = m_Reader.readLine();
					if (s != null)
						out = out + " " + s.trim();
				}
				return out;
			}
		}
		catch (IOException e)
		{
			System.err.println("ReadHeaderItem exception: " + e);
		}
		return null;
	}
	
	/**
	 * Returns a string escaped appropriately for inclusion in a quoted
	 * string.
	 * @param in String to escape
	 * @return Escaped string
	 */
	String QuotedStringEscape(String in)
	{
		StringBuffer out = new StringBuffer();
		
		for (int i = 0; i < in.length() ; i++)
		{
			char c = in.charAt(i);
			switch (c)
			{
				case '"':
				{
					out.append("'");
				}
				break;
				
				default:
				{
					out.append(c);
				}
			}
		}
			
		return out.toString().trim();
	}
	
	/**
	 * Returns a string escaped appropriately for inclusion in HTML.
	 * @param in String to escape
	 * @return Escaped string
	 */
	String HTMLEscape(String in)
	{
		StringBuffer out = new StringBuffer();
		
		for (int i = 0; i < in.length() ; i++)
		{
			char c = in.charAt(i);
			switch (c)
			{
				case '<':
				{
					out.append("&lt;");
				}
				break;
				
				case '>':
				{
					out.append("&gt;");
				}
				break;
				
				case '"':
				{
					// we don't need no quotes!
				}
				break;
				
				default:
				{
					out.append(c);
				}
			}
		}
			
		return out.toString().trim();
	}

	void  WriteProperty(PrintWriter out, String line)
	{
		if (line == null)
			return;
		
		int colon = line.indexOf(":");
		
		// BUGBUG:	Not sure if this is the right format for a document
		//			property meta element. 1) should NAME be DocumentProperty?
		//			2) Should TITLE be the property name?
		//
		out.println("  <META NAME=\"MsgProp" + line.substring(0, colon) + 
					"\" CONTENT=\"" + QuotedStringEscape(line.substring(colon+1, line.length())) + 
					"\">");

	}
	
	void  WriteMsgHeaderItem(PrintWriter out, String line)
	{
		if (line == null)
			return;
		
		int colon = line.indexOf(":")+1;
		out.println("<tr><td class=\"HeaderItemLabel\">" + line.substring(0, colon) + 
					"</td><td class=\"HeaderItemContent\">" + HTMLEscape(line.substring(colon, line.length())) + 
					"</td></tr>");
	}
	
	
	/**
	 * Processes a digest, creating sub-directories by year. Each
	 * year sub-directory will contain a file for each message sent in that
	 * year.
	 * 
	 * As we write out the individual files we set their modify date to
	 * the date the message was sent.
	 */
	public void Process()
	{
		System.out.println("Splitting digest to " + m_sOutDir);
		m_Reader = new BufferedReader(new InputStreamReader(m_Stream));

		try
		{
			String sTo = null;
			String sFrom = null;
			String sSubject = null;
			String sDate  = null;
			String sCC = null;
			String sReplyTo = null;
			String sOrg = null;
			
			String s = m_Reader.readLine();
			
			int fileNum = 0;
			
			while (s != null)
			{
				// Read header. Thow away anything we don't care about. First blank line
				// is the start of the message.
				if (s.length() > 0)
				{
					if (sFrom == null) sFrom = ReadHeaderItem(s, "FROM:");
					if (sTo == null) sTo = ReadHeaderItem(s, "TO:");
					if (sSubject == null) sSubject = ReadHeaderItem(s, "SUBJECT:");
					if (sDate == null) sDate = ReadHeaderItem(s, "DATE:");
					if (sCC == null) sCC = ReadHeaderItem(s, "CC:");
					if (sReplyTo == null) sReplyTo = ReadHeaderItem(s, "REPLY-TO:");
					if (sReplyTo == null) sReplyTo = ReadHeaderItem(s, "IN-REPLY-TO:");
					if (sOrg == null) sOrg = ReadHeaderItem(s, "ORGANIZATION:");
				}
				else
				{
					// Message body. Create the file and dump the header
					NumberFormat fmt = NumberFormat.getInstance();
					fmt.setMinimumIntegerDigits(3);
					String sOutFile = m_sOutDir + File.separator + m_sRootFileName + fmt.format(fileNum) + m_sOutFileExtension;
					System.out.println("Creating file " + sOutFile);
					PrintWriter out = new PrintWriter(new BufferedWriter(new FileWriter(sOutFile)),false);
					
					out.println("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 3.2 Final//EN\">");
					out.println("<HTML><HEAD>");
					if (sSubject == null) // it's possible (likely) that the subject will be missing
						out.println("<TITLE>Mailing List Message - (no subject)</TITLE>");
					else
						out.println("<TITLE>Mailing List Message - " + 
								sSubject.substring(sSubject.indexOf(":")+1, sSubject.length()) + "</TITLE>");
					out.println("  <META NAME=\"GENERATOR\" Content=\"DigestSplitter\">");
					out.println("  <META HTTP-EQUIV=\"Content-Type\" content=\"text/html; charset=iso-8859-1\">");
					out.println("  <LINK REL=StyleSheet HREF=\"" + m_sStyleSheet + "\" TYPE=\"text/css\" MEDIA=screen>");
					
					WriteProperty(out, sTo);
					WriteProperty(out, sFrom);
					WriteProperty(out, sSubject);
					WriteProperty(out, sDate);
					WriteProperty(out, sCC);
					WriteProperty(out, sOrg);
					WriteProperty(out, sReplyTo);
					
					out.println("</HEAD><BODY>");
					
					out.println("<table>");

					WriteMsgHeaderItem(out, sTo);
					WriteMsgHeaderItem(out, sFrom);
					WriteMsgHeaderItem(out, sSubject);
					WriteMsgHeaderItem(out, sDate);
					WriteMsgHeaderItem(out, sCC);
#if INCLUDE_REPLYTO
					WriteMsgHeaderItem(out, sOrg);
					WriteMsgHeaderItem(out, sReplyTo);
#endif
					out.println("</table>");

					out.println("<pre>");
					s = m_Reader.readLine();
					while (s != null && !s.startsWith("Received:"))
					{
						out.println(s);
						s = m_Reader.readLine();
					}
					out.println("</pre>");

					out.println("</BODY></HTML>");
					out.flush();

					// Prepare for next header.
					fileNum++;
					sTo = null;
					sFrom = null;
					sSubject = null;
					sDate  = null;
					sCC = null;
					sReplyTo = null;
					sOrg = null;
				}
				s = m_Reader.readLine();
			}
		}
		catch (IOException e)
		{
			System.err.println("Exception Processing Digest: " + e);
			System.exit(0);
		}			
	}

}
