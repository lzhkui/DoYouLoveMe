#pragma once

#define LESS_SIGN -1
#define OVER_SING -2
#define RAW_WIDTH 2056
#define RAW_HEIGHT 2048

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

public:
	//·µ»ØÈ¨ÖØ
	int ReturnWeight(int sign);
	void setNum(int num);
	void setStart(int start);
	void setRect(CRect& rect);
	int getWidth(CRect& rect);
	int getHeight(CRect& rect);
	int getStartHeight(CRect &rect);
};
