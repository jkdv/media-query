
// UserInterfaceView.cpp : implementation of the CUserInterfaceView class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "UserInterface.h"
#endif

#include "UserInterfaceDoc.h"
#include "UserInterfaceView.h"

#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CUserInterfaceView

IMPLEMENT_DYNCREATE(CUserInterfaceView, CFormView)

BEGIN_MESSAGE_MAP(CUserInterfaceView, CFormView)
	ON_WM_CREATE()
	ON_EN_CHANGE(IDC_EDIT_BROWSER_INPUT, &CUserInterfaceView::OnChangeEditBrowserInput)
	ON_BN_CLICKED(IDC_BUTTON_SEARCH, &CUserInterfaceView::OnBnClickedButtonSearch)
	ON_BN_CLICKED(IDC_BUTTON_PLAY_INPUT, &CUserInterfaceView::OnBnClickedButtonPlayInput)
	ON_BN_CLICKED(IDC_BUTTON_STOP_INPUT, &CUserInterfaceView::OnBnClickedButtonStopInput)
	ON_MESSAGE(UWM_UPDATED_FRAME, &CUserInterfaceView::OnUpdatedFrame)
	ON_WM_HSCROLL()
	ON_MESSAGE(UWM_END_OF_FRAME, &CUserInterfaceView::OnEndOfFrame)
	ON_BN_CLICKED(IDC_BUTTON_PLAY_OUTPUT, &CUserInterfaceView::OnBnClickedButtonPlayOutput)
	ON_BN_CLICKED(IDC_BUTTON_STOP_OUTPUT, &CUserInterfaceView::OnBnClickedButtonStopOutput)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_OUTPUT, &CUserInterfaceView::OnLvnItemchangedListOutput)
END_MESSAGE_MAP()

// CUserInterfaceView construction/destruction

CUserInterfaceView::CUserInterfaceView()
	: CFormView(CUserInterfaceView::IDD)
{
}

CUserInterfaceView::~CUserInterfaceView()
{
}

void CUserInterfaceView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_BROWSER_INPUT, m_editBrowserInput);
	DDX_Control(pDX, IDC_SLIDER_INPUT, m_sliderInput);
	DDX_Control(pDX, IDC_SLIDER_OUTPUT, m_sliderOutput);
	DDX_Control(pDX, IDC_BUTTON_PLAY_INPUT, m_buttonPlayInput);
	DDX_Control(pDX, IDC_BUTTON_STOP_INPUT, m_buttonStopInput);
	DDX_Control(pDX, IDC_BUTTON_PLAY_OUTPUT, m_buttonPlayOutput);
	DDX_Control(pDX, IDC_BUTTON_STOP_OUTPUT, m_buttonStopOutput);
	DDX_Control(pDX, IDC_PIC_INPUT, m_picInput);
	DDX_Control(pDX, IDC_PIC_OUTPUT, m_picOutput);
	DDX_Control(pDX, IDC_PIC_SCORE, m_picScore);
	DDX_Control(pDX, IDC_LIST_OUTPUT, m_listResult);
	DDX_Control(pDX, IDC_BUTTON_SEARCH, m_buttonSearch);
}

BOOL CUserInterfaceView::PreCreateWindow(CREATESTRUCT& cs)
{
	return CFormView::PreCreateWindow(cs);
}

void CUserInterfaceView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit(FALSE);
	ResizeParentToFit(TRUE);

	m_listResult.InsertColumn(0, L"File name", LVCFMT_LEFT);
	m_listResult.InsertColumn(1, L"Similarity", LVCFMT_RIGHT);

	CDC* cdc = GetDC();
	HDC hdc = cdc->GetSafeHdc();
	int dpiX = GetDeviceCaps(hdc, LOGPIXELSX);
	int dpiY = GetDeviceCaps(hdc, LOGPIXELSY);

	m_listResult.SetColumnWidth(0, (90 * dpiX) / 96);
	m_listResult.SetColumnWidth(1, (60 * dpiX) / 96);

	m_db.read();
	m_db.indexMotion();
}


void CUserInterfaceView::Query()
{
	m_db.makeTree2(m_inputVideo);
	m_tree = m_db.getTree();
	map<string, double> ranks = m_tree.getRanksByMax();

	m_picScore.RemoveBitmap();
	m_picOutput.RemoveBitmap();

	m_buttonPlayOutput.EnableWindow(FALSE);
	m_sliderOutput.EnableWindow(FALSE);

	// Interpolate data in the list
	m_listResult.DeleteAllItems();
	for (auto item : ranks) {
		CString file(m_db.convertToNickname(item.first).c_str());
		SetListItem(file, CString(format("%f", item.second * 100).c_str()));
	}

	// Sort the list
	m_listResult.SortItemsEx([](LPARAM lParam1, LPARAM lParam2,
		LPARAM lParamSort)
	{
		CListCtrl* pListCtrl = (CListCtrl*)lParamSort;
		CString    strItem1 = pListCtrl->GetItemText(static_cast<int>(lParam1), 1);
		CString    strItem2 = pListCtrl->GetItemText(static_cast<int>(lParam2), 1);

		double d1 = _tstof(strItem1.GetBuffer());
		double d2 = _tstof(strItem2.GetBuffer());

		return (int)(d2 * 100 - d1 * 100);
	}, (DWORD_PTR)&m_listResult);

	AfxMessageBox(L"Done.", MB_ICONINFORMATION | MB_OK);
}


void CUserInterfaceView::SetListItem(LPCTSTR lpszItem, LPCTSTR lpszText)
{
	CDC* cdc = GetDC();
	HDC hdc = cdc->GetSafeHdc();
	int dpiX = GetDeviceCaps(hdc, LOGPIXELSX);
	int dpiY = GetDeviceCaps(hdc, LOGPIXELSY);

	int count = m_listResult.GetItemCount();
	m_listResult.SetColumnWidth(0, (85 * dpiX) / 96);
	m_listResult.SetColumnWidth(1, (65 * dpiX) / 96);
	m_listResult.InsertItem(count, lpszItem);
	m_listResult.SetItemText(count, 1, lpszText);
}


// Buttons for input video

void CUserInterfaceView::PlayInputVideo()
{
	CString label;
	m_buttonPlayInput.GetWindowTextW(label);

	label = "Pause";
	m_buttonPlayInput.SetWindowTextW(label);
	m_buttonStopInput.EnableWindow(TRUE);

	m_picInput.Play();
}

void CUserInterfaceView::PauseInputVideo()
{
	CString label;
	m_buttonPlayInput.GetWindowTextW(label);

	label = "Play";
	m_buttonPlayInput.SetWindowTextW(label);
	m_buttonStopInput.EnableWindow(FALSE);

	m_picInput.Pause();
}

void CUserInterfaceView::StopInputVideo()
{
	CString label = L"Play";
	m_buttonPlayInput.SetWindowTextW(label);
	m_buttonStopInput.EnableWindow(FALSE);

	m_picInput.Stop();
}


// Buttons for output video

void CUserInterfaceView::PlayOutputVideo()
{
	CString label;
	m_buttonPlayOutput.GetWindowTextW(label);

	label = "Pause";
	m_buttonPlayOutput.SetWindowTextW(label);
	m_buttonStopOutput.EnableWindow(TRUE);

	m_picOutput.Play();
}

void CUserInterfaceView::PauseOutputVideo()
{
	CString label;
	m_buttonPlayOutput.GetWindowTextW(label);

	label = "Play";
	m_buttonPlayOutput.SetWindowTextW(label);
	m_buttonStopOutput.EnableWindow(FALSE);

	m_picOutput.Pause();
}

void CUserInterfaceView::StopOutputVideo()
{
	CString label = L"Play";
	m_buttonPlayOutput.SetWindowTextW(label);
	m_buttonStopOutput.EnableWindow(FALSE);

	m_picOutput.Stop();
}

// CUserInterfaceView diagnostics

#ifdef _DEBUG
void CUserInterfaceView::AssertValid() const
{
	CFormView::AssertValid();
}

void CUserInterfaceView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CUserInterfaceDoc* CUserInterfaceView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CUserInterfaceDoc)));
	return (CUserInterfaceDoc*)m_pDocument;
}
#endif //_DEBUG


// CUserInterfaceView message handlers


int CUserInterfaceView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFormView::OnCreate(lpCreateStruct) == -1)
		return -1;
	return 0;
}


void CUserInterfaceView::OnChangeEditBrowserInput()
{
	// Stop video first
	StopInputVideo();
	StopOutputVideo();

	// If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CFormView::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	CString path;
	m_editBrowserInput.GetWindowTextW(path);

	// Convert char type
	CT2CA pszConvertedAnsiString(path);
	std::string strStd(pszConvertedAnsiString);
	size_t found = strStd.find_last_of("/\\");
	strStd += "\\" + strStd.substr(found + 1);

	// Load video
	m_inputVideo = Video(Database::WIDTH, Database::HEIGHT, strStd);
	bool bRead = m_inputVideo.readUntilExist();

	// If not a video
	if (!bRead) {
		AfxMessageBox(L"It seems to be not a video file.", MB_OK | MB_ICONSTOP);
		return;
	}

	m_picScore.RemoveBitmap();
	m_picOutput.RemoveBitmap();

	m_buttonPlayOutput.EnableWindow(FALSE);
	m_sliderOutput.EnableWindow(FALSE);
	m_picInput.InsertVideo(&m_inputVideo);
	m_buttonPlayInput.EnableWindow(TRUE);
	m_buttonSearch.EnableWindow(TRUE);
	m_sliderInput.EnableWindow(TRUE);
	m_sliderInput.SetRange(0, m_inputVideo.size() - 1, TRUE);
	m_listResult.DeleteAllItems();
}


// Search !

void CUserInterfaceView::OnBnClickedButtonSearch()
{
	StopInputVideo();
	StopOutputVideo();
	Query();
}


// Input video controllers

void CUserInterfaceView::OnBnClickedButtonPlayInput()
{
	CString label;
	m_buttonPlayInput.GetWindowTextW(label);

	if (label == "Play") {
		PlayInputVideo();
	}
	else if (label == "Pause") {
		PauseInputVideo();
	}
}


void CUserInterfaceView::OnBnClickedButtonStopInput()
{
	StopInputVideo();
}


// Output video controllers

void CUserInterfaceView::OnBnClickedButtonPlayOutput()
{
	CString label;
	m_buttonPlayOutput.GetWindowTextW(label);

	if (label == "Play") {
		PlayOutputVideo();
	}
	else if (label == "Pause") {
		PauseOutputVideo();
	}
}


void CUserInterfaceView::OnBnClickedButtonStopOutput()
{
	StopOutputVideo();
}


/**
 * This message is sent by Direct2DControl
 */
afx_msg LRESULT CUserInterfaceView::OnUpdatedFrame(WPARAM wParam, LPARAM lParam)
{
	UINT frame = (UINT)wParam;
	HWND hWnd = (HWND)lParam;
	if (hWnd == m_picInput.GetSafeHwnd())
		m_sliderInput.SetPos(frame);
	if (hWnd == m_picOutput.GetSafeHwnd())
		m_sliderOutput.SetPos(frame);
	return 0;
}


void CUserInterfaceView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if (pScrollBar->GetSafeHwnd() == m_sliderInput.GetSafeHwnd()) {
		// Change current frame to draw a new frame
		PauseInputVideo();
		int pos = m_sliderInput.GetPos();
		m_picInput.ChangeCurrentFrame(pos);
	}
	if (pScrollBar->GetSafeHwnd() == m_sliderOutput.GetSafeHwnd()) {
		// Change current frame to draw a new frame
		PauseOutputVideo();
		int pos = m_sliderOutput.GetPos();
		m_picOutput.ChangeCurrentFrame(pos);
	}
	CFormView::OnHScroll(nSBCode, nPos, pScrollBar);
}


/**
* This message is sent by Direct2DControl
*/
afx_msg LRESULT CUserInterfaceView::OnEndOfFrame(WPARAM wParam, LPARAM lParam)
{
	HWND hwnd = (HWND)lParam;
	if (hwnd == m_picInput) {
		CString label = L"Play";
		m_buttonPlayInput.SetWindowTextW(label);
		m_buttonStopInput.EnableWindow(FALSE);
		m_picInput.ChangeCurrentFrame(0);
	}
	else if (hwnd == m_picOutput) {
		CString label = L"Play";
		m_buttonPlayOutput.SetWindowTextW(label);
		m_buttonStopOutput.EnableWindow(FALSE);
		m_picOutput.ChangeCurrentFrame(0);
	}
	return 0;
}


void CUserInterfaceView::OnLvnItemchangedListOutput(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	CString nickname = m_listResult.GetItemText(pNMLV->iItem, 0);
	string fullname = m_db.convertToFullname(to_string(nickname));

	if ((pNMLV->uChanged & LVIF_STATE)
		&& (pNMLV->uNewState & LVNI_SELECTED))
	{
		// Output the video
		Video* pVideo = &m_db.videoAt(fullname);
		m_picOutput.InsertVideo(pVideo);
		m_buttonPlayOutput.EnableWindow(TRUE);
		m_sliderOutput.EnableWindow(TRUE);
		m_sliderOutput.SetRange(0, pVideo->size() - 1, TRUE);

		// Show the best frame
		int nBestFrame = m_tree.locateMaxChild(fullname);
		StopOutputVideo();
		m_picOutput.ChangeCurrentFrame(nBestFrame);

		// Draw histogram
		double* dHist = new double[pVideo->size()];
		double dNow;
		size_t nCount = m_tree.getChildrenCount(fullname);

		for (int i = 0; i < pVideo->size(); ++i) {
			if (i < nCount) {
				dNow = m_tree.getChild(fullname, i);
				dHist[i] = dNow * 100.0;
			}
			else {
				dHist[i] = 0.0;
			}
		}

		m_picScore.DrawHistogram(pVideo->size(), dHist);
		delete[] dHist;
	}

	*pResult = 0;
}
