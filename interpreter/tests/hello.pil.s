        main:
0x00    LDA64 hello_string
0x90    CAL strlen
0xAA    SWP
0x00    LDA64 hello_string
0x81    PHA64
0x06    LDA8 0x01
0x90    CAL write
0xFF    HLT

        strlen: # A - Pointer, Returns A len
0x81    PHA64 # Copy to save for later
0x81    PHA64 # Copy that will be overwritten
        strlen_loop:
0x46    LDA8 A
0x0E    LDB8 0x00
0xA7    CMP
0x93    JEQ strlen_end
0x80    PLA64
0x0E    LDB8 0x01
0xA0    ADD
0x81    PHA64
0x92    JMP strlen_loop
        strlen_end:
0x80    PLA64
0x88    PLB64
0xA1    SUB
0x91    RET

        write: # A - File Pointer, B - Length, Stack(1) - Pointer
0x01    STA64 write_buf
0x09    STB64 write_buf+8
0x80    PLA64
0x01    STA64 write_buf+16
0xB2    MCA native_write
0x91    RET

        write_buf: # MCA - Pointer to FP, Length, and Buffer
        QWORD
        QWORD
        QWORD

        hello_string:
        BYTE "Hello World!\r\n", 0
        end_hello_string: