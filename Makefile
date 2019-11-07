SELFVRFY?= 1
REMSKIP?= 0

STRIP?= strip
PETCAT?= petcat

CFLAGS?= -g0 -O2

ifeq ($(SELFVRFY),1)
M2BARGS?= -v
else
M2BARGS?=
endif

SUBDIRS:= vic20 c64 plus4 c128
vic20_BASICVER:= 2
vic20_LDADDR:= 1001
c64_BASICVER:= 2
c64_LDADDR:= 0801
plus4_BASICVER:= 3
plus4_LDADDR:= 1001
c128_BASICVER:= 70
c128_LDADDR:= 1c01

mprg2bas:= tools/mprg2bas
mksums:= mksums

all: $(addsuffix .prg,$(SUBDIRS)) $(mksums)

%.prg: %.bas
	$(PETCAT) -w$($(basename $@)_BASICVER) -l $($(basename $@)_LDADDR) \
		-o $@ -- $<

%.bas: % $(mprg2bas)
	$(mprg2bas) $(M2BARGS) <$</f64summer.prg >$@

clean: $(SUBDIRS)
	rm -f $(mprg2bas)
	rm -f *.bas

distclean: clean
	rm -f $(mksums)
	rm -f *.prg

$(mprg2bas): tools/mprg2bas.c
	$(CC) $(CFLAGS) -o$@ $<

$(mksums): tools/mksums.c
	$(CC) $(CFLAGS) -o$@ $<
	$(STRIP) --strip-all $@*

$(SUBDIRS):
	$(MAKE) -C $@ REMSKIP=$(REMSKIP) $(MAKECMDGOALS)

.PHONY: all clean distclean $(SUBDIRS)
.PRECIOUS: %.bas
