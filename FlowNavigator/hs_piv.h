extern int Load_PIV_Images_8bit(unsigned char *, unsigned char *, int, int);

extern int Load_PIV_Images_16bit(unsigned short *, unsigned short *, int, int);

extern int Unload_PIV_Images();


extern int HSPIV_Cross_Correlation(float *, float *, int, int, 
					float *, float *, int *, 
					float *, float *, float *);

extern int HSPIV_MQD(float *, float *, int, int, 
					float *, float *, int *, 
					float *, float *, float *);

extern int Match_on_Epipolar_Line(float *, float *, int, int, float *, int, int, int,
					float *, float *, float *);

extern int Match_on_Epipolar_Line_FFT(float *, float *, int, int, float *, int, int,
					float *, float *, float *);
					
extern int img_interp_8bit(unsigned char*, int, int, float*, 
			float*, int, unsigned char*);
			
extern int HSPIV_Cross_Correlation_MP(int, float *, float *, int, int, 
					float *, float *, int *, 
					float *, float *, float *);

extern int HSPIV_MQD_MP(int, float *, float *, int, int, 
					float *, float *, int *, 
					float *, float *, float *);
