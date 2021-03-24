

.entry LOOP
.entry LENGTH
.extern L3
.extern W
MAIN: mov2 K, W
	add1 r2  STR
L3:	mov1 $$,r4
	inc1 B
B:
LOOP: jmp1 L3
	prn1 #5
	sub1 $$
	inc1 r0
	mov2 $$,r3
	bne1 L3
END: stop1 r4
STR: .string "abcdef"
LENGTH  : .data 6,-9, 15
K: .data 2
