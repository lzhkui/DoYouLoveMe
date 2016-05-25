#pragma once

#include "ShowView.h"
// SetPivArg 对话框


extern int mLinkFlow; //-1:未开始流场拼接

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
	static int num;               //实例个数计数  
	int paras[8];
	int XYRange[4];
public:
	int* copy_solveFlow;
// 对话框数据
	enum { IDD = IDD_DIALOG_PIVSET };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

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

	//应用默认值
	afx_msg void OnBnClickedDefaultArgPiv();
	//应用当前值
	afx_msg void OnBnClickedUseCurrent();
	//开始计算
	afx_msg void OnBnClickedStartCalc();
	//停止计算
	afx_msg void OnBnClickedStopCalc();
};
