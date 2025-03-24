#include "pch.h"
#include "framework.h"
#include "CircleDrawing.h"
#include "CircleDrawingDlg.h"
#include "afxdialogex.h"
#include <thread>
#include <cstdlib>
#include <ctime>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);

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

CCircleDrawingDlg::CCircleDrawingDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CIRCLEDRAWING_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CCircleDrawingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CCircleDrawingDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_BN_CLICKED(IDC_RESET, &CCircleDrawingDlg::OnBnClickedReset)
	ON_BN_CLICKED(IDC_RANDOM_MOVE, &CCircleDrawingDlg::OnBnClickedRandomMove)
	ON_BN_CLICKED(IDC_SET_VALUES, &CCircleDrawingDlg::OnBnClickedSetValues)
END_MESSAGE_MAP()



BOOL CCircleDrawingDlg::OnInitDialog()
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

	m_Font.CreatePointFont(120, _T("굴림"));

	GetDlgItem(IDC_STATIC_RADIUS)->SetFont(&m_Font);
	GetDlgItem(IDC_STATIC_THICKNESS)->SetFont(&m_Font);
	GetDlgItem(IDC_POINT_RADIUS_EDIT)->SetFont(&m_Font);
	GetDlgItem(IDC_THICKNESS_EDIT)->SetFont(&m_Font);

	GetDlgItem(IDC_SET_VALUES)->SetFont(&m_Font);
	GetDlgItem(IDC_RESET)->SetFont(&m_Font);
	GetDlgItem(IDC_RANDOM_MOVE)->SetFont(&m_Font);

	SetDlgItemText(IDC_POINT_RADIUS_EDIT, _T("10"));
	SetDlgItemText(IDC_THICKNESS_EDIT, _T("1"));

	pointRadius = 10;
    lineThickness = 1;

	return TRUE;
}

void CCircleDrawingDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CCircleDrawingDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	for (auto& p : clickPoints)
	{
		CRect rect(p.x - pointRadius, p.y - pointRadius, p.x + pointRadius, p.y + pointRadius);
		if (rect.PtInRect(point))
		{
			draggedPoint = &p;
			isDragging = true;
			return;
		}
	}

	if (clickPoints.size() < 3)
	{
		clickPoints.push_back(point);
		Invalidate();
	}
}

void CCircleDrawingDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	if (isDragging && draggedPoint)
	{
		*draggedPoint = point;
		Invalidate();
	}
}

void CCircleDrawingDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	isDragging = false;
	draggedPoint = nullptr;
}

void CCircleDrawingDlg::OnPaint()
{
	CPaintDC dc(this);

	CBrush brush(RGB(0, 0, 0));
	CPen pen(PS_SOLID, 1, RGB(0, 0, 0));

	CBrush* pOldBrush = dc.SelectObject(&brush);
	CPen* pOldPen = dc.SelectObject(&pen);

	for (const auto& p : clickPoints)
	{
		dc.Ellipse(p.x - pointRadius, p.y - pointRadius, p.x + pointRadius, p.y + pointRadius);
	}

	if (clickPoints.size() == 3)
	{
		CPoint center;
		int radius;
		CalculateCircumcircle(clickPoints[0], clickPoints[1], clickPoints[2], center, radius);

		CPen pen(PS_SOLID, lineThickness, RGB(0, 0, 0));
		CPen* oldPen = dc.SelectObject(&pen);
		dc.SelectStockObject(HOLLOW_BRUSH);
		dc.Ellipse(center.x - radius, center.y - radius, center.x + radius, center.y + radius);
		dc.SelectObject(oldPen);
		DrawCircle(dc);
	}

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
	{
		CDialogEx::OnPaint();
	}
}

void CCircleDrawingDlg::DrawCircle(CDC& dc)
{
	if (clickPoints.size() < 3) return;

	CPoint center;
	int radius;
	CalculateCircumcircle(clickPoints[0], clickPoints[1], clickPoints[2], center, radius);

	dc.SelectStockObject(HOLLOW_BRUSH);
	dc.Ellipse(center.x - radius, center.y - radius, center.x + radius, center.y + radius);
}

void CCircleDrawingDlg::CalculateCircumcircle(const CPoint& A, const CPoint& B, const CPoint& C, CPoint& center, int& radius)
{
	double D = 2 * (A.x * (B.y - C.y) + B.x * (C.y - A.y) + C.x * (A.y - B.y));
	if (D == 0) return;

	center.x = ((A.x * A.x + A.y * A.y) * (B.y - C.y) + (B.x * B.x + B.y * B.y) * (C.y - A.y) + (C.x * C.x + C.y * C.y) * (A.y - B.y)) / D;
	center.y = ((A.x * A.x + A.y * A.y) * (C.x - B.x) + (B.x * B.x + B.y * B.y) * (A.x - C.x) + (C.x * C.x + C.y * C.y) * (B.x - A.x)) / D;
	radius = sqrt((center.x - A.x) * (center.x - A.x) + (center.y - A.y) * (center.y - A.y));
}

HCURSOR CCircleDrawingDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CCircleDrawingDlg::OnBnClickedReset()
{
	clickPoints.clear();
	draggedPoint = nullptr;
	isDragging = false;
	Invalidate();
}
void CCircleDrawingDlg::OnBnClickedRandomMove()
{
	if (clickPoints.size() < 3) return;

	std::thread randomMoveThread([this]()
		{
			srand((unsigned int)time(NULL));
			for (int i = 0; i < 10; i++)
			{
				Sleep(500);

				for (auto& point : clickPoints)
				{
					point.x = rand() % 400 + 50;
					point.y = rand() % 400 + 50;
				}

				Invalidate();
			}
		});

	randomMoveThread.detach();
}

void CCircleDrawingDlg::OnBnClickedSetValues()
{
	CString strPointRadius, strThickness;

	GetDlgItemText(IDC_POINT_RADIUS_EDIT, strPointRadius);
	GetDlgItemText(IDC_THICKNESS_EDIT, strThickness);

	int newPointRadius = _ttoi(strPointRadius);
	int newThickness = _ttoi(strThickness);

	if (newPointRadius <= 0 || newThickness <= 0)
	{
		AfxMessageBox(_T("점 크기와 선 두께는 0보다 커야 합니다."));
		return;
	}

	pointRadius = newPointRadius;
	lineThickness = newThickness;

	AfxMessageBox(_T("점 크기와 선 두께가 적용되었습니다!"));
}
