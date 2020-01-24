using System;
using System.Collections;
using System.ComponentModel;
using System.Configuration;
using System.Data;
using System.Data.SqlClient;
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
	/// Summary description for ViewImageLink.
	/// </summary>
	public class ViewImageLink : System.Web.UI.Page
	{
		protected System.Web.UI.WebControls.Image Image1;
	
		private void Page_Load(object sender, System.EventArgs e)
		{
			// Put user code to initialize the page here
			System.UriBuilder uribuilder = new UriBuilder(Request.Url);

			Image1.ImageUrl = "ViewImage.aspx" + Request.Url.Query;
		}

		public string GetFullImageURL()
		{
			string sqlText ;
			//get the image id from the url
			string ImageId = Request.QueryString["id"];
			string ImageDir = Request.QueryString["dir"];
			string ImageName = Request.QueryString["name"];
			string ImageExt = Request.QueryString["ext"];
	
			string fullURL ;

			if (ImageId != null)
				sqlText = "SELECT img_name, img_dir, img_contenttype FROM Image WHERE img_pk = '" + ImageId + "'";
			else
				sqlText = "SELECT img_pk, img_name, img_dir, img_contenttype FROM Image WHERE img_name = '" + ImageName + ImageExt + "'";
			if (ImageDir != null && ImageDir.Length > 0)
				sqlText += " AND img_dir = '" + ImageDir + "'";
			SqlConnection connection = new SqlConnection( ConfigurationSettings.AppSettings["DSN"].ToString() );
			SqlCommand command = new SqlCommand( sqlText, connection);
			//open the database and get a datareader
			connection.Open();
			SqlDataReader dr = command.ExecuteReader();
			if (dr.Read())
			{
				ImageName = dr["img_name"].ToString();

				//RegistryKey key = 
				//	Registry.LocalMachine.OpenSubKey("SOFTWARE\\Classes\\MIME\\Database\\Content Type\\" + dr["img_contenttype"].ToString());
				//ext = key.GetValue("Extension", ".jpg").ToString();

				ImageDir = dr["img_dir"].ToString();
				if (ImageDir.Length == 0) ImageDir = null;

			}
			connection.Close();

			fullURL = ImageName ; //+ ext;

			if (ImageDir != null)
				fullURL = ImageDir + "/" + fullURL;

			return fullURL;

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
			this.Load += new System.EventHandler(this.Page_Load);

		}
		#endregion

	}
}
