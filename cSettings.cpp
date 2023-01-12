#include "cSettings.h"

cSettings::cSettings(wxWindow* parent, cDataBase* db) : wxDialog(parent, wxID_ANY, _("Configurações"))
{
    m_Parent = parent;
    m_DB = db;

    InitUI();
    CenterOnScreen();

}

cSettings::~cSettings()
{

}

void cSettings::InitUI()
{
    auto* s_RootSizer = new wxBoxSizer(wxVERTICAL);

    m_Notebook = new wxNotebook(this, wxID_ANY);
    s_RootSizer->Add(m_Notebook, 1, wxEXPAND);

    m_Notebook->AddPage(GetGerenciar(), _("Gerenciar"));
    m_Notebook->AddPage(GetAparencia(), _("Aparência"));

    SetSizer(s_RootSizer);
}

wxPanel* cSettings::GetGerenciar()
{
    auto* panel = new wxPanel(m_Notebook, wxID_ANY);
    auto* rootSizer = new wxBoxSizer(wxVERTICAL);
    
    auto* marcaVerSizer = new wxStaticBoxSizer(wxVERTICAL, panel, _("Marca"));
    auto* marcaAddSizer = new wxBoxSizer(wxHORIZONTAL);
    auto* marcaBtnsSizer = new wxBoxSizer(wxHORIZONTAL);
    wxArrayString marcaChoices = m_DB->GetComboBoxValues("marca");

    m_MarcaAddCtrl = new wxTextCtrl(panel, wxID_ANY);
    m_MarcaAddBtn = new wxButton(panel, wxID_ANY, _("Adicionar"));
    m_MarcaCombo = new wxComboBox(panel, wxID_ANY, marcaChoices[0], wxDefaultPosition, wxSize(120, 23), marcaChoices, wxCB_READONLY | wxCB_SORT);
    m_EditBtn = new wxButton(panel, wxID_ANY, _("Editar"));
    m_DeleteBtn = new wxButton(panel, wxID_ANY, _("Deletar"));

    marcaAddSizer->Add(m_MarcaAddCtrl, 0, wxLEFT, 10);
    marcaAddSizer->Add(m_MarcaAddBtn, 0, wxLEFT, 10);
    marcaVerSizer->Add(marcaAddSizer, 0, wxBOTTOM, 20);
    marcaVerSizer->Add(m_MarcaCombo, 0, wxALIGN_CENTER | wxBOTTOM, 10);

    marcaBtnsSizer->Add(m_EditBtn, 0, wxALIGN_CENTER);
    marcaBtnsSizer->Add(m_DeleteBtn, 0, wxALIGN_CENTER | wxLEFT, 10);
    marcaVerSizer->Add(marcaBtnsSizer, 0, wxALIGN_CENTER);

    m_MarcaAddBtn->Bind(wxEVT_BUTTON, &cSettings::OnAddMarca, this, wxID_ANY);
    m_EditBtn->Bind(wxEVT_BUTTON, &cSettings::OnEditMarca, this, wxID_ANY);
    m_DeleteBtn->Bind(wxEVT_BUTTON, &cSettings::OnDeleteMarca, this, wxID_ANY);

    rootSizer->Add(marcaVerSizer, 0, wxALL, 5);
    panel->SetSizer(rootSizer);
    
    return panel;
}

wxPanel* cSettings::GetAparencia()
{
    auto* panel = new wxPanel(m_Notebook, wxID_ANY);
    
    return panel;
}

void cSettings::OnAddMarca(wxCommandEvent& event)
{
    wxString marca = m_MarcaAddCtrl->GetValue().Strip();
    if (marca.IsEmpty())
    {
        auto* dlg = new wxMessageDialog(this, _("O campo de texto está vazio. Por favor, digite algo."), _("Campo vazio"), wxICON_ERROR);
        dlg->ShowModal();
        return;
    }

    auto* dlg = new wxMessageDialog(this, wxString::Format(_("Você tem certeza que deseja adicionar a marca %s?"), marca),
        _("Confirmação de nome"), wxICON_INFORMATION | wxYES_NO);
    int answer = dlg->ShowModal();

    if (answer == wxID_YES)
    {
        sqlite3_stmt* stmt{};
        sqlite3* db = m_DB->GetDataBase();

        const char* query = { "INSERT INTO biblioteka VALUES (?)" };
        int rc = sqlite3_prepare_v2(db, query, -1, &stmt, nullptr);
        sqlite3_bind_text(stmt, 1, marca, -1, SQLITE_TRANSIENT);

        if (rc == SQLITE_OK && sqlite3_step(stmt) == SQLITE_DONE)
            TellSucessAndUpdateCombo(_("Marca adicionada com sucesso."));
		else
		{
			if (wxString(sqlite3_errmsg(db)).Find("constraint") > -1)
			{
				auto* dlg = new wxMessageDialog(this, _("Esta marca já existe. Por favor, tente outra."), _("Marca já existente"), wxICON_ERROR);
				dlg->ShowModal();
			}
			else
			{
				auto* dlg = new wxMessageDialog(this, _("Erro ao adicionar a marca. A operação não pode ser concluída."), _("Erro"), wxICON_ERROR);
				dlg->ShowModal();
			}
		}

        sqlite3_finalize(stmt);
    }
}

void cSettings::OnEditMarca(wxCommandEvent& event)
{
    wxString previousMarca = m_MarcaCombo->GetValue();
    m_TextDialog = new wxTextEntryDialog(this, wxString::Format(_("Digite a nova marca para substituir %s"), previousMarca), _("Editar"));
    m_TextDialog->ShowModal();

    wxString value = m_TextDialog->GetValue();
    if (!value.IsEmpty())
    {
        sqlite3_stmt* stmt{};
        sqlite3* db = m_DB->GetDataBase();

        const char* query = { "UPDATE biblioteka SET marca=? WHERE marca=?" };
        int rc = sqlite3_prepare_v2(db, query, -1, &stmt, nullptr);
        sqlite3_bind_text(stmt, 1, value, -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, previousMarca, -1, SQLITE_TRANSIENT);

        if (rc == SQLITE_OK && sqlite3_step(stmt) == SQLITE_DONE)
            TellSucessAndUpdateCombo(_("Marca editada com sucesso."));
        else
        {
            if (wxString(sqlite3_errmsg(db)).Find("constraint") > -1)
            {
                auto* dlg = new wxMessageDialog(this, _("Esta marca já existe. Por favor, tente outra."), _("Marca já existente"), wxICON_ERROR);
                dlg->ShowModal();
                return;
            }
        }
    }
}

void cSettings::OnDeleteMarca(wxCommandEvent& event)
{
    wxString marca = m_MarcaCombo->GetValue();
    auto* dlg = new wxMessageDialog(this, wxString::Format(_("Você tem certeza que deseja deletar a marca %s? Todos os motores associados a ela serão deletados!"), marca),
        _("Deletar motor"), wxICON_WARNING | wxYES_NO);
    const int res = dlg->ShowModal();

    if (res == wxID_YES)
    {
        sqlite3_stmt* stmt{};
        sqlite3* db = m_DB->GetDataBase();

        std::vector<fs::path> list{};
        GetListOfImagesPath("marca", marca, list);

        const char* query = { "DELETE from biblioteka WHERE marca=?" };
        int rc = sqlite3_prepare_v2(db, query, -1, &stmt, nullptr);
        sqlite3_bind_text(stmt, 1, marca, -1, SQLITE_TRANSIENT);

        if (rc == SQLITE_OK && sqlite3_step(stmt) == SQLITE_DONE)
        {
            fs::path root = m_DB->GetRoot();

            for (auto path : list)
            {
                fs::remove(root / "images" / path.filename());
                fs::remove(root / "images_low" / path.filename());
            }

            sqlite3_finalize(stmt);
            TellSucessAndUpdateCombo(_("Marca excluída com sucesso."));
        }
        else
        {
            sqlite3_finalize(stmt);
            auto* dlg = new wxMessageDialog(this, _("Erro ao deletar a marca."), _("Erro"), wxICON_ERROR);
            dlg->ShowModal();
        }
    }
}

void cSettings::TellSucessAndUpdateCombo(wxString msg)
{
    cMain* main = static_cast<cMain*>(m_Parent);
    main->UpdateMarcaCombo();
    main->ClearFrame();

    wxArrayString marca = m_DB->GetComboBoxValues("marca");
    m_MarcaCombo->Set(marca);
    m_MarcaCombo->SetValue(marca[0]);

    auto* dlg = new wxMessageDialog(this, msg, _("Sucesso"), wxICON_INFORMATION);
    dlg->ShowModal();
}

void cSettings::GetListOfImagesPath(wxString column, wxString value, std::vector<fs::path>& outList)
{
    sqlite3* db = m_DB->GetDataBase();
    sqlite3_stmt* res{};

    wxString query = wxString::Format("SELECT image FROM motor WHERE %s = '%s'", column, value);
    const int rc = sqlite3_prepare_v2(db, query, -1, &res, nullptr);

    while (sqlite3_step(res) != SQLITE_DONE && rc == SQLITE_OK)
    {
        wxString path = sqlite3_column_text(res, 0);
        outList.push_back(path.ToStdString());        
    }

    sqlite3_finalize(res);
}
