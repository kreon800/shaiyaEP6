using System;
using System.IO;
using System.Text;
using Parsec;
using Parsec.Common;
using Parsec.Shaiya.Item;

namespace ItemTool
{
    internal class Program
    {
        static void Main(string[] args)
        {
            Console.WriteLine("Please choose an option");
            Console.WriteLine("1. Extract");
            Console.WriteLine("2. Compile");
            var option = Console.ReadKey().KeyChar;

            Console.WriteLine();
            Console.WriteLine("Please enter a code page identifier (e.g. 1252)");
            var codepage = Console.ReadLine();
            var encoding = CodePagesEncodingProvider.Instance.GetEncoding(int.Parse(codepage));

            if (encoding == null)
            {
                Console.WriteLine("Invalid encoding");
                return;
            }

            if (option == '1')
            {
                var data = ParsecReader.FromFile<Item>("Item.SData", Episode.EP6, encoding);
                data.WriteCsv("Item.csv", encoding);
            }
            else
            {
                Directory.CreateDirectory("output");
                var data = Item.FromCsv("Item.csv", Episode.EP6, encoding);
                data.WriteEncrypted("output/Item.SData");

                var item = ParsecReader.FromFile<Item>("output/Item.SData", Episode.EP6, encoding);
                WriteSql(item);
            }
        }

        static void WriteSql(Item item)
        {
            var stream = new StreamWriter("output/Item.sql");
            stream.WriteLine("TRUNCATE TABLE [PS_GameDefs].[dbo].[Items]");
            stream.WriteLine("GO");

            int lineCount = 0;
            foreach (var group in item.ItemGroups)
            {
                foreach (var i in group.ItemDefinitions)
                {
                    if (lineCount == 100)
                    {
                        stream.WriteLine("GO");
                        lineCount = 0;
                    }

                    int itemId = (i.ItemType * 1000) + i.ItemTypeId;

                    var line = string.Format(
                        "INSERT INTO [PS_GameDefs].[dbo].[Items] " +
                        "VALUES({0},'{1}',{2},{3},{4},{5},{6},{7},{8},{9},{10},{11},{12},{13},{14},{15},{16},{17},{18},{19},{20},{21},{22},{23},{24},{25},{26},{27},{28},{29},{30},{31},{32},{33},{34},{35},{36},{37},{38},{39},{40},{41},{42},{43},{44},{45},{46},{47},{48})",
                        itemId,
                        // escape single quotes
                        i.Name.Replace("'", "''"),
                        i.ItemType,
                        i.ItemTypeId,
                        (short)i.MinLevel,
                        i.Country,
                        i.AttackFighter,
                        i.DefenseFighter,
                        i.PatrolRogue,
                        i.ShootRogue,
                        i.AttackMage,
                        i.DefenseMage,
                        i.Grow,
                        (short)i.ReqStr,
                        (short)i.ReqDex,
                        (short)i.ReqRec,
                        (short)i.ReqInt,
                        (short)i.ReqWis,
                        (short)i.ReqLuc,
                        (short)i.ReqVg,
                        i.ReqOg,
                        i.ReqIg,
                        (short)i.Range,
                        i.AttackTime,
                        i.Attrib,
                        i.Special,
                        i.Slot,
                        (short)i.Quality,
                        (short)i.Attack,
                        (short)i.AttackAdd,
                        (short)i.Def,
                        (short)i.Resist,
                        (short)i.Hp,
                        (short)i.Sp,
                        (short)i.Mp,
                        (short)i.Str,
                        (short)i.Dex,
                        (short)i.Rec,
                        (short)i.Int,
                        (short)i.Wis,
                        (short)i.Luc,
                        i.Speed,
                        i.Exp,
                        (int)i.BuyPrice,
                        (int)i.SellPrice,
                        (short)i.Grade,
                        (short)i.Drop,
                        i.Server,
                        i.Count
                        );

                    stream.WriteLine(line);
                    ++lineCount;
                }
            }

            stream.Close();
        }
    }
}
