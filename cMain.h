#pragma once
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#	pragma hdrstop
#endif

#ifndef WX_PRECOMP
#	include "wx/wx.h"
#endif

#include "wx/listctrl.h"
#include "cMotor.h"
#include "cImageViewer.h"
#include "cDataBase.h"
#include "cSettings.h"

class cMain : public wxFrame
{
public:
	cMain(cDataBase* database);
	~cMain();

	void ClearFrame();
	void UpdateMarcaCombo();

private:
	void InitUI();
	void InitMenu();
	void InitToolbar();
	void InitMotorDetails();

	void FrameImage();
	void SetDetailsToMono();
	void SetDetailsToTri();

	wxVector<wxString> GetSearchChecks();
	wxString GetOperator(wxString text);
	void FilterSearchResults();
	double ConvertBetweenUnits(wxString fromUnit, wxString toUnit, double value);

	void OnResizing(wxSizeEvent& event);
	void OnMaximize(wxMaximizeEvent& event);
	void OnCheckBox(wxCommandEvent& event);
	void OnAddMotor(wxCommandEvent& event);
	void OnEditMotor(wxCommandEvent& event);
	void OnDeleteMotor(wxCommandEvent& event);
	void OnViewFullscreen(wxCommandEvent& event);
	void OnSettings(wxCommandEvent& event);

	void OnSearch(wxCommandEvent& event);
	void OnListSelected(wxCommandEvent& event);
	void OnListDClicked(wxCommandEvent& event);

	int m_IndexSel{ -1 };
	wxVector<struct motor> m_SearchResult;

	cDataBase* m_DB{};

	wxMenuBar* m_Menubar{};
	wxToolBar* m_Toolbar{};

	wxPanel* m_Panel{};
	wxBoxSizer* s_ImageSizer{};
	wxBoxSizer* s_LeftSizer{};
	wxBitmap m_DefaultBitmap{ "./icons/no-pictures.png", wxBITMAP_TYPE_ANY };
	wxBitmap m_MotorBitmap = m_DefaultBitmap;
	wxPanel* m_ImagePanel{};
	wxStaticBitmap* m_MotorStaticBitmap{};
	wxListCtrl* m_MotorDetails{};
	wxTextCtrl* m_SearchField{};
	wxButton* m_SearchBtn{};
	wxListBox* m_MotorList{};
	wxRichTextCtrl* m_NotesCtrl{};
	wxStaticText* m_MotorTitle{};

	wxFloatingPointValidator<float> floatVal;
	wxIntegerValidator<unsigned int> intVal;

	/*
	* --------------------------------
		Widgets de filtro de pesquisa.
	  --------------------------------
	*/

	wxCheckBox* m_PolosCheckbox{};
	wxStaticText* m_PolosText{};
	wxComboBox* m_PolosCombo{};
	wxTextCtrl* m_PolosCtrl{};

	wxCheckBox* m_PotenciaCheckbox{};
	wxStaticText* m_PotenciaText{};
	wxComboBox* m_PotenciaCombo{};
	wxComboBox* m_PotenciaComboUni{};
	wxTextCtrl* m_PotenciaCtrl{};

	wxCheckBox* m_RPMCheckbox{};
	wxStaticText* m_RPMText{};
	wxComboBox* m_RPMCombo{};
	wxTextCtrl* m_RPMCtrl{};

	wxCheckBox* m_TipoCheckbox{};
	wxStaticText* m_TipoText{};
	wxComboBox* m_TipoCombo{};

	wxCheckBox* m_MarcaCheckbox{};
	wxStaticText* m_MarcaText{};
	wxComboBox* m_MarcaCombo{};
	wxComboBox* m_ModeloCombo{};

	wxDECLARE_EVENT_TABLE();
};

enum ID {
	NEW = 1000,
	EDIT,
	REMOVE,
	POLOS,
	POTENCIA,
	RPM,
	TIPO,
	MARCA,
	MODELO,
	MOTOR_BOX,
	FULLSCREEN,
	SETTINGS
};