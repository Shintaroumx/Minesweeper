// 大作业.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "大作业.h"

//全局变量：
//#define MAX_LOADSTRING 100
#define SHADOW_WIDTH1 1 //阴影宽度
#define SHADOW_WIDTH2 2 
#define SHADOW_WIDTH3 3 
#define BORDER 8 //边界的宽度
#define BOX_WIDTH 20//box的宽度
#define BOX_HEIGHT 20 //box的高度
#define RESERVED_AREA 32 //保留区高度

//自定义函数声明：
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void DrawBorder(HDC hdc, int x, int y, int width, int height, int BorderWidth, COLORREF &color1, COLORREF &color2, bool WTop);
void AutoOpenBoxes(HDC &hdc, int i, int j);
void drawRect(HDC &hdc, int x, int y);
bool allRoundIsOpened(int row, int col);
void InitBox();
void UpdateTheWindow(HWND &hwnd, int rows, int cols, int mineNO);
void ComputeRoundMineNo();
void RandomSetMines();

//结构体Box,roundMineNumber表示周围雷的个数
//containMine表示是否里面是否有雷，
//isOpened表示box是否已经打开
static HMENU hMenu;
struct Box
{
	int roundMineNumber;
	bool containMine;
	bool isOpened;
	bool isMarked;
};
static int mineNumber;
static int ROWS; //行数
static int COLS; //列数 
static int iSM_BorderX;
static int iSM_BorderY;
static int iSM_CaptionY;
static int iSM_MenuY;
static Box **box;
static int i, j, numberOpened;
static RECT mineRect;

// 全局变量: 
HINSTANCE hInst;                                // 当前实例
//LPCSTR szTitle;                  // 标题栏文本
//LPCSTR szWindowClass;            // 主窗口类名

// 此代码模块中包含的函数的前向声明: 
BOOL              MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 在此放置代码。
	MSG msg;
	mineNumber = 5;
	ROWS = 10;
	COLS = 10;
	mineNumber = 10;
	box = new Box*[COLS];
	for (int k = 0; k<COLS; k++)
		box[k] = new Box[ROWS];

	iSM_BorderX = GetSystemMetrics(SM_CXBORDER);
	iSM_BorderY = GetSystemMetrics(SM_CYBORDER);
	iSM_CaptionY = GetSystemMetrics(SM_CYCAPTION);
	iSM_MenuY = GetSystemMetrics(SM_CYMENU);
	mineRect.left = SHADOW_WIDTH3 + BORDER + SHADOW_WIDTH2;
	mineRect.right = mineRect.left + BOX_WIDTH * COLS;
	mineRect.top = SHADOW_WIDTH3 + 2 * BORDER + RESERVED_AREA + 3 * SHADOW_WIDTH2;
	mineRect.bottom = mineRect.top + BOX_HEIGHT * ROWS;


    // 初始化全局字符串
    //LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    //LoadStringW(hInstance, IDC_MY, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 执行应用程序初始化: 
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MY));


    // 主消息循环: 
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  函数: MyRegisterClass()
//
//  目的: 注册窗口类。
//
BOOL MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASS wndclass;
	static TCHAR szAppName[] = TEXT("SineWave");
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MY));;
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	hMenu = LoadMenu(hInstance, MAKEINTRESOURCE(IDC_MY));
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = szAppName;


	if (!RegisterClass(&wndclass))
	{
		MessageBox(NULL, TEXT("Program requires Windows NT!"),
			szAppName, MB_ICONERROR);
		return 0;
	}
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目的: 保存实例句柄并创建主窗口
//
//   注释: 
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	static TCHAR szAppName[] = TEXT("SineWave");
	HWND hwnd;
	hInst = hInstance; // 将实例句柄存储在全局变量中
   int cx = BOX_WIDTH * COLS + 2 * SHADOW_WIDTH3 + 2 * SHADOW_WIDTH2 + 2 * iSM_BorderX + 2 * BORDER;
   int cy = BOX_HEIGHT * ROWS + 2 * SHADOW_WIDTH3 + 4 * SHADOW_WIDTH2 + 2 * iSM_BorderY + 3 * BORDER + RESERVED_AREA + iSM_CaptionY + iSM_MenuY;
   hwnd = CreateWindow(szAppName, TEXT("扫雷"),
	   WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX,
	   CW_USEDEFAULT, CW_USEDEFAULT,
	   cx, cy,
	   NULL, hMenu, hInstance, NULL);


   if (!hwnd)
   {
      return FALSE;
   }

   ShowWindow(hwnd, nCmdShow);
   UpdateWindow(hwnd);

   return TRUE;
}

//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的:    处理主窗口的消息。
//
//  WM_COMMAND  - 处理应用程序菜单
//  WM_PAINT    - 绘制主窗口
//  WM_DESTROY  - 发送退出消息并返回
//
//



LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	HBRUSH hBrush;
	PAINTSTRUCT ps;
	static bool isLButtonDown = false;
	static bool isRButtonDown = false;
	int xStart, yStart;
	static int m, n, cx, cy;
	static COLORREF color1 = RGB(0, 0, 0),//黑色
		color2 = RGB(255, 255, 255),//白色
		color3 = RGB(200, 200, 200),//灰色
		color4 = RGB(120, 120, 200);//淡蓝
	static RECT grid, rect;
	static char szBuffer[2];
	POINT pt;

	switch (message)
    {
	case WM_CREATE:
		//初始化变量
		InitBox();
		RandomSetMines();
		ComputeRoundMineNo();
		return 0;
    case WM_COMMAND:
        {
			switch (LOWORD(wParam))
			{
			case IDM_PLAY:
				InitBox();
				RandomSetMines();
				ComputeRoundMineNo();
				InvalidateRect(hWnd, NULL, TRUE);
				UpdateWindow(hWnd);
				return 0;
			case IDM_BEGINER:
				UpdateTheWindow(hWnd, 10, 10, 5);
				return 0;
			case IDM_MEDIUM:
				UpdateTheWindow(hWnd, 15, 15, 15);
				return 0;
			case IDM_SENIOR:
				UpdateTheWindow(hWnd, 20, 20, 30);
				return 0;
			case IDM_ABOUT:
				DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
				break;
			case IDM_EXIT:
				SendMessage(hWnd, WM_CLOSE, 0, 0);
				return 0;
			}
        }
 
	case WM_PAINT:
        {
			hdc = BeginPaint(hWnd, &ps);
            // TODO: 在此处添加使用 hdc 的任何绘图代码...
			GetClientRect(hWnd, &rect);
			hBrush = CreateSolidBrush(RGB(200, 200, 200));
			SetBkColor(hdc, RGB(200, 200, 200));
			SelectObject(hdc, hBrush);
			SelectObject(hdc, GetStockObject(NULL_PEN));
			Rectangle(hdc, 0, 0, rect.right, rect.bottom);
			//绘制外边界
			DrawBorder(hdc, 0, 0, rect.right, rect.bottom, SHADOW_WIDTH3, color1, color2, true);
			//绘制保留区
			DrawBorder(hdc, SHADOW_WIDTH3 + BORDER, SHADOW_WIDTH3 + BORDER,
				BOX_WIDTH * COLS + 2 * SHADOW_WIDTH2, RESERVED_AREA + 2 * SHADOW_WIDTH2, SHADOW_WIDTH2, color1, color2, false);
			//绘制内边界
			DrawBorder(hdc, SHADOW_WIDTH3 + BORDER, SHADOW_WIDTH3 + 2 * BORDER + RESERVED_AREA + 2 * SHADOW_WIDTH2,
				BOX_WIDTH * COLS + 2 * SHADOW_WIDTH2, BOX_HEIGHT*ROWS + 2 * SHADOW_WIDTH2, SHADOW_WIDTH2, color1, color2, false);
			//绘制雷区的box
			xStart = mineRect.left;
			yStart = mineRect.top;
			RECT rect0;
			for (i = 0; i < ROWS; i++)
			{
				for (j = 0; j < COLS; j++)
				{
					if (!box[i][j].isOpened)
						DrawBorder(hdc, xStart, yStart, BOX_WIDTH - 2, BOX_HEIGHT - 2, SHADOW_WIDTH2,
							color1, color2, true);
					else
					{
						DrawBorder(hdc, xStart, yStart, BOX_WIDTH - 2, BOX_HEIGHT - 2, SHADOW_WIDTH2,
							color3, color3, true);
						DrawBorder(hdc, xStart, yStart, BOX_WIDTH, BOX_HEIGHT, SHADOW_WIDTH1,
							color4, color4, true);
						if (box[i][j].roundMineNumber != 0)
						{
							rect0.left = mineRect.left + BOX_WIDTH * j;
							rect0.right = rect0.left + BOX_WIDTH;
							rect0.top = mineRect.top + BOX_HEIGHT * i;
							rect0.bottom = rect0.top + BOX_HEIGHT;
							wsprintf(szBuffer, TEXT("%d"), box[i][j].roundMineNumber);
							//输出周围雷的个数
							DrawText(hdc, szBuffer, -1, &rect0, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
						}
					}
					xStart += BOX_WIDTH;
				}
				yStart += BOX_HEIGHT;
				xStart = SHADOW_WIDTH3 + BORDER + SHADOW_WIDTH2;
			}
            
            EndPaint(hWnd, &ps);
        }
        return 0;
	case WM_LBUTTONDOWN:
		hdc = GetDC(hWnd);
		isLButtonDown = true;
		pt.x = LOWORD(lParam);
		pt.y = HIWORD(lParam);
		grid.left = mineRect.left;
		grid.right = mineRect.right;
		grid.top = mineRect.top;
		grid.bottom = mineRect.bottom;
		if (PtInRect(&grid, pt))
		{
			j = (pt.x - grid.left) / BOX_WIDTH;
			i = (pt.y - grid.top) / BOX_HEIGHT;
			grid.left += BOX_WIDTH * j;
			grid.top += BOX_HEIGHT * i;
			grid.right = grid.left + BOX_WIDTH;
			grid.bottom = grid.top + BOX_HEIGHT;
			hdc = GetDC(hWnd);
			DrawBorder(hdc, grid.left, grid.top, BOX_WIDTH - 2, BOX_HEIGHT - 2, SHADOW_WIDTH2, color3, color3, false);
			DrawBorder(hdc, grid.left, grid.top, BOX_WIDTH, BOX_HEIGHT, SHADOW_WIDTH1, color4, color4, false);
		}
		DeleteDC(hdc);
		return 0;
	case WM_LBUTTONUP:
		static HPEN hPen;
		static RECT rect1;
		hdc = GetDC(hWnd);
		hPen = CreatePen(PS_SOLID, 2, RGB(255, 255, 0));
		SelectObject(hdc, hPen);
		SetBkColor(hdc, RGB(200, 200, 200));
		isLButtonDown = false;
		rect1.left = mineRect.left + BOX_WIDTH * j;
		rect1.right = rect1.left + BOX_WIDTH;
		rect1.top = mineRect.top + BOX_HEIGHT * i;
		rect1.bottom = rect1.top + BOX_HEIGHT;
		if (!box[i][j].isOpened)
		{
			if (!box[i][j].containMine)
			{
				if (box[i][j].roundMineNumber != 0)
				{
					wsprintf(szBuffer, TEXT("%d"), box[i][j].roundMineNumber);
					//输出周围雷的个数
					DrawText(hdc, szBuffer, -1, &rect1, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
				}

				box[i][j].isOpened = true;
				numberOpened++;
				if (numberOpened == ROWS * COLS - mineNumber)
				{
					MessageBox(hWnd, TEXT("祝贺你赢了!"), TEXT("扫雷"), MB_ICONWARNING);
				}

				AutoOpenBoxes(hdc, i, j);
			}
			else
			{
				for (int k = 0; k<ROWS; k++)
					for (int l = 0; l<COLS; l++)
					{
						if (box[k][l].containMine)//点到雷后,使所有的雷显现
						{
							rect.left = mineRect.left + BOX_WIDTH * l;
							rect.top = mineRect.top + BOX_HEIGHT * k;
							rect.right = rect.left + BOX_WIDTH;
							rect.bottom = rect.top + BOX_HEIGHT;
							DrawBorder(hdc, rect.left, rect.top, BOX_WIDTH - 2, BOX_HEIGHT - 2, SHADOW_WIDTH2, color3, color3, false);
							DrawBorder(hdc, rect.left, rect.top, BOX_WIDTH, BOX_HEIGHT, SHADOW_WIDTH1, color4, color4, false);
							DrawText(hdc, "*", -1, &rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
						}
					}
				MessageBox(hWnd, TEXT("游戏结束"), TEXT("扫雷"), MB_ICONWARNING);
			}
		}
		DeleteDC(hdc);
		return 0;
	case WM_RBUTTONDOWN:
		hdc = GetDC(hWnd);
		isRButtonDown = true;
		pt.x = LOWORD(lParam);
		pt.y = HIWORD(lParam);
		grid.left = mineRect.left;
		grid.right = mineRect.right;
		grid.top = mineRect.top;
		grid.bottom = mineRect.bottom;
		if (PtInRect(&grid, pt))
		{
			j = (pt.x - grid.left) / BOX_WIDTH;
			i = (pt.y - grid.top) / BOX_HEIGHT;
			grid.left += BOX_WIDTH * j;
			grid.top += BOX_HEIGHT * i;
			grid.right = grid.left + BOX_WIDTH;
			grid.bottom = grid.top + BOX_HEIGHT;
			hdc = GetDC(hWnd);
		}
		DeleteDC(hdc);
		return 0;
	case WM_RBUTTONUP:
		//static HPEN hPen;
		hdc = GetDC(hWnd);
		static RECT rect2;
		hPen = CreatePen(PS_SOLID, 2, RGB(255, 255, 0));
		SelectObject(hdc, hPen);
		SetBkColor(hdc, RGB(200, 200, 200));
		isRButtonDown = false;
		if (box[i][j].isMarked==false&&!box[i][j].isOpened)
		{
			rect2.left = mineRect.left + BOX_WIDTH * j;
			rect2.right = rect2.left + BOX_WIDTH;
			rect2.top = mineRect.top + BOX_HEIGHT * i;
			rect2.bottom = rect2.top + BOX_HEIGHT;
			DrawText(hdc, "?", -1, &rect2, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
			box[i][j].isMarked = true;
		}
		else if (box[i][j].isMarked == true && box[i][j].isOpened == true)
		{
			rect.left = mineRect.left + BOX_WIDTH * j;
			rect.right = rect.left + BOX_WIDTH;
			rect.top = mineRect.top + BOX_HEIGHT * i;
			rect.bottom = rect.top + BOX_HEIGHT;
			//DrawBorder(hdc, grid.left, grid.top, BOX_WIDTH - 2, BOX_HEIGHT - 2, SHADOW_WIDTH2, color1, color2, true);
			//DrawBorder(hdc, grid.left, grid.top, BOX_WIDTH, BOX_HEIGHT, SHADOW_WIDTH1, color3, color3, true);
			DrawText(hdc, "  ", -1, &rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
			box[i][j].isMarked = false;
		}
		else if (box[i][j].isMarked == true && !box[i][j].isOpened)
		{
			rect.left = mineRect.left + BOX_WIDTH * j;
			rect.right = rect.left + BOX_WIDTH;
			rect.top = mineRect.top + BOX_HEIGHT * i;
			rect.bottom = rect.top + BOX_HEIGHT;
			//DrawBorder(hdc, grid.left, grid.top, BOX_WIDTH - 2, BOX_HEIGHT - 2, SHADOW_WIDTH2, color1, color2, true);
			//DrawBorder(hdc, grid.left, grid.top, BOX_WIDTH, BOX_HEIGHT, SHADOW_WIDTH1, color3, color3, true);
			DrawText(hdc, "  ", -1, &rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
			box[i][j].isMarked = false;
		}
		else if (box[i][j].isMarked == false && box[i][j].isOpened == true)
		{
			break;
		}
		DeleteDC(hdc);
		return 0;
	case WM_MOUSEMOVE:
		hdc = GetDC(hWnd);
		if (isLButtonDown)
		{
			pt.x = LOWORD(lParam);
			pt.y = HIWORD(lParam);
			rect.left = mineRect.left;
			rect.right = mineRect.right;
			rect.top = mineRect.top;
			rect.bottom = mineRect.bottom;

			if (PtInRect(&rect, pt))//判断是否在雷区
			{
				m = i; n = j;
				j = (pt.x - rect.left) / BOX_WIDTH;
				i = (pt.y - rect.top) / BOX_HEIGHT;
				rect.left += BOX_WIDTH * j;
				rect.top += BOX_HEIGHT * i;
				rect.right = rect.left + BOX_WIDTH;
				rect.bottom = rect.top + BOX_HEIGHT;
				if (grid.left == rect.left && grid.right == rect.right
					&&grid.bottom == rect.bottom && grid.top == rect.top)
					return 0;
				hdc = GetDC(hWnd);
				if (!box[m][n].isOpened)//鼠标移走后,恢复未按下
				{
					DrawBorder(hdc, grid.left, grid.top, BOX_WIDTH - 2, BOX_HEIGHT - 2, SHADOW_WIDTH2, color1, color2, true);
					DrawBorder(hdc, grid.left, grid.top, BOX_WIDTH, BOX_HEIGHT, SHADOW_WIDTH1, color3, color3, true);
				}
				//使当前的box成为按下状态
				grid.left = rect.left;
				grid.right = rect.right;
				grid.bottom = rect.bottom;
				grid.top = rect.top;
				DrawBorder(hdc, grid.left, grid.top, BOX_WIDTH - 2, BOX_HEIGHT - 2, SHADOW_WIDTH2, color3, color3, true);
				DrawBorder(hdc, grid.left, grid.top, BOX_WIDTH, BOX_HEIGHT, SHADOW_WIDTH1, color4, color4, true);
			}
			DeleteDC(hdc);
		}
		return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// “关于”框的消息处理程序。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

//绘制边界
void DrawBorder(HDC hdc, int x, int y, int width, int height, int BorderWidth, COLORREF &color1, COLORREF &color2, bool WTop)
{
	int i;
	HPEN hpen1, hpen2;
	hpen1 = CreatePen(PS_SOLID, 1, color1);
	hpen2 = CreatePen(PS_SOLID, 1, color2);
	if (WTop)
		SelectObject(hdc, hpen2);
	else
		SelectObject(hdc, hpen1);
	for (i = 0; i < BorderWidth; i++)
	{
		MoveToEx(hdc, x + i, y + i, NULL);
		LineTo(hdc, x + width - i, y + i);
		MoveToEx(hdc, x + i, y + i, NULL);
		LineTo(hdc, x + i, y + height - i);
	}
	if (WTop)
		SelectObject(hdc, hpen1);
	else
		SelectObject(hdc, hpen2);
	for (i = 0; i < BorderWidth; i++)
	{
		MoveToEx(hdc, x + width - i, y + height - i, NULL);
		LineTo(hdc, x + width - i, y + i);
		MoveToEx(hdc, x + width - i, y + height - i, NULL);
		LineTo(hdc, x + i, y + height - i);
	}
}
//初始化box
void InitBox()
{
	numberOpened = 0;
	for (i = 0; i<ROWS; i++)
		for (j = 0; j<COLS; j++)
		{
			box[i][j].containMine = false;
			box[i][j].isOpened = false;
			box[i][j].roundMineNumber = 0;
			box[i][j].isMarked = false;
		}
}
//计算周围雷的个数
void ComputeRoundMineNo()
{
	for (i = 0; i<ROWS; i++)
		for (j = 0; j<COLS; j++)
		{
			if (i>0 && box[i - 1][j].containMine == true)
			{
				box[i][j].roundMineNumber++;
			}
			if (i>0 && j<COLS - 1 && box[i - 1][j + 1].containMine == true)
			{
				box[i][j].roundMineNumber++;
			}
			if (j<COLS - 1 && box[i][j + 1].containMine == true)
			{
				box[i][j].roundMineNumber++;
			}
			if (i<ROWS - 1 && j<COLS && box[i + 1][j + 1].containMine == true)
			{
				box[i][j].roundMineNumber++;
			}
			if (i<ROWS - 1 && box[i + 1][j].containMine == true)
			{
				box[i][j].roundMineNumber++;
			}
			if (i<ROWS - 1 && j>0 && box[i + 1][j - 1].containMine == true)
			{
				box[i][j].roundMineNumber++;
			}
			if (j>0 && box[i][j - 1].containMine == true)
			{
				box[i][j].roundMineNumber++;
			}
			if (i>0 && j>0 && box[i - 1][j - 1].containMine == true)
			{
				box[i][j].roundMineNumber++;
			}
		}
}
//随机布雷
void RandomSetMines()
{
	int row, col;
	srand((UINT)GetCurrentTime());
	for (i = 0; i<mineNumber;)
	{
		row = rand() % ROWS;
		col = rand() % COLS;
		if (box[row][col].containMine != true)
		{
			box[row][col].containMine = true;
			i++;
		}
	}
}
//自动打开box
void AutoOpenBoxes(HDC &hdc, int x, int y)
{
	if (x >= 0 && y >= 0 && x < ROWS && y < COLS &&
		box[x][y].roundMineNumber == 0 && !allRoundIsOpened(x, y))
	{
		drawRect(hdc, x - 1, y);
		drawRect(hdc, x - 1, y + 1);
		drawRect(hdc, x, y + 1);
		drawRect(hdc, x + 1, y + 1);
		drawRect(hdc, x + 1, y);
		drawRect(hdc, x + 1, y - 1);
		drawRect(hdc, x, y - 1);
		drawRect(hdc, x - 1, y - 1);
		AutoOpenBoxes(hdc, x - 1, y);
		AutoOpenBoxes(hdc, x - 1, y + 1);
		AutoOpenBoxes(hdc, x, y + 1);
		AutoOpenBoxes(hdc, x + 1, y + 1);
		AutoOpenBoxes(hdc, x + 1, y);
		AutoOpenBoxes(hdc, x + 1, y - 1);
		AutoOpenBoxes(hdc, x, y - 1);
		AutoOpenBoxes(hdc, x - 1, y - 1);
	}
}
//更新窗口
void UpdateTheWindow(HWND &hwnd, int rows, int cols, int mineNO)
{
	ROWS = rows;
	COLS = cols;
	mineNumber = mineNO;
	mineRect.left = SHADOW_WIDTH3 + BORDER + SHADOW_WIDTH2;
	mineRect.right = mineRect.left + BOX_WIDTH * COLS;
	mineRect.top = SHADOW_WIDTH3 + 2 * BORDER + RESERVED_AREA + 3 * SHADOW_WIDTH2;
	mineRect.bottom = mineRect.top + BOX_HEIGHT * ROWS;
	box = new Box*[COLS];
	for (int k = 0; k< COLS; k++)
		box[k] = new Box[ROWS];
	InitBox();
	RandomSetMines();
	ComputeRoundMineNo();
	int cx = BOX_WIDTH * COLS + 2 * SHADOW_WIDTH3 +
		2 * SHADOW_WIDTH2 + 2 * iSM_BorderX + 2 * BORDER;
	int cy = BOX_HEIGHT * ROWS + 2 * SHADOW_WIDTH3 +
		4 * SHADOW_WIDTH2 + 2 * iSM_BorderY + 3 * BORDER + RESERVED_AREA + iSM_CaptionY + iSM_MenuY;
	MoveWindow(hwnd, 0, 0, cx, cy, true);
}
//绘制矩形
void drawRect(HDC &hdc, int x, int y)
{
	RECT rect1;
	COLORREF color1 = RGB(200, 200, 200),
		color2 = RGB(120, 120, 200);
	TCHAR szBuffer[2] = { "0" };
	if (x<0 || y<0 || x >= ROWS || y >= COLS)
		return;
	rect1.left = mineRect.left + BOX_WIDTH * y;
	rect1.right = rect1.left + BOX_WIDTH;
	rect1.top = mineRect.top + BOX_HEIGHT * x;
	rect1.bottom = rect1.top + BOX_HEIGHT;

	if (!box[x][y].isOpened)
	{
		DrawBorder(hdc, rect1.left, rect1.top,
			BOX_WIDTH - 2, BOX_HEIGHT - 2, SHADOW_WIDTH2, color1, color1, true);
		DrawBorder(hdc, rect1.left, rect1.top,
			BOX_WIDTH, BOX_HEIGHT, SHADOW_WIDTH1, color2, color2, true);

		if (!box[x][y].containMine && box[x][y].roundMineNumber != 0)
		{
			wsprintf(szBuffer, TEXT("%d"), box[x][y].roundMineNumber);
			DrawText(hdc, szBuffer, -1, &rect1, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
		}

		box[x][y].isOpened = true;
		numberOpened++;
		if (numberOpened == ROWS * COLS - mineNumber)
		{
			MessageBox(NULL, TEXT("祝贺你赢了!"), TEXT("扫雷"), MB_ICONWARNING);
		}
	}
}
//判断周围的所有box是否已经open
bool allRoundIsOpened(int x, int y)
{
	bool flag = true;
	if (x > 0 && !box[x - 1][y].isOpened)
		flag = false;
	if (x > 0 && y < COLS - 1 && !box[x - 1][y + 1].isOpened)
		flag = false;
	if (y < COLS - 1 && !box[x][y + 1].isOpened)
		flag = false;
	if (x < ROWS - 1 && y< COLS - 1 && !box[x + 1][y + 1].isOpened)
		flag = false;
	if (x < ROWS - 1 && !box[x + 1][y].isOpened)
		flag = false;
	if (x < ROWS - 1 && y > 0 && !box[x + 1][y - 1].isOpened)
		flag = false;
	if (y > 0 && !box[x][y - 1].isOpened)
		flag = false;
	if (x > 0 && y > 0 && !box[x - 1][y - 1].isOpened)
		flag = false;
	return flag;
}


