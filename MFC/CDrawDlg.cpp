// CDrawDlg.cpp: 구현 파일
//

#include "pch.h"
#include "MFC.h"
#include "afxdialogex.h"
#include "CDrawDlg.h"


// CDrawDlg 대화 상자

IMPLEMENT_DYNAMIC(CDrawDlg, CDialogEx)

CDrawDlg::CDrawDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CDrawDlg, pParent)
	,m_nClickCount(0)
	,m_nRadius(5) 
	,m_bImageInitialized(false)
    , m_nLineThickness(2)
    , m_nCurrentRadius(0)
    , m_bDragging(false)
    , m_nDragPointIndex(-1)
    , m_ptCircleCenter(0, 0)
    , m_nCircleRadius(0)
    , m_bRandomRunning(false)
    , m_nCurrentThreadId(0)
{

}

CDrawDlg::~CDrawDlg()
{
    m_bRandomRunning.store(false);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

void CDrawDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CDrawDlg, CDialogEx)
    ON_WM_PAINT()
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONUP()
    ON_WM_MOUSEMOVE()
    ON_MESSAGE(WM_USER + 1, &CDrawDlg::OnRandomMove)
END_MESSAGE_MAP()

BOOL CDrawDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();
    InitDrawing();
    return TRUE;
}

void CDrawDlg::InitDrawing()
{
    if (!m_image.IsNull())
        m_image.Destroy();
    int nWidth = GetSystemMetrics(SM_CXSCREEN) * 8/10;
    int nHeight = GetSystemMetrics(SM_CYSCREEN) * 9/10;
    int nBpp = 8;

    m_image.Create(nWidth, -nHeight, nBpp);

    if (nBpp == 8) {
        static RGBQUAD rgb[256];
        for (int i = 0; i < 256; i++)
            rgb[i].rgbRed = rgb[i].rgbGreen = rgb[i].rgbBlue = i;
        m_image.SetColorTable(0, 256, rgb);
    }
    unsigned char* fm = (unsigned char*)m_image.GetBits();
    int nPitch = m_image.GetPitch();
    memset(fm, 0xff, nPitch * nHeight);
    m_bImageInitialized = true;
    m_nClickCount = 0;
    m_allClickedPoints.RemoveAll();
    Invalidate(FALSE);
}

void CDrawDlg::OnPaint()
{
    CPaintDC dc(this);

    if (m_bImageInitialized && !m_image.IsNull())
        m_image.Draw(dc, 0, 0);
}

void CDrawDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
    if (m_nClickCount >= 3) {
        int pointIndex = GetClickedPointIndex(point);
        if (pointIndex != -1) {
            m_bDragging = true;
            m_nDragPointIndex = pointIndex;
            return;
        }
        else {
            DrawCirclePoint(point, m_nRadius);
            m_allClickedPoints.AddTail(point);
            Invalidate(FALSE);
            return;
        }
    }
    if (m_nClickCount < 3) {
        m_ptClick[m_nClickCount] = point;
        DrawCirclePoint(point, m_nRadius);
        m_allClickedPoints.AddTail(point);
        m_nClickCount++;

        if (m_nClickCount == 3) {
            if (CalculateCircleFrom3Points(m_ptClick[0], m_ptClick[1], m_ptClick[2], m_ptCircleCenter, m_nCircleRadius)) {
                DrawCircleOutline(m_ptCircleCenter, m_nCircleRadius, 0x00, m_nLineThickness);
                UpdateCoordinateDisplay();
            }
        }
        Invalidate(FALSE);
    }
    else {
        DrawCirclePoint(point, m_nRadius);
        Invalidate(FALSE);
    }
    CDialogEx::OnLButtonDown(nFlags, point);
}

void CDrawDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
    if (m_bDragging) {
        m_bDragging = false;
        m_nDragPointIndex = -1;
        UpdateCoordinateDisplay();
    }
    CDialogEx::OnLButtonUp(nFlags, point);
}

void CDrawDlg::OnMouseMove(UINT nFlags, CPoint point)
{
    if (m_bDragging && m_nClickCount >= 3 && m_nDragPointIndex >= 0) {
        m_ptClick[m_nDragPointIndex] = point;

        unsigned char* fm = (unsigned char*)m_image.GetBits();
        int nPitch = m_image.GetPitch();
        int nHeight = abs(m_image.GetHeight());
        memset(fm, 0xff, nPitch * nHeight);
        for (int i = 0; i < 3; i++)
            DrawCirclePoint(m_ptClick[i], m_nRadius);

        POSITION pos = m_allClickedPoints.GetHeadPosition();
        int count = 0;
        while (pos) {
            CPoint additionalPoint = m_allClickedPoints.GetNext(pos);

            // 첫 3개 점은 이미 그렸으므로 스킵
            if (count >= 3) {
                DrawCirclePoint(additionalPoint, m_nRadius);
            }
            count++;
        }

        CPoint newCenter;
        int newRadius;
        if (CalculateCircleFrom3Points(m_ptClick[0], m_ptClick[1], m_ptClick[2],
            newCenter, newRadius)) {
            m_ptCircleCenter = newCenter;
            m_nCircleRadius = newRadius;
            DrawCircleOutline(m_ptCircleCenter, m_nCircleRadius, 0x00, m_nLineThickness);
        }
        UpdateCoordinateDisplay();
        Invalidate(FALSE);
    }

    CDialogEx::OnMouseMove(nFlags, point);
}

void CDrawDlg::DrawCirclePoint(CPoint center, int radius)
{
    if (!m_bImageInitialized || m_image.IsNull()) return;

    unsigned char* fm = (unsigned char*)m_image.GetBits();
    int nPitch = m_image.GetPitch();
    int nWidth = m_image.GetWidth();
    int nHeight = abs(m_image.GetHeight());

    for (int y = -radius; y <= radius; y++) {
        for (int x = -radius; x <= radius; x++) {
            if (x * x + y * y <= radius * radius) {
                DrawPixel(fm, nPitch, nWidth, nHeight,
                    center.x + x, center.y + y, 0x00);
            }
        }
    }
}

void CDrawDlg::DrawCircleOutline(CPoint center, int radius, unsigned char color, int thickness)
{
    if (!m_bImageInitialized || m_image.IsNull() || radius <= 0) return;

    unsigned char* fm = (unsigned char*)m_image.GetBits();
    int nPitch = abs(m_image.GetPitch());
    int nWidth = m_image.GetWidth();
    int nHeight = abs(m_image.GetHeight());

    for (int t = 0; t < thickness; t++) {
        int r = radius - t;
        if (r <= 0) break;

        int x = 0;
        int y = r;
        int d = 3 - 2 * r;

        DrawCircle8Points(fm, nPitch, nWidth, nHeight, center.x, center.y, x, y, color);

        while (y >= x) {
            x++;
            if (d > 0)
                d = d + 4 * (x - --y) + 10;
            else 
                d = d + 4 * x + 6;
            DrawCircle8Points(fm, nPitch, nWidth, nHeight, center.x, center.y, x, y, color);
        }
    }
}

void CDrawDlg::DrawPixel(unsigned char* fm, int nPitch, int nWidth, int nHeight,
    int x, int y, unsigned char color)
{
    if (x < 0 || x >= nWidth || y < 0 || y >= nHeight) return;

    fm[y * abs(nPitch) + x] = color;
}

void CDrawDlg::DrawCircle8Points(unsigned char* fm, int nPitch, int nWidth, int nHeight, int cx, int cy, int x, int y, unsigned char color)
{
    DrawPixelSafe(fm, nPitch, nWidth, nHeight, cx + x, cy + y, color);
    DrawPixelSafe(fm, nPitch, nWidth, nHeight, cx - x, cy + y, color);
    DrawPixelSafe(fm, nPitch, nWidth, nHeight, cx + x, cy - y, color);
    DrawPixelSafe(fm, nPitch, nWidth, nHeight, cx - x, cy - y, color);
    DrawPixelSafe(fm, nPitch, nWidth, nHeight, cx + y, cy + x, color);
    DrawPixelSafe(fm, nPitch, nWidth, nHeight, cx - y, cy + x, color);
    DrawPixelSafe(fm, nPitch, nWidth, nHeight, cx + y, cy - x, color);
    DrawPixelSafe(fm, nPitch, nWidth, nHeight, cx - y, cy - x, color);
}

void CDrawDlg::DrawPixelSafe(unsigned char* fm, int nPitch, int nWidth, int nHeight,
    int x, int y, unsigned char color)
{
    if (x >= 0 && x < nWidth && y >= 0 && y < nHeight)
        fm[y * nPitch + x] = color;
}

bool CDrawDlg::CalculateCircleFrom3Points(CPoint p1, CPoint p2, CPoint p3, CPoint& center, int& radius)
{
    double d = (p1.x * (p2.y - p3.y) + p2.x * (p3.y - p1.y) + p3.x * (p1.y - p2.y));
    if (fabs(d) < 1e-6) return false;

    double x1 = p1.x, y1 = p1.y;
    double x2 = p2.x, y2 = p2.y;
    double x3 = p3.x, y3 = p3.y;

    double s1 = x1 * x1 + y1 * y1;
    double s2 = x2 * x2 + y2 * y2;
    double s3 = x3 * x3 + y3 * y3;

    double a = x1 * (y2 - y3) - y1 * (x2 - x3) + x2 * y3 - x3 * y2;
    double b = s1 * (y3 - y2) + s2 * (y1 - y3) + s3 * (y2 - y1);
    double c = s1 * (x2 - x3) + s2 * (x3 - x1) + s3 * (x1 - x2);

    center.x = static_cast<int>(-b / (2 * a));
    center.y = static_cast<int>(-c / (2 * a));

    double dx = x1 - center.x;
    double dy = y1 - center.y;
    radius = static_cast<int>(sqrt(dx * dx + dy * dy));

    return true;
}

int CDrawDlg::GetClickedPointIndex(CPoint point)
{
    const int CLICK_TOLERANCE = 15;

    for (int i = 0; i < m_nClickCount && i < 3; i++) {
        int dx = point.x - m_ptClick[i].x;
        int dy = point.y - m_ptClick[i].y;
        int distance = static_cast<int>(sqrt(dx * dx + dy * dy));

        if (distance <= CLICK_TOLERANCE)
            return i;
    }
    return -1;
}

void CDrawDlg::UpdateCoordinateDisplay()
{
    if (GetParent())
        GetParent()->PostMessage(WM_USER + 2);
}

void CDrawDlg::ResetDrawing()
{
    m_nClickCount = 0;
    m_bDragging = false;
    m_nDragPointIndex = -1;
    m_ptCircleCenter = CPoint(0, 0);
    m_nCircleRadius = 0;
    
    m_allClickedPoints.RemoveAll();
    StopRandomMovement();
    InitDrawing();
    UpdateCoordinateDisplay();
}

void CDrawDlg::StartRandomMovement()
{
    m_bRandomRunning.store(false);

    int newThreadId = m_nCurrentThreadId.fetch_add(1) + 1;
    m_bRandomRunning.store(true);

    std::thread randomThread(RandomThreadProc, this, newThreadId);
    randomThread.detach();
}

void CDrawDlg::StopRandomMovement()
{
    m_bRandomRunning.store(false);
}

void CDrawDlg::RandomThreadProc(CDrawDlg* pThis, int threadId)
{
    if (!pThis) return;

    srand(static_cast<unsigned>(time(nullptr)) + threadId);

    int count = 0;
    while (count < 10) {
        if (!::IsWindow(pThis->GetSafeHwnd()))
            break;

        if (!pThis->m_bRandomRunning.load())
            break;

        if (threadId != pThis->m_nCurrentThreadId.load())
            break;
        pThis->PostMessage(WM_USER + 1);
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        count++;
    }
    if (threadId == pThis->m_nCurrentThreadId.load())
        pThis->m_bRandomRunning.store(false);
}

void CDrawDlg::RandomMovePoints()
{
    CRect rect;
    GetClientRect(&rect);

    for (int i = 0; i < 3; i++) {
        m_ptClick[i].x = rand() % rect.Width();
        m_ptClick[i].y = rand() % rect.Height();
    }
    m_nClickCount = 3;

    unsigned char* fm = (unsigned char*)m_image.GetBits();
    int nPitch = m_image.GetPitch();
    int nHeight = abs(m_image.GetHeight());
    memset(fm, 0xff, nPitch * nHeight);

    for (int i = 0; i < 3; i++)
        DrawCirclePoint(m_ptClick[i], m_nRadius);

    if (CalculateCircleFrom3Points(m_ptClick[0], m_ptClick[1], m_ptClick[2], m_ptCircleCenter, m_nCircleRadius))
        DrawCircleOutline(m_ptCircleCenter, m_nCircleRadius, 0x00, m_nLineThickness);

    UpdateCoordinateDisplay();
    Invalidate(FALSE);
}

LRESULT CDrawDlg::OnRandomMove(WPARAM wParam, LPARAM lParam)
{
    RandomMovePoints();
    return 0;
}

CPoint CDrawDlg::GetClickPoint(int index) const
{
    if (index >= 0 && index < 3 && index < m_nClickCount)
        return ConvertToScreenCenter(m_ptClick[index]);
    return CPoint();
}

CPoint CDrawDlg::ConvertToScreenCenter(CPoint screenPoint) const
{
    CRect rect;
    GetClientRect(&rect);

    int centerX = rect.Width() / 2;
    int centerY = rect.Height() / 2;

    CPoint centerPoint;
    centerPoint.x = screenPoint.x - centerX;
    centerPoint.y = centerY - screenPoint.y;

    return centerPoint;
}

CPoint CDrawDlg::ConvertFromScreenCenter(CPoint centerPoint) const
{
    CRect rect;
    GetClientRect(&rect);

    int centerX = rect.Width() / 2;
    int centerY = rect.Height() / 2;

    CPoint screenPoint;
    screenPoint.x = centerPoint.x + centerX;
    screenPoint.y = centerY - centerPoint.y;

    return screenPoint;
}

CPoint CDrawDlg::GetCircleCenter() const
{
    return ConvertToScreenCenter(m_ptCircleCenter);
}

void CDrawDlg::SetPointRadius(int radius)
{
    m_nRadius = radius;
}

void CDrawDlg::SetLineThickness(int thickness)
{
    m_nLineThickness = thickness;
}

void CDrawDlg::TriggerRandomMovement()
{
    StartRandomMovement();
}

int CDrawDlg::GetCircleRadius() const
{
    return m_nCircleRadius;
}

int CDrawDlg::GetClickCount() const
{
    return m_nClickCount;
}