; General constants
    TRUE EQU 0FFH
    FALSE EQU 0
    ESC_SCODE EQU 1
    F1_SCODE EQU 03bh
    F2_SCODE EQU 03ch
    F4_SCODE EQU 03EH
    ENTER_SCODE EQU 01CH
    STATE_SETUP_MENU EQU 0
    STATE_MAIN_MENU EQU 1
    STATE_SETUP_LEVEL EQU 2
    STATE_CHAT_SCREEN EQU 3
    STATE_INGAME EQU 4
    STATE_FINISH_GAME EQU 5
    PLAYER1 EQU 00
    PLAYER2 EQU 0FFH
    
    LEVEL1 EQU 00
    LEVEL2 EQU 0FFH
; Bitmap variables
    BMP_COLOR_RES EQU 256
    SCREEN_COLS EQU 320
    SCREEN_ROWS EQU 200
    MAX_X EQU SCREEN_COLS-1
    SET_PIXEL_COLOR EQU 0CH
    FILEHANDLE      DW  ?   
    TOP_Y           DW  0
    ERRORMSG        DB  'ERROR: Cannot open file'
    SCRLINE         DB  SCREEN_COLS DUP (0)
    FILENAME        DB  'GUI1.bmp',0
    HEADER          DB  54 DUP (0)  
    PALETTE         DB  256*4 DUP (14)
    VIDEO_MEMORY_OFFSET EQU  0A000H
    ; Register colors
    REG_COLOR_FULL EQU 0FFF1H
    REG_COLOR_HALF EQU 0FF04H
    REG_COLOR_LOW EQU 0FFE1H
    REG_COLOR_HIGH EQU 0FF4fH
    MEMORY_CELL_COLOR EQU 0FF72H
; General look up tables
    HEX_CONV    DB '0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'
    REG16_NAMES DB 'AX','BX','CX','DX','SI','DI','BP','SP' ; REGISTER 16 BIT 8 REGISTER
    REG16_Address DB '[BX]','[SI]','[DI]','[BP]','[00]','[01]','[02]','[03]','[04]','[05]','[06]','[07]','[08]','[09]','[0A]','[0B]','[0C]','[0D]','[0E]','[0F]' ; ADDRESS REGISTER 
    NUM_ADDRESS DB '[00]','[01]','[02]','[03]','[04]','[05]','[06]','[07]','[08]','[09]','[0A]','[0B]','[0C]','[0D]','[0E]','[0F]'
    REG16_Address_16 DB '[BX]','[SI]','[DI]','[BP]'
    REG8_NAMES DB 'AL','BL','CL','DL','AH','BH','CH','DH' ; REGISTER 8 BIT 8 REGISTER 
    NUM_HEXA DB '0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F' ;  NUMBER TO CHECK FOR FOUR BIT
    INSTRUCTIONS_CONV DB 'PUSH ','OR ','POP ','MOV ','NOP ','ADD ','SUB ','XOR ','AND ','SHR ','SHL ','SAR ','ROR ','INC ','DEC ','ROL ','ADC ','SBB ','CLC ','RCR ','RCL '
    
INPUT_STRING_ DB 16
INPUT_STRING_ACTUAL DB ? 
input_str db 15 dup(?)
ENTERKEY DB 10
INPUT_END DB '#'
OP1_STRING DB '~~~~'
OP2_STRING DB '~~~~'
OP1_SIZE DB ?
OP2_SIZE DB ?
input_buffer DB 15,?,15 dup(?)
STRING_TEST DB 'PUSH '
PLAYER_ACTUAL_INPUT_STRING  DB ?
PLAYER_INPUT_STRING DB 18,?,18 DUP(?)
current_player_turn DB PLAYER1
current_game_state DB STATE_INGAME
current_exec_op1  DW ? 
current_exec_op2  DW ? 
current_exec_cpu DB PLAYER1
current_game_level DB LEVEL1
power_up_exec_both DB 0
ascii_result dd ?
string_chatMsgSize EQU 19 
string_chatMsg DB 'To chat, press (F1)$'
string_gameMsgSize EQU 27
string_gameMsg DB 'To start a game, press (F2)$'
string_exitMsgSize EQU 29
string_exitMsg DB 'To exit the game, press (ESC)$'
string_winnerMsgSize EQU 17
string_winnerMsg DB 'And the winner is $'
string_enterNamep1MsgSize EQU 29
string_enterNamep1Msg DB 'Please enter your name (P1): $'
string_enterNamep2MsgSize EQU 29
string_enterNamep2Msg DB 'Please enter your name (P2): $'
string_initialPtsMsgSize EQU 16
string_initialPtsMsg DB 'Initial Points: $'
string_level1MsgSize EQU 14
string_level1Msg DB 'Level 1 Rules:$'
string_level2MsgSize EQU 14
string_level2Msg DB'Level 2 Rules:$'
string_chooseLevelMsgSize EQU 29
string_chooseLevelMsg DB 'Please choose a level (1/2): $'
string_pressEnterKeyMsgSize EQU 23
string_pressEnterKeyMsg DB 'Press Enter to continue$'
string_level1Rule1Size EQU 30
string_level1Rule1 DB '1. Forbidden chars are visible$'
string_level1Rule2Size EQU 32
string_level1Rule2 DB '2. Exec. command on opponent CPU$'
string_level1Rule3Size EQU 21
string_level1Rule3 DB '3. Initial values = 0$'

string_level2Rule1Size EQU 34
string_level2Rule1 DB '1. Forbidden chars are not visible$'
string_level2Rule2Size EQU 33
string_level2Rule2 DB '2. Exec. command on CPU of choice$'
string_level2Rule3Size EQU 31
string_level2Rule3 DB '3. Initial values can be chosen$'
; Last key pressed by user
var_key_pressed DB 0
TEST_STRING     DB 'THIS IS A TEST STRING$'
test_max_size   DB 5
test_actual     DB ?
test_str        DB 5 dup(?)
TEST_CHAR       DB 'A'
TEST_WORD       DW  0
TEST_ARR        DW  0,0
ddd             DB  '$'
buffer_num      DW  0
buffer_ascii    DB 4 dup(?),'$'
BITMAP_DISPLAYED_FLAG DB 0
current_instruction_width dw 0
temp_x DB 0
temp_y DB 0
; Registers and variables for player 1's CPU
    AX_REG_1 LABEL WORD
    AL_REG_1 DB 0ABH
    AH_REG_1 DB 0CDH
    
    BX_REG_1 LABEL WORD
    BL_REG_1 DB 03FH
    BH_REG_1 DB 09EH

    CX_REG_1 LABEL WORD
    CL_REG_1 DB 033H
    CH_REG_1 DB 0ACH
    
    DX_REG_1 LABEL WORD
    DL_REG_1 DB 0EAH
    DH_REG_1 DB 0FFH
    
    SI_REG_1 DW 0BFADH
    DI_REG_1 DW 099FH
    BP_REG_1 DW 01H
    SP_REG_1 DW 0155H
    PLAYER1_FORBIDDEN_CHARACTER DB ?
    PLAYER1_CARRY DW 0

    PLAYER1_NAME LABEL BYTE
    PLAYER1_NAME_MAX_SIZE DB 15
    PLAYER1_NAME_ACTUAL_SIZE DB ?
    PLAYER1_NAME_ DB 15 DUP(0)

    PLAYER1_POINTS DW 60
    PLAYER1_MEMORY DB 16 DUP(7)
; Registers and variables for player 2's CPU
    AX_REG_2 LABEL WORD
    AL_REG_2 DB 023H
    AH_REG_2 DB 05H
    
    BX_REG_2 LABEL WORD
    BL_REG_2 DB 099H
    BH_REG_2 DB 0ECH

    CX_REG_2 LABEL WORD
    CL_REG_2 DB ?
    CH_REG_2 DB ?
    
    DX_REG_2 LABEL WORD
    DL_REG_2 DB ?
    DH_REG_2 DB ?

    SI_REG_2 DW ?
    DI_REG_2 DW ?
    BP_REG_2 DW ?
    SP_REG_2 DW ?
    
    PLAYER2_FORBIDDEN_CHARACTER DB ?
    PLAYER2_CARRY DB 0

    PLAYER2_NAME LABEL BYTE
    PLAYER2_NAME_MAX_SIZE DB 15
    PLAYER2_NAME_ACTUAL_SIZE DB ?
    PLAYER2_NAME_ DW 15 DUP(0)

    PLAYER2_POINTS DW 60
    PLAYER2_MEMORY DB 16 DUP(0)
    