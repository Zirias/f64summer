lfsr		= $fb
tmp		= $fd
inquot		= $2

.code
		.word	$334

		lda	#<idlehook
		sta	$302
		lda	#>idlehook
		sta	$303
		lda	#<tkhook
		sta	$304
		lda	#>tkhook
		sta	$305
		rts

idlehook:	lda	#$ff
		sta	$15
		jmp	$a483

tkhook:		jsr	$a57c
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
		tax
outloop:	lda	lfsr,x
		and	#$f
		jsr	tohexdigit
		sta	$424,y
		dey
		lda	lfsr,x
		lsr
		lsr
		lsr
		lsr
		jsr	tohexdigit
		sta	$424,y
		inx
		dey
		bpl	outloop
		lda	#$1
		ldy	#$3
colloop:	sta	$d824,y
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

