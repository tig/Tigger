<%@ Page language="c#" Codebehind="GenerateImages.aspx.cs" AutoEventWireup="false" Inherits="imgsample.GenerateImages" %>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN" >
<HTML>
	<HEAD>
		<title>GenerateImages</title>
		<meta content="Microsoft Visual Studio .NET 7.1" name="GENERATOR">
		<meta content="C#" name="CODE_LANGUAGE">
		<meta content="JavaScript" name="vs_defaultClientScript">
		<meta content="http://schemas.microsoft.com/intellisense/ie5" name="vs_targetSchema">
	</HEAD>
	<body MS_POSITIONING="GridLayout">
		<form id="Form1" method="post" runat="server">
			<asp:button id="Button1" style="Z-INDEX: 101; LEFT: 8px; POSITION: absolute; TOP: 8px" runat="server"
				Width="184px" Text="Generate Small Images"></asp:button><asp:datagrid id=DataGrid1 style="Z-INDEX: 102; LEFT: 16px; POSITION: absolute; TOP: 64px" runat="server" AutoGenerateColumns="False" DataMember="image" AllowSorting="True" DataKeyField="img_pk" DataSource="<%# dataSet %>">
				<Columns>
					<asp:BoundColumn DataField="img_pk" SortExpression="img_pk" HeaderText="img_pk"></asp:BoundColumn>
					<asp:BoundColumn DataField="img_name" SortExpression="img_name" HeaderText="img_name"></asp:BoundColumn>
					<asp:BoundColumn DataField="img_dir" SortExpression="img_dir" HeaderText="img_dir"></asp:BoundColumn>
					<asp:BoundColumn DataField="img_desc" SortExpression="img_desc" HeaderText="img_desc"></asp:BoundColumn>
					<asp:BoundColumn DataField="img_datepicturetaken" SortExpression="img_datepicturetaken" HeaderText="img_datepicturetaken"></asp:BoundColumn>
					<asp:BoundColumn DataField="img_contenttype" SortExpression="img_contenttype" HeaderText="img_contenttype"></asp:BoundColumn>
					<asp:TemplateColumn HeaderText="Image">
						<ItemTemplate>
							<a href='<%# GetLinkURL(DataBinder.Eval(Container.DataItem, "img_pk").ToString()) %>'>
								<asp:Image ImageUrl='<%# GetThumbURL(DataBinder.Eval(Container.DataItem, "img_pk").ToString()) %>' Runat=server ID="Image2"/>
							</a>
						</ItemTemplate>
					</asp:TemplateColumn>
				</Columns>
			</asp:datagrid></form>
	</body>
</HTML>
