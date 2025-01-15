#pragma once

#include "model.h"
#include <vector>
#include "Scene.h"
#include <string>
// CObjectDialog dialog

class CObjectDialog : public CDialog
{
    DECLARE_DYNAMIC(CObjectDialog)

public:
    CObjectDialog(CWnd* pParent = NULL);   // standard constructor
    virtual ~CObjectDialog();

    // Dialog interface
    void SetDialogData(std::vector<std::pair<std::string, int>>& table);
    void UpdateObjectList(std::vector<Model*> newObjects);
    std::vector<std::pair<std::string, int>> GetDialogData();

    // Dialog Data
    enum { IDD = IDD_OBJECTS_DLG };

protected:
    std::vector<std::pair<std::string, int>> m_table; // List of objects in the scene
    int m_currentObjectIdx;             // Current selected object index
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    DECLARE_MESSAGE_MAP()

public:
    afx_msg void OnBnClickedRadioObject();
    virtual BOOL OnInitDialog();
};

