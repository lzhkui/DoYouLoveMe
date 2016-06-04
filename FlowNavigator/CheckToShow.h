#pragma once
#include "KGlobalDefine.h"
#define LESS_SIGN -1
#define OVER_SING -2
#define RAW_WIDTH 2056
#define RAW_HEIGHT 2048

//保存checkBox选中的数量及开始位置
typedef struct CheckResult 
{
	int CheckNum;
	int start;
}st_CheckResult;

class CheckToShow
{
public:
	CheckToShow(void);
	CheckToShow(int num, int start);
	~CheckToShow(void);

private:
	int mNum;
	int mStart;
	CRect mRect;
	BOOL  b_DbClk;
	int   CheckCamSign[MAX_CAMERAS];  //保存要展示的相机编号
	int   LBDownCheckCamSign;         //鼠标左击起始点所对应的原始相机编号
	int   Number;                     //鼠标左击起始点所对应的当前顺序编号

	int   Xreal;                      //x方向实际包含的图像像素点
	int   Yreal;                      //y方向实际包含的图像像素点
	static int xLostPixel;            //由于放大 x方向舍弃未展示的像素点
	static int yLostPixel;            //由于放大 y方向舍弃未展示的像素点

public:
	int   showStartAndSize[4];        //数组中各元素含义依次为：原图起点x坐标；起点y坐标；宽；高   以上都是以原图左下角为原点

public:
	void initial();
	//返回权重
	int  ReturnWeight(int sign);

	void setDoubleClk(BOOL b_DbClk);
	BOOL getDoubleClk();
	int  getNumBySign();
	void setNum(int num);
	int  getNum();
	void setStart(int start);
	void setRect(CRect& rect);

	int getWidth();
	int  getWidth(CRect& rect);
	int  getHeight(float Xreal = 2048, float Yreal = 2560);
	int  getHeight(CRect& rect, float Xreal = 2048, float Yreal = 2560);

	//inActive = true：初始8列1行模式
	int  getStartHeight(BOOL inActive = FALSE);
	int  getStartHeight(CRect &rect, BOOL inActive = FALSE);

	//获取选中button数量以及开始位置
	st_CheckResult getCheckNum(CButton* bt[], int bt_num, BOOL setToThis = FALSE);

	//设置选中的相机编号保存至CheckCamSign中
	void setCheckCamSign(CButton* bt[], int bt_num);
	void setCheckCamSign(int* sign);
	//获取选中的相机编号
	int* getCheckCamSign();

	void ChangeButtonState(CButton* bt[], int bt_num, int State);

	//获取鼠标位置的原始和当前cam 编号
	void ReturnSignByPosition(CPoint Point, int* originalSign, int* nowSign);

	void setLBDownCheckCamSign(int sign);
	int  getLBDownCheckCamSign();

	//缩放后X方向， Y方向包含的实际原图像素
	void setXRealPixel(int Xreal);
	void setYRealPixel(int Xreal);

	int  getXRealPixel();
	int  getYRealPixel();

	void setXLostPixel(int xLostPixel);
	void setYLostPixel(int yLostPixel);

	//缩放控制
	void GenerateShowStartAndSize(CPoint PointStart, CPoint PointEnd);

};
