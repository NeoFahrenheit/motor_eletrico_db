#include "cDataBase.h"

cDataBase::cDataBase(fs::path root)
{
	m_Root = root;
}

cDataBase::~cDataBase()
{
	sqlite3_close(m_DataBase);
}

int cDataBase::Open(fs::path fullPath)
{
	if (fs::exists(fullPath))
	{
		const int status = sqlite3_open(wxString(fullPath.c_str()), &m_DataBase);
		if (status == SQLITE_OK)
		{
			sqlite3_exec(m_DataBase, "PRAGMA foreign_keys=ON;", nullptr, nullptr, nullptr);
			return DB::SUCESS;
		}
		else
			return DB::OPEN_ERROR;
	}
	else
		return DB::NOT_FOUND;
}

int cDataBase::Create(fs::path fullPath)
{
	sqlite3_open(wxString(fullPath.c_str()), &m_DataBase);
	std::vector<bool> isGoingOK{};
	isGoingOK.push_back(true);
	sqlite3* db = m_DataBase;

	if (sqlite3_exec(m_DataBase, "PRAGMA foreign_keys=ON;", nullptr, nullptr, nullptr) != SQLITE_OK)
		isGoingOK.push_back(false);

	if (sqlite3_exec(m_DataBase, create_biblioteka, nullptr, nullptr, nullptr) != SQLITE_OK)
		isGoingOK.push_back(false);

	if (sqlite3_exec(m_DataBase, create_material, nullptr, nullptr, nullptr) != SQLITE_OK)
		isGoingOK.push_back(false);

	if (sqlite3_exec(m_DataBase, create_motor, nullptr, nullptr, nullptr) != SQLITE_OK)
		isGoingOK.push_back(false);

	for (auto* str : m_ListaMotores)
	{
		sqlite3_stmt* res{};
		const int rc = sqlite3_prepare_v2(m_DataBase,"INSERT INTO biblioteka VALUES (?)", -1, &res, nullptr);
		sqlite3_bind_text(res, 1, str, -1, SQLITE_TRANSIENT);

		if (rc != SQLITE_OK || sqlite3_step(res) != SQLITE_DONE)
			isGoingOK.push_back(false);

		sqlite3_finalize(res);
	}

	for (auto* str : m_ListaMateriais)
	{
		sqlite3_stmt* res{};
		const int rc = sqlite3_prepare_v2(m_DataBase, "INSERT INTO material VALUES (?)", -1, &res, nullptr);
		sqlite3_bind_text(res, 1, str, -1, SQLITE_TRANSIENT);

		if (rc != SQLITE_OK || sqlite3_step(res) != SQLITE_DONE)
			isGoingOK.push_back(false);

		sqlite3_finalize(res);
	}

	if (std::all_of(isGoingOK.begin(), isGoingOK.end(), [](bool v) { return v; }))
		return DB::SUCESS;
	else
		return DB::CREATE_ERROR;
}

int cDataBase::Close()
{
	return sqlite3_close(m_DataBase);
}

wxArrayString cDataBase::GetComboBoxValues(wxString type)
{
	sqlite3_stmt* res{};
	wxArrayString list{};

	wxString query{};
	if (type == "marca")
		query = { "SELECT * FROM biblioteka ORDER BY marca" };
	else
		query = { "SELECT * FROM material ORDER BY material" };

	const int rc = sqlite3_prepare_v2(m_DataBase, query, -1, &res, nullptr);

	while (sqlite3_step(res) != SQLITE_DONE)
	{
		wxString marca = sqlite3_column_text(res, 0);
		list.push_back(marca);
	}

	sqlite3_finalize(res);

	return list;
}