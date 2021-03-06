.MODEL SMALL
.STACK 64
.DATA     
message db 'This is a message','$' 
ErrorMsg db 'ERROR: READ FILE FAILED','$'
indata db 30,?,30 dup('$') 
filename db 'red.bmp',0 
filehandle dw ?          ; 
ScrLine db 320 dup (0)
imageHeader db 54 dup(?)
Palette db 1024 dup(7)
.CODE
 
OpenFile PROC NEAR
        MOV ah, 3Dh     ; Open file option
        XOR AL,AL       ; Clear AL
        MOV DX, offset filename ; Load filename string offset 
        INT 21h
        JC openerror    ; Error opening file
        MOV [filehandle], AX ; File opened successfully, store in filehandle
    RET
openerror:
        MOV DX, offset ErrorMsg 
        MOV AH, 9H      ; Display string option
        INT 21h
    RET    
OpenFile ENDP 

CloseFile PROC  NEAR
     MOV AH,3EH ; Close file option
     XOR AL,AL  ; Clear AL
     MOV BX,filehandle ; Load filehandle address
     INT 21H
     JC CloseError
 RET
 CloseError: 
     MOV DX, offset ErrorMsg 
     MOV AH, 9H      ; Display string option
     INT 21h
 RET    
CloseFile ENDP  

ReadFile PROC NEAR                     
     MOV AH, 3FH ; Read from file option
     XOR AL,AL   ; Clear AL
     MOV BX,[filehandle] ; File to read from
     LEA DX,Palette
     MOV CX,400H ; Number of bytes to read 1024 bytes -> 256 colors * 4 bytes
     INT 21H
 RET
ReadFile ENDP  

SetPalette PROC NEAR
 PUSHA
 MOV BX,AX
 MOV CL,2
 
 MOV AL,DH
 AND AL,00001111B
 SHL AL,CL
 MOV CH,AL
 
 MOV AL,DL
 AND AL,11110000B
 SHR AL,CL
 MOV DH,AL
 
 MOV AL,DL
 AND AL,00001111B
 SHL AL,CL
 MOV CL,AL
 
 MOV BH,0
 MOV AX,1010H
 INT 10H
 POPA
 RET
SetPalette ENDP      

setPixelPalette MACRO index,r,g,b
        mov bx, index
        mov dh, r
        mov ch, g
        mov cl, b
        mov ax, 1010h  ; BIOS.SetIndividualColorRegister
        int 10h
ENDM setPixelPalette

FIXPALETTE PROC NEAR 
  PUSHA 
  LEA SI,Palette
  MOV AX,[SI]
  MOV ES:[0],AX  

  MOV AX,[SI]
  
  CALL SetPalette
  
  
  
  MOV CX,256  
  
  FIX:
      MOV AX,[SI]
      CALL SetPalette
      ADD SI,4
  LOOP FIX
  
  POPA 
  RET  
FIXPALETTE ENDP 

DrawImage PROC NEAR 
     mov ax, 0A000h
    mov es, ax
    mov cx,200
PrintBMPLoop:
    push cx
    ; di = cx*320, point to the correct screen line
    mov di,cx
    
    ;shift left cx - 6 bits
    shl cx,1
    shl cx,1
    shl cx,1
    shl cx,1
    shl cx,1
    shl cx,1

    ;shift left di - 8 bits
    shl di,1
    shl di,1
    shl di,1
    shl di,1
    shl di,1
    shl di,1
    shl di,1
    shl di,1

    add di,cx
    ; Read one line
    mov ah,3fh
    mov cx,320
    mov dx,offset ScrLine
    int 21h
    ; Copy one line into video memory
    cld ; Clear direction flag, for movsb
    mov cx,320
    mov si,offset ScrLine 

    rep movsb ; Copy line to the screen
    ;rep movsb is same as the following code:
    ;mov es:di, ds:si
    ;inc si
    ;inc di
    ;dec cx
    ;loop until cx=0
    pop cx
    loop PrintBMPLoop
    ret
    ;MOV AX,0A000H
;    MOV ES,AX   ; Extra segment points to Video-RAM
;   
;    
;    LEA SI,Palette ; Image data in BGR format
; 
;    MOV DI,0         ; First pixel pos in vram
;    CLD              ; Increment DI,SI
;    MOV CX,200
;    
;    DRAW:
;    PUSH CX 
;     push cx
;    ; di = cx*320, point to the correct screen line
;    mov di,cx
;    
;    ;shift left cx - 6 bits
;    shl cx,1
;    shl cx,1
;    shl cx,1
;    shl cx,1
;    shl cx,1
;    shl cx,1
;
;    ;shift left di - 8 bits
;    shl di,1
;    shl di,1
;    shl di,1
;    shl di,1
;    shl di,1
;    shl di,1
;    shl di,1
;    shl di,1
;
;    add di,cx
;    mov ah,3fh
;    mov cx,320
;    mov dx,offset ScrLine
;    int 21h
;    MOV CX,160      ; Screen height or lines on screen
;    ; offset = y*320 + x  -> CX * 6 , DI * 8 -->> DI= 8DI + 6CX-->>
;    REP MOVSW   
;    POP CX
;    LOOP DRAW
;    RET 
    
DrawImage ENDP 
        

MAIN PROC FAR
    MOV AX,@DATA
    MOV DS,AX
    
    MOV AX,13h
    INT 10h
    MOV AX,0A000H
    MOV ES,AX   
    CALL OpenFile
    CALL ReadFile
    CALL FIXPALETTE
    CALL DrawImage    
   
    
    
    MOV AH,04CH
    MOV AL,0
    INT 21H
    
    MAIN ENDP
END MAIN