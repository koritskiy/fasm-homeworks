format PE console
include 'win32a.inc'
entry start

section 'data' data readable writeable

str1 db 'Enter 4 numbers: ',0 ;������ ��� ������ � �������
str2 db 'Numbers are prime',0
str3 db 'Numbers are not prime',0
str4 db 'Incorrect input',0
        strGetInt db '%d', 0 ;������ ��� scanf
        var1 dd ?    ;���������� ��� 4� �����
        var2 dd ?
        var3 dd ?
        var4 dd ?
        prime dd 1    ;���������� ��� ������

section 'text' code executable readable
start:
        push var1;���� ������ ����������
        push strGetInt
        call [scanf]
        test eax,eax;�������� ���������� �����
        jz .error
        push var2;���� ������ ����������
        push strGetInt
        call [scanf]
        test eax,eax
        jz .error
        push var3;���� ������� ����������
        push strGetInt
        call [scanf]
        test eax,eax
        jz .error
        push var4;���� ��������� ����������
        push strGetInt
        call [scanf]
        test eax,eax
        jz .error
        call CheckPrime;����� ������� �������� �������� �����
        cmp [prime],2 ;�������� �� ������� ������
        je .error

        cmp [prime],1;���� prime==1 �� ������� st2(Numbers are prime) ����� str3(Numbers are not prime)
        je .yes
        push str3
        call [printf]
        jmp .end     ;������� ������ 2� ������
        .yes:
        push str2
        call [printf]
        .end:

        call [getch];��������
        push 0
        call [ExitProcess];���������� ���������
        .error:
        push str4
        call [printf]
        jmp .end

CheckPrime:
        mov eax,[var1]  ;����� ������������ �����  (min=var1)
        cmp eax,[var2]  ;���� min>var2 �� min=var2
        jl .m1
        mov eax,[var2]
        .m1:
        cmp eax,[var3]  ;���� min>var3 �� min=var3
        jl .m2
        mov eax,[var3]
        .m2:
        cmp eax,[var4]  ;���� min>var4 �� min=var4
        jl .m3
        mov eax,[var4]
        .m3:

        cmp eax,1;�������� ������������ �����, ����� ��� ����� ���� ������ ����
        jae .ok
        mov [prime],2
        jmp .end
        .ok:

        mov ecx,eax     ;������������� ����������� ����� � ecx
        .loop:;������ �����
                mov eax,[var1];�������� ������ ����� � eax
                xor edx,edx;���������������� � �������, ������� edx
                div ecx;����� �� ������� �����
                test edx,edx;��������� ������� �� ������� �� ��������� ����
                jnz .skip;���� ������� �� ����� ���� �� ��� ����� �� ��������, ��������� � ����� �����

                mov eax,[var2];���������, ����� ������� �� ������� �� ������ ���� �����
                xor edx,edx
                div ecx
                test edx,edx
                jnz .skip

                mov eax,[var3]
                xor edx,edx
                div ecx
                test edx,edx
                jnz .skip

                mov eax,[var4]
                xor edx,edx
                div ecx
                test edx,edx
                jnz .skip
                jmp .no;���� ��� ����� ���������� ��� ������� - ������ ����� �� ������� �������
        .skip:
        dec ecx;��������� ����� �� ������� ��������� ���������
        cmp ecx,1;���� ��� ����� ���������� ��� ������ �������, �������� ���� ������
        ja .loop
        jmp .end;���� ���� ������������� ��� �� ����� ������� ������� � ��� ������ �� ���� ������, ������ ����� �� �������
        .no:;���� ��� �� ������� ������� �� ������������� ����� ��� �� �������
        mov [prime],0
        .end:
        ret;����� �� �������



section '.idata' import data readable

library kernel, 'kernel32.dll',\
        msvcrt, 'msvcrt.dll'

import kernel,\
        ExitProcess, 'ExitProcess'

import msvcrt,\
        printf, 'printf',\
        scanf, 'scanf',\
        getch, '_getch'

