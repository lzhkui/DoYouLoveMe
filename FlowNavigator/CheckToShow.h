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
	BOOL b_DbClk;
	int CheckCamSign[MAX_CAMERAS];

public:
	void initial();
	//返回权重
	int  ReturnWeight(int sign);

	void setDoubleClk(BOOL b_DbClk);
	BOOL getDoubleClk();
	int getNumBySign();
	void setNum(int num);
	int  getNum();
	void setStart(int start);
	void setRect(CRect& rect);
	int  getWidth(CRect& rect);
	int  getHeight(CRect& rect, float Xreal = RAW_HEIGHT, float Yreal = RAW_WIDTH);

	//inActive = true：初始8列1行模式 
	int  getStartHeight(CRect &rect, BOOL inActive = FALSE);

	//获取选中button数量以及开始位置
	st_CheckResult getCheckNum(CButton* bt[], int bt_num, BOOL setToThis = FALSE);

	//设置选中的相机编号保存至CheckCamSign中
	void setCheckCamSign(CButton* bt[], int bt_num);
	void setCheckCamSign(int* sign);
	//获取选中的相机编号
	int* getCheckCamSign();

	void ChangeButtonState(CButton* bt[], int bt_num, int State);
};
