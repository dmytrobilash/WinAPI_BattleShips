#define _CRT_SECURE_NO_WARNINGS
#include "Game.h"
#include "Battleship.h"
#include <dwmapi.h>
#pragma comment(lib, "Dwmapi.lib")

ui::Container* BattleshipGame::gmContainer;
int BattleshipGame::window_w, BattleshipGame::window_h;
UiField* BattleshipGame::ui_user_fld, *BattleshipGame::ui_bot_fld;
BattleshipGame::_rcs BattleshipGame::resources;
int BattleshipGame::ships_count;
int BattleshipGame::user_hits, BattleshipGame::bot_hits;
bool BattleshipGame::game_started;
bool BattleshipGame::bot_playing;

void BattleshipGame::init(HINSTANCE rcs_inst, int window_w, int window_h) {
	BattleshipGame::window_w = window_w;
	BattleshipGame::window_h = window_h;
	resources.ship1_bmp = (HBITMAP)LoadImage(rcs_inst, MAKEINTRESOURCE(IDB_SHIP1Q), IMAGE_BITMAP, 0, 0, 0);
	resources.ship2_bmp = (HBITMAP)LoadImage(rcs_inst, MAKEINTRESOURCE(IDB_SHIP2Q), IMAGE_BITMAP, 0, 0, 0);
	resources.ship3_bmp = (HBITMAP)LoadImage(rcs_inst, MAKEINTRESOURCE(IDB_SHIP3Q), IMAGE_BITMAP, 0, 0, 0);
	resources.ship4_bmp = (HBITMAP)LoadImage(rcs_inst, MAKEINTRESOURCE(IDB_SHIP4Q), IMAGE_BITMAP, 0, 0, 0);
	resources.hit_bmp = (HBITMAP)LoadImage(rcs_inst, MAKEINTRESOURCE(IDB_HIT), IMAGE_BITMAP, 0, 0, 0);
	resources.miss_bmp = (HBITMAP)LoadImage(rcs_inst, MAKEINTRESOURCE(IDB_MISS), IMAGE_BITMAP, 0, 0, 0);

	gmContainer = new ui::Container(0, 0, window_w, window_h);
	reload();
}

bool BattleshipGame::start(HWND window, WNDPROC wndproc) {
	MSG msg;
	bool exit = false;
	bool game_over = false;

	while (!exit && !game_over)
	{
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {

			switch (msg.message) {
			case WM_QUIT: {
				exit = true;
				break;
			}
			case WM_MOUSEMOVE: {
				int x = LOWORD(msg.lParam);
				int y = HIWORD(msg.lParam);
				gmContainer->mouseMove(x, y);
				break;
			}
			case WM_LBUTTONDOWN: {
				int x = LOWORD(msg.lParam);
				int y = HIWORD(msg.lParam);
				gmContainer->mouseClick(x, y);
				break;
			}
			case WM_KEYDOWN: {
				gmContainer->keyDown(msg.wParam);
				break;
			}
			case WM_PAINT:
			{
				PAINTSTRUCT ps;
				HDC hdc = BeginPaint(window, &ps);
				HDC bdc = CreateCompatibleDC(hdc);
				HBITMAP bbmp = CreateCompatibleBitmap(hdc, BattleshipGame::window_w, BattleshipGame::window_h);
				HGDIOBJ _tmp = SelectObject(bdc, bbmp);
				HBRUSH white_b = CreateSolidBrush(RGB(255, 255, 255));
				RECT r = { 0, 0, window_w, window_h };
				FillRect(bdc, &r, white_b);
				gmContainer->draw(bdc);
				BitBlt(hdc, 0, 0, BattleshipGame::window_w, BattleshipGame::window_h, bdc, 0, 0, SRCCOPY);
				DeleteObject(white_b);
				SelectObject(bdc, _tmp);
				DeleteObject(bbmp);
				DeleteDC(bdc);
				EndPaint(window, &ps);
				break;
			}
			case WM_ERASEBKGND:
				break;
			default:
				wndproc(window, msg.message, msg.wParam, msg.lParam);
				break;
			}
		}

		if (bot_playing) {
			int x, y;
			bool play = Field::botRandomChoice(x, y);
			ui_user_fld->mouseClick(x * 40 + 40, y * 40 + 40);
			bot_playing = play;
		}

		if (user_hits == 20 || bot_hits == 20) {
			game_over = true;
		}

		InvalidateRect(window, NULL, FALSE);
		DwmFlush();
	}
	return exit;
}

void BattleshipGame::reload() {

	if (ui_user_fld != nullptr) {
		gmContainer->delElement(ui_user_fld);
	}

	if (ui_bot_fld != nullptr) {
		gmContainer->delElement(ui_bot_fld);
	}

	ships_count = 0;
	user_hits = 0;
	bot_hits = 0;
	game_started = false;
	bot_playing = false;
	Field::init();
	Field::botGenerateShips();

	ui_user_fld = new UiField(0, 0, 480, 480);
	ui_user_fld->onClickEvent(on_user_field_click);
	gmContainer->addElement(ui_user_fld);

	ui_bot_fld = new UiField(480, 0, 480, 480);
	ui_bot_fld->onClickEvent(on_bot_field_click);

#ifdef DEBUG_GAME
	openOpponentField();
#endif // DEBUG_GAME

	gmContainer->addElement(ui_bot_fld);

	UiBattleship* s1q = new UiBattleship(20, 500, 40, 40);
	s1q->setImage(resources.ship1_bmp);
	s1q->setImageSize(80, 80);
	s1q->onDropEvent(on_ship_drop);
	s1q->setOrigin(20, 500);
	s1q->setShipLen(1);
	UiBattleship* s2q = new UiBattleship(80, 500, 80, 40);
	s2q->setImage(resources.ship2_bmp);
	s2q->setImageSize(160, 80);
	s2q->onDropEvent(on_ship_drop);
	s2q->setOrigin(80, 500);
	s2q->setShipLen(2);
	UiBattleship* s3q = new UiBattleship(180, 500, 120, 40);
	s3q->setImage(resources.ship3_bmp);
	s3q->setImageSize(240, 80);
	s3q->onDropEvent(on_ship_drop);
	s3q->setOrigin(180, 500);
	s3q->setShipLen(3);
	UiBattleship* s4q = new UiBattleship(320, 500, 160, 40);
	s4q->setImage(resources.ship4_bmp);
	s4q->setImageSize(320, 80);
	s4q->onDropEvent(on_ship_drop);
	s4q->setOrigin(320, 500);
	s4q->setShipLen(4);

	gmContainer->addElement(s1q);
	gmContainer->addElement(s2q);
	gmContainer->addElement(s3q);
	gmContainer->addElement(s4q);
}

void BattleshipGame::cleanup() {
	delete gmContainer;
}

bool BattleshipGame::userWon() {
	return user_hits == 20;
}

void BattleshipGame::openOpponentField() {
	for (int i = 0; i < 10; ++i) {
		for (int j = 0; j < 10; ++j) {
			if (Field::botFieldState(j, i) == FieldState::SHIP) {
				ui::Bitmap* ship = new ui::Bitmap(resources.ship1_bmp, 0, 0, 40, 40);
				ship->setImageSize(80, 80);
				ui_bot_fld->addElement(j, i, ship);
			}
		}
	}
}

void BattleshipGame::on_bot_field_click(UiField* ui_field, int x, int y) {
	if (bot_playing || !game_started) {
		return;
	}

	int cx = x / 40;
	int cy = y / 40;
	FieldState state = Field::botFieldState(cx, cy);
	if (state == FieldState::HIT || state == FieldState::MISS) {
		return;
	}

	bool hit = false;
	ui::Bitmap* new_element = nullptr;
	if (state == FieldState::SHIP) {
		user_hits++;
		hit = true;
		new_element = new ui::Bitmap(resources.hit_bmp, 0, 0, 40, 40);
	} else if (state == FieldState::NONE) {
		new_element = new ui::Bitmap(resources.miss_bmp, 0, 0, 40, 40);
	}

	ui_field->delElement(cx, cy);
	new_element->setImageSize(80, 80);
	ui_field->addElement(cx, cy, new_element);
	if (!Field::userChoice(cx, cy)) {
		bot_playing = true;
	}
	if (hit) {
		auto points = Field::releaseShip(cx, cy, false);
		for (auto p : points) {
			ui::Bitmap* bmp = new ui::Bitmap(resources.miss_bmp, 0, 0, 40, 40);
			bmp->setImageSize(80, 80);
			ui_field->addElement(p.first, p.second, bmp);
		}
	}
}

void BattleshipGame::on_user_field_click(UiField* ui_field, int x, int y) {
	if (!game_started || !bot_playing) {
		return;
	}

	int cx = x / 40;
	int cy = y / 40;
	FieldState state = Field::userFieldState(cx, cy);

	bool hit = false;
	ui::Bitmap* new_element = nullptr;
	if (state == FieldState::HIT) {
		bot_hits++;
		hit = true;
		new_element = new ui::Bitmap(resources.hit_bmp, 0, 0, 40, 40);
	} else if (state == FieldState::MISS) {
		new_element = new ui::Bitmap(resources.miss_bmp, 0, 0, 40, 40);
	}

	ui_field->delElement(cx, cy);
	new_element->setImageSize(80, 80);
	ui_field->addElement(cx, cy, new_element);
	if (hit) {
		auto points = Field::releaseShip(cx, cy, true);
		for (auto p : points) {
			ui::Bitmap* bmp = new ui::Bitmap(resources.miss_bmp, 0, 0, 40, 40);
			bmp->setImageSize(80, 80);
			ui_field->addElement(p.first, p.second, bmp);
		}
	}
}

void BattleshipGame::on_ship_drop(UiBattleship* ui_ship, int x, int y) {
	UiField* fld = ui_user_fld;

	if (fld->inside(x, y)) {
		int cx = 0;
		int cy = 0;
		bool horiz = ui_ship->isHorizontal();
		if (ui_ship->getShipLen() == 1) {
			cx = (x - 40) / 40;
			cy = (y - 40) / 40;
		} else {
			ui_ship->getCoords(cx, cy);
			cx = (cx - 20) / 40;
			cy = (cy - 20) / 40;
		}
		bool area_clear = true;
		int block_x = cx;
		int block_y = cy;
		for (int i = 0; i < ui_ship->getShipLen(); ++i) {
			if (block_x < 0 || block_x >= 10 || block_y < 0 || block_y >= 10) {
				area_clear = false;
				break;
			}
			for (int py = -1; py <= 1; py++) {
				for (int px = -1; px <= 1; px++) {
					if (block_y + py < 0 || block_y + py >= 10 || block_x + px < 0 || block_x + px >= 10)
						continue;
					if (Field::userFieldState(block_x + px, block_y + py) != FieldState::NONE) {
						area_clear = false;
						break;
					}
				}
			}
			if (horiz) { ++block_x; } else { ++block_y; }
		}
		if (area_clear) {
			Field::userSetShip(cx, cy, ui_ship->getShipLen(), horiz);
			for (int i = 0; i < ui_ship->getShipLen(); ++i) {
				ui::Bitmap* ship = new ui::Bitmap(resources.ship1_bmp, 0, 0, 40, 40);
				ship->setImageSize(80, 80);
				fld->addElement(cx, cy, ship);
				if (horiz) { ++cx; } else { ++cy; }
			}

			ui_ship->decShipCount();
			if (ui_ship->getShipCount() == 0) {
				gmContainer->delElement(ui_ship);
			} else {
				ui_ship->setDrag(true);
			}

			ships_count++;
			if (ships_count == 10) {
				game_started = true;
				bot_playing = false;
			}

		} else {
			ui_ship->moveToOrigin();
		}
	} else {
		ui_ship->moveToOrigin();
	}
}