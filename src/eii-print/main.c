#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "../include/eii-puzzle.h"
#include "../common/util.h"

void printUsage(int argc, char** argv) {
  fprintf(stderr, "Usage: cat puzzle | %s [--sort] [--flat|--json]\n", argv[0]);
  fprintf(stderr, "  --sort: rotates each tile to min and sorts the tiles.\n");
  fprintf(stderr, "\n");
  fprintf(stderr, "By default outputs each row of tiles across three lines. Other options:\n");
  fprintf(stderr, "  --flat: outputs in each row of tiles on a single line.\n");
  fprintf(stderr, "  --json: outputs in pretty printed json format.\n");
  fprintf(stderr, "\n");
  fprintf(stderr, "The puzzle to solve is provided via stdin in JSON format.\n");
}

int main(int argc, char** argv) {
  bool sort = false;
  bool flat = false;
  bool json = false;
  char* puzzleStr = NULL;
  for (int i = 1; i < argc; ++i) {
    if (strcmp(argv[i], "--sort") == 0) {
      sort = true;
    }
    else if (strcmp(argv[i], "--flat") == 0) {
      flat = true;
    }
    else if (strcmp(argv[i], "--json") == 0) {
      json = true;
    }
    else {
      printUsage(argc, argv);
      return 1;
    }
  }

  puzzleStr = fileToStr(stdin);
  if (puzzleStr == NULL) {
    printUsage(argc, argv);
    fprintf(stderr, "\nFailed to read stdin\n");
    return 2;
  }

  eiiPuzzleParseState parseState;
  int ok = eiiPuzzleParseInit(&parseState, puzzleStr);
  if (ok != 0) {
    printUsage(argc, argv);
    fprintf(stderr, "\nParse error: %d\n", ok);
    return 3;
  }

  int numPuzzles = 0;
  EiiPuzzle puzzle;
  while ((ok = eiiPuzzleParseNext(&parseState, &puzzle)) == 0) {
    if (++numPuzzles > 1) {
      printf("\n");
    }

    if (sort && (ok = eiiPuzzleSort(&puzzle)) != 0) {
      fprintf(stderr, "Sort error: %d\n", ok);
      return 4;
    }

    if (flat) {
      ok = eiiPuzzlePrintFlat(puzzle, stdout);
      if (ok != 0) {
        fprintf(stderr, "PrintFlat error: %d\n", ok);
        return 5;
      }
    }
    else if (json) {
      ok = eiiPuzzlePrintJSON(puzzle, stdout);
      if (ok != 0) {
        fprintf(stderr, "PrintJSON error: %d\n", ok);
        return 6;
      }
    }
    else  {
      ok = eiiPuzzlePrint(puzzle, stdout);
      if (ok != 0) {
        fprintf(stderr, "Print error: %d\n", ok);
        return 7;
      }
    }
  }

  if (ok != -1) {
    printUsage(argc, argv);
    fprintf(stderr, "\nParse error: %d\n", ok);
    return 3;
  }

  eiiPuzzleParseFree(&parseState);
  free(puzzleStr);

  return 0;
}
