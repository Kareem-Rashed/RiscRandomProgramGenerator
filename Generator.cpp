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
        {{0b000, "1100111"}, "jalr"},
        {{0b000, "0000011"}, "lb"},
        {{0b001, "0000011"}, "lh"},
        {{0b010, "0000011"}, "lw"},
        {{0b100, "0000011"}, "lbu"},
        {{0b101, "0000011"}, "lhu"}
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

    // For regular I-type, jalr, and loads
    uint32_t imm_u = static_cast<uint32_t>(imm) & 0xFFFu;
    string binary = bitset<12>(imm_u).to_string() +
                   bitset<5>(rs1).to_string() +
                   bitset<3>(funct3).to_string() +
                   bitset<5>(rd).to_string() +
                   opcode;

    string assembly;
    if (instr_name == "jalr" || instr_name == "lb" || instr_name == "lh" ||
        instr_name == "lw" || instr_name == "lbu" || instr_name == "lhu") {
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
pair<string,string> Generator::generateSYS() {
    string instr_name1 = "ECALL";
    string intsr_binary1 = "00000000000000000000000001110011";
    string instr_name2 = "EBREAK";
    string instr_binary2 = "00000000000100000000000001110011";
    string instr_name3 = "FENCE";
    string instr_binary3 = "00000011001100000000000000001111";
    string instr_name4 = "PAUSE";
    string instr_binary4 = "00000011001100000000000000001111";
    string instr_name5 = "FENCE.TSO";
    string instr_binary5 = "10000011001100000000000000001111";
    vector <pair<string,string>> sys_instructions = {{instr_name1,intsr_binary1}, {instr_name2,instr_binary2},{ instr_name3,instr_binary3}, {instr_name4,instr_binary4}, {instr_name5,instr_binary5}};
    std::uniform_int_distribution<int> pick(0, (int)sys_instructions.size() - 1);
    int id = pick(rng);
    pair <string,string> selected_instr = sys_instructions[id];

return {selected_instr.second, selected_instr.first};
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
            case 'Y': instr = generateSYS(); break;
            default: instr = generateR(); break;
        }
        cout << "mem[" << i << "] = " << "32'b"<<instr.first << "    // " << instr.second << endl; //formated for vivado
    }
}

void Generator::StartMixed() {
    vector<char> formats = {'R', 'I', 'S', 'B', 'U', 'J', 'Y'};
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
            case 'Y': instr = generateSYS(); break;
            default: instr = generateR(); break;
        }
        cout << "mem[" << i << "] =  32'b" << instr.first << ";    // " << instr.second << endl; //formated for vivado
    }
}





void Generator::GenerateMixedSet()
{
    generatedInstructions.clear();
    vector<char> formats = {'R', 'I', 'S', 'B', 'U', 'J'};
    std::uniform_int_distribution<int> pick(0, (int)formats.size() - 1);

    for (int i = 0; i < NumofInstructions-1; ++i)
    {
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
        generatedInstructions.push_back(instr);
    }

    pair<string, string> sysInstr = generateSYS(); // Ensure  one SYS instruction at the end
    generatedInstructions.push_back(sysInstr);
}

void Generator::GenerateTCFiles() {
    switch(Format)
    {
        case 'R': GenerateAllRType(); break;
            case 'I': GenerateAllIType(); break;
            case 'S': GenerateAllSType(); break;
            case 'B': GenerateAllBType(); break;
            case 'U': GenerateAllUType(); break;
            case 'J': GenerateAllJType(); break;
            case 'M': GenerateMixedSet(); break;
            default: GenerateAllRType(); break;
    }

    string filename = "../TestCases/TC-" + string(1, Format) + ".txt";
    ofstream ofs(filename);

    if (ofs.is_open()) {
        cout << "Opened " << filename << endl;
    }

    int byteAddr = 0;  //for mem address
    for (int i = 0; i < generatedInstructions.size(); ++i) {
        string instruction = generatedInstructions[i].first;
        string assembly = generatedInstructions[i].second;

        // Split 32-bit instruction into 4 bytes

        string byte0 = instruction.substr(24, 8);  // bits [7:0]
        string byte1 = instruction.substr(16, 8);  // bits [15:8]
        string byte2 = instruction.substr(8, 8);   // bits [23:16]
        string byte3 = instruction.substr(0, 8);   // bits [31:24]

        // Output bytes in little-endian order with memory addresses
        ofs << "mem[" << byteAddr++ << "] = 8'b" << byte0 << "; // " << assembly << " [byte 1]" << endl;
        ofs << "mem[" << byteAddr++ << "] = 8'b" << byte1 << "; // " << " [byte 2]" << endl;
        ofs << "mem[" << byteAddr++ << "] = 8'b" << byte2 << "; // " << " [byte 3]" << endl;
        ofs << "mem[" << byteAddr++ << "] = 8'b" << byte3 << "; // " << " [byte 4]" << endl;
    }

    ofs.close();

}


void Generator::GenerateMem() {
    string filename = "../MemData/Mem-" + string(1, Format) + ".txt";
    ofstream ofs(filename);

    if (ofs.is_open()) {
        cout << "Opened " << filename << endl;
    }

    for (int i = 0; i < generatedInstructions.size(); ++i) {
        string instruction = generatedInstructions[i].first;

        // Split 32-bit instruction into 4 bytes using little endian

        string byte0 = instruction.substr(24, 8);  // bits [7:0]
        string byte1 = instruction.substr(16, 8);  // bits [15:8]
        string byte2 = instruction.substr(8, 8);   // bits [23:16]
        string byte3 = instruction.substr(0, 8);   // bits [31:24]

        //Output bytes
        ofs << byte0 << endl;
        ofs << byte1 << endl;
        ofs << byte2 << endl;
        ofs << byte3 << endl;
    }
    ofs.close();

}


void Generator::GenerateAllRType() {
    generatedInstructions.clear();

    struct RTypeSpec {
        string name;
        string funct7;
        string funct3;
        string opcode;
    };

    vector<RTypeSpec> r_instructions = {
        {"add", "0000000", "000", "0110011"},
        {"sub", "0100000", "000", "0110011"},
        {"sll", "0000000", "001", "0110011"},
        {"slt", "0000000", "010", "0110011"},
        {"sltu","0000000", "011", "0110011"},
        {"xor", "0000000", "100", "0110011"},
        {"srl", "0000000", "101", "0110011"},
        {"sra", "0100000", "101", "0110011"},
        {"or",  "0000000", "110", "0110011"},
        {"and", "0000000", "111", "0110011"},
    };

    struct ITypeSpec
    {
        string name;
string imm;
        string rs1;
        string funct3;
        string rd;
        string opcode;

    };

  // To initialize registers used
    vector<ITypeSpec> StartingInstructions= {
        {"addi", "000000000010", "00000", "000", "00010","0010011"}, // x2 = 3
{"addi", "000000000011", "00000", "000", "00011","0010011"}, // x3 = 2
};
    for (auto&s : StartingInstructions) {

        string bin = s.imm + s.rs1 + s.funct3 + s.rd + s.opcode;

        string asmcode = s.name + " x" + to_string(stoi(s.rd, nullptr, 2)) + ", x" + to_string(stoi(s.rs1, nullptr, 2)) + ", " + to_string(stoi(s.imm, nullptr, 2));
        generatedInstructions.push_back({bin, asmcode});
    }


    for (auto &r : r_instructions) {
        // dummy values for rd, rs1, rs2
        string rd = "00001";   // x1
        string rs1 = "00010";  // x2
        string rs2 = "00011";  // x3

        //           funct7 | rs2 | rs1 | funct3 | rd | opcode
        string bin = r.funct7 + rs2 + rs1 + r.funct3 + rd + r.opcode;

        string asmString = r.name + " x1, x2, x3";
        generatedInstructions.push_back({bin, asmString});
    }


    generatedInstructions.push_back(generateSYS());
}

void Generator::GenerateAllUType() {
    generatedInstructions.clear();

    struct USpec {
        string name;
        string opcode;
    };

    vector<USpec> u_instructions = {
        {"lui",   "0110111"},
        {"auipc", "0010111"}
    };

    string rd = "00001";  // x1
    string imm = "00000000000000000001";  // imm = 1

    for (auto &u : u_instructions) {
                  // imm[31:12] + rd + opcode
        string bin = imm + rd + u.opcode;
        string asmcode = u.name + " x1, 0x1";
        generatedInstructions.push_back({bin, asmcode});
    }


    generatedInstructions.push_back(generateSYS());
}
void Generator::GenerateAllJType() {
    generatedInstructions.clear();

    struct JSpec {
        string name;
        string opcode;
    };

    vector<JSpec> j_instructions = {
        {"jal", "1101111"}
    };

    string rd = "00001";  // x1
                // imm[20|10:1|11|19:12]
    string imm = "00000000100000000000"; // imm = 8
    struct ITypeSpec
    {
        string name;
        string imm;
        string rs1;
        string funct3;
        string rd;
        string opcode;

    };

    // To be skipped (jumping)
    vector<ITypeSpec> StartingInstructions= {
        {"addi", "000000000011", "00000", "000", "00010", "0010011"},
        {"addi", "000000000010", "00000", "000","00011","0010011"}
    };

    for (auto &j : j_instructions) {
        // Build J-type layout: imm[20|10:1|11|19:12] + rd + opcode
        string bin = imm + rd + j.opcode;
        string asmcode = j.name + " x1, 8";
        generatedInstructions.push_back({bin, asmcode});
    }
    for (auto&s : StartingInstructions) {

        string bin = s.imm + s.rs1 + s.funct3 + s.rd + s.opcode;

        string asmcode = s.name + " x" + to_string(stoi(s.rd, nullptr, 2)) + ", x" + to_string(stoi(s.rs1, nullptr, 2)) + ", " + to_string(stoi(s.imm, nullptr, 2));
        generatedInstructions.push_back({bin, asmcode});
    }


    generatedInstructions.push_back(generateSYS());
}


void Generator::GenerateAllIType() {
    generatedInstructions.clear();
    struct ITypeSpec {
        string name;
        string funct3;
        string opcode;
    };
    vector<ITypeSpec> i_instructions = {
        {"addi", "000", "0010011"},
        {"slti", "010", "0010011"},
        {"sltiu", "011", "0010011"},
        {"xori", "100", "0010011"},
        {"ori", "110", "0010011"},
        {"andi", "111", "0010011"},
        {"lb", "000", "0000011"},
        {"lh", "001", "0000011"},
        {"lw", "010", "0000011"},
        {"lbu", "100", "0000011"},
        {"lhu", "101", "0000011"},
        {"jalr", "000", "1100111"},
    };

    string rd = "00001";  // x1
    string rs1 = "00010"; // x2
    string imm = "000000000001"; // 12-bit immediate = 1

    struct STypeSpec
    {
        string name;
        string imm;
        string rs1;
        string funct3;
        string rd;
        string opcode;

    };


    vector<STypeSpec> StartingInstructions= {
        {"addi", "000000001000", "00000", "000", "00010", "0010011"},
        {"addi", "000000000011", "00000", "000","00011","0010011"}
    };
    for (auto&s : StartingInstructions) {

        string bin = s.imm + s.rs1 + s.funct3 + s.rd + s.opcode;
        string asmcode = s.name + " x" + to_string(stoi(s.rd, nullptr, 2)) + ", x" + to_string(stoi(s.rs1, nullptr, 2)) + ", " + to_string(stoi(s.imm, nullptr, 2));
        generatedInstructions.push_back({bin, asmcode});
    }





    for (auto &i : i_instructions) {
        // I-type: imm[11:0] | rs1[4:0] | funct3[2:0] | rd[4:0] | opcode[6:0]
        string bin = imm + rs1 + i.funct3 + rd + i.opcode;

        string asmcode;
        if (i.name == "lb" || i.name == "lh" || i.name == "lw" || i.name == "lbu" || i.name == "lhu" || i.name == "jalr") {
            // Load/jalr format
            asmcode = i.name + " x1, 1(x2)";
        } else {
            // Immediate format: instr rd, rs1, imm
            asmcode = i.name + " x1, x2, 1";
        }

        generatedInstructions.push_back({bin, asmcode});
    }
}

void Generator::GenerateAllBType() {
    generatedInstructions.clear();
    struct BTypeSpec {
        string name;
        string funct3;
        string opcode;
    };
    vector<BTypeSpec> b_instructions = {
        {"beq", "000", "1100011"},
        {"bne", "001", "1100011"},
        {"blt", "100", "1100011"},
        {"bge", "101", "1100011"},
        {"bltu", "110", "1100011"},
        {"bgeu", "111", "1100011"},
    };

    string rs1 = "00001"; // x1
    string rs2 = "00010"; // x2
    string imm = "000000000100"; // 12-bit immediate = 4 (for simplicity, as branches use offsets)

    for (auto &b : b_instructions) {
        // B-type: imm[12] imm[10:5] rs2 rs1 funct3 imm[4:1] imm[11] opcode
        string imm_12 = "0";
        string imm_10_5 = "000000";
        string imm_4_1 = "0010";
        string imm_11 = "0";
        string bin = imm_12 + imm_10_5 + rs2 + rs1 + b.funct3 + imm_4_1 + imm_11 + b.opcode;

        string asmcode = b.name + " x1, x2, 4";
        generatedInstructions.push_back({bin, asmcode});
    }
}

void Generator::GenerateAllSType() {
    generatedInstructions.clear();
    struct STypeSpec {
        string name;
        string funct3;
        string opcode;
    };
    vector<STypeSpec> s_instructions = {
        {"sb", "000", "0100011"},
        {"sh", "001", "0100011"},
        {"sw", "010", "0100011"},
    };

    string rs1 = "00001"; // x1
    string rs2 = "00010"; // x2
    string imm = "000000000100"; // 12-bit immediate = 4

    struct ITypeSpec
    {
        string name;
        string imm;
        string rs1;
        string funct3;
        string rd;
        string opcode;

    };

    // To initialize registers used stored large Immediates to see differences in sw, sb, sh
    vector<ITypeSpec> StartingInstructions= {
        {"addi", "001000111111", "00000", "000", "00010", "0010011"},
        {"addi", "000100000010", "00000", "000","00011","0010011"}
    };
    for (auto&s : StartingInstructions) {

        string bin = s.imm + s.rs1 + s.funct3 + s.rd + s.opcode;
        string asmcode = s.name + " x" + to_string(stoi(s.rd, nullptr, 2)) + ", x" + to_string(stoi(s.rs1, nullptr, 2)) + ", " + to_string(stoi(s.imm, nullptr, 2));
        generatedInstructions.push_back({bin, asmcode});
    }


    for (auto &s : s_instructions) {
        // S-type: imm[11:5] rs2 rs1 funct3 imm[4:0] opcode
        string imm_11_5 = "0000000";
        string imm_4_0 = "00100";
        string bin = imm_11_5 + rs2 + rs1 + s.funct3 + imm_4_0 + s.opcode;

        string asmcode = s.name + " x2, 4(x1)";
        generatedInstructions.push_back({bin, asmcode});
    }
}

