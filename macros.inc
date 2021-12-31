; Macro name: display_string
; Function: Displays a string on the screen at given x,y positions
; Inputs: x_pos,y_pos, message variable to print
; Outputs: NONE
display_string MACRO x_pos,y_pos,msg
    PUSH ES
    MOV ES,DS
    MOV AH,2
    MOV DH,y_pos
    MOV DL,x_pos
    INT 10H
    LEA BP,msg
    XOR AL,AL
    MOV AH,13H
    INT 10H
    POP ES
ENDM

; Macro name: display_string_color
; Function: Displays a colored string at given x,y positions
; Inputs: x_pos,y_pos, message variable to print, length of message, color attribute byte
; Outputs: NONE
display_string_color MACRO x_pos,y_pos,msg,len,attr
    PUSH ES
    MOV AX,@DATA
    MOV ES,AX
    LEA BP,msg      ; Msg to print must be in BP -> ES:BP
    MOV AX,attr

    MOV BL,AL     ; Attribute
    MOV BH,AH
    ; MOV BH,BL
    ; MOV CL,4
    ; SHR BH,CL
    MOV AH,13h
    MOV AL,01b      ; bit 0 = 1 ->inc cursor, 
                    ; bit 1 = 0 ->string does not contain attributes 
    MOV CX,len
    MOV DH,y_pos
    MOV DL,x_pos
    INT 10H
    POP ES
ENDM

; Macro name: read_string
; Function: Reads string from user 
; Inputs: String variable name
; Outputs: NONE
read_string MACRO msg 
    MOV AH,0AH
    LEA DX,msg
    INT 21H
ENDM

; Macro name: disp_char
; Function: Prints 1 charcter on screen
; Inputs: Character to be printed must be in AL
; Outputs: NONE
disp_char MACRO 
    mov ah,2
    mov dl,al
    int 21h
ENDM

get_char MACRO DEST
    MOV AH,1
    INT 16H
    MOV [DEST],AL
ENDM

clear_screen MACRO
    MOV AH,06H
    MOV AL,00
    MOV BH,0
    MOV CX,0
    MOV DX,0184FH
    INT 10H
ENDM

newline MACRO
    mov ah,09h
    mov al,10
    mov cx,1
    mov bx,0
    int 10h  
    mov ah,09h
    mov al,13
    mov cx,0
    mov bx,0
    int 10h
ENDM

; Macro name: move_cursor
; Function: Moves the keyboard cursor to specified x,y positions
; Inputs: X_POS, Y_POS
; Outputs: NONE
move_cursor MACRO X_POS,Y_POS
        MOV AH,01H
        MOV CH,0FH
        MOV AH,2
        MOV DH,y_pos
        MOV DL,x_pos
        MOV BH,0
        INT 10H

    ENDM

; Macro name: ConvertToASCII
; Function: Converts a WORD into 4 ASCII characters
; Inputs: data: to be converted
; Outputs: result: (WORD) variable to store the result in
ConvertToASCII MACRO data,result
    LOCAL LOOP_STRING
    PUSH AX
    PUSH BX
    PUSH CX
    PUSH DX
    PUSH SI
    PUSH BP
    PUSH DI

    MOV AX,data
    MOV CX,4
    LEA BX,HEX_CONV
    LEA SI,result
    ;Nibble mask
    MOV DX,0F000H
    ; f000
    ; 000f
    LOOP_STRING: 
    PUSH AX
    AND AX,DX   ;Get a nibble
     ; 1111 0000 0000 0000
    DEC CX
    ; MULTIPLY BY 4
    SHL CX,1 
    SHL CX,1
    ; MOVE NIBBLE TO BE THE LEAST SIGNIFICANT NIBBLE
    SHR AX,CL
    ;DIVIDE BY 4
    SHR CX,1
    SHR CX,1
    INC CX
    ; Use look up table to get ascii value of the number
    XLAT
    ; Copy into target string
    MOV [SI],AL
    ; Move to next byte
    INC SI
    ; Shift the mask by 4 bits
    SHR DX,1
    SHR DX,1
    SHR DX,1
    SHR DX,1

    POP AX
    LOOP LOOP_STRING
    POP DI
    POP BP
    POP SI
    POP DX
    POP CX
    POP BX
    POP AX
    
ENDM

; Macro name: ConvertToDecimal
; Function: Converts a WORD/BYTE of ASCII characters into a decimal number that can be used as a direct value in registers
;   Ex: (01FE) as characters = 510 (decimal value) 
; Inputs: data: to be converted, buffer to process data, length of data to be converted (1-for byte, 2-for word)
; Outputs: result: (WORD/BYTE) variable to store the result in
ConvertToDecimal MACRO var,len,buffer,result
    LOCAL CONVERT,HEX,FINISH,SKIP,SKIP2,BYTES_2, DONE,UPPER,LOWER
    
    MOV CH,len  ; Size of variable (1 or 2 bytes)
    SHL CH,1    ; Multiply by 2 -> since we need to loop 4 times if 2 bytes (FFFF) or 2 times if 1 byte (FF)
    MOV CL,4
    LEA BX,var
    MOV AX,len
    CMP AX,2  
    JZ  BYTES_2
    MOV DX,16   ; Variable is 1 byte so max divisor is 16 (16^1)
    JMP  DONE
    BYTES_2:
    MOV DX,4096 ; Variable is 2 bytes so max divisor is 4096 (16^3)
    DONE:
 
    MOV buffer,0; Clear buffer first

    CONVERT:
    ;16^= 4096  256   16      1 
    ;var= F      F     F      F
    ;BX = 0      1     2      3
    ;SHL= 2      4     4      x
    MOV AH,0    ; Clear AH before multiplication
    MOV AL,[BX] ; Get first character 

    ;Check if number 
    SUB AL,48
    CMP AL,9 ;57-48
    JG  HEX
    ;ELSE-> NUMBER FROM 0-9
    JMP  SKIP
;-------HEXADECIMAL CASE FOR UPPER OR LOWER CASE LETTERS-------;
HEX:
    ADD AL,48

;CHECK IF UPPERCASE OR LOWER
    SUB AL,65
    CMP AL,15  ;70-65
    JBE  UPPER
    ADD AL,65
    
    SUB AL,97   
    CMP AL,5 ; 102-97
    JBE  LOWER
    
UPPER:
    ADD AL,70
    SUB AL,60 ; 70-10  
    JMP  SKIP
LOWER:       
    ADD AL,97
    SUB AL,87 ; 97 - 10
    JMP  SKIP
SKIP: 
    PUSH DX     ; Backup DX since it will change during multiplication
    MUL DX      ; Multiply by 16^CX
    POP DX      ; Retrieve old value of DX
    ADD buffer,AX ; Buffer = AX + oldBuffer
    FINISH:
    ; Divide DL by BL
    SHR DX,CL    ; Divide divisor by 16 (2^4)
    INC BX      ; Get next character
    DEC  CH     ; Decrement counter
    JNZ  CONVERT
    ; Move final result to destination
    MOV AX,buffer
    MOV result,AX
ENDM

; Macro name: ConvertDecimalToASCII
; Function: Converts a number between 00-99 into 2 ASCII characters
; Inputs data: number to be converted
; Outputs: result: (byte) variable to store the result in
ConvertDecimalToASCII MACRO data,result

    LEA SI,result
    MOV AX,data
    MOV DX,0
    mov CX,10D
    DIV CX

    ADD AX,30H 
    ADD DX,30H
    MOV [SI],AX
    INC SI
    MOV [SI],DX


ENDM
; Macro name: Check_Hex
; Function: Checks if given data is hex or not
; Inputs: var: to be checked, length of data to be checked (1-for byte, 2- for word)
; Outputs: ZF=1 if hex, ZF= 0 if not hex
;          Can use it directly before JZ /JNZ 
;           checkHex var,2
;           JZ  valid
checkHex MACRO var,len 

    LOCAL HEX,NOT_HEX,LOWER,UPPER,SKIP,CHECKING
    LEA SI,var ; Pointer to number
    MOV CL,len    ; Length of number (2/4 chars.)
    CHECKING:
    MOV AL,[SI]   
     ;Check if number 
    SUB AL,48
    CMP AL,9 ;57-48
    JA  HEX
    ;ELSE-> NUMBER FROM 0-9
    JMP  SKIP
;-------HEXADECIMAL CASE FOR UPPER OR LOWER CASE LETTERS-------;
HEX:
    ADD AL,48

    ;CHECK IF UPPERCASE OR LOWER
    SUB AL,65
    CMP AL,15  ;70-65
    JBE  UPPER
    ADD AL,65
    
    SUB AL,97   
    CMP AL,5 ; 102-97
    JBE  LOWER
    JMP  NOT_HEX 
UPPER:
    ADD AL,70
    SUB AL,60 ; 70-10  
    JMP  SKIP
LOWER:       
    ADD AL,97
    SUB AL,87 ; 97 - 10
    JMP  SKIP
SKIP:  
   INC SI
   DEC CL
JNZ  CHECKING
NOT_HEX: 
CMP CL,0      ; To check if hex, CL should be zero, so ZF = 1   
ENDM 

getOperands MACRO input_str
   LOCAL CHECK_SPACE, SPLIT_OP1, SPLIT_OP2,SPLIT_OP2_LOOP,ENDED

    LEA SI,input_str
    LEA DI,OP1_STRING
    ;;;;;;;;;;;;;;;;;;;CHECK FOR SPACE;;;;;;;;;;;;;;;;;
    CHECK_SPACE:
        INC SI
        MOV AL,[SI] 
        CMP AL,20H
        JNZ CHECK_SPACE
        JZ SPLIT_OP1

    ;;;;;;;;;;;;;;;;;;;;;SPLIT OP1;;;;;;;;;;;;;;;;;;;;
    SPLIT_OP1:
          INC SI
          MOV DL,[SI] 
          CMP DL,','
          JZ  SPLIT_OP2
          MOV AL,10
          CMP DL,AL
          JZ ENDED
          MOV [DI],DL 
          INC DI
          CMP DL,','
          JNZ SPLIT_OP1
    ;;;;;;;;;;;;;;;;;;;;;;;SPLIT OP2;;;;;;;;;;;;;;;;;;;;;
    SPLIT_OP2:
          LEA DI,OP2_STRING
          SPLIT_OP2_LOOP:
              INC SI
              MOV AL,10
              CMP [SI],AL
              JZ ENDED
              MOV DL,[SI]
              MOV [DI],DL 
              INC DI 
          JMP SPLIT_OP2_LOOP
          ENDED:

ENDM
;---BITWISE INSTRUCTIONS EXECUTION FUNCTIONS---;
XOR_EXECUTE MACRO WIDTH
    LOCAL _8BIT,_16BIT,DONE
    MOV SI,current_exec_op1
    MOV DI,current_exec_op2
    MOV AX,1
    CMP WIDTH,AX
    JNZ  _16BIT
    _8BIT:
    MOV AL,[si]
    MOV BL,[di]
    XOR AL,BL
    MOV BYTE PTR [SI],AL
    JMP  DONE
    _16BIT:
    MOV AX,[si]
    MOV BX,[di]
    XOR AX,BX
    MOV [SI],AX
    DONE:
ENDM
AND_EXECUTE MACRO WIDTH
    LOCAL _8BIT,_16BIT,DONE
    MOV SI,current_exec_op1
    MOV DI,current_exec_op2
    MOV AX,1
    CMP WIDTH,AX
    JNZ  _16BIT
    _8BIT:
    MOV AL,[si]
    MOV BL,[di]
    AND AL,BL
    MOV BYTE PTR [SI],AL
    JMP  DONE
    _16BIT:
    MOV AX,[si]
    MOV BX,[di]
    AND AX,BX
    MOV [SI],AX
    DONE:
ENDM
OR_EXECUTE  MACRO WIDTH
    LOCAL _8BIT,_16BIT,DONE
    MOV SI,current_exec_op1
    MOV DI,current_exec_op2
    MOV AX,1
    CMP WIDTH,AX
    JNZ  _16BIT
    _8BIT:
    MOV AL,[si]
    MOV BL,[di]
    OR AL,BL
    MOV BYTE PTR [SI],AL
    JMP  DONE
    _16BIT:
    MOV AX,[si]
    MOV BX,[di]
    OR AX,BX
    MOV [SI],AX
    DONE:
ENDM
SHL_EXECUTE MACRO WIDTH
    LOCAL _8BIT,_16BIT,DONE
    MOV SI,current_exec_op1
    MOV DI,current_exec_op2
    MOV AX,1
    CMP WIDTH,AX
    JNZ  _16BIT
    _8BIT:
    MOV AL,[si]
    MOV CL,[di]
    SHL AL,CL
    MOV BYTE PTR [SI],AL
    JMP  DONE
    _16BIT:
    MOV AX,[si]
    MOV CX,[di]
    SHL AX,CL
    MOV [SI],AX
    DONE:
ENDM
SHR_EXECUTE MACRO WIDTH
    LOCAL _8BIT,_16BIT,DONE
    MOV SI,current_exec_op1
    MOV DI,current_exec_op2
    MOV AX,1
    CMP WIDTH,AX
    JNZ  _16BIT
    _8BIT:
    MOV AL,[si]
    MOV CL,[di]
    SHR AL,CL
    MOV BYTE PTR [SI],AL
    JMP  DONE
    _16BIT:
    MOV AX,[si]
    MOV CX,[di]
    SHR AX,CL
    MOV [SI],AX
    DONE:
ENDM
SAR_EXECUTE MACRO WIDTH
    LOCAL _8BIT,_16BIT,DONE
    MOV SI,current_exec_op1
    MOV DI,current_exec_op2
    MOV AX,1
    CMP WIDTH,AX
    JNZ  _16BIT
    _8BIT:
    MOV AL,[si]
    MOV CL,[di]
    SAR AL,CL
    MOV BYTE PTR [SI],AL
    JMP  DONE
    _16BIT:
    MOV AX,[si]
    MOV CX,[di]
    SAR AX,CL
    MOV [SI],AX
    DONE:
ENDM
ROR_EXECUTE MACRO WIDTH
    LOCAL _8BIT,_16BIT,DONE
    MOV SI,current_exec_op1
    MOV DI,current_exec_op2
    MOV AX,1
    CMP WIDTH,AX
    JNZ  _16BIT
    _8BIT:
    MOV AL,[si]
    MOV CL,[di]
    ROR AL,CL
    MOV BYTE PTR [SI],AL
    JMP  DONE
    _16BIT:
    MOV AX,[si]
    MOV CX,[di]
    ROR AX,CL
    MOV [SI],AX
    DONE:
ENDM
ROL_EXECUTE MACRO WIDTH
    LOCAL _8BIT,_16BIT,DONE
    MOV SI,current_exec_op1
    MOV DI,current_exec_op2
    MOV AX,1
    CMP WIDTH,AX
    JNZ  _16BIT
    _8BIT:
    MOV AL,[si]
    MOV CL,[di]
    ROL AL,CL
    MOV BYTE PTR [SI],AL
    JMP  DONE
    _16BIT:
    MOV AX,[si]
    MOV CX,[di]
    ROL AX,CL
    MOV [SI],AX
    DONE:
ENDM;CHECK FOR PUSH (5 CHARS)  
RCL_EXECUTE MACRO WIDTH
LOCAL _8BIT,_16BIT,DONE
    MOV SI,current_exec_op1
    MOV DI,current_exec_op2
    MOV AX,1
    CMP WIDTH,AX
    JNZ  _16BIT
    _8BIT:
    MOV AL,[si]
    MOV CL,[di]
    RCL AL,CL
    MOV BYTE PTR [SI],AL
    JMP  DONE
    _16BIT:
    MOV AX,[si]
    MOV CX,[di]
    ROL AX,CL
    MOV [SI],AX
    DONE:
ENDM
RCR_EXECUTE MACRO WIDTH
    MOV AX,1
    CMP WIDTH,AX

ENDM
MOV_EXECUTE MACRO WIDTH
LOCAL _8BIT,_16BIT,DONE
    MOV SI,current_exec_op1
    MOV DI,current_exec_op2
    MOV AX,1
    CMP WIDTH,AX
    JNZ  _16BIT
    _8BIT:
    MOV AL,[si]
    MOV CL,[di]
    MOV AL,CL
    MOV BYTE PTR [SI],AL
    JMP  DONE
    _16BIT:
    MOV AX,[si]
    MOV CX,[di]
    MOV AX,CX
    MOV [SI],AX
    DONE:
ENDM
ADD_EXECUTE MACRO WIDTH
LOCAL _8BIT,_16BIT,DONE
    MOV SI,current_exec_op1
    MOV DI,current_exec_op2
    MOV AX,1
    CMP WIDTH,AX
    JNZ  _16BIT
    _8BIT:
    MOV AL,[si]
    MOV CL,[di]
    ADD AL,CL
    MOV BYTE PTR [SI],AL
    JMP  DONE
    _16BIT:
    MOV AX,[si]
    MOV CX,[di]
    ADD AX,CX
    MOV [SI],AX
    DONE:
ENDM
SUB_EXECUTE MACRO WIDTH
LOCAL _8BIT,_16BIT,DONE
    MOV SI,current_exec_op1
    MOV DI,current_exec_op2
    MOV AX,1
    CMP WIDTH,AX
    JNZ  _16BIT
    _8BIT:
    MOV AL,[si]
    MOV CL,[di]
    SUB AL,CL
    MOV BYTE PTR [SI],AL
    JMP  DONE
    _16BIT:
    MOV AX,[si]
    MOV CX,[di]
    SUB AX,CX
    MOV [SI],AX
    DONE:
ENDM
SBB_EXECUTE MACRO WIDTH
    LOCAL _8BIT,_16BIT,DONE,_8BIT2,_16BIT2,P2
    CMP current_player_turn,0
    JNZ P2
    MOV SI,current_exec_op1
    MOV DI,current_exec_op2
    MOV AX,1
    CMP WIDTH,AX
    JNZ  _16BIT
    _8BIT:
    MOV AL,[si]
    MOV CL,[di]
    SUB AL,CL
    SUB AL,PLAYER1_CARRY
    MOV BYTE PTR [SI],AL
    JMP  DONE
    _16BIT:
    MOV AX,[si]
    MOV CX,[di]
    SUB AX,CX
    SUB AX,PLAYER1_CARRY
    MOV [SI],AX
    JMP  DONE

   P2: 
    MOV SI,current_exec_op1
    MOV DI,current_exec_op2
    MOV AX,1
    CMP WIDTH,AX
    JNZ  _16BIT2
    _8BIT2:
    MOV AL,[si]
    MOV CL,[di]
    SUB AL,CL
    SUB AL,PLAYER2_CARRY
    MOV BYTE PTR [SI],AL
    JMP  DONE
    _16BIT2:
    MOV AX,[si]
    MOV CX,[di]
    SUB AX,CX
    SUB AX,PLAYER2_CARRY
    MOV [SI],AX
    DONE:
ENDM
ADC_EXECUTE MACRO WIDTH
    LOCAL _8BIT,_16BIT,DONE,_8BIT2,_16BIT2,P2
    cmp current_player_turn,0
    jnz p2
    
    MOV SI,current_exec_op1
    MOV DI,current_exec_op2
    MOV AX,1
    CMP WIDTH,AX
    JNZ  _16BIT
    _8BIT:
    MOV AL,[si]
    MOV CL,[di]
    ADD AL,CL
    ADD AL,PLAYER1_CARRY
    MOV BYTE PTR [SI],AL
    JMP  DONE
    _16BIT:
    MOV AX,[si]
    MOV CX,[di]
    ADD AX,CX
    ADD AX,PLAYER1_CARRY
    MOV [SI],AX
    JMP  DONE

   P2: 
    MOV SI,current_exec_op1
    MOV DI,current_exec_op2
    MOV AX,1
    CMP WIDTH,AX
    JNZ  _16BIT2
    _8BIT2:
    MOV AL,[si]
    MOV CL,[di]
    ADD AL,CL
    ADD AL,PLAYER2_CARRY
    MOV BYTE PTR [SI],AL
    JMP  DONE
    _16BIT2:
    MOV AX,[si]
    MOV CX,[di]
    ADD AX,CX
    ADD AX,PLAYER2_CARRY
    MOV [SI],AX
    DONE:
ENDM
PUSH_EXECUTE MACRO 
        mov al,4
ENDM
POP_EXECUTE MACRO
        mov al,4
ENDM
INC_EXECUTE MACRO WIDTH
LOCAL _8BIT,_16BIT,DONE
    MOV SI,current_exec_op1
    MOV DI,current_exec_op2
    MOV AX,1
    CMP WIDTH,AX
    JNZ  _16BIT
    _8BIT:
    MOV AL,[si]
    MOV CL,[di]
    INC AL,CL
    MOV BYTE PTR [SI],AL
    JMP  DONE
    _16BIT:
    MOV AX,[si]
    MOV CX,[di]
    INC AX,C
    MOV [SI],AX
    DONE:
ENDM
DEC_EXECUTE MACRO WIDTH
LOCAL _8BIT,_16BIT,DONE
    MOV SI,current_exec_op1
    MOV AX,1
    CMP WIDTH,AX
    JNZ  _16BIT
    _8BIT:
    MOV AL,[si]
    DEC AL
    MOV BYTE PTR [SI],AL
    JMP  DONE
    _16BIT:
    MOV AX,[si]
    DEC AX
    MOV [SI],AX
    DONE:
ENDM
CLC_EXECUTE MACRO
    LOCAL DONE,PLAYER2
    CMP current_exec_cpu,PLAYER1
    JNZ PLAYER2
    MOV PLAYER1_CARRY,0
    JMP DONE
    PLAYER2:
    MOV PLAYER2_CARRY,0
    DONE:
ENDM

; Macro name: POWER_UP_EXEC_OWN
; Function: Checks if given data is hex or not
; Inputs: var: to be checked, length of data to be checked (1-for byte, 2- for word)
; Outputs: ZF=1 if hex, ZF= 0 if not hex

POWER_UP_execOwn MACRO
    LOCAL PLAYER2,DONE
    CMP current_player_turn,PLAYER2
    JZ PLAYER2
    MOV current_exec_cpu,PLAYER1
    JMP DONE
    PLAYER2:
    MOV current_exec_cpu,PLAYER2
    DONE:
ENDM

POWER_UP_execBoth  MACRO
    MOV power_up_exec_both,0ffh
ENDM

POWER_UP_changeForbiddenChar MACRO
    LOCAL PLAYER2,DONE

    CMP current_player_turn,PLAYER2
    JZ PLAYER2
    MOV current_exec_cpu,PLAYER1


    JMP DONE
    PLAYER2:
    MOV current_exec_cpu,PLAYER2
    
    
    
    DONE:
ENDM

POWER_UP_stuckDataLine MACRO

ENDM

POWER_UP_CLEAR_ALL_REGS  MACRO 
    LOCAL PLAYER2,DONE
    CMP current_player_turn,PLAYER2
    MOV AX_REG_1,0
    MOV BX_REG_1,0
    MOV CX_REG_1,0
    MOV DX_REG_1,0
    MOV SI_REG_1,0
    MOV DI_REG_1,0
    MOV BP_REG_1,0
    MOV SP_REG_1,0
    JMP DONE
    JZ PLAYER2:
    MOV AX_REG_2,0
    MOV BX_REG_2,0
    MOV CX_REG_2,0
    MOV DX_REG_2,0
    MOV SI_REG_2,0
    MOV DI_REG_2,0
    MOV BP_REG_2,0
    MOV SP_REG_2,0
    DONE:

ENDM

checkOperands MACRO 
    MOV AL,0
ENDM
