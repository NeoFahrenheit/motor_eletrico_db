#include "cLog.h"

cLog::cLog() : wxFrame(nullptr, wxID_ANY, "Log Window")
{
	s_Sizer = new wxBoxSizer(wxVERTICAL);
	m_Ctrl = new wxRichTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxTE_READONLY);
	wxCaret* caret = m_Ctrl->GetCaret();
	caret->Hide();

	s_Sizer->Add(m_Ctrl, 1, wxEXPAND);
	SetSizer(s_Sizer);
}


cLog::~cLog()
{
}

void cLog::LogText(wxString msg, bool isError)
{
	if (msg.IsEmpty())
		return;

	if (isError)
		m_Ctrl->BeginTextColour(wxColour(217, 26, 42));
	else
		m_Ctrl->BeginTextColour(wxColour(0, 0, 0));

	m_Ctrl->WriteText(msg);
	m_Ctrl->Newline();
}

void cLog::Clear()
{
	m_Ctrl->Clear();
}
