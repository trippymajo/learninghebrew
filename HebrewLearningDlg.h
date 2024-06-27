
// HebrewLearningDlg.h : header file
//

#pragma once
#include <vector>
#include <tuple>

// CHebrewLearningDlg dialog
class CHebrewLearningDlg : public CDialogEx
{
// Construction
public:
	CHebrewLearningDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_HEBREWLEARNING_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


private:
	unsigned int	m_iCurPos;
	//bool					m_bWithBOM;
	CString				m_strPath;
	//DWORD					m_dwCodePage;
	std::vector<std::tuple<CString, CString, CString>>	m_vecDictionary;

	bool GetNextWord();
	bool GetLine(CFile& file, CStringA& str);
	void GetDictionary();
	void ParseLine(const CString& strLine);

	// Implementation
protected:
	HICON m_hIcon;


	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	bool		bTranslation;
	CString strQustionVal;
	CString strAnswer;
	CString strTranscription;
	CString strResult;
	afx_msg void OnBnClickedCheck();
	afx_msg void OnBnClickedSkip();
	afx_msg void OnBnClickedOpen();
};
