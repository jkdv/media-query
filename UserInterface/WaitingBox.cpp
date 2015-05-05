// WaitingBox.cpp : implementation file
//

#include "stdafx.h"
#include "UserInterface.h"
#include "WaitingBox.h"
#include "afxdialogex.h"


// CWaitingBox dialog

IMPLEMENT_DYNAMIC(CWaitingBox, CDialogEx)

CWaitingBox::CWaitingBox(CWnd* pParent /*=NULL*/)
: CDialogEx(CWaitingBox::IDD, pParent)
{
	Create(CWaitingBox::IDD, pParent);
}

CWaitingBox::~CWaitingBox()
{
}

void CWaitingBox::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MESSAGE, m_msg);
}


BEGIN_MESSAGE_MAP(CWaitingBox, CDialogEx)
END_MESSAGE_MAP()


// CWaitingBox message handlers
