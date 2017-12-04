// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__F2BD47C9_EB5D_4CFD_8EB8_B9AF6597E8BF__INCLUDED_)
#define AFX_STDAFX_H__F2BD47C9_EB5D_4CFD_8EB8_B9AF6597E8BF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers
#define _USE_32BIT_TIME_T	// Force 32-bit time_t usage to provide compatibility with VC8.0

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <string>
#include <vector>
#include <algorithm>
#include <sstream>
#include <iostream>
#include <fstream>
#include <random>

#ifdef _WIN32
#define NOMINMAX
#include <Windows.h>
#else
# include <sys/time.h>
#endif

#include <cpprest/json.h>
#include <cpprest/http_listener.h>
#include <cpprest/uri.h>
#include <cpprest/asyncrt_utils.h>
#include <cpprest/json.h>
#include <cpprest/filestream.h>
#include <cpprest/containerstream.h>
#include <cpprest/producerconsumerstream.h>

#pragma warning ( push )
#pragma warning ( disable : 4457 )
#pragma warning ( pop )


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__F2BD47C9_EB5D_4CFD_8EB8_B9AF6597E8BF__INCLUDED_)
