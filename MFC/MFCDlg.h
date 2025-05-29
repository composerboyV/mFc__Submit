
#pragma once
#include "CDrawDlg.h"

class CMFCDlg : public CDialogEx
{
private:
	CImage	m_image;
	CDrawDlg* m_pDrawDlg;

public:
	CMFCDlg(CWnd* pParent = nullptr);
	virtual ~CMFCDlg();

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MFC_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

protected:
	HICON m_hIcon;

	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedReset();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnStnClickedSpot1();
	afx_msg void OnStnClickedSpot2();
	afx_msg void OnStnClickedSpot3();
	afx_msg void OnBnClickedRandom();
	afx_msg LRESULT OnUpdateCoordinates(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedApply();
	afx_msg void OnEnChangeEditRadius();
	afx_msg void OnEnChangeEditThickness();
	afx_msg void OnBnClickedBtnExit();
};