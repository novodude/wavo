# include <math.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
#include <stdint.h>

#define GET_LENGTH(arr) (sizeof(arr) / sizeof((arr)[0]))


// Standard 44-byte WAV header structure
typedef struct 
{
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

typedef struct 
{
  char chunk_id[4];
  uint32_t chunk_size;
} ChunkData;

typedef struct {
  int16_t *samples;
  uint32_t num_samples;
} SamplesData;

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
int valid_command(char *filename, int flag);
int volume_command(char *amount, char *filename);
int fade_command(char *type, char *filename);
int reverse_command(char *filename);
int swap_command(char *filename);
int trim_command(char *start_timestamp, char *end_timestamp, char *filename);
void three_arg_commands(char **args);
void four_arg_commands(char **args);
void five_arg_commands(char **args);
void seven_arg_commands(char **args);
void put_line(char *filename);
ChunkData find_data_chunk(FILE *file, uint32_t subchunk1_size);
SamplesData read_samples(FILE *file, WavHeader header);
int write_wav(char *filename, WavHeader header, SamplesData sample_data);
char *get_filename(char **args, int arg_amount);
char *make_output_filename(char *filename, char *suffex);


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
    four_arg_commands(argv);
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
    int is_valid = valid_command(args[2], 0);
    if (is_valid == 1)
    {
      info_command(args[2]);
    }
  }
  else if (strcmp(args[1], "valid") == 0 || strcmp(args[1], "-v") == 0)
  {
    valid_command(args[2], 1);
  }
  else if (strcmp(args[1], "reverse") == 0 || strcmp(args[1], "-r") == 0)
  {

  }
  else if (strcmp(args[1], "swap") == 0 || strcmp(args[1], "-s") == 0)
  {

  }
}


void four_arg_commands(char **args)
{
  if (strcmp(args[1], "volume") == 0 || strcmp(args[1], "-V") == 0)
  {
    int is_valid = valid_command(args[3], 0);
    if (is_valid == 1)
    {
      int success = volume_command(args[2], args[3]);
      
      if (success == 1)
      {
        printf("Success! Volume changed.");
      }
      else
      {
        printf("Failed! Volume didn't change.");
      }
    }
  }
  else if (strcmp(args[1], "fade") == 0 || strcmp(args[1], "-f") == 0)
  {
    int is_valid = valid_command(args[3], 0);
  }
}


//------------------------------------------------
//             commands functions
//------------------------------------------------

int valid_command(char *filename, int flag)
{
  FILE *file = fopen(filename, "rb");
  if (file == NULL)
  {
    printf("Error: Could not open file %s.\n", filename);
    return 0;
  }

  WavHeader header;
  size_t bytes_read = fread(&header, 1, sizeof(WavHeader), file);
  ChunkData chunk_data = find_data_chunk(file, header.subchunk1_size);
  fclose(file);

  if (bytes_read < sizeof(WavHeader))
  {
    printf("Error: File is too small to contain a valid WAV header.\n");
    return 0;
  }
  
  if (strncmp(chunk_data.chunk_id, "data", 4) != 0)
  {
      printf("Validation Failed: Missing 'data' subchunk identifier.\n");
      return 0;
  }

  // Validate Magic Markers (Strings do not have null-terminators in headers)
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


  // Mathematical Sanity Checks (Validating structural integrity)
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

  if (flag == 1)
  {
    // Pass all constraints
    printf("Success: %s is a fully valid uncompressed PCM WAV file.\n", filename);
  }
  return 1;
}

void info_command(char *filename)
{
  FILE *file = fopen(filename, "rb");

  // read 44 byte or the header of the file
  WavHeader header;
  size_t bytes_read = fread(&header, 1, sizeof(WavHeader), file);

  ChunkData chunk_data = find_data_chunk(file, header.subchunk1_size);
  fclose(file);
  float duration = ((float) chunk_data.chunk_size / header.byte_rate);


  put_line(filename);
  printf("File path: %s\n", filename);
  printf("Sample rate: %uhz\n", header.sample_rate);
  if (header.num_channels == 1)
  {
    printf("Channels: MONO\n");
  }
  else if (header.num_channels == 2)
  {
    printf("Channels: DUO\n");
  }
  else
  {
    printf("Channels: %u\n", header.num_channels);
  }
  printf("Bit depth: %u\n", header.bits_per_sample);
  if (duration > 60)
  {
    int minutes = (int) (duration / 60);
    int seconds = (int) (duration) % 60;
    printf("Duration: %d:%02d\n", minutes, seconds);
  }
  else
  {
    printf("Duration: 00:%02f\n", duration);
  }
  put_line(filename);
}


int volume_command(char *amount, char *filename)
{
  FILE *file = fopen(filename, "rb");
  if (file == NULL)
  {
    return 0;
  }

  float value = strtof(amount, NULL);
  // read 44 byte or the header of the file
  WavHeader header;
  fread(&header, 1, sizeof(WavHeader), file);

  SamplesData sample_data = read_samples(file, header);
  if (sample_data.samples == NULL)
  {
    fclose(file);
    return 0;
  }


  for (int sample = 0; sample < sample_data.num_samples; sample++)
  {
    sample_data.samples[sample] *= value;
  }
  
  char *out_filename = make_output_filename(filename, amount);

  int success = write_wav(out_filename, header, sample_data);
  if (success != 1)
  {
    fclose(file);
    return 0;
  }
  free(out_filename);
  free(sample_data.samples);
  fclose(file);
  return 1;
}

//-------------------------------
//            helpers
//-------------------------------

void put_line(char *filename)
{
  for (int i = 0, filename_length = strlen(filename) + 15; i < filename_length; i++)
  {
    putchar('=');
  }
  putchar('\n');
}

ChunkData find_data_chunk(FILE *file, uint32_t subchunk1_size)
{
  uint32_t next_chunk = (12 + 8 + subchunk1_size);
  fseek(file, next_chunk, SEEK_SET);
  ChunkData chunk_data;
  long current_pos;
  long file_size;
  while (fread(chunk_data.chunk_id, 1, 4, file) == 4)
  {
    size_t r = fread(&chunk_data.chunk_size, 1, 4, file);

    if (strncmp(chunk_data.chunk_id, "data", 4) == 0)
    {
      if (chunk_data.chunk_size == 0xFFFFFFFF)
      {
        long current_pos = ftell(file);
        fseek(file, 0, SEEK_END);
        long file_size = ftell(file);
        fseek(file, current_pos, SEEK_SET);
        chunk_data.chunk_size = (uint32_t)(file_size - current_pos);
      }
      break;
    }
    else
    {
      fseek(file, chunk_data.chunk_size, SEEK_CUR);
    }
  }
  return chunk_data;
}

SamplesData read_samples(FILE *file, WavHeader header)
{
  SamplesData sample_data;
  ChunkData chunk_data = find_data_chunk(file, header.subchunk1_size);
  sample_data.num_samples = chunk_data.chunk_size / (header.bits_per_sample / 8);
  sample_data.samples = malloc(sample_data.num_samples * sizeof(int16_t));
  if (sample_data.samples == NULL)
  {
    return sample_data;
  }
  fread(sample_data.samples, sizeof(int16_t), sample_data.num_samples, file);
  return sample_data;
}

int write_wav(char *filename, WavHeader header, SamplesData sample_data)
{
  FILE *file = fopen(filename, "wb");
  if (file == NULL)
  {
    return 0;
  }
  
  memcpy(header.subchunk2_id, "data", 4);
  header.subchunk2_size = sample_data.num_samples * sizeof(int16_t);
  
  fwrite(&header, sizeof(WavHeader), 1, file);
  fwrite(sample_data.samples, sizeof(int16_t), sample_data.num_samples, file);
  fclose(file);
  return 1;
}

char *make_output_filename(char *filename, char *suffex)
{
  int string_len = strlen(filename) + strlen(suffex) + 2;
  char *result = malloc(string_len * sizeof(char));
  
  strcpy(result, filename);
  char *wav = strstr(result, ".wav");
  *wav = '\0';

  strcat(result, "_");
  strcat(result, suffex);
  strcat(result, ".wav");

  return result;
}
