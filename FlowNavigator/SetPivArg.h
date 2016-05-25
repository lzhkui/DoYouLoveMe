#pragma once

#include "ShowView.h"
// SetPivArg �Ի���


extern int mLinkFlow; //-1:δ��ʼ����ƴ��

class SetPivArg : public CDialog
{
	DECLARE_DYNAMIC(SetPivArg)

public:
	static SetPivArg* GetInstance();
	virtual ~SetPivArg();

	void setShowView(ShowView* showView);
	ShowView* getShowView();

private:
	SetPivArg(CWnd* pParent = NULL);
	static SetPivArg *setPivArg;
	ShowView *showView; 
	void setShowViewArgs(BOOL default);

private:
	static int num;               //ʵ����������  
	int paras[8];
	int XYRange[4];
public:
	int* copy_solveFlow;
// �Ի�������
	enum { IDD = IDD_DIALOG_PIVSET };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
	virtual void PostNcDestroy();
	virtual void OnCancel();
public:
	int xSmall;
	int xLarge;
	int ySmall;
	int yLarge;
	int step;
	int m_Row;

	//Ӧ��Ĭ��ֵ
	afx_msg void OnBnClickedDefaultArgPiv();
	//Ӧ�õ�ǰֵ
	afx_msg void OnBnClickedUseCurrent();
	//��ʼ����
	afx_msg void OnBnClickedStartCalc();
	//ֹͣ����
	afx_msg void OnBnClickedStopCalc();
};
