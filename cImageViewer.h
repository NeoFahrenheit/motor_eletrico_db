#pragma once
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#	pragma hdrstop
#endif

#ifndef WX_PRECOMP
#	include "wx/wx.h"
#endif

#include <wx/graphics.h>
#include <wx/dcbuffer.h>

class cImageViewer : public wxFrame
{
public:
    cImageViewer(wxWindow* parent, wxString path, wxString title);
    wxRect2DDouble GetUntransformedRect() const;

protected:
    void DoDrawCanvas(wxGraphicsContext*);

private:
    void OnPaint(wxPaintEvent&);

    void OnMouseWheel(wxMouseEvent&);

    void OnLeftDown(wxMouseEvent&);
    void OnLeftDClick(wxMouseEvent&);
    void OnMotion(wxMouseEvent&);
    void OnLeftUp(wxMouseEvent&);
    void OnSize(wxSizeEvent&);
    void OnCaptureLost(wxMouseCaptureLostEvent&);

    void ProcessPan(const wxPoint&, bool);
    void ScaleToFit();
    void FinishPan(bool);

    double m_ZoomFactor{};
    int m_BitmapWidth{};
    int m_BitmapHeight{};
    double m_Start_X_Pos{};
    double m_Start_Y_Pos{};

    wxPoint2DDouble m_panVector;
    wxPoint2DDouble m_inProgressPanVector;
    wxPoint m_inProgressPanStartPoint;
    bool m_scaleToFit;
    bool m_panInProgress;
    bool m_FirstPaint{ true };

    wxBitmap m_RawBitmap;
    wxGraphicsBitmap m_DrawBitmap;
};