
// UserInterface.h : main header file for the UserInterface application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CUserInterfaceApp:
// See UserInterface.cpp for the implementation of this class
//

class CUserInterfaceApp : public CWinAppEx
{
public:
	CUserInterfaceApp();


// Overrides
public:
	virtual BOOL InitInstance();

// Implementation
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CUserInterfaceApp theApp;
