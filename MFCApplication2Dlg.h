
// MFCApplication2Dlg.h: 头文件
//

#pragma once
#include "AttachedLevel.h"
#include "NetworkLevel.h"
#include "level_adjust.h"
#include "horizontal_network.h"
#include "horizontal_adjust.h"


// CMFCApplication2Dlg 对话框
class CMFCApplication2Dlg : public CDialogEx
{
// 构造
public:
	CMFCApplication2Dlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MFCAPPLICATION2_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOpenFile();
	afx_msg void OnCbnSelchangeModSel();
	int mode;
	WenMin::levelAdjust la;
	horiControlNet horiNet;

	// Tab control
	CTabCtrl visual_tab;
	AttachedLevel* attaced_levelDlg;
	NetworkLevel* network_levelDlg;
	HorizontalNetWork* horizontal_netDlg;
	CComboBox mode_sle;
	afx_msg void OnBnClickedBtnSolve();
};
