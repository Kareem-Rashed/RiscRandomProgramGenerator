//
// Created by Kareem Rashed on 07/11/2025.
//

#ifndef GENERATOR_H
#define GENERATOR_H
#include <string>
#include <utility>
#include <random>
#include <vector>

using namespace std;


class Generator {
private:
    char type;   //RV32 I or C
    int NumofInstructions;
    char Format; //R, I, S, B, U, J , Y for SYS (ECALL, EBREAK, FENCE, FENCE.TSO, PAUSE)

    // random number generator thats better than rand() and we can use it to get negatives
    std::mt19937 rng;

    pair<string,string> generateR();
    pair<string,string> generateI();
    pair<string,string> generateS();
    pair<string,string> generateB();
    pair<string,string> generateU();
    pair<string,string> generateJ();
    pair<string,string>generateSYS();


public:
    Generator(char type, int NumofInstructions, char Format);
    void Start();
    void StartMixed();

};


#endif //GENERATOR_H
