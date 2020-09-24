#include "GamePanel.h"
#include <QPainter>
#include <QPushButton>
#include <QDebug>
#include <QMessageBox>
#include <QHBoxLayout>
#include "CardSeq.h"

GamePanel::GamePanel(QWidget *parent) :
    QWidget(parent)
{
    m_gameControl = new GameControl(this);
    m_gameControl->Init();

    m_cardSize = QSize(100, 151);

    InitCardWidgetMap();//分割图片
    InitControls();//加载按钮
    InitPlayerContext();

    //////////////////////////////////////////////////////////////////////////

    connect(m_gameControl, &GameControl::NotifyPlayerCalledLord, this, &GamePanel::OnPlayerCalledLord);
    connect(m_gameControl, &GameControl::NotifyPlayerPlayHand, this, &GamePanel::OnPlayerPlayHand);
    connect(m_gameControl, &GameControl::NotifyPlayerPickCards, this, &GamePanel::OnPlayerPickCards);
    connect(m_gameControl, &GameControl::NotifyGameStatus, this, &GamePanel::ProcessGameStatus);
    connect(m_gameControl, &GameControl::NotifyPlayerStatus, this, &GamePanel::OnPlayerStatus);

    //////////////////////////////////////////////////////////////////////////

    m_baseCard = new CardWidget(this);
    m_baseCard->SetPic(m_cardBackPic, m_cardBackPic);

    // 剩下3张牌抢地主
    CardWidget* restCardWidget = new CardWidget(this);
    m_restThreeCards << restCardWidget;
    restCardWidget = new CardWidget(this);
    m_restThreeCards << restCardWidget;
    restCardWidget = new CardWidget(this);
    m_restThreeCards << restCardWidget;

    // 发牌效果所用的计时器
    m_pickCardTimer = new QTimer(this);
    m_pickCardTimer->setInterval(80);
    connect(m_pickCardTimer, &QTimer::timeout, this, &GamePanel::OnCardWidgetPicking);
}

void GamePanel::InitPlayerContext()
{
    PlayerContext context;

    context.cardsAlign = Vertical;
    context.isFrontSide = false;
    m_playerContextMap.insert(m_gameControl->GetLeftRobot(), context);

    context.cardsAlign = Vertical;
    context.isFrontSide = false;
    m_playerContextMap.insert(m_gameControl->GetRightRobot(), context);

    context.cardsAlign = Horizontal;
    context.isFrontSide = true;
    m_playerContextMap.insert(m_gameControl->GetUserPlayer(), context);

    QMap<Player*, PlayerContext>::Iterator it = m_playerContextMap.begin();
    for (; it != m_playerContextMap.end(); it++)
    {
        it->info = new QLabel(this);
        it->info->resize(100, 50);
        it->info->hide();
    }
}

void GamePanel::InitControls()
{
    m_userTool = new QFrame(this);
    QHBoxLayout* horLayout = new QHBoxLayout(m_userTool);
    horLayout->setSpacing(9);
    horLayout->addItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));

    m_startButton = new QPushButton(m_userTool);
    m_startButton->setText(tr("Start Game"));
    m_startButton->show();
    m_startButton->setFixedSize(89, 32);
    horLayout->addWidget(m_startButton);
    connect(m_startButton, &QPushButton::clicked, this, &GamePanel::OnStartBtnClicked);

    m_continueButton = new QPushButton(m_userTool);
    m_continueButton->setText(tr("Continue Game"));
    m_continueButton->hide();
    m_continueButton->setFixedSize(89, 32);
    horLayout->addWidget(m_continueButton);
    connect(m_continueButton, &QPushButton::clicked, this, &GamePanel::OnContinueBtnClicked);

    m_passButton = new QPushButton(m_userTool);
    m_passButton->setText(tr("btn_pass"));
    m_passButton->setFocusPolicy(Qt::NoFocus);
    m_passButton->hide();
    m_passButton->setFixedSize(89, 32);
    horLayout->addWidget(m_passButton);
    connect(m_passButton, &QPushButton::clicked, this, &GamePanel::OnUserPass);

    m_playHandButton = new QPushButton(m_userTool);
    m_playHandButton->setText(tr("play hand"));
    m_playHandButton->hide();
    m_playHandButton->setFixedSize(89, 32);
    QFont font = m_playHandButton->font();
    font.setBold(true);
    font.setPixelSize(14);
    m_playHandButton->setFont(font);
    horLayout->addWidget(m_playHandButton);
    connect(m_playHandButton, &QPushButton::clicked, this, &GamePanel::OnUserPlayHand);

    //////////////////////////////////////////////////////////////////////////

    m_bet0Button = new QPushButton(m_userTool);
    m_bet0Button->setObjectName("button");
    m_bet0Button->setText(tr("bet0"));
    m_bet0Button->hide();
    m_bet0Button->setFixedSize(89, 32);
    horLayout->addWidget(m_bet0Button);
    connect(m_bet0Button, &QPushButton::clicked, this, &GamePanel::OnUserNoBet);

    m_bet1Button = new QPushButton(m_userTool);
    m_bet1Button->setText(tr("bet1"));
    m_bet1Button->hide();
    m_bet1Button->setFixedSize(89, 32);
    horLayout->addWidget(m_bet1Button);
    connect(m_bet1Button, &QPushButton::clicked, this, &GamePanel::OnUserBet1);

    m_bet2Button = new QPushButton(m_userTool);
    m_bet2Button->setText(tr("bet2"));
    m_bet2Button->hide();
    m_bet2Button->setFixedSize(89, 32);
    horLayout->addWidget(m_bet2Button);
    connect(m_bet2Button, &QPushButton::clicked, this, &GamePanel::OnUserBet2);

    m_bet3Button = new QPushButton(m_userTool);
    m_bet3Button->setText(tr("bet3"));
    m_bet3Button->hide();
    m_bet3Button->setFixedSize(89, 32);
    horLayout->addWidget(m_bet3Button);
    connect(m_bet3Button, &QPushButton::clicked, this, &GamePanel::OnUserBet3);

    horLayout->addItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));
}

void GamePanel::CutCardWidget(const QPixmap& cardsPic,int x, int y, Card card)
{
    QPixmap pic = cardsPic.copy(x, y, m_cardSize.width(), m_cardSize.height());

    CardWidget* cardPic = new CardWidget(this);
    cardPic->hide();
    cardPic->SetPic(pic, m_cardBackPic);

    cardPic->SetCard(card);
    m_CardWidgetMap.insert(card, cardPic);

    connect(cardPic, &CardWidget::NotifySelected, this, &GamePanel::OnCardWidgetSelected);
}

void GamePanel::InitCardWidgetMap()
{
    QPixmap cardsPic(":/res/card.png");

    //卡背单独处理
    m_cardBackPic = cardsPic.copy(13 * m_cardSize.width(), 2 * m_cardSize.height(),
        m_cardSize.width(), m_cardSize.height());

    for (int suit = Suit_Begin + 1, i = 0; suit < Suit_End; suit++, i++)
    {
        for (int pt = Card_Begin + 1, j = 0; pt < Card_SJ; pt++, j++)
        {
            Card card;
            card.point = (CardPoint)pt;
            card.suit = (CardSuit)suit;

            CutCardWidget(cardsPic, j * m_cardSize.width(), i * m_cardSize.height(), card);
        }
    }

    // 大小王较特殊，分别处理
    Card card;

    card.point = Card_SJ;
    card.suit = Suit_Begin;
    CutCardWidget(cardsPic, 13*m_cardSize.width(), 0, card);

    card.point = Card_BJ;
    card.suit = Suit_Begin;
    CutCardWidget(cardsPic, 13*m_cardSize.width(), m_cardSize.height(), card);
}

void GamePanel::paintEvent(QPaintEvent* event)
{
    static QPixmap bk(":/res/table.png");
    QPainter painter(this);
    painter.drawPixmap(rect(), bk);
}

void GamePanel::resizeEvent(QResizeEvent* event)
{
    m_baseCardPos = QPoint((width() - m_cardSize.width()) / 2, height() / 2 - 100);
    m_baseCard->move(m_baseCardPos);

    //////////////////////////////////////////////////////////////////////////
    QMap<Player*, PlayerContext>::Iterator it;

    // 左机器人
    it = m_playerContextMap.find(m_gameControl->GetLeftRobot());
    it->cardsRect = QRect(10, 180, 100, height() - 200);
    it->playHandRect = QRect(180, 150, 100, 100);

    // 右机器人
    it = m_playerContextMap.find(m_gameControl->GetRightRobot());
    it->cardsRect = QRect(rect().right() - 110, 180, 100, height() - 200);
    it->playHandRect = QRect(rect().right() - 280, 150, 100, 100);

    // 我
    it = m_playerContextMap.find(m_gameControl->GetUserPlayer());
    it->cardsRect = QRect(250, rect().bottom() - 120, width() - 500, 100);
    it->playHandRect = QRect(150, rect().bottom() - 280, width() - 300, 100);

    //
    QRect rectUserTool(it->playHandRect.left(), it->playHandRect.top(),
        it->playHandRect.width(), it->cardsRect.top() - it->playHandRect.bottom());

    m_userTool->setGeometry(rectUserTool);

    //
    for (it = m_playerContextMap.begin(); it != m_playerContextMap.end(); it++)
    {
        QRect playCardsRect = it->playHandRect;
        QLabel* infoLabel = it->info;

        QPoint pt( playCardsRect.left() + (playCardsRect.width() - infoLabel->width()) / 2,
            playCardsRect.top() + (playCardsRect.height() - infoLabel->height()) / 2 );
        infoLabel->move(pt);
    }

    // 显示剩下三张牌
    int base = (width() - 3 * m_cardSize.width() - 2 * 10) / 2;
    for (int i = 0; i < m_restThreeCards.size(); i++)
    {
        m_restThreeCards[i]->move(base + (m_cardSize.width() + 10) * i, 20);
    }

    UpdatePlayerCards(m_gameControl->GetUserPlayer());
    UpdatePlayerCards(m_gameControl->GetLeftRobot());
    UpdatePlayerCards(m_gameControl->GetRightRobot());
}

void GamePanel::OnCardWidgetPicking()
{
    Player* curPlayer = m_gameControl->GetCurrentPlayer();

    Card card = m_gameControl->PickOneCard();
    curPlayer->PickCard(card);

    m_gameControl->SetCurrentPlayer(curPlayer->GetNextPlayer());

    if (m_gameControl->GetRestCards().Count() == 3)	// 还剩下3张牌，摸牌结束
    {
        m_pickCardTimer->stop();
        ProcessGameStatus(GameControl::CallingLordStatus);
    }

}

void GamePanel::OnCardWidgetSelected(Qt::MouseButton mouseButton)
{
    if (m_gameStatus == GameControl::PickingCardStatus) return;

    CardWidget* cardPic = (CardWidget*)sender();
    if (cardPic->GetOwner() != m_gameControl->GetUserPlayer()) return;

    if (mouseButton == Qt::LeftButton)
    {
        cardPic->SetSelected(!cardPic->IsSelected());
        UpdatePlayerCards(cardPic->GetOwner());

        QSet<CardWidget*>::Iterator it = m_selectedCards.find(cardPic);
        if (it == m_selectedCards.end())		// 选中牌
        {
            m_selectedCards.insert(cardPic);
        }
        else	// 取消选中牌
        {
            m_selectedCards.erase(it);
        }
    }
}

void GamePanel::HidePlayerLastCards(Player* player)
{
    QMap<Player*, PlayerContext>::Iterator it = m_playerContextMap.find(player);
    if (it != m_playerContextMap.end())
    {
        if (it->lastCards.IsEmpty())	// 上一次打的空牌，即pass
        {
            it->info->hide();
        }
        else
        {
            CardList lastCardList = it->lastCards.ToCardList();
            CardList::ConstIterator itLast = lastCardList.constBegin();
            for (; itLast != lastCardList.constEnd(); itLast++)
            {
                m_CardWidgetMap[*itLast]->hide();
            }
        }
    }
}

void GamePanel::UpdatePlayerCards(Player* player)
{
    Cards restCards = player->GetCards();
    CardList restCardList = restCards.ToCardList(Cards::Desc);

    const int cardSpacing = 40;		// 牌间隔

    // 显示剩下的牌
    QRect cardsRect = m_playerContextMap[player].cardsRect;
    CardList::ConstIterator itRest = restCardList.constBegin();
    for (int i = 0; itRest != restCardList.constEnd(); itRest++, i++)
    {
        CardWidget* cardPic = m_CardWidgetMap[*itRest];
        cardPic->SetFrontSide(m_playerContextMap[player].isFrontSide);
        cardPic->show();
        cardPic->raise();

        if (m_playerContextMap[player].cardsAlign == Horizontal)
        {
            int leftBase = cardsRect.left() + (cardsRect.width() - (restCardList.size() - 1) * cardSpacing - cardPic->width()) / 2;
            int top = cardsRect.top() + (cardsRect.height() - cardPic->height()) / 2;
            if (cardPic->IsSelected()) top -= 10;

            cardPic->move(leftBase + i * cardSpacing, top);
        }
        else
        {
            int left = cardsRect.left() + (cardsRect.width() - cardPic->width()) / 2;
            if (cardPic->IsSelected()) left += 10;
            int topBase = cardsRect.top() + (cardsRect.height() - (restCardList.size() - 1) * cardSpacing - cardPic->height()) / 2;
            cardPic->move(left, topBase + i * cardSpacing);
        }
    }

    // 显示上一次打出去的牌
    QRect playCardsRect = m_playerContextMap[player].playHandRect;
    if (!m_playerContextMap[player].lastCards.IsEmpty())		// 不是空牌
    {
        int playSpacing = 24;
        CardList lastCardList = m_playerContextMap[player].lastCards.ToCardList();
        CardList::ConstIterator itPlayed = lastCardList.constBegin();
        for (int i = 0; itPlayed != lastCardList.constEnd(); itPlayed++, i++)
        {
            CardWidget* cardPic = m_CardWidgetMap[*itPlayed];
            cardPic->SetFrontSide(true);
            cardPic->raise();

            if (m_playerContextMap[player].cardsAlign == Horizontal)
            {
                int leftBase = playCardsRect.left () +
                    (playCardsRect.width() - (lastCardList.size() - 1) * playSpacing - cardPic->width()) / 2;
                int top = playCardsRect.top() + (playCardsRect.height() - cardPic->height()) / 2;
                cardPic->move(leftBase + i * playSpacing, top);
            }
            else
            {
                int left = playCardsRect.left() + (playCardsRect.width() - cardPic->width()) / 2;
                int topBase = playCardsRect.top();
                cardPic->move(left, topBase + i * playSpacing);
            }
        }
    }
}

void GamePanel::OnPlayerCalledLord(Player* player, int bet)
{
    QString betInfo;
    if (bet == 0)
    {
        betInfo = tr("bet0");
    }
    else if (bet == 1)
    {
        betInfo = tr("bet1");
    }
    else if (bet == 2)
    {
        betInfo = tr("bet2");
    }
    else if (bet == 3)
    {
        betInfo = tr("bet3");
    }

    QLabel* infoLabel = m_playerContextMap[player].info;
    infoLabel->setText(betInfo);
    infoLabel->show();

    // 用户叫完分，隐藏叫分按钮
    if (player == m_gameControl->GetUserPlayer())
    {
        m_bet0Button->hide();
        m_bet1Button->hide();
        m_bet2Button->hide();
        m_bet3Button->hide();
    }
}

void GamePanel::OnPlayerPlayHand(Player* player, const Cards& cards)
{
    HidePlayerLastCards(player);

    // 记录上一次打的牌
    QMap<Player*, PlayerContext>::Iterator itContext = m_playerContextMap.find(player);
    itContext->lastCards = cards;

    // 打空牌，显示“不要”
    if (cards.IsEmpty())
    {
        itContext->info->setText(tr("label_pass"));
        itContext->info->show();
    }

    UpdatePlayerCards(player);
}

void GamePanel::OnPlayerPickCards(Player* player, const Cards& cards)
{
    CardList cardList = cards.ToCardList();
    CardList::ConstIterator it = cardList.constBegin();
    for (; it != cardList.constEnd(); it++)
    {
        CardWidget* pickedCard = m_CardWidgetMap[*it];
        pickedCard->SetOwner(player);
    }

    UpdatePlayerCards(player);
}

void GamePanel::OnUserNoBet()
{
    m_gameControl->GetUserPlayer()->CallLord(0);
}

void GamePanel::OnUserBet1()
{
    m_gameControl->GetUserPlayer()->CallLord(1);
}

void GamePanel::OnUserBet2()
{
    m_gameControl->GetUserPlayer()->CallLord(2);
}

void GamePanel::OnUserBet3()
{
    m_gameControl->GetUserPlayer()->CallLord(3);
}

void GamePanel::OnUserPass()
{
    if (m_gameControl->GetCurrentPlayer() != m_gameControl->GetUserPlayer()) return;

    Player* punchPlayer = m_gameControl->GetPunchPlayer();
    if (punchPlayer == m_gameControl->GetUserPlayer() || punchPlayer == NULL) return;

    m_gameControl->GetUserPlayer()->PlayHand(Cards());

    QSet<CardWidget*>::ConstIterator it = m_selectedCards.constBegin();
    for (; it != m_selectedCards.constEnd(); it++)
    {
        (*it)->SetSelected(false);
    }
    m_selectedCards.clear();

    UpdatePlayerCards(m_gameControl->GetUserPlayer());
}

void GamePanel::OnUserPlayHand()
{
    if (m_gameStatus != GameControl::PlayingHandStatus)
    {
        return;
    }

    if (m_gameControl->GetCurrentPlayer() != m_gameControl->GetUserPlayer())
    {
        return;
    }

    if (m_selectedCards.isEmpty()) return;

    Cards playCards;
    QSet<CardWidget*>::const_iterator it = m_selectedCards.constBegin();
    for (; it != m_selectedCards.constEnd(); it++)
    {
        playCards.Add((*it)->GetCard());
    }

    CardSeq hand(playCards);
    Type ht = hand.GetType();

    if (ht == Unknown)		// 未知牌型，不允许出
    {
        return;
    }

    // 管不住其他人的牌
    if (m_gameControl->GetPunchPlayer() != m_gameControl->GetUserPlayer())
    {
        if (!hand.CanBeat(CardSeq(m_gameControl->GetPunchCards())))
        {
            return;
        }
    }

    m_gameControl->GetUserPlayer()->PlayHand(playCards);
    m_selectedCards.clear();
}

void GamePanel::OnStartBtnClicked()
{
    m_startButton->hide();

    ProcessGameStatus(GameControl::PickingCardStatus);
}

void GamePanel::OnContinueBtnClicked()
{
    m_continueButton->hide();
    ProcessGameStatus(GameControl::PickingCardStatus);
}

void GamePanel::StartPickCard()
{
    QMap<Card, CardWidget*>::const_iterator it = m_CardWidgetMap.constBegin();
    for (; it != m_CardWidgetMap.constEnd(); it++)
    {
        it.value()->SetSelected(false);
        it.value()->SetFrontSide(true);
        it.value()->hide();
    }

    for (int i = 0; i < m_restThreeCards.size(); i++)
    {
        m_restThreeCards[i]->hide();
    }

    m_selectedCards.clear();
    m_playerContextMap[m_gameControl->GetLeftRobot()].lastCards.Clear();
    m_playerContextMap[m_gameControl->GetRightRobot()].lastCards.Clear();
    m_playerContextMap[m_gameControl->GetUserPlayer()].lastCards.Clear();

    m_playerContextMap[m_gameControl->GetLeftRobot()].info->hide();
    m_playerContextMap[m_gameControl->GetRightRobot()].info->hide();
    m_playerContextMap[m_gameControl->GetUserPlayer()].info->hide();

    m_playerContextMap[m_gameControl->GetLeftRobot()].isFrontSide = false;
    m_playerContextMap[m_gameControl->GetRightRobot()].isFrontSide = false;
    m_playerContextMap[m_gameControl->GetUserPlayer()].isFrontSide = true;

    m_gameControl->Clear();

    m_baseCard->show();

    m_bet0Button->hide();
    m_bet1Button->hide();
    m_bet2Button->hide();
    m_bet3Button->hide();

    m_playHandButton->hide();
    m_passButton->hide();

    m_pickCardTimer->start();
}

void GamePanel::ProcessGameStatus(GameControl::GameStatus gameStatus)
{
    m_gameStatus = gameStatus;

    if (gameStatus == GameControl::PickingCardStatus)
    {
        StartPickCard();
    }
    else if (gameStatus == GameControl::CallingLordStatus)
    {
        CardList restThreeCards = m_gameControl->GetRestCards().ToCardList();

        for (int i = 0; i < m_restThreeCards.size(); i++)
        {
            m_restThreeCards[i]->SetPic(m_CardWidgetMap[restThreeCards[i]]->GetPic(), m_cardBackPic);
            m_restThreeCards[i]->hide();
        }

        m_gameControl->StartCallLord();
    }
    else if (gameStatus == GameControl::PlayingHandStatus)
    {
        m_baseCard->hide();

        for (int i = 0; i < m_restThreeCards.size(); i++)
        {
            m_restThreeCards[i]->show();
        }

        QMap<Player*, PlayerContext>::ConstIterator it = m_playerContextMap.constBegin();
        for (; it != m_playerContextMap.constEnd(); it++)
        {
            it.value().info->hide();
        }
    }
}

void GamePanel::OnPlayerStatus(Player* player, GameControl::PlayerStatus playerStatus)
{
    if (playerStatus == GameControl::ThinkingForCallLordStatus)
    {
        if (player == m_gameControl->GetUserPlayer())	// 用户叫分，显示叫分按钮
        {
            m_bet0Button->show();
            m_bet1Button->show();
            m_bet2Button->show();
            m_bet3Button->show();
        }
    }
    else if (playerStatus == GameControl::ThinkingForPlayHandStatus)
    {
        HidePlayerLastCards(player);

        if (player == m_gameControl->GetUserPlayer())
        {
            m_playHandButton->show();

            Player* punchPlayer = m_gameControl->GetPunchPlayer();
            if (punchPlayer == m_gameControl->GetUserPlayer() || punchPlayer == NULL)
            {
                m_passButton->hide();
            }
            else
            {
                m_passButton->show();
            }
        }
        else
        {
            m_playHandButton->hide();
            m_passButton->hide();
        }
    }
    else if (playerStatus == GameControl::WinningStatus)
    {
        m_playerContextMap[m_gameControl->GetLeftRobot()].isFrontSide = true;
        m_playerContextMap[m_gameControl->GetRightRobot()].isFrontSide = true;
        UpdatePlayerCards(m_gameControl->GetLeftRobot());
        UpdatePlayerCards(m_gameControl->GetRightRobot());

        QString str = tr("%1 wins the game.");
        str = str.arg(player->GetName());
        QMessageBox::information(this, tr("tip"), str);

        m_playHandButton->hide();
        m_passButton->hide();
        m_continueButton->show();

        // 赢者下一局先叫分
        m_gameControl->SetCurrentPlayer(player);
    }
}

