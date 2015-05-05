#pragma once
#include "afxwin.h"


// CWaitingBox dialog

class CWaitingBox : public CDialogEx
{
	DECLARE_DYNAMIC(CWaitingBox)

public:
	CWaitingBox(CWnd* pParent = NULL);   // standard constructor
	virtual ~CWaitingBox();

// Dialog Data
	enum { IDD = IDD_WAITINGBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	CStatic m_msg;
};
