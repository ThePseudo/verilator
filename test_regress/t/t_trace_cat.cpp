// -*- mode: C++; c-file-style: "cc-mode" -*-
//
// DESCRIPTION: Verilator: Verilog Test module
//
// This file ONLY is placed under the Creative Commons Public Domain, for
// any use, without warranty, 2008 by Wilson Snyder.
// SPDX-License-Identifier: CC0-1.0

#include <verilated.h>
#include <verilated_vcd_c.h>

#include <memory>

#include VM_PREFIX_INCLUDE

#include "TestCheck.h"

int errors = 0;

unsigned long long main_time = 0;
double sc_time_stamp() { return (double)main_time; }

const char* trace_name() {
    static char name[1000];
    VL_SNPRINTF(name, 1000, VL_STRINGIFY(TEST_OBJ_DIR) "/simpart_%04d.vcd", (int)main_time);
    return name;
}

int main(int argc, char** argv) {
    Verilated::debug(0);
    Verilated::traceEverOn(true);
    Verilated::commandArgs(argc, argv);

    {
        VM_PREFIX top{"top"};

        VerilatedVcdC* tfp = new VerilatedVcdC;
        top.trace(tfp, 99);

        // Test for traceCapable - randomly-ish selected this test
        TEST_CHECK_EQ(top.traceCapable, true);

        tfp->open(trace_name());

        top.clk = 0;

        while (main_time < 190) {  // Creates 2 files
            top.clk = !top.clk;
            top.eval();

            if ((main_time % 100) == 0) {
#if defined(T_TRACE_CAT)
                tfp->openNext(true);
#elif defined(T_TRACE_CAT_REOPEN)
                tfp->close();
                tfp->open(trace_name());
#elif defined(T_TRACE_CAT_RENEW)
                tfp->close();
                tfp = new VerilatedVcdC;
                top.trace(tfp, 99);
                tfp->open(trace_name());
#else
#error "Unknown test"
#endif
            }
            tfp->dump((unsigned int)(main_time));
            ++main_time;
        }
        tfp->close();
        top.final();
    }
    printf("*-* All Finished *-*\n");
    return errors;
}
