//
// Created by Kareem Rashed on 07/11/2025.
//

#include "Generator.h"
#include <iostream>
#include <bitset>

using namespace std;

Generator::Generator(char type, int NumofInstructions, char Format)
    : type(type), NumofInstructions(NumofInstructions), Format(Format) {
    std::random_device rd;
    rng.seed(rd());
}

pair<string,string> Generator::generateR() {
    std::uniform_int_distribution<int> regDist(0, 31);
    int rd = regDist(rng);
    int rs1 = regDist(rng);
    int rs2 = regDist(rng);

    // R-format: (funct7, funct3, name)
    vector<tuple<int, int, string>> R_INSTRUCTIONS = {
        {0b0000000, 0b000, "add"},
        {0b0100000, 0b000, "sub"},
        {0b0000000, 0b111, "and"},
        {0b0000000, 0b110, "or"},
        {0b0000000, 0b100, "xor"},
        {0b0000000, 0b001, "sll"},
        {0b0000000, 0b101, "srl"},
        {0b0100000, 0b101, "sra"}
    };

    std::uniform_int_distribution<int> pick(0, (int)R_INSTRUCTIONS.size() - 1);
    int id = pick(rng);
    int funct7 = get<0>(R_INSTRUCTIONS[id]);
    int funct3 = get<1>(R_INSTRUCTIONS[id]);
    string instr_name = get<2>(R_INSTRUCTIONS[id]);

    // Create binary string (R-format: funct7 | rs2 | rs1 | funct3 | rd | opcode)
    string binary =
        bitset<7>(funct7).to_string() +
        bitset<5>(rs2).to_string() +
        bitset<5>(rs1).to_string() +
        bitset<3>(funct3).to_string() +
        bitset<5>(rd).to_string() +
        "0110011";

    string assembly = instr_name + " x" + to_string(rd) + ", x" +
                     to_string(rs1) + ", x" + to_string(rs2);

    return {binary, assembly};
}
pair<string,string> Generator::generateI() {
    std::uniform_int_distribution<int> regDist(0, 31);
    int rd = regDist(rng);
    int rs1 = regDist(rng);
    // signed 12-bit immediate: -2048..2047
    std::uniform_int_distribution<int> immDist(-2048, 2047);
    int imm = immDist(rng);

    vector<pair<pair<int, string>, string>> I_TYPE = {
        {{0b000, "0010011"}, "addi"},
        {{0b111, "0010011"}, "andi"},
        {{0b110, "0010011"}, "ori"},
        {{0b100, "0010011"}, "xori"},
        {{0b001, "0010011"}, "slli"},
        {{0b101, "0010011"}, "srli"},
        {{0b000, "1100111"}, "jalr"}  // Added JALR instruction
    };

    std::uniform_int_distribution<int> pick(0, (int)I_TYPE.size() - 1);
    int id = pick(rng);
    int funct3 = I_TYPE[id].first.first;
    string opcode = I_TYPE[id].first.second;
    string instr_name = I_TYPE[id].second;

    if (instr_name == "slli" || instr_name == "srli") {
        // shift amount is 0..31
        std::uniform_int_distribution<int> sh(0, 31);
        int shamt = sh(rng);
        string binary = bitset<7>(0).to_string() +
                       bitset<5>(shamt).to_string() +
                       bitset<5>(rs1).to_string() +
                       bitset<3>(funct3).to_string() +
                       bitset<5>(rd).to_string() +
                       opcode;
        string assembly = instr_name + " x" + to_string(rd) + ", x" + to_string(rs1) + ", " + to_string(shamt);
        return {binary, assembly};
    }

    // For regular I-type and jalr
    uint32_t imm_u = static_cast<uint32_t>(imm) & 0xFFFu;
    string binary = bitset<12>(imm_u).to_string() +
                   bitset<5>(rs1).to_string() +
                   bitset<3>(funct3).to_string() +
                   bitset<5>(rd).to_string() +
                   opcode;

    string assembly;
    if (instr_name == "jalr") {
        assembly = instr_name + " x" + to_string(rd) + ", " + to_string(imm) + "(x" + to_string(rs1) + ")";
    } else {
        assembly = instr_name + " x" + to_string(rd) + ", x" + to_string(rs1) + ", " + to_string(imm);
    }

    return {binary, assembly};
}

pair<string,string> Generator::generateS() {
    std::uniform_int_distribution<int> regDist(0, 31);
    int rs1 = regDist(rng);
    int rs2 = regDist(rng);
    // signed 12-bit immediate
    std::uniform_int_distribution<int> immDist(-2048, 2047);
    int imm = immDist(rng);
    uint32_t imm12 = static_cast<uint32_t>(imm) & 0xFFFu;

    vector<pair<int, string>> S_TYPE = {
        {0b000, "sb"},
        {0b001, "sh"},
        {0b010, "sw"}
    };

    std::uniform_int_distribution<int> pick(0, (int)S_TYPE.size() - 1);
    int id = pick(rng);
    int funct3 = S_TYPE[id].first;
    string instr_name = S_TYPE[id].second;

    string imm_11_5 = bitset<7>((imm12 >> 5) & 0x7F).to_string();
    string imm_4_0 = bitset<5>(imm12 & 0x1F).to_string();

    string binary = imm_11_5 +
                   bitset<5>(rs2).to_string() +
                   bitset<5>(rs1).to_string() +
                   bitset<3>(funct3).to_string() +
                   imm_4_0 +
                   "0100011";  // S-type opcode

    string assembly = instr_name + " x" + to_string(rs2) + ", " + to_string(imm) + "(x" + to_string(rs1) + ")";

    return {binary, assembly};
}

pair<string,string> Generator::generateB() {
    std::uniform_int_distribution<int> regDist(0, 31);
    int rs1 = regDist(rng);
    int rs2 = regDist(rng);
    // branch offset is 13 bits (imm/2 typically); generate k in -2048..2047 then imm = k*2
    std::uniform_int_distribution<int> kDist(-2048, 2047);
    int k = kDist(rng);
    int imm = k * 2;
    uint32_t imm13 = static_cast<uint32_t>(imm) & 0x1FFFu;

    vector<pair<int, string>> B_TYPE = {
        {0b000, "beq"},
        {0b001, "bne"},
        {0b100, "blt"},
        {0b101, "bge"},
        {0b110, "bltu"},
        {0b111, "bgeu"}
    };

    std::uniform_int_distribution<int> pick(0, (int)B_TYPE.size() - 1);
    int id = pick(rng);
    int funct3 = B_TYPE[id].first;
    string instr_name = B_TYPE[id].second;

    uint32_t imm_12 = (imm13 >> 12) & 0x1u;
    uint32_t imm_11 = (imm13 >> 11) & 0x1u;
    uint32_t imm_10_5 = (imm13 >> 5) & 0x3Fu;
    uint32_t imm_4_1 = (imm13 >> 1) & 0xFu;

    string binary = to_string(imm_12) +
                   bitset<6>(imm_10_5).to_string() +
                   bitset<5>(rs2).to_string() +
                   bitset<5>(rs1).to_string() +
                   bitset<3>(funct3).to_string() +
                   bitset<4>(imm_4_1).to_string() +
                   to_string(imm_11) +
                   "1100011";  // B-type opcode

    string assembly = instr_name + " x" + to_string(rs1) + ", x" + to_string(rs2) + ", " + to_string(imm);

    return {binary, assembly};
}

pair<string,string> Generator::generateU() {
    std::uniform_int_distribution<int> regDist(0, 31);
    int rd = regDist(rng);

    //Generate immediate (-524288 to 524287)
std::uniform_int_distribution<int> immDist(-524288, 524287);    int imm = immDist(rng);

    //selection between LUI and AUIPC
    bool isLui = rng() % 2 == 0;
    string opcode = isLui ? "0110111" : "0010111";
    string name = isLui ? "lui" : "auipc";

    // Format binary (20 bits immediate, maintain sign)
    string binary = bitset<20>(imm & 0xFFFFF).to_string() +
                   bitset<5>(rd).to_string() +
                   opcode;

    // Format assembly showing signed immediate
    string assembly = name + " x" + to_string(rd) + ", " + to_string(imm);

    return {binary, assembly};
}
pair<string,string> Generator::generateJ() {
    std::uniform_int_distribution<int> regDist(0, 31);
    int rd = regDist(rng);

    // Generate signed 20-bit immediate (range -524288 .. 524287)
    std::uniform_int_distribution<int> immDist(-524288, 524287);
    int imm = immDist(rng);

    // The immediate represents bits [20:1] of the offset (bit 0 is implicit 0)
    uint32_t offset = static_cast<uint32_t>(imm << 1); // Shift to get actual offset

    string instr_name = "jal";

    // Extract fields from the offset
    uint32_t imm_20 = (offset >> 20) & 0x1u;        // bit 20
    uint32_t imm_19_12 = (offset >> 12) & 0xFFu;    // bits 19:12
    uint32_t imm_11 = (offset >> 11) & 0x1u;        // bit 11
    uint32_t imm_10_1 = (offset >> 1) & 0x3FFu;     // bits 10:1

    // Build binary in J-format field order: imm[20] imm[10:1] imm[11] imm[19:12] rd opcode
    string binary = to_string(imm_20) +
                   bitset<10>(imm_10_1).to_string() +
                   to_string(imm_11) +
                   bitset<8>(imm_19_12).to_string() +
                   bitset<5>(rd).to_string() +
                   "1101111";

    string assembly = instr_name + " x" + to_string(rd) + ", " + to_string(imm << 1);

    return {binary, assembly};
}


void Generator::Start() {
    for (int i = 0; i < NumofInstructions; ++i) {
        pair<string,string> instr;
        switch (Format) {
            case 'R': instr = generateR(); break;
            case 'I': instr = generateI(); break;
            case 'S': instr = generateS(); break;
            case 'B': instr = generateB(); break;
            case 'U': instr = generateU(); break;
            case 'J': instr = generateJ(); break;
            default: instr = generateR(); break;
        }
        cout << "Mem[" << i << "] = " << instr.first << "    // " << instr.second << endl; //formated for vivado
    }
}

void Generator::StartMixed() {
    vector<char> formats = {'R', 'I', 'S', 'B', 'U', 'J'};
    std::uniform_int_distribution<int> pick(0, (int)formats.size() - 1);

    for (int i = 0; i < NumofInstructions; ++i) {
        char format = formats[pick(rng)];
        pair<string,string> instr;
        switch (format) {
            case 'R': instr = generateR(); break;
            case 'I': instr = generateI(); break;
            case 'S': instr = generateS(); break;
            case 'B': instr = generateB(); break;
            case 'U': instr = generateU(); break;
            case 'J': instr = generateJ(); break;
            default: instr = generateR(); break;
        }
        cout << "Mem[" << i << "] = 1'b" << instr.first << ";    // " << instr.second << endl; //formated for vivado
    }
}