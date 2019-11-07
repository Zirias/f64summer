Build requirements:
-------------------

- gcc or clang (for `mksums` and converting a checksummer to BASIC)
- ca65 / ld65 (for building the individual checksummers)
- petcat (for creating .PRG files from BASIC sources)

Build instructions:
-------------------

To build everything:

    make

make is expected to be GNU make, so e.g. on a BSD system, type `gmake` instead
of `make` everywhere.

To build only the `mksums` tool:

    make mksums

To clean up:

    make clean

To clean up AND remove the targets:

    make distclean

make variables:

    SELFVRFY    if set to 1, checksummer tools verify themselves (default: 1)
    REMSKIP     if set to 1, REM comments in lines with other code are
                ignored (default: 0) [1]

    CC          command to execute the C compiler (default: cc)
    CFLAGS      flags for the C compiler (default: -g0 -O2)
    C64AS       command to execute ca65 (default: ca65)
    C64LD       command to execute ld65 (default: ld65)
    PETCAT      command to execute petcat (default: petcat)

For example, to build checksummers that don't verify themselves, but ignore
REM comments, type:

    make clean
    make SELFVRFY=0 REMSKIP=1

Usage:
------

For creating checksums:

    mksums [-s] [-r] < input.prg > checksums.txt

creates checksums for the input.prg BASIC program in checksums.txt
-s:     allow shifted spaces in input.prg
-r:     skip REM comments in lines with other code; use this option if you
        built the checksummers with REMSKIP=1 [1]

For typing a BASIC program with checksumming:

  - Load the .prg for your target platform
  - Type `RUN` and `NEW`
  - Start typing your program, checksums appear in upper right corner
  - When finished, SAVE your program to disk (never use the datasette!!)
  - Reset or turn of computer to deactivate the checksummer
  - Load your typed program and enjoy.


----

[1] Feature for skipping REM comments:
    This convenience feature allows to print listings with REM comments in
    the same lines as the code without requiring to type them for getting
    the correct checksums. Therefore, any duplicate colons (optionally with
    spaces between them) are skipped as well, and a final colon before REM
    is ignored.

    NOTE1: Single colons at the beginning of a line aren't skipped

    NOTE2: Lines containing only a REM comment are treated normally (it is
           expected someone typing a listing without REM comments will skip
           these lines entirely)

    This behavior isn't 100% consistent in an attempt to use as little code
    as possible to implement this feature.

