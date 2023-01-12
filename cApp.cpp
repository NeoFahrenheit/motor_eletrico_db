#include "cApp.h"

wxIMPLEMENT_APP(cApp);

cApp::cApp()
{
	GetUserAppDataFolder();	// Get the m_Root
	m_DBPath = m_Root / "biblioteka.db";
}

cApp::~cApp()
{
}

bool cApp::OnInit()
{
	m_DB = new cDataBase(m_Root);
	const int status = m_DB->Open(m_DBPath);
	int c_status{};
	if (status == DB::NOT_FOUND)
	{
		WorkoutDirectories();
		c_status = m_DB->Create(m_DBPath);
		if (c_status != DB::SUCESS)
			return false;
	}

	GetLanguage();
	m_Locale.Init(wxLANGUAGE_PORTUGUESE_BRAZILIAN);
	m_Locale.AddCatalogLookupPathPrefix(".\\langs");
	m_Locale.AddCatalog("pt_BR");

	wxInitAllImageHandlers();

	m_Main = new cMain(m_DB);
	m_Main->CenterOnScreen();
	m_Main->Show();

	return true;
}

void cApp::GetUserAppDataFolder()
{
	fs::path path;
	PWSTR path_tmp;

	auto get_folder_path_ret = SHGetKnownFolderPath(FOLDERID_LocalAppData, KF_FLAG_CREATE, nullptr, &path_tmp);

	/* Error check */
	if (get_folder_path_ret != S_OK) {
		CoTaskMemFree(path_tmp);
			exit(1);
	}

	path = path_tmp;
	m_Root = path / "BibliotekaMotora";

	/* Free memory :) */
	CoTaskMemFree(path_tmp);
}

void cApp::WorkoutDirectories()
{
	// Diretorio existe. Ta tudo Ok?
	if (!(fs::is_directory(m_Root) && fs::is_directory(m_Root / "images") && fs::is_directory(m_Root / "images_low")))
	{
		fs::remove_all(m_Root);
		CreateWorkingFolder();
	}
}

void cApp::CreateWorkingFolder()
{
	fs::create_directory(m_Root);
	fs::create_directory(m_Root / "images");
	fs::create_directory(m_Root / "images_low");
}

void cApp::GetLanguage()
{
	// TODO: Retrieve language from database.
}
