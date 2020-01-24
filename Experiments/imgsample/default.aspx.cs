using System;
using System.Collections;
using System.ComponentModel;
using System.Configuration;
using System.Data;
using System.Drawing;
using System.Web;
using System.Web.SessionState;
using System.Web.UI;
using System.Web.UI.WebControls;
using System.Web.UI.HtmlControls;
using Microsoft.Win32;

namespace imgsample
{
	/// <summary>
	/// Summary description for _default1.
	/// </summary>
	public class _default : System.Web.UI.Page
	{
		private System.Data.SqlClient.SqlDataAdapter sqlDataAdapter;
		protected System.Web.UI.WebControls.Repeater repeater;
		protected System.Data.SqlClient.SqlCommand sqlSelectCommand;
		protected System.Data.SqlClient.SqlConnection sqlConnection;
		//protected System.Web.UI.WebControls.DataGrid DataGrid1;

		protected System.Web.UI.WebControls.Label lblGalleryName;
	
		private void Page_Load(object sender, System.EventArgs e)
		{
			// Put user code to initialize the page here
			if (!IsPostBack)
				BindControls();
		}

		public string GetThumbURL(string str)
		{
			return "viewimage.aspx?ID=" + str + "&size=thumb";
		}

		public string GetLinkURL(string str)
		{
			return "viewimagelink.aspx?ID=" + str + "&size=med";
		}

		public string GetMedURL(string str)
		{
			return "viewimage.aspx?ID=" + str + "&size=med";
		}

		public string GetFullImageURL(string ImageDir, string ImageName, string ImageType)
		{
			string fullURL ;
			
			RegistryKey key = 
				Registry.LocalMachine.OpenSubKey("SOFTWARE\\Classes\\MIME\\Database\\Content Type\\" + ImageType);

			fullURL = ImageName ; // + key.GetValue("Extension", ".jpg");

			if (ImageDir != null && ImageDir.Length > 0)
				fullURL = ImageDir + "/" + fullURL;

			return Request.ApplicationPath + "/" + fullURL;

		}


		void BindControls()
		{
			sqlConnection.ConnectionString = ConfigurationSettings.AppSettings["DSN"].ToString();
			sqlConnection.Open();
			System.Data.SqlClient.SqlDataReader Reader;
			Reader = sqlSelectCommand.ExecuteReader();
			//DataGrid1.DataSource = Reader;
			//DataGrid1.DataBind();

			repeater.DataSource = Reader;
			repeater.DataBind();
			
			Reader.Close();
			sqlConnection.Close();
		}

		#region Web Form Designer generated code
		override protected void OnInit(EventArgs e)
		{
			//
			// CODEGEN: This call is required by the ASP.NET Web Form Designer.
			//
			InitializeComponent();
			base.OnInit(e);
		}
		
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{    
			this.sqlDataAdapter = new System.Data.SqlClient.SqlDataAdapter();
			this.sqlSelectCommand = new System.Data.SqlClient.SqlCommand();
			this.sqlConnection = new System.Data.SqlClient.SqlConnection();
			// 
			// sqlDataAdapter
			// 
			this.sqlDataAdapter.SelectCommand = this.sqlSelectCommand;
			this.sqlDataAdapter.TableMappings.AddRange(new System.Data.Common.DataTableMapping[] {
																									 new System.Data.Common.DataTableMapping("Table", "image", new System.Data.Common.DataColumnMapping[] {
																																																			  new System.Data.Common.DataColumnMapping("img_pk", "img_pk"),
																																																			  new System.Data.Common.DataColumnMapping("img_name", "img_name"),
																																																			  new System.Data.Common.DataColumnMapping("img_desc", "img_desc"),
																																																			  new System.Data.Common.DataColumnMapping("img_dir", "img_dir"),
																																																			  new System.Data.Common.DataColumnMapping("img_full", "img_full"),
																																																			  new System.Data.Common.DataColumnMapping("img_med", "img_med"),
																																																			  new System.Data.Common.DataColumnMapping("img_thumb", "img_thumb"),
																																																			  new System.Data.Common.DataColumnMapping("img_contenttype", "img_contenttype"),
																																																			  new System.Data.Common.DataColumnMapping("img_datepicturetaken", "img_datepicturetaken")})});
			// 
			// sqlSelectCommand
			// 
			this.sqlSelectCommand.CommandText = "SELECT img_pk, img_name, img_desc, img_dir, img_full, img_med, img_thumb, img_con" +
				"tenttype, img_datepicturetaken FROM image ORDER BY img_datepicturetaken ASC";
			this.sqlSelectCommand.Connection = this.sqlConnection;
			this.repeater.ItemCommand += new System.Web.UI.WebControls.RepeaterCommandEventHandler(this.repeater_ItemCommand);
			this.Load += new System.EventHandler(this.Page_Load);

		}
		#endregion

/*
 		private void DataGrid1_SortCommand(object source, System.Web.UI.WebControls.DataGridSortCommandEventArgs e)
		{
			sqlSelectCommand.Parameters["@orderby"].Value = e.SortExpression;
			BindControls();		
		}
*/
		private void repeater_ItemCommand(object source, System.Web.UI.WebControls.RepeaterCommandEventArgs e)
		{
		
		}
	}
}
