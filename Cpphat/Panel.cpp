#include "Panel.hpp"

Canvas::Canvas(Panel& _panel)
	: m_panel(_panel)
{ }

void Canvas::MoveCursor(Point point) {
	COORD coord = { (SHORT)point.X, (SHORT)point.Y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void Canvas::DrawOutline() {
	int width = m_panel.GetWidth();
	int height = m_panel.GetHeight();
	bool focus = m_panel.IsFocused();
	Point point;

	//char box[12];
	//char shadow[5];
	//box[0] = '\xDA';//  ┌
	//box[1] = '\xBF';//  ┐
	//box[2] = '\xC0';//  └
	//box[3] = '\xD9';//  ┘
	//box[4] = '\xB3';//  │ 
	//box[5] = '\xC4';//  ─
	//box[6] = '\xC9';//  ╔ 
	//box[7] = '\xBB';//  ╗
	//box[8] = '\xC8';//  ╚
	//box[9] = '\xBC';//  ╝
	//box[10] = '\xBA';// ║
	//box[11] = '\xCD';// ═
	//shadow[1] = '\xB0';// ░
	//shadow[2] = '\xB1';// ▒
	//shadow[3] = '\xB2';// ▓
	//shadow[4] = '\xDB';// █
	//int offset = focus ? 6 : 0;

	//SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN);
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			MoveCursor(m_panel.GetPosition() + point);
			if ((x == 0) + (y == 0) == 2)
				std::cout << (focus ? "┏" : "┌");
			else if ((y == 0) + (x == width - 1) == 2)
				std::cout << (focus ? "┓" : "┐");
			else if ((x == 0) + (y == height - 1) == 2)
				std::cout << (focus ? "┗" : "└");
			else if ((x == width - 1) + (y == height - 1) == 2)
				std::cout << (focus ? "┛" : "┘");
			else if (y != 0 && y != height - 1 && (x == 0 || x == width - 1))
				std::cout << (focus ? "┃" : "│");
			else if (x != 0 && x != width - 1 && (y == 0 || y == height - 1))
				std::cout << (focus ? "━" : "─");

			//if ((x == 0) + (y == 0) == 2)
			//	_cputs(&box[0 + offset]);
			//else if ((y == 0) + (x == width - 1) == 2)
			//	_cputs(&box[1 + offset]);
			//else if ((x == 0) + (y == height - 1) == 2)
			//	_cputs(&box[2 + offset]);
			//else if ((x == width - 1) + (y == height - 1) == 2)
			//	_cputs(&box[3 + offset]);
			//else if (y != 0 && y != height - 1 && (x == 0 || x == width - 1))
			//	_cputs(&box[4 + offset]);
			//else if (x != 0 && x != width - 1 && (y == 0 || y == height - 1))
			//	_cputs(&box[5 + offset]);

			point.X++;
		}
		point.X = 0;
		point.Y++;
	}
	std::string title = m_panel.GetTitle();
	if (!title.empty()) {
		DrawTitle(title);
	}
}

void Canvas::Clear(bool outline) {
	if (outline) for (int y = 0; y < m_panel.GetHeight(); y++) {
		MoveCursor(m_panel.GetPosition() + Point(0, y));
		for (int x = 0; x < m_panel.GetWidth(); x++) {
			_cputs(" ");
		}
	}
	else for (int y = 0; y < m_panel.GetInnerHeight(); y++) {
		MoveCursor(m_panel.GetInnerPosition() + Point(0, y));
		for (int x = 0; x < m_panel.GetInnerWidth(); x++) {
			_cputs(" ");
		}
	}
}

bool Canvas::DrawAscii(Point& point, const char c) {
	// bound check
	if (!(Point(0, 0) <= point && point <= Point(m_panel.GetInnerWidth(), m_panel.GetInnerHeight()))) {
		return true; // overflow
	}
	switch (c) {
	case '\n':
		if (++point.Y == m_panel.GetInnerHeight()) {
			point.Y--;
			return true; // overflow
		}
		point.X = 0;
		MoveCursor(m_panel.GetInnerPosition() + point);
		break;
	case '\b':
		if (--point.X == -1) {
			if (--point.Y == -1) {
				point.X = 0;
				point.Y = 0;
				return true; // underflow
			}
			point.X = m_panel.GetInnerWidth() - 1;
		}
		std::cout << "\b \b";
		break;
	default:
		if (point.X + 1 == m_panel.GetInnerWidth()) {
			if (point.Y + 1 == m_panel.GetInnerHeight()) {
				return true; // overflow;
			}
			point.X = 0;
			point.Y++;
		}
		MoveCursor(m_panel.GetInnerPosition() + point);
		std::cout << c;
		point.X++;
	}
	return false;
}

bool Canvas::DrawAscii(int& offset, const char c) {
	Point point = Point::from1D(offset, m_panel.GetInnerWidth());
	bool overflow = DrawAscii(point, c); // overflow or underflow
	offset = point.to1D(m_panel.GetInnerWidth());
	return overflow;
}

bool Canvas::DrawString(Point& point, const std::string& str) {
	for (size_t i = 0; i < str.length(); i++) {
		if (DrawAscii(point, str[i])) {
			return true;
		}
	}
	return false;
}

bool Canvas::DrawString(int& offset, const std::string& str) {
	Point point = Point::from1D(offset, m_panel.GetInnerWidth());
	bool overflow = DrawString(point, str); // overflow or underflow
	offset = point.to1D(m_panel.GetInnerWidth());
	return overflow;
}

void Canvas::DrawTitle(const std::string& str) {
	MoveCursor(m_panel.GetPosition() + Point(2, 0));
	std::cout << "  ";
	std::cout << str;
	std::cout << "  ";
}

Panel::Panel(int _width, int _height)
	: m_padding(2, 1)
	, m_canvas(*this)
	, m_invisibleCanvas(*this)
	, m_focusable(true)
	, m_focus(false)
	, m_visible(false)
	, m_cursorVisibility(false)
{
	SetSize(_width, _height);
}

void Panel::SetSize(int _width, int _height) {
	GetCanvas().Clear(true);

	m_width = _width;
	m_height = _height;

	RecalculateSize();
	Draw();
}

void Panel::SetPosition(Point _position) {
	GetCanvas().Clear(true);

	m_position = _position;
	RecalculateSize();

	Draw();
}

void Panel::SetTitle(const char * _title) {
	m_title = _title;
	GetCanvas().DrawTitle(m_title);
}

void Panel::SetVisible(bool _visible) {
	if (m_visible == _visible) {
		return;
	}
	if (m_visible) { // current visible
		GetCanvas().Clear(true); // if visible, clear
		m_visible = false;
	}
	else {
		m_visible = true;
		Draw(); // not visible, draw
	}
}
void Panel::SetCursorVisibility(bool _visibility) {
	m_cursorVisibility = _visibility;
}

void Panel::SetFocus(bool _focus) {
	m_focus = _focus;

	// Cursor visibility set
	CONSOLE_CURSOR_INFO curinfo = { 10, m_cursorVisibility };
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curinfo);

	m_canvas.DrawOutline();
}

void Panel::AddKeyListener(std::function<void(char)> listener) {
	m_keyListeners.push_back(listener);
}

void Panel::OnKey(char input) {
	for (size_t i = 0; i < m_keyListeners.size(); i++)
		m_keyListeners[i](input);
}

TextPanel::TextPanel(int _width, int _height, const char * _text)
	: Panel(_width, _height)
	, m_text(_text)
	, m_cursor()
{ }

std::string TextPanel::GetText() {
	return m_text;
}

void TextPanel::SetText(const char * _text) {
	m_text = _text;
	GetCanvas().Clear();
	m_cursor.X = 0;
	m_cursor.Y = 0;
	GetCanvas().DrawString(m_cursor, _text);
	GetCanvas().MoveCursor(GetInnerPosition() + m_cursor);
}

void TextPanel::SetText(std::string& _text) {
	m_text = _text;
	GetCanvas().Clear();
	m_cursor.X = 0;
	m_cursor.Y = 0;
	GetCanvas().DrawString(m_cursor, _text);
	GetCanvas().MoveCursor(GetInnerPosition() + m_cursor);
}

void TextPanel::AppendText(const char _text) {
	m_text += _text;
	GetCanvas().DrawAscii(m_cursor, _text);
}
void TextPanel::AppendText(const char * _text) {
	m_text += _text;
	GetCanvas().DrawString(m_cursor, _text);
}
void TextPanel::AppendText(std::string& _text) {
	m_text += _text;
	GetCanvas().DrawString(m_cursor, _text);
}

Point TextPanel::GetCursor() {
	return m_cursor;
}

void TextPanel::Draw() {
	GetCanvas().DrawOutline(); GetCanvas().DrawString(m_cursor, m_text);
}

InputPanel::InputPanel(int _width, int _height)
	: TextPanel(_width, _height)
{
	SetCursorVisibility(true);
	AddKeyListener([&](char input) {
		if (input == 224 || input == 0) {
			switch (_getch()) {
			case 72: // UP
			case 80: // DOWN
			case 75: // LEFT
			case 77: // RIGHT
				break;
			}
		}
		else if (input == '\r') {
			std::string str = GetText();
			if (str == "/client start") {
				Client::Instance.Init();
			}
			else if (str == "/client close") {
				Client::Instance.Close();
			}
			else if (str == "/server start") {
				Server::Instance.Init();
			}
			//else if (str == "/server stop") {
			//	Server::Instance.Close();
			//}
			else {
				Client::Instance.SendMessage(str);
			}
			SetText("");
		}
		else if (input == '\b') {
			if (m_text.empty()) return;

			m_text.pop_back();
			GetCanvas().DrawAscii(m_cursor, input);
		}
		else {
			AppendText(input);
		}
	});
}

void InputPanel::SetFocus(bool _focus) {
	Panel::SetFocus(_focus);

	GetCanvas().MoveCursor(GetInnerPosition() + m_cursor);
}

ScrollPanel::ScrollPanel(int _width, int _height)
	: TextPanel(_width, _height)
	, m_scrollPosition(0)
{
	AddKeyListener([&](char input) {
		switch (input) {
		case 'j': ScrollUp(); break; // UP
		case 'k': ScrollDown(); break; // DOWN
		}
		//if (input == 224 || input == 0) {
		//	switch (_getch()) {
		//	case 72: // UP
		//		ScrollUp(); break;
		//	case 80: // DOWN
		//		ScrollDown(); break;
		//	case 75: // LEFT
		//	case 77: // RIGHT
		//		break;
		//	}
		//}
	});
}

bool ScrollPanel::Empty() {
	return m_list.empty();
}

void ScrollPanel::AddItem(const char * item) {
	m_list.push_back(item);
	DrawItems();
}

void ScrollPanel::AddItem(std::string item) {
	m_list.push_back(item);
	DrawItems();
}

std::string& ScrollPanel::GetItem(int index) {
	return m_list[index];
}

void ScrollPanel::ScrollUp() {
	m_scrollPosition = max(0, m_scrollPosition - 1);
	DrawItems();
}

void ScrollPanel::ScrollDown() {
	m_scrollPosition = min(m_list.size() - 1, m_scrollPosition + 1);
	DrawItems();
}

void ScrollPanel::Draw() {
	GetCanvas().DrawOutline();

	DrawItems();
}

void ScrollPanel::DrawItems() {
	GetCanvas().Clear();

	if (m_list.empty()) return;

	int index = m_list.size() - m_scrollPosition - 1;
	Point offset = Point(0, GetInnerHeight());
	Point temp = offset;
	do {
		int linelen = m_list[index].length() / GetInnerWidth() + 1;
		offset.Y -= linelen;
		temp = offset;
	} while (!GetCanvas().DrawString(temp, m_list[index--]) && index != -1);
}

LogPannel::LogPannel(int _width, int _height)
	: ScrollPanel(_width, _height)
	, std::ostream(this)
{
	Logger::SetOutputStream(*this);
}

int LogPannel::overflow(int c) {
	static std::string buff;

	if ((char)c == Logger::EOL) {
		AddItem(buff);
		buff.clear();
	}
	else {
		buff += c;
	}

	return 0;
}

ChatPannel::ChatPannel(int _width, int _height)
	: ScrollPanel(_width, _height)
	, std::ostream(this)
{
	Client::SetOutputStream(*this);

	for (auto kv : Configure::Ini.GetAll()) {
		Client::GetOutputStream() << kv.first << "=" << kv.second << Logger::EOL;
	}

	Client::GetOutputStream() << "Tip : You can expand chat panel with 'e' key" << Logger::EOL;
	Client::GetOutputStream() << "Tip : This program will quicken when log pannel is collapsed, since the panel is slow" << Logger::EOL;
	Client::GetOutputStream() << "Tip : Start client with '/client start'" << Logger::EOL;
	Client::GetOutputStream() << "Tip : Start server with '/server start'" << Logger::EOL;
	Client::GetOutputStream() << "Tip : Stop client with '/client close'" << Logger::EOL;
	//Client::GetOutputStream() << "Tip : Stop server with '/server stop'" << Logger::EOL;
}

int ChatPannel::overflow(int c) {
	static std::string buff;

	if ((char)c == Logger::EOL) {
		AddItem(buff);
		buff.clear();
	}
	else {
		buff += c;
	}

	return 0;
}