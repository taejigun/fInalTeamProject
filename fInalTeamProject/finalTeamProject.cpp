#define _CRT_SECURE_NO_WARNINGS
#include <bangtal>
using namespace bangtal;
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define ANIMATION_TIME		0.1f	//움직이는 시간 정의
#define ANIMATION_STEP		150	//점프할때 이동하는 발걸음 y좌표
#define DELAY_TIME 0.9f	//점프키:릴리즈 처리 시간 정의

SceneID start_scene, main_scene, end_scene;
ObjectID cat, silver[4], gold[4]; //움직이는 오브젝트(고양이), 은색 코인, 금색 코인 선언

ObjectID start_button, end_button, end_button2; //스타트 버튼, 엔드 버튼

ObjectID bar;	//거리 바 오브젝트 정의
ObjectID barCh;	//바를 움직이는 캐릭터 정의
SoundID sound;	//bgm

int coin[4]; //코인 선언, 장애물 위에 배치할 것이므로 장애물과 마찬가지로 배열로 선언
int coin_Y[4]; // 코인의 Y좌표 선언	
int coin_count; //점수
char buf[30]; //점수를 표현해주기 위한 함수

bool ended = false; // 게임이 끝나지 않았음 = 기본값

double barCh_x = 320;	//바를 움직이는 캐릭터의 초기 좌표 정의

TimerID timerRelease = createTimer(DELAY_TIME);	//점프키 릴리즈 처리 타이머(점프 시간)
TimerID sceneTimer = createTimer(1000); //게임 총 시간 세는 타이머
TimerID barTimer = createTimer(0.01f);	//바를 움직이는 캐릭터 타이머 정의

int x = 200, y = 50;	//초기 물체 좌표
int dx = 0, dy = 0;		//물체 이동 좌표 정의

KeyState keyState = KeyState::KEY_RELEASED;	//키보드 상태 정의

void keyboardCallback(KeyCode code, KeyState state);	//키보드 콜백 함수
void timerCallback(TimerID timer);	//타이머 콜백 함수
void mouseCallback(ObjectID object, int x, int y, MouseAction action);

//장애물
ObjectID ob[4];
int ob_x[4]; //장애물의 x좌표
int ob_dx = 0, speed = 6;
int ob_check[4];//장애물의 이미지를 확인하기 위한 변수

TimerID ob_speed_timer; //게임이 진행될수록 장애물이 이동하는 속도가 빨라지게 조절하는 타이머
TimerID ob_timer; //장애물 배치, 장애물이 이동하도록 하는 타이머
TimerID check_timer; //장애물과 쿠키의 충돌 확인하는 타이머

//오브젝트 선언하는 함수
ObjectID createObject(const char* name, const char* image, SceneID scene, int x, int y, bool shown) {
	ObjectID object = createObject(image);
	locateObject(object, scene, x, y);
	showObject(object);

	if (shown) {
		showObject(object);
	}
	return object;
}

//캐릭터가 코인에 부딪혔는지 체크하는 함수.
//부딪히면 코인이 사라지면서 점수가 5 증가
void CoinCheck() {
	for (int j = 0; j < 4; j++) {
		if (x >= ob_x[j] && x < ob_x[j] + 76 && coin_Y[j] > 0) {
			if (ob_check[j] == 1 && y == 200) {
				coin_Y[j] = -200;
				locateObject(coin[j], main_scene, ob_x[j] + 32, coin_Y[j]);
				hideObject(coin[j]);
				coin_count += 5;
			}
			if (ob_check[j] == 2 && y == 350) {
				coin_Y[j] = -200;
				locateObject(coin[j], main_scene, ob_x[j] + 32, coin_Y[j]);
				hideObject(coin[j]);
				coin_count += 5;
			}
		}
	}
}
void relocate(int i) {
	locateObject(ob[i], main_scene, ob_x[i], 50);
	locateObject(coin[i], main_scene, ob_x[i] + 32, coin_Y[i]);
	showObject(ob[i]);
	showObject(coin[i]);
}

void check() {
	ended = true;
	enterScene(start_scene); //장애물과 닿으면 다시 초기 화면으로
	stopTimer(ob_speed_timer);
	stopTimer(ob_timer);
	stopTimer(barTimer);
	speed = 5;
	sprintf(buf, "Score : %d", coin_count);
	showMessage(buf);
	stopTimer(check_timer);
}

void keyboardCallback(KeyCode code, KeyState state)	//점프키 이중점프를 위해 점프키:릴리즈 상태->타이머에서 처리
{
	if (code == KeyCode::KEY_SPACE && state == KeyState::KEY_PRESSED) {
		if (y > 50 + ANIMATION_STEP) return;	//2단 점프만 가능하게 만들기
		y += ANIMATION_STEP;
		locateObject(cat, main_scene, x, y);
		stopTimer(timerRelease);	//이전에 실행된 타이머 취소
		setTimer(timerRelease, DELAY_TIME);	//점프키 릴리즈 타이머 다시 실행
		startTimer(timerRelease);	//타이머 시작
	}
}

void mouseCallback(ObjectID object, int x, int y, MouseAction action)
{
	if (object == start_button) { //시작 버튼을 누르면 타이머 시작
		coin_count = 0; // 점수 초기화
		ended = false; // 게임이 끝나지 않음 초기화
		playSound(sound);
		enterScene(main_scene);
		for (int i = 0; i < 4; i++) {
			setObjectImage(ob[i], "images\\ob_low.png");
			coin_Y[i] = 200;
			ob_check[i] = 1;
			ob_x[i] = 500 + 500 * i;
			showObject(coin[i]);
			locateObject(ob[i], main_scene, ob_x[i], 50);
			locateObject(coin[i], main_scene, ob_x[i] + 32, 200);
		}
		barCh_x = 320;
		locateObject(barCh, main_scene, barCh_x, 680);
		startTimer(ob_timer);
		startTimer(check_timer);
		startTimer(ob_speed_timer);
		startTimer(barTimer);
	}
	else if (object == end_button) { //엔드 버튼을 누르면 타이머 종료
		endGame();
	}
}

void timerCallback(TimerID timer)
{
	if (timer == timerRelease)
	{
		y = 50;
		locateObject(cat, main_scene, x, y);	//움직이는 물체 다시 돌아오게 만들기
	}

	if (timer == barTimer) {	//바를 움직이는 함수
		hideObject(barCh);
		barCh_x = barCh_x + 0.076;
		locateObject(barCh, main_scene, barCh_x, 680);
		showObject(barCh);

		if (barCh_x > 930) {
			showMessage("Puang Run Complete!!");
			barCh_x = 320;
			stopSound(sound);
			enterScene(end_scene);
		}
		setTimer(barTimer, 0.01f);	//바를 움직이는 시간 간격
		startTimer(barTimer);	//바 타이머 시작
	}

	if (timer == ob_timer) {
		if (y == 50 + ANIMATION_STEP or y == 50 + ANIMATION_STEP * 2) {
			CoinCheck();
		}
		for (int i = 0; i < 4; i++) {
			if (ob_x[i] >= -100) {
				ob_x[i] -= speed;
				locateObject(ob[i], main_scene, ob_x[i], 50);
				if (coin_Y[i] > 0) {
					locateObject(coin[i], main_scene, ob_x[i] + 32, coin_Y[i]);
				}
			}
			else if (ob_x[i] < -100) {
				if (i == 0) {
					ob_x[i] = 1800;
					int n = rand() % 2;
					switch (n) {
					case 0: setObjectImage(ob[i], "images\\ob_low.png"); ob_check[i] = 1; coin_Y[i] = 200; break;
					case 1: setObjectImage(ob[i], "images\\ob_high.png"); ob_check[i] = 2; coin_Y[i] = 350; break;
					}
					relocate(i);
				}
				else if (i == 1) {
					int n = rand() % 2;
					switch (n) {
					case 0: ob_x[i] = ob_x[0] + 400 + ob_dx, setObjectImage(ob[i], "images\\ob_low.png"); coin_Y[i] = 200; ob_check[i] = 1; break;
					case 1: ob_x[i] = ob_x[0] + 500 + ob_dx, setObjectImage(ob[i], "images\\ob_high.png"); coin_Y[i] = 350;  ob_check[i] = 2; break;
					}
					relocate(i);
				}

				else if (i == 2) {
					int n = rand() % 2;
					switch (n) {
					case 0: ob_x[i] = ob_x[1] + 400 + ob_dx, setObjectImage(ob[i], "images\\ob_low.png"); coin_Y[i] = 200;  ob_check[i] = 1; break;
					case 1: ob_x[i] = ob_x[1] + 500 + ob_dx, setObjectImage(ob[i], "images\\ob_high.png"); coin_Y[i] = 350; ob_check[i] = 2; break;
					}
					relocate(i);
				}
				else if (i == 3) {
					int n = rand() % 2;
					switch (n) {
					case 0: ob_x[i] = ob_x[2] + 400 + ob_dx, setObjectImage(ob[i], "images\\ob_low.png"); coin_Y[i] = 200; ob_check[i] = 1; break;
					case 1: ob_x[i] = ob_x[2] + 500 + ob_dx, setObjectImage(ob[i], "images\\ob_high.png"); coin_Y[i] = 350; ob_check[i] = 2; break;
					}
					relocate(i);
				}
			}
		}
		setTimer(ob_timer, 0.01f);
		startTimer(ob_timer);
	}

	if (timer == ob_speed_timer) {
		if (speed <= 50) {
			ob_dx += 0.2;
			speed += 0.2;
		}
		setTimer(ob_speed_timer, 5.0f);
		startTimer(ob_speed_timer);
	}

	if (timer == check_timer) {   //0.01초마다 충돌 확인, check number를 이용해서 장애물의 이미지를 구분, 쿠키 점프할 때 중간 위치변경이 없어서 y = 50, 200을 기준으로 충돌 확인함.
		for (int i = 0; i < 4; i++) {
			if (ob_x[i] < 279 && ob_x[i]> 94) {
				if (ob_check[i] == 1) {  // 낮은 장애물의 경우
					if (y == 50) {
						check();
					}

				}
				else if (ob_check[i] == 2) {
					if (y == 50 || y == 200) {  //높은 장애물의 경우
						check();
					}
				}
			}
		}
		if (ended == false) { // 게임이 끝나지 않았으면 타이머 실행, 끝났으면 타이머 실행 X
			setTimer(check_timer, 0.01f);
			startTimer(check_timer);
		}
		else
			stopSound(sound);
	}
}

int main() {
	setGameOption(GameOption::GAME_OPTION_ROOM_TITLE, false);
	setGameOption(GameOption::GAME_OPTION_INVENTORY_BUTTON, false);
	setGameOption(GameOption::GAME_OPTION_MESSAGE_BOX_BUTTON, false);

	setTimerCallback(timerCallback);
	setKeyboardCallback(keyboardCallback);
	setMouseCallback(mouseCallback);

	start_scene = createScene("시작 화면", "images\\start_scene.png");
	main_scene = createScene("게임 화면", "images\\main_scene.png");
	end_scene = createScene("종료 화면", "images\\end_scene.png");

	sound = createSound("Sounds/bgm.mp3");

	cat = createObject("cat", "images\\cat.png", main_scene, x, y, true);
	scaleObject(cat, 0.2f);


	for (int i = 0; i < 4; i++) {
		ob_x[i] = 500 + 500 * i;
		coin_Y[i] = 200;
		ob[i] = createObject("ob_block", "images\\ob_low.png", main_scene, ob_x[i], 0, true);
		ob_check[i] = 1;
		coin[i] = createObject("coin", "images\\silver.png", main_scene, ob_x[i] + 30, coin_Y[i], true);
		scaleObject(coin[i], .25f);
	}

	start_button = createObject("start_button", "images\\start_button.png", start_scene, 590, 80, true);
	end_button = createObject("end_button", "images\\end_button.png", start_scene, 590, 30, true);
	end_button2 = createObject("end_button", "images\\end_button.png", end_scene, 590, 50, true);
	bar = createObject("bar", "images\\bar.png", main_scene, 320, 680, true);	//바
	barCh = createObject("barCh", "images\\cat.png", main_scene, 320, 680, true);	//바를 움직이는 캐릭터
	scaleObject(barCh, 0.07f);

	ob_speed_timer = createTimer(5.0f);
	ob_timer = createTimer(0.01f);
	check_timer = createTimer(0.01f);

	startGame(start_scene);
}