#include <fmt/format.h>
#include <fstream>
#include <nlohmann/json.hpp>

using namespace std::string_literals;
using json    = nlohmann::json;
using u8      = std::uint8_t;
using u16     = std::uint16_t;
using u32     = std::uint32_t;

const u32 RomStartAddress = 0x8000000;

namespace {

    enum Language {
        Language_USA,
        Language_Japanese,
        Language_English,
        Language_French,
        Language_German,
        Language_Spanish,
        Language_Italian,

        Language_Count
    };

    struct LanguageTable {
        Language lang;
        const char *name;
        u32 address;
    };

    enum Color {
        Color_White,
        Color_Red,
        Color_Green,
        Color_Blue,
        Color_Yellow,

        Color_Count,
    };

    constexpr const char *const ColorStrings[] = {
        [Color_White]  = "White",
        [Color_Red]    = "Red",
        [Color_Green]  = "Green",
        [Color_Blue]   = "Blue",
        [Color_Yellow] = "Yellow",
    };

    enum Input {
        Input_A,
        Input_B,
        Input_Left,
        Input_Right,
        Input_DUp,
        Input_DDown,
        Input_DLeft,
        Input_DRight,
        Input_Dpad,
        Input_Select,
        Input_Start,

        Input_Count,
    };

    constexpr const char *const InputStrings[] = {
        [Input_A]      = "A",
        [Input_B]      = "B",
        [Input_Left]   = "Left",
        [Input_Right]  = "Right",
        [Input_DUp]    = "DUp",
        [Input_DDown]  = "DDown",
        [Input_DLeft]  = "DLeft",
        [Input_DRight] = "DRight",
        [Input_Dpad]   = "Dpad",
        [Input_Select] = "Select",
        [Input_Start]  = "Start",
    };

    std::pair<u8, std::string> CharConvertArray[] = {
        {0x0a, "\n"},
        {0x0d, "\r"},
        {0x20, " "},
        {0x21, "!"},
        {0x22, "\""},
        {0x23, "#"},
        {0x24, "$"},
        {0x25, "%"},
        {0x26, "&"},
        {0x27, "\'"},
        {0x28, "("},
        {0x29, ")"},
        {0x2a, "*"},
        {0x2b, "+"},
        {0x2c, ","},
        {0x2d, "-"},
        {0x2e, "."},
        {0x2f, "/"},
        {0x30, "0"},
        {0x31, "1"},
        {0x32, "2"},
        {0x33, "3"},
        {0x34, "4"},
        {0x35, "5"},
        {0x36, "6"},
        {0x37, "7"},
        {0x38, "8"},
        {0x39, "9"},
        {0x3a, ":"},
        {0x3b, ";"},
        {0x3c, "<"},
        {0x3d, "="},
        {0x3e, ">"},
        {0x3f, "?"},
        {0x40, "@"},
        {0x41, "A"},
        {0x42, "B"},
        {0x43, "C"},
        {0x44, "D"},
        {0x45, "E"},
        {0x46, "F"},
        {0x47, "G"},
        {0x48, "H"},
        {0x49, "I"},
        {0x4a, "J"},
        {0x4b, "K"},
        {0x4c, "L"},
        {0x4d, "M"},
        {0x4e, "N"},
        {0x4f, "O"},
        {0x50, "P"},
        {0x51, "Q"},
        {0x52, "R"},
        {0x53, "S"},
        {0x54, "T"},
        {0x55, "U"},
        {0x56, "V"},
        {0x57, "W"},
        {0x58, "X"},
        {0x59, "Y"},
        {0x5a, "Z"},
        {0x5b, "["},
        {0x5c, "\'"},
        {0x5d, "]"},
        {0x5e, "^"},
        {0x5f, "_"},
        {0x60, "`"},
        {0x61, "a"},
        {0x62, "b"},
        {0x63, "c"},
        {0x64, "d"},
        {0x65, "e"},
        {0x66, "f"},
        {0x67, "g"},
        {0x68, "h"},
        {0x69, "i"},
        {0x6a, "j"},
        {0x6b, "k"},
        {0x6c, "l"},
        {0x6d, "m"},
        {0x6e, "n"},
        {0x6f, "o"},
        {0x70, "p"},
        {0x71, "q"},
        {0x72, "r"},
        {0x73, "s"},
        {0x74, "t"},
        {0x75, "u"},
        {0x76, "v"},
        {0x77, "w"},
        {0x78, "x"},
        {0x79, "y"},
        {0x7a, "z"},
        {0x82, ","},
        {0x84, "„"},
        {0x85, "⋯"},
        {0x8A, "Š"},
        {0x8B, "‹"},
        {0x8C, "Œ"},
        {0x8E, "Ž"},
        {0x91, "‘"},
        {0x92, "’"},
        {0x93, "“"},
        {0x94, "”"},
        {0x95, "·"},
        {0x99, "™"},
        {0x9A, "š"},
        {0x9B, "›"},
        {0x9C, "œ"},
        {0x9E, "ž"},
        {0x9F, "Ÿ"},
        {0xA1, "¡"},
        {0xA3, "♪"},
        {0xAA, "ª"},
        {0xAB, "«"},
        {0xB0, "º"},
        {0xB4, "'"},
        {0xB7, "´"},
        {0xBA, "º"},
        {0xBB, "»"},
        {0xBF, "¿"},
        {0xC0, "À"},
        {0xC1, "Á"},
        {0xC2, "Â"},
        {0xC3, "Ã"},
        {0xC4, "Ä"},
        {0xC5, "Å"},
        {0xC6, "Æ"},
        {0xC7, "Ç"},
        {0xC8, "È"},
        {0xC9, "É"},
        {0xCA, "Ê"},
        {0xCB, "Ë"},
        {0xCC, "Ì"},
        {0xCD, "Í"},
        {0xCE, "Î"},
        {0xCF, "Ï"},
        {0xD0, "Đ"},
        {0xD1, "Ñ"},
        {0xD2, "Ò"},
        {0xD3, "Ó"},
        {0xD4, "Ô"},
        {0xD5, "Õ"},
        {0xD6, "Ö"},
        {0xD7, "×"},
        {0xD8, "Ø"},
        {0xD9, "Ù"},
        {0xDA, "Ú"},
        {0xDB, "Û"},
        {0xDC, "Ü"},
        {0xDD, "Ý"},
        {0xDE, "Þ"},
        {0xDF, "β"},
        {0xE0, "à"},
        {0xE1, "á"},
        {0xE2, "â"},
        {0xE3, "ã"},
        {0xE4, "ä"},
        {0xE5, "å"},
        {0xE6, "æ"},
        {0xE7, "ç"},
        {0xE8, "è"},
        {0xE9, "é"},
        {0xEA, "ê"},
        {0xEB, "ë"},
        {0xEC, "ì"},
        {0xED, "í"},
        {0xEE, "î"},
        {0xEF, "ï"},
        {0xF0, "ð"},
        {0xF1, "ñ"},
        {0xF2, "ò"},
        {0xF3, "ó"},
        {0xF4, "ô"},
        {0xF5, "õ"},
        {0xF6, "ö"},
        {0xF7, "÷"},
        {0xF8, "ø"},
        {0xF9, "ù"},
        {0xFA, "ú"},
        {0xFB, "û"},
        {0xFC, "ü"},
        {0xFD, "ý"},
        {0xFE, "þ"},
        {0xFF, "ÿ"},
    };

    std::string Unk1Handler(const char *&ptr);
    std::string ColorHandler(const char *&ptr);
    std::string SoundHandler(const char *&ptr);
    std::string Unk4Handler(const char *&ptr);
    std::string ChoiceHandler(const char *&ptr);
    std::string VariableHandler(const char *&ptr);
    std::string Unk7Handler(const char *&ptr);
    std::string Unk8Handler(const char *&ptr);
    std::string Unk9Handler(const char *&ptr);
    std::string InputHandler(const char *&ptr);
    std::string SymbolHandler(const char *&ptr);

    using ConvertFunction = std::string (*const)(const char *&);

    std::pair<u8, ConvertFunction> FuncConvertArray[] = {
        {0x01, Unk1Handler},
        {0x02, ColorHandler},
        {0x03, SoundHandler},
        {0x04, Unk4Handler},
        {0x05, ChoiceHandler},
        {0x06, VariableHandler},
        {0x07, Unk7Handler},
        {0x08, Unk8Handler},
        {0x09, Unk9Handler},
        {0x0c, InputHandler},
        {0x0f, SymbolHandler},
    };

    std::string ParseTMCString(const char *ptr) {
        std::string ret;

        while (*ptr) {
            u8 c = *ptr++;

            /* Convert character. */
            {
                const auto it = std::find_if(std::begin(CharConvertArray), std::end(CharConvertArray), [c](std::pair<u8, std::string> &data) {
                    return c == data.first;
                });

                if (it != std::end(CharConvertArray)) {
                    ret += it->second;
                    continue;
                }
            }

            /* Convert function. */
            {
                const auto it = std::find_if(std::begin(FuncConvertArray), std::end(FuncConvertArray), [c](std::pair<u8, ConvertFunction> &data) {
                    return c == data.first;
                });

                if (it != std::end(FuncConvertArray)) {
                    ret += it->second(ptr);
                    continue;
                }
            }

            throw std::runtime_error(fmt::format("Unknown characters: {}", ptr));
        }

        return ret;
    }

    std::string Unk1Handler(const char *&ptr) {
        u8 a = *ptr++;
        return fmt::format("{{01:{:02X}}}", a);
    }

    std::string ColorHandler(const char *&ptr) {
        u8 color = *ptr++;
        if (color >= Color_Count)
            throw std::runtime_error(ptr);
        return fmt::format("{{Color:{}}}", ColorStrings[color]);
    }

    std::string SoundHandler(const char *&ptr) {
        u8 a = *ptr++;
        u8 b = *ptr++;
        return fmt::format("{{Sound:{:02X}:{:02X}}}", a, b);
    }

    std::string Unk4Handler(const char *&ptr) {
        u8 a = *ptr++;
        if (a == 0x10) {
            u8 b = *ptr++;
            return fmt::format("{{04:{:02X}:{:02X}}}", a, b);
        } else {
            return fmt::format("{{04:{:02X}}}", a);
        }
    }

    std::string ChoiceHandler(const char *&ptr) {
        u8 category = *ptr++;
        if (category == 0xff) {
            return "{Choice:FF}";
        } else {
            u8 action = *ptr++;
            return fmt::format("{{Choice:{:02X}:{:02X}}}", category, action);
        }
    }

    std::string VariableHandler(const char *&ptr) {
        u8 idx = *ptr++;
        if (idx == 0) {
            return "{Player}";
        } else {
            return fmt::format("{{Var:{:X}}}", idx);
        }
    }

    std::string Unk7Handler(const char *&ptr) {
        u8 a = *ptr++;
        u8 b = *ptr++;
        return fmt::format("{{07:{:02X}:{:02X}}}", a, b);
    }

    std::string Unk8Handler(const char *&ptr) {
        u8 a = *ptr++;
        if (a != 0xff)
            throw std::runtime_error("unmatched unk8: "s + std::to_string(a));
        return "{08:FF}";
    }

    std::string Unk9Handler(const char *&ptr) {
        u8 a = *ptr++;
        if (a != 0x78 && a != 0x00)
            throw std::runtime_error("unmatched unk8: "s + std::to_string(a));
        return fmt::format("{{09:{:02X}}}", a);
    }

    std::string InputHandler(const char *&ptr) {
        u8 key = *ptr++;
        if (key > 8)
            throw std::runtime_error("unmatched key: "s + std::to_string(key));
        return fmt::format("{{Key:{}}}", InputStrings[key]);
    }

    std::string SymbolHandler(const char *&ptr) {
        u8 a = *ptr++;
        return fmt::format("{{Symbol:{:02X}}}", a);
    }

    using RevertFunction = void (*)(const char *&, char *&);

    void ColorRevert(const char *&src, char *&dst) {
        *dst++ = 0x02;
        src++; // ':'
        for (u32 i = 0; i < Color_Count; i++) {
            const char *const color     = ColorStrings[i];
            const std::size_t color_len = std::strlen(color);
            if (std::strncmp(src, color, color_len) == 0) {
                *dst++ = i;
                src += color_len;
                return;
            }
        }

        throw std::runtime_error(fmt::format("Color not found: {}", src));
    }

    void SoundRevert(const char *&src, char *&dst) {
        *dst++ = 0x03;
        *dst++ = std::strtoul(++src, nullptr, 0x10);
        src += 2;
        *dst++ = std::strtoul(++src, nullptr, 0x10);
        src += 2;
    }

    void ChoiceRevert(const char *&src, char *&dst) {
        *dst++    = 0x05;
        u8 choice = std::strtoul(++src, nullptr, 0x10);

        *dst++ = choice;
        src += 2;

        if (choice == 0xff)
            return;

        *dst++ = std::strtoul(++src, nullptr, 0x10);
        src += 2;
    }

    void PlayerRevert(const char *&src, char *&dst) {
        *dst++ = 0x06;
        *dst++ = 0x00;

        (void)src;
    }

    void VariableRevert(const char *&src, char *&dst) {
        *dst++ = 0x06;
        src++; // ':'

        *dst++ = std::strtoul(src, nullptr, 0x10);
        src++;
    }

    void KeyRevert(const char *&src, char *&dst) {
        *dst++ = 0x0c;
        src++; // ':'
        for (u32 i = 0; i < Input_Count; i++) {
            const char *const input     = InputStrings[i];
            const std::size_t input_len = std::strlen(input);
            if (std::strncmp(src, input, input_len) == 0) {
                *dst++ = i;
                src += input_len;
                return;
            }
        }

        throw std::runtime_error(fmt::format("Input not found: {}", src));
    }

    void SymbolRevert(const char *&src, char *&dst) {
        *dst++ = 0x0f;
        src++; // ':'

        *dst++ = std::strtoul(src, nullptr, 0x10);
        src += 2;
    }

    std::pair<std::string, RevertFunction> FuncRevertArray[] = {
        {"Color", ColorRevert},
        {"Sound", SoundRevert},
        {"Choice", ChoiceRevert},
        {"Player", PlayerRevert},
        {"Var", VariableRevert},
        {"Key", KeyRevert},
        {"Symbol", SymbolRevert},
    };

    char *WriteTMCString(char *dst, const std::string &src) {
        const char *ptr = src.data();

        while (*ptr) {
            /* Parse special */
            {
                if (*ptr == '{') {
                    ptr++;
                    auto it = std::find_if(std::begin(FuncRevertArray), std::end(FuncRevertArray), [&](std::pair<std::string, RevertFunction> &data) {
                        auto &str = data.first;
                        return std::strncmp(ptr, str.c_str(), str.size()) == 0;
                    });

                    if (it != std::end(FuncRevertArray)) {
                        ptr += it->first.size();
                        it->second(ptr, dst);
                    } else {
                        do {
                            *dst++ = std::strtoul(ptr, nullptr, 0x10);
                            ptr += 2;
                        } while (*ptr == ':' && (ptr++, true));
                    }

                    if (*ptr != '}')
                        throw std::runtime_error(fmt::format("unmatched characters: \"{}\"\n", ptr));

                    ptr++;
                    continue;
                }
            }

            /* Convert character. */
            {
                const auto it = std::find_if(std::begin(CharConvertArray), std::end(CharConvertArray), [ptr](std::pair<u8, std::string> &data) {
                    if (data.second.size() == 1)
                        return data.second[0] == *ptr;
                    else if (data.second.size() > 0)
                        return std::strncmp(ptr, data.second.c_str(), data.second.length()) == 0;
                    else
                        throw std::runtime_error(ptr);
                });

                if (it != std::end(CharConvertArray)) {
                    ptr += it->second.size();
                    *dst++ = it->first;
                    continue;
                }
            }

            throw std::runtime_error(fmt::format("unmatched characters: \"{}\"\n", ptr));
        }
        *dst++ = '\0';

        return dst;
    }

}

void DumpStrings(const char *const rom_path, const std::vector<LanguageTable> &tables) {
    std::vector<char> rom;

    {
        /* Open ROM. */
        std::ifstream rom_file(rom_path, std::ios::in);

        /* Get ROM size. */
        rom_file.seekg(0, std::ios::end);
        const std::size_t rom_size = rom_file.tellg();

        /* Read ROM to buffer. */
        rom.resize(rom_size);
        rom_file.seekg(0, std::ios::beg);
        rom_file.read(&rom[0], rom_size);
    }

    auto ReadAbsolute = [&](u32 address) -> u32 {
        return *(u32 *)&rom[address - RomStartAddress];
    };

    for (auto &language : tables) {
        /* Get category table start. */
        const std::size_t table_start = language.address;

        /* Get category count. */
        const std::size_t category_count = ReadAbsolute(table_start) / sizeof(u32);

        /* Read category offsets. */
        std::vector<u32> category_table(category_count);
        std::memcpy(&category_table[0], &rom[table_start - RomStartAddress], category_count * sizeof(u32));

        json j = json::array();

        for (auto &category_offset : category_table) {
            /* Get string table start. */
            const std::size_t category_start = table_start + category_offset;

            /* Get string count. */
            const std::size_t string_count = ReadAbsolute(category_start) / sizeof(u32);

            /* Read string offsets. */
            std::vector<u32> string_table(string_count);
            std::memcpy(&string_table[0], &rom[category_start - RomStartAddress], string_count * sizeof(u32));

            auto &category = j.emplace_back();

            for (std::size_t l = 0; l < string_count; l++) {
                /* Get string start. */
                const std::size_t string_start = category_start + string_table[l];

                /* Parse TMC. */
                if constexpr (false) {
                    category[l] = fmt::format("{:X}: {}", string_start, ParseTMCString(&rom[string_start - RomStartAddress]));
                } else {
                    category[l] = ParseTMCString(&rom[string_start - RomStartAddress]);
                }
            }
        }

        const std::string out_path = language.name + ".json"s;
        std::ofstream ofs(out_path);
        ofs << j.dump(4);
    }
}

void makeStringTable(const char *const src_path, const char *const dst_path) {
    const json j = [&]() -> json {
        std::ifstream ifs(src_path);
        return json::parse(ifs);
    }();

    std::vector<char> buffer(301536);
    std::uintptr_t root_start = (std::uintptr_t)buffer.data();

    char *root_ptr = buffer.data();
    char *table    = buffer.data() + j.size() * sizeof(u32);

    for (auto &category : j) {
        char *table_ptr = table;
        char *str_start = table_ptr + category.size() * sizeof(u32);
        char *str_ptr   = str_start;
        for (auto &str_j : category) {
            /* Write string offset to table. */
            *(u32 *)table_ptr = (std::uintptr_t)str_ptr - (std::uintptr_t)table;
            table_ptr += sizeof(u32);

            auto str = str_j.get<std::string>();

            /* Copy string to table. */
            if constexpr (false) {
                std::memcpy(str_ptr, str.data(), str.size());
                str_ptr += str.size() + 1;
            } else {
                str_ptr = WriteTMCString(str_ptr, str);
            }
        }

        /* Align string table size by 16 bytes. */
        while (static_cast<std::uintptr_t>(str_ptr - str_start) & 0xf) {
            *str_ptr = 0xff;
            str_ptr++;
        }

        /* Write category offset to root table. */
        *(u32 *)root_ptr = (std::uintptr_t)table - root_start;
        root_ptr += sizeof(u32);

        /* */
        table = str_ptr;
    }

    while ((std::uintptr_t)table & 0xf) {
        *table = 0xff;
        table++;
    }

    const std::size_t table_size = (uintptr_t)table - root_start;
    std::ofstream ofs(dst_path);
    ofs.write(buffer.data(), table_size);
}

const std::vector<LanguageTable> LanguageTableUS = {
    {Language_USA, "USA", 0x89B1D90},
};

const std::vector<LanguageTable> LanguageTableEU = {
    {Language_English, "English", 0x89AEB60},
    {Language_French, "French", 0x89F7420},
    {Language_German, "German", 0x8A3EEB0},
    {Language_Spanish, "Spanish", 0x8A81E70},
    {Language_Italian, "Italian", 0x8AC37A0},
};

int main() {
    DumpStrings("us.gba", LanguageTableUS);
    DumpStrings("eu.gba", LanguageTableEU);

    makeStringTable("USA.json", "USA.bin");
    makeStringTable("English.json", "English.bin");
    makeStringTable("French.json", "French.bin");
    makeStringTable("German.json", "German.bin");
    makeStringTable("Spanish.json", "Spanish.bin");
    makeStringTable("Italian.json", "Italian.bin");
}
