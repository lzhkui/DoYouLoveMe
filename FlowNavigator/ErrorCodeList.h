
#define CREATEDIRFAIL_TXT _T("创建项目路径失败")
enum ErrorCodeK
{
	CREATEDIRFAIL = 0,
	CREATENOTTXT
};

enum ParamTypeK
{
	CSTRING = 0,
	STRING,
	P_TCHAR

};

typedef unsigned int UINT_K;
namespace ErrorCodeList
{
	extern BOOL isNULL(CString ErrorTxt, enum ParamTypeK type);
	extern CString ErrorMessageK(enum ErrorCodeList code, CString ErrorTxt);
	extern void showErrorMessageK(enum ErrorCodeK code);
}