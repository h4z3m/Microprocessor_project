.MODEL SMALL
.386
.STACK 64
.DATA
include vars.inc
.CODE
include macros.inc
include procs.inc
MAIN proc FAR
PROGRAM_START:
    ; Setup data segment
        MOV AX,@DATA
        MOV DS,AX
    ;Setup video mode to graphic mode
    MOV AX,13h  ; 320x200 256 colors
   ;MOV AX,06H  ; 640x200 16  colors
    MOV BH,0H   ; First page on screen
    INT 10H    
    ;clear_screen 0,0,0,0
    
; Main program loop
SUPER_LOOP:       
    CALL flushBuffer    
    ; Game state checks are checked every iteration to jump to the current state
        CMP current_game_state,STATE_SETUP_MENU
        JZ  SETUP_MENU    
        CMP current_game_state,STATE_MAIN_MENU
        JZ  MAIN_MENU 
        CMP current_game_state,STATE_INGAME
        JZ  INGAME  
        CMP current_game_state,STATE_FINISH_GAME
        JZ  FINISH_GAME    
        CMP current_game_state,STATE_CHAT_SCREEN
        JZ  CHAT_SCREEN  
    ; Setup menu for setting up name and initial points
    SETUP_MENU:
        CALL displaySetupMenu
        
        MOV current_game_state,STATE_MAIN_MENU
        JMP SUPER_LOOP
    ; Main menu in which the player chooses to chat, start game, or exit 
    MAIN_MENU:
        ;------------------------ MAIN MENU ------------------------
        CALL displayMainMenu
        ;Reset zero flag
        OR AX,0FFFFH
        ;Check for key pressed, if any
        CALL getKeyPressed
        ;No char was pressed
        JZ  MAIN_MENU_NoKey

        MOV var_key_pressed,AH
        
        CMP var_key_pressed,F1_SCODE
        MOV current_game_state,STATE_CHAT_SCREEN
        JZ  CHAT_SCREEN
        
        CMP var_key_pressed,F2_SCODE
        MOV current_game_state,STATE_SETUP_LEVEL
        JZ  SETUP_LEVEL
        
        CMP var_key_pressed,ESC_SCODE
        JZ  EXIT_GAME

        MAIN_MENU_NoKey:
        JMP SUPER_LOOP
        
    ; Players set up the level and forbidden characters here
    SETUP_LEVEL:
        CALL displaySetupLevelMenu
        MOV current_game_state,STATE_INGAME
        JMP SUPER_LOOP
   
    ; Players have entered the actual game here
    INGAME:
        ; MOV current_game_state,STATE_INGAME
        ; CALL getKeyPressed
        ; MOV var_key_pressed,AH
        ; CMP var_key_pressed,F4_SCODE
        ; JZ FINISH_GAME
        ; Checks if bitmap was displayed to just display it once
            
            CMP BITMAP_DISPLAYED_FLAG,0
            JNZ YES
            CALL SHOW_BITMAP
            MOV BITMAP_DISPLAYED_FLAG,1
            CALL flushBuffer
            YES:
        ; Refresh screen
            CALL RefreshScreenContents
        ; Compare scores first to see if a player lost
            CMP PLAYER1_POINTS,0
            JZ FINISH_GAME
            CMP PLAYER2_POINTS,0
            JZ FINISH_GAME
        ; Determine player turn
            CMP current_player_turn, PLAYER1
            JNZ PLAYER2_TURN
        
        ;-------PLAYER 1'S TURN------;
        
        PLAYER1_TURN:
            ; Move cursor to player 1's instruction line
            move_cursor 4,20
            ; Check level

            ; Level1
                
                ; Read instruction string from player 
                CALL flushBuffer
                read_instruction
                ; Parse instruction
                CALL checkInstruction
                
            
            
            
            ; Level2
            JMP TURN_OVER
        
        ;-------PLAYER 2'S TURN------;    
        
        PLAYER2_TURN:
             ; Move cursor to player 1's instruction line
            move_cursor 21,20
            
            ; Read instruction string from player 
            CALL flushBuffer
            read_instruction
            ; Parse instruction
            CALL checkInstruction
                        
            JMP TURN_OVER
        TURN_OVER: 
        JMP SUPER_LOOP
    
    ; Chat screen is displayed here
    CHAT_SCREEN:
        display_string_color 20,15,string_exitMsg,string_exitMsgSize,0ah
        JMP SUPER_LOOP
    FINISH_GAME:
        clear_screen
        ; Display names and scores
            ;Player 1
        display_string_color 5,3,PLAYER1_NAME_,6,03h
        ConvertToASCII PLAYER1_POINTS,buffer_ascii
        display_string_color 5+2,6,buffer_ascii,2,0Fh
            ;Player 2
        display_string_color 32,3,PLAYER2_NAME_,3,04H
        ConvertToASCII PLAYER2_POINTS,buffer_ascii
        display_string_color 32,6,buffer_ascii,2,0Fh

        ;Display winner
        display_string_color 15,20,string_winnerMsg,string_winnerMsgSize,07h
        MOV BX,PLAYER2_POINTS
        CMP PLAYER1_POINTS,BX
        JB PLAYER2_WON
        display_string_color 20,23,PLAYER1_NAME_,6,0Dh
        JMP GAME_OVER
        PLAYER2_WON:
        display_string_color 20,23,PLAYER2_NAME_,3,0Dh
        GAME_OVER:
        ; WAIT 5 SECONDS
        CALL Delay
        CALL Delay
        CALL Delay
        CALL Delay
        CALL Delay 
        MOV current_game_state,STATE_MAIN_MENU
        clear_screen
        JMP SUPER_LOOP
    ; Player has chosen to exit the program
    EXIT_GAME:
    
    ; Return control to DOS
    MOV AH,04CH
    XOR AL,AL
    INT 21H
MAIN ENDP
END MAIN
END