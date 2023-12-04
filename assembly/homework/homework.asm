//==========================================================================================
// PROBLEM 1: Counting Up From 1 to 10 in Little Man Assembly
//==========================================================================================

// You can develop your code via the online emulator and then paste it in here
// Please use a jump in your solution
// Please use the OUT instruction to print values

        LDA ONE
        STA COUNT
        OUT
LOOPONE LDA COUNT
        ADD ONE
        OUT
        STA COUNT
        SUB TEN
        BRP ENDLOOP
        BRA LOOPONE
ENDLOOP HLT
ONE     DAT 001
TEN     DAT 010
COUNT   DAT

//==========================================================================================
// PROBLEM 2: Counting Up From 1 to 10 in MIPS Assembly
//==========================================================================================

// You can develop your code via the MARS emulator and then paste it in here
// Please use a jump in your solution
// Please use sys-calls to output the value

# MIPS Assembly code to print numbers from 1 to 10
.text
main:
    li $a1, 1          # Load 1 into register $a1 (start value)
    li $a2, 10         # Load 10 into register $a2 (end value)

loop:
    bgt $a1, $a2, exit # If $a1 > $a2, exit the loop
    li $v0, 1          # Load 1 into register $v0 (system call for printing integers)
    move $a0, $a1      # Move the current number to $a0 (argument to be printed)
    syscall            # Perform the system call to print the integer in $a0
    addi $a1, $a1, 1   # Increment the current number by 1
    j loop             # Jump back to the beginning of the loop
j exit                 # Exit the loop

exit:
    li $v0, 10         # Load 10 into register $v0 (system call code for program exit)
    syscall            # Perform the system call to exit the program