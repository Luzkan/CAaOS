; https://cs.fit.edu/~mmahoney/cse3101/float.html
section .data
    msgTop: db "|        ~ Assembler Calculator ~", 10, 0
    msgSubTop: db "|", 10, 0
    msgInfo1: db "| Available: add [+], sub [-], div [/]", 10, 0
    msgInfo2: db "|            mul [*], sqrt [root], fact [fact]", 10, 0
    msgInfo3: db "|            sin [sin], cos [cos], tan [tan]", 10, 0
    msgFormat1: db "| Format:    w/ or w/o spaces", 10, 0
    msgFormat2: db "|     ex:    [2+2], [9/3], [8tan], [9root]", 10, 0
    msgFormat3: db "|", 10, 0
    msgCalc: db "|   Calc: ", 0
    scanfformat: db "%lf %c %lf", 0
    resultformat: db "|         = %lf", 10, 0

section .bss
    num1 resb 8
    num2 resb 8
    operand resb 1
  
section .text
    global calc
    extern printf
    extern scanf

calc:
    push msgTop
    call printf
    push msgSubTop
    call printf
    push msgInfo1
    call printf
    push msgInfo2
    call printf
    push msgInfo3
    call printf
    push msgFormat1
    call printf
    push msgFormat2
    call printf
    push msgFormat3
    call printf
    add esp, 14

waitInput:
    push msgCalc
    call printf
    add esp, 2
    push num2
    push operand
    push num1
    push scanfformat
    call scanf
    add esp, 8

    finit
    fld qword [num1]

    cmp byte [operand], '+'
    je addition
    cmp byte [operand], '-'
    je substraction
    cmp byte [operand], '*'
    je multiplication
    cmp byte [operand], 'x'
    je multiplication
    cmp byte [operand], '/'
    je division
    cmp byte [operand], 'r'
    je squareroot
    cmp byte [operand], 'c'
    je cosinus
    cmp byte [operand], 's'
    je sinus
    cmp byte [operand], 'f'
    je factorial

addition:
    fadd qword [num2]
    jmp printing
substraction:
    fsub qword [num2]
    jmp printing
multiplication:
    fmul qword [num2]
    jmp printing
division:
    fdiv qword [num2]
    jmp printing
squareroot:                     ; since program won't repeat after usage (loop back)
    fsqrt                       ;     then we can just fake that input was important
    jmp printing
cosinus:
    fcos
    jmp printing
sinus:
    fsin
    jmp printing
factorial:
    fsub 
    jmp printing


printing:
    fst qword [num1]            ; store floating point
    push dword [num1+4]
    push dword [num1]
    push resultformat
    call printf
    add esp, 4 

exit:
    mov ebx, 0
    mov eax, 1                  
    int 0x80