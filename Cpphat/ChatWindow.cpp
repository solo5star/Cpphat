#include "ChatWindow.hpp"

ChatWindow::ChatWindow()
	: m_focusedPanel(NULL)
{ }

void ChatWindow::Init() {
	ChatPannel* p1 = new ChatPannel(50, 25);
	p1->SetPosition(Point(0, 0));
	p1->SetTitle("Chat");
	p1->SetVisible(true);
	m_panels.push_back(p1);

	LogPannel* p2 = new LogPannel(50, 25);
	p2->SetPosition(Point(51, 0));
	p2->SetTitle("Log");
	p2->SetVisible(true);
	m_panels.push_back(p2);

	p1->AddKeyListener([p1, p2](char input) {
		static bool expanded = false;
		if (input == 'e') {
			expanded = !expanded;
			if (expanded) {
				p2->SetVisible(false);
				p1->SetSize(101, 25);
			}
			else {
				p1->SetSize(50, 25);
				p2->SetVisible(true);
			}
		}
	});

	TextPanel* p3 = new TextPanel(10, 5);
	p3->SetPosition(Point(102, 0));
	p3->SetText("HAVE FUN");
	p3->SetVisible(true);
	m_panels.push_back(p3);

	ScrollPanel* p4 = new ScrollPanel(10, 20);
	p4->SetPosition(Point(102, 5));
	p4->SetVisible(true);
	m_panels.push_back(p4);

	InputPanel* p5 = new InputPanel(112, 3);
	p5->SetPosition(Point(0, 25));
	p5->SetVisible(true);
	m_panels.push_back(p5);

	for (size_t i = 0; i < m_panels.size(); i++) {
		if (InputPanel* panel = dynamic_cast<InputPanel*>(m_panels[i])) {
			SetFocus(panel);
			break;
		}
	}
}

void ChatWindow::Tick() {
	if (!_kbhit()) {
		return;
	}
	int input = _getch();

	if (input == 224) {

	}
	else if (input == '\t') {
		NextFocus();
	}
	else if (m_focusedPanel != NULL) {
		m_focusedPanel->OnKey(input);
	}
}

void ChatWindow::NextFocus() {
	int index = 0;
	for (size_t i = 0; i < m_panels.size(); i++) {
		if (m_panels[i] == m_focusedPanel) {
			// find last focus
			index = i;
			break;
		}
	}
	// if cant find current focused panel, set first
	if (m_focusedPanel == NULL) {
		SetFocus(m_panels[0]);
	}
	for (size_t i = 1; i < m_panels.size(); i++) {
		// circular iterate
		Panel* check = m_panels[(index + i) % m_panels.size()];
		if (check->IsFocusable() && check->IsVisible()) {
			SetFocus(check);
			break;
		}
	}
}

void ChatWindow::SetFocus(Panel* panel) {
	if (m_focusedPanel != NULL)
		m_focusedPanel->SetFocus(false);

	m_focusedPanel = panel;
	panel->SetFocus(true);
}