#include "Core.h"

Core *initCore(Instruction_Memory *i_mem)
{
    Core *core = (Core *)malloc(sizeof(Core));
    core->clk = 0;
    core->PC = 0;
    core->instr_mem = i_mem;
    core->tick = tickFunc;

    core->controlSigs = malloc(sizeof(ControlSignals)); 

    // initialize register file here.
    // core->reg_file[0] = ...
    core->reg_file[1] = 0;
    core->reg_file[2] = 10;
    core->reg_file[3] = -15;
    core->reg_file[4] = 20;
    core->reg_file[5] = 30;
    core->reg_file[6] = -35;

    // initialize data memory here.
    // core->data_mem[0] = ...
    core->data_mem[40] = -63;
    core->data_mem[41] = 0;
    core->data_mem[42] = 0;
    core->data_mem[43] = 0;
    core->data_mem[44] = 0;
    core->data_mem[45] = 0;
    core->data_mem[46] = 0;
    core->data_mem[47] = 0;

    core->data_mem[48] = 63;
    core->data_mem[49] = 0;
    core->data_mem[50] = 0;
    core->data_mem[51] = 0;
    core->data_mem[52] = 0;
    core->data_mem[53] = 0;
    core->data_mem[54] = 0;
    core->data_mem[55] = 0;

    core->ALU_result = malloc(sizeof(Signal));
    core->Zero = malloc(sizeof(Signal));
    core->ALU_in_2 = malloc(sizeof(Signal));
    core->ReadData = malloc(sizeof(Signal));

    core->ifid_reg = malloc(sizeof(IF_ID));

    core->idex_reg = malloc(sizeof(ID_EX));
        core->idex_reg->writeback_IDEX = malloc(sizeof(WB_IDEX));
        core->idex_reg->memory_IDEX = malloc(sizeof(M_IDEX));
        core->idex_reg->execute_IDEX = malloc(sizeof(EX_IDEX));

    core->exmem_reg = malloc(sizeof(EX_MEM));
        core->exmem_reg->writeback_EXMEM = malloc(sizeof(WB_EXMEM));
        core->exmem_reg->memory_EXMEM = malloc(sizeof(M_EXMEM));

    core->memwb_reg = malloc(sizeof(MEM_WB));
        core->memwb_reg->ReadData = malloc(sizeof(int64_t));
        core->memwb_reg->ALU_result = malloc(sizeof(Signal));
        core->memwb_reg->writeback_MEMWB = malloc(sizeof(WB_MEMWB));

    core->ifid_reg->empty = 1;
    core->idex_reg->empty = 1;
    core->exmem_reg->empty = 1;
    core->memwb_reg->empty = 1;
    return core;
}

void printInstructionBinary(unsigned instruction){
    
    for (int i = 0; i < 32; ++i) {
        if (instruction >> (31-i) & 0x1) putchar('1');
        else putchar('0');
    }

    putchar('\n');
}

void dataMemoryGrab(Core* core, EX_MEM *exmemreg, unsigned rs1, unsigned immediate, int64_t *ReadData){
    int64_t temp;
    if(exmemreg->memory_EXMEM->MemRead){
        temp = immediate+core->reg_file[rs1];
            for(int i = temp, j=0; i<temp+8; i++,j+=8){
                *ReadData += core->data_mem[i]<<j;
            }
    }
}

// void old_dataMemoryGrab(Core *core, unsigned rs1, unsigned immediate, int64_t *ReadData){
//     int64_t temp;
//     if(core->controlSigs->MemRead){
//     temp = immediate+core->reg_file[rs1];
//         for(int i = temp, j=0; i<temp+8; i++,j+=8){
//             *ReadData += core->data_mem[i]<<j;
//         }
//     }
// }
// FIXME, implement this function

bool tickFunc(Core *core)
{

    if(!core->memwb_reg->empty){ 
        writeBackStage(core);
    }
    else{
        ++core->clk;
        return false;
    }

    if(!core->exmem_reg->empty){ 
        memoryStage(core);
    }
    else core->memwb_reg->empty = 1;

    if(!core->idex_reg->empty){
        executeStage(core);
    }
    else core->exmem_reg->empty = 1;

    if(!core->ifid_reg->empty){
        decodeStage(core);
    }
    else core->idex_reg->empty = 1;

    if(!(core->PC > core->instr_mem->last->addr)){
        fetchStage(core);
        core->PC += 4;  
    }
    else core->ifid_reg->empty = 1;
    ++core->clk;
    return true;
        

    // printf("%d\n", opcode);
    // ControlUnit(opcode, core->controlSigs);

    // printf("instruction:    ");
    // printInstructionBinary(instruction);
    // printf("opcode:         ");
    // printInstructionBinary(opcode);
    // printf("immediate:      ");
    // printInstructionBinary(immediate);
    // printf("rs2:            ");
    // printInstructionBinary(rs2);
    // printf("rs1:            ");
    // printInstructionBinary(rs1);
    // printf("rd:             ");
    // printInstructionBinary(rd);
    // printf("funct7:         ");
    // printInstructionBinary(funct7);
    // printf("funct3:         ");
    // printInstructionBinary(funct3);
    // printf("\n");

    // Step 3, compute ALU computation based on ALUSrc, ALUOp (which sets ALU control pin), as well as rs1, rs2 and the immediate.
    // printf("func3: %d | aluop %lld | func7: %d | opcode: %d  ::  ", funct3, core->controlSigs->ALUOp, funct7, opcode);
    

    
    
    
    // printf("ALU RESULT: %lld \n", *core->ALU_result);

    

    // printInstructionBinary(*core->ALU_result);
    
    // (Step N) Increment PC. FIXME, is it correct to always increment PC by 4?!

    //must be able to increment program counter from jump statements
    //jump statement process if beq in the ALU evaluates to true, and if there is a jump statement
}

// FIXME (1). Control Unit. Refer to Figure 4.18.
void ControlUnit(Signal input,
                 ControlSignals *signals)
{
    // For R-type
    if (input == 51)
    {   
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
        signals->ALUSrc = 1;
        signals->MemtoReg = 0;
        signals->RegWrite = 1;
        signals->MemRead = 0;
        signals->MemWrite = 0;
        signals->Branch = 0;
        signals->ALUOp = 2;
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

    // For shift left
    if (ALUOp == 2 && Funct7 == 0 && Funct3 == 1)
    {
        return 3;
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
    return -1;
}

// Imme. Generator
Signal ImmeGen(Signal input, Signal opcode)
{   
    unsigned immediate = 0;
    //I
    if (opcode == 3 || opcode == 19)  immediate = input >> 20;
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

    // For subtraction
    if (ALU_ctrl_signal == 6)
    {
        *ALU_result = (input_0 - input_1);
        if (*ALU_result == 0) { *zero = 1; } else { *zero = 0; }
    }

    // For and
    if (ALU_ctrl_signal == 0)
    {
        *ALU_result = (input_0 & input_1);
        if (*ALU_result == 0) { *zero = 1; } else { *zero = 0; }
    }
    
    // For or
    if (ALU_ctrl_signal == 1)
    {
        *ALU_result = (input_0 | input_1);
        if (*ALU_result == 0) { *zero = 1; } else { *zero = 0; }
    }

    // For shift left
    if (ALU_ctrl_signal == 3)
    {
        *ALU_result = (input_0 << input_1);
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

void loadFetch(Core *core){

    
}

void fetchStage(Core *core){

    core->ifid_reg->PC = core->PC;
    core->ifid_reg->Instruction = core->instr_mem->instructions[core->ifid_reg->PC / 4].instruction;

}

void decodeStage(Core *core){


    core->idex_reg->PC = core->ifid_reg->PC;

    core->idex_reg->opcode = (core->ifid_reg->Instruction & 127);
    core->idex_reg->immediate = ImmeGen(core->ifid_reg->Instruction, core->idex_reg->opcode);
    core->idex_reg->rs2 = (core->ifid_reg->Instruction >> 20) & 31;
    core->idex_reg->rs1 = (core->ifid_reg->Instruction >> 15) & 31;
    core->idex_reg->rd = (core->ifid_reg->Instruction >> 7) & 31;
    core->idex_reg->funct3 = (core->ifid_reg->Instruction >> 12) & 7;
    core->idex_reg->funct7 = (core->ifid_reg->Instruction >> 25) & 127;

    ControlUnit(core->idex_reg->opcode, core->controlSigs);
    
    core->idex_reg->memory_IDEX->Branch = core->controlSigs->Branch;
    core->idex_reg->memory_IDEX->MemRead = core->controlSigs->MemRead;
    core->idex_reg->memory_IDEX->MemWrite = core->controlSigs->MemWrite;

    core->idex_reg->writeback_IDEX->MemtoReg = core->controlSigs->MemtoReg;
    core->idex_reg->writeback_IDEX->RegWrite = core->controlSigs->RegWrite;
    
    core->idex_reg->execute_IDEX->ALUOp = core->controlSigs->ALUOp;
    core->idex_reg->execute_IDEX->ALUSrc = core->controlSigs->ALUSrc;
    

}

void executeStage(Core *core){

    Signal ALU_in_2 = MUX(core->idex_reg->execute_IDEX->ALUSrc, core->reg_file[core->idex_reg->rs2], core->idex_reg->immediate);
    ALU(core->reg_file[core->idex_reg->rs1], ALU_in_2, ALUControlUnit(core->idex_reg->execute_IDEX->ALUOp, core->idex_reg->funct7, core->idex_reg->funct3), core->exmem_reg->ALU_result, core->exmem_reg->zero);
    
    core->exmem_reg->add_sum = core->idex_reg->PC + core->idex_reg->immediate;
    core->exmem_reg->rs2 = core->idex_reg->rs2;
    core->exmem_reg->rd = core->idex_reg->rd;

    core->exmem_reg->writeback_EXMEM = core->idex_reg->writeback_IDEX;
    core->exmem_reg->memory_EXMEM = core->idex_reg->memory_IDEX;

}

void memoryStage(Core *core){

    core->memwb_reg->writeback_MEMWB = core->exmem_reg->writeback_EXMEM;
    core->memwb_reg->ALU_result = core->exmem_reg->ALU_result;
    core->memwb_reg->rd = core->exmem_reg->rd;

    if (core->exmem_reg->zero && core->exmem_reg->memory_EXMEM){
        core->PCsrc = 1;
    }
    else 
        core->PCsrc = 0;

    dataMemoryGrab(core, core->idex_reg,  core->exmem_reg->rs2, core->exmem_reg->ALU_result, core->memwb_reg->ReadData);
    

}

void writeBackStage(Core *core){

    if(core->memwb_reg->writeback_MEMWB->RegWrite)
        core->reg_file[core->memwb_reg->rd] = MUX(core->memwb_reg->writeback_MEMWB->MemtoReg, core->exmem_reg->ALU_result, core->memwb_reg->ReadData);

}
