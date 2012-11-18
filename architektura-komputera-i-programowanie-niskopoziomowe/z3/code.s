
	.text
	.global funkcja
funkcja:
	stmfd sp!, { r4, r5, r6, r7, r8, r9, r10, r11, r12, lr }
	ldr r2, [ r0, #0 ]
	add r1, r2, r2
	sub r3, r1, r1
	mul r4, r1, r3
	orr r5, r1, r3
	sub r6, r2, r1
	sub r7, r6, r4
	add r8, r1, r5
	mul r9, r3, r1
	add r10, r5, r3
	add sp, sp, #0
	stmfd sp!, { r10 }
	orr r10, r8, r4
	add sp, sp, #0
	stmfd sp!, { r4 }
	sub r4, r10, r2
	add sp, sp, #0
	stmfd sp!, { r2 }
	and r2, r8, r6
	add sp, sp, #0
	stmfd sp!, { r4 }
	sub r4, r10, r7
	add sp, sp, #0
	stmfd sp!, { r7 }
	add r7, r4, r9
	add sp, sp, #0
	stmfd sp!, { r10 }
	mul r10, r4, r7
	add sp, sp, #0
	stmfd sp!, { r4 }
	add r4, r6, r10
	add sp, sp, #0
	stmfd sp!, { r1 }
	ldr r12, =78560448
	eor r1, r7, r12
	str r4, [ r0, #64 ]
	add sp, sp, #0
	stmfd sp!, { r6 }
	add sp, sp, #8
	ldmfd sp!, { r6 }
	orr r4, r10, r6
	ldr r11, =-8
	add sp, sp, r11
	stmfd sp!, { r4 }
	and r4, r5, r3
	add sp, sp, #0
	stmfd sp!, { r4 }
	add sp, sp, #0
	stmfd sp!, { r1 }
	add sp, sp, #48
	ldmfd sp!, { r1 }
	add r4, r1, r1
	sub sp, sp, #4
	stmfd sp!, { r1 }
	mul r1, r8, r2
	ldr r11, =-40
	add sp, sp, r11
	stmfd sp!, { r8 }
	add sp, sp, #0
	stmfd sp!, { r2 }
	add sp, sp, #44
	ldmfd sp!, { r2 }
	eor r8, r6, r2
	sub sp, sp, #20
	stmfd sp!, { r6 }
	add r6, r5, r10
	str r5, [ r0, #16 ]
	add sp, sp, #20
	stmfd sp!, { r2 }
	add sp, sp, #16
	ldmfd sp!, { r2 }
	sub r5, r9, r2
	ldr r11, =-52
	add sp, sp, r11
	stmfd sp!, { r5 }
	add r5, r7, r1
	add sp, sp, #0
	stmfd sp!, { r7 }
	add sp, sp, #0
	stmfd sp!, { r1 }
	add sp, sp, #48
	ldmfd sp!, { r1 }
	ldr r11, =-48
	add sp, sp, r11
	stmfd sp!, { r6 }
	add sp, sp, #36
	ldmfd sp!, { r6 }
	add r7, r1, r6
	str r7, [ r0, #104 ]
	sub r7, r9, r4
	sub sp, sp, #4
	stmfd sp!, { r6 }
	add r6, r5, r3
	ldr r11, =-28
	add sp, sp, r11
	stmfd sp!, { r7 }
	add sp, sp, #0
	stmfd sp!, { r4 }
	add sp, sp, #28
	ldmfd sp!, { r4 }
	sub r7, r4, r10
	sub sp, sp, #4
	stmfd sp!, { r4 }
	ldr r11, =-20
	add sp, sp, r11
	stmfd sp!, { r3 }
	add sp, sp, #44
	ldmfd sp!, { r3 }
	eor r4, r3, r8
	ldr r11, =-44
	add sp, sp, r11
	stmfd sp!, { r4 }
	add sp, sp, #0
	stmfd sp!, { r6 }
	add sp, sp, #76
	ldmfd sp!, { r6 }
	add r4, r10, r6
	str r6, [ r0, #24 ]
	ldr r11, =-76
	add sp, sp, r11
	stmfd sp!, { r4 }
	add sp, sp, #52
	ldmfd sp!, { r4 }
	sub r6, r4, r8
	str r6, [ r0, #128 ]
	add r6, r9, r8
	str r8, [ r0, #88 ]
	and r8, r2, r1
	add sp, sp, #20
	stmfd sp!, { r1 }
	ldr r11, =-68
	add sp, sp, r11
	stmfd sp!, { r1 }
	add sp, sp, #100
	ldmfd sp!, { r1 }
	sub r1, r1, #6
	ldr r11, =-100
	add sp, sp, r11
	stmfd sp!, { r6 }
	add sp, sp, #0
	stmfd sp!, { r8 }
	add sp, sp, #40
	ldmfd sp!, { r8 }
	sub r6, r3, r8
	ldr r11, =-40
	add sp, sp, r11
	stmfd sp!, { r6 }
	add r6, r2, r9
	add sp, sp, #0
	stmfd sp!, { r9 }
	and r9, r10, r1
	str r10, [ r0, #60 ]
	orr r10, r5, r8
	add sp, sp, #0
	stmfd sp!, { r9 }
	add sp, sp, #116
	stmfd sp!, { r1 }
	sub sp, sp, #16
	ldmfd sp!, { r1 }
	eor r9, r7, r1
	sub sp, sp, #4
	stmfd sp!, { r1 }
	sub r1, r2, r6
	ldr r11, =-88
	add sp, sp, r11
	stmfd sp!, { r1 }
	add sp, sp, #0
	stmfd sp!, { r10 }
	add sp, sp, #40
	ldmfd sp!, { r10 }
	eor r1, r6, r10
	add sp, sp, #80
	stmfd sp!, { r2 }
	ldr r11, =-116
	add sp, sp, r11
	stmfd sp!, { r6 }
	add sp, sp, #52
	ldmfd sp!, { r6 }
	sub r2, r6, r1
	sub sp, sp, #4
	stmfd sp!, { r6 }
	add sp, sp, #44
	stmfd sp!, { r3 }
	sub sp, sp, #44
	ldmfd sp!, { r3 }
	add r6, r3, r3
	ldr r11, =-40
	add sp, sp, r11
	stmfd sp!, { r6 }
	add r6, r4, r8
	str r4, [ r0, #68 ]
	mul r4, r9, r2
	add sp, sp, #0
	stmfd sp!, { r9 }
	add r9, r3, r6
	str r6, [ r0, #180 ]
	add sp, sp, #44
	stmfd sp!, { r3 }
	add sp, sp, #36
	ldmfd sp!, { r3 }
	ldr r11, =-76
	add sp, sp, r11
	stmfd sp!, { r9 }
	add sp, sp, #132
	ldmfd sp!, { r9 }
	mul r6, r3, r9
	sub sp, sp, #4
	stmfd sp!, { r9 }
	sub sp, sp, #72
	stmfd sp!, { r10 }
	sub sp, sp, #0
	ldmfd sp!, { r10 }
	add r9, r10, r4
	str r10, [ r0, #140 ]
	ldr r11, =-48
	add sp, sp, r11
	stmfd sp!, { r9 }
	add sp, sp, #72
	ldmfd sp!, { r9 }
	add r10, r7, r9
	ldr r11, =-72
	add sp, sp, r11
	stmfd sp!, { r4 }
	add sp, sp, #0
	stmfd sp!, { r7 }
	add sp, sp, #128
	ldmfd sp!, { r7 }
	sub sp, sp, #52
	stmfd sp!, { r9 }
	add sp, sp, #32
	ldmfd sp!, { r9 }
	sub r4, r7, r9
	str r7, [ r0, #4 ]
	ldr r11, =-104
	add sp, sp, r11
	stmfd sp!, { r4 }
	add sp, sp, #76
	ldmfd sp!, { r4 }
	sub r7, r6, r4
	ldr r11, =-76
	add sp, sp, r11
	stmfd sp!, { r7 }
	add r7, r3, r5
	add sp, sp, #96
	stmfd sp!, { r3 }
	add r3, r1, r5
	add sp, sp, #16
	stmfd sp!, { r9 }
	sub r9, r8, r10
	str r8, [ r0, #92 ]
	ldr r11, =-104
	add sp, sp, r11
	stmfd sp!, { r9 }
	add sp, sp, #64
	ldmfd sp!, { r9 }
	orr r8, r7, r9
	str r9, [ r0, #136 ]
	str r7, [ r0, #212 ]
	add sp, sp, #56
	ldmfd sp!, { r7 }
	orr r9, r2, r7
	str r9, [ r0, #228 ]
	str r2, [ r0, #172 ]
	sub sp, sp, #40
	ldmfd sp!, { r2 }
	add sp, sp, #36
	stmfd sp!, { r7 }
	sub sp, sp, #16
	ldmfd sp!, { r7 }
	sub r9, r2, r7
	str r9, [ r0, #232 ]
	sub sp, sp, #20
	stmfd sp!, { r2 }
	add sp, sp, #56
	ldmfd sp!, { r2 }
	ldr r11, =-128
	add sp, sp, r11
	stmfd sp!, { r3 }
	add sp, sp, #152
	ldmfd sp!, { r3 }
	and r9, r2, r3
	str r9, [ r0, #236 ]
	str r2, [ r0, #76 ]
	sub sp, sp, #56
	ldmfd sp!, { r2 }
	add sp, sp, #52
	stmfd sp!, { r3 }
	sub sp, sp, #32
	ldmfd sp!, { r3 }
	mul r9, r2, r3
	sub sp, sp, #20
	stmfd sp!, { r2 }
	add sp, sp, #8
	stmfd sp!, { r7 }
	sub sp, sp, #0
	ldmfd sp!, { r7 }
	and r2, r7, r10
	str r10, [ r0, #200 ]
	ldr r11, =-92
	add sp, sp, r11
	stmfd sp!, { r2 }
	add sp, sp, #168
	ldmfd sp!, { r2 }
	add r10, r2, r6
	str r10, [ r0, #248 ]
	sub sp, sp, #4
	ldmfd sp!, { r2 }
	mul r10, r2, r8
	str r8, [ r0, #224 ]
	orr r8, r9, r3
	str r8, [ r0, #256 ]
	str r3, [ r0, #56 ]
	sub sp, sp, #24
	ldmfd sp!, { r3 }
	eor r8, r5, r3
	str r5, [ r0, #100 ]
	str r3, [ r0, #72 ]
	sub sp, sp, #92
	ldmfd sp!, { r3 }
	ldr r11, =-48
	add sp, sp, r11
	stmfd sp!, { r9 }
	add sp, sp, #28
	ldmfd sp!, { r9 }
	add r5, r3, r9
	str r9, [ r0, #116 ]
	str r5, [ r0, #264 ]
	add sp, sp, #148
	ldmfd sp!, { r5 }
	orr r9, r5, r1
	str r5, [ r0, #12 ]
	str r3, [ r0, #148 ]
	sub sp, sp, #136
	ldmfd sp!, { r3 }
	orr r5, r2, r3
	str r5, [ r0, #272 ]
	str r2, [ r0, #44 ]
	add sp, sp, #16
	ldmfd sp!, { r2 }
	orr r5, r2, r4
	str r5, [ r0, #276 ]
	str r2, [ r0, #156 ]
	add sp, sp, #24
	ldmfd sp!, { r2 }
	ldr r11, =-80
	add sp, sp, r11
	stmfd sp!, { r1 }
	add sp, sp, #112
	ldmfd sp!, { r1 }
	sub r5, r2, r1
	str r5, [ r0, #280 ]
	str r1, [ r0, #108 ]
	sub sp, sp, #40
	ldmfd sp!, { r1 }
	orr r5, r1, r10
	str r10, [ r0, #252 ]
	str r1, [ r0, #32 ]
	add sp, sp, #36
	ldmfd sp!, { r1 }
	add r10, r3, r1
	str r3, [ r0, #188 ]
	str r1, [ r0, #80 ]
	sub sp, sp, #92
	ldmfd sp!, { r1 }
	sub r3, r6, r1
	str r3, [ r0, #292 ]
	str r1, [ r0, #216 ]
	add sp, sp, #36
	ldmfd sp!, { r1 }
	orr r3, r1, r4
	str r4, [ r0, #120 ]
	ldr r11, =-52
	add sp, sp, r11
	stmfd sp!, { r9 }
	add sp, sp, #188
	ldmfd sp!, { r9 }
	eor r4, r9, r8
	str r4, [ r0, #300 ]
	sub sp, sp, #4
	stmfd sp!, { r9 }
	sub sp, sp, #40
	ldmfd sp!, { r9 }
	ldr r11, =-140
	add sp, sp, r11
	stmfd sp!, { r8 }
	add sp, sp, #168
	ldmfd sp!, { r8 }
	sub r4, r9, r8
	str r9, [ r0, #28 ]
	add r9, r2, r6
	str r9, [ r0, #308 ]
	add r9, r7, r3
	str r9, [ r0, #312 ]
	str r7, [ r0, #8 ]
	sub sp, sp, #148
	ldmfd sp!, { r7 }
	str r4, [ r0, #304 ]
	add sp, sp, #116
	ldmfd sp!, { r4 }
	eor r9, r7, r4
	str r7, [ r0, #244 ]
	str r2, [ r0, #132 ]
	sub sp, sp, #64
	ldmfd sp!, { r2 }
	orr r7, r5, r2
	str r7, [ r0, #320 ]
	str r5, [ r0, #284 ]
	add sp, sp, #32
	ldmfd sp!, { r5 }
	sub r7, r4, r5
	str r7, [ r0, #324 ]
	str r5, [ r0, #112 ]
	sub sp, sp, #60
	ldmfd sp!, { r5 }
	orr r7, r9, r5
	str r9, [ r0, #316 ]
	str r7, [ r0, #328 ]
	add sp, sp, #12
	ldmfd sp!, { r7 }
	str r5, [ r0, #184 ]
	add sp, sp, #104
	ldmfd sp!, { r5 }
	add r9, r7, r5
	str r7, [ r0, #160 ]
	str r5, [ r0, #20 ]
	sub sp, sp, #84
	ldmfd sp!, { r5 }
	sub r7, r4, r5
	str r7, [ r0, #336 ]
	orr r7, r8, r10
	str r8, [ r0, #52 ]
	str r7, [ r0, #340 ]
	add sp, sp, #24
	ldmfd sp!, { r7 }
	add r8, r5, r7
	str r8, [ r0, #344 ]
	str r7, [ r0, #124 ]
	add sp, sp, #40
	ldmfd sp!, { r7 }
	eor r8, r7, r10
	str r8, [ r0, #348 ]
	mul r8, r1, r10
	str r10, [ r0, #288 ]
	str r8, [ r0, #352 ]
	sub sp, sp, #112
	ldmfd sp!, { r8 }
	str r7, [ r0, #48 ]
	sub sp, sp, #24
	ldmfd sp!, { r7 }
	eor r10, r8, r7
	str r1, [ r0, #176 ]
	ldr r11, =-4
	add sp, sp, r11
	stmfd sp!, { r1 }
	add sp, sp, #52
	ldmfd sp!, { r1 }
	ldr r12, =-971081995
	mul r1, r1, r12
	str r10, [ r0, #356 ]
	str r7, [ r0, #260 ]
	add sp, sp, #124
	ldmfd sp!, { r7 }
	eor r10, r6, r7
	str r7, [ r0, #40 ]
	str r6, [ r0, #192 ]
	sub sp, sp, #140
	ldmfd sp!, { r6 }
	sub r7, r3, r6
	str r1, [ r0, #196 ]
	add sp, sp, #52
	ldmfd sp!, { r1 }
	ldr r12, =-1098870552
	add r1, r1, r12
	str r7, [ r0, #368 ]
	mul r7, r4, r9
	str r9, [ r0, #332 ]
	str r7, [ r0, #376 ]
	sub sp, sp, #76
	ldmfd sp!, { r7 }
	orr r9, r7, r10
	str r10, [ r0, #364 ]
	str r9, [ r0, #380 ]
	sub sp, sp, #8
	ldmfd sp!, { r9 }
	str r6, [ r0, #208 ]
	add sp, sp, #192
	ldmfd sp!, { r6 }
	and r10, r9, r6
	str r10, [ r0, #384 ]
	str r6, [ r0, #36 ]
	sub sp, sp, #180
	ldmfd sp!, { r6 }
	str r5, [ r0, #152 ]
	add sp, sp, #4
	ldmfd sp!, { r5 }
	orr r10, r6, r5
	str r10, [ r0, #388 ]
	str r6, [ r0, #168 ]
	add sp, sp, #20
	ldmfd sp!, { r6 }
	add r10, r7, r6
	str r7, [ r0, #268 ]
	sub r7, r10, r8
	str r10, [ r0, #392 ]
	str r8, [ r0, #220 ]
	add sp, sp, #92
	ldmfd sp!, { r8 }
	eor r10, r9, r8
	str r1, [ r0, #144 ]
	mul r1, r10, #33
	ldr r11, =-132
	add sp, sp, r11
	stmfd sp!, { r1 }
	add sp, sp, #4
	ldmfd sp!, { r1 }
	mul r1, r1, #32
	str r1, [ r0, #404 ]
	ldr r12, =-1
	mul r1, r10, r12
	str r1, [ r0, #412 ]
	str r2, [ r0, #164 ]
	str r3, [ r0, #296 ]
	str r4, [ r0, #96 ]
	str r5, [ r0, #240 ]
	str r6, [ r0, #204 ]
	str r7, [ r0, #396 ]
	str r8, [ r0, #84 ]
	str r9, [ r0, #360 ]
	str r10, [ r0, #400 ]
	add sp, sp, #196
	ldmfd sp!, { r4, r5, r6, r7, r8, r9, r10, r11, r12, pc }
