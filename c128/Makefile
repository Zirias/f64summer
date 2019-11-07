C64SYS?=c64
C64AS?=ca65
C64LD?=ld65

ifeq ($(REMSKIP),1)
DEFINES=-DREMSKIP
else
DEFINES=
endif

C64ASFLAGS?=-t $(C64SYS) -g
C64LDFLAGS?=-Ln f64summer.lbl -m f64summer.map -Csrc/f64summer.cfg

f64summer_OBJS:=$(addprefix obj/,main.o)
f64summer_BIN:=f64summer.prg

all: $(f64summer_BIN)

$(f64summer_BIN): $(f64summer_OBJS)
	$(C64LD) -o$@ $(C64LDFLAGS) $^

obj:
	mkdir obj

obj/%.o: src/%.s src/f64summer.cfg Makefile | obj
	$(C64AS) $(C64ASFLAGS) $(DEFINES) -o$@ $<

clean:
	rm -fr obj *.lbl *.map
	rm -f $(f64summer_BIN)

distclean: clean

.PHONY: all clean distclean

