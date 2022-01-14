#pragma once

#include <Windows.h>
#include <vector>
#include <algorithm>

namespace ui {

class BaseElement {
protected:
	int x, y, w, h;
	int angle;
public:
	BaseElement(int x, int y, int w, int h) : x(x), y(y), w(w), h(h), angle(0) {}
	virtual ~BaseElement() {}
	virtual void draw(HDC context) const = 0;
	virtual void mouseMove(int x, int y) {}
	virtual bool mouseClick(int x, int y) { return false; }
	virtual void keyDown(int key) {}
	void setCoords(int nx, int ny) { x = nx; y = ny; }
	void getCoords(int& cx, int& cy) { cx = x; cy = y; }
	virtual void setSize(int nw, int nh) { w = nw; h = nh; }
	virtual bool inside(int px, int py);
	virtual void rotate(int angle) { int t = w; w = h; h = t; this->angle = angle; }
};

class Container : public BaseElement {
protected:
	std::vector<BaseElement*> elems;
public:
	Container(int x, int y, int w, int h) : BaseElement(x, y, w, h) {}
	virtual ~Container();
	virtual void addElement(BaseElement* e);
	virtual void delElement(BaseElement* e);
	void draw(HDC context) const override;
	void mouseMove(int x, int y) override;
	bool mouseClick(int x, int y) override;
	void keyDown(int key) override;
};

class Bitmap : public BaseElement {
protected:
	HBITMAP bmp;
	int image_w, image_h;
public:
	Bitmap(HBITMAP bmp, int x, int y, int w, int h);
	virtual ~Bitmap();
	void draw(HDC context) const override;
	void rotate(int angle) override;
	virtual void setImageSize(int image_w, int image_h);
};

class DragnDrop : public BaseElement {
protected:
	BaseElement* target;
	bool drag;
	int dx, dy;
public:
	DragnDrop(int x, int y, int w, int h) : BaseElement(x, y, w, h), drag(false) {}
	virtual ~DragnDrop();
	void draw(HDC context) const override;
	void mouseMove(int x, int y) override;
	bool mouseClick(int x, int y) override;
	void rotate(int angle) override;
	virtual void setTarget(BaseElement* t);
	void setDrag(bool d);
};

class Grid : public BaseElement {
protected:
	BaseElement*** elems;
	int cell_w, cell_h;
	int rows, cols;
public:
	Grid(int x, int y, int w, int h) : BaseElement(x, y, w, h), elems(nullptr) {}
	virtual ~Grid();
	virtual void setMatrix(int rows, int cols);
	virtual void setCellSize(int w, int h);
	virtual void addElement(int cell_x, int cell_y, BaseElement* elem);
	virtual void delElement(int cell_x, int cell_y);
	void draw(HDC context) const override;
};

}