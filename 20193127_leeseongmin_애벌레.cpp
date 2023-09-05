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


#define ITEMNUM 10 //먹이의 갯수를 저장해준다.

static int board[22][22] = {}; //보드 벽 그리기, 벽의 값은 -1 
static int wormX[30] = {}, wormY[30] = {}; //추가되는 애벌레 좌표 저장 배열 변수
int xDirect = 0, yDirect = 0, len = 0; //현재 위치 좌표 변수

static TCHAR play_time[1024]; //현재 시간 문자열 저장 변수
static int playtime_1 = 0, playtime_se = 0, playtime_me = 0; //분 초 밀리초 저장 변수

static TCHAR point1[1024]; // 현재 점수 문자여 저장 변수
static int point_1 = 0; //현재 점수 0점으로 초기화 변수

static int gameover = 0; //게임오버 했는지 안했는지 알려주는 변수이다. 게임 진행중일때는 0의값이, 게임이 오버 되었을때는 1의값을 넣어준다.
static int gameset = 0;//게임을 초기화 해주기 위해 사용되는 변수이다. 게임 초기화가 필요한 경우 1의 값을 넣어준다.

void ItemGenerator() //항목 생성기(먹이 생성기)
{
	int i, x, y; //반복문을 위한 변수 i와 보드를 board값의 좌표값을 넣어줄 x,y의 값을 저장할 변수를 선언해준다.

	if (gameset == 1) { //gameset의 값이 1일떄 밑에 부분을 실행 시켜준다
		for (y = 0; y < 22; y++) { // 보더의 행 부분을 반복 시켜준다
			for (x = 0; x < 22; x++) //보더의 열 부분을 반복 시켜준다.
			{
				switch (board[y][x])//board[y][x]에 저장된 값을 이용하여 switch를 수행한다. 
				{
				case 2:// 값이 2으로 저장 되어있을떄  
					board[y][x] = 0; //먹이가 있던 자리를 전부 0으로 초기화 해준다.
					break;
				case 3: //값이 3로 저장 되어있을때 
					board[y][x] = 0; // 저번 판에 있던 애벌레의 위치값들을 0으로 초기화 해준다.
					break;
				}
			}
		}
	}

	for (i = 0; i < ITEMNUM; i++) //10만큼 반복
	{
		x = rand() % 20 + 1; //먹이 x좌표 랜덤으로 생성
		y = rand() % 20 + 1; //먹이 y좌표 랜덤으로 생성
		if (board[y][x] == 0) // 보더에 0이 들어 가있는 좌표 값인 조건문
			board[y][x] = 2; // 0이 들어가있는 빈공간에 먹이 값인 2의 값을 board x, y좌교에 값을 저장해준다.

		else //만약 보더의 좌표에 들어있는 값이 0이 아닐 경우
		{
			i = i - 1; // i의 값을 1 빼주어 해당i번째를 다시 한번 반복 시켜주도록 유도한다.
			continue;
		}
	}
	return;
}

void DrawGameBoard(HDC hdc) //게임 보드 그리기
{
	int i, x, y; //board의 좌표값 x, y값과 for문에서 사용할 i값을 선언해준다.
	for (y = 0; y < 22; y++) { // 보더의 행 부분을 반복 시켜준다
		for (x = 0; x < 22; x++) //보더의 열 부분을 반복 시켜준다.
		{
			switch (board[y][x]) //board[y][x]에 저장된 값을 이용하여 switch를 수행한다. 
			{
			case -1://값이 -1이 저장되어 있는 값일 때
				Rectangle(hdc, x * 20, y * 20, (x + 1) * 20, (y + 1) * 20);// Rectangle함수를 이용하여 보드 바깥 경계 그려준다.
				break;
			case 2:// 값이 2로 저장 되어있는 값일 때
				SelectObject(hdc, GetStockObject(BLACK_BRUSH)); //먹이의 색깔
				Ellipse(hdc, x * 20, y * 20, (x + 1) * 20, (y + 1) * 20); //Ellipse함수를 이용하여 먹이를 그려준다.
				SelectObject(hdc, GetStockObject(WHITE_BRUSH)); //먹이의 색깔
				break;

			}
		}
	}
	SelectObject(hdc, CreatePen(PS_SOLID, 2, RGB(255, 0, 0))); //빨강 애벌레 색깔
	Ellipse(hdc, wormX[0] * 20, wormY[0] * 20, (wormX[0] + 1) * 20, (wormY[0] + 1) * 20); //빨강 애벌레를 그려준다.
	SelectObject(hdc, CreatePen(PS_SOLID, 2, RGB(0, 0, 255))); //파랑 애벌레 색깔

	for (i = 1; i < len; i++) { //애벌레의 길이 만큼 for문을 반복해준다.
		point_1 = len - 2; //점수의 차와 애벌레의 길리의 차가 2만큼 나므로 이것을 이용하여 점수를 카운트 해준다.
		Ellipse(hdc, wormX[i] * 20, wormY[i] * 20, (wormX[i] + 1) * 20, (wormY[i] + 1) * 20); //추가 되는 파랑 애벌래를 그려준다.

		playtime_1++;//시간을 증가 시켜준다.
		if (playtime_1 == 60) { //밀리초가 60이 되었을때
			playtime_1 = 0; //밀리초값을 0으로 초기화 시켜준다.
			playtime_se++; //초의 값을 1증가 시켜준다.
			if (playtime_se == 60) {//초의 값이 60이 되었을 때
				playtime_se = 0;//초의 값을 0으로 초기화 시켜준다.
				playtime_me++;//분의 값을 1증가 시켜준다.
			}
		}
	}
}

void GameInit() //게임 초기화
{
	int i;
	for (i = 0; i < 22; i++) // 22번 반복 시켜준다.
	{
		board[i][0] = -1; //보더 부분의 바깥경게 왼쪽 부분에 전부 -1를 넣어준다
		board[i][21] = -1; //보더 부분의 바깥경게 오른쪽 부분에 전부 -1를 넣어준다
		board[0][i] = -1; //보더 부분의 바깥경게 위쪽 부분에 전부 -1를 넣어준다
		board[21][i] = -1; //보더 부분의 바깥경게 아래쪽 부분에 전부 -1를 넣어준다
	}



	wormX[0] = 2; wormY[0] = 1; //처음 시작할 때 애벌레 머리 본채 좌표를 넣어준다.
	wormX[1] = 1; wormY[1] = 1; //처음 시작할 때 애벌레 첫번째 몸통 본채 좌표를 넣어준다.
	board[wormY[0]][wormX[0]] = 3; //처음 시작할 때 빨강 애벌레가있는 board의 값에 3을 넣어준다
	board[wormY[1]][wormX[1]] = 3; //처음 시작할 떄 파랑 애벌레가있는 board의 값에 3을 넣어준다

	ItemGenerator();

	len = 2; //처음 시작할때 애벌레의 길이인 2의 값을 len에 넣어준다.
	xDirect = 1; yDirect = 0; //처음 애벌레의 움직이는 방향이 오른쪽 이기 때문에 각각 xDirect = 1 yDirect = 0를 넣어준다.
}


void DirectControl(int DirectKey) //애벌레의 진행 방향을 설정 해주는 함수.
{
	switch (DirectKey) {

	case VK_LEFT:
		if (xDirect == 1) //왼쪽 이동
			break; //애벌레가 방향이동을 할때 바로 갈 수 있는것이 아니라 돌아서 가야하기 때문에 break를 걸어준다.

		if (board[wormY[0]][wormX[0] - 1] != -1) //벽이 아니면 움직여라
		{
			xDirect = -1; //왼쪽이동이기 때문에 -1값을 넣어준다.
			yDirect = 0;
		}
		break;
	case VK_RIGHT: //오른쪽 이동
		if (xDirect == -1)
			break; //애벌레가 방향이동을 할때 바로 갈 수 있는것이 아니라 돌아서 가야하기 때문에 break를 걸어준다.
		if (board[wormY[0]][wormX[0] + 1] != -1) //벽이 아니면 움직여라
		{
			xDirect = 1; //왼쪽이동이기 때문에 +1값을 넣어준다.
			yDirect = 0;
		}
		break;
	case VK_UP: //위로 이동
		if (yDirect == 1)
			break; //애벌레가 방향이동을 할때 바로 갈 수 있는것이 아니라 돌아서 가야하기 때문에 break를 걸어준다.
		if (board[wormY[0] - 1][wormX[0]] != -1) //벽이 아니면 움직여라
		{
			xDirect = 0;
			yDirect = -1; //위쪽 이동이기 때문에 -1값을 넣어준다.
		}
		break;
	case VK_DOWN: //아래로 이동
		if (yDirect == -1)
			break; //애벌레가 방향이동을 할때 바로 갈 수 있는것이 아니라 돌아서 가야하기 때문에 break를 걸어준다.
		if (board[wormY[0] + 1][wormX[0]] != -1) //벽이 아니면 움직여라
		{
			xDirect = 0;
			yDirect = 1; //아래로 이동이기 때문에 +1값을 넣어준다.
		}
		break;
	}
}



void MovingWorm() // 움직이는 애벌레
{
	int tmpx, tmpy, i;

	tmpx = wormX[0]; //애벌레의 머리좌표x값을 tmpx에 넣어준다.
	tmpy = wormY[0]; //애벌레의 머리좌표y값을 tmpy에 넣어준다.
	wormX[0] += xDirect; wormY[0] += yDirect; // DirectControl함수의 값을 더해주어 애벌레의 좌표를 변동 시켜 이동시켜준다.

	if (board[wormY[0]][wormX[0]] == -1 || board[wormY[0]][wormX[0]] == 3) //만약 벽이거나 애벌레 몸통일 경우
	{
		wormX[0] = tmpx; //애벌레의 머리 전 위치 x좌표를 현재 애벌레의 머리좌표에 넣어줘 못움직이게 한다.
		wormY[0] = tmpy; //애벌레의 머리 전 위치 y좌표를 현재 애벌레의 머리좌표에 넣어줘 못움직이게 한다.
		gameover = 1;
	}
	else
	{
		if (board[wormY[0]][wormX[0]] == 2) //먹이 있는 좌표에 도착하게 되면
		{
			len = len + 1; //애벌레의 길이를 추가해준다
			board[wormY[0]][wormX[0]] = 0; // 먹이값 2를 0으로 초기화 시켜준다.
		}
		else
			board[wormY[len - 1]][wormX[len - 1]] = 0; //애벌레의 마지막 꼬리가 지나간 자리의 좌표 값에 0의 값을 넣어준다. 

		for (i = len - 1; i > 1; i--) //애버레의 길이 만큼 반복 새켜준다.
		{
			wormX[i] = wormX[i - 1]; //애벌레의 전단계 좌표x값을 애벌레의 다음단계 좌표에 넣어준다.
			wormY[i] = wormY[i - 1]; //애벌레의 전단계 좌표y값을 애벌레의 다음단계 좌표에 넣어준다.
		}
		wormX[1] = tmpx; //애벌레의 1단계 머리에 애벌레 머리의 x위치를 넣어준다.
		wormY[1] = tmpy; //애벌레의 1단계 머리에 애벌레 머리의 y위치를 넣어준다.
		board[wormY[0]][wormX[0]] = 3; //애벌레의 머리가 가고있는 좌표에 3을 넣어준다.
	}
}


LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;

	SYSTEMTIME st;
	static TCHAR sTime[128] = _T("");  //현재 시간을 저장하기 위한 문자열 변수를 저장해준다.
	static RECT rt = { 100,100,400,120 }; //

	/////////////방향키 변수들/////////////
	static int select = -1; //입력 받았을때 해당 if문을 실행 시키기 위한 카운터 변수
	RECT rect[4] = { {600,100,700,200}, {700,200,800,300}, {500,200,600,300}, {600,300,700,400} }; //방향키의 사각형 좌표 저장 변수들
	TCHAR direct[4][10] = { _T("위쪽"),_T("오른쪽"),_T("왼쪽") ,_T("아래쪽") }; //사각형위에 위쪽 ,오른쪽, 왼쪽, 아래쪽 텍스트를 저장하는 배열 변수
	HPEN hpen, oldpen; //펜 생성
	HBRUSH hBrush, oldBrush; //브러쉬 생성
	//////////////////////////////////////

	static TCHAR str[128] = _T("");//게임의 정보들을 알려주는 텍스처를 저장하기 위한 char형 배열 변수

	switch (iMsg) {
	case WM_CREATE:

		select = -1; //현재 방향키에서 입력 받은 것이 없기 때문에 초기값을 -1로 값을 넣어준다.
		GameInit(); //GameInit()함수를 호출하여준다.
		SetTimer(hwnd, 1, 100, NULL); //SetTime을 설정해 준다.
		return 0;

	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		DrawGameBoard(hdc); //DrawGameBoard함수를 호출을 해준다.
		TextOut(hdc, 0, 450, sTime, _tcslen(sTime)); //현재 시간 위치 출력
		TextOut(hdc, 10, 10, str, _tcslen(str)); //일시 정지


		//게임 상태 메세지
		lstrcpy(str, _T("알림")); // 게임 모드 , 슬로우 모션 알림
		wsprintf(play_time, TEXT("총 플레이 시간 : %d분 %d초"), playtime_me, playtime_se);//playtime_me, playtime_se변수를 이용하여 텍스처를 출력해준다.
		TextOut(hdc, 270, 450, play_time, lstrlen(play_time));//paly_time변수를 이용하여 총 플레이 시간을 출력해준다.
		wsprintf(point1, TEXT("점수 : %d점"), point_1);//point_1변수를 이용하여 현재 점수와 TEXT문자들을 변수 point1에 저장해준다
		TextOut(hdc, 450, 0, point1, lstrlen(point1)); //변수 point1을 이용하여 텍스처를 출력해준다.

		//게임 알림창
		Rectangle(hdc, 2, 500, 440, 700); //알림창의 범위를 Recatangle을 해당 좌표에 이용하여 직사각형을 출력해준다.
		TextOut(hdc, 5, 499, _T("게임 알림창"), _tcslen(_T("게임 알림창"))); //해당 좌표에 텍스처를 출력해준다.
		TextOut(hdc, 5, 580, _T("게임 방법"), _tcslen(_T("게임 방법"))); //해당 좌표에 텍스처를 출력해준다.
		TextOut(hdc, 5, 600, _T("방향키 : 위, 아래 , 좌, 우"), _tcslen(_T("방향키 : 위, 아래 , 좌, 우")));//해당 좌표에 텍스처를 출력해준다.
		TextOut(hdc, 5, 620, _T("모드 번경 : 1(하), 2(중), 3(상)"), _tcslen(_T("모드 번경 : 1(하), 2(중), 3(상)")));//해당 좌표에 텍스처를 출력해준다.
		TextOut(hdc, 5, 640, _T("슬로우 모션 : 마우스 왼쪽(슬로우 모션)해제 후 난이도 중 설정"), _tcslen(_T("슬로우 모션 : 마우스 왼쪽(슬로우 모션)해제 후 난이도 중 설정")));//해당 좌표에 텍스처를 출력해준다.
		TextOut(hdc, 5, 660, _T("게임 오버 후 재시작 방법 : ENTER"), _tcslen(_T("게임 오버 후 재시작 방법 : ENTER")));//해당 좌표에 텍스처를 출력해준다.

		switch (point_1) //point_1에 저장된 값에 따라 case문을 실행해 준다
		{
		case 0://point_1에 저장된 값이 0 일떄
			TextOut(hdc, 50, 520, _T("먹이를 먹어 주세요"), _tcslen(_T("먹이를 먹어 주세요"))); //해당 좌표에 텍스처를 출력해준다
			break;
		case 1://point_1에 저장된 값이 1 일떄
			TextOut(hdc, 50, 520, _T("먹은 먹이의 수 : 1/10"), _tcslen(_T("먹은 먹이의 수 : 1/10")));//해당 좌표에 텍스처를 출력해준다
			break;
		case 2://point_1에 저장된 값이 2 일떄
			TextOut(hdc, 50, 520, _T("먹은 먹이의 수 : 2/10"), _tcslen(_T("먹은 먹이의 수 : 2/10")));//해당 좌표에 텍스처를 출력해준다
			break;
		case 3://point_1에 저장된 값이 3 일떄
			TextOut(hdc, 50, 520, _T("먹은 먹이의 수 : 3/10"), _tcslen(_T("먹은 먹이의 수 : 3/10")));//해당 좌표에 텍스처를 출력해준다
			break;
		case 4://point_1에 저장된 값이 4 일떄
			TextOut(hdc, 50, 520, _T("먹은 먹이의 수 : 4/10"), _tcslen(_T("먹은 먹이의 수 : 4/10")));//해당 좌표에 텍스처를 출력해준다
			break;
		case 5://point_1에 저장된 값이 5 일떄
			TextOut(hdc, 50, 520, _T("먹은 먹이의 수 : 5/10"), _tcslen(_T("먹은 먹이의 수 : 5/10")));//해당 좌표에 텍스처를 출력해준다
			break;
		case 6://point_1에 저장된 값이 6 일떄
			TextOut(hdc, 50, 520, _T("먹은 먹이의 수 : 6/10"), _tcslen(_T("먹은 먹이의 수 : 6/10")));//해당 좌표에 텍스처를 출력해준다
			break;
		case 7://point_1에 저장된 값이 7 일떄
			TextOut(hdc, 50, 520, _T("먹은 먹이의 수 : 7/10"), _tcslen(_T("먹은 먹이의 수 : 7/10")));//해당 좌표에 텍스처를 출력해준다
			break;
		case 8://point_1에 저장된 값이 8 일떄
			TextOut(hdc, 50, 520, _T("먹은 먹이의 수 : 8/10"), _tcslen(_T("먹은 먹이의 수 : 8/10")));//해당 좌표에 텍스처를 출력해준다
			break;
		case 9://point_1에 저장된 값이 9 일떄
			TextOut(hdc, 50, 520, _T("먹은 먹이의 수 :  9/10 * 마지막 먹이가 남았습니다."), _tcslen(_T("먹은 먹이의 수 : 9/10 * 마지막 먹이가 남았습니다.")));//해당 좌표에 텍스처를 출력해준다
			break;
		case 10://point_1에 저장된 값이 10 일떄
			TextOut(hdc, 50, 520, _T("게임 클리어"), _tcslen(_T("게임 클리어")));//해당 좌표에 텍스처를 출력해준다
			break;
		default:
			break;
		}

		//게임 오버 출력창
		if (gameover == 1) { //gameover변수의 값이 1이면,
			TextOut(hdc, 50, 520, _T("게임 오버             [다시 시작 : Enter]"), _tcslen(_T("게임 오버             [다시 시작 : Enter]")));//해당 좌표에 텍스처를 출력해준다
			KillTimer(hwnd, 1);//타이머를 죽인다.
		}
		else if (gameover == 0) { //gameover변수의 값이 1이고
			if (gameset == 1) { //gameset변수의 값이 1이면
				select = -1; //현재 방향키에서 입력 받은 것이 없기 때문에 초기값을 -1로 값을 넣어준다.
				GameInit(); //GameInit()함수를 호출하여준다.
				SetTimer(hwnd, 1, 100, NULL);//SetTime을 설정해 준다.
				gameset = 0; //gameset변수에 0을 넣어준다. 게임이 리셋되었기 때문이다.
			}
		}

		//방향키 코드
		TextOut(hdc, 610, 50, _T("방향키 표시"), _tcslen(_T("방향키 표시"))); //해당 좌표에 텍스처를 출력해준다.
		for (int i = 0; i < 5; i++) {//rect배열의 인덱스를 돌기 위하여 for문을 사용하였다.
			Rectangle(hdc, rect[i].left, rect[i].top, rect[i].right, rect[i].bottom); //rect 배열에 저장된 사각형들의 좌표을 반복문을 통하여 전부 출력해준다. 
			DrawText(hdc, direct[i], _tcslen(direct[i]), &rect[i], DT_SINGLELINE | DT_CENTER | DT_VCENTER); //해당 좌표에 텍스처를 출력해준다.
		}
		if (select == 0) { //select변수에 값이 0일떄
			hBrush = CreateSolidBrush(RGB(225, 0, 0)); //펜을 생성
			oldBrush = (HBRUSH)SelectObject(hdc, hBrush); //펜을 집
			Rectangle(hdc, rect[2].left, rect[2].top, rect[2].right, rect[2].bottom); //해당좌표에 Rectangle출력
			SelectObject(hdc, oldBrush);//이전의 펜으로 돌아감감
			DeleteObject(hBrush);
		}
		else if (select == 1) {
			hBrush = CreateSolidBrush(RGB(225, 0, 0)); //펜을 생성
			oldBrush = (HBRUSH)SelectObject(hdc, hBrush); //펜을 집
			Rectangle(hdc, rect[0].left, rect[0].top, rect[0].right, rect[0].bottom);//해당좌표에 Rectangle출력
			SelectObject(hdc, oldBrush);//이전의 펜으로 돌아감감
			DeleteObject(hBrush);
		}
		else if (select == 2) {
			hBrush = CreateSolidBrush(RGB(225, 0, 0)); //펜을 생성
			oldBrush = (HBRUSH)SelectObject(hdc, hBrush); //펜을 집
			Rectangle(hdc, rect[1].left, rect[1].top, rect[1].right, rect[1].bottom);//해당좌표에 Rectangle출력
			SelectObject(hdc, oldBrush);//이전의 펜으로 돌아감감
			DeleteObject(hBrush);
		}
		else if (select == 3) {
			hBrush = CreateSolidBrush(RGB(225, 0, 0)); //펜을 생성
			oldBrush = (HBRUSH)SelectObject(hdc, hBrush); //펜을 집
			Rectangle(hdc, rect[3].left, rect[3].top, rect[3].right, rect[3].bottom);//해당좌표에 Rectangle출력
			SelectObject(hdc, oldBrush);//이전의 펜으로 돌아감감
			DeleteObject(hBrush);
		}
		else if (select == -1) { //값이 -1인 상태 일때
			for (int i = 0; i < 5; i++) { ////rect배열의 인덱스를 돌기 위하여 for문을 사용하였다.
				Rectangle(hdc, rect[i].left, rect[i].top, rect[i].right, rect[i].bottom); //rect 배열에 저장된 사각형들의 좌표을 반복문을 통하여 전부 출력해준다. 
				DrawText(hdc, direct[i], _tcslen(direct[i]), &rect[i], DT_SINGLELINE | DT_CENTER | DT_VCENTER);//해당 좌표에 텍스처를 출력해준다.
			}
		}
		EndPaint(hwnd, &ps);
		break;

	case WM_LBUTTONDOWN: //마우스 왼쪽을 눌렀을 때
		lstrcpy(str, _T("모드 : 슬로우 모션"));//해당 좌표에 텍스처를 출력해준다.
		InvalidateRect(hwnd, NULL, TRUE);
		SetTimer(hwnd, 1, 1000, NULL); //타이머의 속도르 늘려준다.
		return 0;
	case WM_LBUTTONUP: //마우스 왼쪽을 땠을 떄
		lstrcpy(str, _T("난이도 : 중"));//해당 좌표에 텍스처를 출력해준다.
		InvalidateRect(hwnd, NULL, TRUE);
		SetTimer(hwnd, 1, 100, NULL);//타이머의 속돌를 (난이도 : 중) 상태로 돌려준다.
		return 0;
	case WM_KEYDOWN: //해당키를 눌렀을 때
		DirectControl((int)wParam);
		switch (wParam) {
		case VK_LEFT://왼쪽키를 눌렀을 떄
			if (gameover == 0) {
				select = 0;
			}
			break;

		case  VK_UP://위쪽키를 눌렀을 떄
			if (gameover == 0) {
				select = 1;
			}
			break;

		case VK_RIGHT://오른쪽 키를 눌렀을 때
			if (gameover == 0) {
				select = 2;
			}
			break;

		case VK_DOWN://아래쪽 키를 눌렀을 때
			if (gameover == 0) {
				select = 3;
			}
			break;

		case VK_RETURN://엔터값의 키를 눌렀을 때
			if (gameover == 1) { //만약 gameover값이 1이면
				gameset = 1; //gameset값에 1을 넣어준다.
				gameover = 0; //gameover값에 을 넣어준다.
			}
			break;
		case VK_NUMPAD1: //숫자키 1을 눌렀을 때
			lstrcpy(str, _T("난이도 : 하")); //해당 좌표에 텍스처를 출력
			InvalidateRect(hwnd, NULL, TRUE);
			SetTimer(hwnd, 1, 500, NULL); //난이도 하에 따른 SetTimer로 속도를 조절
			break;

		case VK_NUMPAD2: //숫자키 2을 눌렀을 때
			lstrcpy(str, _T("난이도 : 중")); //해당 좌표에 텍스처를 출력
			InvalidateRect(hwnd, NULL, TRUE);
			SetTimer(hwnd, 1, 100, NULL);//난이도 중에 따른 SetTimer로 속도를 조절
			break;

		case VK_NUMPAD3: //숫자키 3을 눌렀을 때
			lstrcpy(str, _T("난이도 : 상")); //해당 좌표에 텍스처를 출력
			InvalidateRect(hwnd, NULL, TRUE);
			SetTimer(hwnd, 1, 50, NULL);//난이도 상에 따른 SetTimer로 속도를 조절
			break;
		dafault: //위의 어떠한 키값이 아닐때
			select = -1; //select변수에 -1의 값을 넣어준다.
			break;
		}
		InvalidateRgn(hwnd, NULL, TRUE);
		break;

	case WM_KEYUP: //해당 키에서 손을 땟을때
		switch (wParam) {
		case VK_LEFT://왼쪽키를 땟을 떄
			select = -1;
			break;
		case  VK_UP://위쪽키를 땟을 떄
			select = -1;
			break;
		case VK_RIGHT://오른쪽키를 땟을 떄
			select = -1;
			break;
		case VK_DOWN://아래쪽키를 땟을 떄
			select = -1;
			break;
		}
		InvalidateRgn(hwnd, NULL, TRUE);
		break;

	case WM_TIMER: //타이머가 자동할때
		MovingWorm();
		switch (wParam) {
		case 1:
			GetLocalTime(&st);
			_stprintf_s(sTime, _T("현재 시간 : %d:%d:%d입니다"), st.wHour, st.wMinute, st.wSecond); //현재 시간에st.wHour, st.wMinute, st.wSecond 저장된 값과 문자를 함께 출력해준다.			InvalidateRect(hwnd, &rt, TRUE);
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