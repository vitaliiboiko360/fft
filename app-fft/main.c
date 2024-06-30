#include <stdio.h>
#include <stdint.h>

#include <fftw3.h>

const int SIZE_128 = 128;
const int REAL = 0;
const int IMAG = 1;

int main(int argc, char* argv[]) {
  if (argc < 2) {
    printf("provide path to WAV file\nexiting...\n");
    return 1;
  }
  
  FILE* inputFile = fopen(argv[1], "r");
  if (!inputFile) {
    printf("could not open file \"%s\"\nexiting...\n", argv[1]);
    return 1;
  }

  fseek(inputFile, 0, SEEK_END);
  int sizeOfFile = ftell(inputFile);

  printf("input file size is %d\n", sizeOfFile);

  char buffer128[SIZE_128];

  fseek(inputFile, 0, SEEK_SET);

  fgets(buffer128, 5, inputFile);
  printf("ckID: first 4 bytes=%s\n", buffer128);

  uint32_t readValue = 0;
  for (int i=0;i<4;++i) {
    uint8_t readCharacter = fgetc(inputFile);
    readValue = readValue | (uint32_t)readCharacter << i*8; 
  }
  printf("header chsize =%u\n", readValue);

  fgets(buffer128, 5, inputFile);
  printf("8..16 bytes=%s\n", buffer128);

  fgets(buffer128, 5, inputFile);
  printf("16..24 bytes=%s\n", buffer128);

  uint32_t chunkSize = 0;
  for (int i=0;i<4;++i) {
    uint8_t readCharacter = fgetc(inputFile);
    chunkSize = chunkSize | (uint32_t)readCharacter << i*8; 
  }
  printf("fmt chunkSize =%u\n", chunkSize);

  uint32_t formatTag = 0;
  for (int i=0;i<2;++i) {
    uint8_t readCharacter = fgetc(inputFile);
    formatTag = formatTag | (uint32_t)readCharacter << i*8; 
  }
  printf("wFormatTag: Format code=%u\n", formatTag);

  uint32_t nChannels = 0;
  for (int i=0;i<2;++i) {
    uint8_t readCharacter = fgetc(inputFile);
    nChannels = nChannels | (uint32_t)readCharacter << i*8; 
  }
  printf("nChannels: number of channels=%u\n", nChannels);

  uint32_t nSamplesPerSec = 0;
  for (int i=0;i<4;++i) {
    uint8_t readCharacter = fgetc(inputFile);
    nSamplesPerSec = nSamplesPerSec | (uint32_t)readCharacter << i*8; 
  }
  printf("nSamplesPerSec: sampling rate (blocks per second)=%u\n", nSamplesPerSec);

  uint32_t nAverageBytesPerSecond = 0;
  for (int i=0;i<4;++i) {
    uint8_t readCharacter = fgetc(inputFile);
    nAverageBytesPerSecond = nAverageBytesPerSecond | (uint32_t)readCharacter << i*8; 
  }
  printf("nAverageBytesPerSecond: Data rate =%u\n", nAverageBytesPerSecond);

  uint32_t nBlockAlign = 0;
  for (int i=0;i<2;++i) {
    uint8_t readCharacter = fgetc(inputFile);
    nBlockAlign = nBlockAlign | (uint32_t)readCharacter << i*8; 
  }
  printf("nBlockAlign: Block Align=%u\n", nBlockAlign);

  uint32_t nBitsPerSample = 0;
  for (int i=0;i<2;++i) {
    uint8_t readCharacter = fgetc(inputFile);
    nBitsPerSample = nBitsPerSample | (uint32_t)readCharacter << i*8; 
  }
  printf("nBitsPerSample: Bits per  sample=%u\n", nBitsPerSample);

  int32_t currentPosition = ftell(inputFile);
  fgets(buffer128, 5, inputFile);
  printf("%d..%d bytes=%s\n", currentPosition, currentPosition+4, buffer128);

  uint32_t dataChunkSize = 0;
  for (int i=0;i<4;++i) {
    uint8_t readCharacter = fgetc(inputFile);
    dataChunkSize = dataChunkSize | (uint32_t)readCharacter << i*8; 
  }
  printf("dataChunkSize: chunk size=%u\n", dataChunkSize);

  //
  // FFTW part
  //
  const int64_t N = (int64_t)(dataChunkSize/2);
  fftw_complex *in, *out;
  fftw_plan p;

  in = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N);
  out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N);

  p = fftw_plan_dft_1d(N, in, out, FFTW_FORWARD, FFTW_ESTIMATE);

  for (int i=0; i<N; ++i) {
    in[i][REAL] = (uint8_t)fgetc(inputFile);
    in[i][IMAG] = (uint8_t)fgetc(inputFile);
  }

  fftw_execute(p);

  FILE* outputFile = fopen("./output.txt", "w");
  if (outputFile)
    for (int i=0; i<N; ++i) { fprintf(outputFile, "%g\n", *out[i]); }

  fftw_destroy_plan(p);
  fftw_free(in); 
  fftw_free(out);

  return 0;
}