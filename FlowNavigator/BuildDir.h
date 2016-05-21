#pragma once

#define MAX_SUBDIR 10                            //子目录最大数
#define DEFAULT_PATH _T("C:\\Users\\Default")    //默认根目录

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

//这个是我用了write直接写入CString类型后，工程字符集是UNicode，存下来看到字符编码是UCS-2 little endian OVR（notepad显示的）
extern ULONGLONG ReadTxt(CString pathDir, CString& buf);
//通常情况下用这个
extern 	ULONGLONG ReadTxtUsual(CString pathDir, CString& buf); 
//查找符号（mark）间的字符串，并将其返回到result中
extern void FindMark(CString buf, CString mark, CString& result, int markLength);
//查找自第loopCount个符号（mark）后的字符串并将其返回到result中
extern void LoopFindMark(CString buf, CString mark, CString& result, int markLength, int loopCount);

extern void ParseConfig(CString pathDir, enum SetUpType type, CString& Result, CString& buf);

extern unsigned SelectDir( CString &strDir, void *hwnd/* = NULL*/, BOOL hasCreateFile);
extern CString getEXEPath(CString EXEPath, unsigned int length = MAX_PATH);
extern BOOL CreatePathDir(CString pathDir);

}