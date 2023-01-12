#pragma once
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#	pragma hdrstop
#endif

#ifndef WX_PRECOMP
#	include "wx/wx.h"
#endif

#include "sqlite/sqlite3.h"
#include "wx/stdpaths.h"
#include "cLog.h"
#include <filesystem>
#include <vector>

namespace fs = std::filesystem;
class cDataBase
{
public:
	cDataBase(fs::path root);
	~cDataBase();

	int Open(fs::path fullPath);
	int Create(fs::path fullPath);
	int Close();

	sqlite3* GetDataBase() { return m_DataBase; };
	// cLog* log = new cLog();

	/* Convert functions */
	inline double CV_HP(double cv) { return cv * 0.98632; };
	inline double CV_W(double cv) { return cv * 735.499; };

	inline double HP_CV(double hp) { return hp * 1.01387; };
	inline double HP_W(double hp) { return hp * 745.699; };

	inline double W_HP(double w) { return w / 745.699; };
	inline double W_CV(double w) { return w / 735.499; };

	wxArrayString GetComboBoxValues(wxString type);

	fs::path GetRoot() { return m_Root; };

private:
	sqlite3* m_DataBase{};
	fs::path m_Root{};

	std::vector<const char*> m_ListaMotores{ "ABB", "ECOVIS", "GE", "SIEMENS", "WEG", "Hercules", "Schneider" };
	std::vector<const char*> m_ListaMateriais{ "Cobre", "Alumínio" };

	const char* create_biblioteka{ "CREATE TABLE biblioteka (" \
			"marca TEXT PRIMARY KEY" \
			") STRICT;"
	};

	const char* create_material{ " CREATE TABLE material (" \
			"material TEXT PRIMARY KEY" \
			") STRICT;"
	};

	const char* create_motor{ "CREATE TABLE motor (" \
		"marca TEXT NOT NULL," \
		"modelo TEXT NOT NULL," \
		"tipo TEXT NOT NULL," \
		"polos INTEGER NOT NULL CHECK (polos > 0)," \
		"rotacao INTEGER NOT NULL CHECK (rotacao > 0)," \
		"tensao REAL NOT NULL CHECK (tensao > 0)," \
		"potencia REAL NOT NULL CHECK (potencia > 0)," \
		"potencia_tipo TEXT NOT NULL CHECK (potencia_tipo IN ('W', 'HP', 'CV'))," \

		// Variáveis do motor monofásico.
		"n_espira_trabalho_mono INTEGER CHECK (n_espira_trabalho_mono > 0)," \
		"esp_fio_trabalho_mono REAL CHECK (esp_fio_trabalho_mono > 0)," \
		"material_fio_trabalho_mono TEXT," \
		"n_espira_auxiliar_mono INTEGER CHECK (n_espira_auxiliar_mono > 0)," \
		"esp_fio_auxiliar_mono REAL CHECK (esp_fio_auxiliar_mono > 0)," \
		"material_fio_auxiliar_mono TEXT," \
		"n_fio_espira_mono INTEGER CHECK (n_fio_espira_mono > 0)," \

		// Variáveis do motor trifásico:
		"passo_espira_tri INTEGER CHECK (passo_espira_tri > 0)," \
		"esp_fio_tri REAL CHECK (esp_fio_tri > 0)," \
		"n_fios_espira_tri INTEGER CHECK (n_fios_espira_tri > 0)," \

		"obs TEXT NOT NULL," \
		"image TEXT NOT NULL," \
		"image_small TEXT NOT NULL," \
		"PRIMARY KEY (marca, modelo), " \
		"FOREIGN KEY (marca) REFERENCES biblioteka (marca) ON UPDATE CASCADE ON DELETE CASCADE, " \
		"FOREIGN KEY (material_fio_trabalho_mono) REFERENCES material (material) ON UPDATE CASCADE ON DELETE CASCADE, " \
		"FOREIGN KEY (material_fio_auxiliar_mono) REFERENCES material (material) ON UPDATE CASCADE ON DELETE CASCADE" \
		") STRICT;"
	};
};

enum DB {
	SUCESS = 1000,
	OPEN_ERROR,
	CREATE_ERROR,
	INSERT_ERROR,
	EDIT_ERROR,
	NOT_FOUND,
    NAME_ALREADY_EXISTS,
	MONOFASICO,
	TRIFASICO
};

struct motor
{
	int searchIndex{};
	int rowid{};
	wxString marca{};
	wxString modelo{};
	wxString tipo{};
	int polos{};
	int rotacao{};
	double tensao{};
	double potencia{};
	wxString potencia_tipo{};

	/* Monofasico */
	int espirasTrabalhoMono{};
	double espessuraFioTrabMono{};
	wxString materialFioTrabMono{};
	int espirasAuxMono{};
	double espessuraAuxMono{};
	wxString materialFioAuxMono{};
	int fiosAuxMono{};
	/* Monofasico */

	/* ---------- */

	/* Trifasico */
	int passoEspiraTri{};
	double espessuraFioTri{};
	int fiosEspiraTri{};
	/* Trifasico */

	wxString obs{};
	fs::path image_path{};
	fs::path image_small_path{};

};