#include "StdAfx.h"
#include "ErrorCodeList.h"

namespace ErrorCodeList
{
	BOOL isNULL(CString ErrorTxt, enum ParamTypeK type)
	{
		switch (type)
		{
		case CSTRING:
		case STRING:
		case P_TCHAR:
			if (ErrorTxt.IsEmpty())
			{
				return FALSE;
			}
			else
			{
				return TRUE;
			}
			break;
		default :
			{
				;
			}
		}
		
	}
	CString ErrorMessageK(enum ErrorCodeK code, CString ErrorTxt)
	{
		if(FALSE == isNULL(ErrorTxt, CSTRING))
		{
			return NULL;
		}

		ErrorTxt.Format(_T("code%d: %s"), code, ErrorTxt);

		return ErrorTxt;
	}

	void showErrorMessageK(enum ErrorCodeK code)
	{
		CString ErrorTxt;
		switch(code)
		{


		case CREATEDIRFAIL: ErrorTxt = _T("创建项目路径失败！"); break;
		case CREATENOTTXT : ErrorTxt = _T("只能为文本类型txt!"); break;

		default:
			{
				ErrorTxt = _T("未知错误");//这句不执行的...
			}
		}
		ErrorTxt = ErrorMessageK(code, ErrorTxt);
		

		AfxMessageBox(ErrorTxt, MB_ICONINFORMATION);
	}
}