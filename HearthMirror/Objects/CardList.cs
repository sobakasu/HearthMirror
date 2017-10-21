using System.Collections.Generic;

namespace HearthMirror.Objects
{
    public class CardList 
    {
        public List<Card> Cards { get; set; } = new List<Card>();

        public static CardList FromList(List<Card> cards)
        {
            CardList list = new CardList();
            list.Cards = cards;
            return list;
        }

        public Card GetItem(int index)
        {
            return Cards[index];
        }

        public int Count()
        {
            return Cards.Count;
        }

    }
}