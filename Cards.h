#ifndef CARDS_H
#define CARDS_H

#include <QVector>
#include <QSet>
#include <QMap>
#include <QList>

class CardSeq;

enum CardPoint//卡牌大小用枚举类型表示
{
    Card_Begin,

    Card_3,
    Card_4,
    Card_5,
    Card_6,
    Card_7,
    Card_8,
    Card_9,
    Card_10,
    Card_J,
    Card_Q,
    Card_K,
    Card_A,
    Card_2,

    Card_SJ,
    Card_BJ,

    Card_End
};

const int PointCount = 15;

enum CardSuit
{
    Suit_Begin,

    Diamond,	// 方块
    Club,		// 梅花
    Heart,		// 红桃
    Spade,		// 黑桃

    Suit_End
};

struct Card
{
    CardPoint point;
    CardSuit suit;
};

inline bool operator ==(const Card& left, const Card& right)
{
    return (left.point == right.point && left.suit == right.suit);
}

inline bool operator <(const Card& left, const Card& right)
{
    if (left.point == right.point)
    {
        return left.suit>right.suit;
    }
    else return left.point < right.point;
}

inline uint qHash(const Card& card)
{
    return card.point * 20 + card.suit;
}

typedef QVector<Card> CardList;

class Cards
{
public:
    Cards();
    ~Cards();

public:
    enum SortType
    {
        NoSort,
        Asc,
        Desc
    };

    void Add(const Cards& cards);//将一张牌加入
    void Add(const Card& card);
    void Add(const QVector<Cards>& cardsArray);

    void Remove(const Cards& cards);
    void Remove(const Card& card);
    void Remove(const QVector<Cards>& cardsArray);

    bool Contains(const Cards& cards) const;
    bool Contains(const Card& card) const;

    Cards& operator <<(const Cards& cards);
    Cards& operator <<(const Card& card);
    Cards& operator <<(const QVector<Cards>& cardsArray);

    CardList ToCardList(SortType sortType = Asc) const;

    int Count();
    int PointCount(CardPoint point);
    CardPoint MinPoint();
    CardPoint MaxPoint();

    Card TakeRandomCard();

    bool IsEmpty() const;
    void Clear();

protected:
    QSet<Card> cards;
};

#endif // CARDS_H
