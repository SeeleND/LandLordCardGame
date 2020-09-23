#include "CardWidget.h"
#include <QPainter>
#include "Player.h"

 CardWidget:: CardWidget(QWidget *parent) :
    QWidget(parent)
{
    m_owner = NULL;
    m_selected = false;
    m_frontSide = true;
}

void  CardWidget::SetPic(const QPixmap& pic, const QPixmap& back)
{
    m_pic = pic;
    m_back = back;

    resize(pic.size());
    update();
}

QPixmap  CardWidget::GetPic()
{
    return m_pic;
}

void  CardWidget::SetOwner(Player* owner)
{
    m_owner = owner;
}

Player*  CardWidget::GetOwner()
{
    return m_owner;
}

void  CardWidget::SetCard(Card card)
{
    m_card = card;
}

Card  CardWidget::GetCard()
{
    return m_card;
}

void  CardWidget::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);

    if (m_frontSide)
    {
        painter.drawPixmap(rect(), m_pic);
    }
    else
    {
        painter.drawPixmap(rect(), m_back);
    }

}

void  CardWidget::mousePressEvent(QMouseEvent* event)
{
    emit NotifySelected(event->button());
}

void  CardWidget::SetFrontSide(bool frontSide)
{
    if (m_frontSide != frontSide)
    {
        m_frontSide = frontSide;
        update();
    }
}

bool  CardWidget::IsFrontSide()
{
    return m_frontSide;
}

void  CardWidget::SetSelected(bool selected)
{
    m_selected = selected;
}

bool  CardWidget::IsSelected()
{
    return m_selected;
}
