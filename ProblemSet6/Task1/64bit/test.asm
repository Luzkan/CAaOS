;http://www.6502.org/tutorials/6502opcodes.html#LSR
        opt f-g-h+l+o+
        org $1000           ; org 0

start   equ *

        lda <text           ; przesyła pamięć do akumulatora
        sta $80             ; przesyła zaw. akumulatora do pamięci
        lda >text
        sta $81             
        ldy #1              ; przesyła pamieć do rejestru Y
        lda #$e5
        jsr phex

        lda <text
        ldx >text           ; przesyła pamięć do rejestru X
        jsr $ff80
        brk

; str. 64 
PHEX    PHA                 ; odklada na stos zaw. akum.
        JSR PXDIG
        PLA                 ; zdejmuje wartosc ze stosu i daje do akum.
        LSR @               ; bity w prawo o 1 pozycje x4
        LSR @
        LSR @
        LSR @
PXDIG   AND #%00001111      ; bitowy and (A <- A & M)
        ORA #'0'            ; bitowy or (A <- A | M)
        CMP #'9'+1          ; porownuje zaw. akum. z pamiecia
        BCC PR              ; jump if C = 0
        ADC #'A'-'9'-2      ; dodaje zaw. akum. do pamieci
PR      STA ($80),Y
        DEY                 ; zmniejsza rejestr Y o 1
        RTS
byte    dta b(0)            ; dta umieszcza dane

        org $2000           ; org zapisuje ciag bajtow
text    equ *
        dta b(0),b(0)
        dta b(10)           ; 10 = '\n'
        dta a(0)

        org $2E0
        dta a(start)

        end of file