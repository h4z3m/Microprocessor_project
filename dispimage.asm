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
                 JMP  LOOPCLSTOP

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
                 MOV  CX,400H
                 MOV  DX,OFFSET PALETTE
                 INT  21H
                 RET
READPALETTE ENDP 
 
COPYPAL PROC 
                 MOV  SI,OFFSET PALETTE
                 MOV  CX,256
                 MOV  DX,3C8H
                 MOV  AL,0
    ; COPY STARTING COLOR TO PORT 3C8H
                 OUT  DX,AL
    ; COPY PALETTE ITSELF TO PORT 3C9H
                 INC  DX
    PALLOOP:     
    ; NOTE: COLORS IN A BMP FILE ARE SAVED AS BGR VALUES RATHER THAN RGB.
                 MOV  AL,[SI+2]              ; GET RED VALUE.
                 SHR  AL,1                   ; MAX. IS 255, BUT VIDEO PALETTE MAXIMAL
                 SHR  AL,1                   ; MAX. IS 255, BUT VIDEO PALETTE MAXIMAL
    ; VALUE IS 63. THEREFORE DIVIDING BY 4.
                 OUT  DX,AL                  ; SEND IT.
                 MOV  AL,[SI+1]              ; GET GREEN VALUE.
 
                 SHR  AL,1
                 SHR  AL,1
                 OUT  DX,AL                  ; SEND IT.
                 MOV  AL,[SI]                ; GET BLUE VALUE.

                 SHR  AL,1
                 SHR  AL,1
                 OUT  DX,AL                  ; SEND IT.
                 ADD  SI,4                   ; POINT TO NEXT COLOR.
    ; (THERE IS A NULL CHR. AFTER EVERY COLOR.)

                 LOOP PALLOOP
                 RET
COPYPAL ENDP 

COPYBITMAP PROC 
                 MOV  AX, 0A000H
                 MOV  ES, AX
                 MOV  CX,200
    PRINTBMPLOOP:
                 PUSH CX
    ; DI = CX*320, POINT TO THE CORRECT SCREEN LINE
                 MOV  DI,CX
    
    ;SHIFT LEFT CX - 6 BITS
                 SHL  CX,1
                 SHL  CX,1
                 SHL  CX,1
                 SHL  CX,1
                 SHL  CX,1
                 SHL  CX,1

    ;SHIFT LEFT DI - 8 BITS
                 SHL  DI,1
                 SHL  DI,1
                 SHL  DI,1
                 SHL  DI,1
                 SHL  DI,1
                 SHL  DI,1
                 SHL  DI,1
                 SHL  DI,1

                 ADD  DI,CX
    ; READ ONE LINE
                 MOV  AH,3FH
                 MOV  CX,320
                 MOV  DX,OFFSET SCRLINE
                 INT  21H
    ; COPY ONE LINE INTO VIDEO MEMORY
                 CLD                         ; CLEAR DIRECTION FLAG, FOR MOVSB
                 MOV  CX,320
                 MOV  SI,OFFSET SCRLINE

                 REP  MOVSB                  ; COPY LINE TO THE SCREEN
    ;REP MOVSB IS SAME AS THE FOLLOWING CODE:
    ;MOV ES:DI, DS:SI
    ;INC SI
    ;INC DI
    ;DEC CX
    ;LOOP UNTIL CX=0
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

                 CALL OPENFILE
                 CALL READHEADER
                 CALL READPALETTE
                 CALL COPYPAL
                 CALL COPYBITMAP
  
                 CALL CLOSEFILE

    ; CLEAN THE TO TOP MOST LINE BY SET ALL THE TOP LINE PIXELS TO BLACK
                 MOV  AX,0
                 MOV  [TOP_Y],AX
                 CALL CLEANTOPLINE
    
    
                 POP  SI
                 POP  DX
                 POP  CX
                 POP  BX
                 POP  AX
                 RET
SHOW_BITMAP ENDP 
