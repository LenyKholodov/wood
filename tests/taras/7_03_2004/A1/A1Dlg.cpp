// A1Dlg.cpp : implementation file
//

#include "stdafx.h"
#include "A1.h"
#include "A1Dlg.h"
#include ".\a1dlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void ReadString(FILE *f,char *String)
{
	char c;
	int i=0;
	bool fl=true;
	while(fl)
	{
		String[i]=fgetc(f);
		String[i+1]=0;
		if(String[i]==10)
			{
				String[i]=0;
				return;
			}
		if(feof(f))
		{
			String[i]=0;
			fl=false;
		}
		i++;
	}
	return;
}

// CA1Dlg dialog

CA1Dlg::CA1Dlg(CWnd* pParent /*=NULL*/)
	: CDialog(CA1Dlg::IDD, pParent)
{
	il=false;
	fl=false;
	ColorRefs[0]=RGB(0,0,0);
	ColorRefs[1]=RGB(255,255,255);
	ColorRefs[2]=RGB(0,255,0);
	ColorRefs[3]=RGB(0,0,255);
	ColorRefs[4]=RGB(255,255,0);
	ColorRefs[5]=RGB(255,0,255);
	ColorRefs[6]=RGB(0,255,255);
	ColorRefs[7]=RGB(128,128,128);
	ColorRefs[8]=RGB(128,128,0);
	ColorRefs[9]=RGB(128,0,128);
	ColorRefs[10]=RGB(255,0,0);
	ColorRefs[11]=RGB(0,128,128);

	for(int i=0;i<12;i++)
		ColorBitmaps[i].LoadBitmap(134+i);
	
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CA1Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON1, ColorButton);
	DDX_Control(pDX, IDC_BUTTON2, ExitButton);
	DDX_Control(pDX, IDC_LIST1, PicturesList);
	DDX_Control(pDX, IDC_BUTTON3, Colors[0]);
	DDX_Control(pDX, IDC_BUTTON4, Colors[1]);
	DDX_Control(pDX, IDC_BUTTON5, Colors[2]);
	DDX_Control(pDX, IDC_BUTTON6, Colors[3]);
	DDX_Control(pDX, IDC_BUTTON7, Colors[4]);
	DDX_Control(pDX, IDC_BUTTON8, Colors[5]);
	DDX_Control(pDX, IDC_BUTTON9, Colors[6]);
	DDX_Control(pDX, IDC_BUTTON10, Colors[7]);
	DDX_Control(pDX, IDC_BUTTON11, Colors[8]);
	DDX_Control(pDX, IDC_BUTTON12, Colors[9]);
	DDX_Control(pDX, IDC_BUTTON13, Colors[10]);
	DDX_Control(pDX, IDC_BUTTON14, Colors[11]);
}

BEGIN_MESSAGE_MAP(CA1Dlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_BN_CLICKED(IDC_BUTTON2, OnBnClickedButton2)
	ON_WM_CREATE()
	ON_WM_ACTIVATE()
//	ON_LBN_DBLCLK(IDC_LIST1, OnLbnDblclkList1)
ON_LBN_SELCHANGE(IDC_LIST1, OnLbnSelchangeList1)
ON_BN_CLICKED(IDC_BUTTON3, OnBnClickedButton3)
ON_BN_CLICKED(IDC_BUTTON4, OnBnClickedButton4)
ON_BN_CLICKED(IDC_BUTTON5, OnBnClickedButton5)
ON_BN_CLICKED(IDC_BUTTON6, OnBnClickedButton6)
ON_BN_CLICKED(IDC_BUTTON7, OnBnClickedButton7)
ON_BN_CLICKED(IDC_BUTTON8, OnBnClickedButton8)
ON_BN_CLICKED(IDC_BUTTON9, OnBnClickedButton9)
ON_BN_CLICKED(IDC_BUTTON10, OnBnClickedButton10)
ON_BN_CLICKED(IDC_BUTTON11, OnBnClickedButton11)
ON_BN_CLICKED(IDC_BUTTON12, OnBnClickedButton12)
ON_BN_CLICKED(IDC_BUTTON13, OnBnClickedButton13)
ON_BN_CLICKED(IDC_BUTTON14, OnBnClickedButton14)
ON_LBN_DBLCLK(IDC_LIST1, OnLbnDblclkList1)
END_MESSAGE_MAP()


// CA1Dlg message handlers

BOOL CA1Dlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	for(int i=0;i<12;i++)
		Colors[i].SetBitmap(ColorBitmaps[i]);

	CFirstDlg fd;
	fd.Create(IDD_DIALOG2);
	fd.ShowWindow(SW_SHOW);
	
	Sleep(2000);
	fd.SendMessage(WM_CLOSE);

	CPartDlg Pd;
	Pd.pd=this;
	Pd.DoModal();

	FILE *f=fopen(PictFile,"r");
	if(f!=NULL)
	{
		n=0;
		while(!feof(f))
		{
			ReadString(f,Names[n]);
			ReadString(f,ColorFiles[n]);
			ReadString(f,NoncolorFiles[n]);
			PicturesList.AddString(Names[n]);
			n++;
		}
		fclose(f);
	}
	else
		return false;

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CA1Dlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}

	CDC *DC=GetDC();
	CRect r;
	GetWindowRect(&r);
	if(il)
	{
		Image.Draw(DC->m_hDC,0,0,r.right-196,r.bottom-1);
		ColorImage.Draw(DC->m_hDC,r.right-185,270,(r.right-196)/3.6,(r.bottom-1)/3.6);
	}
	
	DC->MoveTo(r.right-195,0);
	DC->LineTo(r.right-195,r.bottom);
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CA1Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CA1Dlg::OnTtnGetDispInfoCustom1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTTDISPINFO pNMTTDI = reinterpret_cast<LPNMTTDISPINFO>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

void CA1Dlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	OnOK();
}

void CA1Dlg::OnBnClickedButton1()
{
	CColorDialog cd;
	if(cd.DoModal()==IDOK)
		Color=cd.GetColor();
}

void CA1Dlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	p=point;
	CDialog::OnLButtonDown(nFlags, point);
}

void CA1Dlg::OnLButtonUp(UINT nFlags, CPoint point)
{
CRect r;
GetWindowRect(&r);
if(r.right-195>p.x)
	if(il)
	{
		float kx=r.right-196-r.left;
		kx=kx/Image.GetWidth();
		float ky=r.bottom-1-r.top;
		ky=ky/Image.GetHeight();
		float x1=p.x/kx;
		float y1=p.y/ky;
		CDC *dc=GetDC();
		CDC* IDC = CDC::FromHandle(Image.GetDC());
		CBrush *OldBrush;
		CBrush *NewBrush=new CBrush(Color);

		OldBrush=IDC->SelectObject(NewBrush);

		int x=(int)x1;
		int y=(int)y1;
		COLORREF c=Image.GetPixel(x,y);
		bool res=IDC->ExtFloodFill(x,y,c,FLOODFILLSURFACE);//FLOODFILLBORDER);

		IDC->SelectObject(OldBrush);

//	dc->BitBlt(0,0,380,325,IDC,0,0,SRCCOPY);
		delete NewBrush;
		RedrawWindow();
		CDialog::OnLButtonUp(nFlags, point);
	}
}

void CA1Dlg::OnBnClickedButton2()
{
	if(MessageBox("Ви упевнені?","Вихід",MB_YESNO|MB_ICONQUESTION)==IDYES)
		SendMessage(WM_CLOSE);
}

int CA1Dlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}

void CA1Dlg::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	CDialog::OnActivate(nState, pWndOther, bMinimized);
	WINDOWPLACEMENT p;
	p.ptMaxPosition.x=0;
	p.ptMaxPosition.y=0;
	p.showCmd=SW_SHOWMAXIMIZED;
	SetWindowPlacement(&p);

	CRect r;
	CRect r1;
	GetWindowRect(&r);
	w=r.right-r.left;
	h=r.top-r.bottom;

	PicturesList.GetWindowRect(&r1);
	PicturesList.SetWindowPos(this,w-r1.right+r1.left-11,r1.top-3,
		0,0,
		SWP_NOSIZE|SWP_NOZORDER);

	ExitButton.GetWindowRect(&r1);
	ExitButton.SetWindowPos(this,w-r1.right+r1.left-11,r1.top-3,
		0,0,
		SWP_NOSIZE|SWP_NOZORDER);

	ColorButton.GetWindowRect(&r1);
	ColorButton.SetWindowPos(this,w-r1.right+r1.left-112,r1.top-3,
		0,0,
		SWP_NOSIZE|SWP_NOZORDER);

	int k=-100;
	for(int i=0;i<12;i++)
	{
		k+=30;
		Colors[i].GetWindowRect(&r1);
		Colors[i].SetWindowPos(this,w-r1.right+r1.left-90+k,r1.top-3,
			0,0,
			SWP_NOSIZE|SWP_NOZORDER);
		if(i==5)
			k=-100;
	}
}


void CA1Dlg::OnLbnSelchangeList1()
{
}


void CA1Dlg::OnBnClickedButton3()
{
	Color=ColorRefs[0];
}

void CA1Dlg::OnBnClickedButton4()
{
	Color=ColorRefs[1];
}

void CA1Dlg::OnBnClickedButton5()
{
	Color=ColorRefs[2];
}

void CA1Dlg::OnBnClickedButton6()
{
	Color=ColorRefs[3];
}

void CA1Dlg::OnBnClickedButton7()
{
	Color=ColorRefs[4];
}

void CA1Dlg::OnBnClickedButton8()
{
	Color=ColorRefs[5];
}

void CA1Dlg::OnBnClickedButton9()
{
	Color=ColorRefs[6];
}

void CA1Dlg::OnBnClickedButton10()
{
	Color=ColorRefs[7];
}

void CA1Dlg::OnBnClickedButton11()
{
	Color=ColorRefs[8];
}

void CA1Dlg::OnBnClickedButton12()
{
	Color=ColorRefs[9];
}

void CA1Dlg::OnBnClickedButton13()
{
	Color=ColorRefs[10];
}

void CA1Dlg::OnBnClickedButton14()
{
	Color=ColorRefs[11];
}

void CA1Dlg::OnLbnDblclkList1()
{
	char s[255];
	int num=PicturesList.GetCurSel();

	FILE *f=fopen("Pictures.cfg","r");
	if(f!=NULL)
		for(int i=0;i<num;i++)
		{
			ReadString(f,s);
			ReadString(f,s);
			ReadString(f,s);
			PicturesList.AddString(Names[n]);
		}

	if(il)
	{
		Image.Destroy();
		ColorImage.Destroy();
		il=false;
		RedrawWindow();
	}

	HRESULT res;
	ReadString(f,s);
	ReadString(f,s);
	res=Image.Load(s);
	if(res!=S_OK)
	{
		MessageBox("Неможу завантажити малюнок","Error",MB_OK|MB_ICONERROR);
		Image.Destroy();
		il=false;
		RedrawWindow();
		return;
	}
	ReadString(f,s);
	res=ColorImage.Load(s);
	if(res!=S_OK)
	{
		MessageBox("Неможу завантажити малюнок","Error",MB_OK|MB_ICONERROR);
		Image.Destroy();
		ColorImage.Destroy();
		il=false;
		RedrawWindow();
		return;
	}
	il=true;
	fclose(f);
	RedrawWindow();
	return;
}
