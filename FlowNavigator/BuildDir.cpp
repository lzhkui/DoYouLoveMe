
#include "StdAfx.h"
#include "BuildDir.h"
#include "shlwapi.h"
#pragma comment(lib,"shlwapi.lib")

#include "ErrorCodeList.h"
using namespace ErrorCodeList;

char szFilter[] = {"TXT Files (*.txt)|*.txt||"};

namespace BuildDir{
	/* */
	void WriteTxt(CString pathDir, CString initContent)
	{
		CFile file(pathDir, CFile::modeCreate| CFile::modeWrite);
		file.Write(initContent, initContent.GetLength() * sizeof(TCHAR));// sizeof(TCHAR)在这里是2..
		file.Flush();
		file.Close();
	}

	void WriteTxtByLine(CString pathDir, CString initContent)
	{
		CFileFind fileFind;
		CStdioFile stdFile;
		if(!fileFind.FindFile(pathDir))
		{
			stdFile.Open(pathDir, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary);

		}
		else
		{
			//stdFile.Open(pathDir, CFile::modeWrite | CFile::modeNoTruncate | CFile::typeBinary);//追加方式打开，但是。。。，
			stdFile.Open(pathDir, CFile::modeWrite | CFile::typeBinary);

			stdFile.SeekToEnd();
		}
		stdFile.WriteString(initContent);

		stdFile.Flush();
		stdFile.Close();
	}

	ULONGLONG ReadTxt(CString pathDir, CString buf)
	{
		CFile file(pathDir, CFile::modeRead);
		ULONGLONG len = file.GetLength();
		if (len <= 0)
		{
			return -1;
		}

		file.Read((LPTSTR)(LPCTSTR)buf, len);
		file.Close();
		return len;
	}

	void FindMark(CString buf, CString mark, CString &result, int markLength)
	{
		CString buff;
		buff.Format(_T("%s"), buf);
		int begin = buff.Find(mark);

		begin += markLength; 
		result = buff.Mid(begin);

		begin = result.Find(mark);
		result = result.Left(begin);

	}
	void ParseConfig(CString pathDir, enum SetUpType type, CString& Result, CString buf)
	{
		ULONGLONG len = ReadTxt(pathDir, buf);

		CString mark;
		switch (type)
		{
		case Exposure: mark = _T("<Exposure>"); break;
		case Gain: mark = _T("<Gain>"); break;
		case Frame: mark = _T("<Frame>"); break;
		case ProPath: mark = _T("<ProPath>"); break;
		default:{;}
		}

		FindMark(buf, mark, Result, mark.GetLength());

	}
	void FileDialog_k(CString title, CString defaultFileName, CString pathDir, CString initContent)
	{
		CFileDialog fileD(FALSE,(LPCTSTR)".txt",defaultFileName,
			OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT|OFN_NOCHANGEDIR ,(LPCTSTR)szFilter);
		fileD.m_ofn.lpstrTitle = title;
		if (fileD.DoModal() == IDOK)
		{
			if (fileD.GetFileExt().Compare(_T("txt"))!= 0)
			{
				AfxMessageBox(_T("文件类型只能为txt!"), MB_ICONINFORMATION);
				return;
			}
			pathDir = fileD.GetPathName();

			//初始化txt 内容
			WriteTxt(pathDir,initContent);
		}
	}


	int CALLBACK BrowseCallbackProc(HWND hwnd,UINT uMsg,LPARAM lParam,LPARAM lpData)
	{
		switch(uMsg)
		{
		case BFFM_INITIALIZED:
			::SendMessage(hwnd,BFFM_SETSELECTION,TRUE,lpData);
			break;
		}

		return 0;
	}

	unsigned SelectDir( CString &strDir, void *hwnd/* = NULL*/)
	{
		TCHAR szDir[MAX_PATH];
		BROWSEINFO bi; 
		bi.hwndOwner = (HWND)hwnd;
		bi.pidlRoot = NULL;
		bi.pszDisplayName = szDir;
		bi.lpszTitle = _T("选择位置：");
		bi.iImage = 0;

		bi.ulFlags = BIF_USENEWUI | BIF_RETURNONLYFSDIRS;//BIF_USENEWUI： 多了一个新建文件夹选项 

		//BIF_RETURNONLYFSDIRS 只返回文件系统中存在的文件夹  
		//如果有给定路径，转到给定路径，否则默认bi.pidlRoot指定路径
		//CFileFind   finder;
		if(!PathIsDirectory(strDir))
		{
			bi.lParam =0;
			bi.lpfn = NULL;
		}
		else
		{
			bi.lParam = /*(long)*/(LPARAM)(LPCTSTR)(strDir.GetBuffer(strDir.GetLength()));//初始化路径，形如(_T("c:\\Symbian"));
			bi.lpfn = BrowseCallbackProc;
		}
		LPITEMIDLIST lp = SHBrowseForFolder(&bi);
		if (lp && SHGetPathFromIDList(lp,szDir))
		{
			strDir = szDir;
			return IDOK;
		}

		return IDCANCEL;
	}

	CString getEXEPath(CString EXEPath, unsigned int length)
	{
		TCHAR str[MAX_PATH];
		//得到exe的完整路径，包括exe的文件名
		GetModuleFileName(NULL,str,MAX_PATH); //原来这里用 LPWCH(str) 结果路径是每个字符后加了个 0 ！！！
		CString pathDir(str);
		int pos = pathDir.ReverseFind('\\');
		EXEPath = pathDir.Left(pos);

		return EXEPath;
// 		CString strNumCam(numCam);
// 		pathDir = pathDir + _T("\\")+ strNumCam;
// 		if(!PathIsDirectory(pathDir))
// 		{
// 			::CreateDirectory(pathDir,NULL);
// 		}
// 
// 		filePath.Format(_T("%s\\%d.tiff"),pathDir,Count);
// 		filePath.Replace(_T("\\"),_T("\\\\"));

	}

	BOOL CreatePathDir(CString pathDir)
	{
		if(!PathIsDirectory(pathDir))
		{
			if(FALSE == ::CreateDirectory(pathDir,NULL))
			{
				showErrorMessageK(CREATEDIRFAIL);
				return FALSE;
			}
		}

		return TRUE;
	}
}