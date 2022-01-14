#include "Field.h"

int Field::user_field[10][10] = { int(FieldState::NONE) };
int Field::bot_field[10][10] = { int(FieldState::NONE) };

namespace {
struct ship {
	int len;
	bool orientation;
	int count;
};
bool check_ship(int mat[10][10], ship s, int x, int y);
std::vector<std::pair<int, int>> release_ship(int mat[10][10], int x, int y);
}

void Field::init() {
	for (int i = 0; i < 10; ++i) {
		for (int j = 0; j < 10; ++j) {
			Field::bot_field[i][j] = int(FieldState::NONE);
			Field::user_field[i][j] = int(FieldState::NONE);
		}
	}
}

void Field::botGenerateShips() {
	std::srand(time(nullptr));
	ship ships[4] = {
		{1, 1, 4},
		{2, 1, 3},
		{3, 1, 2},
		{4, 1, 1},
	};

	for (int i = 0; i < 10; ++i) {
		int s = 0;
		do {
			s = rand() % 4;
		} while (ships[s].count <= 0);

		int y = rand() % 6;
		int x = 0;
		for (; y < 10; ++y) {
			x = 0;
			for (; x < 10; ++x) {
				if (!check_ship(Field::bot_field, ships[s], x, y)) {
					ships[s].orientation = !ships[s].orientation;
					if (check_ship(Field::bot_field, ships[s], x, y)) {
						goto __for_end;
					}
				} else {
					goto __for_end;
				}
			}
		}
	__for_end:
		for (int j = 0; j < ships[s].len; ++j) {
			Field::bot_field[y][x] = int(FieldState::SHIP);
			for (int py = -1; py <= 1; py++) {
				for (int px = -1; px <= 1; px++) {
					if (y + py < 0 || y + py >= 10 || x + px < 0 || x + px >= 10)
						continue;
					if (Field::bot_field[y + py][x + px] != int(FieldState::NONE))
						continue;
					Field::bot_field[y + py][x + px] = 8;
				}
			}
			if (ships[s].orientation) {++x;} else {++y;}
		}
		ships[s].count--;
	}
	for (int i = 0; i < 10; ++i) {
		for (int j = 0; j < 10; ++j) {
			if (Field::bot_field[i][j] == 8) {
				Field::bot_field[i][j] = int(FieldState::NONE);
			}
		}
	}
}

bool Field::botRandomChoice(int &x, int &y) {
	int bx = 0;
	int by = 0;
	do {
		bx = rand() % 10;
		by = rand() % 10;
		if (user_field[by][bx] == int(FieldState::SHIP) || user_field[by][bx] == int(FieldState::NONE)) {
			break;
		}
	} while (true);
	x = bx; y = by;
	if (user_field[by][bx] == int(FieldState::SHIP)) {
		user_field[by][bx] = int(FieldState::HIT);
		return true;
	} else if (user_field[by][bx] == int(FieldState::NONE)) {
		user_field[by][bx] = int(FieldState::MISS);
	}
	return false;
}

bool Field::userChoice(int x, int y) {
	if (bot_field[y][x] == int(FieldState::SHIP)) {
		bot_field[y][x] = int(FieldState::HIT);
		return true;
	} else if (bot_field[y][x] == int(FieldState::NONE)){
		bot_field[y][x] = int(FieldState::MISS);
	}
	return false;
}

void Field::userSetShip(int x, int y, int len, bool orient) {
	for (int i = 0; i < len; ++i) {
		Field::user_field[y][x] = int(FieldState::SHIP);
		if (orient) { ++x; } else { ++y; }
	}
}

FieldState Field::userFieldState(int x, int y) {
	if (x < 0 || x >= 10 || y < 0 || y >= 10) {
		return FieldState::NONE;
	}
	if (Field::user_field[y][x] == 1) {
		return FieldState::SHIP;
	} else {
		return FieldState(Field::user_field[y][x]);
	}
}

FieldState Field::botFieldState(int x, int y) {
	if (x < 0 || x >= 10 || y < 0 || y >= 10) {
		return FieldState::NONE;
	}
	if (Field::bot_field[y][x] == 1) {
		return FieldState::SHIP;
	} else {
		return FieldState(Field::bot_field[y][x]);
	}
}

std::vector<std::pair<int, int>> Field::releaseShip(int x, int y, bool user_field) {
	std::vector<std::pair<int, int>> points;
	if (user_field) {
		points = release_ship(Field::user_field, x, y);
	} else {
		points = release_ship(Field::bot_field, x, y);
	}
	for (auto p : points) {
		if (user_field) {
			Field::user_field[p.second][p.first] = int(FieldState::MISS);
		} else {
			Field::bot_field[p.second][p.first] = int(FieldState::MISS);
		}
	}
	return points;
}

namespace {

bool check_ship(int mat[10][10], ship s, int x, int y) {
	bool ret = true;
	for (int i = 0; i < s.len; ++i) {
		if (mat[y][x] != int(FieldState::NONE) || mat[y][x] == 8) {
			ret = false;
			break;
		}
		if (s.orientation) {++x;} else {++y;}
		}
	return ret;
}

std::vector<std::pair<int, int>> release_ship(int mat[10][10], int x, int y) {
	bool dead = true;
	std::vector<std::pair<int, int>> points;
	std::vector<std::pair<int, int>> viewed;
	std::stack<std::pair<int, int>> next_p;
	next_p.push({ x, y });
	do {
		auto p = next_p.top();
		next_p.pop();
		int x = p.first;
		int y = p.second;
		viewed.push_back({ x, y });

		for (int py = -1; py <= 1; py++) {
			for (int px = -1; px <= 1; px++) {
				if (y + py < 0 || y + py >= 10 || x + px < 0 || x + px >= 10)
					continue;
				if (mat[y + py][x + px] == int(FieldState::HIT)) {
					std::pair<int, int> p = { x + px, y + py };
					if (std::find(viewed.begin(), viewed.end(), p) == viewed.end()) {
						next_p.push(p);
					}
				} else if (mat[y + py][x + px] == int(FieldState::NONE)) {
					points.push_back({ x + px, y + py });
				} else if (mat[y + py][x + px] == int(FieldState::SHIP)) {
					dead = false;
				}
			}
		}
	} while (!next_p.empty());

	if (dead) {
		return points;
	} else {
		return {};
	}
}

}