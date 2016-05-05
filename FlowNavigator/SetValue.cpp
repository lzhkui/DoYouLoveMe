// SetValue.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "FlowNavigator.h"
#include "SetValue.h"

// SetValue �Ի���

IMPLEMENT_DYNAMIC(CSetValue, CDialog)

CSetValue::CSetValue(CWnd* pParent /*=NULL*/, CString curProPath)
	: CDialog(CSetValue::IDD, pParent)
	, m_MinExposure(0)
	, m_MaxExposure(0)
	, m_CurExposure(0)
	, m_CameraCount(0)
	, m_CurGain(0)
	, m_MinGain(0)
	, m_MaxGain(0)
	, m_CurFrame(0.0)
	, m_MinFrame(0)
	, m_MaxFrame(0)
{
	for (int i=0; i< MAX_CAMERAS; i++)
	{
		m_hCam[i] = NULL;
	}
	m_hExposureNode = NULL;
	m_CurrentProPath = curProPath;
}

CSetValue::~CSetValue()
{
	OnBnClickedSaveSetup();
}

void CSetValue::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SLIDER1, mExpSliderCtrl);
	DDX_Text(pDX, IDC_MinExposure, m_MinExposure);
	DDX_Text(pDX, IDC_MaxExposure, m_MaxExposure);
	DDX_Text(pDX, IDC_CurExposure, m_CurExposure);
	//DDV_MinMaxInt(pDX, m_CurExposure, 10, 44730);
	DDX_Text(pDX, IDC_CurGain, m_CurGain);
	DDX_Text(pDX, IDC_MinGain, m_MinGain);
	DDX_Text(pDX, IDC_MaxGain, m_MaxGain);
	DDX_Text(pDX, IDC_CurFrame, m_CurFrame);
	DDX_Text(pDX, IDC_MinFrame, m_MinFrame);
	DDX_Text(pDX, IDC_MaxFrame, m_MaxFrame);
	DDX_Control(pDX, IDC_SLIDER2, mGainSliderCtrl);
	DDX_Control(pDX, IDC_SLIDER3, mFrameSliderCtrl);
}


BEGIN_MESSAGE_MAP(CSetValue, CDialog)
    ON_WM_TIMER()

	ON_STN_CLICKED(IDC_MinExposure, &CSetValue::OnStnClickedMinexposure)
	ON_EN_CHANGE(IDC_CurExposure, &CSetValue::OnEnChangeCurexposure)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_SAVESETUP, &CSetValue::OnBnClickedSaveSetup)
	ON_BN_CLICKED(IDC_LOADLASTSETUP, &CSetValue::OnBnClickedLoadlastsetup)
END_MESSAGE_MAP()


// SetValue ��Ϣ�������

void CSetValue::Start()
{
	SetTimer(1,1000,NULL);
}

void CSetValue::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == 1)
	{
		int iPos = mExpSliderCtrl.GetPos();
		
		if (m_hCam[0] != NULL)
		{
			TRACE("�Ӵ��� iPos = %d\n",iPos);

		}
	}
}

void CSetValue::SetRangeMM(void)
{
	CSliderCtrl *pSCtrl;
	int iPos;
	J_STATUS_TYPE retval;
	NODE_HANDLE hNode;
	int64_t int64Val;
	double d_val;

	if (m_hCam[0] != NULL)
	{
		if(-1 == SetValueMM(m_hCam,NODE_NAME_EXPOSURE,mExpSliderCtrl,&m_MinExposure,&m_MaxExposure))
		{
			AfxMessageBox(_T("δ����ȷ��ȡ����ع�ʱ�����!"),MB_ICONINFORMATION);
		}
		if(-1 == SetValueMM(m_hCam,NODE_NAME_GAIN,mGainSliderCtrl,&m_MinGain,&m_MaxGain))
		{
			AfxMessageBox(_T("δ����ȷ��ȡ����������!"),MB_ICONINFORMATION);
		}
// 		if(-1 == SetValueMM(m_hCam,"AcquisitionFrameRate",mFrameSliderCtrl,&m_MinFrame,&m_MaxFrame))
// 		{
// 			AfxMessageBox(_T("δ����ȷ��ȡ���֡�ʲ���!"),MB_ICONINFORMATION);
// 		}
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
		UpdateData(FALSE);
	
	}
	else
	{
		setInitValueMM(mExpSliderCtrl,m_MinExposure,m_MaxExposure,10,44730);
		setInitValueMM(mGainSliderCtrl,m_MinGain,m_MaxGain,100,1600);
		UpdateData(FALSE);
	}

	m_MinFrame = MIN_FRAME;
	m_MaxFrame = MAX_FRAME;
	mFrameSliderCtrl.SetRangeMin(m_MinFrame,TRUE);
	mFrameSliderCtrl.SetRangeMax(m_MaxFrame,TRUE);
	mFrameSliderCtrl.SetSelection(m_MinFrame,m_MaxFrame);
	mFrameSliderCtrl.SetTicFreq((m_MaxFrame - m_MinFrame)/20);
	UpdateData(FALSE);

	//��������������껬���� �����޺����Ч������...
	if (m_CurExposure != 0)
	{
		mExpSliderCtrl.SetPos(m_CurExposure);
		//UpdateData(FALSE);
	}
	if (m_CurGain != 0)
	{
		mGainSliderCtrl.SetPos(m_CurGain);
	}
	if(m_CurFrame != 0)
	{
		mFrameSliderCtrl.SetPos(m_CurFrame);
	}
}


void CSetValue::setInitValueMM(CSliderCtrl& mSlider, int& mMin, int& mMax,int Min, int Max)
{
	mMin = Min;
	mMax = Max;

	mSlider.SetRangeMin(Min,TRUE);
	mSlider.SetRangeMax(Max,TRUE);

	mSlider.SetSelection(Min,44730);
	mSlider.SetTicFreq((Max-Min)/20);

}

int CSetValue::SetValueMM(CAM_HANDLE hCam[], int8_t* sNodeName,
						  CSliderCtrl& mSlider, int* Min, int* Max)
{
	int retval;
	int64_t int64Val;
	NODE_HANDLE m_hNode;

	retval = J_Camera_GetNodeByName(m_hCam[0],sNodeName,&m_hNode);
	if (retval == J_ST_SUCCESS)
	{
		retval = J_Node_GetMinInt64(m_hNode,&int64Val);
		if (retval != J_ST_SUCCESS)
		{
			return -1;
		}
		mSlider.SetRangeMin((int)int64Val,TRUE);
		*Min = (int)int64Val;

		retval = J_Node_GetMaxInt64(m_hNode,&int64Val);
		if (retval != J_ST_SUCCESS)
		{
			return -1;
		}
		mSlider.SetRangeMax((int)int64Val,TRUE);
		*Max = (int)int64Val;

		//retval = J_Node_GetValueInt64(m_hCam[0],TRUE,&int64Val);
		//SetDlgItemInt(IDC_CurExposure,(int)int64Val);	

		mSlider.SetSelection(*Min,*Max);
		mSlider.SetTicFreq((*Max-*Min)/20);
	}
	else
	{
		return -1;
	}

	return 0;
}

void CSetValue::OnStnClickedMinexposure()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}

void CSetValue::OnEnChangeCurexposure()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ�������������
	// ���͸�֪ͨ��������д CDialog::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	/*
	int retval;

	//Sleep(1000);
	UpdateData(TRUE);
	mExpSliderCtrl.SetPos(m_CurExposure);
	if(m_hCam[0] != NULL)
	{
		retval = J_Camera_GetNodeByName(m_hCam[0],NODE_NAME_EXPOSURE,&m_hExposureNode);
		for(int i = 0; i < m_CameraCount; i++)
		{
			retval = J_Camera_SetValueInt64(m_hCam[i],NODE_NAME_EXPOSURE,(int64_t)m_CurExposure);
			if(retval != J_ST_SUCCESS)
			{
				::AfxMessageBox((LPCTSTR)"����δ���óɹ�!\n",MB_ICONINFORMATION);
			}
		}
	}
	*/
}

void CSetValue::OnOK()
{
	// TODO: �ڴ����ר�ô����/����û���
	if(GetDlgItem(IDC_CurExposure) == GetFocus())
	{
		TRACE("����Enter Exposure!\n");
		UpdateData(TRUE);
		int min;
		int max;
		if (m_hCam[0] == NULL)
		{
			min = Min_EXPOSURE;
			max = Max_EXPOSURE;
		}
		else
		{
			min = m_MinExposure;
			max = m_MaxExposure;
		}
		if (-1 == SetFromEditToSlider(m_CurExposure,mExpSliderCtrl,min,max))
		{
			CString str = _T("������");//"������10��44730�������!"
			CString minStr;
			CString MaxStr;
			minStr.Format(_T("%d"),min);
			MaxStr.Format(_T("%d"),max);
			str += minStr +_T("��")+ MaxStr + _T("�������!"); 
			AfxMessageBox(str,MB_ICONINFORMATION);
			return;
		}

		if(m_hCam[0] != NULL)
		{
			if(J_ST_SUCCESS != SetValueInt_CWJ(m_hCam,NODE_NAME_EXPOSURE,(int64_t)m_CurExposure,m_CameraCount))
			{
				return;
			}
		}
		
		return;
	}
	else if (GetDlgItem(IDC_CurGain) == GetFocus())
	{
		TRACE("����Enter Gain!\n");
		UpdateData(TRUE);
		if(-1 == SetFromEditToSlider(m_CurGain,mGainSliderCtrl,Min_GAIN,MAX_GAIN))
		{
             AfxMessageBox(_T("������100��1600�������!"),MB_ICONINFORMATION);
			 return;
		}
	
		if(m_hCam[0] != NULL)
		{
			if(J_ST_SUCCESS != SetValueInt_CWJ(m_hCam,NODE_NAME_GAIN,(int64_t)m_CurGain,m_CameraCount))
			{
				return;
			}
		}

		return;
	}
	else if (GetDlgItem(IDC_CurFrame) == GetFocus())
	{
		TRACE("����Enter Frame!\n");
		UpdateData(TRUE);
		TRACE("m_CurFrame = %lf\n",m_CurFrame);
		int CurFr = (int)m_CurFrame;
		if(-1 == SetFromEditToSlider(CurFr,mFrameSliderCtrl,MIN_FRAME,MAX_FRAME))
		{
			AfxMessageBox(_T("������1��20�������!"),MB_ICONINFORMATION);
			return;
		}
		//�ɻ�ĵط�
        m_CurFrame = CurFr;

		NODE_HANDLE hNode;
		if(m_hCam[0] != NULL)
		{
			for (int i = 0; i < m_CameraCount; i++)
			{
				if(J_ST_SUCCESS == J_Camera_GetNodeByName(m_hCam[i],"AcquisitionFrameRate",&hNode))
				{
					J_Node_SetValueDouble(hNode,TRUE,m_CurFrame);
				}
				else
				{
					AfxMessageBox(_T("֡��δ���óɹ����������豸!"),MB_ICONINFORMATION);
				}
			}

			//֡�ʸı䣬�ع�ʱ������Ҳ��֮�ı�
			SetValueMM(m_hCam,NODE_NAME_EXPOSURE,mExpSliderCtrl,&m_MinExposure,&m_MaxExposure);
			//֡�ʱ������ԭ�����õ��ع�ʱ��������ڵ��ع�ʱ�����ޣ����½��
			if (m_CurExposure > m_MaxExposure)
			{
				m_CurExposure = m_MaxExposure;
			}
        }
		UpdateData(FALSE);
		return;
	}
	CDialog::OnOK();
}


int CSetValue::SetFromEditToSlider(int m_CurrentValue, CSliderCtrl& mSlider, int Min, int Max)
{
	TRACE("m_CurrentValue= %d\n",m_CurrentValue);
	//�����������������ݼ���Ƿ������֣�Ȼ���ֿ�ʼ���ÿؼ�����ʱ����ѡ�����int��
	//����ֻҪ��ͷ�����֣����ǽ�ȡ����������Ρ���������Ȳ�������
	CString str = NULL;
	str.Format(_T("%d"),m_CurrentValue);
	if ((str != str.SpanIncluding(_T("0123456789"))) ||
		(m_CurrentValue < Min || m_CurrentValue > Max))
	{
		//AfxMessageBox(_T("������10��44730�������!"),MB_ICONINFORMATION);
		return -1;
	}
	mSlider.SetPos(m_CurrentValue);

	return 0;
}
int CSetValue::SetValueInt_CWJ(CAM_HANDLE hCam[], int8_t* sNodeName, int64_t Value, int m_CameraCount)
{
	int retval;
	NODE_HANDLE m_hNode;
	for (int i = 0; i < m_CameraCount; i++)
	{
		//retval = J_Camera_GetNodeByName(m_hCam[i],sNodeName,&m_hNode);
		retval = J_Camera_SetValueInt64(m_hCam[i],sNodeName,Value);

		if (retval == J_ST_SUCCESS)
		{
		}
		else
		{
			AfxMessageBox(_T("����δ���óɹ�!"),MB_ICONINFORMATION);
			return -1;
		}
	}

	return J_ST_SUCCESS;
}

void CSetValue::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
	TRACE("����������\n");
	if(mExpSliderCtrl == *(CSliderCtrl *)pScrollBar)
	{
		UpdateData(TRUE);
		m_CurExposure = mExpSliderCtrl.GetPos();
		UpdateData(FALSE);
		if(m_hCam[0] != NULL)
		{
			if(J_ST_SUCCESS != SetValueInt_CWJ(m_hCam,NODE_NAME_EXPOSURE,(int64_t)m_CurExposure,m_CameraCount))
			{
				return;
			}
		}	
	}
	else if(mGainSliderCtrl == *(CSliderCtrl *)pScrollBar)
	{
		UpdateData(TRUE);
		m_CurGain = mGainSliderCtrl.GetPos();
		UpdateData(FALSE);
		if(m_hCam[0] != NULL)
		{
			if(J_ST_SUCCESS != SetValueInt_CWJ(m_hCam,NODE_NAME_GAIN,(int64_t)m_CurGain,m_CameraCount))
			{
				return;
			}
		}	

	}
	else if (mFrameSliderCtrl == *(CSliderCtrl *)pScrollBar)
	{
		UpdateData(TRUE);
		m_CurFrame = mFrameSliderCtrl.GetPos();

		NODE_HANDLE hNode;
		if(m_hCam[0] != NULL)
		{
			for (int i = 0; i < m_CameraCount; i++)
			{
				if(J_ST_SUCCESS == J_Camera_GetNodeByName(m_hCam[i],"AcquisitionFrameRate",&hNode))
				{
					J_Node_SetValueDouble(hNode,TRUE,m_CurFrame);
				}
				else
				{
					AfxMessageBox(_T("֡������δ�ɹ����������豸!"),MB_ICONINFORMATION);
				}
			}

			//֡�ʸı䣬�ع�ʱ������Ҳ��֮�ı�
			SetValueMM(m_hCam,NODE_NAME_EXPOSURE,mExpSliderCtrl,&m_MinExposure,&m_MaxExposure);
			//֡�ʱ������ԭ�����õ��ع�ʱ��������ڵ��ع�ʱ�����ޣ����½��
			if (m_CurExposure > m_MaxExposure)
			{
				m_CurExposure = m_MaxExposure;
			}
	    }
		UpdateData(FALSE);
    }
}

void CSetValue::OnCancel()
{
	// TODO: �ڴ����ר�ô����/����û���
    DestroyWindow();
	//CDialog::OnCancel();
}


void CSetValue::OnBnClickedSaveSetup()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString content;
	CString SetUpPath = m_CurrentProPath + _T("\\�����ļ�") + _T("\\CameraSetup.txt");

	content.Format(_T("<Exposure>%d<Exposure>\r\n<Gain>%d<Gain>\r\n<Frame>%lf<Frame>\r\n"),
		m_CurExposure, m_CurGain, m_CurFrame);

	content = _T("<ProPath>") + m_CurrentProPath +  _T("<ProPath>")  + _T("\r\n") + content;

	WriteTxt(SetUpPath, content);

// 	content = _T("<ProPath>") + m_CurrentProPath + _T("\n");
// 	WriteTxtByLine(SetUpPath,  content);
// 
// 	content.Format(_T("<Exposure>%d\n"), m_CurExposure);
// 	WriteTxtByLine(SetUpPath,  content);
// 
// 	content.Format(_T("<Gain>%d\n"), m_CurGain);
// 	WriteTxtByLine(SetUpPath,  content);
// 
// 	content.Format(_T("<Frame>%lf\n"), m_CurFrame);
// 	WriteTxtByLine(SetUpPath,  content);
	
}

void CSetValue::OnBnClickedLoadlastsetup()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString lastExposure;
	CString lastGain;
	CString lastFrame;

	CString configFileTxT;
	ParseConfig(m_CurrentProPath + _T("\\�����ļ�\\CameraSetup.txt"), Exposure, lastExposure, configFileTxT);
	ParseConfig(m_CurrentProPath + _T("\\�����ļ�\\CameraSetup.txt"), Gain, lastGain, configFileTxT);
	ParseConfig(m_CurrentProPath + _T("\\�����ļ�\\CameraSetup.txt"), Frame, lastFrame, configFileTxT);

	m_CurExposure = _ttoi(lastExposure);
	m_CurGain = _ttoi(lastGain);
	m_CurFrame = _ttol(lastFrame);
	UpdateData(FALSE);

	//����������֡�ʣ��������ϴα�����ع���� ��ǰĬ��֡��(�����ʱ������õ�)���ܴﵽ�����ֵʱ��
    //���ع����ò��ɹ�_2016��5��4��13:15:35
	NODE_HANDLE hNode;
	if(m_hCam[0] != NULL)
	{
		for (int i = 0; i < m_CameraCount; i++)
		{
			if(J_ST_SUCCESS == J_Camera_GetNodeByName(m_hCam[i],"AcquisitionFrameRate",&hNode))
			{
				J_Node_SetValueDouble(hNode,TRUE,m_CurFrame);
			}
			else
			{
				AfxMessageBox(_T("֡��δ���óɹ����������豸!"),MB_ICONINFORMATION);
			}
		}
	}
	if(m_hCam[0] != NULL)
	{
		if(J_ST_SUCCESS != SetValueInt_CWJ(m_hCam,NODE_NAME_EXPOSURE,(int64_t)m_CurExposure,m_CameraCount))
		{
			return;
		}
		if(J_ST_SUCCESS != SetValueInt_CWJ(m_hCam,NODE_NAME_GAIN,(int64_t)m_CurGain,m_CameraCount))
		{
			return;
		}
	}

	mExpSliderCtrl.SetPos(m_CurExposure);
	mGainSliderCtrl.SetPos(m_CurGain);
	mFrameSliderCtrl.SetPos(m_CurFrame);
}

BOOL CSetValue::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}
