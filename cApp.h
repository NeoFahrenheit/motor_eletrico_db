#pragma once
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#	pragma hdrstop
#endif

#ifndef WX_PRECOMP
#	include "wx/wx.h"
#endif

#include "cMain.h"
#include "cDataBase.h"
#include "shlobj_core.h"

class cApp : public wxApp
{
public:
	cApp();
	~cApp();

	virtual bool OnInit();

private:
	cDataBase* m_DB{};
	cMain* m_Main{};
	wxLocale m_Locale{};

	fs::path m_Root{};
	fs::path m_DBPath{};

	void GetUserAppDataFolder();
	void WorkoutDirectories();
	void CreateWorkingFolder();
	void GetLanguage();
};