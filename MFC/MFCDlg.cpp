
// MFCDlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "MFC.h"
#include "MFCDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

CMFCDlg::CMFCDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MFC_DIALOG, pParent)
	, m_pDrawDlg(nullptr)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CMFCDlg::~CMFCDlg()
{
	if (m_pDrawDlg) {
		delete m_pDrawDlg;
		m_pDrawDlg = nullptr;
	}
}

void CMFCDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMFCDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_Reset, &CMFCDlg::OnBnClickedReset)
	ON_BN_CLICKED(IDCANCEL, &CMFCDlg::OnBnClickedCancel)
	ON_STN_CLICKED(IDC_SPOT3, &CMFCDlg::OnStnClickedSpot3)
	ON_STN_CLICKED(IDC_SPOT2, &CMFCDlg::OnStnClickedSpot2)
	ON_STN_CLICKED(IDC_SPOT1, &CMFCDlg::OnStnClickedSpot1)
	ON_BN_CLICKED(IDC_Random, &CMFCDlg::OnBnClickedRandom)
	ON_MESSAGE(WM_USER + 2, &CMFCDlg::OnUpdateCoordinates)
	ON_BN_CLICKED(IDC_BTN_APPLY, &CMFCDlg::OnBnClickedApply)
	ON_EN_CHANGE(IDC_EDIT_Radius, &CMFCDlg::OnEnChangeEditRadius)
	ON_EN_CHANGE(IDC_EDIT_Thickness, &CMFCDlg::OnEnChangeEditThickness)
	ON_BN_CLICKED(IDC_BTN_EXIT, &CMFCDlg::OnBnClickedBtnExit)
END_MESSAGE_MAP()

BOOL CMFCDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}
	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);

	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);
	MoveWindow(0, 0, screenWidth, screenHeight);

	m_pDrawDlg = new CDrawDlg;
	if (m_pDrawDlg->Create(IDD_CDrawDlg, this)) {
		int dialogWidth = screenWidth * 4 / 5;
		int dialogHeight = screenHeight * 9 / 10;
		m_pDrawDlg->MoveWindow(0, 0, dialogWidth - 20, dialogHeight - 50);
		m_pDrawDlg->ShowWindow(SW_SHOW);
		int rightAreaX = screenWidth * 4 / 5 + 10;
		int rightAreaWidth = screenWidth / 5 - 20;
		int rightAreaHeight = screenHeight - 100;
		int yPos = 10;
		CFont	font;
		font.CreatePointFont(140, _T("맑은 고딕"));


		int labelHeight = 30;
		int editHeight = 35;
		int buttonHeight = 45;
		int spotInfoHeight = (rightAreaHeight - yPos - 300) / 3;
		CWnd* pRadiusLabel = GetDlgItem(IDC_STATIC_RADIUS_LABEL);
		if (pRadiusLabel) {
			pRadiusLabel->MoveWindow(rightAreaX, yPos, rightAreaWidth - 10, labelHeight);
			pRadiusLabel->SetFont(&font);
			yPos += labelHeight + 5;
		}
		CWnd* pRadiusEdit = GetDlgItem(IDC_EDIT_Radius);
		if (pRadiusEdit) {
			pRadiusEdit->MoveWindow(rightAreaX, yPos, rightAreaWidth - 30, editHeight);
			pRadiusEdit->SetFont(&font);
			pRadiusEdit->SetWindowText(_T("5"));
			yPos += editHeight + 15;
		}
		CWnd* pThicknessLabel = GetDlgItem(IDC_STATIC_THICKNESS_LABEL);
		if (pThicknessLabel) {
			pThicknessLabel->MoveWindow(rightAreaX, yPos, rightAreaWidth - 30, labelHeight);
			pThicknessLabel->SetFont(&font);
			yPos += labelHeight + 5;
		}
		CWnd* pThicknessEdit = GetDlgItem(IDC_EDIT_Thickness);
		if (pThicknessEdit) {
			pThicknessEdit->MoveWindow(rightAreaX, yPos, rightAreaWidth - 30, editHeight);
			pThicknessEdit->SetFont(&font);
			pThicknessEdit->SetWindowText(_T("2"));
			yPos += editHeight + 15;
		}
		CWnd* pApplyButton = GetDlgItem(IDC_BTN_APPLY);
		if (pApplyButton) {
			pApplyButton->MoveWindow(rightAreaX, yPos, rightAreaWidth - 30, buttonHeight);
			pApplyButton->SetFont(&font);
			yPos += buttonHeight + 10;
		}
		CWnd* pResetButton = GetDlgItem(IDC_Reset);
		if (pResetButton) {
			pResetButton->MoveWindow(rightAreaX, yPos, rightAreaWidth - 30, buttonHeight);
			pResetButton->SetFont(&font);
			yPos += buttonHeight + 10;
		}
		CWnd* pRandomButton = GetDlgItem(IDC_Random);
		if (pRandomButton) {
			pRandomButton->MoveWindow(rightAreaX, yPos, rightAreaWidth - 30, buttonHeight);
			pRandomButton->SetFont(&font);
			yPos += buttonHeight + 20;
		}
		CWnd* pSpot1 = GetDlgItem(IDC_SPOT1);
		if (pSpot1) {
			pSpot1->MoveWindow(rightAreaX, yPos, rightAreaWidth - 30, spotInfoHeight - 10);
			pSpot1->SetFont(&font);
			pSpot1->SetWindowText(_T("점1: "));
			yPos += spotInfoHeight - 130;
		}
		CWnd* pSpot2 = GetDlgItem(IDC_SPOT2);
		if (pSpot2) {
			pSpot2->MoveWindow(rightAreaX, yPos, rightAreaWidth - 30, spotInfoHeight - 10);
			pSpot2->SetFont(&font);
			pSpot2->SetWindowText(_T("점2: "));
			yPos += spotInfoHeight - 130;
		}
		CWnd* pSpot3 = GetDlgItem(IDC_SPOT3);
		if (pSpot3) {
			pSpot3->MoveWindow(rightAreaX, yPos, rightAreaWidth - 10, spotInfoHeight);
			pSpot3->SetFont(&font);
			pSpot3->SetWindowText(_T("점3: "));
			yPos += spotInfoHeight - 130;
		}

		CWnd* pExitButton = GetDlgItem(IDC_BTN_EXIT);
		if (pExitButton) {
			int exitButtonY = screenHeight - 300;
			pExitButton->MoveWindow(rightAreaX, exitButtonY, rightAreaWidth - 10, buttonHeight);
			pExitButton->SetFont(&font);
		}
		return TRUE;
	}
	return FALSE;
}

void CMFCDlg::OnBnClickedApply()
{
	if (!m_pDrawDlg) return;

	CString strRadius;
	GetDlgItemText(IDC_EDIT_Radius, strRadius);
	int radius = _ttoi(strRadius);

	if (radius < 1) {
		radius = 1;
		SetDlgItemText(IDC_EDIT_Radius, _T("1"));
	}
	if (radius > 50) {
		radius = 50;
		SetDlgItemText(IDC_EDIT_Radius, _T("50"));
	}

	CString strThickness;
	GetDlgItemText(IDC_EDIT_Thickness, strThickness);
	int thickness = _ttoi(strThickness);

	if (thickness < 1) {
		thickness = 1;
		SetDlgItemText(IDC_EDIT_Thickness, _T("1"));
	}
	if (thickness > 10) {
		thickness = 10;
		SetDlgItemText(IDC_EDIT_Thickness, _T("10"));
	}
	m_pDrawDlg->SetPointRadius(radius);
	m_pDrawDlg->SetLineThickness(thickness);

	CString msg;
	msg.Format(_T("설정완료."));
	AfxMessageBox(msg);
}
void CMFCDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

void CMFCDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this);

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
		CDialogEx::OnPaint();
}

HCURSOR CMFCDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CMFCDlg::OnBnClickedReset()
{
	if (m_pDrawDlg) {
		m_pDrawDlg->ResetDrawing();
	}
}

void CMFCDlg::OnBnClickedCancel()
{
	CDialogEx::OnCancel();
}

void CMFCDlg::OnStnClickedSpot3()
{
	if (m_pDrawDlg && m_pDrawDlg->GetClickCount() >= 3) {
		CPoint pt = m_pDrawDlg->GetClickPoint(2);
		CPoint center = m_pDrawDlg->GetCircleCenter();
		int radius = m_pDrawDlg->GetCircleRadius();
	}
}

void CMFCDlg::OnStnClickedSpot2()
{
	if (m_pDrawDlg && m_pDrawDlg->GetClickCount() >= 2)
		CPoint pt = m_pDrawDlg->GetClickPoint(1);
}

void CMFCDlg::OnStnClickedSpot1()
{
	if (m_pDrawDlg && m_pDrawDlg->GetClickCount() >= 1)
		CPoint pt = m_pDrawDlg->GetClickPoint(0);
}

void CMFCDlg::OnBnClickedRandom()
{
	if (m_pDrawDlg) {
		m_pDrawDlg->TriggerRandomMovement();
	}
}

LRESULT CMFCDlg::OnUpdateCoordinates(WPARAM wParam, LPARAM lParam)
{
	if (m_pDrawDlg && m_pDrawDlg->GetClickCount() >= 3) {
		CPoint pt1 = m_pDrawDlg->GetClickPoint(0);
		CPoint pt2 = m_pDrawDlg->GetClickPoint(1);
		CPoint pt3 = m_pDrawDlg->GetClickPoint(2);
		CPoint center = m_pDrawDlg->GetCircleCenter();
		int radius = m_pDrawDlg->GetCircleRadius();

		CString str1, str2, str3;
		str1.Format(_T("점1: (%d, %d)"), pt1.x, pt1.y);
		str2.Format(_T("점2: (%d, %d)"), pt2.x, pt2.y);
		str3.Format(_T("점3: (%d, %d)\r\n중심: (%d, %d)\r\n반지름: %d"), pt3.x, pt3.y, center.x, center.y, radius);

		SetDlgItemText(IDC_SPOT1, str1);
		SetDlgItemText(IDC_SPOT2, str2);
		SetDlgItemText(IDC_SPOT3, str3);
	}
	else {
		SetDlgItemText(IDC_SPOT1, _T("점1: "));
		SetDlgItemText(IDC_SPOT2, _T("점2: "));
		SetDlgItemText(IDC_SPOT3, _T("점3: "));
	}
	return 0;
}

void CMFCDlg::OnEnChangeEditRadius()
{
	CString str;
	GetDlgItemText(IDC_EDIT_Radius, str);

	if (!str.IsEmpty()) {
		int value = _ttoi(str);
		if (value < 1 || value > 50) {
			GetDlgItem(IDC_STATIC_RADIUS_LABEL)->SetWindowText(_T("기준점 반지름 (1-50 사이 값 입력해야 합니다 !)"));
		}
		else {
			GetDlgItem(IDC_STATIC_RADIUS_LABEL)->SetWindowText(_T("기준점 반지름: "));
		}
	}
}

void CMFCDlg::OnEnChangeEditThickness()
{
	CString str;
	GetDlgItemText(IDC_EDIT_Thickness, str);

	if (!str.IsEmpty()) {
		int value = _ttoi(str);
		if (value < 1 || value > 10) {
			GetDlgItem(IDC_STATIC_THICKNESS_LABEL)->SetWindowText(_T("선 두께 (1-10 사이 값 필요합니다!)"));
		}
		else {
			GetDlgItem(IDC_STATIC_THICKNESS_LABEL)->SetWindowText(_T("선 두께: "));
		}
	}
}

void CMFCDlg::OnBnClickedBtnExit()
{
	OnCancel();
}