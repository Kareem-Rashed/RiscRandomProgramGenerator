#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <cctype>
#include <vector>
using namespace std;
#include "Generator.h"

static string toUpper(string s) {
    transform(s.begin(), s.end(), s.begin(), [](unsigned char c){ return (char)toupper(c); });
    return s;
}

static char decoder(const string &fmt)
{
    // to avoid confusion with SYS and Mixed Instructions Modes
    string f = toUpper(fmt);
    if (f == "MIXED" || f == "MIXEDSET") return 'M';
    if (f == "SYS") return 'Y';
    if (f.size() >= 1) {
        char c = f[0];
        switch (c) {
            case 'R': return 'R';
            case 'I': return 'I';
            case 'S': return 'S';
            case 'B': return 'B';
            case 'U': return 'U';
            case 'J': return 'J';
            case 'M': return 'M';
            case 'Y': return 'Y';
            default: return '\0';
        }
    }
    return '\0';
}


int main(int argc, char **argv) {

    string mode;
    int count = 16; //default count

    if (argc >= 3) {
        mode = string(argv[1]);
        try { count = stoi(string(argv[2])); } catch (...) { count = 16; }
    } else {
        // In case user didnt provide enough arguments
        cout << "RISC Random Program Generator - minimal mode\n";
        cout << "Enter MODE (R,I,S,B,U,J,M,ALL): ";
        if (!getline(cin, mode)) return 1;
        string Scount;
        cout << "Enter number of instructions: ";
        if (!getline(cin, Scount)) return 1;
        try { count = stoi(Scount); } catch (...) { count = 16; }
    }

    string modeUC = toUpper(mode);
    if (modeUC == "ALL") {
        vector<char> allFormats = {'R','I','S','B','U','J'};
        for (char fmt : allFormats) {
            cout << "[DEBUG] Constructing Generator with type='I', count=" << count << ", format='" << fmt << "'\n";
            Generator gen('I', count, fmt);
            gen.GenerateTCFiles();
            gen.GenerateMem();
            cout << "Processed format " << fmt << " with " << count << " instructions.\n";
        }
        return 0;
    }

    char fmtChar = decoder(mode);
    if (fmtChar == '\0') {
        cout << "Invalid MODE '" << mode << "'. Valid: R,I,S,B,U,J,M,ALL\n";
        return 1;
    }

    cout << "[DEBUG] Resolved mode '" << mode << "' -> format '" << fmtChar << "'\n";
    cout << "[DEBUG] Constructing Generator with type='I', count=" << count << ", format='" << fmtChar << "'\n";

    Generator gen('I', count, fmtChar);
    gen.GenerateTCFiles();
    gen.GenerateMem();
    cout << "Processed mode " << mode << " with " << count << " instructions.\n";


    return 0;
}