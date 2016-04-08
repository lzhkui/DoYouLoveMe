#include "StdAfx.h"
#include "ImageInfo.h"

ImageInfo::ImageInfo(void)
{
}

ImageInfo::~ImageInfo(void)
{
}
int ImageInfo::IsMallocOk(pImageNode ImageNode)
{
	if (ImageNode == NULL)
	{
		AfxMessageBox(_T("内存不足!请重启设备."),MB_ICONINFORMATION);
		return -1;
	}
	else
	{
		return 0;
	}
}

pImageNode ImageInfo::CreateLink(void)
{
	pImageNode pImageHead;
	pImageHead = (pImageNode)malloc(sizeof(ImageNode));//这个大小 没有在乎
	IsMallocOk(pImageHead);
	(pImageHead)->next = NULL;
	return pImageHead;
}

void ImageInfo::InsertHead(pImageNode head, pImageNode pImage)
{
	pImage->next = head->next;
	head->next = pImage;
}

void ImageInfo::InsertTail(pImageNode head, pImageNode pImage)
{
	pImageNode temp1 = head;
	pImageNode temp2 = temp1;

	while(temp1 != NULL)
	{
		temp2 = temp1;
		temp1 = temp1->next;
	}
	temp2->next = pImage;
	pImage->next = NULL;
}

void ImageInfo::DelTail(pImageNode head)
{
	if(head == NULL || head->next == NULL)
	{
		return;
	}
	pImageNode temp1 = head;
	pImageNode temp2 = temp1;
	pImageNode temp3 = temp1; 
	while(temp1 != NULL)
	{
		temp3 = temp2;
		temp2 = temp1;
		temp1 = temp1->next;
	}
	temp3->next = NULL;
	free(temp2);
	temp2 = NULL;
}

pImageNode ImageInfo::getTailNode(pImageNode head)
{
	if(head == NULL || head->next == NULL)
	{
		return NULL;
	}
	pImageNode temp1 = head->next;
	pImageNode temp2 = temp1;
	while(temp1 != NULL)
	{
		temp2 = temp1;
		temp1 = temp1->next;
	}
	return temp2;
}
