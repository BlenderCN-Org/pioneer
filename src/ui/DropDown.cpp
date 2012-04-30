#include "DropDown.h"
#include "Context.h"
#include "ColorBackground.h"
#include "text/TextureFont.h"

namespace UI {

DropDown::DropDown(Context *context) : Widget(context), m_textWidth(0.0f), m_popupActive(false)
{
	m_popup = GetContext()->List();
	m_popup->onOptionSelected.connect(sigc::mem_fun(onOptionSelected, &sigc::signal<void,const std::string &>::emit));
	m_popup->onClick.connect(sigc::mem_fun(this, &DropDown::HandlePopupClick));
}

void DropDown::CalcSizePos()
{
	const float textHeight = GetContext()->GetFont()->GetHeight() + GetContext()->GetFont()->GetDescender();

	m_textPos = vector2f(Skin::s_backgroundNormal.borderWidth);
	m_textSize = vector2f(m_textWidth,textHeight);

	m_backgroundPos = vector2f(0);
	m_backgroundSize = m_textSize+Skin::s_backgroundNormal.borderWidth*2;

	m_buttonPos = vector2f(m_backgroundSize.x,0);
	m_buttonSize = vector2f(m_backgroundSize.y);

	m_preferredSize = vector2f(m_backgroundSize.x+m_buttonSize.x,m_backgroundSize.y);
}

vector2f DropDown::PreferredSize()
{
	CalcSizePos();
	return m_preferredSize;
}

void DropDown::Layout()
{
	if (m_preferredSize.ExactlyEqual(0))
		CalcSizePos();

	const vector2f size(GetSize());
	SetActiveArea(vector2f(std::min(m_preferredSize.x,size.x), std::min(m_preferredSize.y,size.y)));
}

void DropDown::Draw()
{
	if (IsMouseActive()) {
		GetContext()->GetSkin().DrawBackgroundActive(m_backgroundPos, m_backgroundSize);
		GetContext()->GetSkin().DrawButtonActive(m_buttonPos, m_buttonSize);
	}
	else {
		GetContext()->GetSkin().DrawBackgroundNormal(m_backgroundPos, m_backgroundSize);
		GetContext()->GetSkin().DrawButtonNormal(m_buttonPos, m_buttonSize);
	}

	// XXX scissor
	GetContext()->GetFont()->RenderString(m_popup->GetSelectedOption().c_str(), m_textPos.x, m_textPos.y);
}

void DropDown::HandleClick()
{
	TogglePopup();
	Widget::HandleClick();
}

bool DropDown::HandlePopupClick()
{
	TogglePopup();
	return true;
}

void DropDown::TogglePopup()
{
	Context *c = GetContext();

	if (m_popupActive) {
		c->RemoveFloatingWidget(m_popup);
		m_popupActive = false;
	}

	else {
		const vector2f pos(GetAbsolutePosition() + vector2f(0, m_backgroundSize.y));
		c->AddFloatingWidget(m_popup, pos, m_popup->PreferredSize());
		m_popupActive = true;
	}

}

DropDown *DropDown::AddOption(const std::string &text)
{
	float w, h;
	GetContext()->GetFont()->MeasureString(text.c_str(), w, h);
	if (m_textWidth < w) m_textWidth = w;

	m_popup->AddOption(text);
	return this;
}

const std::string &DropDown::GetSelectedOption() const
{
	return m_popup->GetSelectedOption();
}

}
