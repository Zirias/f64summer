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

    CC          command to execute the C compiler (default: cc)
    CFLAGS      flags for the C compiler (default: -g0 -O2)
    C64AS       command to execute ca65 (default: ca65)
    C64LD       command to execute ld65 (default: ld65)
    PETCAT      command to execute petcat (default: petcat)

Usage:
------

For creating checksums:

    mksums [-s] < input.prg > checksums.txt

creates checksums for the input.prg BASIC program in checksums.txt
-s:     allow shifted spaces in input.prg

For typing a BASIC program with checksumming:

  - Load the .prg for your target platform
  - Type `RUN` and `NEW`
  - Start typing your program, checksums appear in upper right corner
  - When finished, SAVE your program to disk (never use the datasette!!)
  - Reset or turn of computer to deactivate the checksummer
  - Load your typed program and enjoy.

