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

	if (sign > mStart + this->mNum)
	{
		return OVER_SING;
	}

	return (sign - mStart);
}

void CheckToShow::setNum(int num)
{
	this->mNum = num;
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

int CheckToShow::getHeight(CRect& rect)
{
	float Weight = RAW_HEIGHT/ RAW_WIDTH;
	return getWidth(rect)*RAW_HEIGHT/ RAW_WIDTH;
}

int CheckToShow::getStartHeight(CRect &rect)
{
	return (rect.Height() / 2 -  getHeight(rect) / 2);
}