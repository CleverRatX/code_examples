#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cmath>

using namespace std;

int hex_char_to_int(char c)
{
    if (c >= '0' && c <= '9')
        return c - '0';
    else if (c >= 'A' && c <= 'F')
        return c - 'A' + 10;
    else if (c >= 'a' && c <= 'f')
        return c - 'a' + 10;
    return 0;
}

char int_to_hex_char(int v)
{
    if (v >= 0 && v <= 9)
        return v + '0';
    else if (v >= 10 && v <= 15)
        return v - 10 + 'A';
    return '?';
}

int bin_to_int(string s)
{
    int res = 0;
    for (size_t i = 0; i < s.length(); i++)
    {
        res = res * 2 + (s[i] - '0');
    }
    return res;
}

string int_to_bin(int v, int len)
{
    string s = "";
    for (int i = 0; i < len; i++)
    {
        s = char((v % 2) + '0') + s;
        v /= 2;
    }
    return s;
}

string hex_to_bin(string hex)
{
    string res = "";
    for (size_t i = 0; i < hex.length(); i++)
    {
        int v = hex_char_to_int(hex[i]);
        res += int_to_bin(v, 4);
    }
    return res;
}

char bin_to_hex_char(string b)
{
    int v = bin_to_int(b);
    return int_to_hex_char(v);
}

string bin_to_hex(string b)
{
    while (b.length() % 4 != 0)
        b = "0" + b;
    while (b.length() > 8 && b[0] == '0')
        b = b.substr(1);
    while (b.length() < 8)
        b = "0" + b;

    string h = "";
    for (size_t i = 0; i < b.length(); i += 4)
    {
        h += bin_to_hex_char(b.substr(i, 4));
    }
    return "0x" + h;
}

string bin_to_hex_shift(string b)
{
    b = b + "0";
    return bin_to_hex(b);
}

string invert(string s)
{
    for (size_t i = 0; i < s.length(); i++)
    {
        if (s[i] == '0')
            s[i] = '1';
        else
            s[i] = '0';
    }
    return s;
}

string get_reg(string b, int offset = 0)
{
    int n = bin_to_int(b) + offset;
    return "r" + to_string(n);
}

string get_addr_offset(string b)
{
    string res = ".";
    if (b[0] == '0')
    {
        res += "+" + to_string(bin_to_int(b) * 2);
    }
    else
    {
        res += "-" + to_string((bin_to_int(invert(b)) + 1) * 2);
    }
    return res;
}

struct Record
{
    string addr;
    string h1;
    string h2;
};

void parse_line(vector<Record>& vec, string line)
{
    if (line.length() < 11)
        return;
    if (line[0] != ':')
        return;

    int type = hex_char_to_int(line[7]) * 16 + hex_char_to_int(line[8]);
    if (type != 0)
        return;

    int bytes = hex_char_to_int(line[1]) * 16 + hex_char_to_int(line[2]);
    string base_addr = line.substr(3, 4);

    for (int i = 0; i < bytes; i += 2)
    {
        int off = i / 2;
        string addr = base_addr.substr(0, 3);
        addr += int_to_hex_char(off % 16);

        Record r;
        r.addr = addr;
        r.h1 = line.substr(9 + i * 2, 2);
        r.h2 = line.substr(9 + i * 2 + 2, 2);
        vec.push_back(r);
    }
}

bool match(string bin, string mask)
{
    if (bin.length() != mask.length())
        return false;
    for (size_t i = 0; i < mask.length(); i++)
    {
        if (mask[i] == '0' || mask[i] == '1')
        {
            if (bin[i] != mask[i])
                return false;
        }
    }
    return true;
}

string extract(string bin, string mask, char c)
{
    string res = "";
    for (size_t i = 0; i < mask.length(); i++)
    {
        if (mask[i] == c)
            res += bin[i];
    }
    return res;
}

void decode(ofstream& out, vector<Record>& vec)
{
    size_t i = 0;
    while (i < vec.size())
    {
        string line = vec[i].addr + ": " + vec[i].h1 + " " + vec[i].h2;

        bool is_double = false;
        if (vec[i].h2[0] == '9')
        {
            if (vec[i].h2[1] == '4' || vec[i].h2[1] == '5')
            {
                if (vec[i].h1[1] >= 'C' && vec[i].h1[1] <= 'F')
                    is_double = true;
            }
            else if (vec[i].h2[1] >= '0' && vec[i].h2[1] <= '3')
            {
                if (vec[i].h1[1] == '0')
                    is_double = true;
            }
        }

        string bin = "";
        if (is_double && i + 1 < vec.size())
        {
            line += " " + vec[i+1].h1 + " " + vec[i+1].h2;
            bin = hex_to_bin(vec[i].h2 + vec[i].h1 + vec[i+1].h2 + vec[i+1].h1);
        }
        else
        {
            line += "      ";
            bin = hex_to_bin(vec[i].h2 + vec[i].h1);
        }

        string cmd = "";

        if (match(bin, "0000000000000000"))
        {
            cmd = "nop";
        }
        else if (match(bin, "000011rdddddrrrr"))
        {
            cmd = "add " + get_reg(extract(bin, "000011rdddddrrrr", 'd')) + ", " +
                  get_reg(extract(bin, "000011rdddddrrrr", 'r'));
        }
        else if (match(bin, "001001rdddddrrrr"))
        {
            cmd = "eor " + get_reg(extract(bin, "001001rdddddrrrr", 'd')) + ", " +
                  get_reg(extract(bin, "001001rdddddrrrr", 'r'));
        }
        else if (match(bin, "001011rdddddrrrr"))
        {
            cmd = "mov " + get_reg(extract(bin, "001011rdddddrrrr", 'd')) + ", " +
                  get_reg(extract(bin, "001011rdddddrrrr", 'r'));
        }
        else if (match(bin, "0100KKKKddddKKKK"))
        {
            cmd = "sbci " + get_reg(extract(bin, "0100KKKKddddKKKK", 'd'), 16) + ", " +
                  bin_to_hex(extract(bin, "0100KKKKddddKKKK", 'K'));
        }
        else if (match(bin, "0101KKKKddddKKKK"))
        {
            cmd = "subi " + get_reg(extract(bin, "0101KKKKddddKKKK", 'd'), 16) + ", " +
                  bin_to_hex(extract(bin, "0101KKKKddddKKKK", 'K'));
        }
        else if (match(bin, "1001010011111000"))
        {
            cmd = "cli";
        }
        else if (match(bin, "10011000PPPPPbbb"))
        {
            cmd = "cbi " + bin_to_hex(extract(bin, "10011000PPPPPbbb", 'P')) + ", " +
                  to_string(bin_to_int(extract(bin, "10011000PPPPPbbb", 'b')));
        }
        else if (match(bin, "10011001PPPPPbbb"))
        {
            cmd = "sbic " + bin_to_hex(extract(bin, "10011001PPPPPbbb", 'P')) + ", " +
                  to_string(bin_to_int(extract(bin, "10011001PPPPPbbb", 'b')));
        }
        else if (match(bin, "10011010PPPPPbbb"))
        {
            cmd = "sbi " + bin_to_hex(extract(bin, "10011010PPPPPbbb", 'P')) + ", " +
                  to_string(bin_to_int(extract(bin, "10011010PPPPPbbb", 'b')));
        }
        else if (match(bin, "10111PPrrrrrPPPP"))
        {
            cmd = "out " + bin_to_hex(extract(bin, "10111PPrrrrrPPPP", 'P')) + ", " +
                  get_reg(extract(bin, "10111PPrrrrrPPPP", 'r'));
        }
        else if (match(bin, "10110PPdddddPPPP"))
        {
            cmd = "in " + get_reg(extract(bin, "10110PPdddddPPPP", 'd')) + ", " +
                  bin_to_hex(extract(bin, "10110PPdddddPPPP", 'P'));
        }
        else if (match(bin, "1100kkkkkkkkkkkk"))
        {
            cmd = "rjmp " + get_addr_offset(extract(bin, "1100kkkkkkkkkkkk", 'k'));
        }
        else if (match(bin, "1110KKKKddddKKKK"))
        {
            cmd = "ldi " + get_reg(extract(bin, "1110KKKKddddKKKK", 'd'), 16) + ", " +
                  bin_to_hex(extract(bin, "1110KKKKddddKKKK", 'K'));
        }
        else if (match(bin, "111100kkkkkkk001"))
        {
            cmd = "breq " + get_addr_offset(extract(bin, "111100kkkkkkk001", 'k'));
        }
        else if (match(bin, "111101kkkkkkk001"))
        {
            cmd = "brne " + get_addr_offset(extract(bin, "111101kkkkkkk001", 'k'));
        }
        else if (is_double)
        {
            if (match(bin, "1001010kkkkk110kkkkkkkkkkkkkkkkk"))
            {
                cmd = "jmp " + bin_to_hex_shift(extract(bin, "1001010kkkkk110kkkkkkkkkkkkkkkkk", 'k'));
            }
            else if (match(bin, "1001010kkkkk111kkkkkkkkkkkkkkkkk"))
            {
                cmd = "call " + bin_to_hex_shift(extract(bin, "1001010kkkkk111kkkkkkkkkkkkkkkkk", 'k'));
            }
            else if (match(bin, "1001000ddddd0000kkkkkkkkkkkkkkkk"))
            {
                cmd = "lds " + get_reg(extract(bin, "1001000ddddd0000kkkkkkkkkkkkkkkk", 'd')) + ", " +
                      bin_to_hex(extract(bin, "1001000ddddd0000kkkkkkkkkkkkkkkk", 'k'));
            }
            else if (match(bin, "1001001rrrrr0000kkkkkkkkkkkkkkkk"))
            {
                cmd = "sts " + bin_to_hex(extract(bin, "1001001rrrrr0000kkkkkkkkkkkkkkkk", 'k')) + ", " +
                      get_reg(extract(bin, "1001001rrrrr0000kkkkkkkkkkkkkkkk", 'r'));
            }
            else
            {
                cmd = "[unknown double]";
            }
        }
        else
        {
            cmd = "[unknown]";
        }

        out << line << " " << cmd << endl;

        if (is_double && i + 1 < vec.size())
            i += 2;
        else
            i++;
    }
}

int main()
{
    string name = "Hex";

    ifstream in(name + ".txt");
    ofstream out(name + "_output.txt");

    if (!in.is_open() || !out.is_open())
    {
        cerr << "Error opening files" << endl;
        return 1;
    }

    vector<Record> records;
    string line;

    while (getline(in, line))
    {
        if (line.length() > 0)
            parse_line(records, line);
    }

    decode(out, records);

    in.close();
    out.close();

    cout << "Done, records: " << records.size() << endl;

    return 0;
}
