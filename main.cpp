#include <iostream>
#include "Generator.h"

int main() {
    // I is for RV32I, 10 is number of instructions, 'I' is format
    //possible formats: R, I, S, B, U, J
    //will add C later
    Generator gen('I', 10, 'R');


    //gen.Start();  //for single format  (change from constructor)

    //gen.StartMixed();  //for mixed formats
    return 0;
}