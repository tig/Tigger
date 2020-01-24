<%@ Page language="c#" Codebehind="edit.aspx.cs" inherits="imgsample.edit" %>
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
			<asp:label id="Label1" style="Z-INDEX: 101; LEFT: 11px; POSITION: absolute; TOP: 8px" runat="server">ImgSample Sample App</asp:label>
			<asp:button id="ButtonAdd" style="Z-INDEX: 105; LEFT: 12px; POSITION: absolute; TOP: 33px" runat="server"
				Height="25px" Text="Add" Width="56px"></asp:button><INPUT id="UploadFile" style="Z-INDEX: 106; LEFT: 73px; WIDTH: 548px; POSITION: absolute; TOP: 35px; HEIGHT: 22px"
				type="file" size="72" name="UploadFile" runat="server">
			<asp:datagrid id=DataGrid1 style="Z-INDEX: 104; LEFT: 5px; POSITION: absolute; TOP: 62px" runat="server" Width="90%" ShowFooter="True" Font-Size="X-Small" DataSource="<%# dataSet %>" DataKeyField="img_pk" DataMember="image" AutoGenerateColumns="False" BorderColor="#CC9966" BorderStyle="None" BorderWidth="1px" BackColor="White" CellPadding="4">
				<SelectedItemStyle Font-Names="Verdana" Font-Bold="True" ForeColor="#663399" BackColor="#FFCC66"></SelectedItemStyle>
				<ItemStyle Height="140px" ForeColor="#330099" Width="140px" BackColor="White"></ItemStyle>
				<HeaderStyle Font-Bold="True" ForeColor="#FFFFCC" BackColor="#990000"></HeaderStyle>
				<FooterStyle ForeColor="#330099" BackColor="#FFFFCC"></FooterStyle>
				<Columns>
					<asp:BoundColumn DataField="img_pk" SortExpression="img_pk" ReadOnly="True" HeaderText="ID"></asp:BoundColumn>
					<asp:EditCommandColumn ButtonType="PushButton" UpdateText="Update" CancelText="Cancel" EditText="Edit"></asp:EditCommandColumn>
					<asp:ButtonColumn Text="Delete" ButtonType="PushButton" CommandName="Delete"></asp:ButtonColumn>
					<asp:BoundColumn DataField="img_dir" SortExpression="img_dir" HeaderText="Dir"></asp:BoundColumn>
					<asp:BoundColumn DataField="img_name" SortExpression="img_name" HeaderText="Name"></asp:BoundColumn>
					<asp:BoundColumn DataField="img_desc" SortExpression="img_desc" HeaderText="Desc"></asp:BoundColumn>
					<asp:TemplateColumn HeaderText="Image">
						<HeaderStyle Width="140px"></HeaderStyle>
						<ItemStyle HorizontalAlign="Center" VerticalAlign="Middle"></ItemStyle>
						<ItemTemplate>
							<A href='<%# GetLinkURL(DataBinder.Eval(Container.DataItem, "img_pk").ToString()) %>'>
								<asp:Image id=Image2 Runat="server" ImageUrl='<%# GetThumbURL(DataBinder.Eval(Container.DataItem, "img_pk").ToString()) %>' BorderWidth="1">
								</asp:Image></A>
						</ItemTemplate>
					</asp:TemplateColumn>
					<asp:BoundColumn DataField="img_contenttype" SortExpression="img_contenttype" ReadOnly="True" HeaderText="type"></asp:BoundColumn>
				</Columns>
				<PagerStyle HorizontalAlign="Center" ForeColor="#330099" BackColor="#FFFFCC"></PagerStyle>
			</asp:datagrid></form>
	</body>
</HTML>
