
// MFCApplication2Dlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "MFCApplication2.h"
#include "MFCApplication2Dlg.h"
#include "afxdialogex.h"
#include "level_adjust.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
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


// CMFCApplication2Dlg 对话框



CMFCApplication2Dlg::CMFCApplication2Dlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MFCAPPLICATION2_DIALOG, pParent) {
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFCApplication2Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB1, visual_tab);
	DDX_Control(pDX, IDC_MOD_SEL, mode_sle);
}

BEGIN_MESSAGE_MAP(CMFCApplication2Dlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_OPEN_FILE, &CMFCApplication2Dlg::OnBnClickedOpenFile)
	ON_CBN_SELCHANGE(IDC_MOD_SEL, &CMFCApplication2Dlg::OnCbnSelchangeModSel)
	ON_BN_CLICKED(IDC_BTN_SOLVE, &CMFCApplication2Dlg::OnBnClickedBtnSolve)
END_MESSAGE_MAP()


// CMFCApplication2Dlg 消息处理程序

BOOL CMFCApplication2Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
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

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	visual_tab.InsertItem(0, _T("附合水准"));
	visual_tab.InsertItem(1, _T("水准网平差"));
	visual_tab.InsertItem(2, _T("平面网平差"));
	attaced_levelDlg = new AttachedLevel();
	network_levelDlg = new NetworkLevel();
	horizontal_netDlg = new HorizontalNetWork();
	attaced_levelDlg->Create(IDD_ATTACHED_LEVEL, &visual_tab);
	network_levelDlg->Create(IDD_NETWORK_LEVEL, &visual_tab);
	horizontal_netDlg->Create(IDD_HORIZONTAL_ADJUST, &visual_tab);
	CRect rect;
	visual_tab.GetClientRect(&rect);
	rect.top += 60; rect.bottom -= 20; rect.left += 15; rect.right -= 15;
	attaced_levelDlg->MoveWindow(&rect);
	network_levelDlg->MoveWindow(&rect);
	attaced_levelDlg->ShowWindow(SW_SHOW);
	network_levelDlg->ShowWindow(SW_HIDE);
	horizontal_netDlg->ShowWindow(SW_HIDE);


	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CMFCApplication2Dlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMFCApplication2Dlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		// 获取指定控件的设备上下文
		CWnd* pStaticCtrl = horizontal_netDlg->GetDlgItem(IDC_PAINT_AREA); // 获取控件指针
		if (pStaticCtrl)
		{
			CRect rect;
			pStaticCtrl->GetClientRect(&rect); // 获取控件的客户区矩形
			CDC* pDC = pStaticCtrl->GetDC(); // 获取控件的设备上下文

			if (horiNet.painted) {
				horiNet.OnDraw(pDC);
			}

			pStaticCtrl->ReleaseDC(pDC); // 释放设备上下文
		}

		// 调用基类的 OnPaint 函数以处理其他默认绘制
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CMFCApplication2Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CMFCApplication2Dlg::OnBnClickedOpenFile()
{
	// TODO: Add your control notification handler code here   
// 设置过滤器   
	TCHAR szFilter[] = _T("文本文件(*.txt)|*.txt|所有文件(*.*)|*.*||");
	// 构造打开文件对话框   
	CFileDialog fileDlg(TRUE, _T("txt"), NULL, 0, szFilter, this);
	CString strFilePath;

	// 显示打开文件对话框   
	if (IDOK == fileDlg.DoModal())
	{
		// 如果点击了文件对话框上的“打开”按钮，则将选择的文件路径显示到编辑框里   
		strFilePath = fileDlg.GetPathName();
		CT2CA pszConvertedAnsiString(strFilePath);
		std::string strStd(pszConvertedAnsiString);
		if(mode!=2)la.ReadData(strStd.c_str());
		else horiNet.readData(strStd.c_str());
	}
}


void CMFCApplication2Dlg::OnCbnSelchangeModSel()
{
	// TODO: 在此添加控件通知处理程序代码
	mode = mode_sle.GetCurSel();
	visual_tab.SetCurSel(mode); // 选择第一个标签页

	switch (visual_tab.GetCurSel()) {
	case 0:
		attaced_levelDlg->ShowWindow(SW_SHOW);
		network_levelDlg->ShowWindow(SW_HIDE);
		horizontal_netDlg->ShowWindow(SW_HIDE);
		break;
	case 1:
		attaced_levelDlg->ShowWindow(SW_HIDE);
		network_levelDlg->ShowWindow(SW_SHOW);
		horizontal_netDlg->ShowWindow(SW_HIDE);
		break;
	case 2:
		attaced_levelDlg->ShowWindow(SW_HIDE);
		network_levelDlg->ShowWindow(SW_HIDE);
		horizontal_netDlg->ShowWindow(SW_SHOW);
	}
}


void CMFCApplication2Dlg::OnBnClickedBtnSolve()
{
	// TODO: 在此添加控件通知处理程序代码
	CWnd* pStatic = nullptr;
	std::string str2 = "";
	switch (mode) {
	case 0:
		la.SolveAttachedLevel();
		pStatic = attaced_levelDlg->GetDlgItem(IDC_ATTACHED_LEVEL_STRING1);
		if (pStatic != nullptr) {
			std::string str = la.getAttachedPointString();
			CString cstrText(str.c_str());
			pStatic->SetWindowText(cstrText);
		}
		pStatic = attaced_levelDlg->GetDlgItem(IDC_ATTACHED_LEVEL_STRING2);
		if (pStatic != nullptr) {
			std::string str = la.getAttachedEdgeString();
			CString cstrText(str.c_str());
			pStatic->SetWindowText(cstrText);
		}
		break;
	case 1:
		la.SolveNetworkLevel(str2);
		pStatic = network_levelDlg->GetDlgItem(IDC_NETWORK_LEVEL_STRING1);
		if (pStatic != nullptr) {
			std::string str = la.getAttachedPointString();
			CString cstrText(str.c_str());
			pStatic->SetWindowText(cstrText);
		}
		pStatic = network_levelDlg->GetDlgItem(IDC_NETWORK_LEVEL_STRING2);
		if (pStatic != nullptr) {
			std::string str = la.getAttachedEdgeString();
			CString cstrText(str.c_str());
			pStatic->SetWindowText(cstrText);
		}
		pStatic = network_levelDlg->GetDlgItem(IDC_NETWORK_LEVEL_STRING3);
		if (pStatic != nullptr) {
			CString cstrText(str2.c_str());
			pStatic->SetWindowText(cstrText);
		}
		break;
	case 2:
		std::string str = horiNet.solve();
		CString cstrText(str.c_str());
		pStatic = horizontal_netDlg->GetDlgItem(IDC_HORI_MATRIX_STRING);
		if (pStatic != nullptr)pStatic->SetWindowText(cstrText);
		pStatic = horizontal_netDlg->GetDlgItem(IDC_HORIZONTAL_COORD_STRING);
		if (pStatic != nullptr) {
			std::string str = horiNet.toString();
			CString cstrText(str.c_str());
			pStatic->SetWindowText(cstrText);
		}
		OnPaint();
		break;
	}
}
