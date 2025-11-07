#include <iostream>
#include "Generator.h"

int main() {
    // Generate and print 10 random R-format RV32I instructions
    Generator gen('I', 10, 'I');
    gen.Start();
    return 0;
}