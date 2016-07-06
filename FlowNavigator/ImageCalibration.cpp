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

	initital();
}

ImageCalibration::~ImageCalibration()
{
}

void ImageCalibration::initital()
{
	for (int i = 0 ; i < MAX_CAMERAS; i++)
	{
		relatedArray[i]     = NULL;
		//adjustImage[i]      = NULL;
		relatedArraySize[i] = 0;
		memset(&CB_Point[i], 0, sizeof(CB_Point[i]));
	}
	ClbNum = 0;
	CurrentCamSign = -1;

}

void ImageCalibration::Get_unCh(int sign)
{
	ASSERT(sign < MAX_CAMERAS);

	if(relatedArray[sign] == NULL)
	{
		relatedArray[sign] = (unsigned int*)malloc(relatedArraySize[sign] * sizeof(unsigned int));
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

unsigned int* ImageCalibration::getRelatedArray(int sign)
{
	return this->relatedArray[sign];
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

/*
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
*/

void ImageCalibration::setCBPoint(CalibrationPoint* CB_Point, int Num)
{
	ASSERT(Num < MAX_CALIBRATION);

	this->ClbNum = Num;
	for (int i = 0; i < Num; i++)
	{
		this->CB_Point[i] = CB_Point[i];
	}
}

CalibrationPoint* ImageCalibration::getCBPoint(void)
{
	return this->CB_Point;
}

int ImageCalibration::getDlbNum()
{
	return this->ClbNum;
}

float* ImageCalibration::GenerateRelated(int sign)
{
	
	return GenerateRelated(this->CB_Point, sign);
}

float* ImageCalibration::GenerateRelated_(CheckToShow* checkShow, int sign)
{

	return GenerateRelated(checkShow, this->CB_Point, sign);
}

float* ImageCalibration::GenerateRelated(CalibrationPoint* CB_Point, int sign)
{
	float *D = (float*)malloc(ClbNum*2*11*sizeof(float));
	float* e = (float*)malloc(2*ClbNum*sizeof(float));

	for (int i = 0; i < 2*ClbNum; i++)
	{
		int sn = (int)(i / 2 + 0.5);

		if(i % 2 == 0)
		{
			D[i*11]    = CB_Point[sn].x_p;
			D[i*11+1]  = CB_Point[sn].y_p;
			D[i*11+2]  = 0;
			D[i*11+3]  = 1;
			D[i*11+4]  = 0; D[i*11+5]  = 0; D[i*11+6]  = 0; D[i*11+7]  = 0;
			D[i*11+8]  = -(CB_Point[sn].x_p * CB_Point[sn].x_i);
			D[i*11+9]  = -(CB_Point[sn].y_p * CB_Point[sn].x_i);
			D[i*11+10] = 0;

			e[i] = CB_Point[sn].x_i; 
		}
		else
		{
			for (int j = 0; j < 4; j++)
			{
				D[i*11 + j] = 0;
			}
			D[i*11+4]  = CB_Point[sn].x_p;
			D[i*11+5]  = CB_Point[sn].y_p;
			D[i*11+6]  = 0;
			D[i*11+7]  = 1;
			D[i*11+8]  = -(CB_Point[sn].x_p * CB_Point[sn].y_i);
			D[i*11+9]  = -(CB_Point[sn].y_p * CB_Point[sn].y_i);
			D[i*11+10] = 0;

			e[i] = CB_Point[sn].y_i;
		}
	}
#ifdef MYTEST_LOAD_MATLAB_DLL
	GenerateRelated_Use_Matlab_By_Save2File(D, e, sign, ClbNum);
#endif
#ifndef MYTEST_LOAD_MATLAB_DLL
	double* D_T = Matrix_T<double>((double*)D, 2*ClbNum, 11);

	emxArray_real_T* emx_D = emxCreateWrapper_real_T(D_T,2*ClbNum ,11);
	emxArray_real_T* emx_e = emxCreateWrapper_real_T((double*)e, 2*ClbNum, 1);
	emxArray_real_T* L     = emxCreate_real_T(11, 1);

	y(emx_D, emx_e, L);

	memcpy(related[sign], L->data,11*sizeof(float));

#endif
	free(D);
	D = NULL;

	free(e);
	e = NULL;

	return this->related[sign];
}

void ImageCalibration::GenerateRelated_Use_Matlab_By_Save2File(float* D, float* e, int CamSign, int ClbNum)
{
	FILE* fp;
	char defaultPath[MAX_PATH] = DEFAULT_C_USE_MATLAB_TEMPVAR_PATH;
	char tempPath[MAX_PATH]    = {0};

	sprintf(tempPath, "%s\\D.txt", defaultPath); // 也是忽略'\0'的
	fp = fopen(tempPath,"w");
	for(int i = 0; i < 2*ClbNum; i++)
	{
		for(int j = 0; j < 11; j++)
		{
			fprintf(fp, "%f ", D[i*11+j]);
		}
		fprintf(fp,"\n");
	}
	fclose(fp);

	sprintf(tempPath, "%s\\e.txt", defaultPath);
	fp = fopen(tempPath, "w");

	for(int j = 0; j < 2*ClbNum; j++)
	{
		fprintf(fp, "%f\n", e[j]);
	}
	fclose(fp);

	hFig();

	sprintf(tempPath, "%s\\L.txt", defaultPath);
	fp = fopen(tempPath, "r");
	float* result_ = (float*)malloc(11*sizeof(float));

	for(int k = 0; k < 11; k++)
	{
		fscanf(fp, "%f", &(result_[k]));
		TRACE(_T("%f "), result_[k]);
	}
	memcpy(related[CamSign], result_,11*sizeof(float));
	fclose(fp);

	free(result_);
	result_ = NULL;

}

void ImageCalibration::GenerateRelatedArray(AdjustImage* adjustImage, CheckToShow* checkShow, int sign)
{
	float* L = adjustImage->getL();

	int Yrange = adjustImage->getYRange();
	int Xrange = adjustImage->getXRange();

	st_Range range = adjustImage->getSingleRange();

	this->relatedArraySize[sign] = Xrange * Yrange;

	Get_unCh(sign);

	CPoint point;
	float x;
	float y;
	int nRow = 0;
	int nCol = 0;
	for(int i = 0; i < Yrange; i++)
	{
		for(int j = 0; j < Xrange; j++)
		{
			x = ((*(related[sign]  )*(j*L[0]+range.xMin)+ *(related[sign]+1)*(range.yMax-i*L[1]) + *(related[sign]+3)) / (*(related[sign]+8)*(j*L[0]+range.xMin) +
				*(related[sign]+9)*(range.yMax-i*L[1]) + 1) + 0.5);

			y = ((*(related[sign]+4)*(j*L[0]+range.xMin) + *(related[sign]+5)*(range.yMax-i*L[1]) + *(related[sign]+7)) / (*(related[sign]+8)*(j*L[0]+range.xMin) +
				*(related[sign]+9)*(range.yMax-i*L[1]) + 1) + 0.5);

			//nRow = getRowInRawImg(checkShow, point.y);
			//nCol = getColInRawImg(checkShow, point.x, sign);

			nRow = (y - checkShow->getStartHeight()) * CAMERA_PIXEL_ROW / checkShow->getHeight() + 0.5;
			nCol = (x - checkShow->ReturnWeight(sign) * checkShow->getWidth()) * CAMERA_PIXEL_COL / checkShow->getWidth() + 0.5;
			if(nRow < 0 )
			{
				//AfxMessageBox(_T("越界!"), MB_ICONINFORMATION);
				nRow = 0;
			}
			else if(nRow > CAMERA_PIXEL_ROW)
			{
				nRow = CAMERA_PIXEL_ROW;
			}

			if(nCol < 0)
			{
				nCol = 0;
			}
			else if(nCol > CAMERA_PIXEL_COL)
			{
				nCol = CAMERA_PIXEL_COL;
			}

			*(relatedArray[sign] + i * Xrange + j) = nRow * CAMERA_PIXEL_COL + nCol;
		}
	}
	TRACE(_T("lastRelated=%d, nRow=%d, nCol=%d"), *(relatedArray[sign] + Yrange * Xrange - 1), nRow, nCol);

// 	char path_WMTD[MAX_PATH];
// 	sprintf(path_WMTD, "%s[%d]", "D:\\relatedArray", sign);
// 	WriteMatrixToDisk(path_WMTD, Yrange, Xrange, relatedArray[sign]);
}
float* ImageCalibration::GenerateRelated(CheckToShow* checkShow, CalibrationPoint* CB_Point, int sign)
{
	float *D = (float*)malloc(ClbNum*2*11*sizeof(float));
	float* e = (float*)malloc(2*ClbNum*sizeof(float));
	float nRow = -1;
	float nCol = -1;
	int sn     = -1;

	for (int i = 0; i < 2*ClbNum; i++)
	{
		sn = (int)(i / 2 + 0.5);

		nCol = getfloat_ColInRawImg(checkShow, CB_Point[sn].x_i, sign);
		nRow = getfloat_RowInRawImg(checkShow, CB_Point[sn].y_i);
		if(i % 2 == 0)
		{
			D[i*11]    = CB_Point[sn].x_p;
			D[i*11+1]  = CB_Point[sn].y_p;
			D[i*11+2]  = 0;
			D[i*11+3]  = 1;
			D[i*11+4]  = 0; D[i*11+5]  = 0; D[i*11+6]  = 0; D[i*11+7]  = 0;
			D[i*11+8]  = -(CB_Point[sn].x_p * nCol);
			D[i*11+9]  = -(CB_Point[sn].y_p * nCol);
			D[i*11+10] = 0;

			e[i] = nCol; 
		}
		else
		{
			for (int j = 0; j < 4; j++)
			{
				D[i*11 + j] = 0;
			}
			D[i*11+4]  = CB_Point[sn].x_p;
			D[i*11+5]  = CB_Point[sn].y_p;
			D[i*11+6]  = 0;
			D[i*11+7]  = 1;
			D[i*11+8]  = -(CB_Point[sn].x_p * nRow);
			D[i*11+9]  = -(CB_Point[sn].y_p * nRow);
			D[i*11+10] = 0;

			e[i] = nRow;
		}
	}
#ifdef MYTEST_LOAD_MATLAB_DLL
	GenerateRelated_Use_Matlab_By_Save2File(D, e, sign, ClbNum);
#endif
#ifndef MYTEST_LOAD_MATLAB_DLL

	double* D_D = SwitchType_From_T_to_Double<float>(D, 2*ClbNum*11);          //类型转换
	double* D_T = Matrix_T<double>(D_D, 2*ClbNum, 11);                         //转置

	emxArray_real_T* emx_D = emxCreateWrapper_real_T(D_T,2*ClbNum ,11);        

	double* D_e = SwitchType_From_T_to_Double<float>(e, 2*ClbNum);
	emxArray_real_T* emx_e = emxCreateWrapper_real_T(D_e, 2*ClbNum, 1);
	emxArray_real_T* L     = emxCreate_real_T(11, 1);

	y(emx_D, emx_e, L);

	for (int i = 0; i < 11; i++)
	{
		*(related[sign] + i) = (float)(*(L->data + i));
	}

	free(D_D);
	D_D = NULL;
	
	free(D_e);
	D_e = NULL;

	free(D_T);
	D_T = NULL;
#endif
	free(D);
	D = NULL;

	free(e);
	e = NULL;

	return this->related[sign];
}

void ImageCalibration::GenerateRelatedArray_(AdjustImage* adjustImage, CheckToShow* checkShow, int sign)
{
	float* L = adjustImage->getL();

	int Yrange = adjustImage->getYRange();
	int Xrange = adjustImage->getXRange();

	st_Range range = adjustImage->getSingleRange();

	this->relatedArraySize[sign] = Xrange * Yrange;

	Get_unCh(sign);

	CPoint point;
	float x;
	float y;
	int nRow = 0;
	int nCol = 0;
	int err = 1;
	for(int i = 0; i < Yrange; i++)
	{
		for(int j = 0; j < Xrange; j++)
		{
			x = ((*(related[sign]  )*(j*L[0]+range.xMin)+ *(related[sign]+1)*(range.yMax-i*L[1]) + *(related[sign]+3)) / (*(related[sign]+8)*(j*L[0]+range.xMin) +
				*(related[sign]+9)*(range.yMax-i*L[1]) + 1) );

			y = ((*(related[sign]+4)*(j*L[0]+range.xMin) + *(related[sign]+5)*(range.yMax-i*L[1]) + *(related[sign]+7)) / (*(related[sign]+8)*(j*L[0]+range.xMin) +
				*(related[sign]+9)*(range.yMax-i*L[1]) + 1));

			//nRow = getRowInRawImg(checkShow, point.y);
			//nCol = getColInRawImg(checkShow, point.x, sign);

			nRow = (int)(y + 0.5);
			nCol = (int)(x + 0.5);
			if(nRow < 0 )
			{
				//AfxMessageBox(_T("越界!"), MB_ICONINFORMATION);
				//TRACE(_T("校正越界!可能出现错误"));
				err = 1;
				nRow = 0;
			}
			else if(nRow > CAMERA_PIXEL_ROW)
			{
				//TRACE(_T("校正越界!可能出现错误"));
				nRow = CAMERA_PIXEL_ROW;
				err = 1;
			}

			if(nCol < 0)
			{
				//TRACE(_T("校正越界!可能出现错误"));
				err = 1;
				nCol = 0;
			}
			else if(nCol > CAMERA_PIXEL_COL)
			{
				//TRACE(_T("校正越界!可能出现错误"));
				err = 1;
				nCol = CAMERA_PIXEL_COL;
			}
			*(relatedArray[sign] + i * Xrange + j) = nRow * CAMERA_PIXEL_COL + nCol;
		}
	}
	if(err == 1)
	{
		TRACE(_T("校正越界!可能出现错误"));
	}
#ifdef MYTEST_IMAGECALIBRATION
	 	char path_WMTD[MAX_PATH];
	 	sprintf(path_WMTD, "%s[%d].txt", "D:\\relatedArray", sign);
	 	WriteMatrixToDisk(path_WMTD, Yrange, Xrange, relatedArray[sign]);
#endif
}

int ImageCalibration::getRowInRawImg(CheckToShow* checkShow, int ClientPointY)
{
	return (int)(((float)ClientPointY - checkShow->getStartHeight()) * CAMERA_PIXEL_ROW / checkShow->getHeight() + 0.5);
}

int ImageCalibration::getColInRawImg(CheckToShow* checkShow, int ClientPointX, int sign)
{
	return (int)(((float)ClientPointX - checkShow->ReturnWeight(sign) * checkShow->getWidth()) * CAMERA_PIXEL_COL / checkShow->getWidth() + 0.5);
}

float ImageCalibration::getfloat_RowInRawImg(CheckToShow* checkShow, int ClientPointY)
{
	return (((float)ClientPointY - checkShow->getStartHeight()) * CAMERA_PIXEL_ROW / checkShow->getHeight());
}

float ImageCalibration::getfloat_ColInRawImg(CheckToShow* checkShow, int ClientPointX, int sign)
{
	return (((float)ClientPointX - checkShow->ReturnWeight(sign) * checkShow->getWidth()) * CAMERA_PIXEL_COL / checkShow->getWidth());
}
void ImageCalibration::Save_RelatedArray_ToDisk(CONST CHAR* path, int sign)
{
	char fileName[MAX_PATH] = "";
	//strcpy(fileName, path);
	//strcat(fileName, "relatedArray_sign_");
	sprintf(fileName,"%s\\配置文件\\relatedArray_sign_%d.txt", path, sign);

	FILE* fp;
	fp = fopen(fileName, "wb");
	ASSERT(fp != NULL);
	if(fp == NULL)
	{
		return;
	}

// 	for(int i =0;i < relatedArraySize[sign]; i++)
// 	{
// 		fprintf(fp, "%d ", relatedArray[sign]);
// 	}
	//size_t count = fwrite(relatedArray[sign], relatedArraySize[sign], sizeof(unsigned char), fp);
	size_t count = fwrite(relatedArray[sign], sizeof(unsigned int), relatedArraySize[sign], fp);

	ASSERT(count == relatedArraySize[sign]);

	fclose(fp);
}

void ImageCalibration::Load_RelatedArray_ToMemory(CONST CHAR* fullPathName, int sign)
{
	char fileName[MAX_PATH] = "";
	//strcpy(fileName, path);
	//strcat(fileName, "relatedArray_sign_");
	sprintf(fileName,"%s\\配置文件\\relatedArray_sign_%d.txt", fullPathName, sign);

	FILE* fp;
	fp = fopen(fileName, "rb");
	ASSERT(fp != NULL);

	if(fp == NULL)
	{
		return;
	}
	
	Get_unCh(sign);
	size_t count = fread(relatedArray[sign], sizeof(unsigned int), relatedArraySize[sign], fp);
	ASSERT(count == relatedArraySize[sign]);

	if(count != relatedArraySize[sign])
	{
		return;
	}

	fclose(fp);
}

void ImageCalibration::UpdateRelatedArraySize(int sign)
{
	this->relatedArraySize[sign] = this->adjustImage[sign]->getAdjustImageSize();
}

void ImageCalibration::setCurrentCamSign(int sign)
{
	this->CurrentCamSign = sign;
}

int ImageCalibration::getCurrentCamSign()
{
	return this->CurrentCamSign;
}