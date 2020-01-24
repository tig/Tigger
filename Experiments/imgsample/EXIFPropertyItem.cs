using System;
using System.Drawing.Imaging;
using System.Runtime.InteropServices;

namespace imgsample
{

	/// <summary>
	/// Enumerates known EXIF ID Codes
	/// See http://www.ba.wakwak.com/~tsuruzoh/Computer/Digicams/exif-e.html
	/// Also (note some of these documents are Japanese language):
	/// http://www.itojun.org/diary/199610.IWOOOS/exif.html
	/// http://www.yk.rim.or.jp/~mamo/Computer/DS-7/exif.html
	/// http://www.pima.net/standards/it10/PIMA15740/exif.htm
	/// 
	/// Many thanks to TsuruZoh Tachibanaya for his efforts in posting this
	/// information on the net.
	/// </summary>
	public enum KnownEXIFIDCodes
	{
		/// <summary>
		/// For other codes
		/// </summary>
		UnknownCode = 0x0,
		/// <summary>
		/// Describes image. Two-byte character code such as Chinese/Korean/Japanese cannot be used. 
		///  ascii string 
		/// </summary>
		ImageDescription = 0x010e,
		/// <summary>
		/// Manfacturer of digicam. In the Exif standard, this tag is optional, but it is mandatory for DCF. 
		/// ascii string 
		/// </summary>
		Manufacturer = 0x010f ,
		/// <summary>
		/// Shows model number of digicam. In the Exif standard, this tag is optional, but it is mandatory for DCF. 
		/// ascii string 
		/// </summary>
		Model  = 0x0110,
		/// <summary>
		/// The orientation of the camera relative to the scene, when the image was captured. The relation of the '0th row' and '0th column' to visual position is shown as right. 
		/// 1 top left side 
		/// 2 top right side 
		/// 3 bottom right side 
		/// 4 bottom left side 
		/// 5 left side top 
		/// 6 right side top 
		/// 7 right side bottom 
		/// 8 left side bottom 
		/// unsigned short 1 
		/// </summary>		
		Orientation  = 0x0112,		
		/// <summary>
		/// Display/Print resolution of image. Default value is 1/72inch, but it has no mean because personal computer doesn't use this value to display/print out. 
		/// unsigned rational 1  
		/// </summary>
		XResolution   = 0x011a,
		/// <summary>
		/// Display/Print resolution of image. Default value is 1/72inch, but it has no mean because personal computer doesn't use this value to display/print out. 
		/// unsigned rational 1  
		/// </summary>
		YResolution  = 0x011b,		
		/// <summary>
		/// Unit of XResolution(0x011a)/YResolution(0x011b). '1' means no-unit, '2' means inch, '3' means centimeter. Default value is '2'(inch). 
		/// unsigned short 1  
		/// </summary>
		ResolutionUnit  = 0x0128, 
		/// <summary>
		/// Shows firmware(internal software of digicam) version number. 
		/// ascii string 
		/// </summary>
		Software = 0x0131, 
		/// <summary>
		/// Date/Time of image was last modified. Data format is "YYYY:MM:DD HH:MM:SS"+0x00, total 20bytes. If clock has not set or digicam doesn't have clock, the field may be filled with spaces. In usual, it has the same value of DateTimeOriginal(0x9003) 
		/// ascii string 20  
		/// </summary>
		DateTime  = 0x0132 , 
		/// <summary>
		/// Defines chromaticity of white point of the image. If the image uses CIE Standard Illumination D65(known as international standard of 'daylight'), the values are '3127/10000,3290/10000'. 
		/// unsigned rational 2  
		/// </summary>
		WhitePoint  = 0x013e ,
		/// <summary>
		/// Defines chromaticity of the primaries of the image. If the image uses CCIR Recommendation 709 primaries, values are '640/1000,330/1000,300/1000,600/1000,150/1000,0/1000'. 
		/// unsigned rational 6  
		/// </summary>
		PrimaryChromaticities  = 0x013f,
		/// <summary>
		///  When image format is YCbCr, this value shows a constant to translate it to RGB format. In usual, values are '0.299/0.587/0.114'. 
		///  unsigned rational 3  
		/// </summary>
		YCbCrCoefficients  =0x0211,
		/// <summary>
		/// When image format is YCbCr and uses 'Subsampling'(cropping of chroma data, all the digicam do that), defines the chroma sample point of subsampling pixel array. '1' means the center of pixel array, '2' means the datum point. 
		/// unsigned short 1  
		/// </summary>
		YCbCrPositioning = 0x0213 ,
		/// <summary>
		///  Shows reference value of black point/white point. In case of YCbCr format, first 2 show black/white of Y, next 2 are Cb, last 2 are Cr. In case of RGB format, first 2 show black/white of R, next 2 are G, last 2 are B. 
		///  unsigned rational 6  
		/// </summary>
		ReferenceBlackWhite =0x0214,		
		/// <summary>
		///	Shows copyright information 
		///ascii string 
		/// </summary>
		Copyright = 0x8298,
		/// <summary>
		///  unsigned long 1  Offset to Exif Sub IFD 
		/// </summary>
		ExifOffset =0x8769,
		/// <summary>
		///  Exposure time (reciprocal of shutter speed). Unit is second. 
		///  unsigned rational 1  
		/// </summary>
		ExposureTime =0x829a,
		/// <summary>
		/// The actual F-number(F-stop) of lens when the image was taken. 
		/// unsigned rational 1  
		/// </summary>
		FNumber = 0x829d,  
		/// <summary>
		///  Exposure program that the camera used when image was taken. '1' means manual control, '2' program normal, '3' aperture priority, '4' shutter priority, '5' program creative (slow program), '6' program action(high-speed program), '7' portrait mode, '8' landscape mode. 
		///  unsigned short 1  
		/// </summary>
		ExposureProgram  =0x8822,
		/// <summary>
		/// CCD sensitivity equivalent to Ag-Hr film speedrate. 
		/// unsigned short 2  
		/// </summary>
		ISOSpeedRatings =0x8827, 
		/// <summary>
		/// Exif version number. Stored as 4bytes of ASCII character. If the picture is based on Exif V2.1, value is "0210". Since the type is 'undefined', there is no NULL(0x00) for termination. 
		/// undefined 4  
		/// </summary>
		ExifVersion = 0x9000,
		/// <summary>
		/// Date/Time of original image taken. This value should not be modified by user program. Data format is "YYYY:MM:DD HH:MM:SS"+0x00, total 20bytes. If clock has not set or digicam doesn't have clock, the field may be filled with spaces. In the Exif standard, this tag is optional, but it is mandatory for DCF. 
		/// ascii string 20  
		/// </summary>
		DateTimeOriginal = 0x9003,
		/// <summary>
		///  Date/Time of image digitized. Usually, it contains the same value of DateTimeOriginal(0x9003). Data format is "YYYY:MM:DD HH:MM:SS"+0x00, total 20bytes. If clock has not set or digicam doesn't have clock, the field may be filled with spaces. In the Exif standard, this tag is optional, but it is mandatory for DCF. 
		///  ascii string 20  
		/// </summary>
		DateTimeDigitized = 0x9004,
		/// <summary>
		/// Shows the order of pixel data. Most of case '0x04,0x05,0x06,0x00' is used for RGB-format and '0x01,0x02,0x03,0x00' for YCbCr-format. 0x00:does not exist, 0x01:Y, 0x02:Cb, 0x03:Cr, 0x04:Red, 0x05:Green, 0x06:Bllue. 
		/// undefined 
		/// </summary>
		ComponentsConfiguration =0x9101,
		/// <summary>
		///  The average compression ratio of JPEG (rough estimate). 
		///  unsigned rational 1  
		/// </summary>
		CompressedBitsPerPixel =0x9102,
		/// <summary>
		/// Shutter speed by APEX value. To convert this value to ordinary 'Shutter Speed'; calculate this value's power of 2, then reciprocal. For example, if the ShutterSpeedValue is '4', shutter speed is 1/(24)=1/16 second. 
		/// signed rational 1  
		/// </summary>
		ShutterSpeedValue = 0x9201,
		/// <summary>
		///  The actual aperture value of lens when the image was taken. Unit is APEX. To convert this value to ordinary F-number(F-stop), calculate this value's power of root 2 (=1.4142). For example, if the ApertureValue is '5', F-number is Pow(1.4142,5) = F5.6. 
		///  unsigned rational 1  
		/// </summary>
		ApertureValue  =0x9202,
		/// <summary>
		///  Brightness of taken subject, unit is APEX. To calculate Exposure(Ev) from BrigtnessValue(Bv), you must add SensitivityValue(Sv).
		///  	Ev=Bv+Sv   Sv=log2(ISOSpeedRating/3.125)
		///  	ISO100:Sv=5, ISO200:Sv=6, ISO400:Sv=7, ISO125:Sv=5.32.  
		///  signed rational 1  
		/// </summary>
		BrightnessValue   =0x9203,
		/// <summary>
		///  Exposure bias(compensation) value of taking picture. Unit is APEX(EV). 
		///  signed rational 1  
		/// </summary>
		ExposureBiasValue =0x9204 ,
		/// <summary>
		/// Maximum aperture value of lens. You can convert to F-number by calculating power of root 2 (same process of ApertureValue:0x9202). 
		///unsigned rational 1  
		/// </summary>
		MaxApertureValue = 0x9205,
		/// <summary>
		///  Distance to focus point, unit is meter. 
		///  signed rational 1  
		/// </summary>
		SubjectDistance  = 0x9206,
		/// <summary>
		///  Exposure metering method. '0' means unknown, '1' average, '2' center weighted average, '3' spot, '4' multi-spot, '5' multi-segment, '6' partial, '255' other. 
		///  unsigned short 1  
		/// </summary>
		MeteringMode = 0x9207,
		/// <summary>
		/// Light source, actually this means white balance setting. '0' means unknown, '1' daylight, '2' fluorescent, '3' tungsten, '10' flash, '17' standard light A, '18' standard light B, '19' standard light C, '20' D55, '21' D65, '22' D75, '255' other. 
		/// unsigned short 1  
		/// </summary>
		LightSource  = 0x9208,
		/// <summary>
		///  '0' means flash did not fire, '1' flash fired, '5' flash fired but strobe return light not detected, '7' flash fired and strobe return light detected. 
		///  unsigned short 1  
		/// </summary>
		Flash  = 0x9209,
		/// <summary>
		///  Focal length of lens used to take image. Unit is millimeter. 
		///  unsigned rational 1  
		/// </summary>
		FocalLength = 0x920a,
		/// <summary>
		/// Maker dependent internal data. Some of maker such as Olympus/Nikon/Sanyo etc. uses IFD format for this area. 
		/// undefined
		/// </summary>
		MakerNote =0x927c,
		/// <summary>
		/// Stores user comment. This tag allows to use two-byte character code or unicode. First 8 bytes describe the character code. 'JIS' is a Japanese character code (known as Kanji).
		///	'0x41,0x53,0x43,0x49,0x49,0x00,0x00,0x00':ASCII
		///'0x4a,0x49,0x53,0x00,0x00,0x00,0x00,0x00':JIS
		///'0x55,0x4e,0x49,0x43,0x4f,0x44,0x45,0x00':Unicode
		///'0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00':Undefined
		///  undefined 
		/// </summary>
		UserComment =0x9286,
		/// <summary>
		///Some of digicam can take 2~30 pictures per second, but DateTime/DateTimeOriginal/DateTimeDigitized tag can't record the sub-second time. SubsecTime tag is used to record it.
		/// For example, DateTimeOriginal = "1996:09:01 09:15:30", SubSecTimeOriginal = "130", Combined original time is "1996:09:01 09:15:30.130" 
		/// ascii string 
		/// </summary>
		SubsecTime  =0x9290,
		/// <summary>
		/// See SubsecTime
		///  ascii string 
		/// </summary>
		SubsecTimeOriginal =0x9291,
		/// <summary>
		/// See SubsecTime
		/// ascii string 
		/// </summary>
		SubsecTimeDigitized = 0x9292,
		/// <summary>
		/// Stores FlashPix version. If the image data is based on FlashPix formar Ver.1.0, value is "0100". Since the type is 'undefined', there is no NULL(0x00) for termination.  
		/// undefined 4  
		/// </summary>
		FlashPixVersion = 0xa000,
		/// <summary>
		/// Defines Color Space. DCF image must use sRGB color space so value is always '1'. If the picture uses the other color space, value is '65535':Uncalibrated. 
		/// unsigned short 1  
		/// </summary>
		ColorSpace = 0xa001,
		/// <summary>
		///  Size of main image. 
		///  unsigned short/long 1  
		/// </summary>
		ExifImageWidth = 0xa002,
		/// <summary>
		/// Size of main image
		///  unsigned short/long 1  
		/// </summary>
		ExifImageHeight  = 0xa003,
		/// <summary>
		/// If this digicam can record audio data with image, shows name of audio data. 
		/// ascii string 
		/// </summary>
		RelatedSoundFile = 0xa004,
		/// <summary>
		/// Extension of "ExifR98", detail is unknown. This value is offset to IFD format data. Currently there are 2 directory entries, first one is Tag0x0001, value is "R98", next is Tag0x0002, value is "0100". 
		/// unsigned long 1  
		/// </summary>
		ExifInteroperabilityOffset  = 0xa005,
		/// <summary>
		///   Pixel density at CCD's position. If you have MegaPixel digicam and take a picture by lower resolution(e.g.VGA mode), this value is re-sampled by picture resolution. In such case, FocalPlaneResolution is not same as CCD's actual resolution. 
		///   unsigned rational 1  
		/// </summary>
		FocalPlaneXResolution =0xa20e,
		/// <summary>
		/// See FocalPlaneXResolution
		///  unsigned rational 1  
		/// </summary>
		FocalPlaneYResolution = 0xa20f,
		/// <summary>
		///  Unit of FocalPlaneXResoluton/FocalPlaneYResolution. '1' means no-unit, '2' inch, '3' centimeter. 
		///  Note:Some of Fujifilm's digicam(e.g.FX2700,FX2900,Finepix4700Z/40i etc) uses value '3' so it must be 'centimeter', but it seems that they use a '8.3mm?'(1/3in.?) to their ResolutionUnit. Fuji's BUG? Finepix4900Z has been changed to use value '2' but it doesn't match to actual value also. 
		///  unsigned short 1  
		/// </summary>
		FocalPlaneResolutionUnit =0xa210,
		/// <summary>
		///  Same as ISOSpeedRatings(0x8827) but data type is unsigned rational. Only Kodak's digicam uses this tag instead of ISOSpeedRating, I don't know why(historical reason?).  
		///  unsigned rational 1  
		/// </summary>
		ExposureIndex =0xa215,
		/// <summary>
		///  Shows type of image sensor unit. '2' means 1 chip color area sensor, most of all digicam use this type. 
		///  unsigned short 1  
		/// </summary>
		SensingMethod =0xa217 ,
		/// <summary>
		///  Indicates the image source. Value '0x03' means the image source is digital still camera. 
		///  undefined 1  
		/// </summary>
		FileSource =0xa300,
		/// <summary>
		///  Indicates the type of scene. Value '0x01' means that the image was directly photographed. 
		///  undefined 1
		/// </summary>
		SceneType =0xa301,
		/// <summary>
		/// Indicates the Color filter array(CFA) geometric pattern.
		/// Length Type Meaning 
		/// 2 short Horizontal repeat pixel unit = n 
		/// 2 short Vertical repeat pixel unit = m 
		/// 1 byte CFA value[0,0] 
		/// 1 byte CFA value[n-1,0] 
		/// 1 byte CFA value[0,1] 
		/// 1 byte CFA value[n-1,m-1] 
		///
		///The relation of filter color to CFA value is shown below.
		/// Red = 0 Green = 1 Blue = 2 Cyan = 3 Magenta = 4 Yellow = 5 White = 6 
		/// 
		/// undefined 
		/// </summary>
		CFAPattern  =0xa302,
		/// <summary>
		/// If this IFD is main image's IFD and the file content is equivalent to ExifR98 v1.0, the value is "R98". If thumbnail image's, value is "THM". 
		/// Ascii string  4  
		/// </summary>
		InteroperabilityIndex = 0x0001,
		/// <summary>
		///  Records the interoperability version. "0100" means version 1.00. 
		///  Undefined  4  
		/// </summary>
		InteroperabilityVersion =0x0002 ,
		/// <summary>
		/// Records the file format of image file. Value is ascii string (e.g. "Exif JPEG Ver. 2.1"). 
		/// Ascii string  any
		/// </summary>
		RelatedImageFileFormat = 0x1000,
		/// <summary>
		///  Records the image size. 
		///  Short or Long  1  
		/// </summary>
		RelatedImageWidth  =0x1001,
		/// <summary>
		/// Records the image size
		/// Short or Long  1  
		/// </summary>
		RelatedImageLength  = 0x1001,
		/// <summary>
		///  Shows size of thumbnail image. 
		///  unsigned short/long 1  
		/// </summary>
		ImageWidth = 0x0100,
		/// <summary>
		///  Shows size of thumbnail image. 
		/// unsigned short/long 1  
		/// </summary>
		ImageLength  = 0x0101,
		/// <summary>
		///  When image format is no compression, this value shows the number of bits per component for each pixel. Usually this value is '8,8,8' 
		///  unsigned short 3  
		/// </summary>
		BitsPerSample =0x0102,
		/// <summary>
		///  Shows compression method. '1' means no compression, '6' means JPEG compression. 
		///  unsigned short 1  
		/// </summary>
		Compression  =0x0103,
		/// <summary>
		///  Shows the color space of the image data components. '1' means monochrome, '2' means RGB, '6' means YCbCr. 
		///  unsigned short 1  
		/// </summary>
		PhotometricInterpretation  = 0x0106,
		/// <summary>
		/// When image format is no compression, this value shows offset to image data. In some case image data is striped and this value is plural. 
		/// unsigned short/long 
		/// </summary>
		StripOffsets = 0x0111,
		/// <summary>
		///  When image format is no compression, this value shows the number of components stored for each pixel. At color image, this value is '3'. 
		///  unsigned short 1  
		/// </summary>
		SamplesPerPixel  = 0x0115,
		/// <summary>
		/// When image format is no compression and image has stored as strip, this value shows how many rows stored to each strip. If image has not striped, this value is the same as ImageLength(0x0101). 
		/// unsigned short/long 1  
		/// </summary>
		RowsPerStrip = 0x0116,
		/// <summary>
		/// When image format is no compression and stored as strip, this value shows how many bytes used for each strip and this value is plural. If image has not stripped, this value is single and means whole data size of image. 
		/// unsigned short/long 
		/// </summary>
		StripByteConunts = 0x0117 ,
		/// <summary>
		///  When image format is no compression YCbCr, this value shows byte aligns of YCbCr data. If value is '1', Y/Cb/Cr value is chunky format, contiguous for each subsampling pixel. If value is '2', Y/Cb/Cr value is separated and stored to Y plane/Cb plane/Cr plane format. 
		///  unsigned short 1  
		/// </summary>
		PlanarConfiguration  = 0x011c,
		/// <summary>
		/// When image format is JPEG, this value show offset to JPEG data stored. 
		/// unsigned long 1  
		/// </summary>
		JpegIFOffset  = 0x0201,
		/// <summary>
		///   When image format is JPEG, this value shows data size of JPEG image. 
		///   unsigned long 1  
		/// </summary>
		JpegIFByteCount =0x0202,
		/// <summary>
		///  When image format is YCbCr and uses subsampling(cropping of chroma data, all the digicam do that), this value shows how many chroma data subsampled. First value shows horizontal, next value shows vertical subsample rate. 
		///  unsigned short 2  
		/// </summary>
		YCbCrSubSampling =0x0212 ,
	}

	/// <summary>
	/// Enumerates EXIF Property Type Codes
	/// See http://www.ba.wakwak.com/~tsuruzoh/Computer/Digicams/exif-e.html
	/// </summary>
	public enum EXIFPropertyTypes
	{
		/// <summary>
		/// Data contains unsigned bytes
		/// </summary>
		ExifTypeUnsignedByte = 1,
		/// <summary>
		/// Data contains a string
		/// </summary>
		ExifTypeString = 2,
		/// <summary>
		/// Data contains unsigned 2-byte values
		/// </summary>
		ExifTypeUnsignedChar = 3,
		/// <summary>
		/// Data contains unsigned 4-byte values
		/// </summary>
		ExifTypeUnsignedInt = 4,
		/// <summary>
		/// Data is fractional: each item in the data is 8 bytes long.
		/// The first 4 bytes of each item in the data contain the
		/// numerator (unsigned int), the second 4 bytes the denominator
		/// (also unsigned int).
		/// </summary>
		ExifTypeUnsignedRational = 5,
		/// <summary>
		/// Data contains signed bytes
		/// </summary>
		ExifTypeSignedByte = 6,
		/// <summary>
		/// Data has arbitrary data type, tag specific
		/// </summary>
		ExifTypeUndefined = 7,
		/// <summary>
		/// Data contains signed 2-byte values
		/// </summary>
		ExifTypeSignedChar = 8,
		/// <summary>
		/// Data contains signed 4-byte values
		/// </summary>
		ExifTypeSignedInt = 9,
		/// <summary>
		/// Data is fractional: each item in the data is 8 bytes long.
		/// The first 4 bytes of each item in the data contain the
		/// numerator (signed int), the second 4 bytes the denominator
		/// (also signed int).
		/// </summary>
		ExifTypeSignedRational = 10,
		/// <summary>
		/// Data contains 4-byte floating point values
		/// </summary>
		ExifTypeFloat = 11,
		/// <summary>
		/// Data contains 8-byte floating point values
		/// </summary>
		ExifTypeDouble = 12
	}

	/// <summary>
	/// A structure containing an EXIFRational value
	/// </summary>
	public struct EXIFRational
	{
		/// <summary>
		/// 
		/// </summary>
		public int Denominator;
		public int Numerator;

		/// <summary>
		/// Creates an EXIFRational object from EXIF byte data
		/// </summary>
		/// <param name="data">Byte data to create from</param>
		public EXIFRational(byte[] data)
		{
			Denominator = 0;
			Numerator = 0;
			if (data.Length >= 8)
			{
				Numerator = data[0];
				Numerator |= (data[1] >> 8);
				Numerator |= (data[2] >> 16);
				Numerator |= (data[3] >> 24);
				Denominator = data[4];
				Denominator |= (data[5] >> 8);
				Denominator |= (data[6] >> 16);
				Denominator |= (data[7] >> 24);
			}
		}
		/// <summary>
		/// Creates an EXIFRational object from EXIF byte data,
		/// starting at the byte specified by ofs
		/// </summary>
		/// <param name="data">EXIF byte data</param>
		/// <param name="ofs">Initial Byte</param>
		public EXIFRational(byte[] data, int ofs)
		{
			Denominator = 0;
			Numerator = 0;
			if (data.Length >= ofs + 8)
			{
				Numerator = data[ofs];
				Numerator |= (data[ofs + 1] >> 8);
				Numerator |= (data[ofs + 2] >> 16);
				Numerator |= (data[ofs + 3] >> 24);
				Denominator = data[ofs + 4];
				Denominator |= (data[ofs + 5] >> 8);
				Denominator |= (data[ofs + 6] >> 16);
				Denominator |= (data[ofs + 7] >> 24);
			}
		}
		/// <summary>
		/// Returns the value of the fraction as a string
		/// </summary>
		/// <returns>The value of the fraction as a string</returns>
		public override string ToString()
		{
			string ret;
			if (this.Denominator == 0)
			{
				ret = "N/A";
			}
			else
			{
				ret = String.Format("{0:F2}", this.Numerator * 1.0/this.Denominator);
			}
			return ret;
		}
	}

	/// <summary>
	/// A class to assist in converting .NET PropertyItem
	/// objects into usable data
	/// </summary>
	public class EXIFPropertyItem
	{
		private KnownEXIFIDCodes exifCode;
		private EXIFPropertyTypes exifType;
		private int id;
		private int type;
		private int len;
		private byte[] data;

		public EXIFPropertyTypes EXIFType
		{
			get
			{
				return this.exifType;
			}
		}
		public KnownEXIFIDCodes EXIFCode
		{
			get
			{
				return this.exifCode;
			}
		}
		public byte[] Value
		{
			get
			{
				return this.data;
			}
		}
		public string ParsedString
		{
			get
			{
				string parsed = "";
				if (this.data.Length > 1)
				{
					IntPtr h = Marshal.AllocHGlobal(this.data.Length);
					int i = 0;
					foreach (byte b in this.data)
					{
						Marshal.WriteByte(h, i, b);
						i++;
					}
					// qu: can any of these items be non-ansi?
					parsed = Marshal.PtrToStringAuto(h);
					//parsed = Marshal.PtrToStringAnsi(h);
					Marshal.FreeHGlobal(h);
				}
				return parsed;
			}
		}
		public EXIFRational ParsedRational
		{
			get
			{
				EXIFRational parsed = new EXIFRational(this.data);
				return parsed;
			}
		}
		public EXIFRational[] ParsedRationalArray
		{
			get
			{
				EXIFRational[] parsed = null;
				int arraySize = (this.data.Length / 8);
				if (arraySize > 0)
				{
					parsed = new EXIFRational[arraySize];
					for (int i = 0; i < arraySize; i++)
					{
						parsed[i] = new EXIFRational(this.data, i * 8);
					}
				}
				return parsed;
			}
		}
		public DateTime ParsedDate
		{
			get
			{
				DateTime ret = new DateTime(1900,1,1,0,0,0);
				
				
				string date = "";
				if (this.data.Length > 1)
				{
					IntPtr h = Marshal.AllocHGlobal(this.data.Length);
					int i = 0;
					foreach (byte b in this.data)
					{
						Marshal.WriteByte(h, i, b);
						i++;
					}
					date = Marshal.PtrToStringAnsi(h);
					Marshal.FreeHGlobal(h);
				}

				
				if (date.Length >= 19)
				{
					try
					{
						int year = int.Parse(date.Substring(0, 4));
						int month = int.Parse(date.Substring(5,2));
						int day = int.Parse(date.Substring(8,2));
						int hour = int.Parse(date.Substring(11,2));
						int minute = int.Parse(date.Substring(14,2));
						int second = int.Parse(date.Substring(17,2));
						ret = new DateTime(year, month, day, hour, minute, second);
					}
					catch {}
				}
				return ret;
			}
		}
		public bool IsKnownCode
		{
			get
			{
				return (exifCode != KnownEXIFIDCodes.UnknownCode);
			}
		}

		public EXIFPropertyItem(PropertyItem prop)
		{
			// Does KnownEXIFCodes have a corresponding entry?
			if (Enum.IsDefined(exifCode.GetType(), prop.Id))
			{
				exifCode = (KnownEXIFIDCodes)prop.Id;		
			}
			else
			{
				exifCode = KnownEXIFIDCodes.UnknownCode;
			}
			exifType = (EXIFPropertyTypes)prop.Type;
			data = prop.Value;
			this.type = prop.Type;
			this.id = prop.Id;
			this.len = prop.Len;

			/*
			Console.WriteLine("{0:X8} {1} {2} {3}", prop.Id, prop.Type, prop.Len, Enum.GetName(exifCode.GetType(), exifCode));
			if (exifType == EXIFPropertyTypes.ExifTypeString)
			{
				Console.WriteLine(this.ParsedString);
			}
			/*
			foreach (byte b in data)
			{
				Console.Write("{0:X2} ", b);
			}
			Console.WriteLine("");
			*/
		}
	}
}
