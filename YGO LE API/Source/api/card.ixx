module;

#include <map>
#include <string>

#include "../globals.h"
#include "../util/assembly.h" // Hook
#include "../util/error.h"

export module card;

using namespace std;

import basictypes;
import log;
import mask; // BitSet, BitTest, SBits
import stream;

export namespace card {
    using Attack = Dword; // Size: 0x4
    using CardID = Word; // Size: 0x2
    using Defense = Dword; // Size: 0x4
    using Genre = Qword; // Size: 0x8
    using Level = Dword; // Size: 0x4
    using LongCardID = Dword; // Size: 0x4
    using Rating = Dword; // Size: 0x4
    using Rank = Dword; // Size: 0x4
    using Scale = Dword; // Size: 0x4
    using TrunkID = Word; // Size: 0x2

    constexpr Attack QuestionMarkAttack = 10 * 0x1FF;
    constexpr Defense QuestionMarkDefense = 10 * 0x1FF;

    constexpr CardID original_cards_maxloadid = 0x27B5;
    constexpr CardID original_cards_offseted_maxid = 0x2B3D;
    constexpr CardID original_cards_offset = 0xF3C;
    constexpr CardID original_cards_maxid = original_cards_offseted_maxid + original_cards_offset; // 0x3A79
    constexpr CardID new_card_limit = 0xFFFF;

    // loc - ([(id*2 + 2)*2] + 0x1A3*2 + 2)*2
    // size - (id*2 + 3)*2
    enum class Archetype : Word { // Size: 0x2
        Madolche = 0x86,
        WorldLegacy = 0x142,
        Orcust = 0x167,
        Adamancipator = 0x197
    };

    enum class Attribute : Dword { // Size: 0x4
        None, // 0x0
        Light, // 0x1
        Dark, // 0x2
        Water, // 0x3
        Fire, // 0x4
        Earth, // 0x5
        Wind, // 0x6
        Divine, // 0x7
        Spell, // 0x8
        Trap // 0x9
    };

    enum class Frame : Dword { // Size: 0x4
        Normal, // 0x0
        Effect, // 0x1
        Ritual, // 0x2
        Fusion, // 0x3
        DuelLinksGod, // 0x4
        DuelLinksDummy, // 0x5
        Spell = 0x7, // 0x7
        Trap, // 0x8
        Token, // 0x9
        Synchro, // 0xA
        DarkSynchro, // 0xB
        XYZ, // 0xC
        PendulumNormal, // 0xD
        PendulumEffect, // 0xE
        XYZPendulum, // 0xF
        SynchroPendulum, // 0x10
        FusionPendulum, // 0x11
        Link // 0x12
    };

    enum class Icon : Dword { // Size: 0x4
        Normal, // 0x0
        Counter, // 0x1
        Field, // 0x2
        Equip, // 0x3
        Continuous, // 0x4
        QuickPlay, // 0x5
        Ritual // 0x6
    };

    // Special kinds cannot be normal summoned
    enum class Kind : Dword { // Size: 0x4
        Normal, // 0x0
        Effect, // 0x1
        Fusion, // 0x2
        FusionEffect, // 0x3
        Ritual, // 0x4
        RitualEffect, // 0x5
        Toon, // 0x6
        Spirit, // 0x7
        Union, // 0x8
        Gemini, // 0x9
        Token, // 0xA
        //Duel Links -> God, // 0xB
        //Duel Links -> Dummy, // 0xC
        Spell = 0xD, // 0xD
        Trap, // 0xE
        Tuner, // 0xF
        TunerEffect, // 0x10
        Synchro, // 0x11
        SynchroEffect, // 0x12
        SynchroTuner, // 0x13
        DarkTuner, // 0x14
        DarkSynchro, // 0x15
        Xyz, // 0x16
        XyzEffect, // 0x17
        Flip, // 0x18
        Pendulum, // 0x19
        PendulumEffect, // 0x1A
        SpecialEffect, // 0x1B
        SpecialToon, // 0x1C
        SpecialSpirit, // 0x1D
        SpecialTuner, // 0x1E
        SpecialDarkTuner, // 0x1F
        FlipTuner, // 0x20
        PendulumTunerEffect, // 0x21
        XyzPendulum, // 0x22
        PendulumFlip, // 0x23
        SynchroPendulum, // 0x24
        UnionTuner, // 0x25
        RitualSpirit, // 0x26
        FusionTuner, // 0x27
        SpecialPendulum, // 0x28
        FusionPendulum, // 0x29
        Link, // 0x2A
        LinkEffect, // 0x2B
        PendulumTuner, // 0x2C
        PendulumSpirit // 0x2D
    };

    enum class Limitation : Dword { // Size: 0x4
        Forbidden, // 0x0
        Limited, // 0x1
        SemiLimited, // 0x2
        NotLimited // 0x3
    };

    enum class StarType : Dword { // Size: 0x4
        None, // 0x0
        Level, // 0x1
        Rank, // 0x2
        LinkRating // 0x3
    };

    enum class SubKind : Word { // Size: 0x4
        None, // 0x0
        NoEffect, // 0x1
        Normal, // 0x2
        Effect, // 0x3
        Flip, // 0x4
        Toon, // 0x5
        Spirit, // 0x6
        Union, // 0x7
        Gemini // 0x8
    };

    enum class Type : Dword { // Size: 0x4
        QuestionMark, // 0x0
        Dragon, // 0x1
        Zombie, // 0x2
        Fiend, // 0x3
        Pyro, // 0x4
        SeaSerpent, // 0x5
        Rock, // 0x6
        Machine, // 0x7
        Fish, // 0x8
        Dinosaur, // 0x9
        Insect, // 0xA
        Beast, // 0xB
        BeastWarrior, // 0xC
        Plant, // 0xD
        Aqua, // 0xE
        Warrior, // 0xF
        WingedBeast, // 0x10
        Fairy, // 0x11
        Spellcaster, // 0x12
        Thunder, // 0x13
        Reptile, // 0x14
        Psychic, // 0x15
        Wyrm, // 0x16
        Cyberse, // 0x17
        DivineBeast, // 0x18
        CreatorGod, // 0x19
        Spell = 0x1E, // 0x1E
        Trap, // 0x1F
    };

#pragma pack(push, 1)
    struct Arrows { // Size: 0x4
        // Data:
        // xxxx xxx1 - Top Left (bits = 1, offset = 0x0)
        // xxxx xx1x - Top (bits = 1, offset = 0x1)
        // xxxx x1xx - Top Right (bits = 1, offset = 0x2)
        // xxxx 1xxx - Left (bits = 1, offset = 0x3)
        // xxx1 xxxx - Right (bits = 1, offset = 0x4)
        // xx1x xxxx - Bottom Left (bits = 1, offset = 0x5)
        // x1xx xxxx - Bottom (bits = 1, offset = 0x6)
        // 1xxx xxxx - Bottom Right (bits = 1, offset = 0x7)
        // 1111 1111 1111 1111 1111 1111 xxxx xxxx - Unused Bits (bits = 24, offset = 0x8)

        enum class Direction : char {
            TopLeft,
            Top,
            TopRight,
            Left,
            Right,
            BottomLeft,
            Bottom,
            BottomRight
        };

    private:
        Dword Data; // 0x0

        template <Direction direction, typename Flags>
        void ArrowSet(Flags& flags, bool value)
        {
            BitSet<static_cast<SBits>(direction)>(flags, value);
        }

        template <Direction direction>
        bool ArrowTest(const Dword& flags)
        {
            return BitTest<static_cast<SBits>(direction)>(flags);
        }

    public:

        Arrows(Dword data) : Data(data) {}

        operator Dword(void) const
        {
            return Data;
        }

        bool GetTopLeft(void)
        {
            return ArrowTest<Direction::TopLeft>(Data);
        }

        void PutTopLeft(bool flag)
        {
            ArrowSet<Direction::TopLeft>(Data, flag);
        }

        bool GetTop(void)
        {
            return ArrowTest<Direction::Top>(Data);
        }

        void PutTop(bool flag)
        {
            ArrowSet<Direction::Top>(Data, flag);
        }

        bool GetTopRight(void)
        {
            return ArrowTest<Direction::TopRight>(Data);
        }

        void PutTopRight(bool flag)
        {
            ArrowSet<Direction::TopRight>(Data, flag);
        }

        bool GetLeft(void)
        {
            return ArrowTest<Direction::Left>(Data);
        }

        void PutLeft(bool flag)
        {
            ArrowSet<Direction::Left>(Data, flag);
        }

        bool GetRight(void)
        {
            return ArrowTest<Direction::Right>(Data);
        }

        void PutRight(bool flag)
        {
            ArrowSet<Direction::Right>(Data, flag);
        }

        bool GetBottomLeft(void)
        {
            return ArrowTest<Direction::BottomLeft>(Data);
        }

        void PutBottomLeft(bool flag)
        {
            ArrowSet<Direction::BottomLeft>(Data, flag);
        }

        bool GetBottom(void)
        {
            return ArrowTest<Direction::Bottom>(Data);
        }

        void PutBottom(bool flag)
        {
            ArrowSet<Direction::Bottom>(Data, flag);
        }

        bool GetBottomRight(void)
        {
            return ArrowTest<Direction::BottomRight>(Data);
        }

        void PutBottomRight(bool flag)
        {
            ArrowSet<Direction::BottomRight>(Data, flag);
        }

        __declspec(property(get = GetTopLeft, put = PutTopLeft)) bool TopLeft;
        __declspec(property(get = GetTop, put = PutTop)) bool Top;
        __declspec(property(get = GetTopRight, put = PutTopRight)) bool TopRight;
        __declspec(property(get = GetLeft, put = PutLeft)) bool Left;
        __declspec(property(get = GetRight, put = PutRight)) bool Right;
        __declspec(property(get = GetBottomLeft, put = PutBottomLeft)) bool BottomLeft;
        __declspec(property(get = GetBottom, put = PutBottom)) bool Bottom;
        __declspec(property(get = GetBottomRight, put = PutBottomRight)) bool BottomRight;
    };

    struct BritishCard { // Size: 0x30
        CardID ID; // 0x0
        bool IsAttackQuestionMark; // 0x2
        bool IsDefenseQuestionMark; // 0x3
        Attack Attack; // 0x4
        Defense DefenseOrArrows; // 0x8
        Kind Kind; // 0xC
        Type Type; // 0x10
        Attribute Attribute; // 0x14
        Dword Stars; // 0x18
        Icon Icon; // 0x1C
        Limitation Limitation; // 0x20
        Scale PendulumScale; // 0x24
        StarType StarType; // 0x28
        Word NameSize; // 0x2C
        Word DescriptionSize; // 0x2E
    };

    struct Card { // Size: 0xA0
        bool ExistsOnDuel; // 0x0
        Undefined1 Unknown1____[7]; // 0x1
        WString Name; // 0x8
        WString Description; // 0x10
        TrunkID TrunkID; // 0x18
        Undefined2 Unknown3____; // 0x1A
        bool IsMonster; // 0x1C
        bool IsSpell; // 0x1D
        bool IsTrap; // 0x1E
        bool IsFieldSpell; // 0x1F
        bool IsNormalMonster; // 0x20
        bool IsEffectMonster; // 0x21
        bool IsFusion; // 0x22
        bool IsSynchro; // 0x23
        Undefined1 Unknown22____; // 0x24
        bool IsXyz; // 0x25
        bool IsExtraMonster; // 0x26
        bool IsRitual; // 0x27
        bool IsToken; // 0x28
        Undefined1 Unknown27____; // 0x29
        bool IsToon; // 0x2A
        bool IsSpirit; // 0x2B
        bool IsGemini; // 0x2C
        Undefined1 Unknown31____; // 0x2D
        Undefined1 Unknown32____; // 0x2E
        bool IsPendulum; // 0x2F
        Undefined1 Unknown34____; // 0x30
        bool IsLink; // 0x31
        bool UnusedFlags[6]; // 0x32
        Genre Genre; // 0x38
        Dword SoundPoolName; // 0x40 (always starts at 0)

    private:
        Attack Attack1; // 0x44
        Attack Attack2; // 0x48

    public:
        Attribute Attribute; // 0x4C

    private:
        Defense Defense1; // 0x50
        Defense Defense2; // 0x54

    public:
        Icon Icon; // 0x58
        Kind Kind; // 0x5C
        Level Level; // 0x60
        Limitation Limitation; // 0x64

    private:
        LongCardID ID1; // 0x68
        LongCardID ID2; // 0x6C
        LongCardID ID3; // 0x70

    public:
        Rank Rank; // 0x74

    private:
        Scale LeftPendulumScale; // 0x78
        Scale RightPendulumScale; // 0x7C

    public:
        Dword LevelOrLinkRatingOrRank; // 0x80
        Type Type; // 0x84
        Rating LinkRating; // 0x88
        Arrows LinkArrows; // 0x8C
        Bool Valid; // 0x90
        Frame Frame; // 0x94
        Undefined4 Unknown38____; // 0x98
        Undefined4 UnknownType3____; // 0x9C

        Attack GetAttack(void) const
        {
            return Attack2;
        }

        void PutAttack(Attack attack)
        {
            if (attack == QuestionMarkAttack) {
                Attack1 = 0;
                Attack2 = 0xFFFF;
            }
            else {
                Attack1 = Attack2 = attack;
            }
        }

        Defense GetDefense(void) const
        {
            return Defense2;
        }

        void PutDefense(Defense defense)
        {
            if (defense == QuestionMarkDefense) {
                Defense1 = 0;
                Defense2 = 0xFFFF;
            }
            else {
                Defense1 = Defense2 = defense;
            }
        }

        CardID GetID(void) const
        {
            return static_cast<CardID>(ID1);
        }

        void PutID(CardID id)
        {
            ID1 = ID2 = ID3 = id;
        }

        Scale GetPendulumScale(void) const
        {
            return LeftPendulumScale;
        }

        void PutPendulumScale(Scale scale)
        {
            LeftPendulumScale = RightPendulumScale = scale;
        }

        __declspec(property(get = GetAttack, put = PutAttack)) Attack Attack;
        __declspec(property(get = GetDefense, put = PutDefense)) Defense Defense;
        __declspec(property(get = GetID, put = PutID)) CardID ID;
        __declspec(property(get = GetPendulumScale, put = PutPendulumScale)) Scale PendulumScale;
    };

    // Used in the CardProps.bin file
    struct CardProperties { // Size: 0x8
    private:
        Qword Data;
    };

    // Used immediately after reading CardProps.bin
    struct CardData { // Size: 0x30
    public:
        LongCardID ID1; // 0x0

    private:
        Attack Attack10; // 0x4
        Dword ArrowsOrDefense10; // 0x8

    public:
        Kind Kind; // 0xC
        Type Type; // 0x10
        Attribute Attribute; // 0x14
        Dword LevelOrLinkRatingOrRank; // 0x18
        Icon Icon; // 0x1C
        Limitation Limitation; // 0x20
        Scale PendulumScale; // 0x24
        StarType StarType; // 0x28

    private:
        CardID ID2; // 0x2C (Used in CARD_IsThisSameCard)
        CardID ID3; // 0x2E

    public:
        CardData(void) {}

        CardData(CardProperties properties) {}

        CardData(BritishCard card)
        {
            ID = card.ID;
            Attack = card.IsAttackQuestionMark ? QuestionMarkAttack : card.Attack;

            if (card.Kind == Kind::Link || card.Kind == Kind::LinkEffect)
            {
                ArrowsOrDefense10 = card.DefenseOrArrows;
            }
            else
            {
                ArrowsOrDefense10 = (card.IsDefenseQuestionMark ? QuestionMarkDefense : card.DefenseOrArrows) / 10;
            }

            Kind = card.Kind;
            Type = card.Type;
            Attribute = card.Attribute;
            LevelOrLinkRatingOrRank = card.Stars;
            Icon = card.Icon;
            Limitation = card.Limitation;
            PendulumScale = card.PendulumScale;
            StarType = card.StarType;
        }

        Attack GetAttack(void) const
        {
            return Attack10 * 10;
        }

        void PutAttack(Attack attack)
        {
            Attack10 = attack / 10;
        }

        Defense GetDefense(void) const
        {
            return StarType != StarType::LinkRating ? ArrowsOrDefense10 * 10 : 0;
        }

        void PutDefense(Defense defense)
        {
            ArrowsOrDefense10 = defense / 10;
        }

        Frame GetFrame(void) const
        {
            return (card::Frame)(*(short*)(0x140BF7820 + 0xC * (Dword)Kind));
        }

        CardID GetID(void) const
        {
            return static_cast<CardID>(ID1);
        }

        void PutID(CardID id)
        {
            ID1 = ID2 = ID3 = id;
        }

        Level GetLevel(void) const
        {
            return StarType == StarType::Level ? LevelOrLinkRatingOrRank : 0;
        }

        void PutLevel(Level level)
        {
            LevelOrLinkRatingOrRank = level;
        }

        Arrows GetLinkArrows(void) const
        {
            return StarType == StarType::LinkRating ? ArrowsOrDefense10 : 0;
        }

        void PutLinkArrows(Arrows arrows)
        {
            ArrowsOrDefense10 = arrows;
        }

        Rating GetLinkRating(void) const
        {
            return StarType == StarType::LinkRating ? LevelOrLinkRatingOrRank : 0;
        }

        void PutLinkRating(Rating rating)
        {
            LevelOrLinkRatingOrRank = rating;
        }

        Rank GetRank(void) const
        {
            return StarType == StarType::Rank ? LevelOrLinkRatingOrRank : 0;
        }

        void PutRank(Rank rank)
        {
            LevelOrLinkRatingOrRank = rank;
        }

        __declspec(property(get = GetAttack, put = PutAttack)) Attack Attack;
        __declspec(property(get = GetDefense, put = PutDefense)) Defense Defense;
        __declspec(property(get = GetFrame)) Frame Frame;
        __declspec(property(get = GetID, put = PutID)) CardID ID;
        __declspec(property(get = GetLevel, put = PutLevel)) Level Level;
        __declspec(property(get = GetLinkArrows, put = PutLinkArrows)) Arrows LinkArrows;
        __declspec(property(get = GetLinkRating, put = PutLinkRating)) Rating LinkRating;
        __declspec(property(get = GetRank, put = PutRank)) Rank Rank;
    };

    struct OldCardIllustrations // Size: 0x1B090
    {
        Qword Unknown0____; // 0x0
        Qword Unknown1____[4]; // 0x8
        Qword Pointers[original_cards_offseted_maxid]; // 0x28 = -0x79b8 + 0xF3C * 8
        Word Ids[original_cards_offseted_maxid]; // 0x15A10 = 0x13B98 + 0xF3C * 2
        Undefined1 Unknown4____[6]; // 0x1B08A
    };

    struct NewCardIllustrations // Size: 0xA0024
    {
        Qword Unknown0____; // 0x0
        Qword Unknown1____[4]; // 0x8
        Qword Pointers[new_card_limit]; // 0x28
        Word Ids[new_card_limit]; // 0x80020
        Undefined1 Unknown4____[6]; // 0xA001E
    };

    struct CardImageData // Size: 0x8
    {
        Dword Unknown0____; // 0x0
        Dword Unknown1____; // 0x4
    };

    struct OldCardImages // Size: 0x13F60
    { // FUN_1407521e0
        Undefined1 Unknown4____[0xC0]; // 0x0
        CardImageData Data[original_cards_maxloadid + 1]; // 0xC0
        // 0x13E70
    };
#pragma pack(pop)

    struct NewCard {
        CardData Data;
        wstring Name;
        wstring Description;

        NewCard(CardData data, wstring name, wstring description) : Data(data), Name(name), Description(description) {}

        NewCard(BritishCard data, wstring name, wstring description) : Data(data), Name(name), Description(description) {}
    };

    Hook CheckCardNameHook = Hook(0x14076CFF0);

    Hook CreateCardPropsHook = Hook(0x14081A080);

    Hook GetCardDataHook = Hook(0x1407CAB30);

    Hook GetCardImageHook = Hook(0x140753BA0);

    Hook GetCardPropertiesHook = Hook(0x1407CAB00);

    Hook GetLoadIDHook = Hook(0x14076E000);

    Hook ReadCardBinHook = Hook(0x14076C3B0);

    Hook SetupImageTableHook = Hook(0x1407521E0);

    Hook UnlockedTrunkCardsHook1 = Hook(0x1408BFF37);

    Hook WriteLoadIDTableHook = Hook(0x14076C0A1);

    // check for El Shaddoll Apkallone (maybe I skipped him when copying load/card data)
    // 00000001407F815C -> bp to GetCardList (max offset is 0x4e20)
    // 00000001408C0067 -> loading cards in card edit

    Patch CheckCardNamePatch = Patch(0x14076CFF0, "\x48\xB8\xA0\xD3\xC8\x40\x01\x00\x00\x00\x48\x8B\x00\x4C\x8B\x88\xC8\x00\x00\x00\x45\x0F\xBF\x11\x4C\x8B\xD9\x90\x90\x90"); // Allows CheckCardNameHook

    Patch DuelPatch1 = Patch(0x1407BAF98, "\x90\x90\x66\x41\x3B\xC2\x90\x90"); // Card in hand
    Patch DuelPatch2 = Patch(0x140789836, "\x83\xE8\x01\x90\x90\x3D\xFD\xFF\x00\x00"); // Open card details
    //Patch DuelPatch2 = Patch(0x140789840, "\x90\x90"); // Old open card details
    Patch DuelPatch3 = Patch(0x140778180, "\x3D\xC2\xF0\x00\x00"); // Card click
    //Patch DuelPatch3 = Patch(0x140778185, "\x90\x90"); // Old card click
    Patch DuelPatch4 = Patch(0x14088676D, "\x41\xBE\x01\x00\x00\x00\x0F\xB7\x87\x5E\x04\x00\x00\x66\x41\x2B\xC6\x41\xBF\xFD\xFF\x00\x00"); // On mouse over

    Patch GetIllustrationPatch1 = Patch(0x14086D057, "\x41\x8D\x02\x90\x90\x90\x90\x3D\xFF\xFF\x00\x00");
    Patch GetIllustrationPatch2 = Patch(0x14086D073, "\x46\x0F\xB6\x8C\x50\x20\x00\x08\x00\x4B\x8D\x04\x91\x0F\xBF\x54\x41\x28\x90\x90\x90");

    Patch LoadIllustrationsPatch1 = Patch(0x1407523A9, "\xB9\x24\x00\x0A\x00");
    Patch LoadIllustrationsPatch2 = Patch(0x14086CDA5, "\x48\x8D\x97\x21\x00\x08\x00");
    Patch LoadIllustrationsPatch3 = Patch(0x14086CDD3, "\x81\xF9\xFF\xFF\x00\x00");
    Patch LoadIllustrationsPatch4 = Patch(0x14086CEE7, "\x90\x90\x90\x90\x90\x3D\xFF\xFF\x00\x00");
    Patch LoadIllustrationsPatch5 = Patch(0x14086CEFF, "\x0F\xB6\x84\x4F\x20\x00\x08\x00\x0F\xAB\xF0\x88\x84\x4F\x20\x00\x08\x00");
    Patch LoadIllustrationsPatch6 = Patch(0x14086CF78, "\x48\x63\xF3\x0F\xB6\xAC\x77\x20\x00\x08\x00");
    Patch LoadIllustrationsPatch7 = Patch(0x14086CFA4, "\xC6\x84\x77\x20\x00\x08\x00\x03");
    Patch LoadIllustrationsPatch8 = Patch(0x14086CFC5, "\xC6\x84\x77\x20\x00\x08\x00\x02");
    Patch LoadIllustrationsPatch9 = Patch(0x14086CFD4, "\xC6\x84\x77\x20\x00\x08\x00\x01");
    Patch LoadIllustrationsPatch10 = Patch(0x14086CFE3, "\xC6\x84\x77\x20\x00\x08\x00\x00");
    Patch LoadIllustrationsPatch11 = Patch(0x14086CFF0, "\xC6\x84\x77\x20\x00\x08\x00\x01");
    Patch LoadIllustrationsPatch12 = Patch(0x14086CFFF, "\xC6\x84\x77\x20\x00\x08\x00\x00");
    Patch LoadIllustrationsPatch13 = Patch(0x14086D00A, "\x41\x81\xFE\xFF\xFF\x00\x00");
    Patch LoadIllustrationsPatch14 = Patch(0x14086CF70, "\x33\xDB\x90\x90\x90");

    Patch LoadImagePatch1 = Patch(0x140752D05, "\x90\x90\x90\x90\x90\x90\x4C\x8B\x3D\x72\x51\x0F\x02\x4D\x8D\x3C\xC7");

    // This function gets the card image given an ID
    Patch LoadImagePatch2 = Patch(0x140752A31, "\x8B\xC0\x48\x8B\x0D\x4A\x54\x0F\x02\x48\x83\xC1\x04\x90\x90\x90");

    Patch LoadImagePatch3 = Patch(0x140752CE4, "\x90\x90\x90\x90\x90\x90\xB8\x78\x3A\x00\x00\x66\x3B\xE8\x90\x90\x90\x90\x90\x90");
    Patch LoadImagePatch4 = Patch(0x140752427, "\x90\x90\xE8\xD2\xBB\x01\x00\x3D\xB6\x27\x00\x00\x48\x8B\x0D\x4A\x5A\x0F\x02\x90\x48\x8D\x0C\xC1");
    Patch LoadImagePatch5 = Patch(0x140754197, "\x90\x90\x90\x90\x90\x90\x4C\x8B\x35\xE0\x3C\x0F\x02\x4D\x8D\x34\xC6");

    Patch SetupTrunkPatch1 = Patch(0x1407F9B66, "\xB8\x30\x10\x01\x00");
    Patch SetupTrunkPatch2 = Patch(0x1407F9D14, "\x48\x81\xC4\x30\x10\x01\x00");
    Patch SetupTrunkPatch3 = Patch(0x1407F9B7D, "\x48\x89\x84\x24\x00\x10\x01\x00");
    Patch SetupTrunkPatch4 = Patch(0x1407F9BA7, "\x48\x89\xAC\x24\x68\x10\x01\x00\x48\x8D\x4C\x24\x40\x48\x89\xB4\x24\x28\x10\x01\x00\x33\xD2\x48\x89\xBC\x24\x20\x10\x01\x00\x41\xB8\xFF\xFF\x00\x00\x4C\x89\xB4\x24\x18\x10\x01\x00\x4C\x89\xBC\x24\x10\x10\x01\x00");
    Patch SetupTrunkPatch5 = Patch(0x1407F9CB6, "\x81\xFD\xFF\xFF\x00\x00");
    Patch SetupTrunkPatch6 = Patch(0x1407F9CC3, "\x4C\x8B\xBC\x24\x10\x10\x01\x00\x4C\x8B\xB4\x24\x18\x10\x01\x00\x48\x8B\xBC\x24\x20\x10\x01\x00\x0F\xB7\x08\xB8\x28\x00\x00\x00\x48\x8B\xB4\x24\x28\x10\x01\x00\x48\x8B\xAC\x24\x68\x10\x01\x00");
    Patch SetupTrunkPatch7 = Patch(0x1407F9D04, "\x48\x8B\x8C\x24\x00\x10\x01\x00");
    Patch SetupTrunkPatch8 = Patch(0x1407F9D14, "\x48\x81\xC4\x30\x10\x01\x00");

    Patch MoveCardFromTrunkPatch1 = Patch(0x1408363A1, "\x90\x90\x90\x90\x90\x90");
    Patch MoveCardFromTrunkPatch2 = Patch(0x140834393, "\x90\x90\x90\x90\x90\x90");

    Patch OpenCardDetailsPatch1 = Patch(0x140833BF8, "\x90\x90\xB8\x78\x3A\x00\x00\x66\x3B\xD0\x90\x90");
    Patch OpenCardDetailsPatch2 = Patch(0x1407CA592, "\x90\x90\x90\x90\x90\x90\xB8\x78\x3A\x00\x00\x66\x3B\xD8\x90\x90\x90\x90\x90\x90");
    Patch OpenCardDetailsPatch3 = Patch(0x14088962E, "\x90\x90\x66\x3B\xD8\x90\x90");
    Patch OpenCardDetailsPatch4 = Patch(0x140889797, "\x90\x90\x66\x45\x3B\xFD\x90\x90");

    Patch SetupImageTablePatch = Patch(0x14075228A, "\x48\xA1\x84\x7E\x84\x42\x01\x00\x00\x00\xBA\xFF\xFF\x00\x00\x90\x90\x90\x90\x90\x90\x90");

    // Need to make hooks for both of these
    //Patch TrunkDeckCardsPatch1 = Patch(0x14075618E, "\x90\x90"); // Get the current number of cards in the deck
    //Patch TrunkDeckCardsPatch2 = Patch(0x140755F51, "\x"); // Increase the number of cards in the trunk deck

    Patch UnlockedTrunkCardsPatch1 = Patch(0x1408C0061, "\x81\xFB\xFF\xFF\x00\x00"); // Load the current unlocked cards to show on trunk
    Patch UnlockedTrunkCardsPatch2 = Patch(0x1408C0025, "\x42\x89\x14\x80\x90\x90\x90\x90");
    Patch UnlockedTrunkCardsPatch3 = Patch(0x1408BFF49, "\xBB\xFF\xFF\x00\x00");
    Patch UnlockedTrunkCardsPatch4 = Patch(0x1408BEFCD, "\x90\x90"); // Get the current unlocked cards to show on trunk
    Patch UnlockedTrunkCardsPatch5 = Patch(0x1408BEFD9, "\x4C\x63\x04\x90\x90\x90\x90\x90");

    //[+2700+30]
    class Cards {
    public:
        static void Setup(void)
        {
            ApplyPatches();
            Logger::Log("Cards patches applied.");

            ApplyHooks();
            Logger::Log("Cards hooks applied.");

            ReadExtraCards();
            Logger::Log("Extra cards loaded.");
        }

    private:
        static map<CardID, NewCard> cards;

        static CardData card_datas[new_card_limit];
        static CardData card_datas_by_id[new_card_limit];
        static CardImageData card_images[new_card_limit];
        static CardID load_ids[new_card_limit];

        static void ApplyHooks(void)
        {
            CheckCardNameHook.Apply(&CheckCardName_);
            CreateCardPropsHook.Apply(&CreateCardProps_);
            GetCardDataHook.Apply(&GetCardData_);
            GetCardImageHook.Apply(&GetCardImage_);
            GetCardPropertiesHook.Apply(&GetCardProperties_);
            GetLoadIDHook.Apply(&GetLoadID_);
            ReadCardBinHook.Apply(&ReadCardBin_);
            SetupImageTableHook.Apply(&SetupImageTable_);
            UnlockedTrunkCardsHook1.Apply(&UnlockedTrunkCards1_);
            WriteLoadIDTableHook.Apply(&WriteLoadIDTable_);
        }

        static void ApplyPatches(void)
        {
            CheckCardNamePatch.Apply();

            DuelPatch1.Apply();
            DuelPatch2.Apply();
            DuelPatch3.Apply();
            DuelPatch4.Apply();

            GetIllustrationPatch1.Apply();
            GetIllustrationPatch2.Apply();

            LoadIllustrationsPatch1.Apply();
            LoadIllustrationsPatch2.Apply();
            LoadIllustrationsPatch3.Apply();
            LoadIllustrationsPatch4.Apply();
            LoadIllustrationsPatch5.Apply();
            LoadIllustrationsPatch6.Apply();
            LoadIllustrationsPatch7.Apply();
            LoadIllustrationsPatch8.Apply();
            LoadIllustrationsPatch9.Apply();
            LoadIllustrationsPatch10.Apply();
            LoadIllustrationsPatch11.Apply();
            LoadIllustrationsPatch12.Apply();
            LoadIllustrationsPatch13.Apply();
            LoadIllustrationsPatch14.Apply();

            LoadImagePatch1.Apply();
            LoadImagePatch2.Apply();
            LoadImagePatch3.Apply();
            LoadImagePatch4.Apply();
            LoadImagePatch5.Apply();

            MoveCardFromTrunkPatch1.Apply();
            MoveCardFromTrunkPatch2.Apply();

            OpenCardDetailsPatch1.Apply();
            OpenCardDetailsPatch2.Apply();
            OpenCardDetailsPatch3.Apply();
            OpenCardDetailsPatch4.Apply();

            SetupImageTablePatch.Apply();

            SetupTrunkPatch1.Apply();
            SetupTrunkPatch2.Apply();
            SetupTrunkPatch3.Apply();
            SetupTrunkPatch4.Apply();
            SetupTrunkPatch5.Apply();
            SetupTrunkPatch6.Apply();
            SetupTrunkPatch7.Apply();
            SetupTrunkPatch8.Apply();

            UnlockedTrunkCardsPatch1.Apply();
            UnlockedTrunkCardsPatch2.Apply();
            UnlockedTrunkCardsPatch3.Apply();
            UnlockedTrunkCardsPatch4.Apply();
            UnlockedTrunkCardsPatch5.Apply();
            //UnlockAllCards3.Apply();
        }

        static void ReadExtraCards(void)
        {
            ByteStream extracards(string(data_folder) + "extracards.british");

            if (!extracards) {
                Logger::Show(Warning("Skipping extra cards because the file \"extracards.british\" is missing."));
                return;
            }

            Word version = extracards.Read<Word>();

            if (version != 0x100)
            {
                Logger::Show(Warning("Skipping extra cards because \"extracards.british\" version {} is not supported."));

                return;
            }

            Word n_cards = extracards.Read<Word>();

            for (Word index = 0; index < n_cards; ++index) {
                BritishCard card = extracards.Read<BritishCard>();

                wchar_t* name = extracards.ReadFixedString<wchar_t>(card.NameSize);
                wchar_t* description = extracards.ReadFixedString<wchar_t>(card.DescriptionSize);

                cards.insert({ card.ID, NewCard(card, name, description) });
            }
        }

        static SubKind GetSubKind(Kind kind)
        {
            return (SubKind)(*(Word*)(0x140BF7824 + (Dword)kind * 12));
        }

        // Hooks

        __declspec(noinline) static bool CheckCardName_(CardID id, Archetype archetype)
        {
            if (cards.contains(id))
            {
                NewCard card = cards.at(id);

                switch (archetype)
                {
                case Archetype::Madolche:
                    return card.Name.contains(L"Madolche");
                case Archetype::Adamancipator:
                    return card.Name.contains(L"Adamancipator");
                default:
                    return false;
                }
            }
            else
            {
                return CheckCardNameHook.CallOriginal(&CheckCardName_)(id, archetype);
            }
        }

        __declspec(noinline) static void CreateCardProps_(void)
        {
            // Load the game cards
            CreateCardPropsHook.CallOriginal(&CreateCardProps_)();

            // Add our own cards
            CardID load_id = original_cards_maxloadid;

            for (auto const& [id, card] : cards)
            {
                load_ids[id] = ++load_id;
            }

            // Add our own cards' names and descriptions
            Card* game_cards = (Card*)0x142927600;
            for (auto const& [id, card] : cards)
            {
                Card* game_card = &game_cards[id];

                StarType star_type = card.Data.StarType;
                Frame frame = card.Data.Frame;
                Icon icon = card.Data.Icon;
                Kind kind = card.Data.Kind;
                SubKind subkind = GetSubKind(kind);

                game_card->Name = card.Name.c_str();
                game_card->Description = card.Description.c_str();
                game_card->IsMonster = subkind != SubKind::None;
                game_card->IsSpell = kind == Kind::Spell;
                game_card->IsFieldSpell = icon == Icon::Field;
                game_card->IsTrap = kind == Kind::Trap;
                game_card->IsNormalMonster = subkind == SubKind::Normal;
                game_card->IsEffectMonster = subkind > SubKind::Normal;
                game_card->IsFusion = frame == Frame::Fusion;
                game_card->IsSynchro = frame == Frame::Synchro;
                game_card->IsXyz = frame == Frame::XYZ;
                game_card->IsExtraMonster =
                    kind == Kind::Fusion ||
                    kind == Kind::FusionEffect ||
                    kind == Kind::Synchro ||
                    kind == Kind::SynchroEffect ||
                    kind == Kind::SynchroTuner ||
                    kind == Kind::DarkSynchro ||
                    kind == Kind::Xyz ||
                    kind == Kind::XyzEffect ||
                    kind == Kind::XyzPendulum ||
                    kind == Kind::SynchroPendulum ||
                    kind == Kind::FusionTuner ||
                    kind == Kind::FusionPendulum ||
                    kind == Kind::Link ||
                    kind == Kind::LinkEffect;
                game_card->IsRitual = frame == Frame::Ritual;
                game_card->IsToken = kind == Kind::Token;
                game_card->IsToon = subkind == SubKind::Toon;
                game_card->IsSpirit = subkind == SubKind::Spirit;
                game_card->IsGemini = subkind == SubKind::Gemini;
                game_card->IsPendulum =
                    frame == Frame::PendulumNormal ||
                    frame == Frame::PendulumEffect ||
                    frame == Frame::XYZPendulum ||
                    frame == Frame::SynchroPendulum ||
                    frame == Frame::FusionPendulum;
                game_card->IsLink = frame == Frame::Link;
                game_card->Attack = card.Data.Attack;
                game_card->Attribute = card.Data.Attribute;
                game_card->Defense = (kind == Kind::Link || kind == Kind::LinkEffect) ? QuestionMarkDefense : card.Data.Defense;
                game_card->Icon = icon;
                game_card->Kind = kind;
                game_card->Level = card.Data.Level;
                game_card->Limitation = card.Data.Limitation;
                game_card->ID = card.Data.ID;
                game_card->Rank = card.Data.Rank;
                game_card->PendulumScale = card.Data.PendulumScale;
                game_card->LevelOrLinkRatingOrRank = card.Data.LevelOrLinkRatingOrRank;
                game_card->Type = card.Data.Type;
                game_card->LinkRating = card.Data.LinkRating;
                game_card->LinkArrows = card.Data.LinkArrows;
                game_card->Frame = frame;
                game_card->Valid = 1;
                /*
                Undefined8 Unknown1____; // 0x0
        Undefined2 Unknown2____; // 0x18
        Undefined2 Unknown3____; // 0x1A
        Undefined1 Unknown14____; // 0x1C
        Undefined1 Unknown15____; // 0x1D
        Undefined1 Unknown16____; // 0x1E
        Undefined1 Unknown17____; // 0x1F
        Undefined1 Unknown18____; // 0x20
        Undefined1 Unknown19____; // 0x21
        Undefined1 Unknown20____; // 0x22
        Undefined1 Unknown21____; // 0x23
        Undefined1 Unknown22____; // 0x24
        Undefined1 Unknown23____; // 0x25
        Undefined1 Unknown24____; // 0x26
        Undefined1 Unknown25____; // 0x27
        Undefined1 Unknown26____; // 0x28
        Undefined1 Unknown27____; // 0x29
        Undefined1 Unknown28____; // 0x2A
        Undefined1 Unknown29____; // 0x2B
        Undefined1 Unknown30____; // 0x2C
        Undefined1 Unknown31____; // 0x2D
        Undefined1 Unknown32____; // 0x2E
        Undefined1 Unknown33____; // 0x2F
        Undefined1 Unknown34____; // 0x30
        Undefined1 Unknown35____; // 0x31 Link?
        Undefined2 UnknownType1____; // 0x32
        Undefined4 UnknownType2____; // 0x34
        Undefined8 Unknown36____; // 0x38
        Undefined4 Unknown4____; // 0x40
        Undefined4 Unknown38____; // 0x98
        Undefined4 UnknownType3____; // 0x9C
                */
            }
        }

        __declspec(noinline) static CardData* GetCardData_(CardID load_id)
        {
            return &card_datas[load_id];
        }

        __declspec(noinline) static CardImageData* GetCardImage_(CardImageData* original_table, CardID id)
        {
            return &card_images[load_ids[id]];
        }

        __declspec(noinline) static CardData* GetCardProperties_(CardID id)
        {
            return &card_datas_by_id[id];
        }

        __declspec(noinline) static CardID GetLoadID_(CardID id)
        {
            return load_ids[id];
        }

        __declspec(noinline) static Return* ReadCardBin__(Register rcx, Register rdx, Register r8, Register r9)
        {
            Return* ret = (Return*)_malloca(sizeof(Return));

            ret->rcx = rcx;
            ret->rdx = rdx;
            ret->r8 = r8;
            ret->r9 = r9;
            ret->rip = ReadCardBinHook.Original;

            // Copy game's loaded cards to our buffer
            memcpy(&card_datas, (Pointer)0x1427D0C30, original_cards_maxloadid * sizeof(CardData));
            memcpy(&card_datas_by_id, (Pointer)0x142847E50, original_cards_maxid * sizeof(CardData));

            // Add our own cards
            CardID load_id = original_cards_maxloadid;

            for (auto const& [id, card] : cards)
            {
                card_datas[++load_id] = card.Data;
                card_datas_by_id[id] = card.Data;
            }

            return ret;
        }

        __declspec(noinline) static void ReadCardBin_()
        {
            CallEx(ReadCardBin__);
        }

        __declspec(noinline) static void SetupImageTable_(void)
        {
            CardImageData** dummy = (CardImageData**)0x142847E84;
            *dummy = &card_images[0];

            SetupImageTableHook.CallOriginal(&SetupImageTable_)();
        }

        static Byte unlocked_cards[new_card_limit];

        __declspec(noinline) static Return* UnlockedTrunkCards1__(Byte original_unlocked_cards[original_cards_maxloadid], Register rdx, Register r8, Register r9)
        {
            Return* ret = (Return*)_malloca(sizeof(Return));

            ret->rdx = rdx;
            ret->r8 = r8;
            ret->r9 = r9;
            ret->rip = UnlockedTrunkCardsHook1.Original;

            memcpy(&unlocked_cards[0], &original_unlocked_cards[0], original_cards_maxloadid * sizeof(Byte));

            for (auto const& [id, card] : cards)
            {
                unlocked_cards[load_ids[id]] = 3;
            }

            ret->rax = (Register)(&unlocked_cards[0]);

            return ret;
        }

        __declspec(noinline) static void UnlockedTrunkCards1_(void)
        {
            MovRcxRax();

            CallEx(UnlockedTrunkCards1__);
        }

        __declspec(noinline) static void WriteLoadIDTable_(void)
        {
            // Copy game's loaded cards to our buffer
            memcpy(&load_ids[original_cards_offset], (Pointer)0x140D55480, original_cards_offseted_maxid * sizeof(CardID));

            WriteLoadIDTableHook.CallOriginal(&WriteLoadIDTable_)();
        }
    };

    map<CardID, NewCard> Cards::cards;

    CardData Cards::card_datas[new_card_limit];

    CardData Cards::card_datas_by_id[new_card_limit];

    CardImageData Cards::card_images[new_card_limit];

    CardID Cards::load_ids[new_card_limit];

    Byte Cards::unlocked_cards[new_card_limit];
}
