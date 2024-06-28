
// HebrewLearningDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "HebrewLearning.h"
#include "HebrewLearningDlg.h"
#include "afxdialogex.h"

#include <random>
#include <ctime>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CHebrewLearningDlg dialog

/// @brief Get Random number from range 0 to 1;
/// @return false - Native string true - Translated string
bool getRandInt()
{
	return rand() % 2;
}

void CHebrewLearningDlg::ParseLine(const CString& strLine)
{
	int curPos = 0;
	CString strToken;
	std::vector<CString> vecStrTokens;
	strToken = strLine.Tokenize(L"-",curPos);

	while (strToken[0] != L'\0')
	{
		vecStrTokens.emplace_back(strToken);
		strToken = strLine.Tokenize(L"-", curPos);
	}

	if (vecStrTokens[0] != L'\0' && vecStrTokens[1] != L'\0' && vecStrTokens[2] != L'\0')
		m_vecDictionary.emplace_back(vecStrTokens[0], vecStrTokens[1], vecStrTokens[2]);
}

bool CHebrewLearningDlg::GetLine(CFile& file, CStringA& str)
{
	str = "";
	try
	{
		char ch;
		BOOL bEOL = false;

		while (false == bEOL) 
		{
			if (1 != file.Read(&ch, 1)) 
				return str.GetLength() > 0 ? true : false;

			switch (ch) 
			{
			case '\n': bEOL = true;	break;
			case '\r': break;
			default: str += ch;	break;
			}
		}
		return true;
	}
	catch(...)
	{
		return false;
	}
}

void CHebrewLearningDlg::GetDictionary()
{
	CFile file; // For me it is ugly. Probably need to make an interface from CWldFile?

	const TCHAR szFilter[] = L"Dictionary (*.dic)|*.dic|All Files (*.*)|*.*||";
	CFileDialog dlg(TRUE, _T("dic"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter, this);
	if (dlg.DoModal() == IDOK)
	{
		m_strPath = dlg.GetPathName();
	}

	try
	{
		CFileStatus status;
		if (CFile::GetStatus(m_strPath, status))
		{
			if (file.Open(m_strPath, CFile::modeRead | CFile::typeUnicode))
			{
				////Dealing wih encoding of the document
				//BYTE bom[3] = { 0,0,0 };
				//file.CFile::Read(bom, 3);
				//m_bWithBOM = (bom[0] == 0xEF && bom[1] == 0xBB && bom[2] == 0xBF);
				//if (m_bWithBOM)
				//	m_dwCodePage = CP_UTF8;
				//else
				//{
				//	file.Seek(0, CFile::begin);
				//	m_dwCodePage = CP_ACP;
				//}

				CString strLine;
				CStringA strLineA;
				while (GetLine(file, strLineA))
				{
					strLine = CA2WEX<>(strLineA, CP_UTF8);
					ParseLine(strLine);
				}
				file.Close();
			}
		}
		else
		{
			AfxMessageBox(L"File path could not be null!", MB_OK | MB_ICONERROR);
		}
	}
	catch (CFileException* e) {}

	//randomizing current dictionary
	std::srand(unsigned int(std::time(0)));
	std::random_shuffle(m_vecDictionary.begin(), m_vecDictionary.end());

	m_iCurPos = 0;
	GetNextWord();
}

CHebrewLearningDlg::CHebrewLearningDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_HEBREWLEARNING_DIALOG, pParent)
	, strQustionVal(_T(""))
	, strAnswer(_T(""))
	, strTranscription(_T(""))
	, strResult(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_iCurPos = 0;
	m_strPath = L"";
	bTranslation = false;
}

void CHebrewLearningDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_QUESTION, strQustionVal);
	DDX_Text(pDX, IDC_ANSWER, strAnswer);
	DDX_Text(pDX, IDC_TRANSCRIPTION, strTranscription);
	DDX_Text(pDX, IDC_RESULT, strResult);
}

BEGIN_MESSAGE_MAP(CHebrewLearningDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(ID_CHECK, &CHebrewLearningDlg::OnBnClickedCheck)
	ON_BN_CLICKED(IDC_SKIP, &CHebrewLearningDlg::OnBnClickedSkip)
	ON_BN_CLICKED(IDC_OPEN, &CHebrewLearningDlg::OnBnClickedOpen)
	ON_BN_CLICKED(ID_EXIT, &CHebrewLearningDlg::OnBnClickedExit)
END_MESSAGE_MAP()


// CHebrewLearningDlg message handlers

BOOL CHebrewLearningDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CHebrewLearningDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CHebrewLearningDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

bool CHebrewLearningDlg::GetNextWord()
{
	if (m_iCurPos < m_vecDictionary.size())
	{
		std::tuple<CString, CString, CString>tuple;
		tuple = m_vecDictionary[m_iCurPos];
		bTranslation = getRandInt();
		strQustionVal = bTranslation ? std::get<2>(tuple) : std::get<0>(tuple);
		strTranscription.Empty();
		strAnswer.Empty();
		UpdateData(FALSE);
		return true;
	}
	return false;
}

void CHebrewLearningDlg::OnBnClickedCheck()
{
	if (strQustionVal == L"EOL")
		return;

	UpdateData(TRUE);
	CString strRightAnswer;
	strRightAnswer = bTranslation ? std::get<0>(m_vecDictionary[m_iCurPos]) : std::get<2>(m_vecDictionary[m_iCurPos]);

	//UTF-8 problem CaseSensetive comparision
	if (strAnswer == strRightAnswer)
	{
		OnBnClickedSkip();
		strResult = L"True!";
	}
	else
	{
		strResult = L"FALSE!";
		strTranscription = std::get<1>(m_vecDictionary[m_iCurPos]);
	}
	UpdateData(FALSE);
}


void CHebrewLearningDlg::OnBnClickedSkip()
{
	UpdateData(TRUE);
	++m_iCurPos;
	strTranscription.Empty();
	strAnswer.Empty();
	if (!GetNextWord())
	{
		strQustionVal = L"EOL";
		m_vecDictionary.clear();
	}
	UpdateData(FALSE);
}


void CHebrewLearningDlg::OnBnClickedOpen()
{
	GetDictionary();
	UpdateData(FALSE);
}


void CHebrewLearningDlg::OnBnClickedExit()
{
	PostQuitMessage(0);
}
