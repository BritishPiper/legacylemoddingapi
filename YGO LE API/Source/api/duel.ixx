export module duel;

import basictypes;
import card;
import mask;

using namespace card;

export namespace duel {
    using UniqueID = Byte;

    enum class Duelist : Word { // Size: 0x2
        Player, // 0x0
        Opponent // 0x1
    };

    enum class Zone : Word { // Size: 0x2
        MainMonsterLeftmost, // 0x0
        MainMonsterCenterLeft, // 0x1
        MainMonsterCenter, // 0x2
        MainMonsterCenterRight, // 0x3
        MainMonsterRightmost, // 0x4
        ExtraMonsterLeft, // 0x5
        ExtraMonsterRight, // 0x6
        SpellTrapLeftmost, // 0x7
        PendulumLeft = SpellTrapLeftmost, // 0x7
        SpellTrapCenterLeft, // 0x8
        SpellTrapCenter, // 0x9
        SpellTrapCenterRight, // 0xA
        SpellTrapRightmost, // 0xB
        PendulumRight = SpellTrapRightmost, // 0xB
        Field, // 0xC
        Hand, // 0xD
        ExtraDeck, // 0xE
        Deck, // 0xF
        Graveyard, // 0x10
        Banished // 0x11
    };

#pragma pack(push, 1)
    struct Card { // Size: 0x4
        // Data1:
        // xx11 1111 1111 1111 - ID (bits = 14, offset = 0x0)
        // x1xx xxxx xxxx xxxx - Owner (bits = 1, offset = 0xE)
        // ?xxx xxxx xxxx xxxx - ? (bits = 1, offset = 0xF)
        // 
        // Data2:
        // xxxx xxxx x??? ???? - ? (bits = 7, offset = 0x0)
        // x111 1111 1xxx xxxx - ObjectID (bits = 8, offset = 7)
        // ?xxx xxxx xxxx xxxx - ? (bits = 1, offset = 0xF)

    private:
        Word Data1; // 0x0
        Word Data2; // 0x2

    public:
        CardID GetID(void)
        {
            return GetWithMask<CardID, 14, 0x0>(Data1);
        }

        void PutID(CardID id)
        {
            SetWithMask<14, 0x0>(Data1, id);
        }

        Duelist GetOwner(void)
        {
            return GetWithMask<Duelist, 1, 0xE>(Data1);
        }

        void PutOwner(Duelist owner)
        {
            SetEnumWithMask<1, 0xE>(Data1, owner);
        }

        UniqueID GetUniqueID(void)
        {
            return GetWithMask<duel::UniqueID, 8, 0x7>(Data2);
        }

        void PutUniqueID(UniqueID id)
        {
            SetWithMask<8, 0x7>(Data2, id);
        }

    public:
        __declspec(property(get = GetID, put = PutID)) CardID ID;
        __declspec(property(get = GetOwner, put = PutOwner)) Duelist Owner;
        __declspec(property(get = GetUniqueID, put = PutUniqueID)) UniqueID UniqueID;
    };

    struct FieldZone { // Size: 0x18
        Card Card; // 0x0
        Dword Unknown1____; // 0x4
        Word Unknown2____; // 0x8
        Word Unknown3____; // 0xA
        Dword Unknown4____; // 0xC
        Dword Unknown5____; // 0x10
        Dword Unknown6____; // 0x14
    };
#pragma pack(pop)
}
