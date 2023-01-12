#include "cMotor.h"

cMotor::cMotor(wxWindow* parent, cDataBase* database, bool isEdit, int rowid) : wxDialog(parent, wxID_ANY, "")
{
	m_Parent = parent;
	m_DB = database;
	b_IsEdit = isEdit;

	m_Root = m_DB->GetRoot();

	floatVal.SetRange(0, 9999999);
	intVal.SetRange(0, 99999);

	InitUI();
	CenterOnParent();

	if (isEdit)
	{
		SetTitle(_("Editar motor"));
		m_RowId = rowid;
		GetMotorData();
		PutDataOnScreen();
		// Copiando os dados do motor atual para a variável temporária.
		m_Temp = m_Data;
	}
	else
		SetTitle(_("Adicionar motor"));
}

cMotor::~cMotor()
{
}

void cMotor::InitUI()
{
	s_RootSizer = new wxBoxSizer(wxHORIZONTAL);
	s_ImageSizer = new wxBoxSizer(wxVERTICAL);
	s_DetailsSizer = new wxBoxSizer(wxVERTICAL);
	s_NotesSizer = new wxStaticBoxSizer(wxVERTICAL, this, _("Observações"));

	m_NotesCtrl = new wxRichTextCtrl(this, wxID_ANY);
	m_SaveBtn = new wxBitmapButton(this, wxID_ANY, m_SaveBitmap);
	m_SaveBtn->SetToolTip(_("Salvar motor"));
	m_LeftBtn = new wxBitmapButton(this, ORI_ID::LEFT, m_LeftBitmap);
	m_LeftBtn->SetToolTip(_("Rotacionar imagem para esquerda"));
	m_RightBtn = new wxBitmapButton(this, ORI_ID::RIGHT, m_RightBitmap);
	m_RightBtn->SetToolTip(_("Rotacionar imagem para direita"));
	m_ImageBtn = new wxBitmapButton(this, wxID_ANY, m_ImageBitmap);
	m_ImageBtn->SetToolTip(_("Trocar imagem"));

	wxBoxSizer* s_ButtonsNotesSizer = new wxBoxSizer(wxHORIZONTAL);
	s_ButtonsNotesSizer->Add(m_SaveBtn, 0, wxLEFT | wxRIGHT | wxALIGN_CENTER, 15);
	s_ButtonsNotesSizer->Add(m_LeftBtn, 0, wxLEFT | wxRIGHT | wxALIGN_CENTER, 15);
	s_ButtonsNotesSizer->Add(m_RightBtn, 0, wxLEFT | wxRIGHT | wxALIGN_CENTER, 15);
	s_ButtonsNotesSizer->Add(m_ImageBtn, 0, wxLEFT | wxRIGHT | wxALIGN_CENTER, 15);
	s_NotesSizer->Add(m_NotesCtrl, 1, wxEXPAND);
	s_NotesSizer->Add(s_ButtonsNotesSizer, 0, wxALL | wxALIGN_CENTER, 5);

	m_AddBtn = new wxButton(this, wxID_ANY, _("Adicionar imagem"));
	m_AddBtn->Bind(wxEVT_BUTTON, &cMotor::OnAddImage, this, wxID_ANY);
	m_LeftBtn->Bind(wxEVT_BUTTON, &cMotor::OnRotateImage, this, wxID_ANY);
	m_RightBtn->Bind(wxEVT_BUTTON, &cMotor::OnRotateImage, this, wxID_ANY);
	m_ImageBtn->Bind(wxEVT_BUTTON, &cMotor::OnAddImage, this, wxID_ANY);
	m_SaveBtn->Bind(wxEVT_BUTTON, &cMotor::OnInsert, this, wxID_ANY);

	m_Bitmap = wxBitmap();
	m_Image = new wxStaticBitmap(this, wxID_ANY, m_Bitmap);
	m_Image->Hide();

	InitWidgets();
	s_ImageSizer->Add(m_AddBtn, 0, wxTOP | wxALIGN_CENTER, 200);
	s_ImageSizer->Add(m_Image, 1, wxEXPAND);
	s_RootSizer->Add(s_ImageSizer, 3, wxALL | wxEXPAND, 5);
	s_RootSizer->Add(s_DetailsSizer, 2, wxALL | wxEXPAND, 5);
	s_RootSizer->Add(s_NotesSizer, 2, wxALL | wxEXPAND, 5);

	SetSizerAndFit(s_RootSizer);
	Layout();
}

void cMotor::InitWidgets()
{
	wxArrayString MarcaChoices = m_DB->GetComboBoxValues("marca");
	m_MarcaCombo = new wxComboBox(this, wxID_ANY, MarcaChoices[0], wxDefaultPosition, wxDefaultSize, MarcaChoices, wxCB_READONLY | wxCB_SORT);

	wxArrayString TipoChoices; TipoChoices.Add(_("Monofásico")), TipoChoices.Add(_("Trifásico"));
	m_TipoCombo = new wxComboBox(this, wxID_ANY, _("Monofásico"), wxDefaultPosition, wxDefaultSize, TipoChoices, wxCB_READONLY);
	m_TipoCombo->Bind(wxEVT_COMBOBOX, &cMotor::OnTipoChanged, this, wxID_ANY);

	wxArrayString PotenciaChoices; PotenciaChoices.Add(_("W")), PotenciaChoices.Add(_("HP")); PotenciaChoices.Add(_("CV"));
	m_PotenciaCombo = new wxComboBox(this, wxID_ANY, _("W"), wxDefaultPosition, wxDefaultSize, PotenciaChoices, wxCB_READONLY);

	m_ModeloCtrl = new wxTextCtrl(this, wxID_ANY);
	m_PolosCtrl = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, 0L, intVal);
	m_RPMCtrl = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, 0L, intVal);
	m_TensaoCtrl = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, 0L, floatVal);
	m_PotenciaCtrl = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, 0L, floatVal);

	const unsigned int leftPadding{ 5 };
	const unsigned int topPadding{ 5 };
#ifdef __linux__
	const unsigned int textPadding{ 7 };
	const wxSize textSize{ wxSize(180, 23) };
	const wxSize detailTextSize{ wxSize(240, 23) };
#else
	const unsigned int textPadding{ 3 };
	const wxSize textSize{ wxSize(100, 23) };
	const wxSize detailTextSize{ wxSize(180, 23) };
#endif

	auto* s_MarcaSizer = new wxBoxSizer(wxHORIZONTAL);
	s_MarcaSizer->Add(new wxStaticText(this, wxID_ANY, _("Marca"), wxDefaultPosition, textSize), 0, wxTOP, textPadding);
	s_MarcaSizer->Add(m_MarcaCombo, 1, wxLEFT | wxEXPAND, leftPadding);
	s_DetailsSizer->Add(s_MarcaSizer, 0, wxEXPAND);

	auto* s_ModeloSizer = new wxBoxSizer(wxHORIZONTAL);
	s_ModeloSizer->Add(new wxStaticText(this, wxID_ANY, _("Modelo"), wxDefaultPosition, textSize), 0, wxTOP, textPadding);
	s_ModeloSizer->Add(m_ModeloCtrl, 1, wxLEFT, leftPadding);
	s_DetailsSizer->Add(s_ModeloSizer, 0, wxTOP | wxEXPAND, topPadding);

	auto* s_PolosSizer = new wxBoxSizer(wxHORIZONTAL);
	s_PolosSizer->Add(new wxStaticText(this, wxID_ANY, _("Número de polos"), wxDefaultPosition, textSize), 0, wxTOP, textPadding);
	s_PolosSizer->Add(m_PolosCtrl, 1, wxLEFT, leftPadding);
	s_DetailsSizer->Add(s_PolosSizer, 0, wxTOP | wxEXPAND, topPadding);

	auto* s_RPMSizer = new wxBoxSizer(wxHORIZONTAL);
	s_RPMSizer->Add(new wxStaticText(this, wxID_ANY, _("Rotações (RPM)"), wxDefaultPosition, textSize), 0, wxTOP, textPadding);
	s_RPMSizer->Add(m_RPMCtrl, 1, wxLEFT, leftPadding);
	s_DetailsSizer->Add(s_RPMSizer, 0, wxTOP | wxEXPAND, topPadding);

	auto s_TensaoSizer = new wxBoxSizer(wxHORIZONTAL);
	s_TensaoSizer->Add(new wxStaticText(this, wxID_ANY, _("Tensão (Volts)"), wxDefaultPosition, textSize), 0, wxTOP, textPadding);
	s_TensaoSizer->Add(m_TensaoCtrl, 1, wxLEFT, leftPadding);
	s_DetailsSizer->Add(s_TensaoSizer, 0, wxTOP | wxEXPAND, topPadding);

	auto* s_PotenciaSizer = new wxBoxSizer(wxHORIZONTAL);
	s_PotenciaSizer->Add(new wxStaticText(this, wxID_ANY, _("Potência"), wxDefaultPosition, textSize), 0, wxTOP, textPadding);
	s_PotenciaSizer->Add(m_PotenciaCtrl, 1, wxLEFT, leftPadding);
	s_PotenciaSizer->Add(m_PotenciaCombo, 0, wxLEFT, leftPadding);
	s_DetailsSizer->Add(s_PotenciaSizer, 0, wxTOP | wxEXPAND, topPadding);

	auto* s_TipoSizer = new wxBoxSizer(wxHORIZONTAL);
	s_TipoSizer->Add(new wxStaticText(this, wxID_ANY, _("Tipo do motor"), wxDefaultPosition, textSize), 0, wxTOP, textPadding);
	s_TipoSizer->Add(m_TipoCombo, 1, wxLEFT, leftPadding);
	s_DetailsSizer->Add(s_TipoSizer, 0, wxTOP | wxEXPAND, topPadding);

	// Iniciando o sizer do motor monof?sico
	s_MonoSizer = new wxStaticBoxSizer(wxVERTICAL, this, _("Monofásico"));

	m_EspirasBobinaTrabalhoMono = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, 0L, intVal);
	m_EspessuraFioTrabMono = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, 0L, floatVal);
	wxArrayString MaterialChoices = m_DB->GetComboBoxValues("material");
	m_MaterialFioTrabMono = new wxComboBox(this, wxID_ANY, MaterialChoices[0], wxDefaultPosition, wxDefaultSize, MaterialChoices, wxCB_READONLY);

	m_EspirasBobinaAuxMono = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, 0L, intVal);
	m_EspessuraAuxMono = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, 0L, floatVal);
	m_MaterialFioAuxMono = new wxComboBox(this, wxID_ANY, MaterialChoices[0], wxDefaultPosition, wxDefaultSize, MaterialChoices, wxCB_READONLY);
	m_NumeroFiosEspiraAuxMono = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, 0L, intVal);

	auto* s_EspirasTrabalhoMono = new wxBoxSizer(wxHORIZONTAL);
	s_EspirasTrabalhoMono->Add(new wxStaticText(this, wxID_ANY, _("n° espiras da bobina de trabalho"), wxDefaultPosition, detailTextSize), 0, wxTOP, textPadding);
	s_EspirasTrabalhoMono->Add(m_EspirasBobinaTrabalhoMono, 0, wxLEFT, leftPadding);
	s_MonoSizer->Add(s_EspirasTrabalhoMono, 1, wxALL | wxEXPAND, topPadding);
	auto* s_EspessuraTrabalhoMono = new wxBoxSizer(wxHORIZONTAL);
	s_EspessuraTrabalhoMono->Add(new wxStaticText(this, wxID_ANY, _("Espessura do fio (mm)"), wxDefaultPosition, detailTextSize, wxALIGN_RIGHT), 0, wxTOP, textPadding);
	s_EspessuraTrabalhoMono->Add(m_EspessuraFioTrabMono, 1, wxLEFT, leftPadding);
	s_MonoSizer->Add(s_EspessuraTrabalhoMono, 1, wxALL | wxLEFT | wxEXPAND, topPadding);

	auto* s_MaterialFioTrabMono = new wxBoxSizer(wxHORIZONTAL);
	s_MaterialFioTrabMono->Add(new wxStaticText(this, wxID_ANY, _("Material do fio"), wxDefaultPosition, detailTextSize, wxALIGN_RIGHT), 0, wxTOP, textPadding);
	s_MaterialFioTrabMono->Add(m_MaterialFioTrabMono, 1, wxLEFT, leftPadding);
	s_MonoSizer->Add(s_MaterialFioTrabMono, 1, wxALL | wxEXPAND, topPadding);

	auto* s_EspirasAuxlhoMono = new wxBoxSizer(wxHORIZONTAL);
	s_EspirasAuxlhoMono->Add(new wxStaticText(this, wxID_ANY, _("n° espiras da bobina auxiliar"), wxDefaultPosition, detailTextSize), 0, wxTOP, textPadding);
	s_EspirasAuxlhoMono->Add(m_EspirasBobinaAuxMono, 0, wxLEFT, leftPadding);
	s_MonoSizer->Add(s_EspirasAuxlhoMono, 1, wxALL | wxEXPAND, topPadding);
	auto* s_EspessuraAuxMono = new wxBoxSizer(wxHORIZONTAL);
	s_EspessuraAuxMono->Add(new wxStaticText(this, wxID_ANY, _("Espessura do fio (mm)"), wxDefaultPosition, detailTextSize, wxALIGN_RIGHT), 0, wxTOP, textPadding);
	s_EspessuraAuxMono->Add(m_EspessuraAuxMono, 1, wxLEFT, leftPadding);
	s_MonoSizer->Add(s_EspessuraAuxMono, 1, wxALL | wxEXPAND, topPadding);

	auto* s_MaterialFioAuxMono = new wxBoxSizer(wxHORIZONTAL);
	s_MaterialFioAuxMono->Add(new wxStaticText(this, wxID_ANY, _("Material do fio"), wxDefaultPosition, detailTextSize, wxALIGN_RIGHT), 0, wxTOP, textPadding);
	s_MaterialFioAuxMono->Add(m_MaterialFioAuxMono, 1, wxLEFT, leftPadding);
	s_MonoSizer->Add(s_MaterialFioAuxMono, 1, wxALL | wxEXPAND, topPadding);

	auto* s_NumeroFioAuxMono = new wxBoxSizer(wxHORIZONTAL);
	s_NumeroFioAuxMono->Add(new wxStaticText(this, wxID_ANY, _("n° de fios por espira"), wxDefaultPosition, detailTextSize, wxALIGN_RIGHT), 0, wxTOP, textPadding);
	s_NumeroFioAuxMono->Add(m_NumeroFiosEspiraAuxMono, 1, wxLEFT, leftPadding);
	s_MonoSizer->Add(s_NumeroFioAuxMono, 1, wxALL | wxEXPAND, topPadding);

	s_DetailsSizer->Add(s_MonoSizer, 0, wxTOP, 5);

	// Iniciando o sizer do motor trifásico
	s_TriSizer = new wxStaticBoxSizer(wxVERTICAL, this, _("Trifásico"));

	const wxSize triTextSize{ wxSize(160, 23) };
	m_PassoEspiraTri = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, 0L, intVal);
	m_EspessuraFioTri = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, 0L, floatVal);
	m_NumeroFioTri = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, 0L, intVal);

	auto* s_PassoEspiraTri = new wxBoxSizer(wxHORIZONTAL);
	s_PassoEspiraTri->Add(new wxStaticText(this, wxID_ANY, _("Passo de cada espira"), wxDefaultPosition, triTextSize), 0, wxTOP, textPadding);
	s_PassoEspiraTri->Add(m_PassoEspiraTri, 1, wxLEFT, leftPadding);
	s_TriSizer->Add(s_PassoEspiraTri, 1, wxALL | wxEXPAND, topPadding);

	auto* s_EspessuraFioTri = new wxBoxSizer(wxHORIZONTAL);
	s_EspessuraFioTri->Add(new wxStaticText(this, wxID_ANY, _("Espessura do fio (mm)"), wxDefaultPosition, triTextSize), 0, wxTOP, textPadding);
	s_EspessuraFioTri->Add(m_EspessuraFioTri, 1, wxLEFT, leftPadding);
	s_TriSizer->Add(s_EspessuraFioTri, 1, wxALL | wxEXPAND, topPadding);

	auto* s_NumeroFioTri = new wxBoxSizer(wxHORIZONTAL);
	s_NumeroFioTri->Add(new wxStaticText(this, wxID_ANY, _("n° de fios por espira"), wxDefaultPosition, triTextSize), 0, wxTOP, textPadding);
	s_NumeroFioTri->Add(m_NumeroFioTri, 1, wxLEFT, leftPadding);
	s_TriSizer->Add(s_NumeroFioTri, 1, wxALL | wxEXPAND, topPadding);

	s_TriSizer->ShowItems(false);
	s_DetailsSizer->Add(s_TriSizer, 0, wxTOP | wxEXPAND, 5);

	s_TriSizer->ShowItems(false);
	s_MonoSizer->ShowItems(true);
}
void cMotor::OnTipoChanged(wxCommandEvent& event)
{
	if (m_TipoCombo->GetValue() == _("Monofásico"))
	{
		s_TriSizer->ShowItems(false);
		s_MonoSizer->ShowItems(true);
	}

	else
	{
		s_TriSizer->ShowItems(true);
		s_MonoSizer->ShowItems(false);
	}

	Layout();
	event.Skip();
}

void cMotor::OnAddImage(wxCommandEvent& event)
{
	wxFileDialog* dialog = new wxFileDialog(this, _("Selecione uma imagem"), "", "",
		_("Imagens (*.jpg, *.png, *.jpeg, *.gif, *.bmp)|*.jpg;*.png;*.jpeg;*.gif;*.bmp"),
		wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_PREVIEW);

	if (dialog->ShowModal() == wxID_OK)
	{
		m_SourceImagePath = dialog->GetPath().ToStdString();
		m_Bitmap = wxBitmap(m_SourceImagePath.string(), wxBITMAP_TYPE_ANY);

		Freeze();
		b_IsAnotherImage = true;
		m_AddBtn->Hide();
		m_Image->Show();
		FrameImage();
		Thaw();
	}
}

void cMotor::OnRotateImage(wxCommandEvent& event)
{
	if (!m_Bitmap.IsOk())
		return;

	const int ID = event.GetId();
	bool isClockwise{};

	if (ID == ORI_ID::LEFT)
		isClockwise = false;
	else
		isClockwise = true;

	wxImage image = m_Bitmap.ConvertToImage();
	image = image.Rotate90(isClockwise);
	m_Bitmap = wxBitmap(image);
	b_IsAnotherImage = true;
	FrameImage();
}

void cMotor::OnInsert(wxCommandEvent& event)
{
	if (b_IsEdit)
	{
		m_SaveBtn->Disable();
		EditMotor();
		m_SaveBtn->Enable();
		return;
	}

	m_SaveBtn->Disable();

	sqlite3_stmt* stmt{};
	sqlite3* db = m_DB->GetDataBase();

	const char* query = { "INSERT INTO motor (marca, modelo, tipo, polos, rotacao, tensao, potencia, " \
						 "potencia_tipo, n_espira_trabalho_mono, esp_fio_trabalho_mono, material_fio_trabalho_mono, " \
						 "n_espira_auxiliar_mono, esp_fio_auxiliar_mono, material_fio_auxiliar_mono, n_fio_espira_mono, " \
						 "passo_espira_tri, esp_fio_tri, n_fios_espira_tri, obs, image, image_small) " \
						 "VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)"
	};
	int rc = sqlite3_prepare_v2(db, query, -1, &stmt, nullptr);
	if (rc != SQLITE_OK)
	{
		TellUnkownError();
		sqlite3_finalize(stmt);
		m_SaveBtn->Enable();
		return;
	}

	if (!GetFrameInputs())
	{
		TellErrorOnData();
		m_SaveBtn->Enable();
		return;
	}

	// Fazendo o binding para a query SQL.
	sqlite3_bind_text(stmt, 1, m_Data.marca, -1, SQLITE_TRANSIENT);
	sqlite3_bind_text(stmt, 2, m_Data.modelo, -1, SQLITE_TRANSIENT);
	sqlite3_bind_text(stmt, 3, m_Data.tipo, -1, SQLITE_TRANSIENT);
	sqlite3_bind_int(stmt, 4, m_Data.polos);
	sqlite3_bind_int(stmt, 5, m_Data.rotacao);
	sqlite3_bind_double(stmt, 6, m_Data.tensao);
	sqlite3_bind_double(stmt, 7, m_Data.potencia);
	sqlite3_bind_text(stmt, 8, m_Data.potencia_tipo, -1, SQLITE_TRANSIENT);

	if (m_Data.tipo == _("Monofásico"))
	{
		sqlite3_bind_int(stmt, 9, m_Data.espirasTrabalhoMono);
		sqlite3_bind_double(stmt, 10, m_Data.espessuraFioTrabMono);
		sqlite3_bind_text(stmt, 11, m_Data.materialFioTrabMono, -1, SQLITE_TRANSIENT);
		sqlite3_bind_int(stmt, 12, m_Data.espirasAuxMono);
		sqlite3_bind_double(stmt, 13, m_Data.espessuraAuxMono);
		sqlite3_bind_text(stmt, 14, m_Data.materialFioAuxMono, -1, SQLITE_TRANSIENT);
		sqlite3_bind_int(stmt, 15, m_Data.fiosAuxMono);

		sqlite3_bind_null(stmt, 16);
		sqlite3_bind_null(stmt, 17);
		sqlite3_bind_null(stmt, 18);
	}
	else
	{
		sqlite3_bind_null(stmt, 9);
		sqlite3_bind_null(stmt, 10);
		sqlite3_bind_null(stmt, 11);
		sqlite3_bind_null(stmt, 12);
		sqlite3_bind_null(stmt, 13);
		sqlite3_bind_null(stmt, 14);
		sqlite3_bind_null(stmt, 15);

		sqlite3_bind_int(stmt, 16, m_Data.passoEspiraTri);
		sqlite3_bind_double(stmt, 17, m_Data.espessuraFioTri);
		sqlite3_bind_int(stmt, 18, m_Data.fiosEspiraTri);
	}

	sqlite3_bind_text(stmt, 19, m_Data.obs, -1, SQLITE_TRANSIENT);
	sqlite3_bind_text(stmt, 20, wxString(m_Data.image_path.c_str()), -1, SQLITE_TRANSIENT);
	sqlite3_bind_text(stmt, 21, wxString(m_Data.image_small_path.c_str()), -1, SQLITE_TRANSIENT);

	InsertAndCheck(stmt, db);
	m_SaveBtn->Enable();
}

// Gera um nome aleatório para as imagens e popula as variáveis correspondentes em m_Data.
// Retorna false se não houver imagem.
bool cMotor::GenerateImagesPath()
{
	if (m_Bitmap.IsOk())
	{
		fs::path ext = m_SourceImagePath.extension();
		std::string filename = GetRandomFilename(ext.string());

		fs::path imagePathDest = m_Root / "images" / filename;
		m_Data.image_path = imagePathDest;

		fs::path imageLowPath = m_Root / "images_low" / filename;
		m_Data.image_small_path = imageLowPath;
		return true;
	}
	else
		return false;
}

void cMotor::SendImagesToPath()
{
	wxImage image = m_Bitmap.ConvertToImage();
	const int X = static_cast<int>(image.GetWidth() / 3);
	const int Y = static_cast<int>(image.GetHeight() / 3);
	wxImage image_down = image.Scale(X, Y, wxIMAGE_QUALITY_BOX_AVERAGE);

	image.SaveFile(m_Data.image_path.string());
	image_down.SaveFile(m_Data.image_small_path.string());
}

bool cMotor::InsertAndCheck(sqlite3_stmt* stmt, sqlite3* db)
{
	int rc = sqlite3_step(stmt);
	if (rc != SQLITE_DONE)
	{
		sqlite3_finalize(stmt);

		// Motor já existe?
		if (wxString(sqlite3_errmsg(db)).Find("UNIQUE constraint failed") > -1)
		{
			TellNameAlreadyExists();
			return false;
		}
		return false;
	}

	// A partir daqui, já inserimos o motor e só precisamos jogar as imagens nas pastas de destino.
	SendImagesToPath();
	wxMessageDialog dlg = wxMessageDialog(this, _("Motor inserido com sucesso. Dejesa adicionar outro?"), _("Inserção concluída"), wxYES_NO | wxICON_INFORMATION);
	int res = dlg.ShowModal();
	if (res == wxID_YES)
	{
		ClearFields();
		sqlite3_finalize(stmt);
		return true;
	}
	else
	{
		sqlite3_finalize(stmt);
		Destroy();
		return true;
	}
}

std::string cMotor::GetRandomFilename(std::string fileExt)
{
	const char alphanum[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
	int stringLength = sizeof(alphanum) - 1;
	std::string filename;
	bool isUniqueFound{ false };
	srand((unsigned int)time(NULL));

	while (!isUniqueFound)
	{
		filename.clear();
		for (int i = 0; i < 12; i++)
			filename += alphanum[rand() % stringLength];

		filename += fileExt;
		if (!fs::exists(m_Root / "images" / filename))
			isUniqueFound = true;

	}

	return filename;
}

// Pega as informações específicas do motor monofásico nos wxTextCtrl e popula as respectivas
// variáveis em m_Data. Retorna true em caso de sucesso.
bool cMotor::FillMonoStruct()
{
	std::vector<bool> isGoingOK{};

	isGoingOK.push_back(m_EspirasBobinaTrabalhoMono->GetValue().ToInt(&m_Data.espirasTrabalhoMono));
	isGoingOK.push_back(m_EspessuraFioTrabMono->GetValue().ToDouble(&m_Data.espessuraFioTrabMono));
	m_Data.materialFioTrabMono = m_MaterialFioTrabMono->GetValue();
	isGoingOK.push_back(m_EspirasBobinaAuxMono->GetValue().ToInt(&m_Data.espirasAuxMono));
	isGoingOK.push_back(m_EspessuraAuxMono->GetValue().ToDouble(&m_Data.espessuraAuxMono));
	m_Data.materialFioAuxMono = m_MaterialFioAuxMono->GetValue();
	isGoingOK.push_back(m_NumeroFiosEspiraAuxMono->GetValue().ToInt(&m_Data.fiosAuxMono));

	return std::all_of(isGoingOK.begin(), isGoingOK.end(), [](bool v) { return v; });
}

// Pega as informações específicas do motor trifásico nos wxTextCtrl e popula as respectivas
// variáveis em m_Data. Retorna true em caso de sucesso.
bool cMotor::FillTriStruct()
{
	std::vector<bool> isGoingOk{};

	isGoingOk.push_back(m_PassoEspiraTri->GetValue().ToInt(&m_Data.passoEspiraTri));
	isGoingOk.push_back(m_EspessuraFioTri->GetValue().ToDouble(&m_Data.espessuraFioTri));
	isGoingOk.push_back(m_NumeroFioTri->GetValue().ToInt(&m_Data.fiosEspiraTri));

	return std::all_of(isGoingOk.begin(), isGoingOk.end(), [](bool v) { return v; });
}

// Popula a struct motor com a query SQL do ID presente em m_RowId.
void cMotor::GetMotorData()
{
	sqlite3* db = m_DB->GetDataBase();
	sqlite3_stmt* res{};

	wxString query = wxString::Format("SELECT * FROM motor WHERE rowid = %d", m_RowId);
	const int rc = sqlite3_prepare_v2(db, query, -1, &res, nullptr);

	int step = sqlite3_step(res);
	if (rc == SQLITE_OK)
	{
		m_Data.marca = sqlite3_column_text(res, 0);
		m_Data.modelo = sqlite3_column_text(res, 1);
		m_Data.tipo = sqlite3_column_text(res, 2);
		m_Data.polos = sqlite3_column_int(res, 3);
		m_Data.rotacao = sqlite3_column_int(res, 4);
		m_Data.tensao = sqlite3_column_double(res, 5);
		m_Data.potencia = sqlite3_column_double(res, 6);
		m_Data.potencia_tipo = sqlite3_column_text(res, 7);

		m_Data.espirasTrabalhoMono = sqlite3_column_int(res, 8);
		m_Data.espessuraFioTrabMono = sqlite3_column_double(res, 9);
		m_Data.materialFioTrabMono = sqlite3_column_text(res, 10);
		m_Data.espirasAuxMono = sqlite3_column_int(res, 11);
		m_Data.espessuraAuxMono = sqlite3_column_double(res, 12);
		m_Data.materialFioAuxMono = sqlite3_column_text(res, 13);
		m_Data.fiosAuxMono = sqlite3_column_int(res, 14);

		m_Data.passoEspiraTri = sqlite3_column_int(res, 15);
		m_Data.espessuraFioTri = sqlite3_column_double(res, 16);
		m_Data.fiosEspiraTri = sqlite3_column_int(res, 17);

		m_Data.obs = sqlite3_column_text(res, 18);
		m_Data.image_path = wxString(sqlite3_column_text(res, 19)).ToStdString();
		m_Data.image_small_path = wxString(sqlite3_column_text(res, 20)).ToStdString();

		sqlite3_finalize(res);
	}
	else
		return;

}

void cMotor::PutDataOnScreen()
{
	m_MarcaCombo->SetValue(m_Data.marca);
	m_ModeloCtrl->SetValue(m_Data.modelo);
	m_TipoCombo->SetValue(m_Data.tipo);
	m_PolosCtrl->SetValue(std::to_string(m_Data.polos));
	m_RPMCtrl->SetValue(std::to_string(m_Data.rotacao));
	m_TensaoCtrl->SetValue(std::to_string(m_Data.tensao));
	m_PotenciaCtrl->SetValue(std::to_string(m_Data.potencia));
	m_PotenciaCombo->SetValue(m_Data.potencia_tipo);

	if (m_Data.tipo == _("Monofásico"))
	{
		m_EspirasBobinaTrabalhoMono->SetValue(std::to_string(m_Data.espirasTrabalhoMono));
		m_EspessuraFioTrabMono->SetValue(std::to_string(m_Data.espessuraFioTrabMono));
		m_MaterialFioTrabMono->SetValue(m_Data.materialFioTrabMono);
		m_EspirasBobinaAuxMono->SetValue(std::to_string(m_Data.espirasAuxMono));
		m_EspessuraAuxMono->SetValue(std::to_string(m_Data.espessuraAuxMono));
		m_MaterialFioAuxMono->SetValue(m_Data.materialFioAuxMono);
		m_NumeroFiosEspiraAuxMono->SetValue(std::to_string(m_Data.fiosAuxMono));
		m_NotesCtrl->SetValue(m_Data.obs);

		m_SourceImagePath = m_Data.image_path;
		m_Bitmap = wxBitmap(m_SourceImagePath.string(), wxBITMAP_TYPE_ANY);
	}
	else
	{
		m_PassoEspiraTri->SetValue(std::to_string(m_Data.passoEspiraTri));
		m_EspessuraFioTri->SetValue(std::to_string(m_Data.espessuraFioTri));
		m_NumeroFioTri->SetValue(std::to_string(m_Data.fiosEspiraTri));
		m_NotesCtrl->SetValue(m_Data.obs);

		m_SourceImagePath = m_Data.image_path;
		m_Bitmap = wxBitmap(m_SourceImagePath.string(), wxBITMAP_TYPE_ANY);
	}

	wxCommandEvent dummy;
	OnTipoChanged(dummy);
	m_AddBtn->Hide();
	m_Image->Show();
	FrameImage();
}

void cMotor::EditMotor()
{
	// Captura os valores digitados de volta para m_Data.
	GetFrameInputs();

	// Fazemos a edição.
	int status = DoEditQuery(m_Data);
	if (status == DB::SUCESS)
	{
		// O usuário modificou ou escolheu outra imagem?
		if (b_IsAnotherImage)
		{
			if (fs::remove(m_Data.image_path) && fs::remove((m_Data.image_small_path)))
				SendImagesToPath();
			else
			{
				DoEditQuery(m_Temp);

				auto* dlg = new wxMessageDialog(this, _("Erro na edição. As imagens antigas não puderam ser removidas. Talvez estejam sendo usadas por outro programa?"),
					_("Erro na remoção"), wxICON_ERROR);
				dlg->ShowModal();
				return;
			}
		}

		TellEditSucess();
		auto* main = dynamic_cast<cMain*>(m_Parent);
		main->ClearFrame();

	}
	else
	{
		if (status == DB::NAME_ALREADY_EXISTS)
			TellNameAlreadyExists();
		else
			TellErrorOnData();
	}
}

void cMotor::TellErrorOnData()
{
	wxMessageDialog dlg = wxMessageDialog(this, _("Erro nos dados inseridos. Evite usar a função colar para preencher os campos e verifique-os novamente. "
		"Tentar salvar sem imagem também acarretará em erro."),
		_("Erro"), wxICON_ERROR);

	dlg.ShowModal();
}

void cMotor::TellUnkownError()
{
	wxMessageDialog dlg = wxMessageDialog(this, _("Erro desconhecido. Por favor, tente realizar a operação novamente."),
		_("Erro"), wxICON_ERROR);

	dlg.ShowModal();
}

void cMotor::TellNameAlreadyExists()
{
	auto* dlg = new wxMessageDialog(this, _("Um motor com este nome já existe. Por favor, escolha um nome diferente."),
		_("Motor já existe"), wxICON_ERROR);
	dlg->ShowModal();
}

void cMotor::FrameImage()
{
	if (!m_Bitmap.IsOk())
		return;

	wxImage image = m_Bitmap.ConvertToImage();
	const double imageAspect = static_cast<double>(image.GetWidth()) / image.GetHeight();

	Layout();
	const wxSize SizerSize = s_ImageSizer->GetSize();
	int newImageWidth = SizerSize.GetWidth();
	int newImageHeight = static_cast<int>(newImageWidth / imageAspect);

	if (newImageHeight > SizerSize.GetHeight())
	{
		newImageHeight = SizerSize.GetHeight();
		newImageWidth = static_cast<int>(newImageHeight * imageAspect);
	}

	wxImage scaledImage = image.Scale(newImageWidth, newImageHeight, wxIMAGE_QUALITY_HIGH);
	m_Image->SetBitmap(wxBitmap(scaledImage));
	Layout();
	Refresh();
}

void cMotor::ClearFields()
{
	m_MarcaCombo->SetValue("ABB");
	m_ModeloCtrl->Clear();
	m_PolosCtrl->Clear();
	m_RPMCtrl->Clear();
	m_TensaoCtrl->Clear();
	m_PotenciaCtrl->Clear();
	m_PotenciaCombo->SetValue("W");

	m_EspirasBobinaTrabalhoMono->Clear();
	m_EspessuraFioTrabMono->Clear();
	m_MaterialFioTrabMono->SetValue("Cobre");

	m_EspirasBobinaAuxMono->Clear();
	m_EspessuraAuxMono->Clear();
	m_MaterialFioAuxMono->SetValue("Cobre");
	m_NumeroFiosEspiraAuxMono->Clear();

	m_PassoEspiraTri->Clear();
	m_EspessuraFioTri->Clear();
	m_NumeroFioTri->Clear();

	m_NotesCtrl->Clear();

	m_Bitmap = wxNullBitmap;
	m_Image->SetBitmap(wxNullBitmap);
	FrameImage();
	m_Image->Hide();
	m_AddBtn->Show();
	Layout();
}

// Faz a edição na tabela do conteúdo de data e com o id m_RowId.
int cMotor::DoEditQuery(struct motor& data)
{
	sqlite3_stmt* stmt{};
	sqlite3* db = m_DB->GetDataBase();

	// Preparando a edição.
	const char* query = { "UPDATE motor SET marca=?, modelo=?, tipo=?, polos=?, rotacao=?, tensao=?, potencia=?, potencia_tipo=?, n_espira_trabalho_mono=?," \
		"esp_fio_trabalho_mono=?, material_fio_trabalho_mono=?, n_espira_auxiliar_mono=?, esp_fio_auxiliar_mono=?, material_fio_auxiliar_mono=?," \
		"n_fio_espira_mono=?, passo_espira_tri=?, esp_fio_tri=? ,n_fios_espira_tri=?, obs=?, image=?, image_small=? WHERE rowid=?" \
	};
	int rc = sqlite3_prepare_v2(db, query, -1, &stmt, nullptr);
	if (rc != SQLITE_OK)
	{
		return DB::EDIT_ERROR;
	}

	sqlite3_bind_text(stmt, 1, data.marca, -1, SQLITE_TRANSIENT);
	sqlite3_bind_text(stmt, 2, data.modelo, -1, SQLITE_TRANSIENT);
	sqlite3_bind_text(stmt, 3, data.tipo, -1, SQLITE_TRANSIENT);
	sqlite3_bind_int(stmt, 4, data.polos);
	sqlite3_bind_int(stmt, 5, data.rotacao);
	sqlite3_bind_double(stmt, 6, data.tensao);
	sqlite3_bind_double(stmt, 7, data.potencia);
	sqlite3_bind_text(stmt, 8, data.potencia_tipo, -1, SQLITE_TRANSIENT);

	if (data.tipo == _("Monofásico"))
	{
		if (FillMonoStruct())
		{
			sqlite3_bind_int(stmt, 9, data.espirasTrabalhoMono);
			sqlite3_bind_double(stmt, 10, data.espessuraFioTrabMono);
			sqlite3_bind_text(stmt, 11, data.materialFioTrabMono, -1, SQLITE_TRANSIENT);
			sqlite3_bind_int(stmt, 12, data.espirasAuxMono);
			sqlite3_bind_double(stmt, 13, data.espessuraAuxMono);
			sqlite3_bind_text(stmt, 14, data.materialFioAuxMono, -1, SQLITE_TRANSIENT);
			sqlite3_bind_int(stmt, 15, data.fiosAuxMono);

			sqlite3_bind_null(stmt, 16);
			sqlite3_bind_null(stmt, 17);
			sqlite3_bind_null(stmt, 18);
		}
		else
			return DB::EDIT_ERROR;
	}

	else
	{
		if (FillTriStruct())
		{
			sqlite3_bind_null(stmt, 9);
			sqlite3_bind_null(stmt, 10);
			sqlite3_bind_null(stmt, 11);
			sqlite3_bind_null(stmt, 12);
			sqlite3_bind_null(stmt, 13);
			sqlite3_bind_null(stmt, 14);
			sqlite3_bind_null(stmt, 15);

			sqlite3_bind_int(stmt, 16, data.passoEspiraTri);
			sqlite3_bind_double(stmt, 17, data.espessuraFioTri);
			sqlite3_bind_int(stmt, 18, data.fiosEspiraTri);
		}
		else
			return DB::EDIT_ERROR;
	}

	sqlite3_bind_text(stmt, 19, data.obs, -1, SQLITE_TRANSIENT);
	sqlite3_bind_text(stmt, 20, wxString(data.image_path.c_str()), -1, SQLITE_TRANSIENT);
	sqlite3_bind_text(stmt, 21, wxString(data.image_small_path.c_str()), -1, SQLITE_TRANSIENT);
	sqlite3_bind_int(stmt, 22, m_RowId);

	if (sqlite3_step(stmt) != SQLITE_DONE)
	{
		if (wxString(sqlite3_errmsg(db)).Find("UNIQUE constraint failed") > -1)
			return DB::NAME_ALREADY_EXISTS;
		else
		{
			return DB::EDIT_ERROR;
		}
	}

	sqlite3_finalize(stmt);
	return DB::SUCESS;
}

void cMotor::TellEditSucess()
{
	auto* dlg = new wxMessageDialog(this, _("Edição concluída com sucesso."), _("Sucesso"), wxICON_INFORMATION);
	dlg->ShowModal();
}

// Captura os wxTextCtrl e wxComboBox da tela e popula a variável m_Data.
bool cMotor::GetFrameInputs()
{
	std::vector<bool> isGoingOk{};
	isGoingOk.push_back(true);

	// Capturando os inputs de texto.
	m_Data.marca = m_MarcaCombo->GetValue();
	m_Data.modelo = m_ModeloCtrl->GetValue().Trim();
	m_Data.obs = m_NotesCtrl->GetValue();

	isGoingOk.push_back(m_PolosCtrl->GetValue().ToInt(&m_Data.polos));
	isGoingOk.push_back(m_RPMCtrl->GetValue().ToInt(&m_Data.rotacao));
	isGoingOk.push_back(m_TensaoCtrl->GetValue().ToDouble(&m_Data.tensao));
	isGoingOk.push_back(m_PotenciaCtrl->GetValue().ToDouble(&m_Data.potencia));
	m_Data.potencia_tipo = m_PotenciaCombo->GetValue();
	m_Data.tipo = m_TipoCombo->GetValue();

	if (m_Data.tipo == _("Monofásico"))
		isGoingOk.push_back(FillMonoStruct());
	else
		isGoingOk.push_back((FillTriStruct()));

	// Na edição, não precisamos gerar um novo nome para as imagens.
	if (!b_IsEdit)
		isGoingOk.push_back(GenerateImagesPath());

	bool status = std::all_of(isGoingOk.begin(), isGoingOk.end(), [](bool v) { return v; });
	return status;
}
