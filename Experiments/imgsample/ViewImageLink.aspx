<%@ Page language="c#" Codebehind="ViewImageLink.aspx.cs" Inherits="imgsample.ViewImageLink" %>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN" >
<HTML>
	<HEAD>
		<title>ViewImageLink</title>
		<meta content="Microsoft Visual Studio .NET 7.1" name="GENERATOR">
		<meta content="C#" name="CODE_LANGUAGE">
		<meta content="JavaScript" name="vs_defaultClientScript">
		<meta content="http://schemas.microsoft.com/intellisense/ie5" name="vs_targetSchema">
	</HEAD>
	<body MS_POSITIONING="GridLayout">
		<form id="Form1" method="post" runat="server">
			<a href='<%=GetFullImageURL() %>'>
				<asp:Image id="Image1" style="Z-INDEX: 101; LEFT: 9px; POSITION: absolute; TOP: 11px" runat="server"
					BorderStyle="Solid" BorderWidth="1"></asp:Image>
			</a>
		</form>
	</body>
</HTML>
