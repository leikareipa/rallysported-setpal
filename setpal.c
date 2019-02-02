/*
 * Tarpeeksi Hyvae Soft 2016, 2019 /
 * SETPAL for RallySportED (DOS)
 * 
 * Lets the user modify Rally-Sport's hard-coded palettes. (Only works with
 * the demo version of Rally-Sport; and only modifies the palettes in RALLYE.EXE,
 * i.e. ignores VALIKKO.EXE).
 * 
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "palettes.c"

#define PROGRAM_NAME "SETPAL"

/* The byte offset to where the palette data block is stored in SETPAL's binary.*/
#define SELF_PAL_BLOCK_OFFS 0x2527

/* The byte offsets in RALLYE.EXE of the different palettes. There are four palettes in total
 * in that file, but this list ignores the one that isn't used at all by the game.*/
static const unsigned long PALETTE_OFFSET[NUM_PALS] = {0x202d6, 0x20336, 0x203f6};

/* Names of the various relevant binary files.*/
#define RALLYE_EXE_NAME "RALLYE.EXE"
#define SELF_EXE_NAME "SETPAL.EXE" /* The name of this program's executable.*/

/* Prints as a table into the console the color values of each of SETPAL's internal palettes.*/
static int printout(void)
{
	unsigned i;

	printf("The palettes currently stored in %s:\n\nINDEX\tPALETTE 0\t\tPALETTE 1\t\tPALETTE 2", SELF_EXE_NAME);
	for (i = 0; i < PAL_SIZE; i++)
	{
		if (i == 20)
		{
			printf("\n<More...>");
			getchar();
		}

		printf("\n%-2i\t[%-4i %-4i %-4i]\t[%-4i %-4i %-4i]\t[%-4i %-4i %-4i]",
			   i,
			   palettes[0][i*3]*4, palettes[0][i*3+1]*4, palettes[0][i*3+2]*4,
			   palettes[1][i*3]*4, palettes[1][i*3+1]*4, palettes[1][i*3+2]*4,
			   palettes[2][i*3]*4, palettes[2][i*3+1]*4, palettes[2][i*3+2]*4);
	}
	printf("\n");
	
	return 0;
}

/* Print out user-facing usage instructions into the console.*/
static int usage_instructions(void)
{
	printf(PROGRAM_NAME" - a tool for editing Rally-Sport's internal palettes (only supports the Rally-Sport demo). "
		   "By Tarpeeksi Hyvae Soft 2016, 2019.\n");

	printf("\n"SELF_EXE_NAME" <no parameters>\n\tPrints as a table into the console the palettes currently stored in\n\t"SELF_EXE_NAME".\n");
	
	printf("\n"SELF_EXE_NAME" m [T C R G B]\n\tModify a color in a palette stored in "SELF_EXE_NAME". T = palette index\n\t"
		   "(0-2); C = the nth color in T to be modified (0-31); R = red (0-255),\n\tG = green, B = blue of the new color.\n");
	
	printf("\n"SELF_EXE_NAME" p [T]\n\tPatch "RALLYE_EXE_NAME" (in "SELF_EXE_NAME"'s directory) with palette T (0-2) as\n\t"
		   "stored in "SELF_EXE_NAME".\n");

	return 0;
}

/* Terminate the program with the given error message if the given condition is met.*/
static void bail_if(const int condition, const char *const errMessage)
{
	if (condition)
	{
		fprintf(stderr, PROGRAM_NAME" ERROR: %s\n", errMessage);
		exit(1);
	}

	return;
}

int main(int argc, char **argv)
{
	if (argc <= 1)
	{
		return printout();
	}
	else if (strncmp(argv[1], "-?", 2) == 0 ||
			 strncmp(argv[1], "/?", 2) == 0)
	{
		return usage_instructions();
	}
	/* Modify an entry in one of SETPAL.EXE's palettes.*/
	else if (argv[1][0] == 'm')
	{
		unsigned char rgb[3];

		bail_if((argc != 7), "Malformed command-line arguments. Run with -? for instructions.");

		/* Get the new RGB values from the command-line. Note that we convert them
		 * to the range 0..63 as used by the VGA mode 13h palette. Conversion error-
		 * checking's not on the agenda today.*/
		rgb[0] = atoi(argv[4]) / 4;
		rgb[1] = atoi(argv[5]) / 4;
		rgb[2] = atoi(argv[6]) / 4;

		if (rgb[0] > 63) rgb[0] = 63;
		if (rgb[1] > 63) rgb[1] = 63;
		if (rgb[2] > 63) rgb[2] = 63;
		
		/* Write the new RGB values into SETPAL.EXE.*/
		{
			FILE *const f = fopen(SELF_EXE_NAME, "rb+");
			const int paletteIdx = atoi(argv[2]);
			long int offset = atoi(argv[3]);

			bail_if(!f, "Failed to open the SETPAL executable "SELF_EXE_NAME" for writing.");

			/* Get offset relative to the start of the first palette.*/
			offset = (offset * 3 + paletteIdx * PAL_SIZE * 3);

			/* Make the offset global in the context of the SETPAL.EXE binary.*/
			offset += SELF_PAL_BLOCK_OFFS;

			bail_if((fseek(f, offset, SEEK_SET) != 0 ||
					 fwrite(rgb, 1, 3, f) != 3), "Failed to modify "PROGRAM_NAME"'s internal palette in "SELF_EXE_NAME". "
					 							 "The palette may have become corrupted as a result.");

			fclose(f);
		}

		return 0;
	}
	/* Apply one of SETPAL's palettes into Rally-Sport's executable.*/
	else if (argv[1][0] == 'p')
	{
		const unsigned paletteIdx = atoi(argv[2]);
		const long offset = PALETTE_OFFSET[paletteIdx];
		FILE *const f = fopen(RALLYE_EXE_NAME, "rb+");

		bail_if((argc != 3), "Malformed command-line arguments. Run with -? for instructions.");

		bail_if(!f, "Failed to open Rally-Sport's executable "RALLYE_EXE_NAME" for writing.");
		bail_if((paletteIdx >= NUM_PALS), "Bad palette index.");

		/* Test that the byte size of the Rally-Sport executable is as expected.*/
		fseek(f, 0L, SEEK_END);
		bail_if((ftell(f) != 133452), "The Rally-Sport executable "RALLYE_EXE_NAME" is not of the correct size. "
									  "Note that "PROGRAM_NAME" only supports the demo version of Rally-Sport.");
		rewind(f);

		/* Write the data into the Rally-Sport executable.*/
		bail_if((fseek(f, offset, SEEK_SET) != 0 ||
				 fwrite(palettes[paletteIdx], 1, 3 * PAL_SIZE, f) != 3 * PAL_SIZE),
				 "Failed to patch the palette into "RALLYE_EXE_NAME". The file may have become corrupted as a result.");

		fclose(f);
	}
	else
	{
		bail_if(1, "Malformed command-line arguments. Run with -? for instructions.");
	}

	return 0;
}
