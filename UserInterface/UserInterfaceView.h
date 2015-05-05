
// UserInterfaceView.h : interface of the CUserInterfaceView class
//

#pragma once

#include "resource.h"
#include "afxeditbrowsectrl.h"
#include "afxcmn.h"
#include "afxbutton.h"
#include "Direct2DControl.h"
#include "WaitingBox.h"

class CUserInterfaceView : public CFormView
{
protected: // create from serialization only
	CUserInterfaceView();
	DECLARE_DYNCREATE(CUserInterfaceView)

public:
	enum{ IDD = IDD_USERINTERFACE_FORM };

// Attributes
public:
	CUserInterfaceDoc* GetDocument() const;

// Operations
public:

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnInitialUpdate(); // called first time after construct

// Implementation
public:
	virtual ~CUserInterfaceView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	DECLARE_MESSAGE_MAP()
	CMFCEditBrowseCtrl m_editBrowserInput;
	CSliderCtrl m_sliderInput;
	CSliderCtrl m_sliderOutput;
	CMFCButton m_buttonPlayInput;
	CMFCButton m_buttonStopInput;
	CMFCButton m_buttonPlayOutput;
	CMFCButton m_buttonStopOutput;
	CMFCButton m_buttonSearch;
	CDirect2DControl m_picInput;
	CDirect2DControl m_picOutput;
	CDirect2DControl m_picScore;
	CListCtrl m_listResult;
	Database m_db;
	Video m_inputVideo;
	Tree m_tree;

public:
	void Query();
	void PlayInputVideo();
	void PauseInputVideo();
	void StopInputVideo();
	void PlayOutputVideo();
	void PauseOutputVideo();
	void StopOutputVideo();
	void SetListItem(LPCTSTR lpszItem, LPCTSTR lpszText);

private:
	string to_string(CString cs) {
		CT2CA pszConvertedAnsiString(cs);
		std::string strStd(pszConvertedAnsiString);
		return strStd;
	}

	inline CString to_CString(string sz) {
		return CString(sz.c_str());
	}


// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnChangeEditBrowserInput();
	afx_msg void OnBnClickedButtonSearch();
	afx_msg void OnBnClickedButtonPlayInput();
	afx_msg void OnBnClickedButtonStopInput();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg LRESULT OnUpdatedFrame(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnEndOfFrame(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedButtonPlayOutput();
	afx_msg void OnBnClickedButtonStopOutput();
	afx_msg void OnLvnItemchangedListOutput(NMHDR *pNMHDR, LRESULT *pResult);
};

#ifndef _DEBUG  // debug version in UserInterfaceView.cpp
inline CUserInterfaceDoc* CUserInterfaceView::GetDocument() const
   { return reinterpret_cast<CUserInterfaceDoc*>(m_pDocument); }
#endif

