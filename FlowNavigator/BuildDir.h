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
extern ULONGLONG ReadTxt(CString pathDir, CString buf);
extern void FindMark(CString buf, CString mark, CString& result, int markLength);
extern void ParseConfig(CString pathDir, enum SetUpType type, CString& Result, CString buf);

extern unsigned SelectDir( CString &strDir, void *hwnd/* = NULL*/);
extern CString getEXEPath(CString EXEPath, unsigned int length = MAX_PATH);
extern BOOL CreatePathDir(CString pathDir);

}