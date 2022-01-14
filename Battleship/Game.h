#pragma once

#include "GameUI.h"
#include "Field.h"

class BattleshipGame {
private:
	static ui::Container* gmContainer;
	static UiField *ui_user_fld, *ui_bot_fld;
	static int window_w, window_h;
	static int ships_count, user_hits, bot_hits;
	static bool game_started, bot_playing;
	static struct _rcs {
		HBITMAP ship1_bmp, ship2_bmp, ship3_bmp, ship4_bmp;
		HBITMAP hit_bmp, miss_bmp;
	} resources;

	static void on_bot_field_click(UiField* ui_field, int x, int y);
	static void on_user_field_click(UiField* ui_field, int x, int y);
	static void on_ship_drop(UiBattleship* ui_ship, int x, int y);
	BattleshipGame() = delete;
public:

	static void init(HINSTANCE rcs_inst, int window_w, int window_h);
	static bool start(HWND window, WNDPROC wndproc);
	static void reload();
	static void cleanup();
	static bool userWon();
	static void openOpponentField();
};