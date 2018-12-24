STRIP?= strip
PETCAT?= petcat

CFLAGS?= -g0 -O2

SUBDIRS:= vic20 c64 plus4 c128
vic20_BASICVER:= 2
c64_BASICVER:= 2
plus4_BASICVER:= 3
c128_BASICVER:= 70

mprg2bas:= tools/mprg2bas
mksums:= mksums

all: $(addsuffix .prg,$(SUBDIRS)) $(mksums)

%.prg: %.bas
	$(PETCAT) -w$($(basename $@)_BASICVER) -o $@ -- $<

%.bas: % $(mprg2bas)
	$(mprg2bas) <$</f64summer.prg >$@

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
	$(STRIP) --strip-all $@

$(SUBDIRS):
	$(MAKE) -C $@ $(MAKECMDGOALS)

.PHONY: all clean distclean $(SUBDIRS)
.PRECIOUS: %.bas

