using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace HearthMirror.Objects
{
    public class Player
    {
        public Player(int id, string name, int standardRank, int standardLegendRank, int standardStars, int wildRank, int wildLegendRank, int wildStars, int cardBackId, AccountId accountId)
        {
            Id = id;
            Name = name;
            StandardRank = standardRank;
            StandardLegendRank = standardLegendRank;
            StandardStars = standardStars;
            WildRank = wildRank;
            WildLegendRank = wildLegendRank;
            WildStars = wildStars;
            CardBackId = cardBackId;
            AccountId = accountId;
        }

        public string Name { get; set; }
        public int Id { get; set; }
        public int StandardRank { get; set; }
        public int StandardLegendRank { get; set; }
        public int StandardStars { get; set; }
        public int WildRank { get; set; }
        public int WildLegendRank { get; set; }
        public int WildStars { get; set; }
        public int CardBackId { get; set; }
        public AccountId AccountId { get; set; }
    }
}
