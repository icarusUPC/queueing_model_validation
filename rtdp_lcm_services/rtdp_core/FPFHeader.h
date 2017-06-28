/*
 * FPFHeader.h
 *
 *  Created on: 26/02/2015
 *      Author: esther
 */

#ifndef FPFHEADER_H_
#define FPFHEADER_H_

typedef struct
{
   char fpfID[32];                 /*  "FLIR Public Image Format" */
   unsigned int  version;         /*  = 1 */
   unsigned int  pixelOffset;     /*  Offset to pixel values from start of fpfID. */
   unsigned short ImageType;       /*  Temperature = 0,
                                       Diff Temp = 2,
                                       Object Signal = 4,
                                       Diff Object Signal = 5, etc */
   unsigned short pixelFormat;     /*  0 = short integer = 2 bytes
                                       1 = int integer = 4 bytes
                                       2 = float (single precision)  = 4 bytes
                                       3 = double (double precision) = 8 bytes */

   unsigned short xSize;
   unsigned short ySize;

   unsigned int  trig_count;      /* external trig counter */
   unsigned int  frame_count;     /* frame number in sequence */

   int  spareLong[16];            /* = 0 */
} FPF_IMAGE_DATA_T;

/*  String lengths    */
#define FPF_CAMERA_TYPE_LEN   31   /* Camera name string */
#define FPF_CAMERA_PARTN_LEN  31   /* Camera part number string */
#define FPF_CAMERA_SN_LEN     31   /* Scanner serial number string */
#define FPF_LENS_TYPE_LEN     31   /* Lens name string */
#define FPF_LENS_PARTN_LEN    31   /* Lens part number string */
#define FPF_LENS_SN_LEN       31   /* Lens serial number string */
#define FPF_FILTER_TYPE_LEN   31   /* Filter name string */
#define FPF_FILTER_PARTN_LEN  31   /* Filter part number string */
#define FPF_FILTER_SN_LEN     31   /* Filter serial number string */

typedef struct
{
    char camera_name[FPF_CAMERA_TYPE_LEN+1];
    char camera_partn[FPF_CAMERA_PARTN_LEN+1];
    char camera_sn[FPF_CAMERA_SN_LEN+1];

    float camera_range_tmin;
    float camera_range_tmax;

    char lens_name[FPF_LENS_TYPE_LEN+1];
    char lens_partn[FPF_LENS_PARTN_LEN+1];
    char lens_sn[FPF_LENS_SN_LEN+1];

    char filter_name[FPF_FILTER_TYPE_LEN+1];
    char filter_partn[FPF_FILTER_PARTN_LEN+1];
    char filter_sn[FPF_FILTER_SN_LEN+1];

   int  spareLong[16];            /* = 0 */
}FPF_CAMDATA_T;

typedef struct
{
   float emissivity;               /* 0 - 1 */
   float objectDistance;           /* Meters */
   float ambTemp;                  /* Reflected Ambient temperature in Kelvin */
   float atmTemp;                  /* Atmospheric temperature in Kelvin */
   float relHum;                   /* 0 - 1 */
   float compuTao;                 /* Computed atmospheric transmission 0 - 1*/
   float estimTao;                 /* Estimated atmospheric transmission 0 - 1*/
   float refTemp;                  /* Reference temperature in Kelvin */
   float extOptTemp;               /* Kelvin */
   float extOptTrans;              /* 0 - 1 */

   int  spareLong[16];            /* = 0 */
} FPF_OBJECT_PAR_T;

typedef struct
{
   int Year;
   int Month;
   int Day;
   int Hour;
   int Minute;
   int Second;
   int MilliSecond;

   int  spareLong[16];            /* = 0 */
} FPF_DATETIME_T;

typedef struct
{
   float tMinCam;                  /* Camera scale min, in current output */
   float tMaxCam;                  /* Camera scale max */
   float tMinCalc;                 /* Calculated min (almost true min) */
   float tMaxCalc;                 /* Calculated max (almost true max) */
   float tMinScale;                /* Scale min */
   float tMaxScale;                /* Scale max */
   int  spareLong[16];            /* = 0 */

} FPF_SCALING_T;

typedef struct
{
   FPF_IMAGE_DATA_T  imgData;
   FPF_CAMDATA_T     camData;
   FPF_OBJECT_PAR_T  objPar;
   FPF_DATETIME_T    datetime;
   FPF_SCALING_T     scaling;
   int  spareLong[32];            /* = 0 */

} FPFHEADER_T;
#endif /* FPFHEADER_H_ */
