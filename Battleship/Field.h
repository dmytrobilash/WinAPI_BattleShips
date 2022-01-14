#pragma once

#include <ctime>
#include <cstdlib>
#include <vector>
#include <stack>
#include <utility>

enum class FieldState {
	NONE = 10,
	SHIP,
	MISS,
	HIT
};

class Field {
private:
	static int user_field[10][10];
	static int bot_field[10][10];
public:
	static void init();
	static void botGenerateShips();
	static bool botRandomChoice(int &x, int &y);
	static bool userChoice(int x, int y);
	static void userSetShip(int x, int y, int len, bool orient);
	static FieldState userFieldState(int x, int y);
	static FieldState botFieldState(int x, int y);
	static std::vector<std::pair<int, int>> releaseShip(int x, int y, bool user_field);
};