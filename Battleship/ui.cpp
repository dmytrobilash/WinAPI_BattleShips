#include "ui.h"

using namespace ui;

bool BaseElement::inside(int px, int py) {
	if (px >= x && px <= this->x + w && py >= y && py <= this->y + h) {
		return true;
	}
	return false;
}

Container::~Container() {
	for (auto e : elems) {
		delete e;
	}
}

void Container::addElement(BaseElement* e) {
	elems.push_back(e);
}

void Container::delElement(BaseElement* e) {
	auto it = std::find(elems.begin(), elems.end(), e);
	if (it != elems.end()) {
		delete* it;
		elems.erase(it);
	}
}

void Container::draw(HDC context) const {
	POINT org;
	SetViewportOrgEx(context, x, y, &org);
	for (const auto e : elems) {
		e->draw(context);
	}
	SetViewportOrgEx(context, org.x, org.y, nullptr);
}

void Container::mouseMove(int x, int y) {
	int cnt_x = x - this->x;
	int cnt_y = y - this->y;

	if (inside(cnt_x, cnt_y)) {
		for (auto e : elems) {
			e->mouseMove(cnt_x, cnt_y);
		}
	}
}

bool Container::mouseClick(int x, int y) {
	int cnt_x = x - this->x;
	int cnt_y = y - this->y;

	if (inside(cnt_x, cnt_y)) {
		for (auto e : elems) {
			e->mouseClick(cnt_x, cnt_y);
		}
		return true;
	}
	return false;
}

void Container::keyDown(int key) {
	for (auto e : elems) {
		e->keyDown(key);
	}
}

Bitmap::Bitmap(HBITMAP bmp, int x, int y, int w, int h) : BaseElement(x, y, w, h) {
	this->bmp = (HBITMAP)CopyImage(bmp, IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE);
}

Bitmap::~Bitmap() {
	DeleteObject(bmp);
}

void Bitmap::draw(HDC context) const {
	HDC idc = CreateCompatibleDC(context);
	HGDIOBJ _tmp = SelectObject(idc, bmp);
	StretchBlt(context, x, y, w, h, idc, 0, 0, image_w, image_h, SRCCOPY);
	SelectObject(idc, _tmp);
	DeleteDC(idc);
}

void Bitmap::rotate(int angle) {
	if (angle != 90) {
		return; 
	}

	HDC idc = CreateCompatibleDC(GetDC(NULL));
	HDC rotated_dc = CreateCompatibleDC(GetDC(NULL));
	HBITMAP rotated_bmp = CreateCompatibleBitmap(rotated_dc, image_h, image_w);

	HGDIOBJ _tmp = SelectObject(idc, bmp);
	HGDIOBJ _rt_tmp = SelectObject(rotated_dc, rotated_bmp);
	XFORM xForm = { 0 };
	xForm.eDx = 0;
	xForm.eDy = image_w;
	xForm.eM12 = (FLOAT)-1.0;
	xForm.eM21 = (FLOAT)1.0;
	SetGraphicsMode(rotated_dc, GM_ADVANCED);
	SetWorldTransform(rotated_dc, &xForm);

	BitBlt(rotated_dc, 0, 0, image_w, image_h, idc, 0, 0, SRCCOPY);
	SelectObject(rotated_dc, _rt_tmp);
	DeleteDC(rotated_dc);
	SelectObject(idc, _tmp);
	DeleteDC(idc);
	HBITMAP t = bmp;
	bmp = rotated_bmp;
	DeleteObject(t);

	BaseElement::rotate(90);
	int it = image_w;
	image_w = image_h;
	image_h = it;
}

void Bitmap::setImageSize(int w, int h) {
	image_w = w;
	image_h = h;
}

DragnDrop::~DragnDrop() {
	delete target;
}

void DragnDrop::draw(HDC context) const {
	target->draw(context);
}

void DragnDrop::mouseMove(int x, int y) {
	if (drag) {
		target->setCoords(x - dx, y - dy);
		this->setCoords(x - dx, y - dy);
	}
}

bool DragnDrop::mouseClick(int x, int y) {
	if (inside(x, y)) {
		drag = !drag;
		dx = x - this->x;
		dy = y - this->y;
		return true;
	}
	return false;
}

void DragnDrop::rotate(int angle) {
	if (angle == 90) {
		target->rotate(angle);
		std::swap(w, h);
		std::swap(dx, dy);
		int tx, ty;
		target->getCoords(tx, ty);
		target->setCoords(tx + dy - dx, ty + dx - dy);
		this->setCoords(tx + dy - dx, ty + dx - dy);
	}
}

void DragnDrop::setTarget(BaseElement* t) {
	target = t;
}

void DragnDrop::setDrag(bool d) {
	drag = d;
}

Grid::~Grid() {
	if (elems != nullptr) {
		for (int i = 0; i < rows; ++i) {
			delete elems[i];
		}
		delete elems;
	}
	elems = nullptr;
}

void Grid::setMatrix(int r, int c) {
	if (elems != nullptr) {
		for (int i = 0; i < rows; ++i) {
			delete elems[i];
		}
		delete elems;
	}
	elems = nullptr;
	rows = r;
	cols = c;
	elems = new BaseElement**[rows];
	for (int i = 0; i < rows; ++i) {
		elems[i] = new BaseElement*[cols];
		for (int j = 0; j < cols; ++j) {
			elems[i][j] = nullptr;
		}
	}
}

void Grid::setCellSize(int w, int h) {
	cell_w = w;
	cell_h = h;
}

void Grid::addElement(int cell_x, int cell_y, BaseElement* elem) {
	if (cell_x < 0 || cell_x >= cols || cell_y < 0 || cell_y >= rows) {
		return;
	}
	elem->setCoords(cell_x * cell_w + x, cell_y * cell_h + y);
	elem->setSize(cell_w, cell_h);
	elems[cell_y][cell_x] = elem;
}

void Grid::delElement(int cell_x, int cell_y) {
	if (cell_x < 0 || cell_x >= cols || cell_y < 0 || cell_y >= rows) {
		return;
	}
	if (elems[cell_y][cell_x] != nullptr) {
		delete elems[cell_y][cell_x];
		elems[cell_y][cell_x] = nullptr;
	}
}

void Grid::draw(HDC context) const {
	for (int y = 0; y < rows; ++y) {
		for (int x = 0; x < cols; ++x) {
			if (elems[y][x] != nullptr) {
				elems[y][x]->draw(context);
			}
		}
	}
}