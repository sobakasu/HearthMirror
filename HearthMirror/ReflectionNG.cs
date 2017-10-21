using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using HearthMirror.Objects;

namespace HearthMirror
{
    // non generic version of reflection
    public class ReflectionNG
    {
        public static CardList GetCollection()
        {
            return CardList.FromList(Reflection.GetCollection());
        }

        public static DeckList getDecks()
        {
            List<Deck> decks = Reflection.GetDecks();
            DeckList list = new DeckList();
            list.Decks = decks;
            return list;
        }

        public static CardList GetArenaDraftChoices()
        {
            return CardList.FromList(Reflection.GetArenaDraftChoices());
        }

    }
}
