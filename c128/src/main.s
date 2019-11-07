lfsr		= $fb
tmp		= $fd
inquot		= $fe

.code
		.word	$c00

		lda	#<idlehook
		sta	$302
		lda	#<tkhook
		sta	$304
		lda	#>idlehook
		sta	$303
		sta	$305
		rts

idlehook:	ldx	#$ff
		stx	$17
		jmp	$4dc8

tkhook:		jsr	$430d
		lda	$200
		beq	noline
		lda	$17
		eor	#$ff
		bne	showsum
noline:		rts

showsum:	sta	lfsr+1
		txa
		pha
		tya
		pha
		lda	$16
		eor	#$ff
		sta	lfsr
		ldx	#$0
		stx	inquot
skiplineno:	lda	$200,x
		cmp	#$30
		bcc	lineloop
		cmp	#$3a
		bcs	lineloop
		inx
		bne	skiplineno
lineloop:	lda	$200,x
		sta	tmp
		beq	done
		bit	inquot
		bmi	skipspcheck
		cmp	#' '
		beq	next
.ifdef REMSKIP
		cmp	#':'
		bne	skipspcheck
skipcolloop:	lda	$201,x
		cmp	#$8f	; REM
		beq	done
		cmp	#' '
		beq	skipcolon
		cmp	#':'
		bne	skipquot
skipcolon:	inx
		bne	skipcolloop
.endif
skipspcheck:	cmp	#'"'
		bne	skipquot
		lda	#$ff
		eor	inquot
		sta	inquot
skipquot:	ldy	#$8
bitloop:	asl	tmp
		rol
		eor	lfsr
		lsr
		bcc	rotate
		lda	#$68
		eor	lfsr+1
		sta	lfsr+1
rotate:		ror	lfsr+1
		ror	lfsr
		dey
		bne	bitloop
next:		inx
		bne	lineloop
done:		dec	$ff00
.ifdef REMSKIP
		lda	#$0
.endif
		bit	$d7
		bmi	vdcout
		tax
		ldy	#$3
outloop:	jsr	getlb
		sta	$424,y
		dey
		jsr	gethb
		sta	$424,y
		inx
		dey
		bpl	outloop
		lda	#$1
		ldy	#$3
colloop:	sta	$d824,y
		dey
		bpl	colloop
exit:		inc	$ff00
		pla
		tay
		pla
		tax
		rts
vdcout:		jsr	wrvdcdat
		ldx	#$1
vdcoloop:	jsr	gethb
		sta	$d601
		jsr	getlb
		sta	$d601
		dex
		bpl	vdcoloop
		lda	#$8
		jsr	wrvdcdat
		lda	#$d
		ldx	#$3
vdccol:		sta	$d601
		dex
		bpl	vdccol
		bmi	exit

getlb:		lda	lfsr,x
		and	#$f
		bpl	tohexdigit
gethb:		lda	lfsr,x
		lsr
		lsr
		lsr
		lsr
tohexdigit:	cmp	#$a
		bcc	digit
		sbc	#$9
		rts
digit:		adc	#$30
dorts:		rts

wrvdcdat:	ldx	#$12
wrvdcloop:	stx	$d600
		bit	$d600
		bpl	*-3
		inx
		cpx	#$20
		beq	dorts
dowr:		sta	$d601
		lda	#$4c
		cpx	#$14
		bne	wrvdcloop
		ldx	#$1f
		bne	wrvdcloop
