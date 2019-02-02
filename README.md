# SETPAL (RallySportED)
A small tool - part of the [RallySportED toolset](../../../rallysported) - for modifying the hard-coded VGA mode 13h palettes in `RALLYE.EXE` of the demo version of Rally-Sport.

SETPAL acts as both the container and delivery mechanism for these modifications. Its executable stores a set of palettes that you can modify via command-line parameters to the executable, and ask it to apply one of those palettes to Rally-Sport's executable.

## Usage
Place `SETPAL.EXE` into Rally-Sport's directory, and run it via the command line (preferably under DOS or DOSBox) with the -? switch to receive further instructions on its use.

## Building
A script for building SETPAL using Borland's Turbo C 2 for DOS is provided under the `bin` directory; but you could use some other ANSI C-compatible DOS compiler, too.

Note that SETPAL (crudely) self-modifies its executable, and needs to be told in its source code the byte offset in the compiled executable of the palette data block (which itself is defined in [palettes.c](palettes.c)). You'll find a variable/define near the top of [setpal.c](setpal.c), which is where you need to record that offset. More specifically, this process might involve you compiling the SETPAL executable, scrutinizing it with a hex editor to find the start of the palette block, noting that offset in the variable in [setpal.c](setpal.c), and recompiling.
