#pragma once

#define MAX_SUBDIR 10                            //��Ŀ¼�����
#define DEFAULT_PATH _T("C:\\Users\\Default")    //Ĭ�ϸ�Ŀ¼

enum SetUpType
{
	ProPath = 0,
	Exposure,
	Gain,
	Frame
};

namespace BuildDir{

extern void WriteTxt(CString pathDir, CString initContent);
extern void WriteTxtByLine(CString pathDir, CString initContent);
extern void FileDialog_k(CString title, CString defaultFileName, CString pathDir, CString initContent);

//�����������writeֱ��д��CString���ͺ󣬹����ַ�����UNicode�������������ַ�������UCS-2 little endian OVR��notepad��ʾ�ģ�
extern ULONGLONG ReadTxt(CString pathDir, CString& buf);
//ͨ������������
extern 	ULONGLONG ReadTxtUsual(CString pathDir, CString& buf); 
//���ҷ��ţ�mark������ַ����������䷵�ص�result��
extern void FindMark(CString buf, CString mark, CString& result, int markLength);
//�����Ե�loopCount�����ţ�mark������ַ��������䷵�ص�result��
extern void LoopFindMark(CString buf, CString mark, CString& result, int markLength, int loopCount);

extern void ParseConfig(CString pathDir, enum SetUpType type, CString& Result, CString& buf);

extern unsigned SelectDir( CString &strDir, void *hwnd/* = NULL*/, BOOL hasCreateFile);
extern CString getEXEPath(CString EXEPath, unsigned int length = MAX_PATH);
extern BOOL CreatePathDir(CString pathDir);

}