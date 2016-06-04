#include "StdAfx.h"
#include "CheckToShow.h"


int CheckToShow::xLostPixel = 0;
int CheckToShow::yLostPixel = 0;

CheckToShow::CheckToShow(void)
{
	mNum    = 0;
	mStart  = 0;
	initial();
}
CheckToShow::CheckToShow(int num, int start)
{
	mNum    = num;
	mStart  = start;
	initial();
}

void CheckToShow::initial()
{
	b_DbClk = FALSE;
	for(int i = 0; i < MAX_CAMERAS; i++)
	{
		CheckCamSign[i] = -1;
	}
	LBDownCheckCamSign = -1;

	int tempShow[4] = {0, 2048, 2560, -2048}; //实际上使得放大操作失效
	for (int i = 0; i < 4; i++)
	{
		showStartAndSize[i] = tempShow[i];
	}

	Xreal = 2560;
	Yreal = 2048;
}

CheckToShow::~CheckToShow(void)
{
}

//返回权重
int CheckToShow::ReturnWeight(int sign)
{
	if (sign < mStart)
	{
		return LESS_SIGN;
	}

	if (sign > (mStart + this->mNum))
	{
		return OVER_SING;
	}

	return (sign - mStart);
}

void CheckToShow::setDoubleClk(BOOL b_DbClk)
{
	this->b_DbClk = b_DbClk;
}

BOOL CheckToShow::getDoubleClk()
{
	return this->b_DbClk;
}

void CheckToShow::setNum(int num)
{
	this->mNum = num;
}
int CheckToShow::getNum()
{
	return this->mNum;
}

int CheckToShow::getNumBySign()
{
	if (getDoubleClk())
	{
		return this->mNum;
	}

	return MAX_CAMERAS;
	
}

void CheckToShow::setStart(int start)
{
	this->mStart = start;
}

void CheckToShow::setRect(CRect& rect)
{
	this->mRect = rect;
}

int CheckToShow::getWidth()
{
	return getWidth(this->mRect);
}

int CheckToShow::getWidth(CRect& rect)
{
	if(this->mNum == 0)
	{
		return 0;
	}
	if(getDoubleClk())
	{
		return (rect.Width() / (this->mNum));
	}
	
	return rect.Width() / MAX_CAMERAS;
}

int CheckToShow::getHeight(float Xreal, float Yreal)
{
	return getHeight(this->mRect, Xreal, Yreal);
}

int CheckToShow::getHeight(CRect& rect, float Xreal, float Yreal)
{
	//float Weight = RAW_HEIGHT/ RAW_WIDTH;
	int x = Xreal;
	int y = Yreal;
	int height = getWidth(rect) * x / y;
	return height;
}

int CheckToShow::getStartHeight(BOOL inActive /* = FALSE */)
{
	return getStartHeight(this->mRect, inActive);
}

int CheckToShow::getStartHeight(CRect &rect, BOOL inActive)
{
	if (inActive == TRUE)
	{
		int showHeigh = rect.Width()/ 8 * 2048 / 2560;
		int height = rect.Height() / 2 - showHeigh / 2;
		return height;
	}
	int startHeight = rect.Height() / 2 -  getHeight(rect) / 2;
	return startHeight;
}

st_CheckResult CheckToShow::getCheckNum(CButton* bt[], int bt_num, BOOL setVarToThis)
{
	st_CheckResult checkResult;

	checkResult.CheckNum = 0;
	checkResult.start    = -1;
	int sign             = -1;

	for (int i = 0; i < bt_num; i++)
	{
		if(bt[i]->GetCheck())
		{
			if(sign == -1)
			{
				checkResult.start = i;
				sign = 0;
			}
			checkResult.CheckNum++;
		}
	}

	
	if(setVarToThis)
	{
		this->mNum   = checkResult.CheckNum;
		this->mStart = checkResult.start;
	}

	return checkResult;
}

void CheckToShow::setCheckCamSign(CButton* bt[], int bt_num)
{
	for (int i = 0; i < MAX_CAMERAS; i++)
	{
		this->CheckCamSign[i] = -1;
	}
	int j = 0;
	for (int i = 0; i < bt_num; i++)
	{
		if(bt[i]->GetCheck())
		{
			this->CheckCamSign[j] = i;
			j++;
		}
	}
}

void CheckToShow::setCheckCamSign(int* sign)
{
	for (int i = 0; i < MAX_CAMERAS; i++)
	{
		CheckCamSign[i] = *(sign + i);
	}
}

int* CheckToShow::getCheckCamSign()
{
	return this->CheckCamSign;
}

void CheckToShow::ChangeButtonState(CButton* bt[], int bt_num, int State)
{
	for (int i = 0; i< bt_num; i++)
	{
		bt[i]->EnableWindow(State);
	}
}

void CheckToShow::ReturnSignByPosition(CPoint Point, int* originalSign, int* nowSign)
{
	int startCam = -1;
	int i = 0;
	for (i =0; i < MAX_CAMERAS; i++)
	{
		if(Point.x < mRect.Width()/mNum * (1+i))
		{
			startCam = CheckCamSign[i];
			break;
		}
	}

	this->Number = i;
	this->LBDownCheckCamSign = startCam;
	*originalSign            = startCam;
	*nowSign                 = i;
}
void CheckToShow::setXRealPixel(int Xreal)
{
	this->Xreal = Xreal;
}

void CheckToShow::setYRealPixel(int Yreal)
{
	this->Yreal = Yreal;
}
void CheckToShow::setXLostPixel(int xLostPixel)
{
	this->xLostPixel = xLostPixel;
}

void CheckToShow::setYLostPixel(int yLostPixel)
{
	this->yLostPixel = yLostPixel;
}

int CheckToShow::getXRealPixel()
{
	return this->Xreal;
}

int CheckToShow::getYRealPixel()
{
	return this->Yreal;
}
void CheckToShow::GenerateShowStartAndSize(CPoint PointStart, CPoint PointEnd)
{
	if (mRect == NULL)
	{
		return;
	}
	int singleWidth  = getWidth(mRect);
	int singleHeight = getHeight(mRect);
	int xTempLength  = (PointStart.x - this->Number * singleWidth)   * getXRealPixel() / singleWidth;
	int yTempLength  = (PointStart.y - getStartHeight(mRect, FALSE)) * getYRealPixel() / singleHeight;

	this->showStartAndSize[0] = xLostPixel +  xTempLength ;
	this->showStartAndSize[1] = 2048 - yLostPixel-yTempLength; 
	this->showStartAndSize[2] = (PointEnd.x - PointStart.x) * getXRealPixel() / singleWidth;
	this->showStartAndSize[3] = (PointEnd.y - PointStart.y) * getYRealPixel() / singleHeight;

	this->Xreal = showStartAndSize[2];
	this->Yreal = showStartAndSize[3];

	this->xLostPixel = xLostPixel + xTempLength;
	this->yLostPixel = yLostPixel + yTempLength;
}

void CheckToShow::setLBDownCheckCamSign(int sign)
{
	this->LBDownCheckCamSign = sign;
}

int CheckToShow::getLBDownCheckCamSign()
{
	return this->LBDownCheckCamSign;
}