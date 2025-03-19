// horizontal_networkcpp: 实现文件
//

#include "pch.h"
#include "MFCApplication2.h"
#include "afxdialogex.h"
#include "horizontal_network.h"


// HorizontalNetWork 对话框

IMPLEMENT_DYNAMIC(HorizontalNetWork, CDialogEx)

HorizontalNetWork::HorizontalNetWork(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_HORIZONTAL_ADJUST, pParent)
{

}

HorizontalNetWork::~HorizontalNetWork()
{
}

void HorizontalNetWork::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(HorizontalNetWork, CDialogEx)
END_MESSAGE_MAP()


// HorizontalNetWork 消息处理程序
