#pragma once

#define LESS_SIGN -1
#define OVER_SING -2
#define RAW_WIDTH 2056
#define RAW_HEIGHT 2048

//����checkBoxѡ�е���������ʼλ��
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

public:
	//����Ȩ��
	int  ReturnWeight(int sign);
	void setNum(int num);
	int  getNum();
	void setStart(int start);
	void setRect(CRect& rect);
	int  getWidth(CRect& rect);
	int  getHeight(CRect& rect, float Xreal = RAW_HEIGHT, float Yreal = RAW_WIDTH);
	//inActive = true����ʼ8��1��ģʽ 
	int  getStartHeight(CRect &rect, BOOL inActive = FALSE);
	//��ȡѡ��button�����Լ���ʼλ��
	st_CheckResult getCheckNum(CButton* bt[], int bt_num, BOOL setToThis = FALSE);
	void ChangeButtonState(CButton* bt[], int bt_num, int State);

};
