# include <math.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
#include <stdint.h>

#define GET_LENGTH(arr) (sizeof(arr) / sizeof((arr)[0]))


// Standard 44-byte WAV header structure
typedef struct {
    char     chunk_id[4];        // Must contain "RIFF"
    uint32_t chunk_size;        // Overall file size minus 8 bytes
    char     format[4];          // Must contain "WAVE"
    char     subchunk1_id[4];    // Must contain "fmt " (with trailing space)
    uint32_t subchunk1_size;    // 16 for PCM
    uint16_t audio_format;      // 1 for PCM (linear quantization)
    uint16_t num_channels;      // 1 = Mono, 2 = Stereo, etc.
    uint32_t sample_rate;       // e.g., 44100, 48000
    uint32_t byte_rate;         // sample_rate * num_channels * (bits_per_sample / 8)
    uint16_t block_align;       // num_channels * (bits_per_sample / 8)
    uint16_t bits_per_sample;   // 8, 16, 24, or 32
    char     subchunk2_id[4];    // Must contain "data"
    uint32_t subchunk2_size;    // Size of the actual audio data payload
} WavHeader;

char *long_help_message =
  "| WAVO - a wav file editor |\n\n"
  "Commands:\n\n"
  "1. ./wavo info [file.wav] or ./wavo -i [file.wav]\n"
  "     used to see a wav file info like\n"
  "     - Sample rate\n"
  "     - Bit depth\n"
  "     - Channels\n"
  "     - Duration\n\n"
  "2. ./wavo valid [file.wav] or ./wavo -v [file.wav]\n"
  "     Print \"[file.wav] is a valid wav\" if the file is valid wav otherwise\n"
  "     it print \"[file.wav] is not a valid wav\"\n\n"
  "3. ./wavo volume AMOUNT [file.wav] or ./wavo -V AMOUNT [file.wav]\n"
  "     Changes the file sound volume.\n"
  "      - AMOUNT is a float number\n"
  "       can be 0.5 to lower the volume\n"
  "       can be 1.5 to raise the volume\n\n"
  "4. ./wavo fade (in/out/both) [file.wav] or ./wavo -f (in/out/both) [file.wav]\n"
  "     Make a fade effect on the start/end of the file.\n\n"
  "5. ./wavo reverse [file.wav] or ./wavo -r [file.wav]\n"
  "     Reverse the file back and front.\n\n"
  "6. ./wavo swap [fille.wav] or ./wav -s [file.wav]\n"
  "     Swap the left and right sound channels.\n\n"
  "7. ./wavo trim -I [timestamp] -O [timestamp] [file.wav]\n"
  "     Trim the file from the start/end.\n\n"
  "     The timestamp is formated as MINUTES:SECONDS for example:\n"
  "       ./wavo trim -I 2:4 -O 3:32 file.wav\n"
  "       ./wave trim -I 2:34 file.wav\n"
  "       ./wavo trim -O 4:20 file.wav\n";

char *onelines[] = 
  {
    "./wavo info [file.wav] or ./wavo -i [file.wav]\n",
    "./wavo valid [file.wav] or ./wavo -v [file.wav]\n",
    "./wavo volume AMOUNT [file.wav] or ./wavo -V AMOUNT [file.wav]\n",
    "./wavo fade (in/out/both) [file.wav] or ./wavo -f (in/out/both) [file.wav]\n",
    "./wavo reverse [file.wav] or ./wavo -r [file.wav]\n",
    "./wavo swap [fille.wav] or ./wavo -s [file.wav]\n",
    "./wavo trim -I [timestamp] -O [timestamp] [file.wav]\n"
  };

// proto types
void oneliner_help(char *argument);
void info_command(char *filename);
int valid_command(char *filename);
void volume_command(float amount, char *filename);
void fade_command(char *type, char *filename);
void reverse_command(char *filename);
void swap_command(char *filename);
void trim_command(char *start_timestamp, char *end_timestamp, char *filename);
void three_arg_commands(char **args);
void four_arg_commands(char **args);
void five_arg_commands(char **args);
void seven_arg_commands(char **args);


int main(int argc, char *argv[])
{
  if (argc == 1)
  {
    printf("Run ./wavo -h for breif help.\nRun ./wavo help for the full help message.");
  }

  if (argc == 2 || argc > 7)
  {
    oneliner_help(argv[1]);
  }

  if (argc == 3)
  {
    three_arg_commands(argv);
  }

  if (argc == 4)
  {
    // four_arg_commands(argv);
  }

  if (argc == 5)
  {
    // five_arg_commands(argv);
  }

  if (argc == 5)
  {
    // seven_arg_commands(argv);
  }
}


void oneliner_help(char *argument)
{
  if (strcmp(argument, "info") == 0 || strcmp(argument, "-i") == 0)
  {
    printf("%s", onelines[0]);
  }
  else if (strcmp(argument, "valid") == 0 || strcmp(argument, "-v") == 0)
  {
    printf("%s", onelines[1]);
  }
  else if (strcmp(argument, "volume") == 0 || strcmp(argument, "-V") == 0)
  {
    printf("%s", onelines[2]);
  }
  else if (strcmp(argument, "fade") == 0 || strcmp(argument, "-f") == 0)
  {
    printf("%s", onelines[3]);
  }
  else if (strcmp(argument, "reverse") == 0 || strcmp(argument, "-r") == 0)
  {
    printf("%s", onelines[4]);
  }
  else if (strcmp(argument, "swap") == 0 || strcmp(argument, "-s") == 0)
  {
    printf("%s", onelines[5]);
  }
  else if (strcmp(argument, "trim") == 0)
  {
    printf("%s", onelines[6]);
  }
  else if (strcmp(argument, "help") == 0)
  {
    printf("%s", long_help_message);
  }
  else if (strcmp(argument, "-h") == 0)
  {
    for (size_t index = 0, length = GET_LENGTH(onelines);
        index < length; index++)
    {
      printf("%s", onelines[index]);
    }
    printf("use | ./wavo help | for explanation of each command and examples\n");
  }
  else
  {
    printf("Invalid command.\nUse | ./wavo help | for help.");
  }
}


void three_arg_commands(char **args)
{
  if (strcmp(args[1], "info") == 0 || strcmp(args[1], "-i") == 0)
  {
    int is_valid = valid_command(arg[2]);
    if (is_valid == 1)
    {
      info_command(args[2]);
    }
  }
  else if (strcmp(args[1], "valid") == 0 || strcmp(args[1], "-v") == 0)
  {
    valid_command(args[2]);
  }
  else if (strcmp(args[1], "reverse") == 0 || strcmp(args[1], "-r") == 0)
  {

  }
  else if (strcmp(args[1], "swap") == 0 || strcmp(args[1], "-s") == 0)
  {

  }
}

int valid_command(char *filename)
{
  FILE *file = fopen(filename, "r");
  if (file == NULL)
  {
    printf("Error: Could not open file %s.\n", filename);
    return 0;
  }

  // read 44 byte or the header of the file
  WavHeader header;
  size_t bytes_read = fread(&header, 1, sizeof(WavHeader), file);
  fclose(file);


  if (bytes_read < sizeof(WavHeader))
  {
    printf("Error: File is too small to contain a valid WAV header.\n");
    return 0;
  }

  
    // 4. Validate Magic Markers (Strings do not have null-terminators in headers)
    if (strncmp(header.chunk_id, "RIFF", 4) != 0) {
        printf("Validation Failed: Missing 'RIFF' marker.\n");
        return 0;
    }
    if (strncmp(header.format, "WAVE", 4) != 0) {
        printf("Validation Failed: Missing 'WAVE' format identifier.\n");
        return 0;
    }
    if (strncmp(header.subchunk1_id, "fmt ", 4) != 0) {
        printf("Validation Failed: Missing 'fmt ' subchunk.\n");
        return 0;
    }
    if (strncmp(header.subchunk2_id, "data", 4) != 0) {
        printf("Validation Failed: Missing 'data' subchunk identifier.\n");
        return 0;
    }

    // 5. Mathematical Sanity Checks (Validating structural integrity)
    if (header.audio_format != 1) {
        printf("Notice: Valid WAV container, but uses compressed audio (Format Code %d) instead of PCM.\n", header.audio_format);
        return 1; 
    }

    uint16_t expected_block_align = header.num_channels * (header.bits_per_sample / 8);
    uint32_t expected_byte_rate = header.sample_rate * expected_block_align;

    if (header.block_align != expected_block_align || header.byte_rate != expected_byte_rate) {
        printf("Validation Failed: Corrupt header arithmetic (ByteRate or BlockAlign mismatch).\n");
        return 0;
    }

    // Pass all constraints
    printf("Success: %s is a fully valid uncompressed PCM WAV file.\n", filename);
    return 1;
}
