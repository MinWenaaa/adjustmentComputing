#pragma once
#include "afxdialogex.h"


// AttachedLevel 对话框

class AttachedLevel : public CDialogEx
{
	DECLARE_DYNAMIC(AttachedLevel)

public:
	AttachedLevel(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~AttachedLevel();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ATTACHED_LEVEL };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
