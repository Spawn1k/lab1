#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <math.h>

const char *optString = "h";

char *outputString = NULL;
char *convVariable = NULL;
int endOfOutputString = 0;
int endOfConvVariable = 0;

char hexNumbers[16] = {
  '0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'
};

struct globalArgs_t {
  char* inputPath;
  char* outputPath;
  FILE* inputFile;
  FILE* outputFile;
} globalArgs;

void addCharToOutput(char target_char) {
  endOfOutputString++;
  outputString = realloc(outputString, endOfOutputString * sizeof(char));
  outputString[endOfOutputString-1] = target_char;
}

void addCharToConvertVar(char target_char) {
  endOfConvVariable++;
  convVariable = realloc(convVariable, endOfConvVariable * sizeof(char));
  convVariable[endOfConvVariable-1] = target_char;
  convVariable[endOfConvVariable] = '\0';
}

void printHexToDec() {
  int outputNumber = 0;
  for (int i = endOfConvVariable-1; i >= 2; i--) {
    outputNumber += (convVariable[i] - '0')*(int)pow(16, endOfConvVariable-1-i);
  }
  char *output;
  asprintf(&output, "%d", outputNumber);

  for (int i = 0; i <= strlen(output); i++) {
    addCharToOutput(output[i]);
  }
}

void printDecToHex() {
  int number = atoi(convVariable);
  printf("Number: %d\n", number);
  int len = 0;
  while (number > 0) {
    number /= 16;
    len++;
  }
  printf("Len: %d\n", len);
  char output[len+1];
  output[len] = '\0';
  number = atoi(convVariable);
  while (number > 0) {
    output[len-1] = hexNumbers[number % 16];
    number /= 16;
    len--;
  }

  addCharToOutput('0');
  addCharToOutput('x');
  for (int i = 0; i < strlen(output); i++) {
    addCharToOutput(output[i]);
  }
}

void justPrint() {
  for (int i = 0; i < endOfConvVariable; i++) addCharToOutput(convVariable[i]);
}

void addConvToOutput() {
  short int state = 1;

  // state 1 - numbers
  // state 2 - hex
  // state 0 - nothing

  if (convVariable[0] == '0' && convVariable[1] == 'x') state = 2;
  if (!(convVariable[1] >= 48 && convVariable[1] <= 57) && state == 1) state = 0;

  for (int i = 2; i < endOfConvVariable; i++) {
    if (state == 2) {
      printf(" - hex: '%c'\n", convVariable[i]);
      if (!((convVariable[i] >= 48 && convVariable[i] <= 57)||(convVariable[i] >= 65 && convVariable[i] <= 70))){
        state = 0;
        break;
      }
    }
    if (state == 1) {
      printf(" - num: '%c'\n", convVariable[i]);
      if (convVariable[i] < 48 || convVariable[i] > 57){
        state = 0;
        break;
      }
    }
  }

  switch (state) {
    case 0:
      justPrint();
      break;
    case 1:
      printDecToHex();
      break;
    case 2:
      printHexToDec();
      break;
    default:
      justPrint();
      break;
  }

  convVariable = NULL;
  endOfConvVariable = 0;
}

void startJob() {
  short int state = 0;
  char inChar;

  // state 1 - stasrt reading numbers
  // state 10 - first number is 0
  // state 2 - reading 0x
  // state 3 - reading 10
  // state 4 - finish reading
  // state 0 - casual reading

  while (!feof(globalArgs.inputFile)){
    fscanf(globalArgs.inputFile,"%c",&inChar);
    if ((inChar >= 48 && inChar <= 57) && state == 0 && ((endOfOutputString > 0 && (outputString[endOfOutputString-1] == ' ' || outputString[endOfOutputString-1] == '\n' || outputString[endOfOutputString-1] == '\0')) || endOfOutputString
   == 0)) {
      state = 1;
    } else if ((inChar == ' ' || inChar == '\n' || inChar == '\0') && (state == 1)) {
      state = 2;
    }
    if (state == 1) {
      addCharToConvertVar(inChar);
    } else if (state == 2) {
      addConvToOutput();
      state = 0;
    }
    if (state == 0) {
      addCharToOutput(inChar);
    }
  }
}

void display_usage(char* name) {
  printf("\nUSAGE:\n%s [-h] \n\nARGS: \n-h: Help\n\n", name);
  exit(EXIT_SUCCESS);
}

int getStartData(int argc, char** argv) {
  int opt = 0;

  opt = getopt(argc, argv, optString);
  while (opt != -1) {
    switch (opt) {
      case 'h':
        display_usage(argv[0]);
        break;
      default:
        break;
    }
    opt = getopt(argc, argv, optString);
  }

  if (optind < argc) {
    globalArgs.inputPath = argv[optind++];
    if (optind < argc) {
      globalArgs.outputPath = argv[optind];
    }
  }

  return 1;
}

int main(int argc, char** argv) {

  globalArgs.inputPath = NULL;
  globalArgs.outputPath = NULL;

  if (getStartData(argc, argv) == 0) printf("Error occured - programm has stopped\n");

  if (globalArgs.outputPath != NULL) {
    if ((globalArgs.outputFile = fopen(globalArgs.outputPath, "w")) == NULL) {
      fprintf(stderr, "Can not open output file!\n");
      exit(EXIT_FAILURE);
    }
  } else {
    globalArgs.outputFile = stdout;
  }

  if (globalArgs.inputPath != NULL) {
    if ((globalArgs.inputFile = fopen(globalArgs.inputPath, "r")) == NULL) {
      fprintf(stderr, "Can not open input file!\n");
      exit(EXIT_FAILURE);
    }
  } else {
    globalArgs.inputFile = stdin;
  }

  startJob();

  if (globalArgs.inputPath == NULL) {
    endOfOutputString -= 2;
  }

  if (endOfOutputString != 0) {
    for (int i = 0; i <= endOfOutputString; i++) {
      fprintf(globalArgs.outputFile, "%c", outputString[i]);
    }
  }

  return 0;
}
