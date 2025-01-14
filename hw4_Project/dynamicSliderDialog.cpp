#include "stdafx.h"
#include "CGWork.h"
#include "DynamicSliderDialog.h"

IMPLEMENT_DYNAMIC(CDynamicSliderDialog, CDialog)

CDynamicSliderDialog::CDynamicSliderDialog(CString title, float min, float max, float tickWidth, CWnd* pParent /*=nullptr*/)
    : CDialog(IDD_DIALOG1, pParent), m_title(title), m_min(min), m_max(max), m_tickWidth(tickWidth), m_sliderPos(static_cast<int>((min + max) / 2 / tickWidth))
{
}

CDynamicSliderDialog::~CDynamicSliderDialog()
{
}

void CDynamicSliderDialog::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_SLIDER1, m_sliderCtrl);
    DDX_Control(pDX, IDC_STATIC1, m_staticMin);
    DDX_Control(pDX, IDC_STATIC2, m_staticMax);
}

BEGIN_MESSAGE_MAP(CDynamicSliderDialog, CDialog)
END_MESSAGE_MAP()

BOOL CDynamicSliderDialog::OnInitDialog()
{
    CDialog::OnInitDialog();

    // Initialize the slider control
    SetWindowText(m_title);
    int rangeMin = static_cast<int>(m_min / m_tickWidth);
    int rangeMax = static_cast<int>(m_max / m_tickWidth);
    m_sliderCtrl.SetRange(rangeMin, rangeMax);
    m_sliderCtrl.SetTicFreq(1);
    m_sliderCtrl.SetPos((rangeMax + rangeMin) / 2); // Set initial position to the middle of the range

    // Update static text controls
    CString strMin, strMax;
    strMin.Format(_T("%.1f"), m_min);
    strMax.Format(_T("%.1f"), m_max);
    m_staticMin.SetWindowText(strMin);
    m_staticMax.SetWindowText(strMax);

    return TRUE;  // return TRUE unless you set the focus to a control
}

void CDynamicSliderDialog::OnOK()
{
    // Ensure the slider control is valid before calling GetPos
    if (m_sliderCtrl.GetSafeHwnd() != NULL) {
        m_sliderPos = m_sliderCtrl.GetPos();
    }
    else {
        AfxMessageBox(_T("Slider control is not valid."));
    }

    // Call the base class implementation
    CDialog::OnOK();
}


IMPLEMENT_DYNAMIC(DimensionDialog, CDialog)

void DimensionDialog::DoDataExchange(CDataExchange* pDX) {
    CDialog::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDIT1, m_width);
    DDX_Text(pDX, IDC_EDIT2, m_height);
}// DDX/DDV support

BEGIN_MESSAGE_MAP(DimensionDialog, CDialog)
END_MESSAGE_MAP()

void DimensionDialog::SetDimensions(int width, int height) {
    m_width = width;
    m_height = height;
}
int DimensionDialog::getWidth() const {
    return m_width;
}
int DimensionDialog::getHeight() const {
    return m_height;
}
