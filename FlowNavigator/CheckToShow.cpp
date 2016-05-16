#include "StdAfx.h"
#include "CheckToShow.h"

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
}

CheckToShow::~CheckToShow(void)
{
}

//·µ»ØÈ¨ÖØ
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

int CheckToShow::getHeight(CRect& rect, float Xreal, float Yreal)
{
	//float Weight = RAW_HEIGHT/ RAW_WIDTH;
	return getWidth(rect) * Xreal/ Yreal;
}

int CheckToShow::getStartHeight(CRect &rect, BOOL inActive)
{
	if (inActive == TRUE)
	{
		int showHeigh = rect.Width()/ 8 * 2048 / 2560;
		int height = rect.Height() / 2 - showHeigh / 2;
		return height;
	}
	return (rect.Height() / 2 -  getHeight(rect) / 2);
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