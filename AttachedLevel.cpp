// AttachedLevel.cpp: 实现文件
//

#include "pch.h"
#include "MFCApplication2.h"
#include "afxdialogex.h"
#include "AttachedLevel.h"


// AttachedLevel 对话框

IMPLEMENT_DYNAMIC(AttachedLevel, CDialogEx)

AttachedLevel::AttachedLevel(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_ATTACHED_LEVEL, pParent)
{

}

AttachedLevel::~AttachedLevel()
{
}

void AttachedLevel::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(AttachedLevel, CDialogEx)
END_MESSAGE_MAP()


// AttachedLevel 消息处理程序
