!to "blink.o", plain
* = $0000
nop ;padding for 0x00 (data direction register)
nop ;padding for 0x01 (output register)
sei
lda #%11111111 ;P0-P7 high
sta $01
lda #%11111111 ;P0-P7 output
sta $00
loop:
lda #%11111110 ;P0 low
sta $01
lda #%11111101 ;P1 low
sta $01
jmp loop
