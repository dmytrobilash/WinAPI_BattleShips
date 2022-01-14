#pragma once

#include <functional>
#include "ui.h"

class UiBattleship final : public ui::DragnDrop {
private:
	ui::Bitmap *ship_bmp;
	int origin_x, origin_y, ship_len, ship_count;
	bool horizontal;
	std::function<void(UiBattleship* ship, int x, int y)> on_drop;
public:
	UiBattleship(int x, int y, int w, int h) : ui::DragnDrop(x, y, w, h), horizontal(true) {}
	void setImage(HBITMAP bmp);
	void setImageSize(int nw, int nh);
	void setOrigin(int x, int y);
	void moveToOrigin();
	void setShipLen(int len);
	int getShipLen();
	int getShipCount();
	void decShipCount();
	bool isHorizontal();
	bool mouseClick(int x, int y) override;
	void keyDown(int key) override;
	void onDropEvent(std::function<void(UiBattleship* ship, int x, int y)> on_drop);
};

class UiField final : public ui::BaseElement {
private:
	ui::Container* fldContainer;
	ui::Grid* fldGrid;
	std::function<void(UiField* fld, int x, int y)> on_click;
public:
	UiField(int x, int y, int w, int h);
	~UiField();
	void draw(HDC context) const override;
	void addElement(int cell_x, int cell_y, BaseElement* elem);
	void delElement(int cell_x, int cell_y);
	bool mouseClick(int x, int y) override;
	void onClickEvent(std::function<void(UiField* fld, int x, int y)> on_click);
};