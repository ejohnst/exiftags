/*
 * Copyright (c) 2001, 2002, Eric M. Johnston <emj@postal.net>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *      This product includes software developed by Eric M. Johnston.
 * 4. Neither the name of the author nor the names of any co-contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * $Id: tagdefs.c,v 1.7 2002/08/01 03:10:35 ejohnst Exp $
 */

/*
 * Exif tag definitions.
 *
 * Developed using the TIFF 6.0 specification:
 * (http://partners.adobe.com/asn/developer/pdfs/tn/TIFF6.pdf)
 * and the EXIF 2.1 standard:
 * (http://www.pima.net/standards/it10/PIMA15740/Exif_2-1.PDF)
 */

#include <string.h>

#include "exif.h"
#include "exifint.h"
#include "makers.h"


/* TIFF 6.0 field types. */

struct fieldtype ftypes[] = {
	{ TIFF_BYTE,	"byte",		1 },
	{ TIFF_ASCII,	"ascii",	1 },
	{ TIFF_SHORT,	"short",	2 },
	{ TIFF_LONG,	"long",		4 },
	{ TIFF_RTNL,	"rational",	8 },
	{ TIFF_SBYTE,	"sbyte",	1 },	/* not in Exif 2.1 */
	{ TIFF_UNDEF,	"undefined",	1 },
	{ TIFF_SSHORT,	"sshort",	2 },	/* not in Exif 2.1 */
	{ TIFF_SLONG,	"slong",	4 },
	{ TIFF_SRTNL,	"srational",	8 },
	{ TIFF_FLOAT,	"float",	4 },	/* not in Exif 2.1 */
	{ TIFF_DBL,	"double",	8 },	/* not in Exif 2.1 */
	{ TIFF_UNKN,	"unknown",	0 },
};


/* Exif 2.1 tags. */

struct exiftag tags[] = {
	{ 0x0100, TIFF_UNKN,  1,  ED_IMG, 		/* columns */
	    "ImageWidth", "Image Width" },
	{ 0x0101, TIFF_UNKN,  1,  ED_IMG, 		/* rows */
	    "ImageLength", "Image Height" },
	{ 0x0102, TIFF_SHORT, 3,  ED_IMG, 		/* bits */
	    "BitsPerSample", "Number of Bits Per Component" },
	{ 0x0103, TIFF_SHORT, 1,  ED_IMG,
	    "Compression", "Compression Scheme" },
	{ 0x0106, TIFF_SHORT, 1,  ED_IMG,
	    "PhotometricInterpretation", "Pixel Composition" },
	{ 0x010a, TIFF_UNKN,  0,  ED_UNK,
	    "FillOrder", NULL },
	{ 0x010d, TIFF_UNKN,  0,  ED_UNK,
	    "DocumentName", NULL },
	{ 0x010e, TIFF_ASCII, 0,  ED_UNK,
	    "ImageDescription", "Title" },
	{ 0x010f, TIFF_ASCII, 0,  ED_CAM,
	    "Make", "Equipment Make" },
	{ 0x0110, TIFF_ASCII, 0,  ED_CAM,
	    "Model", "Camera Model" },
	{ 0x0111, TIFF_UNKN,  0,  ED_VRB,		/* bytes */
	    "StripOffsets", "Image Data Location" },
	{ 0x0112, TIFF_SHORT, 1,  ED_IMG,
	    "Orientation", "Image Orientation" },
	{ 0x0115, TIFF_SHORT, 1,  ED_VRB,
	    "SamplesPerPixel", "Number of Components" },
	{ 0x0116, TIFF_UNKN,  1,  ED_VRB,		/* rows */
	    "RowsPerStrip", "Number of Rows Per Strip" },
	{ 0x0117, TIFF_UNKN,  0,  ED_VRB,		/* bytes */
	    "StripByteCounts", "Bytes per Compressed Strip" },
	{ 0x011a, TIFF_RTNL,  1,  ED_IMG,		/* dp[i|cm] */
	    "XResolution", "Horizontal Resolution" },
	{ 0x011b, TIFF_RTNL,  1,  ED_IMG,		/* dp[i|cm] */
	    "YResolution", "Vertical Resolution" },
	{ 0x011c, TIFF_SHORT, 1,  ED_IMG,
	    "PlanarConfiguration", "Image Data Arrangement" },
	{ 0x0128, TIFF_SHORT, 1,  ED_VRB,
	    "ResolutionUnit", "Resolution Unit" },
	{ 0x012d, TIFF_SHORT, 0,  ED_VRB,
	    "TransferFunction", "Transfer Function" },
	{ 0x0131, TIFF_ASCII, 0,  ED_CAM,
	    "Software", "Camera Software" },
	{ 0x0132, TIFF_ASCII, 20, ED_IMG,
	    "DateTime", "Image Created" },
	{ 0x013b, TIFF_ASCII, 0,  ED_CAM,
	    "Artist", "Photographer" },
	{ 0x013e, TIFF_RTNL,  2,  ED_IMG,
	    "WhitePoint", "White Point Chromaticity" },
	{ 0x013f, TIFF_RTNL,  6,  ED_VRB,
	    "PrimaryChromaticities", "Chromaticities of Primary Colors" },
	{ 0x0156, TIFF_UNKN,  0,  ED_UNK,
	    "TransferRange", NULL },
	{ 0x0200, TIFF_UNKN,  0,  ED_UNK,
	    "JPEGProc", NULL },
	{ 0x0201, TIFF_LONG,  1,  ED_VRB,
	    "JPEGInterchangeFormat", "Offset to JPEG SOI" },
	{ 0x0202, TIFF_LONG,  1,  ED_VRB,		/* bytes */
	    "JPEGInterchangeFormatLength", "Bytes of JPEG Data" },
	{ 0x0211, TIFF_RTNL,  3,  ED_VRB,
	    "YCbCrCoefficients", "Color Space Transform Matrix Coefficients" },
	{ 0x0212, TIFF_SHORT, 2,  ED_VRB,
	    "YCbCrSubSampling", "Chrominance Components Sampling Ratio" },
	{ 0x0213, TIFF_SHORT, 1,  ED_VRB,
	    "YCbCrPositioning", "Chrominance Components Positioning" },
	{ 0x0214, TIFF_RTNL,  6,  ED_VRB,
	    "ReferenceBlackWhite", "Black and White Reference Point Values" },
	{ 0x828d, TIFF_UNKN,  0,  ED_UNK,
	    "CFARepeatPatternDim", NULL },
	{ 0x828e, TIFF_UNKN,  0,  ED_UNK,
	    "CFAPattern", NULL },
	{ 0x828f, TIFF_UNKN,  0,  ED_UNK,
	    "BatteryLevel", NULL },
	{ 0x8298, TIFF_ASCII, 0,  ED_UNK,
	    "Copyright", "Copyright" },
	{ 0x829a, TIFF_RTNL,  1,  ED_IMG,		/* s */
	    "ExposureTime", "Exposure Time" },
	{ 0x829d, TIFF_RTNL,  1,  ED_IMG,
	    "FNumber", "F-Number" },
	{ 0x83bb, TIFF_UNKN,  0,  ED_UNK,
	    "IPTC/NAA", NULL },
	{ 0x8769, TIFF_LONG,  1,  ED_VRB,
	    "ExifOffset", "Exif IFD Pointer" },
	{ 0x8773, TIFF_UNKN,  0,  ED_UNK,
	    "InterColorProfile", NULL },
	{ 0x8822, TIFF_SHORT, 1,  ED_IMG,
	    "ExposureProgram", "Exposure Program" },
	{ 0x8824, TIFF_ASCII, 0,  ED_CAM,
	    "SpectralSensitivity", "Spectral Sensitivity" },
	{ 0x8825, TIFF_LONG,  1,  ED_UNK,
	    "GPSInfo", "GPS Info IFD Pointer" },
	{ 0x8827, TIFF_SHORT, 0,  ED_CAM,
	    "ISOSpeedRatings", "ISO Speed Rating" },
	{ 0x8828, TIFF_UNDEF, 0,  ED_CAM,
	    "OECF", "Opto-Electric Conversion Factor" },
	{ 0x9000, TIFF_UNDEF, 4,  ED_VRB,
	    "ExifVersion", "Exif Version" },
	{ 0x9003, TIFF_ASCII, 20, ED_VRB,
	    "DateTimeOriginal", "Image Generated" },
	{ 0x9004, TIFF_ASCII, 20, ED_VRB,
	    "DateTimeDigitized", "Image Digitized" },
	{ 0x9101, TIFF_UNDEF, 4,  ED_VRB,
	    "ComponentsConfiguration", "Meaning of Each Component" },
	{ 0x9102, TIFF_RTNL,  1,  ED_VRB,
	    "CompressedBitsPerPixel", "Image Compression Mode" },
	{ 0x9201, TIFF_SRTNL, 1,  ED_IMG,		/* s */
	    "ShutterSpeedValue", "Shutter Speed" },
	{ 0x9202, TIFF_RTNL,  1,  ED_IMG,
	    "ApertureValue", "Lens Aperture" },
	{ 0x9203, TIFF_SRTNL, 1,  ED_IMG,
	    "BrightnessValue", "Brightness" },
	{ 0x9204, TIFF_SRTNL, 1,  ED_IMG,
	    "ExposureBiasValue", "Exposure Bias" },
	{ 0x9205, TIFF_RTNL,  1,  ED_CAM,
	    "MaxApertureValue", "Maximum Lens Aperture" },
	{ 0x9206, TIFF_RTNL,  1,  ED_IMG,		/* m */
	    "SubjectDistance", "Subject Distance" },
	{ 0x9207, TIFF_SHORT, 1,  ED_IMG,
	    "MeteringMode", "Metering Mode" },
	{ 0x9208, TIFF_SHORT, 1,  ED_IMG,
	    "LightSource", "Light Source" },
	{ 0x9209, TIFF_SHORT, 1,  ED_IMG,
	    "Flash", "Flash" },
	{ 0x920a, TIFF_RTNL,  1,  ED_IMG,		/* mm */
	    "FocalLength", "Focal Length" },
	{ 0x927c, TIFF_UNDEF, 0,  ED_UNK,
	    "MakerNote", "Manufacturer Notes" },
	{ 0x9286, TIFF_UNDEF, 0,  ED_UNK,
	    "UserComment", "Comments" },
	{ 0x9290, TIFF_ASCII, 0,  ED_IMG,
	    "SubsecTime", "DateTime Second Fraction" },
	{ 0x9291, TIFF_ASCII, 0,  ED_IMG,
	    "SubsecTimeOrginal", "DateTimeOriginal Second Fraction" },
	{ 0x9292, TIFF_ASCII, 0,  ED_IMG,
	    "SubsecTimeDigitized", "DateTimeDigitized Second Fraction" },
	{ 0xa000, TIFF_UNDEF, 4,  ED_UNK,
	    "FlashPixVersion", "Supported FlashPix Version" },
	{ 0xa001, TIFF_SHORT, 1,  ED_IMG,
	    "ColorSpace", "Color Space Information" },
	{ 0xa002, TIFF_UNKN,  1,  ED_IMG,		/* pixels */
	    "PixelXDimension", "Image Width" },
	{ 0xa003, TIFF_UNKN,  1,  ED_IMG,		/* pixels */
	    "PixelYDimension", "Image Height" },
	{ 0xa004, TIFF_ASCII, 13, ED_UNK,
	    "RelatedSoundFile", "Related Audio File" },
	{ 0xa005, TIFF_LONG,  1,  ED_UNK,
	    "InteroperabilityOffset", "Interoperability IFD Pointer" },
	{ 0xa20b, TIFF_RTNL,  1,  ED_IMG,		/* bcps */
	    "FlashEnergy", "Flash Energy" },
	{ 0xa20c, TIFF_UNDEF, 0,  ED_VRB,
	    "SpatialFrequencyResponse", "Spatial Frequency Response" },
	{ 0xa20e, TIFF_RTNL,  1,  ED_VRB,		/* dp[i|cm] */
	    "FocalPlaneXResolution", "Focal Plane Horizontal Resolution" },
	{ 0xa20f, TIFF_RTNL,  1,  ED_VRB,		/* dp[i|cm] */
	    "FocalPlaneYResolution", "Focal Plane Vertical Resolution" },
	{ 0xa210, TIFF_SHORT, 1,  ED_VRB,
	    "FocalPlaneResolutionUnit", "Focal Plane Resolution Unit" },
	{ 0xa214, TIFF_SHORT, 2,  ED_VRB,
	    "SubjectLocation", "Subject Location" },
	{ 0xa215, TIFF_RTNL,  1,  ED_IMG,
	    "ExposureIndex", "Exposure Index" },
	{ 0xa217, TIFF_SHORT, 1,  ED_CAM,
	    "SensingMethod", "Sensing Method" },
	{ 0xa300, TIFF_UNDEF, 1,  ED_VRB,
	    "FileSource", "File Source" },
	{ 0xa301, TIFF_UNDEF, 1,  ED_VRB,
	    "SceneType", "Scene Type" },
	{ 0xa302, TIFF_UNDEF, 0,  ED_CAM,
	    "CFAPattern", "Color Filter Array Pattern" },
	{ 0xffff, TIFF_UNKN,  0,  ED_UNK,
	    "Unknown", NULL },
};


/*
 * Maker note lookup table.
 */

struct makerfun makers[] = {
	{ EXIF_MKR_CANON, "canon", canon_prop, readifds },
	{ EXIF_MKR_OLYMPUS, "olympus", olympus_prop, olympus_ifd },
	{ EXIF_MKR_FUJI, "fujifilm", fuji_prop, fuji_ifd },
	{ EXIF_MKR_NIKON, "nikon", nikon_prop, nikon_ifd },
	{ EXIF_MKR_UNKNOWN, "unknown", NULL, NULL },
};


/*
 * Various tag value lookup tables.  All are terminated by the value -1.
 */


/* Compression schemes. */

struct descrip compresss[] = {
	{ 1,	"Uncompressed" },
	{ 6,	"JPEG Compression (Thumbnail)" },
	{ -1,	"Unknown" },
};


/* Pixel compositions. */

struct descrip pixelcomps[] = {
	{ 2,	"RGB" },
	{ 6,	"YCbCr" },
	{ -1,	"Unknown" },
};


/* Image orientation in terms of rows and columns. */

struct descrip orients[] = {
	{ 1,	"Top, Left-Hand" },
	{ 2,	"Top, Right-Hand" },
	{ 3,	"Bottom, Right-Hand" },
	{ 4,	"Bottom, Left-Hand" },
	{ 5,	"Left-Hand, Top" },
	{ 6,	"Right-Hand, Top" },
	{ 7,	"Right-Hand, Bottom" },
	{ 8,	"Left-Hand, Bottom" },
	{ -1,	"Unknown" },
};


/* Planar configurations. */

struct descrip planarconfigs[] = {
	{ 1,	"Chunky Format" },
	{ 2, 	"Planar Format" },
	{ -1,	"Unknown" },
};


/* Resolution units. */

struct descrip resunits[] = {
	{ 2,	"i" },
	{ 3,	"cm" },
	{ -1,	"" },
};


/* Chrominance components sampling ratio. */
/* Note: This only refers to the second short; first is assumed to be 2. */

struct descrip chromratios[] = {
	{ 1,	"YCbCr4:2:2" },
	{ 2,	"YCbCr4:2:0" },
	{ -1,	"Unknown" },
};


/* Chrominance components positioning. */

struct descrip chrompos[] = {
	{ 1,	"Centered" },
	{ 2,	"Co-Sited" },
	{ -1,	"Unknown" },
};


/* Exposure programs. */

struct descrip expprogs[] = {
	{ 0,	"Not Defined" },
	{ 1,	"Manual" },
	{ 2,	"Normal Program" },
	{ 3,	"Aperture Priority" },
	{ 4,	"Shutter Priority" },
	{ 5,	"Creative" },
	{ 6,	"Action" },
	{ 7,	"Portrait Mode" },
	{ 8,	"Landscape Mode" },
	{ -1,	"Unknown" },
};


/* Component configuration. */

struct descrip compconfig[] = {
	{ 0,	"Does Not Exist" },
	{ 1,	"Y" },
	{ 2,	"Cb" },
	{ 3,	"Cr" },
	{ 4,	"R" },
	{ 5,	"G" },
	{ 6,	"B" },
	{ -1,	"Unknown" },
};


/* Metering modes. */

struct descrip metermodes[] = {
	{ 0,	"Unknown" },
	{ 1,	"Average" },
	{ 2,	"Center Weighted Average" },
	{ 3,	"Spot" },
	{ 4,	"Multi Spot" },
	{ 5,	"Pattern" },
	{ 6,	"Partial" },
	{ 255,	"Other" },
	{ -1,	"Unknown" },
};


/* Light sources. */

struct descrip lightsrcs[] = {
	{ 0,	"Unknown" },
	{ 1,	"Daylight" },
	{ 2,	"Fluorescent" },
	{ 3,	"Tungsten" },
	{ 17,	"Standard Light A" },
	{ 18,	"Standard Light B" },
	{ 19,	"Standard Light C" },
	{ 20,	"D55" },
	{ 21,	"D65" },
	{ 22,	"D75" },
	{ 255,	"Other" },
	{ -1,	"Unknown" },
};


/* Flash modes. */

struct descrip flashes[] = {
	{ 0,	"No Flash" },
	{ 1,	"Flash" },
	{ 2,	"No Flash" },
	{ 3,	"Flash" },
	{ 4,	"No Flash" },
	{ 5,	"Flash, Strobe Not Detected" },
	{ 6,	"No Flash" },
	{ 7,	"Flash, Strobe Detected" },
	{ -1,	"No Flash" },
};


/* Color spaces. */

struct descrip colorspcs[] = {
	{ 1,	"sRGB" },
	{0xffff,"Uncalibrated" },
	{ -1,	"Unknown" },
};


/* Image sensor types. */

struct descrip imgsensors[] = {
	{ 1,	"Not Defined" },
	{ 2,	"One-Chip Color Area" },
	{ 3,	"Two-Chip Color Area" },
	{ 4,	"Three-Chip Color Area" },
	{ 5,	"Color Sequential Area" },
	{ 7,	"Trilinear" },
	{ 8,	"Color Sequential Linear" },
	{ -1,	"Unknown" },
};


/* File sources */

struct descrip filesrcs[] = {
	{ 3,	"DSC" },
	{ -1,	"Unknown" },
};


/* Scene types. */

struct descrip scenetypes[] = {
	{ 1,	"Directly Photographed" },
	{ -1,	"Unknown" },
};
