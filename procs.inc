CLEANTOPLINE PROC 
                 PUSH AX
                 PUSH BX
                 PUSH CX
                 PUSH DX
                 PUSH SI

    ; CLEAN THE TO TOP MOST LINE BY SET ALL THE TOP LINE PIXELS TO BLACK
                 MOV  AH,SET_PIXEL_COLOR     ;FOR PIXEL SET
                 MOV  AL,0                   ;COLOR 0 IS BLACK (TO ERASE ALL PIXELS IN THE LINE)
                 MOV  BX,0                   ;NEED TO SET BH=0H FOR PAGE NUMBER ALWAYS 0 FOR 320X200X256 CGA-256
                 MOV  DX,[TOP_Y]             ;USUALLY SET Y=9 FROM "SCREENMOVEMENT"  (OR SET Y TO 0 FORM "SHOW_BITMAP")
                 MOV  DI,0                   ;PIXEL X POSITION COUNTER
    LOOPCLSTOP:  
                 MOV  CX,DI                  ;SET X
                 INT  10H                    ;DRAW(X, Y) CLS THE PIXEL
                 INC  DI                     ;ADD 1 TO X
                 CMP  DI,MAX_X               ;STOP WHEN X==319 (SINCE WE START FROM 0 OFFSET)
                 JE   ENDCLEAN
                  JMP    LOOPCLSTOP

    ENDCLEAN:    
                 POP  SI
                 POP  DX
                 POP  CX
                 POP  BX
                 POP  AX
                 RET
CLEANTOPLINE ENDP 

OPENFILE PROC 
        MOV  AH, 3DH
        XOR  AL, AL
        MOV  DX, OFFSET FILENAME
        INT  21H
        JC   OPENERROR
        MOV  [FILEHANDLE], AX
    RET

    OPENERROR:   
        MOV  DX, OFFSET ERRORMSG
        MOV  AH, 9H
        INT  21H
    RET
OPENFILE ENDP 

READHEADER PROC 
    MOV  AH,3FH
    MOV  BX, [FILEHANDLE]
    MOV  CX,54
    MOV  DX,OFFSET HEADER
    INT  21H
RET
READHEADER ENDP 

READPALETTE PROC 
    MOV  AH,3FH
    MOV  CX,400H    ; Read 1024 bytes
    MOV  DX,OFFSET PALETTE
    INT  21H
RET
READPALETTE ENDP 

COPYPAL PROC 
    MOV  SI,OFFSET PALETTE
    MOV  CX,BMP_COLOR_RES       ; Copy palette from BMP (256 colors)
    MOV  DX,3C8H                ; 3C8H -> Starting color register
    MOV  AL,0
    OUT  DX,AL
    INC  DX                     ; 3C8H -> Palette register
    PALLOOP:     
    ; Color format = B G R
    MOV  AL,[SI+2]              ; Red value is the 3rd byte
    SHR  AL,1                   
    SHR  AL,1                   ; Divide color value by 4
    OUT  DX,AL                  ; Send red val
    
    MOV  AL,[SI+1]              ; Green value is the 2nd byte
    SHR  AL,1                   
    SHR  AL,1                   ; Divide color value by 4
    OUT  DX,AL                  ; Send green val
    
    
    MOV  AL,[SI+0]              ; blue value is the 1st byte
    SHR  AL,1                   
    SHR  AL,1                   ; Divide color value by 4
    OUT  DX,AL                  ; Send blue val

    ADD  SI,4                   ; Point to next color, skip null character first
    LOOP PALLOOP

    RET
COPYPAL ENDP 

COPYBITMAP PROC 
    MOV  AX, VIDEO_MEMORY_OFFSET ; Video memory address
    MOV  ES, AX     ; Video memory is now accessed from ES
    MOV  CX,SCREEN_ROWS
    ; 0 1 2 3 
    ; 4 5 6 7 
    
    PRINTBMPLOOP:
    PUSH CX
    ; offset = y_pos*320 + x_pos, x_pos = external counter (CX) for rows, DI = CX*320
    ; CX is decremented, so first row is the last row in the image (BMP is saved upside down)
    MOV  DI,CX
    ; CX*64
    SHL  CX,1
    SHL  CX,1
    SHL  CX,1
    SHL  CX,1
    SHL  CX,1
    SHL  CX,1

    ; DI*256
    SHL  DI,1
    SHL  DI,1
    SHL  DI,1
    SHL  DI,1
    SHL  DI,1
    SHL  DI,1
    SHL  DI,1
    SHL  DI,1
    ; DI = DI*256
    ; CX = DI*64
    ;DI + CX = DI*320
    ADD  DI,CX
    ; Read line from memory
    MOV  AH,3FH
    MOV  CX,SCREEN_COLS
    MOV  DX,OFFSET SCRLINE
    INT  21H
    ; Copy line by line into video memory
    CLD  ; INC SI,DI
    MOV  CX,SCREEN_COLS
    MOV  SI,OFFSET SCRLINE
    ; Move from SI:DS to DI:ES
    REP  MOVSB                 
    ; Get old value of CX
    POP  CX
    LOOP PRINTBMPLOOP
RET
COPYBITMAP ENDP 

CLOSEFILE PROC 
    MOV  AH,3EH
    MOV  BX, [FILEHANDLE]
    INT  21H
    RET
CLOSEFILE ENDP 

SHOW_BITMAP PROC 
    PUSH AX
    PUSH BX
    PUSH CX
    PUSH DX
    PUSH SI
    PUSH ES
    CALL OPENFILE
    CALL READHEADER
    CALL READPALETTE
    CALL COPYPAL
    CALL COPYBITMAP
    POP ES
    POP  SI
    POP  DX
    POP  CX
    POP  BX
    POP  AX
RET
SHOW_BITMAP ENDP

; Procedure name: getsKeyPressed
; Function: Gets key from buffer, doesn't wait for input
; Inputs: NONE
; Outputs: AH = SCANCODE, AL = ASCII
getKeyPressed PROC FAR
    MOV AH,1
    MOV AL,0
    INT 16H
    RET
getKeyPressed ENDP

; Procedure name: Delay
; Function: Delays for approximately 1s
; Inputs: NONE
; Outputs: NONE
Delay PROC FAR

    MOV DX,0EFFH
    ONE:
        MOV CX,01FFH
        TWO:
            NOP
        LOOP TWO
    DEC DX
     JNZ   ONE
    
    RET
Delay ENDP

; Procedure name: displayMainMenu
; Function: Displays main menu strings
; Inputs: NONE
; Outputs: NONE

; Procedure name: flushBuffer
; Function: Clears keyboard buffer completely
; Inputs: NONE
; Outputs: NONE
flushBuffer PROC FAR
    MOV AH,0CH
    MOV AL,0
    INT 21H
    
    RET
flushBuffer ENDP
; Procedure name: RefreshMemory
; Function: Refreshes the memory for both players
; Inputs: NONE
; Outputs: NONE
RefreshMemory PROC FAR
        
        ; Player 1's memory
        LEA SI,PLAYER1_MEMORY
        MOV temp_x,1
        MOV temp_y,1
        mov cx,16
        MEM1:
        MOV BL,BYTE PTR [SI]
        MOV BH,0
        PUSH BX
        PUSH CX
        PUSH SI
        ConvertDecimalToASCII BX,buffer_ascii
        display_string_color temp_x,temp_y,buffer_ascii,2,MEMORY_CELL_COLOR
        POP SI
        POP CX
        POP BX
        INC temp_y
        INC SI
        LOOP MEM1 
        ; Player 2's memory
        MOV temp_x,38
        MOV temp_y,1
        mov cx,16
        MEM2:
        MOV BL,BYTE PTR [SI]
        MOV BH,0
        PUSH BX
        PUSH CX
        PUSH SI
        ConvertDecimalToASCII BX,buffer_ascii
        
        display_string_color temp_x,temp_y,buffer_ascii,2,MEMORY_CELL_COLOR
        POP SI
        POP CX
        POP BX
        INC temp_y
        INC SI
        LOOP MEM2 
    RET
RefreshMemory ENDP 
; Procedure name: RefreshScreenContents
; Function: Refreshed the screen contents including regs, memory, bitmap
; Inputs: NONE
; Outputs: NONE
RefreshScreenContents PROC FAR
; Refresh player 1's registers
    ;AX Register
        display_string_color 4,13,REG16_NAMES,2,REG_COLOR_HALF
        ConvertToASCII AX_REG_1,buffer_ascii
        display_string_color 7,13,buffer_ascii,2,REG_COLOR_LOW
        display_string_color 9,13,buffer_ascii+2,2,REG_COLOR_HIGH
     ;BX Register
        display_string_color 4,14,REG16_NAMES+2,2,REG_COLOR_HALF 
        ConvertToASCII BX_REG_1,buffer_ascii
        display_string_color 7,14,buffer_ascii,2,REG_COLOR_LOW
        display_string_color 9,14,buffer_ascii+2,2,REG_COLOR_HIGH
     ;CX Register
        display_string_color 4,15,REG16_NAMES+4,2,REG_COLOR_HALF 
        ConvertToASCII CX_REG_1,buffer_ascii
        display_string_color 7,15,buffer_ascii,2,REG_COLOR_LOW
        display_string_color 9,15,buffer_ascii+2,2,REG_COLOR_HIGH
     ;DX Register
        display_string_color 4,16,REG16_NAMES+6,2,REG_COLOR_HALF
        ConvertToASCII DX_REG_1,buffer_ascii
        display_string_color 7,16,buffer_ascii,2,REG_COLOR_LOW
        display_string_color 9,16,buffer_ascii+2,2,REG_COLOR_HIGH
     ;SI Register
        display_string_color 12,13,REG16_NAMES+8,2,REG_COLOR_FULL
        ConvertToASCII SI_REG_1,buffer_ascii
        display_string_color 15,13,buffer_ascii,2,REG_COLOR_LOW
        display_string_color 17,13,buffer_ascii+2,2,REG_COLOR_HIGH
     ;DI Register
        display_string_color 12,14,REG16_NAMES+10,2,REG_COLOR_FULL
        ConvertToASCII DI_REG_1,buffer_ascii
        display_string_color 15,14,buffer_ascii,2,REG_COLOR_LOW
        display_string_color 17,14,buffer_ascii+2,2,REG_COLOR_HIGH
     ;BP Register
        display_string_color 12,15,REG16_NAMES+12,2,REG_COLOR_FULL
        ConvertToASCII BP_REG_1,buffer_ascii
        display_string_color 15,15,buffer_ascii,2,REG_COLOR_LOW
        display_string_color 17,15,buffer_ascii+2,2,REG_COLOR_HIGH
    ;SP Register
        display_string_color 12,16,REG16_NAMES+14,2,REG_COLOR_FULL
        ConvertToASCII SP_REG_1,buffer_ascii
        display_string_color 15,16,buffer_ascii,2,REG_COLOR_LOW
        display_string_color 17,16,buffer_ascii+2,2,REG_COLOR_HIGH



; Refresh player 2's registers

    ;AX Register
        display_string_color 21,13,REG16_NAMES,2,REG_COLOR_HALF
        ConvertToASCII AX_REG_2,buffer_ascii
        display_string_color 24,13,buffer_ascii,2,REG_COLOR_LOW
        display_string_color 26,13,buffer_ascii+2,2,REG_COLOR_HIGH
     ;BX Register
        display_string_color 21,14,REG16_NAMES+2,2,REG_COLOR_HALF 
        ConvertToASCII BX_REG_2,buffer_ascii
        display_string_color 24,14,buffer_ascii,2,REG_COLOR_LOW
        display_string_color 26,14,buffer_ascii+2,2,REG_COLOR_HIGH     
     ;CX Register
        display_string_color 21,15,REG16_NAMES+4,2,REG_COLOR_HALF 
        ConvertToASCII CX_REG_2,buffer_ascii
        display_string_color 24,15,buffer_ascii,2,REG_COLOR_LOW
        display_string_color 26,15,buffer_ascii+2,2,REG_COLOR_HIGH
     ;DX Register
        display_string_color 21,16,REG16_NAMES+6,2,REG_COLOR_HALF
        ConvertToASCII DX_REG_2,buffer_ascii
        display_string_color 24,16,buffer_ascii,2,REG_COLOR_LOW
        display_string_color 26,16,buffer_ascii+2,2,REG_COLOR_HIGH
     ;SI Register
        display_string_color 29,13,REG16_NAMES+8,2,REG_COLOR_FULL
        ConvertToASCII SI_REG_2,buffer_ascii
        display_string_color 32,13,buffer_ascii,2,REG_COLOR_LOW
        display_string_color 34,13,buffer_ascii+2,2,REG_COLOR_HIGH
     ;DI Register
        display_string_color 29,14,REG16_NAMES+10,2,REG_COLOR_FULL
        ConvertToASCII DI_REG_2,buffer_ascii
        display_string_color 32,14,buffer_ascii,2,REG_COLOR_LOW
        display_string_color 34,14,buffer_ascii+2,2,REG_COLOR_HIGH
     ;BP Register
        display_string_color 29,15,REG16_NAMES+12,2,REG_COLOR_FULL
        ConvertToASCII BP_REG_2,buffer_ascii
        display_string_color 32,15,buffer_ascii,2,REG_COLOR_LOW
        display_string_color 34,15,buffer_ascii+2,2,REG_COLOR_HIGH
    ;SP Register
        display_string_color 29,16,REG16_NAMES+14,2,REG_COLOR_FULL
        ConvertToASCII SP_REG_2,buffer_ascii
        display_string_color 32,16,buffer_ascii,2,REG_COLOR_LOW
        display_string_color 34,16,buffer_ascii+2,2,REG_COLOR_HIGH

    ;Refresh score
        display_string_color 0,0,PLAYER1_NAME_,5,04fH
        display_string_color 20,0,PLAYER2_NAME_,5,04fH

        ConvertDecimalToASCII PLAYER1_POINTS,buffer_ascii
        display_string_color 7,1,buffer_ascii,2,0fbfah

        ConvertDecimalToASCII PLAYER2_POINTS,buffer_ascii
        display_string_color 24,1,buffer_ascii,2,0fbfah
        
    ; Refresh colored points

    ; Refresh player 1's memory contents
        CALL RefreshMemory
    CALL Delay
    RET
RefreshScreenContents ENDP
displayMainMenu PROC FAR
    display_string_color 10,5,string_chatMsg,string_chatMsgSize,0Dh
    display_string_color 8,10,string_gameMsg,string_gameMsgSize,0Ah
    display_string_color 7,15,string_exitMsg,string_exitMsgSize,04h
    RET
displayMainMenu ENDP

; Procedure name: displaySetupMenu
; Function: Displays setup screen where players enter name and initial points
; Inputs: NONE
; Outputs: NONE
displaySetupMenu PROC FAR
    clear_screen
    ;----PLAYER 1 INPUT----;
    ; Get player 1 name
    display_string_color 0,1,string_enterNamep1Msg,string_enterNamep1MsgSize,0Dh
    newline
    CALL flushBuffer
    read_string PLAYER1_NAME
    ; Get player 1's initial pts
    display_string_color 0,5,string_initialPtsMsg,string_initialPtsMsgSize,0Dh
    newline
    CALL flushBuffer
    read_string input_buffer
    ConvertToDecimal input_buffer+2,2,buffer_num,PLAYER1_POINTS
    
    display_string_color 0,10,string_pressEnterKeyMsg,string_pressEnterKeyMsgSize,0Dh
    EnterKey1:
        CALL flushBuffer
        CALL getKeyPressed
        CMP AH,ENTER_SCODE
    JNZ EnterKey1
    clear_screen 
    CALL flushBuffer
    ;----PLAYER 2 INPUT----;
    ; Get player 2 name
    display_string_color 0,1,string_enterNamep2Msg,string_enterNamep2MsgSize,0Dh
    newline
    CALL flushBuffer
    read_string PLAYER2_NAME
    ; Get player 2's initial pts
    display_string_color 0,5,string_initialPtsMsg,string_initialPtsMsgSize,0Dh
    newline
    CALL flushBuffer
    read_string input_buffer
    ConvertToDecimal input_buffer+3,2,buffer_num,PLAYER2_POINTS
    
    display_string_color 0,10,string_pressEnterKeyMsg,string_pressEnterKeyMsgSize,0Dh
    EnterKey2:
        CALL flushBuffer
        CALL getKeyPressed
        CMP AH,ENTER_SCODE
    JNZ EnterKey2
    clear_screen 

    RET
displaySetupMenu ENDP

; Procedure name: displaySetupLevelMenu
; Function: Displays setup screen where players choose a level
; Inputs: NONE
; Outputs: NONE
displaySetupLevelMenu PROC FAR
    clear_screen
    display_string_color 0,1,string_level1Msg,string_level1MsgSize,0Dh
    display_string_color 0,4,string_level1Rule1,string_level1Rule1Size,0Dh
    display_string_color 0,6,string_level1Rule2,string_level1Rule2Size,0Dh
    display_string_color 0,8,string_level1Rule3,string_level1Rule3Size,0Dh
    display_string_color 0,12,string_level2Msg,string_level2MsgSize,0Ah
    display_string_color 0,14,string_level2Rule1,string_level2Rule1Size,0Ah
    display_string_color 0,16,string_level2Rule2,string_level2Rule2Size,0Ah
    display_string_color 0,18,string_level2Rule3,string_level2Rule3Size,0Ah
        
    Key:
        CALL flushBuffer
        CALL getKeyPressed
        CMP AH,02
        JZ L1
        CMP AH,03
        JZ L2
    JNZ Key
    L1:
    MOV current_game_level,LEVEL1
    JMP DONE
    
    L2:
    MOV current_game_level,LEVEL2
    JMP DONE
    
    ;clear_screen 
    
    DONE:

    RET
displaySetupLevelMenu ENDP
;description
checkForbiddenCharacter PROC FAR
    RET
checkForbiddenCharacter ENDP
checkInstruction PROC FAR
    ; LOCAL CHECK_ADD,CHECK_AND,CHECK_DEC,CHECK_INC,CHECK_MOV,CHECK_NOP,CHECK_OR,CHECK_POP,CHECK_PUSH
    ; LOCAL CHECK_ROL,CHECK_ROR,CHECK_SAR,CHECK_SHL,CHECK_SHR,CHECK_SUB,CHECK_XOR,INVALID_INPUT,PLAYER2_TURN_CHG
    ; LOCAL PUSH_INSTRUCTION,POP_INSTRUCTION,OR_INSTRUCTION,AND_INSTRUCTION,ADD_INSTRUCTION,ROL_INSTRUCTION,ROR_INSTRUCTION,SAR_INSTRUCTION,SHL_INSTRUCTION,SHR_INSTRUCTION
    ; LOCAL INC_INSTRUCTION,DEC_INSTRUCTION,XOR_INSTRUCTION,SUB_INSTRUCTION,NEXT_TURN,MOV_INSTRUCTION,NOP_INSTRUCTION
    ; LOCAL CHECK_ADC,CHECK_SBB,CHECK_CLC,CHECK_RCR,CHECK_RCL,CLC_INSTRUCTION,RCR_INSTRUCTION,SBB_INSTRUCTION,ADC_INSTRUCTION,RCL_INSTRUCTION
    ; Check if the forbidden character is in the input string
        check_forbidden:
        ; Ptr to string
        LEA SI,input_str
        ; Size of string
        MOV CL,INPUT_STRING_ACTUAL
        MOV CH,0
        CMP current_player_turn,PLAYER1
        JNZ PLAYER2_FORBIDDEN
        ; Player 1 is playing, check his instruction 
        
        PLAYER1_FORBIDDEN:
        MOV AL,PLAYER1_FORBIDDEN_CHARACTER
        CMP BYTE PTR [SI],AL
        JZ INVALID_INPUT
        LOOP PLAYER1_FORBIDDEN
        JMP NOT_FORBIDDEN
        ; Player 2 is playing, check his instruction 
        PLAYER2_FORBIDDEN:
        MOV AL,PLAYER2_FORBIDDEN_CHARACTER
        CMP BYTE PTR [SI],AL        
        JZ INVALID_INPUT
        LOOP PLAYER2_FORBIDDEN
        JMP NOT_FORBIDDEN
    NOT_FORBIDDEN:
    ;Check all instructions from look up table
        ;check push
        LEA SI,input_str
        LEA DI,INSTRUCTIONS_CONV
        MOV CX,5
        CHECK_PUSH:
            CMP CX,0
            JZ PUSH_INSTRUCTION 
            MOV AL,[SI]
            MOV BL,[DI]   
            INC SI
            INC DI
            DEC CX
            CMP AL,BL
        JZ CHECK_PUSH 
        
        ;CHECK OR
        MOV CX,3
        LEA SI,input_str
        LEA DI,INSTRUCTIONS_CONV
        CHECK_OR:
            CMP CX,0
            JZ OR_INSTRUCTION
            MOV AL,[SI+5]
            MOV BL,[DI]   
            INC SI
            INC DI
            DEC CX
            CMP AL,BL
        JZ CHECK_OR
        ;CHECK POP
        MOV CX,4
        LEA SI,input_str
        LEA DI,INSTRUCTIONS_CONV
        CHECK_POP:
            CMP CX,0
            JZ    POP_INSTRUCTION
            MOV AL,[SI+8]
            MOV BL,[DI]   
            INC SI
            INC DI
            DEC CX
            CMP AL,BL
        JZ CHECK_POP
        ;CHECK MOV
        MOV CX,4
        LEA SI,INSTRUCTIONS_CONV
        LEA DI,input_str
        CHECK_MOV:
            CMP CX,0
            JZ MOV_INSTRUCTION
            MOV AL,[SI+12]
            MOV BL,[DI]   
            INC SI
            INC DI
            DEC CX
            CMP AL,BL
        JZ CHECK_MOV
        ;CHECK NOP
        MOV CX,4
        LEA SI,input_str
        LEA DI,INSTRUCTIONS_CONV
        CHECK_NOP:
            CMP CX,0
            JZ    NOP_INSTRUCTION
            MOV AL,[SI+16]
            MOV BL,[DI]   
            INC SI
            INC DI
            DEC CX
            CMP AL,BL
        JZ CHECK_NOP
        ;CHECK ADD
        MOV CX,4
        LEA SI,input_str
        LEA DI,INSTRUCTIONS_CONV
        CHECK_ADD:
            CMP CX,0
            JZ    ADD_INSTRUCTION
            MOV AL,[SI+20]
            MOV BL,[DI]   
            INC SI
            INC DI
            DEC CX
            CMP AL,BL
        JZ CHECK_ADD
        ;CHECK SUB
        MOV CX,4
        LEA SI,input_str
        LEA DI,INSTRUCTIONS_CONV
        CHECK_SUB:
            CMP CX,0
            JZ    SUB_INSTRUCTION
            MOV AL,[SI+24]
            MOV BL,[DI]   
            INC SI
            INC DI
            DEC CX
            CMP AL,BL
        JZ CHECK_SUB
        ;CHECK XOR
        MOV CX,4
        LEA SI,input_str
        LEA DI,INSTRUCTIONS_CONV
        CHECK_XOR:
            CMP CX,0
            JZ    XOR_INSTRUCTION
            MOV AL,[SI+28]
            MOV BL,[DI]   
            INC SI
            INC DI
            DEC CX
            CMP AL,BL
        JZ CHECK_XOR
        ;CHECK AND
        MOV CX,4
        LEA SI,input_str
        LEA DI,INSTRUCTIONS_CONV
        CHECK_AND:
            CMP CX,0
            JZ    AND_INSTRUCTION
            MOV AL,[SI+32]
            MOV BL,[DI]   
            INC SI
            INC DI
            DEC CX
            CMP AL,BL
        JZ CHECK_AND
        ;CHECK SHR              
        MOV CX,4
        LEA SI,input_str
        LEA DI,INSTRUCTIONS_CONV
        CHECK_SHR:
            CMP CX,0
            JZ    SHR_INSTRUCTION
            MOV AL,[SI+36]
            MOV BL,[DI]   
            INC SI
            INC DI
            DEC CX
            CMP AL,BL
        JZ CHECK_SHR
        ;CHECK SHL
        MOV CX,4
        LEA SI,input_str
        LEA DI,INSTRUCTIONS_CONV 
        CHECK_SHL:
            CMP CX,0
            JZ    SHL_INSTRUCTION
            MOV AL,[SI+40]
            MOV BL,[DI]   
            INC SI
            INC DI
            DEC CX
            CMP AL,BL
        JZ CHECK_SHL
        ;CHECK SAR
        CHECK_SAR:
            CMP CX,0
            JZ    SAR_INSTRUCTION
            MOV AL,[SI+44]
            MOV BL,[DI]   
            INC SI
            INC DI
            DEC CX
            CMP AL,BL
        JZ CHECK_SAR
        ;CHECK ROR
        MOV CX,4
        LEA SI,input_str
        LEA DI,INSTRUCTIONS_CONV  
        CHECK_ROR:
            CMP CX,0
            JZ    ROR_INSTRUCTION
            MOV AL,[SI+48]
            MOV BL,[DI]   
            INC SI
            INC DI
            DEC CX
            CMP AL,BL
        JZ CHECK_ROR
        ;CHECK INC
        MOV CX,4
        LEA SI,input_str
        LEA DI,INSTRUCTIONS_CONV  
        CHECK_INC:
            CMP CX,0
            JZ    INC_INSTRUCTION
            MOV AL,[SI+52]
            MOV BL,[DI]   
            INC SI
            INC DI
            DEC CX
            CMP AL,BL
        JZ CHECK_INC
        ;CHECK DEC
        MOV CX,4
        LEA SI,input_str
        LEA DI,INSTRUCTIONS_CONV  
        CHECK_DEC:
            CMP CX,0
            JZ    DEC_INSTRUCTION
            MOV AL,[SI+56]
            MOV BL,[DI]   
            INC SI
            INC DI
            DEC CX
            CMP AL,BL
        JZ CHECK_DEC
        ;CHECK ROL
        MOV CX,4
        LEA SI,input_str
        LEA DI,INSTRUCTIONS_CONV  
        CHECK_ROL:
            CMP CX,0
            JZ    ROL_INSTRUCTION
            MOV AL,[SI+60]
            MOV BL,[DI]   
            INC SI
            INC DI
            DEC CX
            CMP AL,BL
        JZ CHECK_ROL
        ;CHECK ADC
        MOV CX,4
        LEA SI,TEST_STRING
        LEA DI,INSTRUCTIONS_CONV
        CHECK_ADC:
            CMP CX,0
            JZ ADC_INSTRUCTION
            MOV AL,[SI+64]
            MOV BL,[DI]
            INC SI
            INC DI
            DEC CX
            CMP AL,BL
        JZ CHECK_ADC
        ;CHECK SBB
        MOV CX,4
        LEA SI,TEST_STRING
        LEA DI,INSTRUCTIONS_CONV
        CHECK_SBB:
            CMP CX,0
            JZ SBB_INSTRUCTION
            MOV AL,[SI+68]
            MOV BL,[DI]
            INC SI
            INC DI
            DEC CX
            CMP AL,BL
        JZ CHECK_SBB
        ;CHECK CLC
        MOV CX,4
        LEA SI,TEST_STRING
        LEA DI,INSTRUCTIONS_CONV
        CHECK_CLC:
            CMP CX,0
            JZ CLC_INSTRUCTION
            MOV AL,[SI+72]
            MOV BL,[DI]
            INC SI
            INC DI
            DEC CX
            CMP AL,BL
        JZ CHECK_CLC
        ;CHECK RCR
        MOV CX,4
        LEA SI,TEST_STRING
        LEA DI,INSTRUCTIONS_CONV
        CHECK_RCR:
            CMP CX,0
            JZ RCR_INSTRUCTION
            MOV AL,[SI+76]
            MOV BL,[DI]
            INC SI
            INC DI
            DEC CX
            CMP AL,BL
        JZ CHECK_RCR
        ;CHECK RCL
        MOV CX,4
        LEA SI,TEST_STRING
        LEA DI,INSTRUCTIONS_CONV
        CHECK_RCL:
            CMP CX,0
            JZ RCL_INSTRUCTION
            MOV AL,[SI+80]
            MOV BL,[DI]
            INC SI
            INC DI
            DEC CX
            CMP AL,BL
        JZ CHECK_RCL
    

    JMP   INVALID_INPUT 
    
   ; Instruction Execution
    PUSH_INSTRUCTION:
        getOperands input_str
        checkOperands
        CMP current_instruction_width,0
         JZ    INVALID_INPUT
        PUSH_EXECUTE
    OR_INSTRUCTION:
        getOperands input_str
        checkOperands
        OR_EXECUTE current_instruction_width
    POP_INSTRUCTION:
        getOperands input_str
        checkOperands
        POP_EXECUTE
    MOV_INSTRUCTION:
        ; Get operands from the input string 
        getOperands input_str
        MOV AX,@DATA
        MOV ES,AX
        
        MOV_OPERAND:
        MOV SI,OFFSET OP1_STRING    ;get OFFSET OF OP1
        MOV AL,[SI]
        MOV DI,OFFSET REG16_NAMES   ; COMPARE IF OP1 IS 16-BIT REG
        MOV BL,[DI]
        MOV CX,8 
        ;;;;;;;;;;;;;;;;;;;;;;;; CHECK OPERAND1 16 BIT REG ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; 
        CHECK_REG16:
        CMP CX,0         ; IF ZERO MEAN CHECK IF OP1 8 BIT REG
        ;JZ CHECK_REG8
        CMPSW            ; IF OP1 EXIST CHECK OP2 MATCHING WITH OP1
        JZ CHECK_OP2_16  ; CHECK ADDRESS OP2
        INC DI
        INC DI 
        DEC CX
        JNZ CHECK_REG16 ; IF NOT CHECK NEXT OPERAND IN ARRAY
        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;CHECK OP2 REGISTER IS 16 BIT REG;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        CHECK_OP2_16: 
        MOV BX,2
        MOV current_instruction_width,BX
        MOV AX,[DI-2]
        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;COMPARE TO ECEXUTE REG;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        ; COMPARE AX TO EXECUTE 
        CMP AX ,'XA'
        JZ OP1_EXEC_AX
        JNZ OP1_CHECK_BX
        OP1_EXEC_AX:
        CMP current_exec_cpu , PLAYER1
        JNZ OP1_CPU_PLAYER2_AX	
        LEA SI,AX_REG_1
        ;LEA SI,AX_REG_1
        MOV current_exec_op1,SI
        JMP CHECK_OP2
        OP1_CPU_PLAYER2_AX:
        LEA SI,AX_REG_2
        MOV current_exec_op1,SI
         JMP CHECK_OP2
        
        ; COMPARE BX TO EXECUTE
        OP1_CHECK_BX:	
        CMP AX ,'XB'
        JZ OP1_EXEC_BX
        JNZ OP1_CHECK_CX
        OP1_EXEC_BX:
        CMP current_exec_cpu , PLAYER1
        JNZ OP1_CPU_PLAYER2_BX	
        LEA SI,BX_REG_1
        ;LEA SI,AX_REG_1
        MOV current_exec_op1,SI 
        JMP CHECK_OP2
        OP1_CPU_PLAYER2_BX:
        LEA SI,BX_REG_2
        MOV current_exec_op1,SI
        JMP CHECK_OP2
        ; COMPARE CX TO EXECUTE 
        OP1_CHECK_CX:
        CMP AX ,'XC'
        JZ OP1_EXEC_CX
        JNZ OP1_CHECK_DX
        OP1_EXEC_CX:
        CMP current_exec_cpu , PLAYER1
        JNZ OP1_CPU_PLAYER2_CX	
        LEA SI,CX_REG_1
        ;LEA SI,AX_REG_1
        MOV current_exec_op1,SI
        OP1_CPU_PLAYER2_CX:
        LEA SI,CX_REG_2
        MOV current_exec_op1,SI
        
        
        ; COMPARE DX TO EXECUTE
        OP1_CHECK_DX:	
        CMP AX ,'XD'
        JZ OP1_EXEC_DX
        JNZ OP1_CHECK_SI
        OP1_EXEC_DX:
        CMP current_exec_cpu , PLAYER1
        JNZ OP1_CPU_PLAYER2_DX
        LEA SI,DX_REG_1
        ;LEA SI,AX_REG_1
        MOV current_exec_op1,SI
        OP1_CPU_PLAYER2_DX:
        LEA SI,DX_REG_2
        MOV current_exec_op1,SI
        
        ; COMPARE SI TO EXECUTE 
        OP1_CHECK_SI:
        CMP AX ,'IS'
        JZ OP1_EXEC_SI
        JNZ OP1_CHECK_DI
        OP1_EXEC_SI:
        CMP current_exec_cpu , PLAYER1
        JNZ OP1_CPU_PLAYER2_SI	
        LEA SI,SI_REG_1
        ;LEA SI,AX_REG_1
        MOV current_exec_op1,SI
        OP1_CPU_PLAYER2_SI:
        LEA SI,SI_REG_2
        MOV current_exec_op1,SI
        
            ; COMPARE DI TO EXECUTE 
        OP1_CHECK_DI:
        CMP AX ,'ID'
        JZ OP1_EXEC_DI
        JNZ OP1_CHECK_BP
        OP1_EXEC_DI:
        CMP current_exec_cpu , PLAYER1
        JNZ OP1_CPU_PLAYER2_DI	
        LEA SI,DI_REG_1
        ;LEA SI,AX_REG_1
        MOV current_exec_op1,SI
        OP1_CPU_PLAYER2_DI:
        LEA SI,DI_REG_2
        MOV current_exec_op1,SI
        
        
        ; COMPARE BP TO EXECUTE 
        OP1_CHECK_BP:
        CMP AX ,'PB'
        JZ OP1_EXEC_BP
        JNZ OP1_CHECK_SP
        OP1_EXEC_BP:
        CMP current_exec_cpu , PLAYER1
        JNZ OP1_CPU_PLAYER2_BP	
        LEA SI,BP_REG_1
        ;LEA SI,AX_REG_1
        MOV current_exec_op1,SI
        OP1_CPU_PLAYER2_BP:
        LEA SI,BP_REG_2
        MOV current_exec_op1,SI
        
            ; COMPARE SP TO EXECUTE 
        OP1_CHECK_SP:
        CMP AX ,'PS'
        JZ OP1_EXEC_SP
        JNZ INVALID_INPUT
        OP1_EXEC_SP:
        CMP current_exec_cpu , PLAYER1
        JNZ OP1_CPU_PLAYER2_SP	
        LEA SI,SP_REG_1
        ;LEA SI,AX_REG_1
        MOV current_exec_op1,SI
        OP1_CPU_PLAYER2_SP:
        LEA SI,SP_REG_2
        MOV current_exec_op1,SI
        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;FINISH SET OP1 VALID ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
          CHECK_OP2:
        MOV SI,OFFSET OP2_STRING
        MOV AL,[SI]
        MOV DI,OFFSET REG16_NAMES
        MOV BL,[DI]
        MOV CX,8
       
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;CHECK OPERAND2-16-BIT REG;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;  
        CHECK_REG16_OP2:
        CMP CX,0
        ;JZ OP2_ADDRESS                ; IF OP2 IS NOT REGISTER JZ TO CHECK IF IT IS ADDRESS
        CMPSW                         ; CPMPARE OP2 16 BIT IF YES JUMP VALID AND OP2 = 16
        JZ OP2_INTIALIZE_16           ; valid And JUMP MOVE INSTRUCTION
        DEC SI
        DEC SI
        DEC CX
        JNZ CHECK_REG16_OP2           ; IF NOT ZERO CHECK FOR OTHER REG IN ARRAY
        
        
       
        OP2_INTIALIZE_16:
        MOV AX,[DI-2]
        ; COMPARE AX TO EXECUTE 
        CMP AX ,'XA'
        JZ OP2_EXEC_AX
        JNZ OP2_CHECK_BX
        OP2_EXEC_AX:
        CMP current_exec_cpu , PLAYER1
        JNZ OP2_CPU_PLAYER2_AX	
        LEA SI,AX_REG_1
        ;LEA SI,AX_REG_1
        MOV current_exec_op2,SI
        JMP away
        OP2_CPU_PLAYER2_AX:
        LEA SI,AX_REG_2
        MOV current_exec_op2,SI
        JMP away       
        
        ; COMPARE BX TO EXECUTE
        OP2_CHECK_BX:	
        CMP AX ,'XB'
        JZ OP2_EXEC_BX
        JNZ OP2_CHECK_CX
        OP2_EXEC_BX:
        CMP current_exec_cpu , PLAYER1
        JNZ OP2_CPU_PLAYER2_BX	
        LEA SI,BX_REG_1
        ;LEA SI,AX_REG_1
        MOV current_exec_op2,SI    
        JMP away 
        OP2_CPU_PLAYER2_BX:
        LEA SI,BX_REG_2
        MOV current_exec_op2,SI
        JMP away 
        ; COMPARE CX TO EXECUTE 
        OP2_CHECK_CX:
        CMP AX ,'XC'
        JZ OP2_EXEC_CX
        JNZ OP2_CHECK_DX
        OP2_EXEC_CX:
        CMP current_exec_cpu , PLAYER1
        JNZ OP2_CPU_PLAYER2_CX	
        LEA SI,CX_REG_1
        ;LEA SI,AX_REG_1
        MOV current_exec_op2,SI
        OP2_CPU_PLAYER2_CX:
        LEA SI,CX_REG_2
        MOV current_exec_op2,SI
        
        
        ; COMPARE DX TO EXECUTE
        OP2_CHECK_DX:	
        CMP AX ,'XD'
        JZ OP2_EXEC_DX
        JNZ OP2_CHECK_SI
        OP2_EXEC_DX:
        CMP current_exec_cpu , PLAYER1
        JNZ OP2_CPU_PLAYER2_DX	
        LEA SI,DX_REG_1
        ;LEA SI,AX_REG_1
        MOV current_exec_op2,SI
        OP2_CPU_PLAYER2_DX:
        LEA SI,DX_REG_2
        MOV current_exec_op2,SI
        
        ; COMPARE SI TO EXECUTE 
        OP2_CHECK_SI:
        CMP AX ,'IS'
        JZ OP2_EXEC_SI
        JNZ OP2_CHECK_DI
        OP2_EXEC_SI:
        CMP current_exec_cpu , PLAYER1
        JNZ OP2_CPU_PLAYER2_SI	
        LEA SI,SI_REG_1
        ;LEA SI,AX_REG_1
        MOV current_exec_op2,SI
        OP2_CPU_PLAYER2_SI:
        LEA SI,SI_REG_2
        MOV current_exec_op2,SI
        
            ; COMPARE DI TO EXECUTE 
        OP2_CHECK_DI:
        CMP AX ,'ID'
        JZ OP2_EXEC_DI
        JNZ OP2_CHECK_BP
        OP2_EXEC_DI:
        CMP current_exec_cpu , PLAYER1
        JNZ OP2_CPU_PLAYER2_DI	
        LEA SI,DI_REG_1
        ;LEA SI,AX_REG_1
        MOV current_exec_op2,SI
        OP2_CPU_PLAYER2_DI:
        LEA SI,DI_REG_2
        MOV current_exec_op2,SI
        
        
        ; COMPARE BP TO EXECUTE 
        OP2_CHECK_BP:
        CMP AX ,'PB'
        JZ OP2_EXEC_BP
        JNZ OP2_CHECK_SP
        OP2_EXEC_BP:
        CMP current_exec_cpu , PLAYER1
        JNZ OP2_CPU_PLAYER2_BP	
        LEA SI,BP_REG_1
        ;LEA SI,AX_REG_1
        MOV current_exec_op2,SI
        OP2_CPU_PLAYER2_BP:
        LEA SI,BP_REG_2
        MOV current_exec_op2,SI
        
            ; COMPARE SP TO EXECUTE 
        OP2_CHECK_SP:
        CMP AX ,'PS'
        JZ OP2_EXEC_SP
        JNZ INVALID_INPUT
        OP2_EXEC_SP:
        CMP current_exec_cpu , PLAYER1
        JNZ OP2_CPU_PLAYER2_SP	
        LEA SI,SP_REG_1
        ;LEA SI,AX_REG_1
        MOV current_exec_op1,SI
        OP2_CPU_PLAYER2_SP:
        LEA SI,SP_REG_2
        MOV current_exec_op1,SI  
        
        away:
        MOV_EXECUTE current_instruction_width
        CMP power_up_exec_both,0ffh
        JNZ NEXT_TURN
        NOT power_up_exec_both
        NOT current_exec_cpu
        JMP MOV_INSTRUCTION

        JMP NEXT_TURN


    NOP_INSTRUCTION:
        getOperands input_str
        checkOperands

        CMP power_up_exec_both,0ffh
        JNZ NEXT_TURN
        NOT power_up_exec_both
        NOT current_exec_cpu
        JMP NOP_INSTRUCTION

        JMP NEXT_TURN

    ADD_INSTRUCTION:
        getOperands input_str
        checkOperands

        ADD_EXECUTE current_instruction_width
        CMP power_up_exec_both,0ffh
        JNZ NEXT_TURN
        NOT power_up_exec_both
        NOT current_exec_cpu
        JMP ADD_INSTRUCTION

        JMP NEXT_TURN

    SUB_INSTRUCTION:
        getOperands input_str
        checkOperands
        SUB_EXECUTE current_instruction_width
        CMP power_up_exec_both,0ffh
        JNZ NEXT_TURN
        NOT power_up_exec_both
        NOT current_exec_cpu
        JMP SUB_INSTRUCTION

        JMP NEXT_TURN
    XOR_INSTRUCTION:
        getOperands input_str
        checkOperands
        XOR_EXECUTE current_instruction_width
          CMP power_up_exec_both,0ffh
        JNZ NEXT_TURN
        NOT power_up_exec_both
        NOT current_exec_cpu
        JMP XOR_INSTRUCTION

         JMP NEXT_TURN
    AND_INSTRUCTION:
        getOperands input_str
        checkOperands
        AND_EXECUTE current_instruction_width
          CMP power_up_exec_both,0ffh
        JNZ NEXT_TURN
        NOT power_up_exec_both
        NOT current_exec_cpu
        JMP AND_INSTRUCTION

         JMP NEXT_TURN
    SHR_INSTRUCTION:
        getOperands input_str
        checkOperands
        SHR_EXECUTE current_instruction_width
          CMP power_up_exec_both,0ffh
        JNZ NEXT_TURN
        NOT power_up_exec_both
        NOT current_exec_cpu
        JMP SHR_INSTRUCTION

         JMP NEXT_TURN
    SHL_INSTRUCTION:
        getOperands input_str
        checkOperands
        SHL_EXECUTE current_instruction_width
          CMP power_up_exec_both,0ffh
        JNZ NEXT_TURN
        NOT power_up_exec_both
        NOT current_exec_cpu
        JMP SHL_INSTRUCTION

         JMP NEXT_TURN
    SAR_INSTRUCTION:
        getOperands input_str
        checkOperands
        SAR_EXECUTE current_instruction_width
          CMP power_up_exec_both,0ffh
        JNZ NEXT_TURN
        NOT power_up_exec_both
        NOT current_exec_cpu
        JMP SAR_INSTRUCTION

        JMP NEXT_TURN
    ROR_INSTRUCTION:
        getOperands input_str
        checkOperands
        ROR_EXECUTE current_instruction_width
         CMP power_up_exec_both,0ffh
        JNZ NEXT_TURN
        NOT power_up_exec_both
        NOT current_exec_cpu
        JMP ROR_INSTRUCTION

        JMP NEXT_TURN
    ROL_INSTRUCTION:
        getOperands input_str
        checkOperands
        ROL_EXECUTE current_instruction_width
         CMP power_up_exec_both,0ffh
        JNZ NEXT_TURN
        NOT power_up_exec_both
        NOT current_exec_cpu
        JMP ROL_INSTRUCTION

        JMP NEXT_TURN
    INC_INSTRUCTION:
        getOperands input_str
        checkOperands
        INC_EXECUTE current_instruction_width
        CMP power_up_exec_both,0ffh
        JNZ NEXT_TURN
        NOT power_up_exec_both
        NOT current_exec_cpu
        JMP INC_INSTRUCTION

        JMP NEXT_TURN
    DEC_INSTRUCTION:
        getOperands input_str
        checkOperands
        DEC_EXECUTE current_instruction_width
         CMP power_up_exec_both,0ffh
        JNZ NEXT_TURN
        NOT power_up_exec_both
        NOT current_exec_cpu
        JMP DEC_INSTRUCTION

        JMP NEXT_TURN
    RCR_INSTRUCTION:
        getOperands input_str
        checkOperands
        RCR_EXECUTE current_instruction_width
         CMP power_up_exec_both,0ffh
        JNZ NEXT_TURN
        NOT power_up_exec_both
        NOT current_exec_cpu
        JMP RCR_INSTRUCTION

        JMP NEXT_TURN
    RCL_INSTRUCTION:
        getOperands input_str
        checkOperands
        RCL_EXECUTE current_instruction_width
         CMP power_up_exec_both,0ffh
        JNZ NEXT_TURN
        NOT power_up_exec_both
        NOT current_exec_cpu
        JMP RCL_INSTRUCTION

        JMP NEXT_TURN
    ADC_INSTRUCTION:
        getOperands input_str
        checkOperands
        ADC_EXECUTE current_instruction_width
        CMP power_up_exec_both,0ffh
        JNZ NEXT_TURN
        NOT power_up_exec_both
        NOT current_exec_cpu
        JMP ADC_INSTRUCTION

        JMP NEXT_TURN
    SBB_INSTRUCTION:
        getOperands input_str
        checkOperands
        SBB_EXECUTE current_instruction_width
        CMP power_up_exec_both,0ffh
        JNZ NEXT_TURN
        NOT power_up_exec_both
        NOT current_exec_cpu
        JMP SBB_INSTRUCTION

        JMP NEXT_TURN
    CLC_INSTRUCTION:
        getOperands input_str
        checkOperands
        CLC_EXECUTE
         CMP power_up_exec_both,0ffh
        JNZ NEXT_TURN
        NOT power_up_exec_both
        NOT current_exec_cpu
        JMP CLC_INSTRUCTION

        JMP NEXT_TURN


    INVALID_INPUT:
   ; Subtract points from player who entered bad input
        CMP current_player_turn,0FFH
        JZ PLAYER2_TURN_CHG
        MOV AX,PLAYER1_POINTS
        SUB AX,1
        MOV PLAYER1_POINTS,AX
        JMP NEXT_TURN
        
    PLAYER2_TURN_CHG:
        MOV AX,PLAYER2_POINTS
        SUB AX,1
        MOV PLAYER2_POINTS,AX

    ; Switch over current turn
    NEXT_TURN:
        MOV AL,current_player_turn
        NOT AL
        MOV current_player_turn,AL
    RET
checkInstruction ENDP


read_instruction MACRO
    LOCAL get_string,finish_input,full_input,store_length
    OR   AX,0FFFFH
    ; Max number of characters to be entered
    MOV  CL,15
    LEA  SI,input_str
    get_string:  
        call getKeyPressed
        JZ get_string
    ; Check F4
        CMP  AH,F4_SCODE
        JZ   FINISH_GAME
    ; Check enter key
        CMP  AL,10
        JZ   finish_input
        CMP  AL,13
        JZ   finish_input
    ; F4 key was not pressed, so get the rest of string
        MOV  [SI],AL              ; Move ASCII
        MOV AH,0eH
        MOV BX,07H
        INT 10h
        call flushBuffer
    ; Point to next character and read next pressed char
        INC  SI
        DEC  CL
JNZ  get_string
        
finish_input:
; Move actual size of the entered string
CMP CL,0
JZ full_input
MOV AL,INPUT_STRING_ACTUAL
SUB AL,CL
MOV CL,AL
JMP store_length
full_input:
MOV CL,INPUT_STRING_ACTUAL
store_length:
MOV  INPUT_STRING_ACTUAL,CL
 

ENDM
