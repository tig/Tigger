<%@ Page language="c#" Codebehind="UploadImage.aspx.cs" AutoEventWireup="false" Inherits="imgsample.UploadImage" %>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN" >
<HTML>
	<HEAD>
		<title>UploadImage</title>
		<meta name="vs_snapToGrid" content="False">
		<meta content="Microsoft Visual Studio .NET 7.1" name="GENERATOR">
		<meta content="C#" name="CODE_LANGUAGE">
		<meta content="JavaScript" name="vs_defaultClientScript">
		<meta content="http://schemas.microsoft.com/intellisense/ie5" name="vs_targetSchema">
	</HEAD>
	<body MS_POSITIONING="GridLayout">
		<form id="form1" name="form1" encType="multipart/form-data" runat="server">
			Enter A Friendly Name <input id="txtImgName" type="text" runat="server"><BR>
			<asp:requiredfieldvalidator id="RequiredFieldValidator1" runat="server" ErrorMessage="Required" ControlToValidate="txtImgName"></asp:requiredfieldvalidator><BR>
			<BR>
			<br>
			Select File To Upload:&nbsp;
			<BR>
			<input id="UploadFile" type="file" runat="server" style="WIDTH: 514px; HEIGHT: 22px" size="66"><BR>
			<asp:button id="UploadBtn" onclick="UploadBtn_Click" runat="server" Text="Upload Me!"></asp:button><BR>
		</form>
	</body>
</HTML>
