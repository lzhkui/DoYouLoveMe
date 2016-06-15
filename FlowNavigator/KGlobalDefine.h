/************************************************************************/
/* 全局的宏定义                                                         */
/************************************************************************/

//以下可以在调试时打开
//#define MYTEST_MAX_U_V
//#define MYTEST_IMAGECALIBRATION 

//#define MYTEST_LOAD_MATLAB_DLL  // 加载matlab库

#define CAMERA_PIXEL_ROW (int)2048
#define CAMERA_PIXEL_COL (int)2560

#define MAX_CAMERAS 8
#define ARROW_LEN   20  //最大矢量箭头长度 

#define DEFAULT_SAVE_RELATEDARRAY_PATH     _T("C:\\")
#define DEFAULT_C_USE_MATLAB_TEMPVAR_PATH  "C:\\Users\\Default"   // 别用_T
#define DEFAULT_PIV_DLL_LINK_PATH          _T("C:\\Users\\Default\\hs_piv64.dll")