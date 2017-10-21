using System.Collections.Generic;

namespace HearthMirror.Objects
{
    public class DeckList 
    {
        public List<Deck> Decks { get; set; } = new List<Deck>();

        public Deck GetItem(int index)
        {
            return Decks[index];
        }

        public int Count()
        {
            return Decks.Count;
        }

    }
}