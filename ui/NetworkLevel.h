#pragma once
#include "afxdialogex.h"


// NetworkLevel 对话框

class NetworkLevel : public CDialogEx
{
	DECLARE_DYNAMIC(NetworkLevel)

public:
	NetworkLevel(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~NetworkLevel();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_NETWORK_LEVEL };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
