#ifndef CPPHAT_PANEL_H_
#define CPPHAT_PANEL_H_

#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include <conio.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "Math.hpp"
#include "Server.hpp"
#include "Client.hpp"

class Panel;

class Canvas {
protected:
	Panel & m_panel;

public:
	Canvas(Panel& _panel);

	virtual void MoveCursor(Point point);
	virtual void DrawOutline();
	virtual void Clear(bool outline = false);
	virtual bool DrawAscii(Point& point, const char c);
	virtual bool DrawAscii(int& offset, const char c);
	virtual bool DrawString(Point& point, const std::string& str);
	virtual bool DrawString(int& offset, const std::string& str);
	virtual void DrawTitle(const std::string& str);
};

class InvisibleCanvas : public Canvas {
public:
	InvisibleCanvas(Panel& _panel)
		: Canvas(_panel)
	{ }

	void MoveCursor(Point point) { };
	void DrawOutline() { };
	void Clear(bool outline = false) { };
	bool DrawAscii(Point& point, const char c) { return true; };
	bool DrawAscii(int& offset, const char c) { return true; };
	bool DrawString(Point& point, const std::string& str) { return true; };
	bool DrawString(int& offset, const std::string& str) { return true; };
	void DrawTitle(const std::string& str) { };
};

class Panel {
private:
	Canvas m_canvas;
	InvisibleCanvas m_invisibleCanvas;

protected:
	int m_width, m_height;
	Point m_position;
	Point m_padding;
	int m_innerWidth, m_innerHeight;
	Point m_innerPosition;

	std::string m_title;
	bool m_focusable;
	bool m_focus;
	bool m_visible;
	bool m_cursorVisibility;

	std::vector<std::function<void(char)>> m_keyListeners;

	virtual void Draw() {
		GetCanvas().DrawOutline();
	}

	void RecalculateSize() {
		m_innerWidth = m_width - m_padding.X * 2;
		m_innerHeight = m_height - m_padding.Y * 2;
		m_innerPosition = m_position + m_padding;
	}

public:
	Panel(int _width, int _height = 3);

	int GetWidth() { return m_width; }
	int GetHeight() { return m_height; }
	Point GetPosition() { return m_position; }
	Point GetPadding() { return m_padding; }
	int GetInnerWidth() { return m_innerWidth; }
	int GetInnerHeight() { return m_innerHeight; }
	Point GetInnerPosition() { return m_innerPosition; }

	std::string GetTitle() { return m_title; }
	Canvas& GetCanvas() { return m_visible ? m_canvas : m_invisibleCanvas; }

	bool IsFocusable() { return m_focusable; }
	bool IsFocused() { return m_focus; }
	bool IsVisible() { return m_visible; }

	void SetSize(int _width, int _height);
	void SetPosition(Point _position);
	void SetTitle(const char * _title);
	void SetVisible(bool _visible);
	void SetCursorVisibility(bool _visibility);

	virtual void SetFocus(bool _focus);

	void AddKeyListener(std::function<void(char)> listener);
	void OnKey(char input);
};



class TextPanel : public Panel {
protected:
	std::string m_text;
	Point m_cursor;

public:
	TextPanel(int _width, int _height = 3, const char * _text = "");

	std::string GetText();

	void SetText(const char * _text);
	void SetText(std::string& _text);

	void AppendText(const char _text);
	void AppendText(const char * _text);
	void AppendText(std::string& _text);

	Point GetCursor();
	void Draw();
};

class InputPanel : public TextPanel {
public:
	InputPanel(int _width, int _height = 3);
	void SetFocus(bool _focus);
};

class ScrollPanel : public TextPanel {
protected:
	std::vector<std::string> m_list;
	int m_scrollPosition;

public:
	ScrollPanel(int _width, int _height = 3);

	bool Empty();

	void AddItem(const char * item);
	void AddItem(std::string item);
	std::string& GetItem(int index);

	void ScrollUp();
	void ScrollDown();

	void Draw();
	void DrawItems();
};

class LogPannel : public ScrollPanel, public std::ostream, public std::streambuf {
public:
	LogPannel(int _width, int _height = 3);

	int overflow(int c);
};

class ChatPannel : public ScrollPanel, public std::ostream, public std::streambuf {
public:
	ChatPannel(int _width, int _height = 3);

	int overflow(int c);
};

#endif