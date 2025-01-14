#include "stdafx.h"
#include "iritSkel.h"

/*****************************************************************************
* Skeleton for an interface to a parser to read IRIT data files.			 *
******************************************************************************
* (C) Gershon Elber, Technion, Israel Institute of Technology                *
******************************************************************************
* Written by:  Gershon Elber				Ver 1.0, Feb 2002				 *
* Minimal changes made by Amit Mano			November 2008					 *
******************************************************************************/


//Shalom My name is Shachar

class SavingModel {
public:
	// Public method to access the single instance
	static SavingModel& getInstance() {
		static SavingModel instance; // Guaranteed to be destroyed and instantiated on first use
		return instance;
	}
	static bool pushGeom(Geometry* Geom) {
		if(Geom && getInstance().m_models){
			StaticModel* model = new StaticModel(Geom);
			//model->centerizedGeometry();
			getInstance().m_models->push_back(model);
			return true;
		}
		return false;
	}
	// Delete copy constructor and assignment operator to prevent copying
	SavingModel(const SavingModel&) = delete;
	SavingModel& operator=(const SavingModel&) = delete;

	// Method to set the Model pointer
	void set(std::vector<Model*>* container) {
		this->m_models = container;
	}
	// Method to clear the Model pointer
	void handle_missuse(bool insertion_flag) const{
		if (insertion_flag)
			return;
		for (Model* model : *m_models) {
			delete model;
		}
		m_models->clear();
	}
	void release() {
		this->m_models = nullptr;
	}

private:
	// Private constructor to prevent direct instantiation
	SavingModel() : m_models(nullptr) {
	}
	// Pointer to the Model object
	std::vector<Model*>* m_models;
};

int tessellation = 20;
IPFreeformConvStateStruct CGSkelFFCState = {
	FALSE,          /* Talkative */
	FALSE,          /* DumpObjsAsPolylines */
	TRUE,           /* DrawFFGeom */
	FALSE,          /* DrawFFMesh */
	{ 10, 10, 10 }, /* 10 isocurves peru/v/w direction. */
	100,            /* 100 point samples along a curve. */
	SYMB_CRV_APPROX_UNIFORM,  /* CrvApproxMethod */
	FALSE,   /* ShowIntrnal */
	FALSE,   /* CubicCrvsAprox */
	20,      /* Polygonal FineNess */
	FALSE,   /* ComputeUV */
	TRUE,    /* ComputeNrml */
	FALSE,   /* FourPerFlat */
	0,       /* OptimalPolygons */
	FALSE,   /* BBoxGrid */
	TRUE,    /* LinearOnePolyFlag */
	FALSE
};

/*****************************************************************************
* DESCRIPTION:                                                               *
* Use as parser from .itd to Model struct 								     *
*                                                                            *
* PARAMETERS:                                                                *
*   FileNames:  Files to open and read, as a vector of strings.              *
*   NumFiles:   Length of the FileNames vector.								 *
*   modelGC:   Storing model.												 *
*                                                                            *
* RETURN VALUE:                                                              *
*   bool:		false - fail, true - success.                                *
*****************************************************************************/

bool CGSkelProcessIritDataFilesToContainer(CString& FileNames, int NumFiles, std::vector<Model*>& Container) {
	SavingModel::getInstance().set(&Container);
	bool res;
	SavingModel::getInstance().handle_missuse(res = CGSkelProcessIritDataFiles(FileNames, NumFiles));
	SavingModel::getInstance().release();
	return res;
}
void CGSkelSetTes(int tes) {
	tessellation = tes;
}
//CGSkelProcessIritDataFiles(argv + 1, argc - 1);

/*****************************************************************************
* DESCRIPTION:                                                               *
* Main module of skeleton - Read command line and do what is needed...	     *
*                                                                            *
* PARAMETERS:                                                                *
*   FileNames:  Files to open and read, as a vector of strings.              *
*   NumFiles:   Length of the FileNames vector.								 *
*                                                                            *
* RETURN VALUE:                                                              *
*   bool:		false - fail, true - success.                                *
*****************************************************************************/
bool CGSkelProcessIritDataFiles(CString& FileNames, int NumFiles)
{
	IPObjectStruct* PObjects;
	IrtHmgnMatType CrntViewMat;
	IPTraverseObjHierarchyStruct TraversState;
	/* Get the data files: */
	IPSetFlattenObjects(FALSE);
	CStringA CStr(FileNames);
	if ((PObjects = IPGetDataFiles((const char* const*)&CStr, 1/*NumFiles*/, TRUE, FALSE)) == NULL)
		return false;
	PObjects = IPResolveInstances(PObjects);

	if (IPWasPrspMat)
		MatMultTwo4by4(CrntViewMat, IPViewMat, IPPrspMat);
	else
		IRIT_GEN_COPY(CrntViewMat, IPViewMat, sizeof(IrtHmgnMatType));

	/* Here some useful parameters to play with in tesselating freeforms: */
	CGSkelFFCState.FineNess = tessellation;   /* Res. of tesselation, larger is finer. */
	CGSkelFFCState.ComputeUV = TRUE;   /* Wants UV coordinates for textures. */
	CGSkelFFCState.FourPerFlat = TRUE;/* 4 poly per ~flat patch, 2 otherwise.*/
	CGSkelFFCState.LinearOnePolyFlag = TRUE;    /* Linear srf gen. one poly. */

	/* Traverse ALL the parsed data, recursively. */
	IPTraverseObjHierarchyInitState(&TraversState);
	TraversState.ApplyFunc = (IPApplyObjFuncType)CGSkelDumpOneTraversedObject;
	IRIT_GEN_COPY(TraversState.Mat, CrntViewMat, sizeof(IrtHmgnMatType));
	IPTraverseObjListHierarchy(PObjects, &TraversState);
	return true;
}

/*****************************************************************************
* DESCRIPTION:                                                               *
*   Call back function of IPTraverseObjListHierarchy. Called on every non    *
* list object found in hierarchy.                                            *
*                                                                            *
* PARAMETERS:                                                                *
*   PObj:       Non list object to handle.                                   *
*   Mat:        Transformation matrix to apply to this object.               *
*   Data:       Additional data.                                             *
*                                                                            *
* RETURN VALUE:                                                              *
*   void									                                 *
*****************************************************************************/

void CGSkelDumpOneTraversedObject(IPObjectStruct* PObj,
	IrtHmgnMatType Mat,
	void* Data)
{
	IPObjectStruct* PObjs;
	if (IP_IS_FFGEOM_OBJ(PObj))
		PObjs = IPConvertFreeForm(PObj, &CGSkelFFCState);
	else
		PObjs = PObj;

	for (PObj = PObjs; PObj != NULL; PObj = PObj->Pnext){
		Geometry* PGeom = nullptr;
		if (!CGSkelStoreData(PObj, &PGeom))
			exit(1);
		else
			SavingModel::pushGeom(PGeom);
	}
}

/*****************************************************************************
* DESCRIPTION:                                                               *
*   Store the data from given geometry object in model.						 *
*                                                                            *
* PARAMETERS:                                                                *
*   PObj_src:       Object to store.                                             *
*   PGeom_dest:      Model Object.												 *
*                                                                            *
* RETURN VALUE:                                                              *
*   bool:		false - fail, true - success.                                *
*****************************************************************************/
bool CGSkelStoreData(IPObjectStruct* PObj_src, Geometry** PGeom_dest)
{
	if (!PObj_src)
		return false;
	int i;
	const char* Str;
	double RGB[3], Transp;
	IPPolygonStruct* PPolygon;
	IPVertexStruct* PVertex;
	RGB[0] = 100;//default value
	RGB[1] = 22;//default value
	RGB[2] = 222;//default value
	const IPAttributeStruct* Attrs =
		AttrTraceAttributes(PObj_src->Attr, PObj_src->Attr);
	if (PObj_src->ObjType != IP_OBJ_POLY) {
		AfxMessageBox(_T("Non polygonal object detected and ignored"));
		return true;
	}

	/* You can use IP_IS_POLYGON_OBJ(PObj) and IP_IS_POINTLIST_OBJ(PObj)
	   to identify the type of the object*/
	if (!IP_IS_POLYGON_OBJ(PObj_src)) {
		AfxMessageBox(_T("Non polygonal object detected and ignored"));
		return true;
	}



	
	uint8_t red =0;//green
	uint8_t green =165;//red
	uint8_t blue =160;//a
	ColorGC color = ColorGC::defaultColor();
	if (!CGSkelGetObjectColor(PObj_src, RGB))
	{
		AfxMessageBox(_T("No color for the object"));
	}
	else
	{
		//NORMOLAZING
		 red = static_cast<uint8_t>(RGB[0] * 255.0);
		 green = static_cast<uint8_t>(RGB[1] * 255.0);
		 blue = static_cast<uint8_t>(RGB[2] * 255.0);
		 color = ColorGC(red, green, blue);

	}


	if (CGSkelGetObjectTransp(PObj_src, &Transp))
	{
		/* transparency code */
	}
	if ((Str = CGSkelGetObjectTexture(PObj_src)) != NULL)
	{
		/* volumetric texture code */
	}
	if ((Str = CGSkelGetObjectPTexture(PObj_src)) != NULL)
	{
		/* parametric texture code */
	}
	if (Attrs != NULL)
	{
		printf("[OBJECT\n");
		while (Attrs) {
			/* attributes code */
			Attrs = AttrTraceAttributes(Attrs, NULL);
		}
	}
	Geometry* shape = new Geometry(PObj_src->ObjName, color);

	
	for (PPolygon = PObj_src->U.Pl; PPolygon != NULL; PPolygon = PPolygon->Pnext)
	{
		if (PPolygon->PVertex == NULL) {
			AfxMessageBox(_T("Dump: Attemp to dump empty polygon"));
			return false;
		}

		///* Count number of vertices. */
		//for (PVertex = PPolygon->PVertex->Pnext, i = 1;
		//	PVertex != PPolygon->PVertex && PVertex != NULL;
		//	PVertex = PVertex->Pnext, i++);

		/* use if(IP_HAS_PLANE_POLY(PPolygon)) to know whether a normal is defined for the polygon
		   access the normal by the first 3 components of PPolygon->Plane */

		


		PVertex = PPolygon->PVertex;
		PolygonGC* newPoly;
		
			newPoly = new PolygonGC(color);
		do {			     /* Assume at least one edge in polygon! */
			/* code handeling all vertex/normal/texture coords */
			std::shared_ptr<Vertex> newVert;
			Vector3 pos = Vector3(PVertex->Coord[0], PVertex->Coord[1], PVertex->Coord[2]);

			if (IP_HAS_NORMAL_VRTX(PVertex))
			{
				if (shape->m_map.find(pos) == shape->m_map.end()) {
					shape->m_map[pos]= std::shared_ptr<Vertex>( new Vertex(pos,
						Vector3(PVertex->Normal[0],
								PVertex->Normal[1],
								PVertex->Normal[2])));
					newVert = shape->m_map[pos];
					newVert->addNeigberPolygon(newPoly);
				}
				else
				{
					newVert = shape->m_map[pos];
					newVert->addNeigberPolygon(newPoly);					
				}
			}
			else {
				if (shape->m_map.find(pos) == shape->m_map.end()) {
					shape->m_map[pos] = std::shared_ptr<Vertex>(new Vertex(pos));
					newVert = shape->m_map[pos];
					newVert->addNeigberPolygon(newPoly);
				}
				else
				{
					newVert = shape->m_map[pos];
					newVert->addNeigberPolygon(newPoly);
				}
			}

			newPoly->addVertex(newVert);
			PVertex = PVertex->Pnext;
		} while (PVertex != PPolygon->PVertex && PVertex != NULL);
		
		if (IP_HAS_PLANE_POLY(PPolygon)) {
			const Vector3 polygon_data_normal = Vector3(PPolygon->Plane[0], PPolygon->Plane[1], PPolygon->Plane[2]);
			newPoly->setCalcAndDataNormalLines(polygon_data_normal);
		}
		else
		{
			newPoly->setCalcNormalLines();
		}
		
		/* Close the polygon. */
		shape->addPolygon(newPoly);
	}
	shape->calcVertxNormal();
	/* Close the object. */
	*PGeom_dest = shape;
	return true;
}

/*****************************************************************************
* DESCRIPTION:                                                               *
*   Returns the color of an object.                                          *
*                                                                            *
* PARAMETERS:                                                                *
*   PObj:   Object to get its color.                                         *
*   RGB:    as 3 floats in the domain [0, 1].                                *
*                                                                            *
* RETURN VALUE:                                                              *
*   int:    TRUE if object has color, FALSE otherwise.                       *
*****************************************************************************/
int CGSkelGetObjectColor(IPObjectStruct* PObj, double RGB[3])
{
	static int TransColorTable[][4] = {
		{ /* BLACK	*/   0,    0,   0,   0 },
		{ /* BLUE	*/   1,    0,   0, 255 },
		{ /* GREEN	*/   2,    0, 255,   0 },
		{ /* CYAN	*/   3,    0, 255, 255 },
		{ /* RED	*/   4,  255,   0,   0 },
		{ /* MAGENTA 	*/   5,  255,   0, 255 },
		{ /* BROWN	*/   6,   50,   0,   0 },
		{ /* LIGHTGRAY	*/   7,  127, 127, 127 },
		{ /* DARKGRAY	*/   8,   63,  63,  63 },
		{ /* LIGHTBLUE	*/   9,    0,   0, 255 },
		{ /* LIGHTGREEN	*/   10,   0, 255,   0 },
		{ /* LIGHTCYAN	*/   11,   0, 255, 255 },
		{ /* LIGHTRED	*/   12, 255,   0,   0 },
		{ /* LIGHTMAGENTA */ 13, 255,   0, 255 },
		{ /* YELLOW	*/   14, 255, 255,   0 },
		{ /* WHITE	*/   15, 255, 255, 255 },
		{ /* BROWN	*/   20,  50,   0,   0 },
		{ /* DARKGRAY	*/   56,  63,  63,  63 },
		{ /* LIGHTBLUE	*/   57,   0,   0, 255 },
		{ /* LIGHTGREEN	*/   58,   0, 255,   0 },
		{ /* LIGHTCYAN	*/   59,   0, 255, 255 },
		{ /* LIGHTRED	*/   60, 255,   0,   0 },
		{ /* LIGHTMAGENTA */ 61, 255,   0, 255 },
		{ /* YELLOW	*/   62, 255, 255,   0 },
		{ /* WHITE	*/   63, 255, 255, 255 },
		{		     -1,   0,   0,   0 }
	};
	int i, j, Color, RGBIColor[3];

	if (AttrGetObjectRGBColor(PObj,
		&RGBIColor[0], &RGBIColor[1], &RGBIColor[2])) {
		for (i = 0; i < 3; i++)
			RGB[i] = RGBIColor[i] / 255.0;

		return TRUE;
	}
	else if ((Color = AttrGetObjectColor(PObj)) != IP_ATTR_NO_COLOR) {
		for (i = 0; TransColorTable[i][0] >= 0; i++) {
			if (TransColorTable[i][0] == Color) {
				for (j = 0; j < 3; j++)
					RGB[j] = TransColorTable[i][j + 1] / 255.0;
				return TRUE;
			}
		}
	}

	return FALSE;
}

/*****************************************************************************
* DESCRIPTION:                                                               *
*   Returns the volumetric texture of an object, if any.                     *
*                                                                            *
* PARAMETERS:                                                                *
*   PObj:   Object to get its volumetric texture.                            *
*                                                                            *
* RETURN VALUE:                                                              *
*   char *:    Name of volumetric texture map to apply, NULL if none.        *
*****************************************************************************/
const char* CGSkelGetObjectTexture(IPObjectStruct* PObj)
{
	return AttrGetObjectStrAttrib(PObj, "texture");
}

/*****************************************************************************
* DESCRIPTION:                                                               *
*   Returns the parametric texture of an object, if any.                     *
*                                                                            *
* PARAMETERS:                                                                *
*   PObj:   Object to get its parametric texture.                            *
*                                                                            *
* RETURN VALUE:                                                              *
*   char *:    Name of parametric texture map to apply, NULL if none.        *
*****************************************************************************/
const char* CGSkelGetObjectPTexture(IPObjectStruct* PObj)
{
	return AttrGetObjectStrAttrib(PObj, "ptexture");
}

/*****************************************************************************
* DESCRIPTION:                                                               *
*   Returns the transparency level of an object, if any.                     *
*                                                                            *
* PARAMETERS:                                                                *
*   PObj:   Object to get its volumetric texture.                            *
*   Transp: Transparency level between zero and one.                         *
*                                                                            *
* RETURN VALUE:                                                              *
*   int:    TRUE if object has transparency, FALSE otherwise.                *
*****************************************************************************/
int CGSkelGetObjectTransp(IPObjectStruct* PObj, double* Transp)
{
	*Transp = AttrGetObjectRealAttrib(PObj, "transp");

	return !IP_ATTR_IS_BAD_REAL(*Transp);
}


/*****************************************************************************
* DESCRIPTION:                                                               *
*   Computes the ivnerse of a 4x4 matrix.                                    *
*                                                                            *
* PARAMETERS:                                                                *
*   M: The matrix to invert.                                                 *
*   InverseM: The resulting inverse matrix.                                  *
*                                                                            *
* RETURN VALUE:                                                              *
*   int:    TRUE if matrix has inverse, FALSE otherwise.                     *
*****************************************************************************/
int CGSkelInverseMatrix(double M[4][4], double InverseM[4][4])
{
	IrtHmgnMatType Mat, InverseMat;
	int i, j;

	/* IrtHmgnMatType uses IritRType which should be double */
	/* Copy explicitly just incase. */
	for (i = 0; i < 4; i++)
		for (j = 0; j < 4; j++)
			Mat[i][j] = M[i][j];
	if (!MatInverseMatrix(Mat, InverseMat))
		return FALSE;
	for (i = 0; i < 4; i++)
		for (j = 0; j < 4; j++)
			InverseM[i][j] = InverseMat[i][j];
	return TRUE;
}
