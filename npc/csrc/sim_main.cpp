#include <stdio.h>
#include "Vtop.h"
#include "verilated_vcd_c.h"

 
// double sc_time_stamp() { return 0; } // Legacy function required only so linking works on Cygwin and MSVC++
VerilatedVcdC* tfp = new VerilatedVcdC();
const std::unique_ptr<VerilatedContext> contextp{new VerilatedContext};
const std::unique_ptr<Vtop> top{new Vtop{contextp.get(), "TOP"}};


uint32_t mem[] = {
    0x108093, // addi sp sp 1
    0x108013, // addi a0 sp 2
    0x108093,
    0x108013,
    0x108093,
    0x108013,
    0x108093,
    0x100073, // ebreak
};

void inst_fetch(int32_t pc) {
    if(pc < 0x80000000) {
    } else {
        top->inst = mem[(pc - 0x80000000)/4];
    }
};

void cycle() {
    top->clk = 0; 
    top->eval();
    tfp->dump(contextp->time());
    contextp->timeInc(1);
    top->clk = 1; 
    top->eval();
    tfp->dump(contextp->time());
    contextp->timeInc(1);
    inst_fetch(top->pc);
};

void rst() {
    top->rst = 0; cycle();
    top->rst = 1; cycle();
    top->rst = 0; cycle();
};

int main(int argc,char **argv)
{
    Verilated::mkdir("logs");
    contextp->debug(0);
    contextp->traceEverOn(true);
    top->trace(tfp, 0);
    tfp->open("build/wave.vcd");

    
    rst();
    while (!contextp->gotFinish()) {
        cycle();
        printf("pc: %x\n", top->pc);
    }
    top->final();
    tfp->close();
    return 0;
}

extern "C" void ebreak(int inst){
    // if (inst==0x100073){
    //     printf("Hit good trap\n");
    // } else {
    //     printf("Hit bad trap\n");
    // }
    Verilated::gotFinish(true);
}