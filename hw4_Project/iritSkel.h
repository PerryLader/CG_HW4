/*****************************************************************************
* Skeleton for an interface to a parser to read IRIT data files.	     *
******************************************************************************
* Written by Amit Mano						November 2008*
******************************************************************************/

#ifndef	IRIT_SKEL_H
#define	IRIT_SKEL_H

#include <stdlib.h>
#include "irit_sm.h"
#include "iritprsr.h"
#include "attribut.h"
#include "allocate.h"
#include "ip_cnvrt.h"
#include "symb_lib.h"
#include "Model.h"
#include "Geometry.h"



std::vector<Model*> CGSkelProcessIritDataFilesToContainer(CString& FileNames, int NumFiles);
bool CGSkelProcessIritDataFiles(CString& FileNames, int NumFiles);
void CGSkelDumpOneTraversedObject(IPObjectStruct* PObj, IrtHmgnMatType Mat, void* Data);
int CGSkelGetObjectColor(IPObjectStruct* PObj, double RGB[3]);
const char* CGSkelGetObjectTexture(IPObjectStruct* PObj);
const char* CGSkelGetObjectPTexture(IPObjectStruct* PObj);
int CGSkelGetObjectTransp(IPObjectStruct* PObj, double* Transp);
void CGSkelSetTes(int tes);
int CGSkelInverseMatrix(double M[4][4], double InverseM[4][4]);

bool CGSkelStoreData(IPObjectStruct* PObj_src, Geometry** PGeom_dest);
#endif // IRIT_SKEL_H
