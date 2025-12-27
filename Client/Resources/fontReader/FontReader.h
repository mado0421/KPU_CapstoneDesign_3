#pragma once
#include<fstream>
#include<string>
#include<vector>

using namespace std;

struct FontChar
{
    // the unicode id
    int id;

    float x; // u texture coordinate
    float y; // v texture coordinate

    float width;  // width of character in screen coords
    float height; // height of character in screen coords

    // these need to be normalized based on size of font
    float xoffset;  // offset from current cursor pos to left side of character
    float yoffset;  // offset from top of line to top of character
    float xadvance; // how far to move to right for next character

    // skip page,chnl
};

struct FontKerning
{
    int first;
    int second;
    int amount;
};

class FontFormat
{
public:
    string       name; // name of the font
    string       fontImage;
    int          size;                    // size of font, lineheight and baseheight will be based on this as if this is a single unit (1.0)
    float        lineHeight;              // how far to move down to next line, will be normalized
    float        baseHeight;              // height of all characters, will be normalized
    int          textureWidth;            // width of the font texture
    int          textureHeight;           // height of the font texture
    int          numCharacters = 0;       // number of characters in the font
    FontChar*    CharList      = nullptr; // list of characters
    int          numKernings   = 0;       // the number of kernings
    FontKerning* KerningsList  = nullptr; // list to hold kerning values
    //ID3D12Resource* textureBuffer; // the font texture resource
    //D3D12_GPU_DESCRIPTOR_HANDLE srvHandle; // the font srv

    // these are how much the character is padded in the texture. We
    // add padding to give sampling a little space so it does not accidentally
    // padd the surrounding characters. We will need to subtract these paddings
    // from the actual spacing between characters to remove the gaps you would otherwise see
    float leftpadding;
    float toppadding;
    float rightpadding;
    float bottompadding;

    ~FontFormat()
    {
        if (CharList != nullptr) delete[] CharList;
        if (KerningsList != nullptr) delete[] KerningsList;
    }

    // this will return a FontChar given a wide character
    FontChar* GetChar(char c)
    {
        for (int i = 0; i < numCharacters; ++i) if (c == static_cast<wchar_t>(CharList[i].id)) return &CharList[i];
        return nullptr;
    }

    // for test
    void PrintFontChars();
    void PrintFontAll();
};

class FontReader
{
    string     fname;
    ifstream   ifs;
    bool       isKerning;
    FontFormat fontData;

    string curs;

public:
    FontReader();
    FontReader(string fileName);
    virtual ~FontReader();

    void        FontRead();
    void        FontRead(string fileName);
    void        SetKerning(bool x) { isKerning = x; }
    FontFormat& GetFormat() { return fontData; }

private:
    vector<string> Split(string input, char delimiter);

    // parse and return (delimiter is "=") for chars
    inline int   GetInt(ifstream& ifs);
    inline float GetFloat(ifstream& ifs);

    // for testing, not use
    string Find(ifstream& ifs, string target, char delimiter);
    void   FindPadding(ifstream& ifs);
    void   PrintFontChars();
};
