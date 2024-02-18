#include <stdio.h>

#include "Core.h"
#include "Parser.h"

int main(int argc, const char *argv[])
{	
    if (argc != 2)
    {
        printf("Usage: %s %s\n", argv[0], "<trace-file>");

        return 0;
    }

    /* Task One */
    // TODO, (1) parse and translate all the assembly instructions into binary format;
    // (2) store the translated binary instructions into instruction memory.
    Instruction_Memory instr_mem;
    instr_mem.last = NULL;
    loadInstructions(&instr_mem, argv[1]);

    /* Task Two */
    // TODO, implement Core.{h,c}
    Core *core = initCore(&instr_mem);

    
    int j = 1;
    /* Task Three - Simulation */
    while (core->tick(core)){
        printf("Command %d\n", j);
        j++;
        for(int i = 0; i<32; i++){
            printf("x%d %lld\n", i, core->reg_file[i]);
        }
        printf("PC : %lld\n", core->PC);
    }

    printf("Simulation is finished.\n");
    for(int i = 0; i<32; i++){
        printf("x%d %lld\n", i, core->reg_file[i]);
    }
    printf("PC : %lld\n", core->PC);
    free(core);    
}
