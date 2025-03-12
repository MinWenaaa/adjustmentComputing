// NetworkLevel.cpp: 实现文件
//

#include "pch.h"
#include "MFCApplication2.h"
#include "afxdialogex.h"
#include "NetworkLevel.h"


// NetworkLevel 对话框

IMPLEMENT_DYNAMIC(NetworkLevel, CDialogEx)

NetworkLevel::NetworkLevel(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_NETWORK_LEVEL, pParent)
{

}

NetworkLevel::~NetworkLevel()
{
}

void NetworkLevel::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(NetworkLevel, CDialogEx)
END_MESSAGE_MAP()


// NetworkLevel 消息处理程序
