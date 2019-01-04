segment .data
newline: db "%d", 10, 0          ; <--- this thing right here
segment .bss                     ;      we need to send %d to printf and \n
walk: resd 1                     ;      '10' stands for \n 
factor: resd 1
segment .text

    extern printf
    global find_primes
    
find_primes:
    mov eax, 1                   ; lower bound: 1
    mov [walk], eax
    mov dword [factor], 2
   
factortest:                      ; checking if number is prine
    mov eax, [factor]            ; check if factor is too large
    imul eax, eax
    cmp [walk], eax
    jl found

    mov eax, [walk]              ; test for carry on division
    mov ebx, [factor]
    cdq                          ; convert double to quad
    idiv ebx
    cmp edx, 0                   ; check if reminder is 0 
    je next                      ; if yes -> its even -> not prime

    add dword [factor], 1        ; add 1 to factor and loop
    jmp factortest               

found:
    push dword [walk]
    push dword newline
    call printf
    pop eax                      ; obviously goes to next now bcuz its below

next:                            ; checks if next num is prime
    mov eax, [walk]
    add eax, 2                   ; add two to go to the next odd num
    cmp eax, 10000               ; upper bound: 10000
    jg end                       ; if beyond - end
    mov [walk], eax
    mov dword [factor], 2
    jmp factortest

end:
    mov eax, 0
    leave
    ret