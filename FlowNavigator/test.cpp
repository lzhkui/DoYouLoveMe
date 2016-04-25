#include "StdAfx.h"
#include "test.h"
#include "afxwin.h"

test::test(void)
{
	TRACE("test is initial!\n");
}

test::~test(void)
{
}
void test::StreamCBFunc(J_tIMAGE_INFO * pAqImageInfo)
{
	TRACE("test is run\n");

}

void test::StreamCBFunc7(J_tIMAGE_INFO * pAqImageInfo)
{
TRACE("test is run\n");
}