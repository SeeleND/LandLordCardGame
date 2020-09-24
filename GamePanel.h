#ifndef GAMEPANEL_H
#define GAMEPANEL_H

#include <QWidget>
#include <QResizeEvent>
#include <QMap>
#include <QLabel>
#include <QTimer>
#include <QPushButton>
#include "GameControl.h"
#include "CardWidget.h"
#include "MarkBoard.h"

class GamePanel : public QWidget
{
    Q_OBJECT
public:
    explicit GamePanel(QWidget *parent = 0);

public slots:
    // 用户按钮的响应
    void OnStartBtnClicked();
    void OnContinueBtnClicked();

    void OnUserNoBet();
    void OnUserBet1();
    void OnUserBet2();
    void OnUserBet3();

    void OnUserPass();
    void OnUserPlayHand();
    //////////////////////////////////////////////////////////////////////////

    void OnCardWidgetking();

    void OnCardWidgetSelected(Qt::MouseButton mouseButton);

    // 对接Player
    void OnPlayerCalledLord(Player* player, int bet);
    void OnPlayerPlayHand(Player* player, const Cards& cards);
    void OnPlayerPickCards(Player* player, const Cards& cards);

    void OnPlayerStatus(Player* player, GameControl::PlayerStatus playerStatus);

    void StartPickCard();

    void HidePlayerLastCards(Player* player);
    void UpdatePlayerCards(Player* player);

    void ProcessGameStatus(GameControl::GameStatus gameStatus);

    void InitCardWidgetMap();
    void CutCardWidget(const QPixmap& cardsPic, int x, int y, Card card);	// 在卡片图左上角为(x, y)处抠下指定尺寸，与card对应起来
    void InitControls();
    void InitPlayerContext();

protected:
    virtual void paintEvent(QPaintEvent* event);
    virtual void resizeEvent(QResizeEvent* event);

protected:
    enum CardsAlign
    {
        Horizontal,
        Vertical
    };

    struct PlayerContext
    {
        QRect cardsRect;
        QRect playHandRect;
        CardsAlign cardsAlign;
        bool isFrontSide;
        Cards lastCards;
        QLabel* info;
    };

    QMap<Player*, PlayerContext> m_playerContextMap;
private:
    GameControl* m_gameControl;

    // 发牌动画时底牌和移动牌图片
    CardWidget* m_baseCard;

    QVector<CardWidget*> m_restThreeCards;

    // 牌背面的图像
    QPixmap m_cardBackPic;

    QSize m_cardSize;
    QPoint m_baseCardPos;

    QMap<Card, CardWidget*> m_CardWidgetMap;	// 每张卡片对应一个图片
    QSet<CardWidget*> m_selectedCards;		// 用户出牌前选择的牌图片

    GameControl::GameStatus m_gameStatus;

    QTimer* m_pickCardTimer;	// 发牌计时器

    QFrame* m_userTool;
    QPushButton* m_startButton;
    QPushButton* m_continueButton;
    QPushButton* m_playHandButton;
    QPushButton* m_passButton;
    QPushButton* m_bet0Button;
    QPushButton* m_bet1Button;
    QPushButton* m_bet2Button;
    QPushButton* m_bet3Button;

};

#endif // GAMEPANEL_H
