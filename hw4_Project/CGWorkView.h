// CGWorkView.h : interface of the CCGWorkView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_CGWORKVIEW_H__5857316D_EA60_11D5_9FD5_00D0B718E2CD__INCLUDED_)
#define AFX_CGWORKVIEW_H__5857316D_EA60_11D5_9FD5_00D0B718E2CD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "gl\gl.h"    // Include the standard CGWork  headers
#include "gl\glu.h"   // Add the utility library

#include "Light.h"
#include "Scene.h"
#include "ScreenCommand.h"

class CCGWorkView : public CView
{
protected: // create from serialization only
	CCGWorkView();
	DECLARE_DYNCREATE(CCGWorkView)

	// Attributes
public:
	CCGWorkDoc* GetDocument();

	// Operations
public:

private:
	Scene m_scene;
	RenderMode m_rendermode;
	int m_nAxis;				// Axis of Action, X Y or Z
	int m_nAction;				// Rotate, Translate, Scale
	int m_nView;				// Orthographic, perspective
	float m_sensitivity; 
	float m_perspectiveAngle;
	bool m_bIsPerspective;			// is the view perspective
	float m_depth;
	int m_tSpace;
	CString m_strItdFileName;		// file name of IRIT data
	int m_nLightShading;			// shading: Flat, Gouraud.

	double m_lMaterialAmbient;		// The Ambient in the scene
	double m_lMaterialDiffuse;		// The Diffuse in the scene
	double m_lMaterialSpecular;		// The Specular in the scene
	int m_nMaterialCosineFactor;		// The cosine factor for the specular
	bool m_bLeftButtonDown;
	CPoint m_ref_point;
	LightParams m_lights[MAX_LIGHT];	//configurable lights array
	LightParams m_ambientLight;		//ambient light (only RGB is used)
	int m_sceneSpecExp;		//ambient light (only RGB is used)

	RenderCommand createRenderingCommand(int width, int height);
	TransformationCommand createTransformationCommand(const Vector3& point);
	// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CCGWorkView)
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CCGWorkView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	BOOL InitializeCGWork();
	BOOL SetupViewingFrustum(void);
	BOOL SetupViewingOrthoConstAspect(void);

	virtual void RenderScene();


	HGLRC    m_hRC;			// holds the Rendering Context
	CDC* m_pDC;			// holds the Device Context
	int m_WindowWidth;		// hold the windows width
	int m_WindowHeight;		// hold the windows height

	double m_AspectRatio;		// hold the fixed Aspect Ration

	bool m_SaveToFile;
	int m_PngWidth;
	int	m_PngHeight;

	HBITMAP m_pDbBitMap;
	CDC* m_pDbDC;

	// Generated message map functions
protected:
	//{{AFX_MSG(CCGWorkView)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnFileLoad();
	afx_msg void OnViewOrthographic();
	afx_msg void OnUpdateViewOrthographic(CCmdUI* pCmdUI);
	afx_msg void OnViewPerspective();
	afx_msg void OnUpdateViewPerspective(CCmdUI* pCmdUI);
	afx_msg void OnActionRotate();
	afx_msg void OnUpdateActionRotate(CCmdUI* pCmdUI);
	afx_msg void OnActionScale();
	afx_msg void OnUpdateActionScale(CCmdUI* pCmdUI);
	afx_msg void OnActionTranslate();
	afx_msg void OnUpdateActionTranslate(CCmdUI* pCmdUI);
	afx_msg void OnAxisX();
	afx_msg void OnUpdateAxisX(CCmdUI* pCmdUI);
	afx_msg void OnAxisY();
	afx_msg void OnUpdateAxisY(CCmdUI* pCmdUI);
	afx_msg void OnAxisZ();
	afx_msg void OnUpdateAxisZ(CCmdUI* pCmdUI);
	afx_msg void OnAxisXY();
	afx_msg void OnUpdateAxisXY(CCmdUI* pCmdUI);
	afx_msg void OnAxisXYZ();
	afx_msg void OnUpdateAxisXYZ(CCmdUI* pCmdUI);
	afx_msg void OnLightShadingFlat();
	afx_msg void OnUpdateLightShadingFlat(CCmdUI* pCmdUI);
	afx_msg void OnLightShadingGouraud();
	afx_msg void OnUpdateLightShadingGouraud(CCmdUI* pCmdUI);
	afx_msg void OnLightConstants();
	afx_msg void OnShowCalcPolyNormals();
	afx_msg void OnUpdateShowCalcPolyNormals(CCmdUI* pCmdUI);
	afx_msg void OnShowCalcVertNormals();
	afx_msg void OnUpdateShowCalcVertNormals(CCmdUI* pCmdUI);
	afx_msg void OnShowIritPolyNormals();
	afx_msg void OnUpdateShowIritPolyNormals(CCmdUI* pCmdUI);
	afx_msg void OnShowIritVertNormals();
	afx_msg void OnUpdateShowIritVertNormals(CCmdUI* pCmdUI);
	afx_msg void OnShowBBox();
	afx_msg void OnUpdateShowBBox(CCmdUI* pCmdUI);
	afx_msg void OnNormalsColor();
	afx_msg void OnUpdateNormalsColor(CCmdUI* pCmdUI);
	afx_msg void OnWireframeColor();
	afx_msg void OnUpdateWireframeColor(CCmdUI* pCmdUI);
	afx_msg void OnBgColor();
	afx_msg void OnBgPicture();
	afx_msg void OnTransformationSpace();
	afx_msg void OnUpdateTransformationSpace(CCmdUI* pCmdUI);
	afx_msg void OnViewAngle();
	afx_msg void OnUpdateViewAngle(CCmdUI* pCmdUI);
	afx_msg void OnTessellation();
	afx_msg void OnSensitivity();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnFileRender();
	afx_msg void OnUpdateFileRender(CCmdUI* pCmdUI);
	afx_msg void OnFileSetDimension();
	afx_msg void OnUpdateFileSetDimension(CCmdUI* pCmdUI);
	afx_msg void OnBgStrech();
	afx_msg void OnUpdateBgStrech(CCmdUI* pCmdUI);
	afx_msg void OnBgRepeat();
	afx_msg void OnUpdateBgRepeat(CCmdUI* pCmdUI);
	afx_msg void OnBgSolid();
	afx_msg void OnUpdateBgSolid(CCmdUI* pCmdUI);
	afx_msg void OnShadeSolid();
	afx_msg void OnUpdateShadeSolid(CCmdUI* pCmdUI);
	afx_msg void OnShadeNone();
	afx_msg void OnUpdateShadeNone(CCmdUI* pCmdUI);
	afx_msg void OnShadeGouroud();
	afx_msg void OnUpdateShadeGouroud(CCmdUI* pCmdUI);
	afx_msg void OnShadePhong();
	afx_msg void OnUpdateShadePhong(CCmdUI* pCmdUI);
	afx_msg void OnRenderWireFrame();
	afx_msg void OnUpdateWireFrame(CCmdUI* pCmdUI);
	afx_msg void OnRenderSilohette();
	afx_msg void OnUpdateSilohette(CCmdUI* pCmdUI);
	afx_msg void OnFlipNoramls();
	afx_msg void OnUpdateFlipNoramls(CCmdUI* pCmdUI);
	afx_msg void OnBackFaceCull();
	afx_msg void OnUpdateBackFaceCull(CCmdUI* pCmdUI);
	afx_msg void OnRenderDynamic();
	afx_msg void OnUpdateRenderDynamic(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};

#ifndef _DEBUG  // debug version in CGWorkView.cpp
inline CCGWorkDoc* CCGWorkView::GetDocument()
{
	return (CCGWorkDoc*)m_pDocument;
}
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CGWORKVIEW_H__5857316D_EA60_11D5_9FD5_00D0B718E2CD__INCLUDED_)
