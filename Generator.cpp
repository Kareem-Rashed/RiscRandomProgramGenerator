//
// Created by Kareem Rashed on 07/11/2025.
//

#include "Generator.h"
#include <iostream>

Generator::Generator(char type, int NumofInstructions, char Format)
: type(type), NumofInstructions(NumofInstructions), Format(Format) {}

pair<string,string> Generator::generateR() {
    // Select random registers and function bits
    int rd = rand() % 32;     // Random register 0-31
    int rs1 = rand() % 32;    // Random register 0-31
    int rs2 = rand() % 32;    // Random register 0-31
    int funct3 = rand() % 8;      // Random funct3 0-7

    // R-format: common R-type instructions
    vector<pair<int, string>> FUNC7 = {
        {0b0000000, "add"},
        {0b0100000, "sub"},
        {0b0000000, "and"},
        {0b0000000, "or"},
        {0b0000000, "xor"},
        {0b0000000, "sll"},
        {0b0000000, "srl"},
        {0b0100000, "sra"}
    };

    // Select random instruction
    int id = rand() % FUNC7.size();
    int funct7 = FUNC7[id].first;
    string instr_name = FUNC7[id].second;

    // Create binary string (R-format: funct7 | rs2 | rs1 | funct3 | rd | opcode)
    string binary =
        to_string(funct7) +
        bitset<5>(rs2).to_string() +
        bitset<5>(rs1).to_string() +
        bitset<3>(funct3).to_string() +
        bitset<5>(rd).to_string() +
        "0110011"; // R-type opcode

    // Create assembly string
    string assembly = instr_name + " x" + to_string(rd) + ", x" +
                     to_string(rs1) + ", x" + to_string(rs2);

    return {binary, assembly};
}




pair<string,string> Generator::generateI() {
    int rd = rand() % 32;    // Destination register
    int rs1 = rand() % 32;   // Source register
    int imm = rand() % 4096; // 12-bit immediate

    vector<pair<int, string>> I_TYPE = {
        {0b000, "addi"},
        {0b111, "andi"},
        {0b110, "ori"},
        {0b100, "xori"},
        {0b001, "slli"},
        {0b101, "srli"}
    };

    int id = rand() % I_TYPE.size();
    int funct3 = I_TYPE[id].first;
    string instr_name = I_TYPE[id].second;

    string binary = bitset<12>(imm).to_string() +
                   bitset<5>(rs1).to_string() +
                   bitset<3>(funct3).to_string() +
                   bitset<5>(rd).to_string() +
                   "0010011";  // I-type opcode

    string assembly = instr_name + " x" + to_string(rd) + ", x" +
                     to_string(rs1) + ", " + to_string(imm);

    return {binary, assembly};
}

pair<string,string> Generator::generateS() {
    int rs1 = rand() % 32;    // Base register
    int rs2 = rand() % 32;    // Source register
    int imm = rand() % 4096;  // 12-bit immediate

    vector<pair<int, string>> S_TYPE = {
        {0b000, "sb"},
        {0b001, "sh"},
        {0b010, "sw"}
    };

    int id = rand() % S_TYPE.size();
    int funct3 = S_TYPE[id].first;
    string instr_name = S_TYPE[id].second;

    string imm_11_5 = bitset<7>(imm >> 5).to_string();
    string imm_4_0 = bitset<5>(imm & 0x1F).to_string();

    string binary = imm_11_5 +
                   bitset<5>(rs2).to_string() +
                   bitset<5>(rs1).to_string() +
                   bitset<3>(funct3).to_string() +
                   imm_4_0 +
                   "0100011";  // S-type opcode

    string assembly = instr_name + " x" + to_string(rs2) + ", " +
                     to_string(imm) + "(x" + to_string(rs1) + ")";

    return {binary, assembly};
}

pair<string,string> Generator::generateB() {
    int rs1 = rand() % 32;     // First source register
    int rs2 = rand() % 32;     // Second source register
    int imm = (rand() % 4096) * 2;  // 13-bit immediate (multiple of 2)

    vector<pair<int, string>> B_TYPE = {
        {0b000, "beq"},
        {0b001, "bne"},
        {0b100, "blt"},
        {0b101, "bge"},
        {0b110, "bltu"},
        {0b111, "bgeu"}
    };

    int id = rand() % B_TYPE.size();
    int funct3 = B_TYPE[id].first;
    string instr_name = B_TYPE[id].second;

    int imm_12 = (imm >> 12) & 0x1;
    int imm_11 = (imm >> 11) & 0x1;
    int imm_10_5 = (imm >> 5) & 0x3F;
    int imm_4_1 = (imm >> 1) & 0xF;

    string binary = to_string(imm_12) +
                   bitset<6>(imm_10_5).to_string() +
                   bitset<5>(rs2).to_string() +
                   bitset<5>(rs1).to_string() +
                   bitset<3>(funct3).to_string() +
                   bitset<4>(imm_4_1).to_string() +
                   to_string(imm_11) +
                   "1100011";  // B-type opcode

    string assembly = instr_name + " x" + to_string(rs1) + ", x" +
                     to_string(rs2) + ", " + to_string(imm);

    return {binary, assembly};
}

pair<string,string> Generator::generateU() {
    int rd = rand() % 32;      // Destination register
    int imm = rand() % (1 << 20);  // 20-bit immediate

    vector<pair<string, string>> U_TYPE = {
        {"0110111", "lui"},
        {"0010111", "auipc"}
    };

    int id = rand() % U_TYPE.size();
    string opcode = U_TYPE[id].first;
    string instr_name = U_TYPE[id].second;

    string binary = bitset<20>(imm).to_string() +
                   bitset<5>(rd).to_string() +
                   opcode;

    string assembly = instr_name + " x" + to_string(rd) + ", " +
                     to_string(imm);

    return {binary, assembly};
}

pair<string,string> Generator::generateJ() {
    int rd = rand() % 32;      // Destination register
    int imm = (rand() % (1 << 20)) * 2;  // 21-bit immediate (multiple of 2)

    string instr_name = "jal";  // Only JAL for J-format

    int imm_20 = (imm >> 20) & 0x1;
    int imm_19_12 = (imm >> 12) & 0xFF;
    int imm_11 = (imm >> 11) & 0x1;
    int imm_10_1 = (imm >> 1) & 0x3FF;

    string binary = to_string(imm_20) +
                   bitset<10>(imm_10_1).to_string() +
                   to_string(imm_11) +
                   bitset<8>(imm_19_12).to_string() +
                   bitset<5>(rd).to_string() +
                   "1101111";  // J-type opcode

    string assembly = instr_name + " x" + to_string(rd) + ", " +
                     to_string(imm);

    return {binary, assembly};
}