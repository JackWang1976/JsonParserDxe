        TITLE   llmul - SecureCore Tiano(TM) llmul Support.

;
; FILENAME.
;       SecureCore Tiano(TM) llmul Support.
;
;       $PATH:      System\005\SctBootManagerPkg\BootManager\Dxe\Ia32\llmul.asm
;
; FUNCTIONAL DESCRIPTION.
;
;
; MODIFICATION HISTORY.
;
; NOTICE.
;       Copyright (C) 2013 Phoenix Technologies Ltd.  All Rights Reserved.
;

.586
.MODEL FLAT,C

.CODE
;+---------------------------------------------------------------------------
;
; Function:
;        _allmul
;
; Description:
;        Multiply two signed 64-bit values.
;
; Entry:
;        Value1. First multiplicand
;        Value2. Second multiplicand
;
; Exit:
;       EDX:EAX -
;
_allmul  PROC C PUBLIC Value1: QWORD, Value2: QWORD

        mov     eax, DWORD PTR Value1[4]
        mov     ecx, DWORD PTR Value2[4]
        or      ecx,eax                                                ; if both hi-words = 0, then its easy
        mov     ecx, DWORD PTR Value2
        .if zero?
          mov     eax,DWORD PTR Value1
          mul     ecx
        .else
          push    ebx
          mul     ecx                                                ; eax has Value1[32:63], ecx has Value2[0:31]
          mov     ebx,eax                                        ; save result

          mov     eax, DWORD PTR Value1
          mul     DWORD PTR Value2[4]                ; ALO * BHI
          add     ebx,eax                                        ; ebx = ((Value1[0:31] * Value2[32:64]) + (Value1[32:64] * Value2[0:31]))

          mov     eax, DWORD PTR Value1        ; ecx = BLO
          mul     ecx                                                ; EDX:EAX = Value1[0:31]*Value2[0:31]
          add     edx,ebx                                        ;

          pop     ebx
        .endif
        ret 16
_allmul ENDP

        END
