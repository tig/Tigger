using System;
using System.Collections;
using System.Configuration;
using System.ComponentModel;
using System.Data;
using System.Data.SqlClient;
using System.Drawing;
using System.Web;
using System.Web.SessionState;
using System.Web.UI;
using System.Web.UI.WebControls;
using System.Web.UI.HtmlControls;

namespace imgsample
{
	/// <summary>
	/// Summary description for ViewImage.
	/// </summary>
	public class ViewImage : System.Web.UI.Page
	{
	
		public ViewImage() { }

		private void InitializeComponent()
		{
			this.Load += new System.EventHandler(this.Page_Load);

		}

		private void Page_Load(object sender, System.EventArgs e)
		{
			string sqlText ;
			string DataCol = "img_full";
			//get the image id from the url
			string ImageId = Request.QueryString["id"];
			string ImageDir = Request.QueryString["dir"];
			string ImageName = Request.QueryString["name"];
			string ImageSize = Request.QueryString["size"];
			string ImageExt = Request.QueryString["ext"];
		
			if (ImageSize != null)
				DataCol = "img_" + ImageSize;

			if (ImageId != null)
			{
				//build our query statement
				sqlText = "SELECT " + DataCol + ", img_contenttype FROM Image WHERE img_pk = " + ImageId;
			}
			else
			{
				sqlText = "SELECT " + DataCol + ", img_contenttype FROM Image WHERE img_name = '" + ImageName + ImageExt + "'";
				if (ImageDir != null && ImageDir.Length > 0)
					sqlText += " AND img_dir = '" + ImageDir + "'";
			}

			SqlConnection connection = new SqlConnection( ConfigurationSettings.AppSettings["DSN"].ToString() );
			SqlCommand command = new SqlCommand( sqlText, connection);

			//open the database and get a datareader
			connection.Open();
			SqlDataReader dr = command.ExecuteReader();
			if (dr.Read())
			{
				int IdCol = dr.GetOrdinal(DataCol);
				if (!dr.IsDBNull(IdCol))
				{
					if ((ImageSize == "med") || (ImageSize == "thumb"))
						Response.ContentType = "image/png";
					else
						Response.ContentType = dr["img_contenttype"].ToString();
					Response.BinaryWrite( (byte[]) dr[DataCol] );

					connection.Close();
					return;
				}
			}
			connection.Close();
			throw new HttpException(404, ImageDir + "/" + ImageName + ImageExt + " not found.");
		}

		private void ImageButton1_Click(object sender, System.Web.UI.ImageClickEventArgs e)
		{
		
		}
	}
}
