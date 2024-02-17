#include "Core.h"

Core *initCore(Instruction_Memory *i_mem)
{
    Core *core = (Core *)malloc(sizeof(Core));
    core->clk = 0;
    core->PC = 0;
    core->instr_mem = i_mem;
    core->tick = tickFunc;

    core->controlSigs = malloc(sizeof(ControlSignals)); 

    // FIXME, initialize register file here.
    // core->reg_file[0] = ...
    for(int x = 0; x < 32; x++){
        core->reg_file[x] = 0;
    }

    // FIXME, initialize data memory here.
    // core->data_mem[0] = ...
    for(int d = 0; d < 1024; d++){
        core->data_mem[d] = 0;
    }
    
    return core;
}

void printInstructionBinary(unsigned instruction){
    
    for (int i = 0; i < 32; ++i) {
        if (instruction >> (31-i) & 0x1) putchar('1');
        else putchar('0');
    }

    putchar('\n');
}
// FIXME, implement this function
bool tickFunc(Core *core)
{
    // Steps may include
    // (Step 1) Reading instruction from instruction memory
    unsigned instruction = core->instr_mem->instructions[core->PC / 4].instruction;
    unsigned instruction_addr = core->instr_mem->instructions[core->PC / 4].addr;

    

    // (Step 2) get control signals, fill read and write registers, generate immediate, get ALU control signal

    unsigned opcode = (instruction & 127);
    unsigned immediate = ImmeGen(instruction, opcode);
    unsigned rs2 = (instruction >> 20) & 31;
    unsigned rs1 = (instruction >> 15) & 31;
    unsigned rd = (instruction >> 7) & 15;
    unsigned funct3 = (instruction >> 12) & 7;
    unsigned funct7 = (instruction >> 25) & 127;

    printf("%d\n", opcode);
    ControlUnit(opcode, core->controlSigs);

    unsigned ALU_ctrl_signal = ALUControlUnit(core->controlSigs->ALUOp, funct7, funct3);

    
    printf("instruction:    ");
    printInstructionBinary(instruction);
    printf("opcode:         ");
    printInstructionBinary(opcode);
    printf("immediate:      ");
    printInstructionBinary(immediate);
    printf("rs2:            ");
    printInstructionBinary(rs2);
    printf("rs1:            ");
    printInstructionBinary(rs1);
    printf("rd:             ");
    printInstructionBinary(rd);
    printf("funct7:         ");
    printInstructionBinary(funct7);
    printf("funct3:         ");
    printInstructionBinary(funct3);
    printf("\n");

    
    // (Step N) Increment PC. FIXME, is it correct to always increment PC by 4?!

    //must be able to increment program counter from jump statements
    //jump statement process if beq in the ALU evaluates to true, and if there is a jump statement
    core->PC += 4;

    ++core->clk;
    // Are we reaching the final instruction?
    if (core->PC > core->instr_mem->last->addr)
    {
        return false;
    }
    return true;
}

// FIXME (1). Control Unit. Refer to Figure 4.18.
void ControlUnit(Signal input,
                 ControlSignals *signals)
{
    // For R-type
    if (input == 51)
    {   
        printf("R type\n");
        signals->ALUSrc = 0;
        signals->MemtoReg = 0;
        signals->RegWrite = 1;
        signals->MemRead = 0;
        signals->MemWrite = 0;
        signals->Branch = 0;
        signals->ALUOp = 2;
    }

    // For SB-type 
    if (input == 99)
    {
        printf("SB type\n");
        signals->ALUSrc = 0;
        signals->MemtoReg = 0;
        signals->RegWrite = 0;
        signals->MemRead = 0;
        signals->MemWrite = 0;
        signals->Branch = 1;
        signals->ALUOp = 1;
    }

    // For I-type
    if (input == 3)
    {
        printf("I type\n");
        signals->ALUSrc = 1;
        signals->MemtoReg = 1;
        signals->RegWrite = 1;
        signals->MemRead = 1;
        signals->MemWrite = 0;
        signals->Branch = 0;
        signals->ALUOp = 0;
    }

    // For I-type
    if (input == 19)
    {
        printf("I type\n");
        signals->ALUSrc = 1;
        signals->MemtoReg = 0;
        signals->RegWrite = 1;
        signals->MemRead = 1;
        signals->MemWrite = 0;
        signals->Branch = 0;
        signals->ALUOp = 0;
    }
}

// FIXME (2). ALU Control Unit. Refer to Figure 4.12.
Signal ALUControlUnit(Signal ALUOp,
                      Signal Funct7,
                      Signal Funct3)
{
    // For add
    if (ALUOp == 2 && Funct7 == 0 && Funct3 == 0)
    {
        return 2;
    }
    // For sub
    if (ALUOp == 2 && Funct7 == 32 && Funct3 == 0)
    {
        return 6;
    }
    // For and
    if (ALUOp == 2 && Funct7 == 0 && Funct3 == 7)
    {
        return 0;
    }
    // For or
    if (ALUOp == 2 && Funct7 == 0 && Funct3 == 6)
    {
        return 1;
    }
    // For ld (in book as lw)
    if (ALUOp == 0)
    {
        return 2;
    }
    // For bne (in book as beq)
    if (ALUOp == 1)
    {
        return 6;
    }
}

// FIXME (3). Imme. Generator
Signal ImmeGen(Signal input, Signal opcode)
{   
    unsigned immediate = 0;
    //I
    if (opcode == 3)  immediate = input >> 20;
    //SB
    if (opcode == 99) {
       
        unsigned imm12, imm10_5, imm4_1, imm11;
        imm12 = (input & 2147483648) >> 19;
        imm10_5 = (input & 2113929216) >> 20;
        imm4_1 = (input & 3840) >> 7;
        imm11 = (input & 128) << 4;
        immediate += imm4_1 + imm10_5 + imm11 + imm12;

    } 
    return immediate;
}

// FIXME (4). ALU
void ALU(Signal input_0,
         Signal input_1,
         Signal ALU_ctrl_signal,
         Signal *ALU_result,
         Signal *zero)
{
    // For addition
    if (ALU_ctrl_signal == 2)
    {
        *ALU_result = (input_0 + input_1);
        if (*ALU_result == 0) { *zero = 1; } else { *zero = 0; }
    }
}

// (4). MUX
Signal MUX(Signal sel,
           Signal input_0,
           Signal input_1)
{
    if (sel == 0) { return input_0; } else { return input_1; }
}

// (5). Add
Signal Add(Signal input_0,
           Signal input_1)
{
    return (input_0 + input_1);
}

// (6). ShiftLeft1
Signal ShiftLeft1(Signal input)
{
    return input << 1;
}
