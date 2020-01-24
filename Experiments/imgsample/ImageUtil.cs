using System;
using System.Drawing;
using System.Drawing.Imaging;
using System.IO;
using System.Configuration;
using System.Data;

namespace imgsample
{
	/// <summary>
	/// Scales and saves images to db
	/// </summary>
	public class ImageUtil
	{
		public ImageUtil()
		{
			// 
			// TODO: Add constructor logic here
			//

		}

		private static System.Drawing.Imaging.ImageCodecInfo GetEncoderInfo(String mimeType)
		{
			int j;
			System.Drawing.Imaging.ImageCodecInfo[] encoders;
			encoders = System.Drawing.Imaging.ImageCodecInfo.GetImageEncoders();
			for(j = 0; j < encoders.Length; ++j)
			{
				if(encoders[j].MimeType == mimeType)
					return encoders[j];
			} return null;
		}

		public static System.Drawing.Image GetScaledAndCompressedJpeg(System.IO.Stream stm, int max, long quality)
		{
			return GetScaledAndCompressedJpeg(System.Drawing.Image.FromStream(stm), max, quality);
		}

		public static System.Drawing.Image GetScaledAndCompressedJpeg(System.Drawing.Image img, int max, long quality)
		{
			ImageCodecInfo jpgEncoder = GetEncoderInfo("image/jpeg");
			Size s = GetScaledSize(img.Size, max);
			Bitmap newBitmap = new Bitmap(s.Width, s.Height);
						
			// Create a Graphics object which is the "workhorse" for the high quality resizing
			Graphics grfxThumb = Graphics.FromImage(newBitmap);
				
			// Set the interpolation mode
			// Bicubic				: Specifies bicubic interpolation
			// Bilinear				: Specifies bilinear interpolation
			// Default				: Specifies default mode
			// High					: Specifies high quality interpolation
			// HighQualityBicubic	: Specifies high quality bicubic interpolation ****************** THIS IS THE HIGHEST QUALITY
			// HighQualityBilinear	: Specifies high quality bilinear interpolation 
			// Invalid				: Equivalent to the Invalid element of the QualityMode enumeration
			// Low					: Specifies low quality interpolation
			// NearestNeighbor		: Specifies nearest-neighbor interpolation
			grfxThumb.InterpolationMode = System.Drawing.Drawing2D.InterpolationMode.HighQualityBicubic;

			// Draw the original image to the target image
			grfxThumb.DrawImage(img, new Rectangle(0, 0, s.Width, s.Height));

			// Set the quality (must be a long)
			Encoder qualityEncoder = Encoder.Quality;
			EncoderParameter ratio = new EncoderParameter(qualityEncoder, (long)quality);

			// Add the quality parameter to the list
			EncoderParameters codecParams = new EncoderParameters(1);
			codecParams.Param[0] = ratio;

			// Save to JPG
			System.IO.MemoryStream stm = new System.IO.MemoryStream();
			newBitmap.Save(stm, jpgEncoder, codecParams);
			stm.Seek(0, System.IO.SeekOrigin.Begin);

			// Return new bitmap
			return new Bitmap(stm);
		}

		private static Size GetScaledSize(Size orig, int max)
		{
			Size s = new Size();

			if (orig.Height > orig.Width)
			{
				// Portrait
				s.Height = (int)max;
				s.Width = (int)((float)orig.Width / ((float)orig.Height / max));
			}
			else
			{
				// Landscape
				s.Width = (int)max;
				s.Height = (int)((float)orig.Height / ((float)orig.Width / max));
			}
			return s;
		}

		public static byte[] GetImageBytes(Stream stm)
		{
			stm.Seek(0, SeekOrigin.Begin); // reset to beginning
			byte[] rgOrig = new byte[stm.Length];
			stm.Read(rgOrig,0,(int)stm.Length);
			return rgOrig;
		}

		public static byte[] GetScaledImageBytes(Stream stm, int size, long qual)
		{
			System.IO.Stream streamTemp = new System.IO.MemoryStream();
			System.Drawing.Image img = GetScaledAndCompressedJpeg(stm, size, qual);
			img.Save(streamTemp, System.Drawing.Imaging.ImageFormat.Jpeg ); // always jpg
			byte[] rg = new byte[streamTemp.Length];
			streamTemp.Seek(0, SeekOrigin.Begin); // reset to beginning
			streamTemp.Read(rg, 0 , (int)streamTemp.Length);
			return rg;
		}

		public static void SaveToRow(imgsample.ImagesDataSet.imageRow row, Stream streamOrig, String nameOrig, String contenttypeOrig)
		{
			int iThumbSize =  System.Convert.ToInt32(ConfigurationSettings.AppSettings["ThumbSize"]); 
			int iMedSize = System.Convert.ToInt32(ConfigurationSettings.AppSettings["MedSize"]); 
			long lThumbQuality = (long)System.Convert.ToInt32(ConfigurationSettings.AppSettings["ThumbQuality"]);
			long lMedQuality = (long)System.Convert.ToInt32(ConfigurationSettings.AppSettings["MedQuality"]);

			System.Drawing.Image imgOrig = null;
			System.Drawing.Image imgMed = null;
			System.Drawing.Image imgThumb = null;

			imgOrig = System.Drawing.Image.FromStream(streamOrig);
			streamOrig.Seek(0, SeekOrigin.Begin); // reset to beginning
			byte[] rgOrig = new byte[streamOrig.Length];
			int n = streamOrig.Read(rgOrig,0,(int)streamOrig.Length);

			// create thumb and mid
			System.IO.Stream streamTemp = new System.IO.MemoryStream();

			imgMed = GetScaledAndCompressedJpeg(imgOrig, iMedSize, lMedQuality);
			imgMed.Save(streamTemp, System.Drawing.Imaging.ImageFormat.Jpeg ); // Meds are always png
			byte[] rgMed = new byte[streamTemp.Length];
			streamTemp.Seek(0, SeekOrigin.Begin); // reset to beginning
			streamTemp.Read(rgMed, 0 , (int)streamTemp.Length);

			imgThumb = GetScaledAndCompressedJpeg(imgOrig, iThumbSize, lThumbQuality);
			streamTemp.Seek(0, SeekOrigin.Begin); // reset to beginning
			imgThumb.Save(streamTemp, System.Drawing.Imaging.ImageFormat.Jpeg); // thumbs are always png
			byte[] rgThumb = new byte[streamTemp.Length];
			streamTemp.Seek(0, SeekOrigin.Begin); // reset to beginning
			streamTemp.Read(rgThumb, 0 , (int)streamTemp.Length);

			try
			{
				PropertyItem prop = imgOrig.GetPropertyItem(40091);
				EXIFPropertyItem exifProp = new EXIFPropertyItem(prop);
				row.img_desc = exifProp.ParsedString;
			}
			catch {}

			try
			{
				PropertyItem prop = imgOrig.GetPropertyItem((int)KnownEXIFIDCodes.DateTimeOriginal);
				EXIFPropertyItem exifProp = new EXIFPropertyItem(prop);
				row.img_datepicturetaken = exifProp.ParsedDate;
			}
			catch {}

			string [] parts = nameOrig.Split(new Char[] {'\\'});
			string filename = parts[parts.Length-1];
			row.img_name = filename;
			row.img_full = rgOrig;
			row.img_med = rgMed;
			row.img_thumb = rgThumb;
			row.img_contenttype = contenttypeOrig;
		}
	}
}
