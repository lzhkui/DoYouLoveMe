// 
// File: y_types.h 
//  
// MATLAB Coder version            : 2.7 
// C/C++ source code generated on  : 05-Jul-2016 16:59:34 
//

#ifndef __Y_TYPES_H__
#define __Y_TYPES_H__

// Include Files 
#include "rtwtypes.h"

// Type Definitions 
#ifndef struct_emxArray__common
#define struct_emxArray__common
struct emxArray__common
{
    void *data;
    int *size;
    int allocatedSize;
    int numDimensions;
    boolean_T canFreeData;
};
#endif /*struct_emxArray__common*/
#ifndef struct_emxArray_real_T
#define struct_emxArray_real_T
struct emxArray_real_T
{
    double *data;
    int *size;
    int allocatedSize;
    int numDimensions;
    boolean_T canFreeData;
};
#endif /*struct_emxArray_real_T*/
#ifndef struct_emxArray_real_T_11
#define struct_emxArray_real_T_11
struct emxArray_real_T_11
{
    double data[11];
    int size[1];
};
#endif /*struct_emxArray_real_T_11*/

#endif
// 
// File trailer for y_types.h 
//  
// [EOF] 
//
