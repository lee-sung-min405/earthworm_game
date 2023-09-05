#include <windows.h>
#include <TCHAR.H>
#include <windows.h>

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow)
{
	HWND 	hwnd;
	MSG 	msg;
	WNDCLASS	WndClass;
	WndClass.style = CS_HREDRAW | CS_VREDRAW;
	WndClass.lpfnWndProc = WndProc;
	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hInstance = hInstance;
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	WndClass.lpszMenuName = NULL;
	WndClass.lpszClassName = _T("Window Class Name");
	RegisterClass(&WndClass);
	hwnd = CreateWindow(_T("Window Class Name"),
		_T("Window Title Name"),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		NULL,
		NULL,
		hInstance,
		NULL
	);
	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return (int)msg.wParam;
}


#define ITEMNUM 10 //������ ������ �������ش�.

static int board[22][22] = {}; //���� �� �׸���, ���� ���� -1 
static int wormX[30] = {}, wormY[30] = {}; //�߰��Ǵ� �ֹ��� ��ǥ ���� �迭 ����
int xDirect = 0, yDirect = 0, len = 0; //���� ��ġ ��ǥ ����

static TCHAR play_time[1024]; //���� �ð� ���ڿ� ���� ����
static int playtime_1 = 0, playtime_se = 0, playtime_me = 0; //�� �� �и��� ���� ����

static TCHAR point1[1024]; // ���� ���� ���ڿ� ���� ����
static int point_1 = 0; //���� ���� 0������ �ʱ�ȭ ����

static int gameover = 0; //���ӿ��� �ߴ��� ���ߴ��� �˷��ִ� �����̴�. ���� �������϶��� 0�ǰ���, ������ ���� �Ǿ������� 1�ǰ��� �־��ش�.
static int gameset = 0;//������ �ʱ�ȭ ���ֱ� ���� ���Ǵ� �����̴�. ���� �ʱ�ȭ�� �ʿ��� ��� 1�� ���� �־��ش�.

void ItemGenerator() //�׸� ������(���� ������)
{
	int i, x, y; //�ݺ����� ���� ���� i�� ���带 board���� ��ǥ���� �־��� x,y�� ���� ������ ������ �������ش�.

	if (gameset == 1) { //gameset�� ���� 1�ϋ� �ؿ� �κ��� ���� �����ش�
		for (y = 0; y < 22; y++) { // ������ �� �κ��� �ݺ� �����ش�
			for (x = 0; x < 22; x++) //������ �� �κ��� �ݺ� �����ش�.
			{
				switch (board[y][x])//board[y][x]�� ����� ���� �̿��Ͽ� switch�� �����Ѵ�. 
				{
				case 2:// ���� 2���� ���� �Ǿ�������  
					board[y][x] = 0; //���̰� �ִ� �ڸ��� ���� 0���� �ʱ�ȭ ���ش�.
					break;
				case 3: //���� 3�� ���� �Ǿ������� 
					board[y][x] = 0; // ���� �ǿ� �ִ� �ֹ����� ��ġ������ 0���� �ʱ�ȭ ���ش�.
					break;
				}
			}
		}
	}

	for (i = 0; i < ITEMNUM; i++) //10��ŭ �ݺ�
	{
		x = rand() % 20 + 1; //���� x��ǥ �������� ����
		y = rand() % 20 + 1; //���� y��ǥ �������� ����
		if (board[y][x] == 0) // ������ 0�� ��� ���ִ� ��ǥ ���� ���ǹ�
			board[y][x] = 2; // 0�� ���ִ� ������� ���� ���� 2�� ���� board x, y�±��� ���� �������ش�.

		else //���� ������ ��ǥ�� ����ִ� ���� 0�� �ƴ� ���
		{
			i = i - 1; // i�� ���� 1 ���־� �ش�i��°�� �ٽ� �ѹ� �ݺ� �����ֵ��� �����Ѵ�.
			continue;
		}
	}
	return;
}

void DrawGameBoard(HDC hdc) //���� ���� �׸���
{
	int i, x, y; //board�� ��ǥ�� x, y���� for������ ����� i���� �������ش�.
	for (y = 0; y < 22; y++) { // ������ �� �κ��� �ݺ� �����ش�
		for (x = 0; x < 22; x++) //������ �� �κ��� �ݺ� �����ش�.
		{
			switch (board[y][x]) //board[y][x]�� ����� ���� �̿��Ͽ� switch�� �����Ѵ�. 
			{
			case -1://���� -1�� ����Ǿ� �ִ� ���� ��
				Rectangle(hdc, x * 20, y * 20, (x + 1) * 20, (y + 1) * 20);// Rectangle�Լ��� �̿��Ͽ� ���� �ٱ� ��� �׷��ش�.
				break;
			case 2:// ���� 2�� ���� �Ǿ��ִ� ���� ��
				SelectObject(hdc, GetStockObject(BLACK_BRUSH)); //������ ����
				Ellipse(hdc, x * 20, y * 20, (x + 1) * 20, (y + 1) * 20); //Ellipse�Լ��� �̿��Ͽ� ���̸� �׷��ش�.
				SelectObject(hdc, GetStockObject(WHITE_BRUSH)); //������ ����
				break;

			}
		}
	}
	SelectObject(hdc, CreatePen(PS_SOLID, 2, RGB(255, 0, 0))); //���� �ֹ��� ����
	Ellipse(hdc, wormX[0] * 20, wormY[0] * 20, (wormX[0] + 1) * 20, (wormY[0] + 1) * 20); //���� �ֹ����� �׷��ش�.
	SelectObject(hdc, CreatePen(PS_SOLID, 2, RGB(0, 0, 255))); //�Ķ� �ֹ��� ����

	for (i = 1; i < len; i++) { //�ֹ����� ���� ��ŭ for���� �ݺ����ش�.
		point_1 = len - 2; //������ ���� �ֹ����� �渮�� ���� 2��ŭ ���Ƿ� �̰��� �̿��Ͽ� ������ ī��Ʈ ���ش�.
		Ellipse(hdc, wormX[i] * 20, wormY[i] * 20, (wormX[i] + 1) * 20, (wormY[i] + 1) * 20); //�߰� �Ǵ� �Ķ� �ֹ����� �׷��ش�.

		playtime_1++;//�ð��� ���� �����ش�.
		if (playtime_1 == 60) { //�и��ʰ� 60�� �Ǿ�����
			playtime_1 = 0; //�и��ʰ��� 0���� �ʱ�ȭ �����ش�.
			playtime_se++; //���� ���� 1���� �����ش�.
			if (playtime_se == 60) {//���� ���� 60�� �Ǿ��� ��
				playtime_se = 0;//���� ���� 0���� �ʱ�ȭ �����ش�.
				playtime_me++;//���� ���� 1���� �����ش�.
			}
		}
	}
}

void GameInit() //���� �ʱ�ȭ
{
	int i;
	for (i = 0; i < 22; i++) // 22�� �ݺ� �����ش�.
	{
		board[i][0] = -1; //���� �κ��� �ٱ���� ���� �κп� ���� -1�� �־��ش�
		board[i][21] = -1; //���� �κ��� �ٱ���� ������ �κп� ���� -1�� �־��ش�
		board[0][i] = -1; //���� �κ��� �ٱ���� ���� �κп� ���� -1�� �־��ش�
		board[21][i] = -1; //���� �κ��� �ٱ���� �Ʒ��� �κп� ���� -1�� �־��ش�
	}



	wormX[0] = 2; wormY[0] = 1; //ó�� ������ �� �ֹ��� �Ӹ� ��ä ��ǥ�� �־��ش�.
	wormX[1] = 1; wormY[1] = 1; //ó�� ������ �� �ֹ��� ù��° ���� ��ä ��ǥ�� �־��ش�.
	board[wormY[0]][wormX[0]] = 3; //ó�� ������ �� ���� �ֹ������ִ� board�� ���� 3�� �־��ش�
	board[wormY[1]][wormX[1]] = 3; //ó�� ������ �� �Ķ� �ֹ������ִ� board�� ���� 3�� �־��ش�

	ItemGenerator();

	len = 2; //ó�� �����Ҷ� �ֹ����� ������ 2�� ���� len�� �־��ش�.
	xDirect = 1; yDirect = 0; //ó�� �ֹ����� �����̴� ������ ������ �̱� ������ ���� xDirect = 1 yDirect = 0�� �־��ش�.
}


void DirectControl(int DirectKey) //�ֹ����� ���� ������ ���� ���ִ� �Լ�.
{
	switch (DirectKey) {

	case VK_LEFT:
		if (xDirect == 1) //���� �̵�
			break; //�ֹ����� �����̵��� �Ҷ� �ٷ� �� �� �ִ°��� �ƴ϶� ���Ƽ� �����ϱ� ������ break�� �ɾ��ش�.

		if (board[wormY[0]][wormX[0] - 1] != -1) //���� �ƴϸ� ��������
		{
			xDirect = -1; //�����̵��̱� ������ -1���� �־��ش�.
			yDirect = 0;
		}
		break;
	case VK_RIGHT: //������ �̵�
		if (xDirect == -1)
			break; //�ֹ����� �����̵��� �Ҷ� �ٷ� �� �� �ִ°��� �ƴ϶� ���Ƽ� �����ϱ� ������ break�� �ɾ��ش�.
		if (board[wormY[0]][wormX[0] + 1] != -1) //���� �ƴϸ� ��������
		{
			xDirect = 1; //�����̵��̱� ������ +1���� �־��ش�.
			yDirect = 0;
		}
		break;
	case VK_UP: //���� �̵�
		if (yDirect == 1)
			break; //�ֹ����� �����̵��� �Ҷ� �ٷ� �� �� �ִ°��� �ƴ϶� ���Ƽ� �����ϱ� ������ break�� �ɾ��ش�.
		if (board[wormY[0] - 1][wormX[0]] != -1) //���� �ƴϸ� ��������
		{
			xDirect = 0;
			yDirect = -1; //���� �̵��̱� ������ -1���� �־��ش�.
		}
		break;
	case VK_DOWN: //�Ʒ��� �̵�
		if (yDirect == -1)
			break; //�ֹ����� �����̵��� �Ҷ� �ٷ� �� �� �ִ°��� �ƴ϶� ���Ƽ� �����ϱ� ������ break�� �ɾ��ش�.
		if (board[wormY[0] + 1][wormX[0]] != -1) //���� �ƴϸ� ��������
		{
			xDirect = 0;
			yDirect = 1; //�Ʒ��� �̵��̱� ������ +1���� �־��ش�.
		}
		break;
	}
}



void MovingWorm() // �����̴� �ֹ���
{
	int tmpx, tmpy, i;

	tmpx = wormX[0]; //�ֹ����� �Ӹ���ǥx���� tmpx�� �־��ش�.
	tmpy = wormY[0]; //�ֹ����� �Ӹ���ǥy���� tmpy�� �־��ش�.
	wormX[0] += xDirect; wormY[0] += yDirect; // DirectControl�Լ��� ���� �����־� �ֹ����� ��ǥ�� ���� ���� �̵������ش�.

	if (board[wormY[0]][wormX[0]] == -1 || board[wormY[0]][wormX[0]] == 3) //���� ���̰ų� �ֹ��� ������ ���
	{
		wormX[0] = tmpx; //�ֹ����� �Ӹ� �� ��ġ x��ǥ�� ���� �ֹ����� �Ӹ���ǥ�� �־��� �������̰� �Ѵ�.
		wormY[0] = tmpy; //�ֹ����� �Ӹ� �� ��ġ y��ǥ�� ���� �ֹ����� �Ӹ���ǥ�� �־��� �������̰� �Ѵ�.
		gameover = 1;
	}
	else
	{
		if (board[wormY[0]][wormX[0]] == 2) //���� �ִ� ��ǥ�� �����ϰ� �Ǹ�
		{
			len = len + 1; //�ֹ����� ���̸� �߰����ش�
			board[wormY[0]][wormX[0]] = 0; // ���̰� 2�� 0���� �ʱ�ȭ �����ش�.
		}
		else
			board[wormY[len - 1]][wormX[len - 1]] = 0; //�ֹ����� ������ ������ ������ �ڸ��� ��ǥ ���� 0�� ���� �־��ش�. 

		for (i = len - 1; i > 1; i--) //�ֹ����� ���� ��ŭ �ݺ� �����ش�.
		{
			wormX[i] = wormX[i - 1]; //�ֹ����� ���ܰ� ��ǥx���� �ֹ����� �����ܰ� ��ǥ�� �־��ش�.
			wormY[i] = wormY[i - 1]; //�ֹ����� ���ܰ� ��ǥy���� �ֹ����� �����ܰ� ��ǥ�� �־��ش�.
		}
		wormX[1] = tmpx; //�ֹ����� 1�ܰ� �Ӹ��� �ֹ��� �Ӹ��� x��ġ�� �־��ش�.
		wormY[1] = tmpy; //�ֹ����� 1�ܰ� �Ӹ��� �ֹ��� �Ӹ��� y��ġ�� �־��ش�.
		board[wormY[0]][wormX[0]] = 3; //�ֹ����� �Ӹ��� �����ִ� ��ǥ�� 3�� �־��ش�.
	}
}


LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;

	SYSTEMTIME st;
	static TCHAR sTime[128] = _T("");  //���� �ð��� �����ϱ� ���� ���ڿ� ������ �������ش�.
	static RECT rt = { 100,100,400,120 }; //

	/////////////����Ű ������/////////////
	static int select = -1; //�Է� �޾����� �ش� if���� ���� ��Ű�� ���� ī���� ����
	RECT rect[4] = { {600,100,700,200}, {700,200,800,300}, {500,200,600,300}, {600,300,700,400} }; //����Ű�� �簢�� ��ǥ ���� ������
	TCHAR direct[4][10] = { _T("����"),_T("������"),_T("����") ,_T("�Ʒ���") }; //�簢������ ���� ,������, ����, �Ʒ��� �ؽ�Ʈ�� �����ϴ� �迭 ����
	HPEN hpen, oldpen; //�� ����
	HBRUSH hBrush, oldBrush; //�귯�� ����
	//////////////////////////////////////

	static TCHAR str[128] = _T("");//������ �������� �˷��ִ� �ؽ�ó�� �����ϱ� ���� char�� �迭 ����

	switch (iMsg) {
	case WM_CREATE:

		select = -1; //���� ����Ű���� �Է� ���� ���� ���� ������ �ʱⰪ�� -1�� ���� �־��ش�.
		GameInit(); //GameInit()�Լ��� ȣ���Ͽ��ش�.
		SetTimer(hwnd, 1, 100, NULL); //SetTime�� ������ �ش�.
		return 0;

	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		DrawGameBoard(hdc); //DrawGameBoard�Լ��� ȣ���� ���ش�.
		TextOut(hdc, 0, 450, sTime, _tcslen(sTime)); //���� �ð� ��ġ ���
		TextOut(hdc, 10, 10, str, _tcslen(str)); //�Ͻ� ����


		//���� ���� �޼���
		lstrcpy(str, _T("�˸�")); // ���� ��� , ���ο� ��� �˸�
		wsprintf(play_time, TEXT("�� �÷��� �ð� : %d�� %d��"), playtime_me, playtime_se);//playtime_me, playtime_se������ �̿��Ͽ� �ؽ�ó�� ������ش�.
		TextOut(hdc, 270, 450, play_time, lstrlen(play_time));//paly_time������ �̿��Ͽ� �� �÷��� �ð��� ������ش�.
		wsprintf(point1, TEXT("���� : %d��"), point_1);//point_1������ �̿��Ͽ� ���� ������ TEXT���ڵ��� ���� point1�� �������ش�
		TextOut(hdc, 450, 0, point1, lstrlen(point1)); //���� point1�� �̿��Ͽ� �ؽ�ó�� ������ش�.

		//���� �˸�â
		Rectangle(hdc, 2, 500, 440, 700); //�˸�â�� ������ Recatangle�� �ش� ��ǥ�� �̿��Ͽ� ���簢���� ������ش�.
		TextOut(hdc, 5, 499, _T("���� �˸�â"), _tcslen(_T("���� �˸�â"))); //�ش� ��ǥ�� �ؽ�ó�� ������ش�.
		TextOut(hdc, 5, 580, _T("���� ���"), _tcslen(_T("���� ���"))); //�ش� ��ǥ�� �ؽ�ó�� ������ش�.
		TextOut(hdc, 5, 600, _T("����Ű : ��, �Ʒ� , ��, ��"), _tcslen(_T("����Ű : ��, �Ʒ� , ��, ��")));//�ش� ��ǥ�� �ؽ�ó�� ������ش�.
		TextOut(hdc, 5, 620, _T("��� ���� : 1(��), 2(��), 3(��)"), _tcslen(_T("��� ���� : 1(��), 2(��), 3(��)")));//�ش� ��ǥ�� �ؽ�ó�� ������ش�.
		TextOut(hdc, 5, 640, _T("���ο� ��� : ���콺 ����(���ο� ���)���� �� ���̵� �� ����"), _tcslen(_T("���ο� ��� : ���콺 ����(���ο� ���)���� �� ���̵� �� ����")));//�ش� ��ǥ�� �ؽ�ó�� ������ش�.
		TextOut(hdc, 5, 660, _T("���� ���� �� ����� ��� : ENTER"), _tcslen(_T("���� ���� �� ����� ��� : ENTER")));//�ش� ��ǥ�� �ؽ�ó�� ������ش�.

		switch (point_1) //point_1�� ����� ���� ���� case���� ������ �ش�
		{
		case 0://point_1�� ����� ���� 0 �ϋ�
			TextOut(hdc, 50, 520, _T("���̸� �Ծ� �ּ���"), _tcslen(_T("���̸� �Ծ� �ּ���"))); //�ش� ��ǥ�� �ؽ�ó�� ������ش�
			break;
		case 1://point_1�� ����� ���� 1 �ϋ�
			TextOut(hdc, 50, 520, _T("���� ������ �� : 1/10"), _tcslen(_T("���� ������ �� : 1/10")));//�ش� ��ǥ�� �ؽ�ó�� ������ش�
			break;
		case 2://point_1�� ����� ���� 2 �ϋ�
			TextOut(hdc, 50, 520, _T("���� ������ �� : 2/10"), _tcslen(_T("���� ������ �� : 2/10")));//�ش� ��ǥ�� �ؽ�ó�� ������ش�
			break;
		case 3://point_1�� ����� ���� 3 �ϋ�
			TextOut(hdc, 50, 520, _T("���� ������ �� : 3/10"), _tcslen(_T("���� ������ �� : 3/10")));//�ش� ��ǥ�� �ؽ�ó�� ������ش�
			break;
		case 4://point_1�� ����� ���� 4 �ϋ�
			TextOut(hdc, 50, 520, _T("���� ������ �� : 4/10"), _tcslen(_T("���� ������ �� : 4/10")));//�ش� ��ǥ�� �ؽ�ó�� ������ش�
			break;
		case 5://point_1�� ����� ���� 5 �ϋ�
			TextOut(hdc, 50, 520, _T("���� ������ �� : 5/10"), _tcslen(_T("���� ������ �� : 5/10")));//�ش� ��ǥ�� �ؽ�ó�� ������ش�
			break;
		case 6://point_1�� ����� ���� 6 �ϋ�
			TextOut(hdc, 50, 520, _T("���� ������ �� : 6/10"), _tcslen(_T("���� ������ �� : 6/10")));//�ش� ��ǥ�� �ؽ�ó�� ������ش�
			break;
		case 7://point_1�� ����� ���� 7 �ϋ�
			TextOut(hdc, 50, 520, _T("���� ������ �� : 7/10"), _tcslen(_T("���� ������ �� : 7/10")));//�ش� ��ǥ�� �ؽ�ó�� ������ش�
			break;
		case 8://point_1�� ����� ���� 8 �ϋ�
			TextOut(hdc, 50, 520, _T("���� ������ �� : 8/10"), _tcslen(_T("���� ������ �� : 8/10")));//�ش� ��ǥ�� �ؽ�ó�� ������ش�
			break;
		case 9://point_1�� ����� ���� 9 �ϋ�
			TextOut(hdc, 50, 520, _T("���� ������ �� :  9/10 * ������ ���̰� ���ҽ��ϴ�."), _tcslen(_T("���� ������ �� : 9/10 * ������ ���̰� ���ҽ��ϴ�.")));//�ش� ��ǥ�� �ؽ�ó�� ������ش�
			break;
		case 10://point_1�� ����� ���� 10 �ϋ�
			TextOut(hdc, 50, 520, _T("���� Ŭ����"), _tcslen(_T("���� Ŭ����")));//�ش� ��ǥ�� �ؽ�ó�� ������ش�
			break;
		default:
			break;
		}

		//���� ���� ���â
		if (gameover == 1) { //gameover������ ���� 1�̸�,
			TextOut(hdc, 50, 520, _T("���� ����             [�ٽ� ���� : Enter]"), _tcslen(_T("���� ����             [�ٽ� ���� : Enter]")));//�ش� ��ǥ�� �ؽ�ó�� ������ش�
			KillTimer(hwnd, 1);//Ÿ�̸Ӹ� ���δ�.
		}
		else if (gameover == 0) { //gameover������ ���� 1�̰�
			if (gameset == 1) { //gameset������ ���� 1�̸�
				select = -1; //���� ����Ű���� �Է� ���� ���� ���� ������ �ʱⰪ�� -1�� ���� �־��ش�.
				GameInit(); //GameInit()�Լ��� ȣ���Ͽ��ش�.
				SetTimer(hwnd, 1, 100, NULL);//SetTime�� ������ �ش�.
				gameset = 0; //gameset������ 0�� �־��ش�. ������ ���µǾ��� �����̴�.
			}
		}

		//����Ű �ڵ�
		TextOut(hdc, 610, 50, _T("����Ű ǥ��"), _tcslen(_T("����Ű ǥ��"))); //�ش� ��ǥ�� �ؽ�ó�� ������ش�.
		for (int i = 0; i < 5; i++) {//rect�迭�� �ε����� ���� ���Ͽ� for���� ����Ͽ���.
			Rectangle(hdc, rect[i].left, rect[i].top, rect[i].right, rect[i].bottom); //rect �迭�� ����� �簢������ ��ǥ�� �ݺ����� ���Ͽ� ���� ������ش�. 
			DrawText(hdc, direct[i], _tcslen(direct[i]), &rect[i], DT_SINGLELINE | DT_CENTER | DT_VCENTER); //�ش� ��ǥ�� �ؽ�ó�� ������ش�.
		}
		if (select == 0) { //select������ ���� 0�ϋ�
			hBrush = CreateSolidBrush(RGB(225, 0, 0)); //���� ����
			oldBrush = (HBRUSH)SelectObject(hdc, hBrush); //���� ��
			Rectangle(hdc, rect[2].left, rect[2].top, rect[2].right, rect[2].bottom); //�ش���ǥ�� Rectangle���
			SelectObject(hdc, oldBrush);//������ ������ ���ư���
			DeleteObject(hBrush);
		}
		else if (select == 1) {
			hBrush = CreateSolidBrush(RGB(225, 0, 0)); //���� ����
			oldBrush = (HBRUSH)SelectObject(hdc, hBrush); //���� ��
			Rectangle(hdc, rect[0].left, rect[0].top, rect[0].right, rect[0].bottom);//�ش���ǥ�� Rectangle���
			SelectObject(hdc, oldBrush);//������ ������ ���ư���
			DeleteObject(hBrush);
		}
		else if (select == 2) {
			hBrush = CreateSolidBrush(RGB(225, 0, 0)); //���� ����
			oldBrush = (HBRUSH)SelectObject(hdc, hBrush); //���� ��
			Rectangle(hdc, rect[1].left, rect[1].top, rect[1].right, rect[1].bottom);//�ش���ǥ�� Rectangle���
			SelectObject(hdc, oldBrush);//������ ������ ���ư���
			DeleteObject(hBrush);
		}
		else if (select == 3) {
			hBrush = CreateSolidBrush(RGB(225, 0, 0)); //���� ����
			oldBrush = (HBRUSH)SelectObject(hdc, hBrush); //���� ��
			Rectangle(hdc, rect[3].left, rect[3].top, rect[3].right, rect[3].bottom);//�ش���ǥ�� Rectangle���
			SelectObject(hdc, oldBrush);//������ ������ ���ư���
			DeleteObject(hBrush);
		}
		else if (select == -1) { //���� -1�� ���� �϶�
			for (int i = 0; i < 5; i++) { ////rect�迭�� �ε����� ���� ���Ͽ� for���� ����Ͽ���.
				Rectangle(hdc, rect[i].left, rect[i].top, rect[i].right, rect[i].bottom); //rect �迭�� ����� �簢������ ��ǥ�� �ݺ����� ���Ͽ� ���� ������ش�. 
				DrawText(hdc, direct[i], _tcslen(direct[i]), &rect[i], DT_SINGLELINE | DT_CENTER | DT_VCENTER);//�ش� ��ǥ�� �ؽ�ó�� ������ش�.
			}
		}
		EndPaint(hwnd, &ps);
		break;

	case WM_LBUTTONDOWN: //���콺 ������ ������ ��
		lstrcpy(str, _T("��� : ���ο� ���"));//�ش� ��ǥ�� �ؽ�ó�� ������ش�.
		InvalidateRect(hwnd, NULL, TRUE);
		SetTimer(hwnd, 1, 1000, NULL); //Ÿ�̸��� �ӵ��� �÷��ش�.
		return 0;
	case WM_LBUTTONUP: //���콺 ������ ���� ��
		lstrcpy(str, _T("���̵� : ��"));//�ش� ��ǥ�� �ؽ�ó�� ������ش�.
		InvalidateRect(hwnd, NULL, TRUE);
		SetTimer(hwnd, 1, 100, NULL);//Ÿ�̸��� �ӵ��� (���̵� : ��) ���·� �����ش�.
		return 0;
	case WM_KEYDOWN: //�ش�Ű�� ������ ��
		DirectControl((int)wParam);
		switch (wParam) {
		case VK_LEFT://����Ű�� ������ ��
			if (gameover == 0) {
				select = 0;
			}
			break;

		case  VK_UP://����Ű�� ������ ��
			if (gameover == 0) {
				select = 1;
			}
			break;

		case VK_RIGHT://������ Ű�� ������ ��
			if (gameover == 0) {
				select = 2;
			}
			break;

		case VK_DOWN://�Ʒ��� Ű�� ������ ��
			if (gameover == 0) {
				select = 3;
			}
			break;

		case VK_RETURN://���Ͱ��� Ű�� ������ ��
			if (gameover == 1) { //���� gameover���� 1�̸�
				gameset = 1; //gameset���� 1�� �־��ش�.
				gameover = 0; //gameover���� �� �־��ش�.
			}
			break;
		case VK_NUMPAD1: //����Ű 1�� ������ ��
			lstrcpy(str, _T("���̵� : ��")); //�ش� ��ǥ�� �ؽ�ó�� ���
			InvalidateRect(hwnd, NULL, TRUE);
			SetTimer(hwnd, 1, 500, NULL); //���̵� �Ͽ� ���� SetTimer�� �ӵ��� ����
			break;

		case VK_NUMPAD2: //����Ű 2�� ������ ��
			lstrcpy(str, _T("���̵� : ��")); //�ش� ��ǥ�� �ؽ�ó�� ���
			InvalidateRect(hwnd, NULL, TRUE);
			SetTimer(hwnd, 1, 100, NULL);//���̵� �߿� ���� SetTimer�� �ӵ��� ����
			break;

		case VK_NUMPAD3: //����Ű 3�� ������ ��
			lstrcpy(str, _T("���̵� : ��")); //�ش� ��ǥ�� �ؽ�ó�� ���
			InvalidateRect(hwnd, NULL, TRUE);
			SetTimer(hwnd, 1, 50, NULL);//���̵� �� ���� SetTimer�� �ӵ��� ����
			break;
		dafault: //���� ��� Ű���� �ƴҶ�
			select = -1; //select������ -1�� ���� �־��ش�.
			break;
		}
		InvalidateRgn(hwnd, NULL, TRUE);
		break;

	case WM_KEYUP: //�ش� Ű���� ���� ������
		switch (wParam) {
		case VK_LEFT://����Ű�� ���� ��
			select = -1;
			break;
		case  VK_UP://����Ű�� ���� ��
			select = -1;
			break;
		case VK_RIGHT://������Ű�� ���� ��
			select = -1;
			break;
		case VK_DOWN://�Ʒ���Ű�� ���� ��
			select = -1;
			break;
		}
		InvalidateRgn(hwnd, NULL, TRUE);
		break;

	case WM_TIMER: //Ÿ�̸Ӱ� �ڵ��Ҷ�
		MovingWorm();
		switch (wParam) {
		case 1:
			GetLocalTime(&st);
			_stprintf_s(sTime, _T("���� �ð� : %d:%d:%d�Դϴ�"), st.wHour, st.wMinute, st.wSecond); //���� �ð���st.wHour, st.wMinute, st.wSecond ����� ���� ���ڸ� �Բ� ������ش�.			InvalidateRect(hwnd, &rt, TRUE);
			break;
		case 2:
			MessageBeep(0);
			break;
		}
		InvalidateRgn(hwnd, NULL, TRUE);
		break;
	case WM_DESTROY:
		KillTimer(hwnd, 1);
		PostQuitMessage(0);
		return 0;
	}
	return(DefWindowProc(hwnd, iMsg, wParam, lParam));
}