#pragma once

#include "jai_factory.h"

typedef struct st_test
{
	J_tIMAGE_INFO AqImageInfo;
	//unsigned int iImageSize;

	unsigned char* pImageBuffer;
	st_test *next;
}ImageNode; 
typedef struct st_test *pImageNode;

class ImageInfo
{
public:
	ImageInfo(void);
	~ImageInfo(void);
	pImageNode CreateLink(void);
	static void ImageInfo::InsertHead(pImageNode head, pImageNode pImage);
	static int IsMallocOk(pImageNode ImageNode);
	static void InsertTail(pImageNode head, pImageNode pImage);
	static void DelTail(pImageNode head);
	static pImageNode getTailNode(pImageNode head);
};
