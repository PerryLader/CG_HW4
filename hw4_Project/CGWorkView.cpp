// CGWorkView.cpp : implementation of the CCGWorkView class
//
#include "stdafx.h"
#include "CGWork.h"

#include "CGWorkDoc.h"
#include "CGWorkView.h"

#include <iostream>
using std::cout;
using std::endl;
#include "MaterialDlg.h"
#include "LightDialog.h"
#include "dynamicSliderDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#include "PngWrapper.h"
#include "iritSkel.h"
#include <algorithm>
#include "MainFrm.h"
#include <thread>
#include <atomic>
#include <chrono>
// Use this macro to display text messages in the status bar.
#define STATUS_BAR_TEXT(str) (((CMainFrame*)GetParentFrame())->getStatusBar().SetWindowText(str))


/////////////////////////////////////////////////////////////////////////////
// CCGWorkView

IMPLEMENT_DYNCREATE(CCGWorkView, CView)

BEGIN_MESSAGE_MAP(CCGWorkView, CView)
	//{{AFX_MSG_MAP(CCGWorkView)
	ON_WM_ERASEBKGND()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_COMMAND(ID_FILE_LOAD, OnFileLoad)
	ON_COMMAND(ID_VIEW_ORTHOGRAPHIC, OnViewOrthographic)
	ON_UPDATE_COMMAND_UI(ID_VIEW_ORTHOGRAPHIC, OnUpdateViewOrthographic)
	ON_COMMAND(ID_VIEW_PERSPECTIVE, OnViewPerspective)
	ON_UPDATE_COMMAND_UI(ID_VIEW_PERSPECTIVE, OnUpdateViewPerspective)
	ON_COMMAND(ID_ACTION_ROTATE, OnActionRotate)
	ON_UPDATE_COMMAND_UI(ID_ACTION_ROTATE, OnUpdateActionRotate)
	ON_COMMAND(ID_ACTION_SCALE, OnActionScale)
	ON_UPDATE_COMMAND_UI(ID_ACTION_SCALE, OnUpdateActionScale)
	ON_COMMAND(ID_ACTION_TRANSLATE, OnActionTranslate)
	ON_UPDATE_COMMAND_UI(ID_ACTION_TRANSLATE, OnUpdateActionTranslate)
	ON_COMMAND(ID_AXIS_X, OnAxisX)
	ON_UPDATE_COMMAND_UI(ID_AXIS_X, OnUpdateAxisX)
	ON_COMMAND(ID_AXIS_Y, OnAxisY)
	ON_UPDATE_COMMAND_UI(ID_AXIS_Y, OnUpdateAxisY)
	ON_COMMAND(ID_AXIS_Z, OnAxisZ)
	ON_UPDATE_COMMAND_UI(ID_AXIS_Z, OnUpdateAxisZ)
	ON_COMMAND(ID_AXIS_XY, OnAxisXY)
	ON_UPDATE_COMMAND_UI(ID_AXIS_XY, OnUpdateAxisXY)
	ON_COMMAND(ID_AXIS_XYZ, OnAxisXYZ)
	ON_UPDATE_COMMAND_UI(ID_AXIS_XYZ, OnUpdateAxisXYZ)
	ON_COMMAND(ID_LIGHT_SHADING_FLAT, OnLightShadingFlat)
	ON_UPDATE_COMMAND_UI(ID_LIGHT_SHADING_FLAT, OnUpdateLightShadingFlat)
	ON_COMMAND(ID_LIGHT_SHADING_GOURAUD, OnLightShadingGouraud)
	ON_UPDATE_COMMAND_UI(ID_LIGHT_SHADING_GOURAUD, OnUpdateLightShadingGouraud)
	ON_COMMAND(ID_LIGHT_CONSTANTS, OnLightConstants)
	ON_COMMAND(ID_CALC_P_NORMALS, OnShowCalcPolyNormals)
	ON_UPDATE_COMMAND_UI(ID_CALC_P_NORMALS, OnUpdateShowCalcPolyNormals)
	ON_COMMAND(ID_CALC_V_NORMALS, OnShowCalcVertNormals)
	ON_UPDATE_COMMAND_UI(ID_CALC_V_NORMALS, OnUpdateShowCalcVertNormals)
	ON_COMMAND(ID_IRIT_P_NORMALS, OnShowIritPolyNormals)
	ON_UPDATE_COMMAND_UI(ID_IRIT_P_NORMALS, OnUpdateShowIritPolyNormals)
	ON_COMMAND(ID_IRIT_V_NORMALS, OnShowIritVertNormals)
	ON_UPDATE_COMMAND_UI(ID_IRIT_V_NORMALS, OnUpdateShowIritVertNormals)
	ON_COMMAND(ID_BBOX, OnShowBBox)
	ON_UPDATE_COMMAND_UI(ID_BBOX, OnUpdateShowBBox)
	ON_COMMAND(ID_NORMAL_COLOR, OnNormalsColor)
	ON_UPDATE_COMMAND_UI(ID_NORMAL_COLOR, OnUpdateNormalsColor)
	ON_COMMAND(ID_WIREFRAME_COLOR, OnWireframeColor)
	ON_UPDATE_COMMAND_UI(ID_WIREFRAME_COLOR, OnUpdateWireframeColor)
	ON_COMMAND(ID_BG_COLOR, OnBgColor)
	ON_COMMAND(ID_TRANS_SPACE, OnTransformationSpace)
	ON_UPDATE_COMMAND_UI(ID_TRANS_SPACE, OnUpdateTransformationSpace)
	ON_COMMAND(ID_OPTIONS_PERSPECTIVECONTROL, OnViewAngle)
	ON_COMMAND(ID_OPTIONS_MOUSESENSITIVITY, OnSensitivity)
	ON_COMMAND(ID_OPTIONS_TESSELLATION, OnTessellation)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_COMMAND(ID_RENDER_TOFILE, OnFileRender)
	ON_UPDATE_COMMAND_UI(ID_RENDER_TOFILE, OnUpdateFileRender)
	ON_COMMAND(ID_RENDER_SETFILEDIM, OnFileSetDimension)
	ON_UPDATE_COMMAND_UI(ID_TRANS_SPACE, OnUpdateFileSetDimension)
	ON_COMMAND(ID_OPTIONS_UPLOADBGIMAGE, OnBgPicture)
	ON_COMMAND(ID_OPTIONS_BGMODESOLID, OnBgSolid)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_BGMODESOLID, OnUpdateBgSolid)
	ON_COMMAND(ID_OPTIONS_BGMODESTREACHED, OnBgStrech)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_BGMODESTREACHED, OnUpdateBgStrech)
	ON_COMMAND(ID_OPTIONS_BGMODEREPEATED, OnBgRepeat)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_BGMODEREPEATED, OnUpdateBgRepeat)
	ON_COMMAND(ID_RENDER_SOLID, OnShadeSolid)
	ON_UPDATE_COMMAND_UI(ID_RENDER_SOLID, OnUpdateShadeSolid)
	ON_COMMAND(ID_RENDER_GOUROUD, OnShadeGouroud)
	ON_UPDATE_COMMAND_UI(ID_RENDER_GOUROUD, OnUpdateShadeGouroud)
	ON_COMMAND(ID_RENDER_PHONG, OnShadePhong)
	ON_UPDATE_COMMAND_UI(ID_RENDER_PHONG, OnUpdateShadePhong)
	ON_COMMAND(ID_RENDER_NOSHADE, OnShadeNone)
	ON_UPDATE_COMMAND_UI(ID_RENDER_NOSHADE, OnUpdateShadeNone)
	ON_COMMAND(ID_RENDER_WIREFRAME, OnRenderWireFrame)
	ON_UPDATE_COMMAND_UI(ID_RENDER_WIREFRAME, OnUpdateWireFrame)
	ON_COMMAND(ID_RENDER_SILOHETTE, OnRenderSilohette)
	ON_UPDATE_COMMAND_UI(ID_RENDER_SILOHETTE, OnUpdateSilohette)
	ON_UPDATE_COMMAND_UI(ID_RENDER_NOSHADE, OnUpdateShadeNone)
	ON_COMMAND(ID_RENDER_FLIPNORMAL, OnFlipNoramls)
	ON_UPDATE_COMMAND_UI(ID_RENDER_FLIPNORMAL, OnUpdateFlipNoramls)
	ON_COMMAND(ID_RENDER_BFCULL, OnBackFaceCull)
	ON_UPDATE_COMMAND_UI(ID_RENDER_BFCULL, OnUpdateBackFaceCull)
	ON_COMMAND(ID_RENDER_DYAMIC, OnRenderDynamic)
	ON_UPDATE_COMMAND_UI(ID_RENDER_DYAMIC, OnUpdateRenderDynamic)
	//}}AFX_MSG_MAP
	ON_WM_TIMER()
END_MESSAGE_MAP()

// A patch to fix GLaux disappearance from VS2005 to VS2008
void auxSolidCone(GLdouble radius, GLdouble height) {
	GLUquadric* quad = gluNewQuadric();
	gluQuadricDrawStyle(quad, GLU_FILL);
	gluCylinder(quad, radius, 0.0, height, 20, 20);
	gluDeleteQuadric(quad);
}

/////////////////////////////////////////////////////////////////////////////
// CCGWorkView construction/destruction

CCGWorkView::CCGWorkView()
{
	// Set default values
	m_nAxis = ID_AXIS_X;
	m_nAction = ID_ACTION_ROTATE;
	m_nView = ID_VIEW_ORTHOGRAPHIC;
	m_bIsPerspective = false;
	m_sensitivity = 1.0;
	m_nLightShading = ID_LIGHT_SHADING_FLAT;
	m_tSpace = ID_CAMERA_SPACE;
	m_lMaterialAmbient = 0.2;
	m_lMaterialDiffuse = 0.8;
	m_lMaterialSpecular = 1.0;
	m_nMaterialCosineFactor = 32;
	m_depth = 5;
	//init the first light to be enabled
	m_lights[LIGHT_ID_1].enabled = true;
	m_ambientLight.Ipower = 0.15;
	m_pDbBitMap = NULL;
	m_pDbDC = NULL;
	m_PngHeight = 0;
	m_PngWidth = 0;
	m_SaveToFile = false;
	m_sceneSpecExp = 0;
}

CCGWorkView::~CCGWorkView()
{
}


/////////////////////////////////////////////////////////////////////////////
// CCGWorkView diagnostics

#ifdef _DEBUG
void CCGWorkView::AssertValid() const
{
	CView::AssertValid();
}

void CCGWorkView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CCGWorkDoc* CCGWorkView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CCGWorkDoc)));
	return (CCGWorkDoc*)m_pDocument;
}
#endif //_DEBUG


/////////////////////////////////////////////////////////////////////////////
// CCGWorkView Window Creation - Linkage of windows to CGWork

BOOL CCGWorkView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	// An CGWork window must be created with the following
	// flags and must NOT include CS_PARENTDC for the
	// class style.

	cs.style |= WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

	return CView::PreCreateWindow(cs);
}



int CCGWorkView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	InitializeCGWork();
	if (AllocConsole())
	{
		FILE* fp;
		freopen_s(&fp, "CONOUT$", "w", stdout);
		freopen_s(&fp, "CONOUT$", "w", stderr);
		std::cout.clear();
		std::clog.clear();
		std::cerr.clear();
		std::cin.clear();
	}

	return 0;
}


// This method initialized the CGWork system.
BOOL CCGWorkView::InitializeCGWork()
{
	m_pDC = new CClientDC(this);

	if (NULL == m_pDC) { // failure to get DC
		::AfxMessageBox(CString("Couldn't get a valid DC."));
		return FALSE;
	}

	CRect r;
	GetClientRect(&r);
	m_pDbDC = new CDC();
	m_pDbDC->CreateCompatibleDC(m_pDC);
	SetTimer(1, 1, NULL);
	m_pDbBitMap = CreateCompatibleBitmap(m_pDC->m_hDC, r.right, r.bottom);
	m_pDbDC->SelectObject(m_pDbBitMap);
	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CCGWorkView message handlers


void CCGWorkView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	if (0 >= cx || 0 >= cy) {
		return;
	}

	// save the width and height of the current window
	m_WindowWidth = cx;
	m_WindowHeight = cy;

	// compute the aspect ratio
	// this will keep all dimension scales equal
	m_AspectRatio = (GLdouble)m_WindowWidth / (GLdouble)m_WindowHeight;

	CRect r;
	GetClientRect(&r);
	DeleteObject(m_pDbBitMap);
	m_pDbBitMap = CreateCompatibleBitmap(m_pDC->m_hDC, r.right, r.bottom);
	m_pDbDC->SelectObject(m_pDbBitMap);
}


BOOL CCGWorkView::SetupViewingFrustum(void)
{
	return TRUE;
}


// This viewing projection gives us a constant aspect ration. This is done by
// increasing the corresponding size of the ortho cube.
BOOL CCGWorkView::SetupViewingOrthoConstAspect(void)
{
	return TRUE;
}


BOOL CCGWorkView::OnEraseBkgnd(CDC* pDC)
{
	// Windows will clear the window with the background color every time your window 
	// is redrawn, and then CGWork will clear the viewport with its own background color.


	return true;
}


// The coordinating need to be in screen view

#include <cstdint>

// Function to convert RGBA to BGRA
uint32_t RGBAtoBGRA(uint32_t rgba) {
	uint8_t r = (rgba >> 24) & 0xFF; // Extract Red
	uint8_t g = (rgba >> 16) & 0xFF; // Extract Green
	uint8_t b = (rgba >> 8) & 0xFF; // Extract Blue
	uint8_t a = rgba & 0xFF;         // Extract Alpha

	// Pack it as BGRA
	uint32_t bgra = (b << 24) | (g << 16) | (r << 8) | a;
	return bgra;
}


/////////////////////////////////////////////////////////////////////////////
// CCGWorkView drawing
/////////////////////////////////////////////////////////////////////////////

void CCGWorkView::OnDraw(CDC* pDC)
{
	if (m_rendermode.getRenderToPNGFlag()){
		int pngW = m_rendermode.m_toPngRenderWidth ? m_rendermode.m_toPngRenderWidth : m_WindowWidth;
		int pngH = m_rendermode.m_toPngRenderHeight ? m_rendermode.m_toPngRenderHeight : m_WindowHeight;
		m_scene.executeCommand(&createRenderingCommand(pngW, pngH));
		uint32_t* pngBuffer = m_scene.getBuffer();
		PngWrapper png = PngWrapper("renderImage.png", pngW, pngH);
		png.WriteFromBuffer(pngBuffer);
		m_rendermode.setRenderToPNGFlag();
	}
	// Create a bitmap info header
	m_scene.executeCommand(&createRenderingCommand(m_WindowWidth, m_WindowHeight));
	uint32_t* screenBuffer = m_scene.getBuffer();
	BITMAPINFO bmi;
	ZeroMemory(&bmi, sizeof(BITMAPINFO));
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = m_WindowWidth;
	bmi.bmiHeader.biHeight = -m_WindowHeight; // Negative height to indicate a top-down DIB
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biCompression = BI_RGB;

	CDC memDC;
	memDC.CreateCompatibleDC(pDC);
	CBitmap bitmap;
	bitmap.CreateCompatibleBitmap(pDC, m_WindowWidth, m_WindowHeight);

	// Select the bitmap into the memory DC
	CBitmap* pOldBitmap = memDC.SelectObject(&bitmap);



	// Set the bitmap bits from the array
	SetDIBits(memDC, bitmap, 0, m_WindowHeight, screenBuffer, &bmi, DIB_RGB_COLORS);

	// BitBlt the bitmap to the screen DC
	pDC->BitBlt(0, 0, m_WindowWidth, m_WindowHeight, &memDC, 0, 0, SRCCOPY);

	// Clean up
	memDC.SelectObject(pOldBitmap);
}



/////////////////////////////////////////////////////////////////////////////
// CCGWorkView CGWork Finishing and clearing...

void CCGWorkView::OnDestroy()
{
	CView::OnDestroy();

	// delete the DC
	if (m_pDC) {
		delete m_pDC;
	}

	if (m_pDbDC) {
		delete m_pDbDC;
	}
}



/////////////////////////////////////////////////////////////////////////////
// User Defined Functions

void CCGWorkView::RenderScene() {
	// do nothing. This is supposed to be overriden...

	return;
}


void CCGWorkView::OnFileLoad()
{
	TCHAR szFilters[] = _T("IRIT Data Files (*.itd)|*.itd|All Files (*.*)|*.*||");

	CFileDialog dlg(TRUE, _T("itd"), _T("*.itd"), OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, szFilters);

	if (dlg.DoModal() == IDOK) {
		m_strItdFileName = dlg.GetPathName();		// Full path and filename
		PngWrapper p;
		std::vector<Model*> container;
		if(CGSkelProcessIritDataFilesToContainer(m_strItdFileName, 1, container)){
			m_scene.addModels(container);		
		}

		Invalidate();	// force a WM_PAINT for drawing.
	}

}





// VIEW HANDLERS ///////////////////////////////////////////

// Note: that all the following Message Handlers act in a similar way.
// Each control or command has two functions associated with it.

void CCGWorkView::OnViewOrthographic()
{
	m_nView = ID_VIEW_ORTHOGRAPHIC;
	m_bIsPerspective = false;
	m_scene.setCamera(CAMERA_TYPE::ORTHOGONAL);
	Invalidate();		// redraw using the new view.
}

void CCGWorkView::OnUpdateViewOrthographic(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_nView == ID_VIEW_ORTHOGRAPHIC);
}

void CCGWorkView::OnViewPerspective()
{
	m_nView = ID_VIEW_PERSPECTIVE;
	m_bIsPerspective = true;
	m_scene.setCamera(CAMERA_TYPE::PRESPECTIVE);
	Invalidate();
}

void CCGWorkView::OnUpdateViewPerspective(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_nView == ID_VIEW_PERSPECTIVE);
}

// ACTION HANDLERS ///////////////////////////////////////////

void CCGWorkView::OnActionRotate()
{
	m_nAction = ID_ACTION_ROTATE;
	m_nAxis = (m_nAxis == ID_AXIS_XY || m_nAxis == ID_AXIS_XYZ) ? ID_AXIS_X : m_nAxis;
}

void CCGWorkView::OnUpdateActionRotate(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_nAction == ID_ACTION_ROTATE);
}

void CCGWorkView::OnActionTranslate()
{
	m_nAction = ID_ACTION_TRANSLATE;
	m_nAxis = (m_nAxis == ID_AXIS_XYZ) ? ID_AXIS_X : m_nAxis;
}

void CCGWorkView::OnUpdateActionTranslate(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_nAction == ID_ACTION_TRANSLATE);
}

void CCGWorkView::OnActionScale()
{
	m_nAction = ID_ACTION_SCALE;
}

void CCGWorkView::OnUpdateActionScale(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_nAction == ID_ACTION_SCALE);
}

// AXIS HANDLERS ///////////////////////////////////////////


// Gets calles when the X button is pressed or when the Axis->X menu is selected.
// The only thing we do here is set the ChildView member variable m_nAxis to the 
// selected axis.
void CCGWorkView::OnAxisX()
{
	m_nAxis = ID_AXIS_X;
	
}

// Gets called when windows has to repaint either the X button or the Axis pop up menu.
// The control is responsible for its redrawing.
// It sets itself disabled when the action is a Scale action.
// It sets itself Checked if the current axis is the X axis.
void CCGWorkView::OnUpdateAxisX(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_nAxis == ID_AXIS_X);
}


void CCGWorkView::OnAxisY()
{
	m_nAxis = ID_AXIS_Y;
}

void CCGWorkView::OnUpdateAxisY(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_nAxis == ID_AXIS_Y);
}


void CCGWorkView::OnAxisZ()
{
	m_nAxis = ID_AXIS_Z;
}

void CCGWorkView::OnUpdateAxisZ(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_nAxis == ID_AXIS_Z);
}

void CCGWorkView::OnAxisXY()
{
	m_nAxis = ID_AXIS_XY;
}

void CCGWorkView::OnUpdateAxisXY(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_nAction != ID_ACTION_ROTATE);
	pCmdUI->SetCheck(m_nAxis == ID_AXIS_XY);
}
void CCGWorkView::OnAxisXYZ()
{
	m_nAxis = ID_AXIS_XYZ;
}

void CCGWorkView::OnUpdateAxisXYZ(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_nAction == ID_ACTION_SCALE);
	pCmdUI->SetCheck(m_nAxis == ID_AXIS_XYZ);
}

void CCGWorkView::OnShowCalcPolyNormals() {
	m_rendermode.setRenderPolygonsCalcNormalFlag();
	Invalidate();
}
void CCGWorkView::OnUpdateShowCalcPolyNormals(CCmdUI* pCmdUI) {
	pCmdUI->SetCheck(m_rendermode.getPolygonsShowCNormalFlag());

}
void CCGWorkView::OnShowCalcVertNormals() {
	m_rendermode.setRenderCalcVertivesNormalFlag();
	Invalidate();
}
void CCGWorkView::OnUpdateShowCalcVertNormals(CCmdUI* pCmdUI) {
	pCmdUI->SetCheck(m_rendermode.getVertexShowCNormalFlag());

}
void CCGWorkView::OnShowIritPolyNormals() {
	m_rendermode.setRenderPolygonsNormalFromDataFlag();
	
	Invalidate();
}
void CCGWorkView::OnUpdateShowIritPolyNormals(CCmdUI* pCmdUI) {
	pCmdUI->SetCheck(m_rendermode.getPolygonsShowDNormalFlag());

}
void CCGWorkView::OnShowIritVertNormals() {
	m_rendermode.setRenderDataVertivesNormalFlag();
	Invalidate();
}
void CCGWorkView::OnUpdateShowIritVertNormals(CCmdUI* pCmdUI) {
	pCmdUI->SetCheck(m_rendermode.getVertexShowDNormalFlag());

}
void CCGWorkView::OnShowBBox(){
	m_rendermode.setRenderObjBboxFlag();
	Invalidate();
}
void CCGWorkView::OnUpdateShowBBox(CCmdUI* pCmdUI) {
	pCmdUI->SetCheck(m_rendermode.getRenderObjBboxFlag());
}
void CCGWorkView::OnNormalsColor() {
	if (!m_rendermode.getRenderOverrideNormalColorFlag()) {
		// Create a color dialog with the initial color set to white
		CColorDialog colorDlg(RGB(255, 255, 255), CC_FULLOPEN | CC_RGBINIT);

		// Display the color dialog
		if (colorDlg.DoModal() == IDOK)
		{
			// Get the selected color
			COLORREF color = colorDlg.GetColor();
			ColorGC gc_color(GetRValue(color), GetGValue(color), GetBValue(color));
			m_rendermode.setNormalColor(gc_color);
		}
	}
	m_rendermode.setRenderOverrideNormalColorFlag();
	Invalidate();
}
void CCGWorkView::OnUpdateNormalsColor(CCmdUI* pCmdUI) {
	pCmdUI->SetCheck(m_rendermode.getRenderOverrideNormalColorFlag());
}
void CCGWorkView::OnWireframeColor() {
	// Create a color dialog with the initial color set to white
	if (!m_rendermode.getRenderOverrideWireColorFlag()) {
		CColorDialog colorDlg(RGB(255, 255, 255), CC_FULLOPEN | CC_RGBINIT);

		// Display the color dialog
		if (colorDlg.DoModal() == IDOK)
		{
			// Get the selected color
			COLORREF color = colorDlg.GetColor();
			ColorGC gc_color(GetRValue(color), GetGValue(color), GetBValue(color));
			m_rendermode.setWireColor(gc_color);
		}
	}
	m_rendermode.setRenderOverrideWireColorFlag();
	Invalidate();
}
void CCGWorkView::OnUpdateWireframeColor(CCmdUI* pCmdUI) {
	pCmdUI->SetCheck(m_rendermode.getRenderOverrideWireColorFlag());
}
void CCGWorkView::OnBgColor() {
	// Create a color dialog with the initial color set to white
	CColorDialog colorDlg(RGB(255, 255, 255), CC_FULLOPEN | CC_RGBINIT);

	// Display the color dialog
	if (colorDlg.DoModal() == IDOK)
	{
		// Get the selected color
		COLORREF color = colorDlg.GetColor();
		ColorGC gc_color(GetRValue(color), GetGValue(color), GetBValue(color));
		m_rendermode.setBGColor(gc_color);
	}
	m_rendermode.setRenderBGSolidFlag();
	Invalidate();
}
void CCGWorkView::OnBgPicture() {
	TCHAR szFilters[] = _T("PNG Files (*.png)|*.png|All Files (*.*)|*.*||");

	CFileDialog dlg(TRUE, _T("png"), _T("*.png"), OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, szFilters);

	if (dlg.DoModal() == IDOK) {
		m_rendermode.setBGPngPath(CStringA(dlg.GetPathName()));  // Full path and filename
		Invalidate();  
	}
}

void CCGWorkView::OnBgSolid() {
	m_rendermode.setRenderBGSolidFlag();
}
void CCGWorkView::OnUpdateBgSolid(CCmdUI* pCmdUI) {
	pCmdUI->SetCheck(m_rendermode.getRenderBGSolidFlag());
}
void CCGWorkView::OnBgStrech() {
	m_rendermode.setRenderBGStreachedFlag();
}
void CCGWorkView::OnUpdateBgStrech(CCmdUI* pCmdUI) {
	pCmdUI->Enable(m_rendermode.getHasBGPngPath());
	pCmdUI->SetCheck(m_rendermode.getRenderBGStreachedFlag());
}
void CCGWorkView::OnBgRepeat() {
	m_rendermode.setRenderBGRepeatFlag();
}
void CCGWorkView::OnUpdateBgRepeat(CCmdUI* pCmdUI) {
	pCmdUI->Enable(m_rendermode.getHasBGPngPath());
	pCmdUI->SetCheck(m_rendermode.getRenderBGRepeatFlag());
}

void CCGWorkView::OnTransformationSpace() {
	m_tSpace = m_tSpace == ID_OBJECT_SPACE ? ID_CAMERA_SPACE : ID_OBJECT_SPACE;
}
void CCGWorkView::OnUpdateTransformationSpace(CCmdUI* pCmdUI) {
	pCmdUI->SetCheck(m_tSpace == ID_OBJECT_SPACE);
}
void CCGWorkView::OnViewAngle() {
	float min = 0.1;
	float max = 20;
	float tickWidth = 0.01;
	CDynamicSliderDialog dlg("Set d(far) values", min, max, tickWidth);
	if (dlg.DoModal() == IDOK)
	{
		m_depth = dlg.GetSliderPos();
		this->m_scene.getPerspectiveCamera()->setFar(m_depth);
	}
}
void CCGWorkView::OnSensitivity() {
	float min = 0.1;
	float max = 10.0;
	float tickWidth = 0.01;
	CDynamicSliderDialog dlg("Set Mouse Sensitivity", min, max, tickWidth);
	if (dlg.DoModal() == IDOK)
	{
		m_sensitivity = dlg.GetSliderPos();
	}
}
void CCGWorkView::OnTessellation() {
	float min = 2;
	float max = 100;
	float tickWidth = 1;
	CDynamicSliderDialog dlg("Set Polygon Tessellation", min, max, tickWidth);
	if (dlg.DoModal() == IDOK)
	{
		CGSkelSetTes((int)dlg.GetSliderPos());
	}
}

// OPTIONS HANDLERS ///////////////////////////////////////////




// LIGHT SHADING HANDLERS ///////////////////////////////////////////

void CCGWorkView::OnLightShadingFlat()
{
	m_nLightShading = ID_LIGHT_SHADING_FLAT;
}

void CCGWorkView::OnUpdateLightShadingFlat(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_nLightShading == ID_LIGHT_SHADING_FLAT);
}


void CCGWorkView::OnLightShadingGouraud()
{
	m_nLightShading = ID_LIGHT_SHADING_GOURAUD;
}

void CCGWorkView::OnUpdateLightShadingGouraud(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_nLightShading == ID_LIGHT_SHADING_GOURAUD);
}

// LIGHT SETUP HANDLER ///////////////////////////////////////////

void CCGWorkView::OnLightConstants()
{
	CLightDialog dlg;

	for (int id = LIGHT_ID_1; id < MAX_LIGHT; id++)
	{
		dlg.SetDialogData((LightID)id, m_lights[id]);
	}
	dlg.SetDialogData(LIGHT_ID_AMBIENT, m_ambientLight);
	dlg.SetDialogExpData(m_sceneSpecExp);

	if (dlg.DoModal() == IDOK)
	{
		for (int id = LIGHT_ID_1; id < MAX_LIGHT; id++)
		{
			m_lights[id] = dlg.GetDialogData((LightID)id);
		}
		m_ambientLight = dlg.GetDialogData(LIGHT_ID_AMBIENT);
		m_sceneSpecExp = dlg.GetDialogExpData();
	}
	m_scene.invalidateLighting(m_lights, m_ambientLight, m_sceneSpecExp);
	Invalidate();
}

void CCGWorkView::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	//CView::OnTimer(nIDEvent);
	//if (nIDEvent == 1)
	//	Invalidate();
}

RenderCommand CCGWorkView::createRenderingCommand(int width, int height) {
	return RenderCommand(width, height, m_rendermode);
}
TransformationCommand CCGWorkView::createTransformationCommand(const Vector3& point) {
	return TransformationCommand(m_WindowWidth, m_WindowHeight,
		point.x - m_ref_point.x,
		point.y - m_ref_point.y,
		m_AspectRatio, m_nAction, m_nAxis,
		m_tSpace, m_sensitivity, m_bIsPerspective ? m_depth : 1);
}
void CCGWorkView::OnLButtonDown(UINT nFlags, CPoint point) {
	// Handle the left button down event here
	m_bLeftButtonDown = true;
	m_ref_point = point;
	CView::OnLButtonDown(nFlags, point);
}

void CCGWorkView::OnLButtonUp(UINT nFlags, CPoint point) {
	// Handle the left button up event here
	m_bLeftButtonDown = false;
	if (m_rendermode.getRenderKeyFrames())
		m_rendermode.setRenderAddKeyFrame();
	CView::OnLButtonUp(nFlags, point);
}

void CCGWorkView::OnMouseMove(UINT nFlags, CPoint point) {
	// Handle the left button move event here
	if (m_bLeftButtonDown == true) {
		//build commad object
		m_scene.executeCommand(&createTransformationCommand(Vector3(point.x,point.y,0)));
		m_ref_point = point;
	}
	Invalidate();
	CView::OnMouseMove(nFlags, point);
}

void CCGWorkView::OnFileRender() {
	m_rendermode.setRenderToPNGFlag();
}
void CCGWorkView::OnUpdateFileRender(CCmdUI* pCmdUI) {
	pCmdUI->SetCheck(m_rendermode.getRenderToPNGFlag());
}
void CCGWorkView::OnFileSetDimension() {
		// Create an instance of the dialog
		DimensionDialog dlg;
		dlg.SetDimensions(m_rendermode.m_toPngRenderWidth, m_rendermode.m_toPngRenderHeight);
		// Display the dialog
		if (dlg.DoModal() == IDOK) {
			// Retrieve the dimensions entered by the user
			m_rendermode.m_toPngRenderWidth = dlg.getWidth();
			m_rendermode.m_toPngRenderHeight = dlg.getHeight();
		}
}
void CCGWorkView::OnUpdateFileSetDimension(CCmdUI* pCmdUI) {
}

void CCGWorkView::OnShadeSolid() {
	m_rendermode.setRenderShadeSolidFlag();
	Invalidate();
}
void CCGWorkView::OnUpdateShadeSolid(CCmdUI* pCmdUI) {
	pCmdUI->SetCheck(m_rendermode.getRenderShadeSolidFlag());
}
void CCGWorkView::OnShadeNone() {
	m_rendermode.setRenderShadeNoneFlag();
	Invalidate();
}
void CCGWorkView::OnUpdateShadeNone(CCmdUI* pCmdUI) {
	pCmdUI->SetCheck(m_rendermode.getRenderShadeNoneFlag());
}
void CCGWorkView::OnShadeGouroud() {
	m_rendermode.setRenderShadeGouroudFlag();
	Invalidate();
}
void CCGWorkView::OnUpdateShadeGouroud(CCmdUI* pCmdUI) {
	pCmdUI->SetCheck(m_rendermode.getRenderShadeGouroudFlag());
}
void CCGWorkView::OnShadePhong() {
	m_rendermode.setRenderShadePhongFlag();
	Invalidate();
}
void CCGWorkView::OnUpdateShadePhong(CCmdUI* pCmdUI) {
	pCmdUI->SetCheck(m_rendermode.getRenderShadePhongFlag());
}
void CCGWorkView::OnRenderWireFrame() {
	m_rendermode.setWireframeFlag();
	Invalidate();
}
void CCGWorkView::OnUpdateWireFrame(CCmdUI* pCmdUI) {
	pCmdUI->SetCheck(m_rendermode.getWireFrameFlag());
}
void CCGWorkView::OnRenderSilohette() {
	m_rendermode.setSilohetteFlag();
	Invalidate();
}
void CCGWorkView::OnUpdateSilohette(CCmdUI* pCmdUI) {
	pCmdUI->SetCheck(m_rendermode.getSilohetteFlag());
}
void CCGWorkView::OnFlipNoramls() {
	m_rendermode.setRenderWithFlipedNormalsFlag();
	Invalidate();
}
void CCGWorkView::OnUpdateFlipNoramls(CCmdUI* pCmdUI) {
	pCmdUI->SetCheck(m_rendermode.getRenderWithFlipedNormalsFlag());
}
void CCGWorkView::OnBackFaceCull() {
	m_rendermode.setRenderCulledFlag();
	Invalidate();
}
void CCGWorkView::OnUpdateBackFaceCull(CCmdUI* pCmdUI) {
	pCmdUI->SetCheck(m_rendermode.getRenderCulledFlag());
}
void CCGWorkView::OnRenderDynamic() {
	m_rendermode.setRenderDynemic();
	Invalidate();
}
void CCGWorkView::OnUpdateRenderDynamic(CCmdUI* pCmdUI) {
	pCmdUI->SetCheck(!m_rendermode.getRenderDynemic());
}
