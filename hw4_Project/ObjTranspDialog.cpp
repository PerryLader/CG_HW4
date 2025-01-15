#include "stdafx.h"
#include "CGWork.h"
#include "ObjTranspDialog.h"


// CObjectDialog dialog

IMPLEMENT_DYNAMIC(CObjectDialog, CDialog)

CObjectDialog::CObjectDialog(CWnd* pParent /*=NULL*/)
    : CDialog(CObjectDialog::IDD, pParent)
{
    m_currentObjectIdx = 0;
}

CObjectDialog::~CObjectDialog()
{
}

void CObjectDialog::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    
    // Update object alpha values and names dynamically
    int x;
    if (m_currentObjectIdx >= m_table.size())
    {
        SetDlgItemText(IDC_OBJECT_NAME, CString("NUN"));
        DDX_Slider(pDX, IDC_OBJECT_ALPHA, x);
    }
    else
    {
        SetDlgItemText(IDC_OBJECT_NAME, CString(m_table[m_currentObjectIdx].first.c_str()));
        DDX_Slider(pDX, IDC_OBJECT_ALPHA, m_table[m_currentObjectIdx].second);
    }
    
}

BEGIN_MESSAGE_MAP(CObjectDialog, CDialog)
    ON_BN_CLICKED(IDC_RADIO_OBJECT1, &CObjectDialog::OnBnClickedRadioObject)
    ON_BN_CLICKED(IDC_RADIO_OBJECT2, &CObjectDialog::OnBnClickedRadioObject)
    ON_BN_CLICKED(IDC_RADIO_OBJECT3, &CObjectDialog::OnBnClickedRadioObject)
    ON_BN_CLICKED(IDC_RADIO_OBJECT4, &CObjectDialog::OnBnClickedRadioObject)
    ON_BN_CLICKED(IDC_RADIO_OBJECT5, &CObjectDialog::OnBnClickedRadioObject)
    ON_BN_CLICKED(IDC_RADIO_OBJECT6, &CObjectDialog::OnBnClickedRadioObject)
    ON_BN_CLICKED(IDC_RADIO_OBJECT7, &CObjectDialog::OnBnClickedRadioObject)
    ON_BN_CLICKED(IDC_RADIO_OBJECT8, &CObjectDialog::OnBnClickedRadioObject)
    ON_BN_CLICKED(IDC_RADIO_OBJECT9, &CObjectDialog::OnBnClickedRadioObject)

    // Add more radio buttons here if needed
END_MESSAGE_MAP()

void CObjectDialog::SetDialogData(std::vector<std::pair<std::string, int>> &table)
{
    m_table = table;
}

void CObjectDialog::UpdateObjectList(std::vector<Model*> newObjects)
{
   // m_objects = newObjects;
    m_currentObjectIdx = 0; // Reset to the first object
    UpdateData(FALSE);
}

std::vector<std::pair<std::string, int>> CObjectDialog::GetDialogData()
{
    return m_table;
}

BOOL CObjectDialog::OnInitDialog()
{
    CDialog::OnInitDialog();

    // Initialize the slider control for alpha values (0-255)
    CSliderCtrl* pSlider = (CSliderCtrl*)GetDlgItem(IDC_OBJECT_ALPHA);
    if (pSlider)
    {
        pSlider->SetRange(0, 255);
        if (m_currentObjectIdx >= m_table.size())
        {
            pSlider->SetPos(255);

        }
        else
        {
            pSlider->SetPos(this->m_table[this->m_currentObjectIdx].second);

        }
    }

    // Set the radio button for the current object to be selected
    CheckRadioButton(IDC_RADIO_OBJECT1, IDC_RADIO_OBJECT9, m_currentObjectIdx + IDC_RADIO_OBJECT1);

    return TRUE;  // Return TRUE unless you set the focus to a control
}

void CObjectDialog::OnBnClickedRadioObject()
{
    // Save the dialog state into the data variables
    UpdateData(TRUE);

    // Get the newly selected object index from the radio buttons
    m_currentObjectIdx = GetCheckedRadioButton(IDC_RADIO_OBJECT1, IDC_RADIO_OBJECT9) - IDC_RADIO_OBJECT1;

    // Update all dialog fields according to the new object index
    UpdateData(FALSE);

    Invalidate();
}
