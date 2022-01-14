#include "GameUI.h"

void UiBattleship::setImage(HBITMAP bmp) {
	ship_bmp = new ui::Bitmap(bmp, x, y, w, h);
	setTarget(ship_bmp);
}

void UiBattleship::setImageSize(int nw, int nh) {
	ship_bmp->setImageSize(nw, nh);
}

void UiBattleship::setOrigin(int x, int y) {
	origin_x = x;
	origin_y = y;
}

void UiBattleship::moveToOrigin() {
	if (!horizontal) {
		rotate(90);
		horizontal = true;
	}
	setCoords(origin_x, origin_y);
	target->setCoords(origin_x, origin_y);
}

void UiBattleship::setShipLen(int len) {
	ship_len = len;
	ship_count = 5 - len;
}

int UiBattleship::getShipLen() {
	return ship_len;
}

int UiBattleship::getShipCount() {
	return ship_count;
}

void UiBattleship::decShipCount() {
	ship_count--;
}

bool UiBattleship::isHorizontal() {
	return horizontal;
}

bool UiBattleship::mouseClick(int x, int y) {
	bool ret = DragnDrop::mouseClick(x, y);
	if (ret) {
		if (drag == false) {
			if (on_drop) {
				on_drop(this, x, y);
			}
		}
	}
	return ret;
}

void UiBattleship::keyDown(int key) {
	const int R_KEY = 0x52;
	if (key == R_KEY && drag && ship_len > 1) {
		rotate(90);
		horizontal = !horizontal;
	}
}

void UiBattleship::onDropEvent(std::function<void(UiBattleship* ship, int x, int y)> on_drop) {
	this->on_drop = on_drop;
}

UiField::UiField(int x, int y, int w, int h) : BaseElement(x, y, w, h) {
	fldGrid = new ui::Grid(40, 40, 400, 400);
	fldGrid->setCellSize(40, 40);
	fldGrid->setMatrix(10, 10);
	fldContainer = new ui::Container(x, y, w, h);
	fldContainer->addElement(fldGrid);
}

UiField::~UiField() {
	delete fldContainer;
}

void UiField::draw(HDC context) const {
	HBRUSH white_b = CreateSolidBrush(RGB(255, 255, 255));
	RECT r = { x, y, x + w, y + h };
	FillRect(context, &r, white_b);
	DeleteObject(white_b);
	for (int i = 40; i <= 440; i += 40) {
		MoveToEx(context, 40 + x, i + y, nullptr);
		LineTo(context, 440 + x, i + y);
	}
	for (int i = 40; i <= 440; i += 40) {
		MoveToEx(context, i + x, 40 + y, nullptr);
		LineTo(context, i + x, 440 + y);
	}
	char c = 'A';
	for (int i = 55; i <= 420; i += 40, ++c) {
		TextOutA(context, i + x, 10 + y, &c, 1);
	}
	c = '1';
	for (int i = 55; i <= 380; i += 40, ++c) {
		TextOutA(context, 10 + x, i + y, &c, 1);
	}
	TextOutA(context, 5 + x, 420 + y, "10", 2);
	fldContainer->draw(context);
}

void UiField::addElement(int cell_x, int cell_y, BaseElement* elem) {
	fldGrid->addElement(cell_x, cell_y, elem);
}

void UiField::delElement(int cell_x, int cell_y) {
	fldGrid->delElement(cell_x, cell_y);
}

bool UiField::mouseClick(int x, int y) {
	x = x - this->x;
	y = y - this->y;
	if (fldGrid->inside(x, y)) {
		if (on_click) {
			on_click(this, x - 40, y - 40);
		}
		return true;
	}
	return false;
}

void UiField::onClickEvent(std::function<void(UiField *fld, int x, int y)> on_click) {
	this->on_click = on_click;
}