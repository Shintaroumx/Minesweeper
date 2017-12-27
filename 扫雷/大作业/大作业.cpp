// ����ҵ.cpp : ����Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "����ҵ.h"

//ȫ�ֱ�����
//#define MAX_LOADSTRING 100
#define SHADOW_WIDTH1 1 //��Ӱ���
#define SHADOW_WIDTH2 2 
#define SHADOW_WIDTH3 3 
#define BORDER 8 //�߽�Ŀ��
#define BOX_WIDTH 20//box�Ŀ��
#define BOX_HEIGHT 20 //box�ĸ߶�
#define RESERVED_AREA 32 //�������߶�

//�Զ��庯��������
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void DrawBorder(HDC hdc, int x, int y, int width, int height, int BorderWidth, COLORREF &color1, COLORREF &color2, bool WTop);
void AutoOpenBoxes(HDC &hdc, int i, int j);
void drawRect(HDC &hdc, int x, int y);
bool allRoundIsOpened(int row, int col);
void InitBox();
void UpdateTheWindow(HWND &hwnd, int rows, int cols, int mineNO);
void ComputeRoundMineNo();
void RandomSetMines();

//�ṹ��Box,roundMineNumber��ʾ��Χ�׵ĸ���
//containMine��ʾ�Ƿ������Ƿ����ף�
//isOpened��ʾbox�Ƿ��Ѿ���
static HMENU hMenu;
struct Box
{
	int roundMineNumber;
	bool containMine;
	bool isOpened;
	bool isMarked;
};
static int mineNumber;
static int ROWS; //����
static int COLS; //���� 
static int iSM_BorderX;
static int iSM_BorderY;
static int iSM_CaptionY;
static int iSM_MenuY;
static Box **box;
static int i, j, numberOpened;
static RECT mineRect;

// ȫ�ֱ���: 
HINSTANCE hInst;                                // ��ǰʵ��
//LPCSTR szTitle;                  // �������ı�
//LPCSTR szWindowClass;            // ����������

// �˴���ģ���а����ĺ�����ǰ������: 
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

    // TODO: �ڴ˷��ô��롣
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


    // ��ʼ��ȫ���ַ���
    //LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    //LoadStringW(hInstance, IDC_MY, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // ִ��Ӧ�ó����ʼ��: 
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MY));


    // ����Ϣѭ��: 
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
//  ����: MyRegisterClass()
//
//  Ŀ��: ע�ᴰ���ࡣ
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
//   ����: InitInstance(HINSTANCE, int)
//
//   Ŀ��: ����ʵ�����������������
//
//   ע��: 
//
//        �ڴ˺����У�������ȫ�ֱ����б���ʵ�������
//        ��������ʾ�����򴰿ڡ�
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	static TCHAR szAppName[] = TEXT("SineWave");
	HWND hwnd;
	hInst = hInstance; // ��ʵ������洢��ȫ�ֱ�����
   int cx = BOX_WIDTH * COLS + 2 * SHADOW_WIDTH3 + 2 * SHADOW_WIDTH2 + 2 * iSM_BorderX + 2 * BORDER;
   int cy = BOX_HEIGHT * ROWS + 2 * SHADOW_WIDTH3 + 4 * SHADOW_WIDTH2 + 2 * iSM_BorderY + 3 * BORDER + RESERVED_AREA + iSM_CaptionY + iSM_MenuY;
   hwnd = CreateWindow(szAppName, TEXT("ɨ��"),
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
//  ����: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  Ŀ��:    ���������ڵ���Ϣ��
//
//  WM_COMMAND  - ����Ӧ�ó���˵�
//  WM_PAINT    - ����������
//  WM_DESTROY  - �����˳���Ϣ������
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
	static COLORREF color1 = RGB(0, 0, 0),//��ɫ
		color2 = RGB(255, 255, 255),//��ɫ
		color3 = RGB(200, 200, 200),//��ɫ
		color4 = RGB(120, 120, 200);//����
	static RECT grid, rect;
	static char szBuffer[2];
	POINT pt;

	switch (message)
    {
	case WM_CREATE:
		//��ʼ������
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
            // TODO: �ڴ˴����ʹ�� hdc ���κλ�ͼ����...
			GetClientRect(hWnd, &rect);
			hBrush = CreateSolidBrush(RGB(200, 200, 200));
			SetBkColor(hdc, RGB(200, 200, 200));
			SelectObject(hdc, hBrush);
			SelectObject(hdc, GetStockObject(NULL_PEN));
			Rectangle(hdc, 0, 0, rect.right, rect.bottom);
			//������߽�
			DrawBorder(hdc, 0, 0, rect.right, rect.bottom, SHADOW_WIDTH3, color1, color2, true);
			//���Ʊ�����
			DrawBorder(hdc, SHADOW_WIDTH3 + BORDER, SHADOW_WIDTH3 + BORDER,
				BOX_WIDTH * COLS + 2 * SHADOW_WIDTH2, RESERVED_AREA + 2 * SHADOW_WIDTH2, SHADOW_WIDTH2, color1, color2, false);
			//�����ڱ߽�
			DrawBorder(hdc, SHADOW_WIDTH3 + BORDER, SHADOW_WIDTH3 + 2 * BORDER + RESERVED_AREA + 2 * SHADOW_WIDTH2,
				BOX_WIDTH * COLS + 2 * SHADOW_WIDTH2, BOX_HEIGHT*ROWS + 2 * SHADOW_WIDTH2, SHADOW_WIDTH2, color1, color2, false);
			//����������box
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
							//�����Χ�׵ĸ���
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
					//�����Χ�׵ĸ���
					DrawText(hdc, szBuffer, -1, &rect1, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
				}

				box[i][j].isOpened = true;
				numberOpened++;
				if (numberOpened == ROWS * COLS - mineNumber)
				{
					MessageBox(hWnd, TEXT("ף����Ӯ��!"), TEXT("ɨ��"), MB_ICONWARNING);
				}

				AutoOpenBoxes(hdc, i, j);
			}
			else
			{
				for (int k = 0; k<ROWS; k++)
					for (int l = 0; l<COLS; l++)
					{
						if (box[k][l].containMine)//�㵽�׺�,ʹ���е�������
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
				MessageBox(hWnd, TEXT("��Ϸ����"), TEXT("ɨ��"), MB_ICONWARNING);
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

			if (PtInRect(&rect, pt))//�ж��Ƿ�������
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
				if (!box[m][n].isOpened)//������ߺ�,�ָ�δ����
				{
					DrawBorder(hdc, grid.left, grid.top, BOX_WIDTH - 2, BOX_HEIGHT - 2, SHADOW_WIDTH2, color1, color2, true);
					DrawBorder(hdc, grid.left, grid.top, BOX_WIDTH, BOX_HEIGHT, SHADOW_WIDTH1, color3, color3, true);
				}
				//ʹ��ǰ��box��Ϊ����״̬
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

// �����ڡ������Ϣ�������
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

//���Ʊ߽�
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
//��ʼ��box
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
//������Χ�׵ĸ���
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
//�������
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
//�Զ���box
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
//���´���
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
//���ƾ���
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
			MessageBox(NULL, TEXT("ף����Ӯ��!"), TEXT("ɨ��"), MB_ICONWARNING);
		}
	}
}
//�ж���Χ������box�Ƿ��Ѿ�open
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


