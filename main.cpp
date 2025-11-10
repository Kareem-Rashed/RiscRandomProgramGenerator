#include <iostream>
#include <fstream>
using namespace std;
#include "Generator.h"







int main() {
    // I is for RV32I, 10 is number of instructions, 'I' is format
    //possible formats: R, I, S, B, U, J
    //will add C later
    Generator gen('I', 10, 'B');


    //gen.Start();  //for single format  (change from constructor)
    gen.GenerateAllBType();//for mixed formats
    gen.GenerateTCFiles();//for test case file
    gen.GenerateMem();//for mixed formats
    return 0;
}