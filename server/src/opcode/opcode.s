.extern __ZN7Command22function_table_op_codeE 

.global _callOpCodeFunc
_callOpCodeFunc:
    cmp x1, #0x04
    b.ge error_opcode

    ldr x5, =__ZN7Command22function_table_op_codeE
    ldr x6, [x5, x1, lsl #3]
    cbz x6, error_opcode

    mov x1, x2 
    mov x2, x3

    blr x6

error_opcode:
    mov x0, #0 
    ret
