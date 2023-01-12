#pragma once
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#	pragma hdrstop
#endif

#ifndef WX_PRECOMP
#	include "wx/wx.h"
#endif

#include <string>
#include <vector>
#include <algorithm>
#include <filesystem>

#include "sqlite/sqlite3.h"
#include "wx/grid.h"
#include "wx/valnum.h"
#include "wx/richtext/richtextctrl.h"
#include "wx/stdpaths.h"
#include "cDataBase.h"
#include "cMain.h"
#include "cLog.h"

class cMotor : public wxDialog
{
public:
	cMotor(wxWindow* parent, cDataBase* database, bool isEdit, int rowid = -1);
	~cMotor();

private:
	wxWindow* m_Parent{};
	cDataBase* m_DB{};

	bool b_IsAnotherImage{};
	bool b_IsEdit{};
	int m_RowId{};
	fs::path m_Root{};
	fs::path m_SourceImagePath{};
	struct motor m_Data {};
	struct motor m_Temp {};

	void InitUI();
	void InitWidgets();
	void FrameImage();

	void GetMotorData();
	void PutDataOnScreen();
	void EditMotor();
	int DoEditQuery(struct motor& data);
	void TellEditSucess();

	void OnTipoChanged(wxCommandEvent& event);
	void OnAddImage(wxCommandEvent& event);
	void OnRotateImage(wxCommandEvent& event);

	void OnInsert(wxCommandEvent& event);
	void ClearFields();
	bool GetFrameInputs();

	bool FillMonoStruct();
	bool FillTriStruct();
	bool GenerateImagesPath();
	void SendImagesToPath();
	bool InsertAndCheck(sqlite3_stmt* stmt, sqlite3* db);
	std::string GetRandomFilename(std::string fileExt);

	void TellErrorOnData();
	void TellUnkownError();
	void TellNameAlreadyExists();

	wxBitmap m_Bitmap;
	wxStaticBitmap* m_Image{};
	wxButton* m_AddBtn{};
	wxBitmapButton* m_ImageBtn{};
	wxBitmapButton* m_SaveBtn{};
	wxBitmapButton* m_LeftBtn{};
	wxBitmapButton* m_RightBtn{};
	wxRichTextCtrl* m_NotesCtrl{};

	wxFloatingPointValidator<float> floatVal;
	wxIntegerValidator<unsigned int> intVal;

	wxComboBox* m_MarcaCombo{};
	wxTextCtrl* m_ModeloCtrl{};
	wxTextCtrl* m_PolosCtrl{};
	wxTextCtrl* m_RPMCtrl{};
	wxTextCtrl* m_TensaoCtrl{};
	wxTextCtrl* m_PotenciaCtrl{};
	wxComboBox* m_PotenciaCombo{};
	wxComboBox* m_TipoCombo{};

	wxBitmap m_LeftBitmap = wxBitmap("./icons/left.png", wxBITMAP_TYPE_ANY);
	wxBitmap m_RightBitmap = wxBitmap("./icons/right.png", wxBITMAP_TYPE_ANY);
	wxBitmap m_SaveBitmap = wxBitmap("./icons/save.png", wxBITMAP_TYPE_ANY);
	wxBitmap m_ImageBitmap = wxBitmap("./icons/image.png", wxBITMAP_TYPE_ANY);

	wxBoxSizer* s_RootSizer{};
	wxBoxSizer* s_ImageSizer{};
	wxBoxSizer* s_DetailsSizer{};
	wxStaticBoxSizer* s_NotesSizer{};

	/* Widgets do Motor Monofásico */
	wxTextCtrl* m_EspirasBobinaTrabalhoMono{};
	wxTextCtrl* m_EspessuraFioTrabMono{};
	wxComboBox* m_MaterialFioTrabMono{};

	wxTextCtrl* m_EspirasBobinaAuxMono{};
	wxTextCtrl* m_EspessuraAuxMono{};
	wxComboBox* m_MaterialFioAuxMono{};
	wxTextCtrl* m_NumeroFiosEspiraAuxMono{};

	wxStaticBoxSizer* s_MonoSizer{};

	/* Widgets do Motor Trifásico */
	wxTextCtrl* m_PassoEspiraTri{};
	wxTextCtrl* m_EspessuraFioTri{};
	wxTextCtrl* m_NumeroFioTri{};

	wxStaticBoxSizer* s_TriSizer{};
};

enum ORI_ID {
	LEFT = 500,
	RIGHT,
};