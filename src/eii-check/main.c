#include <stdio.h>
#include <stdlib.h>
#include "../include/eii-puzzle.h"
#include "../common/util.h"

void printUsage(int argc, char** argv) {
  fprintf(stderr, "Usage: cat puzzle | %s\n", argv[0]);
  fprintf(stderr, "\n");
  fprintf(stderr, "The puzzle to solve is provided via stdin in JSON format.\n");
}

int main(int argc, char** argv) {
  if (argc != 1) {
    printUsage(argc, argv);
    return 1;
  }

  char* puzzleStr = fileToStr(stdin);
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

  EiiPuzzle puzzle;
  while ((ok = eiiPuzzleParseNext(&parseState, &puzzle)) == 0) {
    printf("{\"numEdgeConflicts\": %d}\n", eiiNumEdgeConflicts(puzzle));
  }

  eiiPuzzleParseFree(&parseState);
  free(puzzleStr);

  return 0;
}
