#ifndef __CORE_H__
#define __CORE_H__

#include "Instruction_Memory.h"

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#define BOOL bool

typedef uint8_t Byte;
typedef int64_t Signal;
typedef int64_t Register;

typedef struct ControlSignals
{
    Signal Branch;
    Signal MemRead;
    Signal MemtoReg;

    
    Signal ALUOp;
    Signal MemWrite;
    Signal ALUSrc;
    Signal RegWrite;



}ControlSignals;


/* FIRST STAGE CONTROL LINE REGS*/
typedef struct WB_IDEX
{

    Signal *Branch;
    Signal *MemRead;

}WB_IDEX;

typedef struct M_IDEX
{

    Signal *MemtoReg;
    Signal *ALUOp;
    Signal *MemWrite;

}M_IDEX;

typedef struct EX_IDEX
{

    Signal *ALUSrc;
    Signal *RegWrite;
    
}EX_IDEX;


/* SECOND STAGE CONTROL LINE REGS*/
typedef struct WB_EXMEM
{

    Signal *Branch;
    Signal *MemRead;

}WB_EXMEM;

typedef struct M_EXMEM
{

    Signal *MemtoReg;
    Signal *ALUOp;
    Signal *MemWrite;    

}M_EXMEM;

/* THIRD STAGE CONTROL LINE REG*/
typedef struct WB_MEMWB
{

    Signal *Branch;
    Signal *MemRead;    

}WB_MEMWB;



typedef struct IF_ID
{

    Signal *PC;
    Signal *InstructionMemory;

}IF_ID;

typedef struct ID_EX
{

    Signal *PC;

    Register *Read_data_1;
    Register *Read_data_2;
    Signal *Immediate;
    unsigned *funt3;
    unsigned *funct7_bit;
    unsigned *rd;
    WB_IDEX *writeback_IDEX;
    M_IDEX *memory_IDEX;
    EX_IDEX *execute_IDEX;

}ID_EX;

typedef struct EX_MEM
{   

    Signal *add_sum;
    Signal *zero;
    Signal *ALU_result;
    Register *Read_data_2;
    unsigned *rd;
    WB_EXMEM *writeback_EXMEM;
    M_EXMEM *memory_EXMEM;

}EX_MEM;

typedef struct MEM_WB
{

    Byte *read_from_data_memory;
    Signal *ALU_result;
    unsigned *rd;
    WB_MEMWB *writeback_MEMWB;

}MEM_WB;



struct Core;
typedef struct Core Core;
typedef struct Core
{
    Tick clk; // Keep track of core clock
    Addr PC; // Keep track of program counter

    // What else you need? Data memory? Register file?
    Instruction_Memory *instr_mem;

    ControlSignals *controlSigs;
   
    Byte data_mem[1024]; // data memory

    Register reg_file[32]; // register file.

    Signal *ALU_in_2;
    Signal *ALU_result;
    Signal *Zero;

    int64_t *ReadData;

    bool (*tick)(Core *core);




}Core;

Core *initCore(Instruction_Memory *i_mem);
bool tickFunc(Core *core);

// FIXME. Implement the following functions in Core.c
// FIXME (1). Control Unit.
void ControlUnit(Signal input,
                 ControlSignals *signals);

// FIXME (2). ALU Control Unit.
Signal ALUControlUnit(Signal ALUOp,
                      Signal Funct7,
                      Signal Funct3);

// FIXME (3). Imme. Generator
Signal ImmeGen(Signal input, Signal opcode);

// FIXME (4). ALU
void ALU(Signal input_0,
         Signal input_1,
         Signal ALU_ctrl_signal,
         Signal *ALU_result,
         Signal *zero);

// (4). MUX
Signal MUX(Signal sel,
           Signal input_0,
           Signal input_1);

// (5). Add
Signal Add(Signal input_0,
           Signal input_1);

// (6). ShiftLeft1
Signal ShiftLeft1(Signal input);
void fetchStage(Core *core);
void decodeStage(Core *core);
void executeStage(Core *core);
void memoryStage(Core *core);
void writeBackStage(Core *core);

#endif
