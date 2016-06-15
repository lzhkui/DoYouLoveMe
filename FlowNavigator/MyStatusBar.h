#pragma once


// MyStatusBar

class MyStatusBar : public CStatusBar
{
	DECLARE_DYNAMIC(MyStatusBar)

public:
	MyStatusBar();
	virtual ~MyStatusBar();

protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void AssertValid() const;
};


