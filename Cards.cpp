#include "Cards.h"
#include "Hand.h"
#include <QTime>

Cards::Cards()
{
}

Cards::~Cards()
{

}

void Cards::Clear()
{
    cards.clear();
}

bool Cards::IsEmpty() const
{
    return cards.isEmpty();
}

void Cards::Add(const Cards& cards)
{
    this->cards = this->cards.unite(cards.cards);

    QSet<Card>::const_iterator it   = cards.cards.constBegin();
    for (; it != cards.cards.constEnd(); it++)
    {
        CardPoint point = it->point;
    }
}

void Cards::Add(const Card& card)
{
    cards.insert(card);
}

void Cards::Add(const QVector<Cards>& cardsArray)
{
    for (int i = 0; i < cardsArray.size(); i++)
    {
        Add(cardsArray[i]);
    }
}

Cards& Cards::operator <<(const Cards& cards)
{
    Add(cards);
    return *this;
}

Cards& Cards:: operator <<(const Card& card)
{
    Add(card);
    return *this;
}

Cards& Cards::operator <<(const QVector<Cards>& cardsArray)
{
    Add(cardsArray);
    return *this;
}

void Cards::Remove(const Cards& cards)
{
    this->cards.subtract(cards.cards);

    QSet<Card>::const_iterator it = cards.cards.constBegin();
    for (; it != cards.cards.constEnd(); it++)
    {
        CardPoint point = it->point;
    }
}

void Cards::Remove(const Card& card)
{
    cards.remove(card);
}

void Cards::Remove(const QVector<Cards>& cardsArray)
{
    for (int i = 0; i < cardsArray.size(); i++)
    {
        Remove(cardsArray[i]);
    }
}

bool Cards::Contains(const Cards& cards) const
{
    return this->cards.contains(cards.cards);
}

bool Cards::Contains(const Card& card) const
{
    return cards.contains(card);
}

Card Cards::TakeRandomCard()
{
    int randomIndex = qrand() % cards.size();
    QSet<Card>::iterator it = cards.begin() + randomIndex;
    Card takeCard = *it;
    cards.erase(it);

    return takeCard;
}

int Cards::Count()
{
    return cards.size();
}

int Cards::PointCount(CardPoint point)
{
    int count = 0;
    for (QSet<Card>::ConstIterator it = cards.constBegin(); it != cards.constEnd(); it++)
    {
        if (it->point == point)
        {
            count++;
        }
    }

    return count;
}

CardPoint Cards::MinPoint()
{
    CardPoint min = Card_End;
    if (cards.isEmpty()) return Card_End;

    QSet<Card>::ConstIterator it = cards.constBegin();
    for (; it != cards.constEnd(); it++)
    {
        if (it->point < min)
        {
            min = it->point;
        }
    }

    return min;
}

CardPoint Cards::MaxPoint()
{
    CardPoint max = Card_Begin;
    if (cards.isEmpty()) return Card_Begin;

    QSet<Card>::ConstIterator it = cards.constBegin();
    for (; it != cards.constEnd(); it++)
    {
        if (it->point > max)
        {
            max = it->point;
        }
    }

    return max;
}

CardList Cards::ToCardList(SortType sortType) const //排序
{
    CardList cardList;
    for (QSet<Card>::const_iterator it = cards.constBegin(); it != cards.constEnd(); it++)
    {
        cardList << *it;
    }

    if (sortType == Asc)
    {
        qSort(cardList.begin(), cardList.end(), qLess<Card>());
    }
    else if (sortType == Desc)
    {
        qSort(cardList.begin(), cardList.end(), qGreater<Card>());
    }

    return cardList;
}
