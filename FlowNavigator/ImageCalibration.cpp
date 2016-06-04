// ImageCalibration.cpp : 实现文件
//

#include "stdafx.h"
#include "FlowNavigator.h"
#include "ImageCalibration.h"


// ImageCalibration

ImageCalibration::ImageCalibration()
{

}

ImageCalibration::ImageCalibration(AdjustImage* adjustImage[], int Num)
{
	ASSERT(Num <= MAX_CAMERAS);
	for (int i = 0; i < Num; i++)
	{
		this->adjustImage[i] = adjustImage[i];	
	}
}

ImageCalibration::~ImageCalibration()
{
}

void ImageCalibration::initital()
{
	for (int i = 0 ; i < MAX_CAMERAS; i++)
	{
		relatedArray[i]     = NULL;
		adjustImage[i]      = NULL;
		relatedArraySize[i] = 0;
		memset(&CB_Point[i], 0, sizeof(CB_Point));
	}
}

void ImageCalibration::Get_unCh(int sign)
{
	ASSERT(sign < MAX_CAMERAS);

	if(relatedArray[sign] == NULL)
	{
		relatedArray[sign] = (unsigned char*)malloc(relatedArraySize[sign] * sizeof(unsigned char));
		ASSERT(relatedArray[sign] != NULL);
		if(relatedArray[sign] == NULL)
		{
			return;
		}
	}
}

void ImageCalibration::Free_unCh(int sign)
{
	if(relatedArray[sign] != NULL)
	{
		free(relatedArray[sign]);
		relatedArray[sign] = NULL;
	}
}

void ImageCalibration::setRelatedArraySize(int size, int sign)
{
	ASSERT(sign < MAX_CAMERAS);
	this->relatedArraySize[sign] = size;
}

int ImageCalibration::getRelatedArraySize(int sign)
{
	ASSERT(sign < MAX_CAMERAS);
	return this->relatedArraySize[sign];
}

void ImageCalibration::setRelatedArraySize(int *size, int Num)
{
	ASSERT((size != NULL) && (Num < MAX_CAMERAS));
	for (int i = 0; i < Num; i++)
	{
		relatedArraySize[i] = size[i]; 
	}
}

int* ImageCalibration::getRelatedArraySize()
{
	return this->relatedArraySize;
}

void ImageCalibration::setCBPoint(CalibrationPoint CB_Point, int sign)
{
	ASSERT(sign < MAX_CALIBRATION);
	this->CB_Point[sign] = CB_Point;
}

CalibrationPoint ImageCalibration::getCBPoint(int sign)
{
	ASSERT(sign < MAX_CALIBRATION);
	return this->CB_Point[sign];
}

void ImageCalibration::setCBPoint(CalibrationPoint* CB_Point, int Num)
{
	ASSERT(Num < MAX_CALIBRATION);

	for (int i = 0; i < Num; i++)
	{
		this->CB_Point[i] = CB_Point[i];
	}
}

CalibrationPoint* ImageCalibration::getCBPoint()
{
	return this->CB_Point;
}

float* ImageCalibration::GenerateRelated(int sign)
{
	
	return GenerateRelated(this->CB_Point, sign);
}

float* ImageCalibration::GenerateRelated(CalibrationPoint* CB_Point, int sign)
{
	return this->related[sign];
}

void ImageCalibration::GenerateRelatedArray(AdjustImage* adjustImage, CheckToShow* checkShow, int sign)
{
	float* L = adjustImage->getL();

	int Yrange = adjustImage->getYRange();
	int Xrange = adjustImage->getXRange();

	this->relatedArraySize[sign] = Xrange * Yrange;

	CPoint point;
	int nRow = 0;
	int nCol = 0;
	for(int i = 0; i < Yrange; i++)
	{
		for(int j = 0; j < Xrange; j++)
		{
			point.x = (*(related[sign]  )*j*L[0] + *(related[sign]+1)*i*L[1] + *(related[sign]+3)) / (*(related[sign]+8)*j*L[0] +
				*(related[sign]+9)*i*L[1] + 1) + 0.5;

			point.y = (*(related[sign]+4)*j*L[0] + *(related[sign]+5)*i*L[1] + *(related[sign]+7)) / (*(related[sign]+8)*j*L[0] +
				*(related[sign]+9)*i*L[1] + 1) + 0.5;

			nRow = getRowInRawImg(checkShow, point.y);
			nCol = getColInRawImg(checkShow, point.x, sign);

			*(relatedArray[sign] + i * Xrange + j) = nRow * CAMERA_PIXEL_COL + nCol;
		}
	}
}

int ImageCalibration::getRowInRawImg(CheckToShow* checkShow, int ClientPointY)
{
	return (int)((ClientPointY - checkShow->getStartHeight()) * CAMERA_PIXEL_ROW / checkShow->getHeight() + 0.5);
}

int ImageCalibration::getColInRawImg(CheckToShow* checkShow, int ClientPointX, int sign)
{
	return (int)((ClientPointX - checkShow->ReturnWeight(sign) * checkShow->getWidth()) * CAMERA_PIXEL_COL / checkShow->getWidth() + 0.5);
}

void ImageCalibration::Save_RelatedArray_ToDisk(CONST CHAR* path, int sign)
{
	char fileName[MAX_PATH] = "";
	strcpy(fileName, path);
	//strcat(fileName, "relatedArray_sign_");
	sprintf(fileName,"%s\\relatedArray_sign_%d.txt", fileName, sign);

	FILE* fp;
	fp = fopen(fileName, "wb");
	ASSERT(fp != NULL);
	if(fp == NULL)
	{
		return;
	}

	size_t count = fwrite(relatedArray[sign], relatedArraySize[sign], sizeof(unsigned char), fp);
	ASSERT(count == relatedArraySize[sign]);

	fclose(fp);
}

void ImageCalibration::Load_RelatedArray_ToMemory(CONST CHAR* fullPathName, int sign)
{
	FILE* fp;
	fp = fopen(fullPathName, "rb");
	ASSERT(fp != NULL);

	if(fp == NULL)
	{
		return;
	}
	
	Get_unCh(sign);
	size_t count = fread(relatedArray[sign], relatedArraySize[sign], sizeof(unsigned char), fp);
	ASSERT(count == relatedArraySize[sign]);

	if(count != relatedArraySize[sign])
	{
		return;
	}

	fclose(fp);
}