#include "lmsm.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>


//======================================================
//  Utilities
//======================================================

void lmsm_cap_value(int * val){
   //TODO - implement capping the value pointed to by this pointer between 999 and -999
}

int lmsm_has_two_values_on_stack(lmsm *our_little_machine) {
    //TODO - return 0 if there are not two values on the stack
    return 1;
}

//======================================================
//  Instruction Implementation
//======================================================

void lmsm_i_jal(lmsm *our_little_machine) {
    our_little_machine->return_address_pointer++;
    our_little_machine->memory[our_little_machine->return_address_pointer] = our_little_machine->program_counter;
    our_little_machine->program_counter = our_little_machine->memory[our_little_machine->stack_pointer];
    our_little_machine->stack_pointer++;
}

void lmsm_i_ret(lmsm *our_little_machine) {
    // Pop the return address from the return address stack and update the program counter.
    our_little_machine->program_counter = our_little_machine->memory[our_little_machine->return_address_pointer];
    // Decrement the return address stack pointer.
    our_little_machine->return_address_pointer--;

    // Restore the return value on the stack (optional, if you want to keep it).
    int returnValue = our_little_machine->memory[our_little_machine->stack_pointer];

    // If you want to update the accumulator with the return value, you can do so.
    our_little_machine->accumulator = returnValue;
}

void lmsm_i_push(lmsm *our_little_machine) {
    if (our_little_machine->stack_pointer >= 0){
        our_little_machine->stack_pointer--;
        our_little_machine->memory[our_little_machine->stack_pointer] = our_little_machine->accumulator;
    }
    else{
        our_little_machine->memory[TOP_OF_MEMORY] = our_little_machine->accumulator;
        our_little_machine->stack_pointer = TOP_OF_MEMORY;
    }
}

void lmsm_i_pop(lmsm *our_little_machine) {
    if (our_little_machine->stack_pointer == TOP_OF_MEMORY) {
        our_little_machine->accumulator = our_little_machine->memory[our_little_machine->stack_pointer];
        our_little_machine->stack_pointer++;
    }
    else {
        our_little_machine->status = STATUS_HALTED;
        our_little_machine->error_code = ERROR_BAD_STACK;
    }
}

void lmsm_i_dup(lmsm *our_little_machine) {
    our_little_machine->memory[TOP_OF_MEMORY - 1] = our_little_machine->memory[TOP_OF_MEMORY];
    our_little_machine->stack_pointer = TOP_OF_MEMORY - 1;
}

void lmsm_i_drop(lmsm *our_little_machine) {
    if (our_little_machine->stack_pointer == TOP_OF_MEMORY) {
        our_little_machine->memory[TOP_OF_MEMORY] = our_little_machine->accumulator;
        our_little_machine->stack_pointer++;
    }
    else {
        our_little_machine->status = STATUS_HALTED;
        our_little_machine->error_code = ERROR_BAD_STACK;
    }
}

void lmsm_i_swap(lmsm *our_little_machine) {
    if (our_little_machine->stack_pointer == TOP_OF_MEMORY - 1){
        our_little_machine->accumulator = our_little_machine->memory[TOP_OF_MEMORY - 1];
        our_little_machine->memory[TOP_OF_MEMORY - 1] = our_little_machine->memory[TOP_OF_MEMORY];
        our_little_machine->memory[TOP_OF_MEMORY] = our_little_machine->accumulator;
        our_little_machine->stack_pointer = TOP_OF_MEMORY - 1;
    }
    else {
        our_little_machine->status = STATUS_HALTED;
        our_little_machine->error_code = ERROR_BAD_STACK;
    }
}

void lmsm_i_sadd(lmsm *our_little_machine) {
    if (our_little_machine->stack_pointer == TOP_OF_MEMORY - 1){
        int value1 = our_little_machine->memory[our_little_machine->stack_pointer];
        int value2 = our_little_machine->memory[our_little_machine->stack_pointer + 1];

        int sum = value1 + value2;

        if (sum > 999){
            sum = 999;
        }

        our_little_machine->stack_pointer++;
        our_little_machine->memory[our_little_machine->stack_pointer] = sum;
    }
}

void lmsm_i_ssub(lmsm *our_little_machine) {
    if (our_little_machine->stack_pointer == TOP_OF_MEMORY - 1){
        int value1 = our_little_machine->memory[our_little_machine->stack_pointer];
        int value2 = our_little_machine->memory[our_little_machine->stack_pointer + 1];

        int difference = value2 - value1;

        if (difference < -999){
            difference = -999;
        }

        our_little_machine->stack_pointer++;
        our_little_machine->memory[our_little_machine->stack_pointer] = difference;
    }
}

void lmsm_i_smax(lmsm *our_little_machine) {
    if(our_little_machine->stack_pointer == TOP_OF_MEMORY - 1){
        int value1 = our_little_machine->memory[our_little_machine->stack_pointer];
        int value2 = our_little_machine->memory[our_little_machine->stack_pointer + 1];
        if (value1 > value2) {
            our_little_machine->stack_pointer++;
            our_little_machine->memory[our_little_machine->stack_pointer] = value1;
        }
        else {
            our_little_machine->stack_pointer++;
            our_little_machine->memory[our_little_machine->stack_pointer] = value2;
        }
    }
    else{
        our_little_machine->status = STATUS_HALTED;
        our_little_machine->error_code = ERROR_BAD_STACK;
    }
}

void lmsm_i_smin(lmsm *our_little_machine) {
    if(our_little_machine->stack_pointer == TOP_OF_MEMORY - 1){
        int value1 = our_little_machine->memory[our_little_machine->stack_pointer];
        int value2 = our_little_machine->memory[our_little_machine->stack_pointer + 1];
        if (value1 < value2) {
            our_little_machine->stack_pointer++;
            our_little_machine->memory[our_little_machine->stack_pointer] = value1;
        }
        else {
            our_little_machine->stack_pointer++;
            our_little_machine->memory[our_little_machine->stack_pointer] = value2;
        }
    }
    else{
        our_little_machine->status = STATUS_HALTED;
        our_little_machine->error_code = ERROR_BAD_STACK;
    }
}

void lmsm_i_smul(lmsm *our_little_machine) {
    if(our_little_machine->stack_pointer == TOP_OF_MEMORY - 1){
        int value1 = our_little_machine->memory[our_little_machine->stack_pointer];
        int value2 = our_little_machine->memory[our_little_machine->stack_pointer + 1];

        int product = value1 * value2;
        if (product > 999) {
            product = 999;
        }
        our_little_machine->stack_pointer++;
        our_little_machine->memory[our_little_machine->stack_pointer] = product;
    }
    else{
        our_little_machine->status = STATUS_HALTED;
        our_little_machine->error_code = ERROR_BAD_STACK;
    }
}

void lmsm_i_sdiv(lmsm *our_little_machine) {
    if(our_little_machine->stack_pointer == TOP_OF_MEMORY - 1){
        int value1 = our_little_machine->memory[our_little_machine->stack_pointer];
        int value2 = our_little_machine->memory[our_little_machine->stack_pointer + 1];

        int quotient = value2 / value1;

        our_little_machine->stack_pointer++;
        our_little_machine->memory[our_little_machine->stack_pointer] = quotient;
    }
    else{
        our_little_machine->status = STATUS_HALTED;
        our_little_machine->error_code = ERROR_BAD_STACK;
    }
}

void lmsm_i_out(lmsm *our_little_machine) {
    // TODO, append the current accumulator to the output_buffer in the LMSM
    // Check if there's enough space in the output_buffer to append the accumulator value.
    if (strlen(our_little_machine->output_buffer) + 1 < OUTPUT_BUFFER_SIZE) {
        // Convert the current accumulator value to a string.
        char accumulator_str[12];  // Assuming a maximum of 12 characters for an int.
        sprintf(accumulator_str, "%d", our_little_machine->accumulator);

        // Calculate the length of the current output_buffer.
        size_t current_length = strlen(our_little_machine->output_buffer);

        // Append the accumulator value to the output_buffer.
        if (current_length > 0) {
            // If there's already content in the output_buffer, add a space before appending.
            strcat(accumulator_str, " ");
            strcat(our_little_machine->output_buffer, accumulator_str);
        }
        else {
            // If the output_buffer is empty, append without adding a space.
            strcat(accumulator_str, " ");
            strcat(our_little_machine->output_buffer, accumulator_str);
        }
    }
}

void lmsm_i_inp(lmsm *our_little_machine) {
    // TODO read a value from the command line and store it as an int in the accumulator
}

void lmsm_i_load(lmsm *our_little_machine, int location) {
    our_little_machine->accumulator = our_little_machine->memory[location];
}

void lmsm_i_add(lmsm *our_little_machine, int location) {
    int sum = our_little_machine->accumulator + our_little_machine->memory[location];
    if(sum > 999){
        our_little_machine->accumulator = 999;
    }
    else{
        our_little_machine->accumulator += our_little_machine->memory[location];
    }
}

void lmsm_i_sub(lmsm *our_little_machine, int location) {
    int difference = our_little_machine->accumulator - our_little_machine->memory[location];
    if(difference < -999){
        our_little_machine->accumulator = -999;
    }
    else{
        our_little_machine->accumulator -= our_little_machine->memory[location];
    }
}

void lmsm_i_load_immediate(lmsm *our_little_machine, int value) {
    our_little_machine->accumulator = value;
}

void lmsm_i_store(lmsm *our_little_machine, int location) {
    our_little_machine->memory[location] = our_little_machine->accumulator;
}

void lmsm_i_halt(lmsm *our_little_machine) {
    our_little_machine->status = STATUS_HALTED;
}

void lmsm_i_branch_unconditional(lmsm *our_little_machine, int location) {
    our_little_machine->program_counter = location;
}

void lmsm_i_branch_if_zero(lmsm *our_little_machine, int location) {
    if(our_little_machine->accumulator == 0){
        our_little_machine->program_counter = location;
    }
}

void lmsm_i_branch_if_positive(lmsm *our_little_machine, int location) {
    if(our_little_machine->accumulator >= 0){
        our_little_machine->program_counter = location;
    }
}

void lmsm_step(lmsm *our_little_machine) {
    // TODO : if the machine is not halted, we need to read the instruction in the memory slot
    //        pointed to by the program counter, bump the program counter then execute
    //        the instruction
    if (our_little_machine->status != STATUS_HALTED) {
        int next_instruction = our_little_machine->memory[our_little_machine->program_counter];
        //lmsm_exec_instruction(our_little_machine, next_instruction);

        our_little_machine->program_counter++;
        our_little_machine->current_instruction = next_instruction;
        int instruction = our_little_machine->current_instruction;
        lmsm_exec_instruction(our_little_machine, instruction);
    }
}

//======================================================
//  LMSM Implementation
//======================================================

void lmsm_exec_instruction(lmsm *our_little_machine, int instruction) {

    // TODO - dispatch the rest of the instruction set and implement
    //        the instructions above

    if (instruction == 0) {
        lmsm_i_halt(our_little_machine);
    }
    else if (100 <= instruction && instruction <= 199){
        lmsm_i_add(our_little_machine, instruction - 100);
    }
    else if (200 <= instruction && instruction <= 299){
        lmsm_i_sub(our_little_machine, instruction - 200);
    }
    else if (300 <= instruction && instruction <= 399){
        lmsm_i_store(our_little_machine, instruction - 300);
    }
    else if (400 <= instruction && instruction <= 499){
        lmsm_i_load_immediate(our_little_machine, instruction - 400);
    }
    else if (500 <= instruction && instruction <= 599){
        lmsm_i_load(our_little_machine, instruction - 500);
    }
    else if (600 <= instruction && instruction <= 699){
        lmsm_i_branch_unconditional(our_little_machine, instruction - 600);
    }
    else if (700 <= instruction && instruction <= 799){
        lmsm_i_branch_if_zero(our_little_machine, instruction - 700);
    }
    else if (800 <= instruction && instruction <= 899){
        lmsm_i_branch_if_positive(our_little_machine, instruction - 800);
    }
    else if (instruction == 902){
        lmsm_i_out(our_little_machine);
    }
    else if (instruction == 910){
        lmsm_i_jal(our_little_machine);
    }
    else if (instruction == 911){
        lmsm_i_ret(our_little_machine);
    }
    else if (instruction == 920){
        lmsm_i_push(our_little_machine);
    }
    else if (instruction == 921){
        lmsm_i_pop(our_little_machine);
    }
    else if (instruction == 922){
        lmsm_i_dup(our_little_machine);
    }
    else if (instruction == 923){
        lmsm_i_drop(our_little_machine);
    }
    else if (instruction == 924){
        lmsm_i_swap(our_little_machine);
    }
    else if (instruction == 930){
        lmsm_i_sadd(our_little_machine);
    }
    else if (instruction == 931){
        lmsm_i_ssub(our_little_machine);
    }
    else if (instruction == 932){
        lmsm_i_smul(our_little_machine);
    }
    else if (instruction == 933){
        lmsm_i_sdiv(our_little_machine);
    }
    else if (instruction == 934){
        lmsm_i_smax(our_little_machine);
    }
    else if (instruction == 935){
        lmsm_i_smin(our_little_machine);
    }
    else {
        our_little_machine->error_code = ERROR_UNKNOWN_INSTRUCTION;
        our_little_machine->status = STATUS_HALTED;
    }
    lmsm_cap_value(&our_little_machine->accumulator);
}

void lmsm_load(lmsm *our_little_machine, int *program, int length) {
    for (int i = 0; i < length; ++i) {
        our_little_machine->memory[i] = program[i];
    }
}

void lmsm_init(lmsm *the_machine) {
    the_machine->accumulator = 0;
    the_machine->status = STATUS_READY;
    the_machine->error_code = ERROR_NONE;
    the_machine->program_counter = 0;
    the_machine->current_instruction = 0;
    the_machine->stack_pointer = TOP_OF_MEMORY + 1;
    the_machine->return_address_pointer = TOP_OF_MEMORY - 100;
    memset(the_machine->output_buffer, 0, sizeof(char) * 1000);
    memset(the_machine->memory, 0, sizeof(int) * TOP_OF_MEMORY + 1);
}

void lmsm_reset(lmsm *our_little_machine) {
    lmsm_init(our_little_machine);
}

void lmsm_run(lmsm *our_little_machine) {
    our_little_machine->status = STATUS_RUNNING;
    while (our_little_machine->status != STATUS_HALTED) {
        lmsm_step(our_little_machine);
    }
}

lmsm *lmsm_create() {
    lmsm *the_machine = malloc(sizeof(lmsm));
    lmsm_init(the_machine);
    return the_machine;
}

void lmsm_delete(lmsm *the_machine) {
    free(the_machine);
}