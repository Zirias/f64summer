lfsr		= $fb
tmp		= $fd
inquot		= $fe

.code
		.word	$334

		lda	#<idlehook
		sta	$302
		lda	#<tkhook
		sta	$304
		lda	#>idlehook
		sta	$303
		sta	$305
		bit	$9002
		bmi	unexpanded
		lda	#$10
		ldy	#$94
		sta	store1+2
		sta	store2+2
		sty	colloop+2
unexpanded:	rts

idlehook:	lda	#$ff
		sta	$15
		jmp	$c483

tkhook:		jsr	$c57c
		lda	$200
		beq	noline
		lda	$15
		eor	#$ff
		bne	showsum
noline:		rts

showsum:	sta	lfsr+1
		txa
		pha
		tya
		pha
		lda	$14
		eor	#$ff
		sta	lfsr
		ldx	#$0
		stx	inquot
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
done:		ldy	#$3
.ifdef REMSKIP
		ldx	#$0
.else
		tax
.endif
outloop:	lda	lfsr,x
		and	#$f
		jsr	tohexdigit
store1:		sta	$1e12,y
		dey
		lda	lfsr,x
		lsr
		lsr
		lsr
		lsr
		jsr	tohexdigit
store2:		sta	$1e12,y
		inx
		dey
		bpl	outloop
		ldy	#$3
		lda	#$2
colloop:	sta	$9612,y
		dey
		bpl	colloop
		pla
		tay
		pla
		tax
		rts

tohexdigit:	cmp	#$a
		bcc	digit
		sbc	#$9
		rts
digit:		adc	#$30
		rts

