#pragma once
#include "afxdialogex.h"

#include <thread>
#include <chrono>
#include <atomic>


class CDrawDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CDrawDlg)

public:
	CDrawDlg(CWnd* pParent = nullptr);
	virtual ~CDrawDlg();

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CDrawDlg };
#endif

private :
	//이미지관리
	CImage	m_image;
	bool	m_bImageInitialized;

	//점 관리
	CPoint			m_ptClick[3];
	int				m_nClickCount;
	int				m_nRadius;

	CList<CPoint> m_allClickedPoints;

	//원 관련
	CPoint	m_ptCircleCenter;
	int		m_nCircleRadius;
	int		m_nLineThickness;
	
	//드래그
	bool	m_bDragging;
	int		m_nDragPointIndex;

	CPoint	m_ptDragStart;
	int		m_nCurrentRadius;

	//스레드 관리
	std::atomic<bool>	m_bRandomRunning;
	std::atomic<int>	m_nCurrentThreadId;

private :
	//초기화 관련
	void		InitDrawing();


	//드로잉
	void		DrawCirclePoint(CPoint center, int radius);
	void		DrawCircleOutline(CPoint center, int radius, unsigned char color, int thickness);
	void		DrawPixel(unsigned char* fm, int nPitch, int nWidth, int nHeight, int x, int y, unsigned char color);
	void		DrawPixelSafe(unsigned char* fm, int nPitch, int nWidth, int nHeight, int x, int y, unsigned char color);
	void		DrawCircle8Points(unsigned char* fm, int nPitch, int nWidth, int nHeight, int cx, int cy, int x, int y, unsigned char color);

	//수학계산
	bool		CalculateCircleFrom3Points(CPoint p1, CPoint p2, CPoint p3, CPoint& center, int& radius);
	
	//마우스 드래그
	int			GetClickedPointIndex(CPoint point);

	//랜덤이동
	void		StartRandomMovement();
	void		StopRandomMovement();
	void		RandomMovePoints();
	static void RandomThreadProc(CDrawDlg* pThis, int threadId);
	
	//UI 업데이트
	void		UpdateCoordinateDisplay();

	// 좌표 변환 함수
	CPoint		ConvertToScreenCenter(CPoint screenPoint) const;
	CPoint		ConvertFromScreenCenter(CPoint centerPoint) const;

public:
	//외부 인터페이스
	void	ResetDrawing();
	void	SetPointRadius(int radius);
	void	SetLineThickness(int thickness);
	void	TriggerRandomMovement();

	//정보반환
	CPoint	GetClickPoint(int index) const;
	CPoint	GetCircleCenter() const;
	int		GetCircleRadius() const;
	int		GetClickCount() const;
protected:

	virtual void	DoDataExchange(CDataExchange* pDX);
	virtual BOOL	OnInitDialog();
	//메시지 핸들러
	afx_msg void	OnPaint();
	afx_msg void	OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void	OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void	OnMouseMove(UINT nFlags, CPoint point);
	afx_msg LRESULT	OnRandomMove(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};
