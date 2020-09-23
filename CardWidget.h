#ifndef CARDWidget_H
#define CARDWidget_H

#include <QWidget>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QPixmap>
#include "Cards.h"

class Player;

class CardWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CardWidget(QWidget *parent = 0);

    void SetPic(const QPixmap& pic, const QPixmap& back);
    QPixmap GetPic();

    void SetOwner(Player* owner);
    Player* GetOwner();

    void SetCard(Card card);
    Card GetCard();

    void SetFrontSide(bool frontSide);
    bool IsFrontSide();

    void SetSelected(bool selected);
    bool IsSelected();

signals:
    void NotifySelected(Qt::MouseButton);

protected:
    virtual void paintEvent(QPaintEvent* event);
    virtual void mousePressEvent(QMouseEvent* event);

protected:
    QPixmap m_pic;//正面
    QPixmap m_back;//背面
    Player* m_owner;//放在哪家前面
    Card m_card;
    bool m_selected;

    bool m_frontSide;
};

#endif // CARDPIC_H
