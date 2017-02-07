using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace HearthMirrorCLRTest
{
    class Program
    {
        static void Main(string[] args)
        {
            var battleTag = HearthMirrorCLR.Reflection.GetBattleTag();

            Console.WriteLine(battleTag.Number);

            Console.ReadKey();
        }
    }
}
