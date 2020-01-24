using System;
using System.Configuration;
using System.Collections;
using System.ComponentModel;
using System.Data;
using System.Data.SqlClient;
using System.Drawing;
using System.Web;
using System.IO;
using System.Web.SessionState;
using System.Web.UI;
using System.Web.UI.WebControls;
using System.Web.UI.HtmlControls;

namespace imgsample
{
	public class UploadImage : System.Web.UI.Page
	{
		protected System.Web.UI.WebControls.Button UploadBtn;
		protected System.Web.UI.WebControls.RequiredFieldValidator RequiredFieldValidator1;
		protected System.Web.UI.HtmlControls.HtmlInputText txtImgName;
		protected System.Web.UI.HtmlControls.HtmlInputFile UploadFile;

		public UploadImage() { }

		private void Page_Load(object sender, System.EventArgs e){ }

		public void UploadBtn_Click(object sender, System.EventArgs e)
		{
			if (Page.IsValid) //save the image
			{

				int RowsAffected = SaveToDB();
				if ( RowsAffected>0 )
				{
					Response.Write("<BR>The Image was saved");
				}
				else
				{
					Response.Write("<BR>An error occurred uploading the image");
				}

			}
		}

		private void InitializeComponent()
		{
			this.Load += new System.EventHandler(this.Page_Load);

		}

		public bool ThumbnailCallback()
		{
			return false;
		}

		private int SaveToDB()
		{
			System.Drawing.Image imgOrig = null;
			System.Drawing.Image imgMed = null;
			System.Drawing.Image imgThumb = null;

			imgOrig = System.Drawing.Image.FromStream(UploadFile.PostedFile.InputStream);
			UploadFile.PostedFile.InputStream.Seek(0, SeekOrigin.Begin); // reset to beginning
			byte[] origBinaryData = new byte[UploadFile.PostedFile.ContentLength];
			int n = UploadFile.PostedFile.InputStream.Read(origBinaryData,0,UploadFile.PostedFile.ContentLength);

			System.Drawing.Image.GetThumbnailImageAbort dummyCallback = new System.Drawing.Image.GetThumbnailImageAbort(ThumbnailCallback);

			// create thumb and mid
			int iThumbWidth, iThumbHeight, iMedWidth, iMedHeight;
			int iWidth  = imgOrig.Width; 
			int iHeight = imgOrig.Height;

			float iMaxThumbSize = 130;
			float iMaxMedSize = 320;

			if (imgOrig.Size.Height > imgOrig.Size.Width)
			{
				// Portrait
				iThumbHeight = (int)iMaxThumbSize;
				iThumbWidth = (int)((float)imgOrig.Size.Width / ((float)imgOrig.Size.Height / iMaxThumbSize));

				iMedHeight = (int)iMaxMedSize;
				iMedWidth = (int)((float)imgOrig.Size.Width / ((float)imgOrig.Size.Height / iMaxMedSize));
			}
			else
			{
				// Landscape
				iThumbWidth = (int)iMaxThumbSize;
				iThumbHeight = (int)((float)imgOrig.Size.Height / ((float)imgOrig.Size.Width / iMaxThumbSize));

				iMedWidth = (int)iMaxMedSize;
				iMedHeight = (int)((float)imgOrig.Size.Height / ((float)imgOrig.Size.Width/ iMaxMedSize));
			}

            System.IO.Stream streamTemp = new System.IO.MemoryStream();

			// Create parallelogram for drawing image.
			Point ulCorner = new Point(0, 0);
			Point urCorner = new Point(iMedWidth, 0);
			Point llCorner = new Point(0, iMedHeight);
			Point[] destPara = {ulCorner, urCorner, llCorner};
			// Draw image to screen.

			imgMed = imgOrig.GetThumbnailImage(iMedWidth, iMedHeight,dummyCallback, IntPtr.Zero);
			imgMed.Save(streamTemp, System.Drawing.Imaging.ImageFormat.Png ); // Meds are always png
			byte[] medBinaryData = new byte[streamTemp.Length];
			streamTemp.Seek(0, SeekOrigin.Begin); // reset to beginning
			streamTemp.Read(medBinaryData, 0 , (int)streamTemp.Length);

			
			imgThumb = imgOrig.GetThumbnailImage(iThumbWidth, iThumbHeight,dummyCallback, IntPtr.Zero);
			streamTemp.Seek(0, SeekOrigin.Begin); // reset to beginning
			imgThumb.Save(streamTemp, System.Drawing.Imaging.ImageFormat.Png); // thumbs are always png
			byte[] thumbBinaryData = new byte[streamTemp.Length];
			streamTemp.Seek(0, SeekOrigin.Begin); // reset to beginning
			streamTemp.Read(thumbBinaryData, 0 , (int)streamTemp.Length);

			//use the web.config to store the connection string
			SqlConnection connection = new SqlConnection(ConfigurationSettings.AppSettings["DSN"]);
			SqlCommand command = new SqlCommand( "INSERT INTO Image (img_name,img_full,img_thumb,img_med,img_contenttype) VALUES ( @img_name, @img_full, @img_thumb, @img_med, @img_contenttype )", connection );

			SqlParameter param0 = new SqlParameter( "@img_name", SqlDbType.VarChar,50 );
			param0.Value = txtImgName.Value;
			command.Parameters.Add( param0 );

			SqlParameter param1 = new SqlParameter( "@img_full", SqlDbType.Image );
			param1.Value = origBinaryData;
			command.Parameters.Add( param1 );

			SqlParameter param2 = new SqlParameter( "@img_thumb", SqlDbType.Image );
			param2.Value = thumbBinaryData;
			command.Parameters.Add( param2 );

			SqlParameter param3 = new SqlParameter( "@img_med", SqlDbType.Image );
			param3.Value = medBinaryData;
			command.Parameters.Add( param3 );

			SqlParameter param4 = new SqlParameter("@img_contenttype", SqlDbType.VarChar,50 );
			param4.Value = UploadFile.PostedFile.ContentType;
			command.Parameters.Add( param4 );

			connection.Open();
			int numRowsAffected = command.ExecuteNonQuery();
			connection.Close();

			return numRowsAffected;
		}
	}
}
