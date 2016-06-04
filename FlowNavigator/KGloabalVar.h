#pragma once
extern int Pair[];        // 采集一对图片进行piv计算
extern BOOL CloseSystem;  //关闭系统

//相机的实际物理范围
typedef struct SingleRange 
{
	float xMin;
	float xMax;
	float yMin;
	float yMax;
}st_Range;

// typedef struct ptr_SingleRange 
// {
// 	float* xMin;
// 	float* xMax;
// 	float* yMin;
// 	float* yMax;
// }st_Range;

//客户区物理范围
typedef struct ClientRange
{
	float xMin;
	float xMax;
	float yMin;
	float yMax;
}st_ClientRange;

//起始点设备坐标单位像素
typedef struct StartPosition
{
	int xStart;
	int yStart;
}st_StartPosition;

//校正后，确定所选相机的宽高基准
typedef struct BaseWidthANDHeight
{
	int baseWidth;
	int baseHeight;
	int diffWidth;
	int diffHeight;
}st_Base;

typedef struct FlowRate
{
	float u;
	float v;
	float uv;
}st_FlowRate;

//标定点位置
typedef struct CalibrationPoint
{
	//物理坐标
	float x_p;
	float y_p;
	//图像坐标
	int   x_i;
	int   y_i;
}CalibrationPoint;

