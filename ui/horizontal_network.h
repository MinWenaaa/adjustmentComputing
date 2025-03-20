#pragma once
#include "afxdialogex.h"


// HorizontalNetWork 对话框

class HorizontalNetWork : public CDialogEx
{
	DECLARE_DYNAMIC(HorizontalNetWork)

public:
	HorizontalNetWork(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~HorizontalNetWork();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_HORIZONTAL_ADJUST };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
