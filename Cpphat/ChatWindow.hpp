#ifndef CPPHAT_CONSOLE_H_
#define CPPHAT_CONSOLE_H_

#include <iostream>
#include <string>
#include <vector>
#include <functional>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <conio.h>
#include "Panel.hpp"
// must be located at here

class ChatWindow {
	std::vector<Panel*> m_panels;
	Panel* m_focusedPanel;

	std::vector<std::function<void(char)>> m_keyListeners;

public:
	ChatWindow();

	void Init();
	void Tick();

	void NextFocus();
	void SetFocus(Panel* panel);
};

#endif