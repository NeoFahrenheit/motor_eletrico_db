#include "cMain.h"

wxBEGIN_EVENT_TABLE(cMain, wxFrame)
	EVT_SIZE(cMain::OnResizing)
	EVT_MAXIMIZE(cMain::OnMaximize)
	EVT_TOOL(ID::NEW, cMain::OnAddMotor)
	EVT_TOOL(ID::EDIT, cMain::OnEditMotor)
	EVT_TOOL(ID::REMOVE, cMain::OnDeleteMotor)

	EVT_LISTBOX(ID::MOTOR_BOX, cMain::OnListSelected)
	EVT_LISTBOX_DCLICK(ID::MOTOR_BOX, cMain::OnListDClicked)
wxEND_EVENT_TABLE()

cMain::cMain(cDataBase* database) : wxFrame(nullptr, wxID_ANY, _("Biblioteka Motora"))
{
	Freeze();
#ifdef __linux__
	SetMinSize(wxSize(1290, 680));
#else
	SetMinSize(wxSize(1100, 680));
#endif

	floatVal.SetRange(0, 9999999);
	intVal.SetRange(0, 99999);

	InitUI();
	SetDetailsToMono();

	m_DB = database;
	SetDoubleBuffered(true);
	ClearFrame();
	UpdateMarcaCombo();

	Thaw();
}

cMain::~cMain()
{
}

void cMain::InitUI()
{
	m_Panel = new wxPanel(this, wxID_ANY);
	wxBoxSizer* s_RootSizer = new wxBoxSizer(wxHORIZONTAL);
	s_LeftSizer = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer* s_RightSizer = new wxBoxSizer(wxVERTICAL);
	s_ImageSizer = new wxBoxSizer(wxVERTICAL);

	s_LeftSizer->Add(s_ImageSizer, 1, wxALL | wxEXPAND, 5);
	s_RootSizer->Add(s_LeftSizer, 1, wxALL | wxEXPAND, 5);
	s_RootSizer->Add(s_RightSizer, 0, wxALL | wxEXPAND, 5);

	// Iniciando os widgets do motor.
	m_MotorStaticBitmap = new wxStaticBitmap(m_Panel, wxID_ANY, m_MotorBitmap);
#ifdef __linux__
	m_MotorDetails = new wxListCtrl(m_Panel, wxID_ANY, wxDefaultPosition, wxSize(800, 215), wxLC_REPORT | wxLC_HRULES);
#else
	m_MotorDetails = new wxListCtrl(m_Panel, wxID_ANY, wxDefaultPosition, wxSize(714, 160), wxLC_REPORT | wxLC_HRULES);
#endif
	m_MotorDetails->AlwaysShowScrollbars(false, false);
	wxFont font = wxFont(14, wxFONTFAMILY_ROMAN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
	m_MotorTitle = new wxStaticText(m_Panel, wxID_ANY, "");
	m_MotorTitle->SetFont(font);
	InitMotorDetails();

	// Iniciando os widgets de pesquisa
	wxBoxSizer* m_SearchSizer = new wxBoxSizer(wxHORIZONTAL);
	m_SearchField = new wxTextCtrl(m_Panel, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
	m_SearchBtn = new wxButton(m_Panel, wxID_ANY, _("Pesquisar"));
	m_SearchBtn->Bind(wxEVT_BUTTON, &cMain::OnSearch, this, wxID_ANY);
	m_SearchField->Bind(wxEVT_TEXT_ENTER, &cMain::OnSearch, this, wxID_ANY);
	m_SearchSizer->Add(m_SearchField, 3);
	m_SearchSizer->Add(m_SearchBtn, 1, wxLEFT, 5);

	// Iniciando os widgets de filtro
	wxBoxSizer* s_FilterSizer = new wxStaticBoxSizer(wxVERTICAL, m_Panel, _("Filtros de pesquisa"));

	const wxSize textSize{ wxSize(130, 23) };
	const wxSize ctrlSize{ wxSize(60, 23) };
	const wxSize comboSize{ wxSize(90, 23) };
	const wxArrayString emptyArr{};

	wxBoxSizer* s_MarcaSizer = new wxBoxSizer(wxHORIZONTAL);
	m_MarcaCheckbox = new wxCheckBox(m_Panel, ID::MARCA, "");
	m_MarcaCheckbox->Bind(wxEVT_CHECKBOX, &cMain::OnCheckBox, this, ID::MARCA);
	m_MarcaText = new wxStaticText(m_Panel, ID::MARCA, _("Marca"), wxDefaultPosition, textSize);
	m_MarcaCombo = new wxComboBox(m_Panel, ID::MARCA, "", wxDefaultPosition, comboSize, emptyArr, wxCB_READONLY | wxCB_SORT);
	m_MarcaCombo->Disable();
	s_MarcaSizer->Add(m_MarcaCheckbox, 0, wxTOP | wxRIGHT, 3);
	s_MarcaSizer->Add(m_MarcaText, 0, wxTOP, 3);
	s_MarcaSizer->Add(m_MarcaCombo, 1, wxLEFT | wxEXPAND, 5);
#ifdef __linux__
	s_FilterSizer->Add(s_MarcaSizer, 0, wxLEFT | wxRIGHT | wxEXPAND, 10);
#else
	s_FilterSizer->Add(s_MarcaSizer, 0, wxEXPAND);
#endif

	wxBoxSizer* s_PotenciaSizer = new wxBoxSizer(wxHORIZONTAL);
	m_PotenciaCheckbox = new wxCheckBox(m_Panel, ID::POTENCIA, "");
	m_PotenciaCheckbox->Bind(wxEVT_CHECKBOX, &cMain::OnCheckBox, this, ID::POTENCIA);
	m_PotenciaText = new wxStaticText(m_Panel, ID::POTENCIA, _("Potência"), wxDefaultPosition, textSize);
	wxArrayString m_PotenciaChoices; m_PotenciaChoices.Add(_("maior que")); m_PotenciaChoices.Add(_("menor que"));
	wxArrayString m_PotenciaUniChoices; m_PotenciaUniChoices.Add(_("W")); m_PotenciaUniChoices.Add(_("HP")); m_PotenciaUniChoices.Add(_("CV"));
#ifdef __linux__
	m_PotenciaCombo = new wxComboBox(m_Panel, ID::POTENCIA, _("maior que"), wxDefaultPosition, wxSize(145, 23), m_PotenciaChoices, wxCB_READONLY);
	m_PotenciaComboUni = new wxComboBox(m_Panel, ID::POTENCIA, _("W"), wxDefaultPosition, wxSize(80, 23), m_PotenciaUniChoices, wxCB_READONLY);
#else
	m_PotenciaCombo = new wxComboBox(m_Panel, ID::POTENCIA, _("maior que"), wxDefaultPosition, wxSize(105, 23), m_PotenciaChoices, wxCB_READONLY);
	m_PotenciaComboUni = new wxComboBox(m_Panel, ID::POTENCIA, _("W"), wxDefaultPosition, wxSize(40, 23), m_PotenciaUniChoices, wxCB_READONLY);
#endif
	m_PotenciaComboUni->Disable();
	m_PotenciaCtrl = new wxTextCtrl(m_Panel, ID::POTENCIA, "", wxDefaultPosition, ctrlSize, 0L, floatVal);
	m_PotenciaCombo->Disable();
	m_PotenciaCtrl->Disable();
	s_PotenciaSizer->Add(m_PotenciaCheckbox, 0, wxTOP | wxRIGHT, 3);
	s_PotenciaSizer->Add(m_PotenciaText, 0, wxTOP, 3);
	s_PotenciaSizer->Add(m_PotenciaCombo, 0, wxLEFT | wxEXPAND, 5);
	s_PotenciaSizer->Add(m_PotenciaCtrl, 0, wxLEFT | wxEXPAND, 5);
	s_PotenciaSizer->Add(m_PotenciaComboUni, 0, wxLEFT | wxEXPAND, 5);
#ifdef __linux__
	s_FilterSizer->Add(s_PotenciaSizer, 1, wxTOP | wxLEFT | wxRIGHT | wxEXPAND, 10);
#else
	s_FilterSizer->Add(s_PotenciaSizer, 1, wxTOP | wxEXPAND, 10);
#endif

	wxBoxSizer* s_RPMSizer = new wxBoxSizer(wxHORIZONTAL);
	m_RPMCheckbox = new wxCheckBox(m_Panel, ID::RPM, "");
	m_RPMCheckbox->Bind(wxEVT_CHECKBOX, &cMain::OnCheckBox, this, ID::RPM);
	m_RPMText = new wxStaticText(m_Panel, ID::RPM, _("RPM"), wxDefaultPosition, textSize);
	wxArrayString m_RPMChoices; m_RPMChoices.Add(_("igual a")); m_RPMChoices.Add(_("maior que")); m_RPMChoices.Add(_("menor que"));
	m_RPMCombo = new wxComboBox(m_Panel, ID::RPM, _("igual a"), wxDefaultPosition, comboSize, m_RPMChoices, wxCB_READONLY);
	m_RPMCtrl = new wxTextCtrl(m_Panel, ID::RPM, "", wxDefaultPosition, ctrlSize, 0L, intVal);
	m_RPMCombo->Disable();
	m_RPMCtrl->Disable();
	s_RPMSizer->Add(m_RPMCheckbox, 0, wxTOP | wxRIGHT, 3);
	s_RPMSizer->Add(m_RPMText, 0, wxTOP, 3);
	s_RPMSizer->Add(m_RPMCombo, 1, wxLEFT | wxEXPAND, 5);
	s_RPMSizer->Add(m_RPMCtrl, 1, wxLEFT | wxEXPAND, 5);
#ifdef __linux__
	s_FilterSizer->Add(s_RPMSizer, 0, wxTOP | wxLEFT | wxRIGHT | wxEXPAND, 10);
#else
	s_FilterSizer->Add(s_RPMSizer, 0, wxTOP | wxEXPAND, 10);
#endif

	wxBoxSizer* s_PolosSizer = new wxBoxSizer(wxHORIZONTAL);
	m_PolosCheckbox = new wxCheckBox(m_Panel, ID::POLOS, "");
	m_PolosCheckbox->Bind(wxEVT_CHECKBOX, &cMain::OnCheckBox, this, ID::POLOS);
	m_PolosText = new wxStaticText(m_Panel, ID::POLOS, _("Número de polos"), wxDefaultPosition, textSize);
	wxArrayString m_PolosChoices; m_PolosChoices.Add(_("igual a")); m_PolosChoices.Add(_("maior que")); m_PolosChoices.Add(_("menor que"));
	m_PolosCombo = new wxComboBox(m_Panel, ID::POLOS, _("igual a"), wxDefaultPosition, comboSize, m_PolosChoices, wxCB_READONLY);
	m_PolosCtrl = new wxTextCtrl(m_Panel, ID::POLOS, "", wxDefaultPosition, ctrlSize, 0L, intVal);
	m_PolosCombo->Disable();
	m_PolosCtrl->Disable();
	s_PolosSizer->Add(m_PolosCheckbox, 0, wxTOP | wxRIGHT, 3);
	s_PolosSizer->Add(m_PolosText, 0, wxTOP, 3);
	s_PolosSizer->Add(m_PolosCombo, 1, wxLEFT | wxEXPAND, 5);
	s_PolosSizer->Add(m_PolosCtrl, 1, wxLEFT | wxEXPAND, 5);
#ifdef __linux__
	s_FilterSizer->Add(s_PolosSizer, 0, wxTOP | wxLEFT | wxRIGHT | wxEXPAND, 10);
#else
	s_FilterSizer->Add(s_PolosSizer, 0, wxTOP | wxEXPAND, 10);
#endif

	auto* s_TipoSizer = new wxBoxSizer(wxHORIZONTAL);
	m_TipoCheckbox = new wxCheckBox(m_Panel, ID::TIPO, "");
	m_TipoCheckbox->Bind(wxEVT_CHECKBOX, &cMain::OnCheckBox, this, ID::TIPO);
	m_TipoText = new wxStaticText(m_Panel, ID::TIPO, _("Tipo do motor"), wxDefaultPosition, textSize);
	wxArrayString m_TipoChoices; m_TipoChoices.Add(_("Monofásico")); m_TipoChoices.Add(_("Trifásico"));
	m_TipoCombo = new wxComboBox(m_Panel, ID::TIPO, _("Monofásico"), wxDefaultPosition, comboSize, m_TipoChoices, wxCB_READONLY);
	m_TipoCombo->Disable();
	s_TipoSizer->Add(m_TipoCheckbox, 0, wxTOP | wxRIGHT, 3);
	s_TipoSizer->Add(m_TipoText, 0, wxTOP, 3);
	s_TipoSizer->Add(m_TipoCombo, 1, wxLEFT | wxEXPAND, 5);
#ifdef __linux__
	s_FilterSizer->Add(s_TipoSizer, 1, wxALL | wxEXPAND, 10);
#else
	s_FilterSizer->Add(s_TipoSizer, 1, wxTOP | wxEXPAND, 10);
#endif

	m_MotorList = new wxListBox(m_Panel, ID::MOTOR_BOX);
	m_NotesCtrl = new wxRichTextCtrl(m_Panel, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxTE_READONLY);

	s_ImageSizer->Add(m_MotorStaticBitmap, 0, wxEXPAND);
	s_LeftSizer->Add(m_MotorTitle, 0, wxALL | wxALIGN_CENTER, 5);
	s_LeftSizer->Add(m_MotorDetails, 0, wxTOP | wxALIGN_CENTER, 5);
	s_RightSizer->Add(m_SearchSizer, 0, wxEXPAND);
#ifdef __linux__
	s_RightSizer->Add(s_FilterSizer, 0, wxTOP | wxEXPAND, 10);
#else
	s_RightSizer->Add(s_FilterSizer, 0, wxTOP, 10);
#endif
	s_RightSizer->Add(m_MotorList, 1, wxTOP | wxEXPAND, 10);
	s_RightSizer->Add(m_NotesCtrl, 1, wxTOP | wxEXPAND, 5);

	m_Panel->SetSizer(s_RootSizer);
	InitMenu();
	InitToolbar();
	CreateStatusBar();
	CenterOnScreen();
}

void cMain::InitMenu()
{
	m_Menubar = new wxMenuBar;

	auto* file = new wxMenu();
	file->Append(wxID_ANY, _("Exportar banco de dados"), _("Exportar o banco de dados atual para um arquivo"));
	file->Append(wxID_ANY, _("Importar banco de dados"), _("Importar um banco de dados a partir de um arquivo"));
	file->Append(ID::SETTINGS, _("Preferências"), _("Acessar a janela de configurações e preferências"));
	file->AppendSeparator();
	file->Append(wxID_EXIT, _("Sair"), _("Sair do programa"));

	auto* manage = new wxMenu();
	manage->Append(ID::NEW, _("Adicionar motor"), _("Cadastrar um novo motor no banco de dados"));
	manage->Append(ID::EDIT, _("Editar motor"), _("Editar um motor existente no banco de dados"));
	manage->Append(ID::REMOVE, _("Remover motor"), _("Remover um motor do banco de dados"));

	auto* view = new wxMenu();
	view->Append(wxID_ANY, _("Próximo"), _("Avançar para o motor seguinte na lista de pesquisa"));
	view->Append(wxID_ANY, _("Anterior"), _("Retroceder para o motor anterior lista de pesquisa"));
	view->Append(ID::FULLSCREEN, _("Ver imagem em tela cheia"), _("Abrir a imagem do motor em tela cheia"));
	view->Append(wxID_ANY, _("Estatísticas"), _("Ver dados gerais sobre o banco de dados"));

	auto* help = new wxMenu();
	help->Append(wxID_ANY, _("Como utilizar"), _("Aprenda como utilizar o programa"));
	help->Append(wxID_ANY, _("Sobre"), _("Abrir a janela de informações sobre este programa"));

	m_Menubar->Append(file, _("Arquivo"));
	m_Menubar->Append(manage, _("Gerenciar"));
	m_Menubar->Append(view, _("Ver"));
	m_Menubar->Append(help, _("Ajuda"));

	Bind(wxEVT_MENU, &cMain::OnViewFullscreen, this, ID::FULLSCREEN);
	Bind(wxEVT_MENU, &cMain::OnDeleteMotor, this, ID::REMOVE);
	Bind(wxEVT_MENU, &cMain::OnSettings, this, ID::SETTINGS);

	SetMenuBar(m_Menubar);
}

void cMain::InitToolbar()
{
	m_Toolbar = CreateToolBar();

	m_Toolbar->AddTool(ID::NEW, _("Adicionar"), wxBitmap("./icons/new.png", wxBITMAP_TYPE_PNG), _("Adicionar Motor"));
	m_Toolbar->AddTool(ID::EDIT, _("Editar"), wxBitmap("./icons/edit.png", wxBITMAP_TYPE_PNG), _("Editar Motor"));
	m_Toolbar->AddTool(ID::REMOVE, _("Deletar"), wxBitmap("./icons/delete.png", wxBITMAP_TYPE_PNG), _("Deletar Motor"));
	m_Toolbar->SetBackgroundColour(wxColour(255, 255, 255));

	m_Toolbar->Realize();
}

void cMain::FrameImage()
{
	if (!m_MotorBitmap.IsOk())
		return;

	Freeze();
	wxImage image = m_MotorBitmap.ConvertToImage();
	const double imageAspect = static_cast<double>(image.GetWidth()) / image.GetHeight();

	m_Panel->Layout();
	const wxSize sizerDim = s_ImageSizer->GetSize();
	int newImageWidth = sizerDim.GetWidth();
	int newImageHeight = static_cast<int>(newImageWidth / imageAspect);

	if (newImageHeight > sizerDim.GetHeight())
	{
		newImageHeight = sizerDim.GetHeight();
		newImageWidth = static_cast<int>(newImageHeight * imageAspect);
	}

	if (newImageWidth <= 0 || newImageHeight <= 0)
	{
		Thaw();
		return;
	}

	wxImage scaledImage = image.Scale(newImageWidth, newImageHeight, wxIMAGE_QUALITY_BILINEAR);
	m_MotorStaticBitmap->SetBitmap(wxBitmap(scaledImage));

	m_Panel->Layout();
	Refresh();
	Thaw();
}

void cMain::SetDetailsToMono()
{
	wxListItem col;
	m_MotorDetails->GetColumn(2, col);
	col.SetText(_("Monofásico"));
	m_MotorDetails->SetColumn(2, col);

	m_MotorDetails->SetItem(0, 2, _("n° espiras da bobina de trabalho"));
	m_MotorDetails->SetItem(0, 3, "");
	m_MotorDetails->SetItem(1, 2, "");
	m_MotorDetails->SetItem(2, 2, "");
	m_MotorDetails->SetItem(1, 3, _("Espessura do fio (mm)"));
	m_MotorDetails->SetItem(2, 3, _("Material do fio"));
	m_MotorDetails->SetItem(3, 3, "");
	m_MotorDetails->SetItem(3, 3, "");

	m_MotorDetails->SetItem(3, 2, _("n° espiras da bobina auxiliar"));
	m_MotorDetails->SetItem(4, 2, "");
	m_MotorDetails->SetItem(5, 2, "");
	m_MotorDetails->SetItem(4, 3, _("Espessura do fio (mm)"));
	m_MotorDetails->SetItem(5, 3, _("Material do fio"));
	m_MotorDetails->SetItem(6, 3, _("n° fios por espira"));

	for (int i{}; i <= 6; i++)
	{
		m_MotorDetails->SetItem(i, 1, "");
		m_MotorDetails->SetItem(i, 4, "");
	}
}

void cMain::SetDetailsToTri()
{
	wxListItem col;
	m_MotorDetails->GetColumn(2, col);
	col.SetText(_("Trifásico"));
	m_MotorDetails->SetColumn(2, col);

	m_MotorDetails->SetItem(0, 2, _("Passo de cada espira"));
	m_MotorDetails->SetItem(1, 2, _("Espessura do fio (mm)"));
	m_MotorDetails->SetItem(2, 2, _("n° fios por espira"));
	m_MotorDetails->SetItem(3, 2, "");
	m_MotorDetails->SetItem(4, 2, "");
	m_MotorDetails->SetItem(5, 2, "");
	m_MotorDetails->SetItem(6, 2, "");

	for (int i{}; i <= 6; i++)
	{
		m_MotorDetails->SetItem(i, 1, "");
		m_MotorDetails->SetItem(i, 3, "");
		m_MotorDetails->SetItem(i, 4, "");
	}
}

void cMain::UpdateMarcaCombo()
{
	wxArrayString marca = m_DB->GetComboBoxValues("marca");
	m_MarcaCombo->Set(marca);
	m_MarcaCombo->SetValue(marca[0]);
}

wxVector<wxString> cMain::GetSearchChecks()
{
	wxVector<wxString> checks{};

	if (m_MarcaCheckbox->IsChecked())
		checks.push_back(wxString::Format("marca = '%s'", m_MarcaCombo->GetValue()));

	if (m_RPMCheckbox->IsChecked())
		checks.push_back(wxString::Format("rotacao %s %s", GetOperator(m_RPMCombo->GetValue()), m_RPMCtrl->GetValue()));

	if (m_PolosCheckbox->IsChecked())
		checks.push_back(wxString::Format("polos %s %s", GetOperator(m_PolosCombo->GetValue()), m_PolosCtrl->GetValue()));

	if (m_TipoCheckbox->IsChecked())
		checks.push_back(wxString::Format("tipo = '%s'", m_TipoCombo->GetValue()));

	return checks;
}

wxString cMain::GetOperator(wxString text)
{
	if (text == _("igual a"))
		return "=";
	if (text == _("maior que"))
		return ">";
	if (text == _("menor que"))
		return "<";

	return "";
}

void cMain::FilterSearchResults()
{
	double userPotenciaVal{};

	bool checkPotencia = m_PotenciaCheckbox->IsChecked();
	bool potenciaCtrl = m_PotenciaCtrl->GetValue().ToDouble(&userPotenciaVal);
	bool isSearchEmpty = m_SearchField->GetValue().IsEmpty();
	std::vector<bool> isGoingOK{};

	wxVector<struct motor> filtered{};
	const wxString search = m_SearchField->GetValue();

	if (checkPotencia || !isSearchEmpty)
	{
		for (auto motor : m_SearchResult)
		{
			isGoingOK.clear();
			isGoingOK.push_back(true);

			if (!isSearchEmpty)
			{
				if (motor.modelo.Lower().find(search.Lower()) == wxNOT_FOUND)
					isGoingOK.push_back(false);
			}

			if (checkPotencia && potenciaCtrl)
			{
				wxString userUnit = m_PotenciaComboUni->GetValue();
				wxString dbUnit = motor.potencia_tipo;
				double searchValue{};

				if (userUnit == dbUnit)
					searchValue = userPotenciaVal;
				else
					searchValue = ConvertBetweenUnits(userUnit, dbUnit, userPotenciaVal);

				if (m_PotenciaCombo->GetValue() == _("maior que"))
				{
					if (motor.potencia < searchValue)
						isGoingOK.push_back(false);
				}
				else
				{
					if (motor.potencia > searchValue)
						isGoingOK.push_back(false);
				}
			}

			bool isAllOk = std::all_of(isGoingOK.begin(), isGoingOK.end(), [](bool v) { return v; });
			if (isAllOk)
				filtered.push_back(motor);
		}
	}
	else
		return;

	m_SearchResult = filtered;
}

void cMain::ClearFrame()
{
	SetDetailsToMono();

	m_MotorStaticBitmap->SetBitmap(m_DefaultBitmap);
	m_MotorBitmap = m_DefaultBitmap;
	FrameImage();
	s_ImageSizer->Layout();
	m_NotesCtrl->Clear();
	m_MotorTitle->SetLabel("");
	m_SearchResult.clear();
	m_MotorList->Clear();
	m_IndexSel = -1;
}

double cMain::ConvertBetweenUnits(wxString fromUnit, wxString toUnit, double value)
{
	if (fromUnit == _("CV"))
	{
		if (toUnit == _("HP"))
			return m_DB->CV_HP(value);
		else
			return m_DB->CV_W(value);
	}

	if (fromUnit == _("HP"))
	{
		if (toUnit == _("CV"))
			return m_DB->HP_CV(value);
		else
			return m_DB->HP_W(value);
	}

	if (fromUnit == _("W"))
	{
		if (toUnit == _("HP"))
			return m_DB->W_HP(value);
		else
			return m_DB->W_CV(value);
	}

	return 0.0;
}

void cMain::InitMotorDetails()
{
	m_MotorDetails->InsertColumn(0, _("Parâmetro"));
	m_MotorDetails->SetColumnWidth(0, 140);

	m_MotorDetails->InsertColumn(1, _("Valor"));
	m_MotorDetails->SetColumnWidth(1, 120);

	m_MotorDetails->InsertColumn(2, _("Monofásico"));
	m_MotorDetails->InsertColumn(3, "");
	m_MotorDetails->InsertColumn(4, "");

#ifdef __linux__
	m_MotorDetails->SetColumnWidth(2, 240);
	m_MotorDetails->SetColumnWidth(3, 180);
	m_MotorDetails->SetColumnWidth(4, 120);
#else
	m_MotorDetails->SetColumnWidth(2, 200);
	m_MotorDetails->SetColumnWidth(3, 140);
	m_MotorDetails->SetColumnWidth(4, 80);
#endif


	m_MotorDetails->InsertItem(0, _("Marca"));
	m_MotorDetails->InsertItem(1, _("Modelo"));
	m_MotorDetails->InsertItem(2, _("Número de Polos"));
	m_MotorDetails->InsertItem(3, _("Rotações (RPM)"));
	m_MotorDetails->InsertItem(4, _("Tensão (Volts)"));
	m_MotorDetails->InsertItem(5, _("Potência"));
	m_MotorDetails->InsertItem(6, "");

	m_MotorDetails->SetItemBackgroundColour(1, wxColour(219, 233, 255));
	m_MotorDetails->SetItemBackgroundColour(3, wxColour(219, 233, 255));
	m_MotorDetails->SetItemBackgroundColour(5, wxColour(219, 233, 255));
}

void cMain::OnResizing(wxSizeEvent& event)
{
	FrameImage();
	event.Skip();
}

void cMain::OnMaximize(wxMaximizeEvent& event)
{
	wxSizeEvent dummy;
	Layout();
	OnResizing(dummy);
	event.Skip();
}

void cMain::OnCheckBox(wxCommandEvent& event)
{
	const int ID = event.GetId();
	const bool value = event.IsChecked();

	switch (ID)
	{
	case ID::POLOS:
		m_PolosCombo->Enable(value);
		m_PolosCtrl->Enable(value);
		break;

	case ID::POTENCIA:
		m_PotenciaCombo->Enable(value);
		m_PotenciaCtrl->Enable(value);
		m_PotenciaComboUni->Enable(value);
		break;

	case ID::RPM:
		m_RPMCombo->Enable(value);
		m_RPMCtrl->Enable(value);
		break;

	case ID::TIPO:
		m_TipoCombo->Enable(value);
		break;

	case ID::MARCA:
		m_MarcaCombo->Enable(value);
		break;

	case ID::MODELO:
		m_ModeloCombo->Enable(value);
		break;
	}

	event.Skip();
}

void cMain::OnAddMotor(wxCommandEvent& event)
{
	cMotor* t_Window = new cMotor(this, m_DB, false);
	t_Window->ShowModal();
	event.Skip();
}

void cMain::OnEditMotor(wxCommandEvent& event)
{
	if (m_IndexSel != -1)
	{
		bool isMono{ false };
		if (m_SearchResult[m_IndexSel].tipo == _("Monofásico"))
			isMono = true;

		cMotor* t_Window = new cMotor(this, m_DB, true, m_SearchResult[m_IndexSel].rowid);
		t_Window->ShowModal();
	}
	else
	{
		auto* dlg = new wxMessageDialog(this, _("Selecione um motor para editá-lo primeiro."), _("Selecione um motor"), wxOK);
		dlg->ShowModal();
	}
}

void cMain::OnDeleteMotor(wxCommandEvent& event)
{
	if (m_IndexSel != -1)
	{
		wxString marca = m_SearchResult[m_IndexSel].marca;
		wxString modelo = m_SearchResult[m_IndexSel].modelo;
		auto* dlg = new wxMessageDialog(this, wxString::Format(_("Tem certeza que deseja deletar o motor %s %s?"), marca, modelo),
			_("Deletar motor"), wxYES_NO | wxICON_WARNING);

		if (dlg->ShowModal() == wxID_YES)
		{
			sqlite3* db = m_DB->GetDataBase();
			sqlite3_stmt* res{};
			wxString query{ "DELETE FROM motor WHERE rowid=? " };

			int rc = sqlite3_prepare_v2(db, query, -1, &res, 0);
			sqlite3_bind_int(res, 1, m_SearchResult[m_IndexSel].rowid);

			if (rc == SQLITE_OK && sqlite3_step(res) == SQLITE_DONE)
			{
				fs::remove(m_SearchResult[m_IndexSel].image_path);
				fs::remove(m_SearchResult[m_IndexSel].image_small_path);

				ClearFrame();
				auto* dlg = new wxMessageDialog(this, wxString::Format(_("Motor %s %s deletado com sucesso."), marca, modelo),
					_("Sucesso"), wxICON_INFORMATION);
				dlg->ShowModal();
			}
			else
			{
				auto* dlg = new wxMessageDialog(this, wxString::Format(_("Erro ao deletar motor %s %s."), marca, modelo), _("Erro"), wxICON_ERROR);
				dlg->ShowModal();
			}
		}
	}
	else
	{
		auto* dlg = new wxMessageDialog(this, _("Selecione um motor para removê-lo primeiro."), _("Selecione um motor"), wxOK);
		dlg->ShowModal();
	}
}

void cMain::OnViewFullscreen(wxCommandEvent& event)
{
	if (m_IndexSel != -1)
	{
		wxString title{ m_SearchResult[m_IndexSel].marca + " " + m_SearchResult[m_IndexSel].modelo };
		auto* window = new cImageViewer(this, m_SearchResult[m_IndexSel].image_path.string(), title);
		window->Show();
	}
}

void cMain::OnSearch(wxCommandEvent& event)
{
	sqlite3* db = m_DB->GetDataBase();
	sqlite3_stmt* res{};
	m_SearchResult.clear();
	m_MotorList->Clear();
	ClearFrame();

	wxVector<wxString> checks = GetSearchChecks();
	wxString last{};
	if (!checks.empty())
	{
		last.append("WHERE (true ");
		for (auto s : checks)
			last.append("and " + s + " ");
		last.append(")");
	}

	wxString query{ "SELECT rowid, * FROM motor " + last };
	unsigned int index{};

	int rc = sqlite3_prepare_v2(db, query, -1, &res, 0);
	while (sqlite3_step(res) != SQLITE_DONE && rc == SQLITE_OK)
	{
		struct motor data;

		data.searchIndex = index;
		data.rowid = sqlite3_column_int(res, 0);
		data.marca = sqlite3_column_text(res, 1);
		data.modelo = sqlite3_column_text(res, 2);
		data.tipo = sqlite3_column_text(res, 3);
		data.polos = sqlite3_column_int(res, 4);
		data.rotacao = sqlite3_column_int(res, 5);
		data.tensao = sqlite3_column_double(res, 6);
		data.potencia = sqlite3_column_double(res, 7);
		data.potencia_tipo = sqlite3_column_text(res, 8);

		data.espirasTrabalhoMono = sqlite3_column_int(res, 9);
		data.espessuraFioTrabMono = sqlite3_column_double(res, 10);
		data.materialFioTrabMono = sqlite3_column_text(res, 11);
		data.espirasAuxMono = sqlite3_column_int(res, 12);
		data.espessuraAuxMono = sqlite3_column_double(res, 13);
		data.materialFioAuxMono = sqlite3_column_text(res, 14);
		data.fiosAuxMono = sqlite3_column_int(res, 15);

		data.passoEspiraTri = sqlite3_column_int(res, 16);
		data.espessuraFioTri = sqlite3_column_double(res, 17);
		data.fiosEspiraTri = sqlite3_column_int(res, 18);

		data.obs = sqlite3_column_text(res, 19);
		data.image_path = wxString(sqlite3_column_text(res, 20)).ToStdString();
		data.image_small_path = wxString(sqlite3_column_text(res, 21)).ToStdString();

		m_SearchResult.push_back(data);
		index++;
	}

	sqlite3_finalize(res);
	FilterSearchResults();

	for (auto& data : m_SearchResult)
		m_MotorList->AppendString(data.marca + " " + data.modelo);
}

void cMain::OnListSelected(wxCommandEvent& event)
{
	Freeze();
	const int id = event.GetSelection();
	m_IndexSel = id;

	if (m_SearchResult[id].tipo == _("Monofásico"))
	{
		SetDetailsToMono();
		m_MotorTitle->SetLabel(m_SearchResult[id].marca + " " + m_SearchResult[id].modelo);
		m_MotorDetails->SetItem(0, 1, m_SearchResult[id].marca);
		m_MotorDetails->SetItem(1, 1, m_SearchResult[id].modelo);
		m_MotorDetails->SetItem(2, 1, std::to_string(m_SearchResult[id].polos));
		m_MotorDetails->SetItem(3, 1, std::to_string(m_SearchResult[id].rotacao));
		m_MotorDetails->SetItem(4, 1, std::to_string(m_SearchResult[id].tensao));
		m_MotorDetails->SetItem(5, 1, std::to_string(m_SearchResult[id].potencia) + " " + m_SearchResult[id].potencia_tipo);

		m_MotorDetails->SetItem(0, 3, std::to_string(m_SearchResult[id].espirasTrabalhoMono));
		m_MotorDetails->SetItem(1, 4, std::to_string(m_SearchResult[id].espessuraFioTrabMono));
		m_MotorDetails->SetItem(2, 4, m_SearchResult[id].materialFioTrabMono);

		m_MotorDetails->SetItem(3, 3, std::to_string(m_SearchResult[id].espirasAuxMono));
		m_MotorDetails->SetItem(4, 4, std::to_string(m_SearchResult[id].espessuraAuxMono));
		m_MotorDetails->SetItem(5, 4, m_SearchResult[id].materialFioAuxMono);
		m_MotorDetails->SetItem(6, 4, std::to_string(m_SearchResult[id].fiosAuxMono));
	}
	else
	{
		SetDetailsToTri();
		m_MotorTitle->SetLabel(m_SearchResult[id].marca + " " + m_SearchResult[id].modelo);
		m_MotorDetails->SetItem(0, 1, m_SearchResult[id].marca);
		m_MotorDetails->SetItem(1, 1, m_SearchResult[id].modelo);
		m_MotorDetails->SetItem(2, 1, std::to_string(m_SearchResult[id].polos));
		m_MotorDetails->SetItem(3, 1, std::to_string(m_SearchResult[id].rotacao));
		m_MotorDetails->SetItem(4, 1, std::to_string(m_SearchResult[id].tensao));
		m_MotorDetails->SetItem(5, 1, std::to_string(m_SearchResult[id].potencia) + " " + m_SearchResult[id].potencia_tipo);

		m_MotorDetails->SetItem(0, 3, std::to_string(m_SearchResult[id].passoEspiraTri));
		m_MotorDetails->SetItem(1, 3, std::to_string(m_SearchResult[id].espessuraFioTri));
		m_MotorDetails->SetItem(2, 3, std::to_string(m_SearchResult[id].fiosEspiraTri));
	}

	m_NotesCtrl->SetValue(m_SearchResult[id].obs);
	m_MotorBitmap = wxBitmap(m_SearchResult[id].image_small_path.c_str(), wxBITMAP_TYPE_ANY);
	FrameImage();
	Thaw();
}

void cMain::OnListDClicked(wxCommandEvent& event)
{
	wxString title{ m_SearchResult[m_IndexSel].marca + " " + m_SearchResult[m_IndexSel].modelo };
	auto* window = new cImageViewer(this, m_SearchResult[m_IndexSel].image_path.string(), title);
	window->Show();
}

void cMain::OnSettings(wxCommandEvent& event)
{
	auto* settings = new cSettings(this, m_DB);
	settings->ShowModal();
}
