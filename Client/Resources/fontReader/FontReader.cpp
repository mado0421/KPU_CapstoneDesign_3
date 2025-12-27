#include "pch.h"
#include "FontReader.h"

FontReader::FontReader() : isKerning(false) {}

FontReader::FontReader(string fileName) : fname(fileName), isKerning(false) {}

FontReader::~FontReader() { if (ifs && ifs.is_open()) ifs.close(); }

void FontReader::FontRead()
{
    ifs.open(fname);
    if (ifs.fail()) cout << "Failed open file name : " << fname << "\n";
    if (ifs.good())
    {
        //parse info
        while (ifs >> curs)
        {
            if (curs.find("face") != string::npos)
            {
                vector<string> sp = Split(curs, '\"');
                fontData.name     = sp[1];
            }
            if (curs.find("size") != string::npos)
            {
                vector<string> sp = Split(curs, '=');
                fontData.size     = atoi(sp[1].c_str());
            }
            if (curs.find("padding") != string::npos)
            {
                vector<string> sp       = Split(curs, '=');
                vector<string> paddings = Split(sp[1], ',');
                fontData.leftpadding    = atof(paddings[0].c_str());
                fontData.toppadding     = atof(paddings[1].c_str());
                fontData.rightpadding   = atof(paddings[2].c_str());
                fontData.bottompadding  = atof(paddings[3].c_str());
            }
            if (curs.find("lineHeight") != string::npos)
            {
                vector<string> sp   = Split(curs, '=');
                fontData.lineHeight = atof(sp[1].c_str());
            }
            if (curs.find("base") != string::npos)
            {
                vector<string> sp   = Split(curs, '=');
                fontData.baseHeight = atof(sp[1].c_str());
            }
            if (curs.find("scaleW") != string::npos)
            {
                vector<string> sp     = Split(curs, '=');
                fontData.textureWidth = atoi(sp[1].c_str());
            }
            if (curs.find("scaleH") != string::npos)
            {
                vector<string> sp      = Split(curs, '=');
                fontData.textureHeight = atoi(sp[1].c_str());
            }
            if (curs.find("file") != string::npos)
            {
                vector<string> sp  = Split(curs, '\"');
                fontData.fontImage = sp[1];
            }
            if (curs.find("chars") != string::npos)
            {
                ifs >> curs;
                if (curs.find("count") != string::npos)
                {
                    vector<string> sp      = Split(curs, '=');
                    int            count   = atoi(sp[1].c_str());
                    fontData.numCharacters = count;
                    cout << "count:" << count << "\n";
                    break; // end of info
                }
            }
        }
        //parse CharList
        int numChars = fontData.numCharacters;
        if (numChars > 0)
        {
            fontData.CharList = new FontChar[numChars];
            for (int i = 0; i < numChars; ++i)
            {
                ifs >> curs; // for skip "char" string

                fontData.CharList[i].id       = GetInt(ifs);
                fontData.CharList[i].x        = GetFloat(ifs);
                fontData.CharList[i].y        = GetFloat(ifs);
                fontData.CharList[i].width    = GetFloat(ifs);
                fontData.CharList[i].height   = GetFloat(ifs);
                fontData.CharList[i].xoffset  = GetFloat(ifs);
                fontData.CharList[i].yoffset  = GetFloat(ifs);
                fontData.CharList[i].xadvance = GetFloat(ifs);

                // for skip "page , chnl"
                ifs >> curs;
                ifs >> curs;
            }
        }
        //parse Kerning
    }
    ifs.close();
}

void FontReader::FontRead(string fileName)
{
    fname = fileName;
    FontRead();
}

vector<string> FontReader::Split(string input, char delimiter = '=')
{
    vector<string> ret;
    stringstream   ss(input);
    string         curs;

    while (getline(ss, curs, delimiter)) ret.push_back(curs);
    return ret;
}

inline int FontReader::GetInt(ifstream& ifs)
{
    ifs >> curs;
    vector<string> temp = Split(curs);
    return atoi(temp[1].c_str());
}

inline float FontReader::GetFloat(ifstream& ifs)
{
    ifs >> curs;
    vector<string> temp = Split(curs);
    return atof(temp[1].c_str());
}


// for testing, not use

void FontFormat::PrintFontChars()
{
    for (int i = 0; i < numCharacters; ++i)
    {
        auto p = CharList[i];
        cout << "id:" << p.id << " ";
        cout << "x:" << p.x << " ";
        cout << "y:" << p.y << " ";
        cout << "width:" << p.width << " ";
        cout << "height:" << p.height << " ";
        cout << "xoffset:" << p.xoffset << " ";
        cout << "yoffset:" << p.yoffset << " ";
        cout << "xadvance:" << p.xadvance << "\n";
    }
}

void FontFormat::PrintFontAll()
{
    cout << "name: " << name << "\n";
    cout << "fontImage: " << fontImage << "\n";
    cout << "size: " << size << "\n";
    cout << "lineHeight: " << lineHeight << "\n";
    cout << "baseHeight: " << baseHeight << "\n";
    cout << "width: " << textureWidth << "\n";
    cout << "height: " << textureHeight << "\n";
    cout << "padding: (" << leftpadding << "," << toppadding << "," << rightpadding << "," << bottompadding << ")\n";
    cout << "count of chars: " << numCharacters << "\n";
    PrintFontChars();
}

string FontReader::Find(ifstream& ifs, string target, char delimiter = '=')
{
    string curs;
    while (ifs >> curs)
    {
        if (curs.find(target) != string::npos)
        {
            vector<string> sp = Split(curs, delimiter);
            return sp[1];
        }
    }
    return "";
}

void FontReader::FindPadding(ifstream& ifs)
{
    string curs;
    while (ifs >> curs)
    {
        if (curs.find("padding") != string::npos)
        {
            vector<string> sp       = Split(curs, '=');
            vector<string> paddings = Split(sp[1], ',');
            fontData.leftpadding    = atof(paddings[0].c_str());
            fontData.toppadding     = atof(paddings[1].c_str());
            fontData.rightpadding   = atof(paddings[2].c_str());
            fontData.bottompadding  = atof(paddings[3].c_str());
            /*	cout << fontData.leftpadding << ","
					<< fontData.toppadding << ","
					<< fontData.rightpadding << ","
					<< fontData.bottompadding << "\n";*/
        }
        return;
    }
}

void FontReader::PrintFontChars()
{
    for (int i = 0; i < fontData.numCharacters; ++i)
    {
        auto p = fontData.CharList[i];
        cout << "id:" << p.id << " ";
        cout << "x:" << p.x << " ";
        cout << "y:" << p.y << " ";
        cout << "width:" << p.width << " ";
        cout << "height:" << p.height << " ";
        cout << "xoffset:" << p.xoffset << " ";
        cout << "yoffset:" << p.yoffset << " ";
        cout << "xadvance:" << p.xadvance << "\n";
    }
}
