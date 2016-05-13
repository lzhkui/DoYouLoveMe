#include "StdAfx.h"
#include "CheckToShow.h"

CheckToShow::CheckToShow(void)
{
	mNum   = 0;
	mStart = 0;
}
CheckToShow::CheckToShow(int num, int start)
{
	mNum   = num;
	mStart = start;
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

void CheckToShow::setNum(int num)
{
	this->mNum = num;
}
int CheckToShow::getNum()
{
	return this->mNum;
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
	return (rect.Width() / (this->mNum));
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
		float height = rect.Height() / 2 - rect.Width()/ 8 * RAW_HEIGHT / RAW_WIDTH;
		return height;
	}
	return (rect.Height() / 2 -  getHeight(rect) / 2);
}

st_CheckResult CheckToShow::getCheckNum(CButton* bt[], int bt_num, BOOL setVarToThis)
{
	st_CheckResult checkResult;
	checkResult.CheckNum = 0;
	checkResult.start = -1;
	int sign  = -1;
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

void CheckToShow::ChangeButtonState(CButton* bt[], int bt_num, int State)
{
	for (int i = 0; i< bt_num; i++)
	{
		bt[i]->EnableWindow(State);
	}
}