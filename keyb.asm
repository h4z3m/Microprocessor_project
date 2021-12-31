.MODEL tiny
.data
str db 'test msg$'
.code             

display_string MACRO x_pos,y_pos,msg
    MOV AH,2
    MOV DH,y_pos
    MOV DL,x_pos
    INT 10H
    LEA DX,msg
    XOR AL,AL
    MOV AH,9H
    INT 21H
ENDM

main proc far
    mov ax,@data
    mov ds,ax  
    mov cl,0
    check:
    call getKeyPressed
    jz no
    display_string Cl,5,str 
    inc cl
    call flushBuffer
    
    no: 
    
    jmp check
    
    
    
main endp
flushBuffer PROC
    MOV AH,0CH
    MOV AL,0
    INT 21H
    RET
flushBuffer ENDP
getKeyPressed PROC
    MOV AH,1
    MOV AL,0
    INT 16H
    RET
getKeyPressed ENDP
end main
