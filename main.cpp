#include <iostream>
#include "Generator.h"

int main() {
    // I is for RV32I, 10 is number of instructions, 'I' is format
    //possible formats: R, I, S, B, U, J
    //will add C later
    Generator gen('I', 10, 'I');
    gen.Start();
    return 0;
}