#pragma once
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#	pragma hdrstop
#endif

#ifndef WX_PRECOMP
#	include "wx/wx.h"
#endif

#include "wx/log.h"
#include "wx/richtext/richtextctrl.h"

class cLog : public wxFrame
{
public:
	cLog();
	~cLog();

	void LogText(wxString msg, bool isError = false);
	void Clear();

private:
	wxBoxSizer* s_Sizer{};
	wxRichTextCtrl* m_Ctrl{};
};

