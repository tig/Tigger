<%@ Page language="c#" Codebehind="default.aspx.cs" AutoEventWireup="false" Inherits="imgsample._default" %>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN" >
<HTML>
	<HEAD>
		<title>default</title>
		<meta content="Microsoft Visual Studio .NET 7.1" name="GENERATOR">
		<meta content="C#" name="CODE_LANGUAGE">
		<meta content="JavaScript" name="vs_defaultClientScript">
		<meta content="http://schemas.microsoft.com/intellisense/ie5" name="vs_targetSchema">
	</HEAD>
	<body MS_POSITIONING="GridLayout">
		<form id="Form1" method="post" runat="server">
			<asp:label id="lblGalleryName" style="Z-INDEX: 101; LEFT: 15px; POSITION: absolute; TOP: 13px"
				runat="server">Photo Gallery</asp:label><asp:repeater id="repeater" runat="server">
				<HeaderTemplate>
					<table width="100%" style="font: 8pt verdana">
				</HeaderTemplate>
				<ItemTemplate>
					<tr>
						<td align="center">
							<A href='<%# GetFullImageURL(DataBinder.Eval(Container.DataItem, "img_dir").ToString(), DataBinder.Eval(Container.DataItem, "img_name").ToString(),DataBinder.Eval(Container.DataItem, "img_contenttype").ToString()) %>'>
								<asp:Image id=Image2 Runat="server" ImageUrl='<%# GetMedURL(DataBinder.Eval(Container.DataItem, "img_pk").ToString()) %>' BorderWidth="1">
								</asp:Image>
							</A>
							<br>
								<%# DataBinder.Eval(Container.DataItem, "img_desc") %>
							</br>
							<br>
								<%# DataBinder.Eval(Container.DataItem, "img_datepicturetaken") %>
							</br>
						</td>
					</tr>
				</ItemTemplate>
				<FooterTemplate>
					</table>
				</FooterTemplate>
			</asp:repeater></form>
	</body>
</HTML>
