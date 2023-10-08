#pragma warning(disable: 4691)
using namespace System;
using namespace System::IO;
using namespace Parsec::Shaiya::SetItem;

int main(array<System::String^>^ args)
{
    Console::WriteLine("Please choose an option");
    Console::WriteLine("1. Extract");
    Console::WriteLine("2. Compile");
    auto option = Console::ReadKey().KeyChar;

    if (option == '1')
    {
        auto data = Parsec::Reader::ReadFromFile<SetItem^>("SetItem.SData");
        data->WriteJson("SetItem.json");
    }
    else
    {
        Console::WriteLine();
        Console::WriteLine("Please choose an option");
        Console::WriteLine("1. Encrypted");
        Console::WriteLine("2. Decrypted");
        option = Console::ReadKey().KeyChar;

        File::Copy("SetItem.SData", "SetItem.SData.bak", true);

        auto data = Parsec::Reader::ReadFromJsonFile<SetItem^>("SetItem.json");
        if (option == '1')
            data->WriteEncrypted("SetItem.SData");
        else
            data->WriteDecrypted("SetItem.SData");
    }

    return 0;
}
