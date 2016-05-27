#pragma once
extern int Pair[];        // �ɼ�һ��ͼƬ����piv����
extern BOOL CloseSystem;  //�ر�ϵͳ


//�����ʵ������Χ
typedef struct SingleRange 
{
	float xMin;
	float xMax;
	float yMin;
	float yMax;
}st_Range;

//�ͻ�������Χ
typedef struct ClientRange
{
	float xMin;
	float xMax;
	float yMin;
	float yMax;
}st_ClientRange;

//��ʼ���豸���굥λ����
typedef struct StartPosition
{
	int xStart;
	int yStart;
}st_StartPosition;

//У����ȷ����ѡ����Ŀ�߻�׼
typedef struct BaseWidthANDHeight
{
	int baseWidth;
	int baseHeight;
	int diffWidth;
	int diffHeight;
}st_Base;