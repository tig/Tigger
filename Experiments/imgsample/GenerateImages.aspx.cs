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
using System.IO;

namespace imgsample
{
	/// <summary>
	/// Summary description for GenerateImages.
	/// </summary>
	public class GenerateImages : System.Web.UI.Page
	{
		protected System.Web.UI.WebControls.DataGrid DataGrid1;
		protected System.Data.SqlClient.SqlConnection sqlConnection1;
		protected System.Data.SqlClient.SqlCommand sqlSelectCommand2;
		protected System.Data.SqlClient.SqlCommand sqlInsertCommand2;
		protected System.Data.SqlClient.SqlCommand sqlUpdateCommand2;
		protected System.Data.SqlClient.SqlCommand sqlDeleteCommand2;
		protected System.Data.SqlClient.SqlDataAdapter sqlDataAdapter;
		protected imgsample.ImagesDataSet dataSet;
		protected System.Web.UI.WebControls.Button Button1;
	
		private void Page_Load(object sender, System.EventArgs e)
		{
			// Put user code to initialize the page here
			sqlConnection1.ConnectionString = ConfigurationSettings.AppSettings["DSN"].ToString();
			sqlDataAdapter.Fill(dataSet);

			DataGrid1.DataSource= dataSet;
			DataGrid1.DataBind();

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
			this.sqlConnection1 = new System.Data.SqlClient.SqlConnection();
			this.sqlSelectCommand2 = new System.Data.SqlClient.SqlCommand();
			this.sqlInsertCommand2 = new System.Data.SqlClient.SqlCommand();
			this.sqlUpdateCommand2 = new System.Data.SqlClient.SqlCommand();
			this.sqlDeleteCommand2 = new System.Data.SqlClient.SqlCommand();
			this.sqlDataAdapter = new System.Data.SqlClient.SqlDataAdapter();
			this.dataSet = new imgsample.ImagesDataSet();
			((System.ComponentModel.ISupportInitialize)(this.dataSet)).BeginInit();
			this.Button1.Click += new System.EventHandler(this.Button1_Click);
			// 
			// sqlConnection1
			// 
			this.sqlConnection1.ConnectionString = "workstation id=CKINDELXP;packet size=4096;integrated security=SSPI;data source=ck" +
				"indel;persist security info=False;initial catalog=imgsample";
			this.sqlConnection1.InfoMessage += new System.Data.SqlClient.SqlInfoMessageEventHandler(this.sqlConnection1_InfoMessage);
			// 
			// sqlSelectCommand2
			// 
			this.sqlSelectCommand2.CommandText = "SELECT img_pk, img_name, img_desc, img_dir, img_full, img_med, img_thumb, img_con" +
				"tenttype, img_datepicturetaken FROM image";
			this.sqlSelectCommand2.Connection = this.sqlConnection1;
			// 
			// sqlInsertCommand2
			// 
			this.sqlInsertCommand2.CommandText = @"INSERT INTO image(img_name, img_desc, img_dir, img_full, img_med, img_thumb, img_contenttype, img_datepicturetaken) VALUES (@img_name, @img_desc, @img_dir, @img_full, @img_med, @img_thumb, @img_contenttype, @img_datepicturetaken); SELECT img_pk, img_name, img_desc, img_dir, img_full, img_med, img_thumb, img_contenttype, img_datepicturetaken FROM image WHERE (img_pk = @@IDENTITY)";
			this.sqlInsertCommand2.Connection = this.sqlConnection1;
			this.sqlInsertCommand2.Parameters.Add(new System.Data.SqlClient.SqlParameter("@img_name", System.Data.SqlDbType.VarChar, 256, "img_name"));
			this.sqlInsertCommand2.Parameters.Add(new System.Data.SqlClient.SqlParameter("@img_desc", System.Data.SqlDbType.VarChar, 1024, "img_desc"));
			this.sqlInsertCommand2.Parameters.Add(new System.Data.SqlClient.SqlParameter("@img_dir", System.Data.SqlDbType.VarChar, 256, "img_dir"));
			this.sqlInsertCommand2.Parameters.Add(new System.Data.SqlClient.SqlParameter("@img_full", System.Data.SqlDbType.VarBinary, 2147483647, "img_full"));
			this.sqlInsertCommand2.Parameters.Add(new System.Data.SqlClient.SqlParameter("@img_med", System.Data.SqlDbType.VarBinary, 2147483647, "img_med"));
			this.sqlInsertCommand2.Parameters.Add(new System.Data.SqlClient.SqlParameter("@img_thumb", System.Data.SqlDbType.VarBinary, 2147483647, "img_thumb"));
			this.sqlInsertCommand2.Parameters.Add(new System.Data.SqlClient.SqlParameter("@img_contenttype", System.Data.SqlDbType.VarChar, 50, "img_contenttype"));
			this.sqlInsertCommand2.Parameters.Add(new System.Data.SqlClient.SqlParameter("@img_datepicturetaken", System.Data.SqlDbType.DateTime, 8, "img_datepicturetaken"));
			// 
			// sqlUpdateCommand2
			// 
			this.sqlUpdateCommand2.CommandText = @"UPDATE image SET img_name = @img_name, img_desc = @img_desc, img_dir = @img_dir, img_full = @img_full, img_med = @img_med, img_thumb = @img_thumb, img_contenttype = @img_contenttype, img_datepicturetaken = @img_datepicturetaken WHERE (img_pk = @Original_img_pk) AND (img_contenttype = @Original_img_contenttype OR @Original_img_contenttype IS NULL AND img_contenttype IS NULL) AND (img_datepicturetaken = @Original_img_datepicturetaken OR @Original_img_datepicturetaken IS NULL AND img_datepicturetaken IS NULL) AND (img_desc = @Original_img_desc OR @Original_img_desc IS NULL AND img_desc IS NULL) AND (img_dir = @Original_img_dir OR @Original_img_dir IS NULL AND img_dir IS NULL) AND (img_name = @Original_img_name); SELECT img_pk, img_name, img_desc, img_dir, img_full, img_med, img_thumb, img_contenttype, img_datepicturetaken FROM image WHERE (img_pk = @img_pk)";
			this.sqlUpdateCommand2.Connection = this.sqlConnection1;
			this.sqlUpdateCommand2.Parameters.Add(new System.Data.SqlClient.SqlParameter("@img_name", System.Data.SqlDbType.VarChar, 256, "img_name"));
			this.sqlUpdateCommand2.Parameters.Add(new System.Data.SqlClient.SqlParameter("@img_desc", System.Data.SqlDbType.VarChar, 1024, "img_desc"));
			this.sqlUpdateCommand2.Parameters.Add(new System.Data.SqlClient.SqlParameter("@img_dir", System.Data.SqlDbType.VarChar, 256, "img_dir"));
			this.sqlUpdateCommand2.Parameters.Add(new System.Data.SqlClient.SqlParameter("@img_full", System.Data.SqlDbType.VarBinary, 2147483647, "img_full"));
			this.sqlUpdateCommand2.Parameters.Add(new System.Data.SqlClient.SqlParameter("@img_med", System.Data.SqlDbType.VarBinary, 2147483647, "img_med"));
			this.sqlUpdateCommand2.Parameters.Add(new System.Data.SqlClient.SqlParameter("@img_thumb", System.Data.SqlDbType.VarBinary, 2147483647, "img_thumb"));
			this.sqlUpdateCommand2.Parameters.Add(new System.Data.SqlClient.SqlParameter("@img_contenttype", System.Data.SqlDbType.VarChar, 50, "img_contenttype"));
			this.sqlUpdateCommand2.Parameters.Add(new System.Data.SqlClient.SqlParameter("@img_datepicturetaken", System.Data.SqlDbType.DateTime, 8, "img_datepicturetaken"));
			this.sqlUpdateCommand2.Parameters.Add(new System.Data.SqlClient.SqlParameter("@Original_img_pk", System.Data.SqlDbType.Int, 4, System.Data.ParameterDirection.Input, false, ((System.Byte)(0)), ((System.Byte)(0)), "img_pk", System.Data.DataRowVersion.Original, null));
			this.sqlUpdateCommand2.Parameters.Add(new System.Data.SqlClient.SqlParameter("@Original_img_contenttype", System.Data.SqlDbType.VarChar, 50, System.Data.ParameterDirection.Input, false, ((System.Byte)(0)), ((System.Byte)(0)), "img_contenttype", System.Data.DataRowVersion.Original, null));
			this.sqlUpdateCommand2.Parameters.Add(new System.Data.SqlClient.SqlParameter("@Original_img_datepicturetaken", System.Data.SqlDbType.DateTime, 8, System.Data.ParameterDirection.Input, false, ((System.Byte)(0)), ((System.Byte)(0)), "img_datepicturetaken", System.Data.DataRowVersion.Original, null));
			this.sqlUpdateCommand2.Parameters.Add(new System.Data.SqlClient.SqlParameter("@Original_img_desc", System.Data.SqlDbType.VarChar, 1024, System.Data.ParameterDirection.Input, false, ((System.Byte)(0)), ((System.Byte)(0)), "img_desc", System.Data.DataRowVersion.Original, null));
			this.sqlUpdateCommand2.Parameters.Add(new System.Data.SqlClient.SqlParameter("@Original_img_dir", System.Data.SqlDbType.VarChar, 256, System.Data.ParameterDirection.Input, false, ((System.Byte)(0)), ((System.Byte)(0)), "img_dir", System.Data.DataRowVersion.Original, null));
			this.sqlUpdateCommand2.Parameters.Add(new System.Data.SqlClient.SqlParameter("@Original_img_name", System.Data.SqlDbType.VarChar, 256, System.Data.ParameterDirection.Input, false, ((System.Byte)(0)), ((System.Byte)(0)), "img_name", System.Data.DataRowVersion.Original, null));
			this.sqlUpdateCommand2.Parameters.Add(new System.Data.SqlClient.SqlParameter("@img_pk", System.Data.SqlDbType.Int, 4, "img_pk"));
			// 
			// sqlDeleteCommand2
			// 
			this.sqlDeleteCommand2.CommandText = @"DELETE FROM image WHERE (img_pk = @Original_img_pk) AND (img_contenttype = @Original_img_contenttype OR @Original_img_contenttype IS NULL AND img_contenttype IS NULL) AND (img_datepicturetaken = @Original_img_datepicturetaken OR @Original_img_datepicturetaken IS NULL AND img_datepicturetaken IS NULL) AND (img_desc = @Original_img_desc OR @Original_img_desc IS NULL AND img_desc IS NULL) AND (img_dir = @Original_img_dir OR @Original_img_dir IS NULL AND img_dir IS NULL) AND (img_name = @Original_img_name)";
			this.sqlDeleteCommand2.Connection = this.sqlConnection1;
			this.sqlDeleteCommand2.Parameters.Add(new System.Data.SqlClient.SqlParameter("@Original_img_pk", System.Data.SqlDbType.Int, 4, System.Data.ParameterDirection.Input, false, ((System.Byte)(0)), ((System.Byte)(0)), "img_pk", System.Data.DataRowVersion.Original, null));
			this.sqlDeleteCommand2.Parameters.Add(new System.Data.SqlClient.SqlParameter("@Original_img_contenttype", System.Data.SqlDbType.VarChar, 50, System.Data.ParameterDirection.Input, false, ((System.Byte)(0)), ((System.Byte)(0)), "img_contenttype", System.Data.DataRowVersion.Original, null));
			this.sqlDeleteCommand2.Parameters.Add(new System.Data.SqlClient.SqlParameter("@Original_img_datepicturetaken", System.Data.SqlDbType.DateTime, 8, System.Data.ParameterDirection.Input, false, ((System.Byte)(0)), ((System.Byte)(0)), "img_datepicturetaken", System.Data.DataRowVersion.Original, null));
			this.sqlDeleteCommand2.Parameters.Add(new System.Data.SqlClient.SqlParameter("@Original_img_desc", System.Data.SqlDbType.VarChar, 1024, System.Data.ParameterDirection.Input, false, ((System.Byte)(0)), ((System.Byte)(0)), "img_desc", System.Data.DataRowVersion.Original, null));
			this.sqlDeleteCommand2.Parameters.Add(new System.Data.SqlClient.SqlParameter("@Original_img_dir", System.Data.SqlDbType.VarChar, 256, System.Data.ParameterDirection.Input, false, ((System.Byte)(0)), ((System.Byte)(0)), "img_dir", System.Data.DataRowVersion.Original, null));
			this.sqlDeleteCommand2.Parameters.Add(new System.Data.SqlClient.SqlParameter("@Original_img_name", System.Data.SqlDbType.VarChar, 256, System.Data.ParameterDirection.Input, false, ((System.Byte)(0)), ((System.Byte)(0)), "img_name", System.Data.DataRowVersion.Original, null));
			// 
			// sqlDataAdapter
			// 
			this.sqlDataAdapter.DeleteCommand = this.sqlDeleteCommand2;
			this.sqlDataAdapter.InsertCommand = this.sqlInsertCommand2;
			this.sqlDataAdapter.SelectCommand = this.sqlSelectCommand2;
			this.sqlDataAdapter.TableMappings.AddRange(new System.Data.Common.DataTableMapping[] {
																									 new System.Data.Common.DataTableMapping("Table", "image", new System.Data.Common.DataColumnMapping[] {
																																																			  new System.Data.Common.DataColumnMapping("img_pk", "img_pk"),
																																																			  new System.Data.Common.DataColumnMapping("img_name", "img_name"),
																																																			  new System.Data.Common.DataColumnMapping("img_desc", "img_desc"),
																																																			  new System.Data.Common.DataColumnMapping("img_dir", "img_dir"),
																																																			  new System.Data.Common.DataColumnMapping("img_full", "img_full"),
																																																			  new System.Data.Common.DataColumnMapping("img_med", "img_med"),
																																																			  new System.Data.Common.DataColumnMapping("img_thumb", "img_thumb"),
																																																			  new System.Data.Common.DataColumnMapping("img_contenttype", "img_contenttype")})});
			this.sqlDataAdapter.UpdateCommand = this.sqlUpdateCommand2;
			// 
			// dataSet
			// 
			this.dataSet.DataSetName = "ImagesDataSet";
			this.dataSet.Locale = new System.Globalization.CultureInfo("en-US");
			this.Load += new System.EventHandler(this.Page_Load);
			((System.ComponentModel.ISupportInitialize)(this.dataSet)).EndInit();

		}
		#endregion

		public bool ThumbnailCallback()
		{
			return false;
		}

		public string GetThumbURL(string str)
		{
			return "viewimage.aspx?ID=" + str + "&size=thumb";
		}

		public string GetLinkURL(string str)
		{
			return "viewimage.aspx?ID=" + str + "&size=med";
		}

		private void Button1_Click(object sender, System.EventArgs e)
		{

			foreach (imgsample.ImagesDataSet.imageRow r in dataSet.Tables["image"].Rows)
			{
				System.IO.MemoryStream stm = new System.IO.MemoryStream((byte[]) r["img_full"]);
				ImageUtil.SaveToRow(r, stm, r.img_name, r.img_contenttype);
/*
				r["img_med"] = ImageUtil.GetScaledImageBytes(stm, 
					System.Convert.ToInt32(ConfigurationSettings.AppSettings["MedSize"]), 
					(long)System.Convert.ToInt32(ConfigurationSettings.AppSettings["MedQuality"]));
				r["img_thumb"] = ImageUtil.GetScaledImageBytes(stm, 
					System.Convert.ToInt32(ConfigurationSettings.AppSettings["ThumbSize"]), 
					(long)System.Convert.ToInt32(ConfigurationSettings.AppSettings["ThumbQuality"]));

*/
			}

			//dataSet.AcceptChanges();
			int i = sqlDataAdapter.Update(dataSet);
		}

		private void sqlConnection1_InfoMessage(object sender, System.Data.SqlClient.SqlInfoMessageEventArgs e)
		{
		
		}
	}
}
