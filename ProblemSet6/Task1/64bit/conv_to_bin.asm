
                opt f-g-h+l+o+
                org $1000

start           equ *

                lda <text
                sta $80
                lda >text
                sta $81
                ldy #0
                lda #$a5
                jsr binary

                lda <text
                ldx >text
                jsr $ff80
                brk

binary          sta byte
                ldx #7
bitslp          lda #'0'
                asl byte
                bcc pr
                lda #'1'
pr              sta ($80),y
                iny
                dex
                bpl bitslp
                rts

byte            dta b(0)

                org $2000
text            equ *
                dta b(0),b(0),b(0),b(0)
                dta b(0),b(0),b(0),b(0)
                dta b(10) ; '\n'
                dta b(0)

                org $2E0
                dta a(start)

                end of file
