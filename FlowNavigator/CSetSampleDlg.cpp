// SetSampleDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "FlowNavigator.h"
#include "CSetSampleDlg.h"


// SetSampleDlg �Ի���

IMPLEMENT_DYNAMIC(CSetSampleDlg, CDialog)

CSetSampleDlg::CSetSampleDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSetSampleDlg::IDD, pParent)
	, m_SampleTime(0)
	, m_CameraCount(0)
	, Count(0)

{
	for (int i=0; i< MAX_CAMERAS; i++)
	{
		m_hCam[i] = NULL;
	}
}

CSetSampleDlg::~CSetSampleDlg()
{
}

void CSetSampleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_SampleTIme, m_SampleTime);
	DDV_MinMaxInt(pDX, m_SampleTime, 1, 300);
}


BEGIN_MESSAGE_MAP(CSetSampleDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CSetSampleDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_CANCEL, &CSetSampleDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// SetSampleDlg ��Ϣ�������

void CSetSampleDlg::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData(TRUE);
	NODE_HANDLE hNode;
	double d_val;

	if(J_ST_SUCCESS == J_Camera_GetNodeByName(m_hCam[0],"AcquisitionFrameRate",&hNode))
	{
		if(J_ST_SUCCESS != J_Node_GetValueDouble(hNode,TRUE,&d_val))	
		{
			AfxMessageBox(_T("δ����ȷ��ȡ���֡�ʲ���!"),MB_ICONINFORMATION);
		}
		else
		{
			m_CurFrame = d_val;
		}
	}
	else
	{
		AfxMessageBox(_T("δ����ȷ��ȡ���֡�ʲ���!"),MB_ICONINFORMATION);
	}

	Count = ((int)m_CurFrame)*m_SampleTime;

	OnOK();
}

void CSetSampleDlg::OnBnClickedCancel()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CDialog::OnCancel();
}
