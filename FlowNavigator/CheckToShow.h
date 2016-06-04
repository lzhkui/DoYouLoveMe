#pragma once
#include "KGlobalDefine.h"
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
	BOOL  b_DbClk;
	int   CheckCamSign[MAX_CAMERAS];  //����Ҫչʾ��������
	int   LBDownCheckCamSign;         //��������ʼ������Ӧ��ԭʼ������
	int   Number;                     //��������ʼ������Ӧ�ĵ�ǰ˳����

	int   Xreal;                      //x����ʵ�ʰ�����ͼ�����ص�
	int   Yreal;                      //y����ʵ�ʰ�����ͼ�����ص�
	static int xLostPixel;            //���ڷŴ� x��������δչʾ�����ص�
	static int yLostPixel;            //���ڷŴ� y��������δչʾ�����ص�

public:
	int   showStartAndSize[4];        //�����и�Ԫ�غ�������Ϊ��ԭͼ���x���ꣻ���y���ꣻ����   ���϶�����ԭͼ���½�Ϊԭ��

public:
	void initial();
	//����Ȩ��
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

	//inActive = true����ʼ8��1��ģʽ
	int  getStartHeight(BOOL inActive = FALSE);
	int  getStartHeight(CRect &rect, BOOL inActive = FALSE);

	//��ȡѡ��button�����Լ���ʼλ��
	st_CheckResult getCheckNum(CButton* bt[], int bt_num, BOOL setToThis = FALSE);

	//����ѡ�е������ű�����CheckCamSign��
	void setCheckCamSign(CButton* bt[], int bt_num);
	void setCheckCamSign(int* sign);
	//��ȡѡ�е�������
	int* getCheckCamSign();

	void ChangeButtonState(CButton* bt[], int bt_num, int State);

	//��ȡ���λ�õ�ԭʼ�͵�ǰcam ���
	void ReturnSignByPosition(CPoint Point, int* originalSign, int* nowSign);

	void setLBDownCheckCamSign(int sign);
	int  getLBDownCheckCamSign();

	//���ź�X���� Y���������ʵ��ԭͼ����
	void setXRealPixel(int Xreal);
	void setYRealPixel(int Xreal);

	int  getXRealPixel();
	int  getYRealPixel();

	void setXLostPixel(int xLostPixel);
	void setYLostPixel(int yLostPixel);

	//���ſ���
	void GenerateShowStartAndSize(CPoint PointStart, CPoint PointEnd);

};
