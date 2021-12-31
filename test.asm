.model tiny
.stack 64
.data
    HEX_CONV            DB '0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'
    ascii_result        db 4 dup(?)
    buffer              dw ?
    num                 db '1','1','1','1'
    res                 dw ?
    OP1_STRING          DB '~~~~'
    OP2_STRING          DB '~~~~'
    PLAYER_INPUT_STRING DW 0
    INPUT_STRING        DB 15,?,15 DUP(0)
    ESC_SCODE EQU 1
    F1_SCODE EQU 03bh
    F2_SCODE EQU 03ch
    F4_SCODE EQU 03EH
    REG_COLOR_FULL EQU 07H
    REG_COLOR_HALF EQU 0BH
    REG_COLOR_LOW EQU 09H
    REG_COLOR_HIGH EQU 0EH

.code

main proc far
                 mov  ax,@data
                 mov  ds,ax
                   MOV AX,13h  ; 320x200 256 colors
   ;MOV AX,06H  ; 640x200 16  colors
    MOV BH,0H   ; First page on screen
    INT 10H    
                 call flushBuffer
                 OR   AX,0FFFFH
    ; Max number of characters to be entered
                 MOV  CL,15
                 LEA  SI,INPUT_STRING+2
    get_string:  
                 call getKeyPressed
                 JZ get_string
    ; Check F4
                 CMP  AH,ESC_SCODE
                 JZ   FINISH_GAME
    ; Check enter key
                 CMP  AL,10
                 JZ   finish_input  
                 CMP  AL,13
                 JZ   finish_input
    ; F4 key was not pressed, so get the rest of string
                 MOV  [SI],AL              ; Move ASCII
                  mov ah,0EH
                MOV BX,0007H
                int 10h
                 call flushBuffer
                  ; Echo pressed character
               
                 INC  SI
                 DEC  CL
                 JNZ  get_string
                 
    finish_input:
    ; Move actual size of the entered string
               ; Move actual size of the entered string
            CMP CL,0
            JZ full_input
            MOV AL,INPUT_STRING
            SUB AL,CL
            MOV CL,AL
            JMP store_length
            full_input:
            MOV CL,INPUT_STRING
            store_length:
            MOV  INPUT_STRING+1,CL
            MOV INPUT_STRING+1,CL  
            finish_game:
            MOV AH,04CH
    XOR AL,AL
    INT 21H
    main endp   

flushBuffer PROC 
                 MOV  AH,0CH
                 MOV  AL,0
                 INT  21H
                 RET
flushBuffer ENDP
 getKeyPressed PROC 
    MOV AH,1
    MOV AL,0
    INT 16H
    RET
getKeyPressed ENDP
end main