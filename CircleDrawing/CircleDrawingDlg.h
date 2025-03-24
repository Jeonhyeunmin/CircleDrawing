#include <vector>

#pragma once

class CCircleDrawingDlg : public CDialogEx
{

public:
	CCircleDrawingDlg(CWnd* pParent = nullptr);
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CIRCLEDRAWING_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);

protected:
	HICON m_hIcon;

	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedReset();
	afx_msg void OnBnClickedRandomMove();
	afx_msg void OnBnClickedSetValues();
	afx_msg void OnBnClickedSetPointRadius();
	DECLARE_MESSAGE_MAP()
	

private:
	std::vector<CPoint> clickPoints;
	void DrawCircle(CDC& dc);
	void CalculateCircumcircle(const CPoint& A, const CPoint& B, const CPoint& C, CPoint& center, int& radius);
	bool isDragging;
	CPoint* draggedPoint;
	int pointRadius;
	int lineThickness;
	CFont m_Font;
};
