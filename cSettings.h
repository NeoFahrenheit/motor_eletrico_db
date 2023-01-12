#pragma once
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#	pragma hdrstop
#endif

#ifndef WX_PRECOMP
#	include "wx/wx.h"
#endif

#include "wx/notebook.h"
#include "sqlite/sqlite3.h"
#include "cMain.h"
#include "cDataBase.h"

class cSettings : public wxDialog
{
public:
    cSettings(wxWindow* parent, cDataBase* db);
    ~cSettings();

private:
    cDataBase* m_DB{};
    wxTextEntryDialog* m_TextDialog{};

    void OnAddMarca(wxCommandEvent& event);
    void OnEditMarca(wxCommandEvent& event);
    void OnDeleteMarca(wxCommandEvent& event);

    void TellSucessAndUpdateCombo(wxString msg);
    void GetListOfImagesPath(wxString column, wxString value, std::vector<fs::path>& outList);

    wxNotebook* m_Notebook{};
    wxWindow* m_Parent{};

    wxTextCtrl* m_MarcaAddCtrl{};
    wxButton* m_MarcaAddBtn{};
    wxComboBox* m_MarcaCombo{};
    wxButton* m_EditBtn{};
    wxButton* m_DeleteBtn{};

    wxPanel* GetGerenciar();
    wxPanel* GetAparencia();

    void InitUI();

};