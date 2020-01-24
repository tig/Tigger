using System;
using System.Collections;
using System.ComponentModel;
using System.Configuration;
using System.Data;
using System.Web;
using System.Web.SessionState;
using System.Web.UI;
using System.Web.UI.WebControls;
using System.Web.UI.HtmlControls;

namespace imgsample
{
	/// <summary>
	/// Summary description for edit.
	/// </summary>
	public class edit : System.Web.UI.Page
	{
		protected System.Web.UI.WebControls.DataGrid DataGrid1;
		protected System.Web.UI.WebControls.Label Label1;
		protected imgsample.ImagesDataSet dataSet;
		protected DataView dv;
		protected System.Web.UI.WebControls.Button ButtonAdd;
		protected System.Data.SqlClient.SqlDataAdapter sqlDataAdapter1;
		protected System.Data.SqlClient.SqlCommand sqlSelectCommand1;
		protected System.Data.SqlClient.SqlCommand sqlInsertCommand1;
		protected System.Data.SqlClient.SqlCommand sqlUpdateCommand1;
		protected System.Data.SqlClient.SqlCommand sqlDeleteCommand1;
		protected System.Data.SqlClient.SqlConnection sqlConnection1;
		protected System.Web.UI.HtmlControls.HtmlInputFile UploadFile;
	
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
			this.dataSet = new imgsample.ImagesDataSet();
			this.sqlDataAdapter1 = new System.Data.SqlClient.SqlDataAdapter();
			this.sqlSelectCommand1 = new System.Data.SqlClient.SqlCommand();
			this.sqlInsertCommand1 = new System.Data.SqlClient.SqlCommand();
			this.sqlUpdateCommand1 = new System.Data.SqlClient.SqlCommand();
			this.sqlDeleteCommand1 = new System.Data.SqlClient.SqlCommand();
			this.sqlConnection1 = new System.Data.SqlClient.SqlConnection();
			((System.ComponentModel.ISupportInitialize)(this.dataSet)).BeginInit();
			this.ButtonAdd.Click += new System.EventHandler(this.ButtonAdd_Click);
			this.DataGrid1.CancelCommand += new System.Web.UI.WebControls.DataGridCommandEventHandler(this.DataGrid1_Cancel);
			this.DataGrid1.EditCommand += new System.Web.UI.WebControls.DataGridCommandEventHandler(this.DataGrid1_Edit);
			this.DataGrid1.SortCommand += new System.Web.UI.WebControls.DataGridSortCommandEventHandler(this.DataGrid1_Sort);
			this.DataGrid1.UpdateCommand += new System.Web.UI.WebControls.DataGridCommandEventHandler(this.DataGrid1_Update);
			this.DataGrid1.DeleteCommand += new System.Web.UI.WebControls.DataGridCommandEventHandler(this.DataGrid1_DeleteCommand);
			this.DataGrid1.SelectedIndexChanged += new System.EventHandler(this.DataGrid1_SelectedIndexChanged);
			// 
			// dataSet
			// 
			this.dataSet.DataSetName = "ImagesDataSet";
			this.dataSet.Locale = new System.Globalization.CultureInfo("en-US");
			// 
			// sqlDataAdapter1
			// 
			this.sqlDataAdapter1.DeleteCommand = this.sqlDeleteCommand1;
			this.sqlDataAdapter1.InsertCommand = this.sqlInsertCommand1;
			this.sqlDataAdapter1.SelectCommand = this.sqlSelectCommand1;
			this.sqlDataAdapter1.TableMappings.AddRange(new System.Data.Common.DataTableMapping[] {
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
			this.sqlDataAdapter1.UpdateCommand = this.sqlUpdateCommand1;
			// 
			// sqlSelectCommand1
			// 
			this.sqlSelectCommand1.CommandText = "SELECT img_pk, img_name, img_desc, img_dir, img_full, img_med, img_thumb, img_con" +
				"tenttype, img_datepicturetaken FROM image ORDER BY img_datepicturetaken ASC";
			this.sqlSelectCommand1.Connection = this.sqlConnection1;
			// 
			// sqlInsertCommand1
			// 
			this.sqlInsertCommand1.CommandText = @"INSERT INTO image(img_name, img_desc, img_dir, img_full, img_med, img_thumb, img_contenttype, img_datepicturetaken) VALUES (@img_name, @img_desc, @img_dir, @img_full, @img_med, @img_thumb, @img_contenttype, @img_datepicturetaken); SELECT img_pk, img_name, img_desc, img_dir, img_full, img_med, img_thumb, img_contenttype, img_datepicturetaken FROM image WHERE (img_pk = @@IDENTITY)";
			this.sqlInsertCommand1.Connection = this.sqlConnection1;
			this.sqlInsertCommand1.Parameters.Add(new System.Data.SqlClient.SqlParameter("@img_name", System.Data.SqlDbType.VarChar, 256, "img_name"));
			this.sqlInsertCommand1.Parameters.Add(new System.Data.SqlClient.SqlParameter("@img_desc", System.Data.SqlDbType.VarChar, 1024, "img_desc"));
			this.sqlInsertCommand1.Parameters.Add(new System.Data.SqlClient.SqlParameter("@img_dir", System.Data.SqlDbType.VarChar, 256, "img_dir"));
			this.sqlInsertCommand1.Parameters.Add(new System.Data.SqlClient.SqlParameter("@img_full", System.Data.SqlDbType.VarBinary, 2147483647, "img_full"));
			this.sqlInsertCommand1.Parameters.Add(new System.Data.SqlClient.SqlParameter("@img_med", System.Data.SqlDbType.VarBinary, 2147483647, "img_med"));
			this.sqlInsertCommand1.Parameters.Add(new System.Data.SqlClient.SqlParameter("@img_thumb", System.Data.SqlDbType.VarBinary, 2147483647, "img_thumb"));
			this.sqlInsertCommand1.Parameters.Add(new System.Data.SqlClient.SqlParameter("@img_contenttype", System.Data.SqlDbType.VarChar, 50, "img_contenttype"));
			this.sqlInsertCommand1.Parameters.Add(new System.Data.SqlClient.SqlParameter("@img_datepicturetaken", System.Data.SqlDbType.DateTime, 8, "img_datepicturetaken"));
			// 
			// sqlUpdateCommand1
			// 
			this.sqlUpdateCommand1.CommandText = @"UPDATE image SET img_name = @img_name, img_desc = @img_desc, img_dir = @img_dir, img_full = @img_full, img_med = @img_med, img_thumb = @img_thumb, img_contenttype = @img_contenttype, img_datepicturetaken = @img_datepicturetaken WHERE (img_pk = @Original_img_pk) AND (img_contenttype = @Original_img_contenttype OR @Original_img_contenttype IS NULL AND img_contenttype IS NULL) AND (img_datepicturetaken = @Original_img_datepicturetaken OR @Original_img_datepicturetaken IS NULL AND img_datepicturetaken IS NULL) AND (img_desc = @Original_img_desc OR @Original_img_desc IS NULL AND img_desc IS NULL) AND (img_dir = @Original_img_dir OR @Original_img_dir IS NULL AND img_dir IS NULL) AND (img_name = @Original_img_name); SELECT img_pk, img_name, img_desc, img_dir, img_full, img_med, img_thumb, img_contenttype, img_datepicturetaken FROM image WHERE (img_pk = @img_pk)";
			this.sqlUpdateCommand1.Connection = this.sqlConnection1;
			this.sqlUpdateCommand1.Parameters.Add(new System.Data.SqlClient.SqlParameter("@img_name", System.Data.SqlDbType.VarChar, 256, "img_name"));
			this.sqlUpdateCommand1.Parameters.Add(new System.Data.SqlClient.SqlParameter("@img_desc", System.Data.SqlDbType.VarChar, 1024, "img_desc"));
			this.sqlUpdateCommand1.Parameters.Add(new System.Data.SqlClient.SqlParameter("@img_dir", System.Data.SqlDbType.VarChar, 256, "img_dir"));
			this.sqlUpdateCommand1.Parameters.Add(new System.Data.SqlClient.SqlParameter("@img_full", System.Data.SqlDbType.VarBinary, 2147483647, "img_full"));
			this.sqlUpdateCommand1.Parameters.Add(new System.Data.SqlClient.SqlParameter("@img_med", System.Data.SqlDbType.VarBinary, 2147483647, "img_med"));
			this.sqlUpdateCommand1.Parameters.Add(new System.Data.SqlClient.SqlParameter("@img_thumb", System.Data.SqlDbType.VarBinary, 2147483647, "img_thumb"));
			this.sqlUpdateCommand1.Parameters.Add(new System.Data.SqlClient.SqlParameter("@img_contenttype", System.Data.SqlDbType.VarChar, 50, "img_contenttype"));
			this.sqlUpdateCommand1.Parameters.Add(new System.Data.SqlClient.SqlParameter("@img_datepicturetaken", System.Data.SqlDbType.DateTime, 8, "img_datepicturetaken"));
			this.sqlUpdateCommand1.Parameters.Add(new System.Data.SqlClient.SqlParameter("@Original_img_pk", System.Data.SqlDbType.Int, 4, System.Data.ParameterDirection.Input, false, ((System.Byte)(0)), ((System.Byte)(0)), "img_pk", System.Data.DataRowVersion.Original, null));
			this.sqlUpdateCommand1.Parameters.Add(new System.Data.SqlClient.SqlParameter("@Original_img_contenttype", System.Data.SqlDbType.VarChar, 50, System.Data.ParameterDirection.Input, false, ((System.Byte)(0)), ((System.Byte)(0)), "img_contenttype", System.Data.DataRowVersion.Original, null));
			this.sqlUpdateCommand1.Parameters.Add(new System.Data.SqlClient.SqlParameter("@Original_img_datepicturetaken", System.Data.SqlDbType.DateTime, 8, System.Data.ParameterDirection.Input, false, ((System.Byte)(0)), ((System.Byte)(0)), "img_datepicturetaken", System.Data.DataRowVersion.Original, null));
			this.sqlUpdateCommand1.Parameters.Add(new System.Data.SqlClient.SqlParameter("@Original_img_desc", System.Data.SqlDbType.VarChar, 1024, System.Data.ParameterDirection.Input, false, ((System.Byte)(0)), ((System.Byte)(0)), "img_desc", System.Data.DataRowVersion.Original, null));
			this.sqlUpdateCommand1.Parameters.Add(new System.Data.SqlClient.SqlParameter("@Original_img_dir", System.Data.SqlDbType.VarChar, 256, System.Data.ParameterDirection.Input, false, ((System.Byte)(0)), ((System.Byte)(0)), "img_dir", System.Data.DataRowVersion.Original, null));
			this.sqlUpdateCommand1.Parameters.Add(new System.Data.SqlClient.SqlParameter("@Original_img_name", System.Data.SqlDbType.VarChar, 256, System.Data.ParameterDirection.Input, false, ((System.Byte)(0)), ((System.Byte)(0)), "img_name", System.Data.DataRowVersion.Original, null));
			this.sqlUpdateCommand1.Parameters.Add(new System.Data.SqlClient.SqlParameter("@img_pk", System.Data.SqlDbType.Int, 4, "img_pk"));
			// 
			// sqlDeleteCommand1
			// 
			this.sqlDeleteCommand1.CommandText = @"DELETE FROM image WHERE (img_pk = @Original_img_pk) AND (img_contenttype = @Original_img_contenttype OR @Original_img_contenttype IS NULL AND img_contenttype IS NULL) AND (img_datepicturetaken = @Original_img_datepicturetaken OR @Original_img_datepicturetaken IS NULL AND img_datepicturetaken IS NULL) AND (img_desc = @Original_img_desc OR @Original_img_desc IS NULL AND img_desc IS NULL) AND (img_dir = @Original_img_dir OR @Original_img_dir IS NULL AND img_dir IS NULL) AND (img_name = @Original_img_name)";
			this.sqlDeleteCommand1.Connection = this.sqlConnection1;
			this.sqlDeleteCommand1.Parameters.Add(new System.Data.SqlClient.SqlParameter("@Original_img_pk", System.Data.SqlDbType.Int, 4, System.Data.ParameterDirection.Input, false, ((System.Byte)(0)), ((System.Byte)(0)), "img_pk", System.Data.DataRowVersion.Original, null));
			this.sqlDeleteCommand1.Parameters.Add(new System.Data.SqlClient.SqlParameter("@Original_img_contenttype", System.Data.SqlDbType.VarChar, 50, System.Data.ParameterDirection.Input, false, ((System.Byte)(0)), ((System.Byte)(0)), "img_contenttype", System.Data.DataRowVersion.Original, null));
			this.sqlDeleteCommand1.Parameters.Add(new System.Data.SqlClient.SqlParameter("@Original_img_datepicturetaken", System.Data.SqlDbType.DateTime, 8, System.Data.ParameterDirection.Input, false, ((System.Byte)(0)), ((System.Byte)(0)), "img_datepicturetaken", System.Data.DataRowVersion.Original, null));
			this.sqlDeleteCommand1.Parameters.Add(new System.Data.SqlClient.SqlParameter("@Original_img_desc", System.Data.SqlDbType.VarChar, 1024, System.Data.ParameterDirection.Input, false, ((System.Byte)(0)), ((System.Byte)(0)), "img_desc", System.Data.DataRowVersion.Original, null));
			this.sqlDeleteCommand1.Parameters.Add(new System.Data.SqlClient.SqlParameter("@Original_img_dir", System.Data.SqlDbType.VarChar, 256, System.Data.ParameterDirection.Input, false, ((System.Byte)(0)), ((System.Byte)(0)), "img_dir", System.Data.DataRowVersion.Original, null));
			this.sqlDeleteCommand1.Parameters.Add(new System.Data.SqlClient.SqlParameter("@Original_img_name", System.Data.SqlDbType.VarChar, 256, System.Data.ParameterDirection.Input, false, ((System.Byte)(0)), ((System.Byte)(0)), "img_name", System.Data.DataRowVersion.Original, null));
			// 
			// sqlConnection1
			// 
			this.sqlConnection1.ConnectionString = "workstation id=KINDELFS;packet size=4096;integrated security=SSPI;data source=KIN" +
				"DELSRV2;persist security info=False;initial catalog=imgsample";
			this.Load += new System.EventHandler(this.Page_Load);
			((System.ComponentModel.ISupportInitialize)(this.dataSet)).EndInit();

		}
		#endregion

		private void Page_Load(object sender, System.EventArgs e)
		{
			// Put user code to initialize the page here
			sqlConnection1.ConnectionString = ConfigurationSettings.AppSettings["DSN"].ToString();
			sqlDataAdapter1.Fill(dataSet);

			dv = new DataView(dataSet.image);
			dv.Sort = "img_pk";

			if (!IsPostBack)
				BindGrid();
		}

		void BindGrid()
		{
			DataGrid1.DataSource = dv;
			DataGrid1.DataBind();
		}
	
		public string GetThumbURL(string str)
		{
			return "viewimage.aspx?ID=" + str + "&size=thumb";
		}

		public string GetLinkURL(string str)
		{
			return "viewimagelink.aspx?ID=" + str + "&size=med";
		}


		public void DataGrid1_Edit(Object sender, DataGridCommandEventArgs e) 
		{

			// Set the EditItemIndex property to the index of the item clicked 
			// in the DataGrid control to enable editing for that item. Be sure
			// to rebind the DateGrid to the data source to refresh the control.
			DataGrid1.EditItemIndex = e.Item.ItemIndex;

			BindGrid();
		}
 
		public void DataGrid1_Cancel(Object sender, DataGridCommandEventArgs e) 
		{

			// Set the EditItemIndex property to -1 to exit editing mode. 
			// Be sure to rebind the DateGrid to the data source to refresh
			// the control.
			DataGrid1.EditItemIndex = -1;
			BindGrid();

		}
 
		public void DataGrid1_Update(Object sender, DataGridCommandEventArgs e) 
		{
			// Retrieve the text boxes that contain the values to update.
			// For bound columns, the edited value is stored in a TextBox.
			// The TextBox is the 0th control in a cell's Controls collection.
			// Each cell in the Cells collection of a DataGrid item represents
			// a column in the DataGrid control.
			TextBox dirText = (TextBox)e.Item.Cells[3].Controls[0];
			TextBox nameText = (TextBox)e.Item.Cells[4].Controls[0];
			TextBox descText = (TextBox)e.Item.Cells[5].Controls[0];
 
			// Retrieve the updated values.
			String item = e.Item.Cells[0].Text;
			String dir = dirText.Text;
			String name = nameText.Text;
			String desc = descText.Text;
        
			DataRow dr = dataSet.image.Rows[e.Item.ItemIndex];

			// ***************************************************************
			// Insert data validation code here. Be sure to validate the
			// values entered by the user before converting to the appropriate
			// data types and updating the data source.
			// ***************************************************************

			// Add the new entry.
			dr["img_dir"] = dir;
			dr["img_name"] = name;
			dr["img_desc"] = desc;

			sqlDataAdapter1.Update(dataSet);
			// Set the EditItemIndex property to -1 to exit editing mode. 
			// Be sure to rebind the DateGrid to the data source to refresh
			// the control.
			DataGrid1.EditItemIndex = -1;
			BindGrid();

		}

		public void DataGrid1_DeleteCommand(object source, System.Web.UI.WebControls.DataGridCommandEventArgs e)
		{
			int rowToDelete = e.Item.ItemIndex;

			// Add code to delete row from data source.
			dv.Delete(rowToDelete);
			
			//dataSet.Tables["image"].Rows[rowToDelete].Delete();		
			sqlDataAdapter1.Update(dataSet);

			BindGrid();
		}


		public void DataGrid1_SelectedIndexChanged(object sender, System.EventArgs e)
		{
		
		}

		private void ButtonAdd_Click(object sender, System.EventArgs e)
		{
			if (Page.IsValid) //save the image
			{
				//dataSet.image.AddimageRow(

				ImagesDataSet.imageRow row = dataSet.image.NewimageRow();
				ImageUtil.SaveToRow(row, UploadFile.PostedFile.InputStream, UploadFile.PostedFile.FileName, UploadFile.PostedFile.ContentType);

				/*
				row.img_med = ImageUtil.GetScaledImageBytes(UploadFile.PostedFile.InputStream, 
										System.Convert.ToInt32(ConfigurationSettings.AppSettings["MedSize"]), 
										(long)System.Convert.ToInt32(ConfigurationSettings.AppSettings["MedQuality"]));
				row.img_thumb = ImageUtil.GetScaledImageBytes(UploadFile.PostedFile.InputStream, 
										System.Convert.ToInt32(ConfigurationSettings.AppSettings["ThumbSize"]), 
										(long)System.Convert.ToInt32(ConfigurationSettings.AppSettings["ThumbQuality"]));
										*/

				// dataSet.image.AddimageRow(row.img_name, row.img_desc, row.img_dir, row.img_full, row.img_med, row.img_thumb, row.img_contenttype, row.img_datepicturetaken);
				dataSet.image.AddimageRow(row);
				int n = sqlDataAdapter1.Update(dataSet);
				if (n > 0)
				{
					DataGrid1.EditItemIndex = -1;
					BindGrid();
					DataGrid1.SelectedIndex = DataGrid1.Items.Count-1;
					//BindGrid();
				}
				else
				{
					Response.Write("<BR>An error occurred uploading the image");
				}

			}
	
		}
	

		private void DataGrid1_Sort(object source, System.Web.UI.WebControls.DataGridSortCommandEventArgs e)
		{
			dv.Sort = e.SortExpression;

			BindGrid();
		
		}
	}
}
