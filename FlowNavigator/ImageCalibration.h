#pragma once

#include "AdjustImage.h"
#include "CheckToShow.h"
#include "hFig.h"
#include "KGloabalFunc.h"

// ImageCalibration

#define MAX_CALIBRATION 8

class ImageCalibration
{
public:
	ImageCalibration();
	ImageCalibration(AdjustImage* adjustImage[], int Num = MAX_CAMERAS);
	virtual ~ImageCalibration();

public:
	void initital();
	void Get_unCh(int sign = 0);
	void Free_unCh(int sign = 0);

	unsigned int* getRelatedArray(int sign);

	void setRelatedArraySize(int size, int sign = 0);
	int  getRelatedArraySize(int sign = 0);

	void setRelatedArraySize(int *size, int Num);
	int* getRelatedArraySize();
/*
	void setCBPoint(CalibrationPoint CB_Point, int sign = 0);
	CalibrationPoint getCBPoint(int sign = 0);
*/
	void setCBPoint(CalibrationPoint* CB_Point, int Num);
	CalibrationPoint* getCBPoint(void);

	int getDlbNum();

	float* GenerateRelated(int sign = 0);
	float* GenerateRelated(CalibrationPoint* CB_Point, int sign = 0);

	void   GenerateRelatedArray(AdjustImage* adjustImage, CheckToShow* checkShow, int sign = 0);

	//下面的算法更合适，精确
	float* GenerateRelated_(CheckToShow* checkShow, int sign = 0);
	float* GenerateRelated(CheckToShow* checkShow, CalibrationPoint* CB_Point, int sign = 0);
	void   GenerateRelatedArray_(AdjustImage* adjustImage, CheckToShow* checkShow, int sign = 0);

	int   getRowInRawImg(CheckToShow* checkShow, int ClientPointY);
	int   getColInRawImg(CheckToShow* checkShow, int ClientPointX, int sign = 0);

	float getfloat_RowInRawImg(CheckToShow* checkShow, int ClientPointY);
	float getfloat_ColInRawImg(CheckToShow* checkShow, int ClientPointX, int sign);



	void Save_RelatedArray_ToDisk(CONST CHAR* path, int sign = 0);
	void Load_RelatedArray_ToMemory(CONST CHAR* fullPathName, int sign = 0);
	void UpdateRelatedArraySize(int sign);

	void setCurrentCamSign(int sign);
	int  getCurrentCamSign();

private:
	AdjustImage*    adjustImage[MAX_CAMERAS];

	unsigned int*  relatedArray[MAX_CAMERAS];
	float    related[MAX_CAMERAS][11];
	int      relatedArraySize[MAX_CAMERAS];     
	CalibrationPoint CB_Point[MAX_CALIBRATION];

	int ClbNum;                                 //输入的标定点个数
	int CurrentCamSign;                         //当前标定的相机编号
};


