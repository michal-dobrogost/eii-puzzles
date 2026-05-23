#pragma once

#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "jsmn.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum EiiDir {
  EII_NORT = 0,
  EII_EAST = 1,
  EII_SOUT = 2,
  EII_WEST = 3,
  EII_NUM_DIRS = 4
} EiiDir;

/** A single tile of an Eternity II puzzle. */
typedef struct EiiTile {
  int nesw[EII_NUM_DIRS];
} EiiTile;

typedef struct EiiPlaced {
  int row;
  int col;
  EiiTile tile;
} EiiPlaced;

/** An Eternity II puzzle. */
typedef struct EiiPuzzle {
  char* name;
  int rows;
  int cols;
  int size; // rows * cols
  EiiTile* tiles;
  EiiPlaced* placed;
  int placedSize;
  char* genJSON; // Contains the JSON string with properties of the puzzle like number of BI colors (same info as name in different format).
} EiiPuzzle;

/** Zero initialize the puzzle's fields. */
static void eiiPuzzleInit(EiiPuzzle* puzzle);

/** Free the memory held by this puzzle object (not the object itself). */
static void eiiPuzzleFree(EiiPuzzle* puzzle);

/**
 * Rotate each tile into it's minimum configuration then sort lexicographically.
 * Corners are rotated into (0,0,x,y), borders into (0,x,y,z).
 * Corner tiles are first, then border tiles then inside tiles.
 *
 * @param puzzle the puzzle to be sorted (in-place).
 *
 * @return 0 on success;
 */
static int eiiPuzzleSort(EiiPuzzle* puzzle);

/**
 * Parse a single JSON eternity ii instance.
 *
 * @param str (in) JSON string to parse. Must stay the same until call to eiiPuzzleParseFree().
 * @param puzzle (mod) parsed pauzzle. Caller responsible for free(puzzle.pieces).
 * @return 0 on success.
 */
static int eiiPuzzleParseOne(const char* str, EiiPuzzle* puzzle);

/** State carried by a parser */
typedef struct eiiPuzzleParseState {
  const char* str;
  jsmn_parser p;
  jsmntok_t* t;
  int numTokens;
  int offset;
} eiiPuzzleParseState;

/**
 * Initialize a JSON eternity ii puzzle parser.
 * You must call eiiPuzzleParseFree() after successful inits.
 *
 * @param state (mod) parser state.
 * @param str (in) JSON string to parse. Must stay the same until call to eiiPuzzleParseFree().
 * @return 0 on success.
 */
static int eiiPuzzleParseInit(eiiPuzzleParseState* state, const char* str);

/**
 * Parse the next substring containing a JSON eternity ii puzzle into a C Struct representation.
 * @return 0 on success, -1 on end of input, positive value on error.
 */
static int eiiPuzzleParseNext(eiiPuzzleParseState* state, EiiPuzzle* puzzle);

/** Must be called after you're done parsing. */
static void eiiPuzzleParseFree(eiiPuzzleParseState* state);

/**
 * Print a string representation of a EiiPuzzle struct to a FILE stream.
 *
 * Format is similar to:
 *  0   0
 * 0:1 1:3 ...
 *  2   4
 *  ...    ...
 *
 * @param puzzle (in) the puzzle to generate a string for.
 * @param stream (mod) the FILE stream to print into.
 * @return 0 on success
 */
static int eiiPuzzlePrint(EiiPuzzle puzzle, FILE* stream);

/**
 * Print a string representation of a EiiPuzzle struct to a FILE stream.
 *
 * Format is similar to:
 * 0120 0341 ...
 * ...       ...
 *
 * @param puzzle (in) the puzzle to generate a string for.
 * @param stream (mod) the FILE stream to print into.
 * @return 0 on success
 */
static int eiiPuzzlePrintFlat(EiiPuzzle puzzle, FILE* stream);

/**
 * Print a string representation of a EiiPuzzle struct to a FILE stream.
 *
 * Format to JSON as used in parsing. Tiles use row-major order.
 * {
 *   "name": "8x8-shuffle-4-5-7-7",
 *   "rows": 16,
 *   "cols": 16,
 *   "tiles": [
 *     [0, 0, 1, 2],
 *     ...
 *   ]
 * }
 *
 * @param puzzle (in) the puzzle to generate a string for.
 * @param stream (mod) the FILE stream to print into.
 * @return 0 on success
 */
static int eiiPuzzlePrintJSON(EiiPuzzle puzzle, FILE* stream);

/**
 * Returns the number of edge conflicts in the puzzle or a negative value on error.
 * An edge mismatch between two tiles is counted twice (once for each tile).
 * An edge mismatch at the border (grey/0) is counted once.
 */
static int eiiNumEdgeConflicts(EiiPuzzle puzzle);

/**
 * Rotate a tile counter-clockwise.
 *
 * @param rot The number of rotations to perform
 * @return The rotated tile.
 */
static EiiTile eiiTileRotate(EiiTile x, int rot);

/**
 * Rotate a tile to it's minimum lexicographic configuration.
 *
 * @param rot The number of rotations to perform
 * @return The rotated tile.
 */
static EiiTile eiiTileRotateToMin(EiiTile x);

/**
 * Find the tilerot (4 * tile-id + rotation) of the given tile.
 *
 * @return The tilerot or -1 if not found.
 */
static int eiiTilerotFromTile(EiiTile target, EiiTile const* tiles, int numTiles);

/** Rotate a tilerot (ie. modify it's rot component while leaving the tile component). */
static int eiiTilerotRotate(int tilerot, int rot);

////////////////////////////////////////////////////////////////////////////////
// Implementation
////////////////////////////////////////////////////////////////////////////////

static void eiiPuzzleInit(EiiPuzzle* puzzle) {
  if (!puzzle) { return; }
  puzzle->name = NULL;
  puzzle->tiles = NULL;
  puzzle->placed = NULL;
  puzzle->genJSON = NULL;
  puzzle->rows = 0;
  puzzle->cols = 0;
  puzzle->size = 0;
  puzzle->placedSize = 0;
}

static void eiiPuzzleFree(EiiPuzzle* puzzle) {
  if (!puzzle) { return; }
  free(puzzle->name);
  free(puzzle->tiles);
  free(puzzle->placed);
  free(puzzle->genJSON);
  eiiPuzzleInit(puzzle);
}

static int eiiTileComp(const void* lVoid, const void* rVoid) {
  EiiTile* l = (EiiTile*) lVoid;
  EiiTile* r = (EiiTile*) rVoid;
  for (int i = 0; i < EII_NUM_DIRS; ++i) {
    if (l->nesw[i] < r->nesw[i]) return -1;
    if (l->nesw[i] > r->nesw[i]) return 1;
  }
  return 0;
}

static EiiTile eiiTileRotate(EiiTile x, int rot) {
  EiiTile y;
  rot = ((rot % 4) + 4) % 4;
  for (int i = 0; i < EII_NUM_DIRS; ++i) {
    y.nesw[i] = x.nesw[(i + rot) % EII_NUM_DIRS];
  }
  return y;
}

static EiiTile eiiTileRotateToMin(EiiTile x) {
  EiiTile minTile = x;
  for (int rot = 1; rot < EII_NUM_DIRS; ++rot) {
    EiiTile y = eiiTileRotate(x, rot);
    if (eiiTileComp(&y, &minTile) < 0) {
      minTile = y;
    }
  }
  return minTile;
}

static int eiiPuzzleSort(EiiPuzzle* puzzle) {
  if (puzzle == NULL) return 1;
  if (puzzle->tiles == NULL) return 2;
  if (puzzle->size < 0) return 3;

  for (int i = 0; i < puzzle->size; ++i) {
    puzzle->tiles[i] = eiiTileRotateToMin(puzzle->tiles[i]);
  }
  qsort(puzzle->tiles, puzzle->size, sizeof(EiiTile), eiiTileComp);
  return 0;
}

static int eiiTilerotFromTile(EiiTile target, const EiiTile* tiles, const int numTiles) {
  EiiTile targetMin = eiiTileRotateToMin(target);
  for (int iTile = 0; iTile < numTiles; ++iTile) {
    EiiTile candidateMin = eiiTileRotateToMin(tiles[iTile]);
    if (eiiTileComp(&targetMin, &candidateMin) == 0) {
      for (int rot = 0; rot < 4; ++rot) {
        EiiTile candidateRotated = eiiTileRotate(tiles[iTile], rot);
        if (eiiTileComp(&target, &candidateRotated) == 0) {
          return 4 * iTile + rot;
        }
      }
    }
  }
  return -1;
}

static int eiiTilerotRotate(int tilerot, int rotation) {
  const int tile = tilerot / 4;
  const int rot = tilerot % 4;
  return tile*4 + ((rot + rotation) % 4);
}

static int eiiJsonEq(const char *json, jsmntok_t *tok, const char *s) {
  if (tok->type == JSMN_STRING && (int)strlen(s) == tok->end - tok->start &&
      strncmp(json + tok->start, s, tok->end - tok->start) == 0) {
    return 0;
  }
  return -1;
}

static int eiiPuzzleParseOne(const char* str, EiiPuzzle* puzzle) {
  eiiPuzzleParseState s;
  int ok = eiiPuzzleParseInit(&s, str);
  if (ok != 0) {
    return ok;
  }

  ok = eiiPuzzleParseNext(&s, puzzle);
  eiiPuzzleParseFree(&s);
  return ok;
}

static int eiiPuzzleParseInit(eiiPuzzleParseState* s, const char* str) {
  if (!s) {
    return 1;
  }

  s->str = str;
  s->t = NULL;
  s->numTokens = 0;
  s->offset = 0;

  if (str == NULL) {
    return 1;
  }

  jsmn_init(&s->p);

  size_t strLen = strlen(str);
  s->t = (jsmntok_t*) malloc(sizeof(jsmntok_t) * strLen);
  if (s->t == NULL) {
    eiiPuzzleParseFree(s);
    return 2;
  }

  s->numTokens = jsmn_parse(&s->p, str, strLen, s->t, strLen);
  if (s->numTokens < 0) {
    int err = s->numTokens;
    eiiPuzzleParseFree(s);
    return err; // r error values are negative.
  }

  return 0;
}

static int eiiPuzzleParseNext(eiiPuzzleParseState* s, EiiPuzzle* puzzle) {
#define EII_PUZZLE_PARSE_NEXT_FREE(retVal) (eiiPuzzleFree(puzzle), retVal) // TODO: use this everywhere.
  if (puzzle == NULL) {
    return 3;
  }
  eiiPuzzleInit(puzzle);

  // Check for end of input.
  if (s->offset >= s->numTokens) {
    return -1; // All done.
  }

  int tilesAllocSize = 16*16;
  puzzle->tiles = (EiiTile*) malloc(sizeof(EiiTile) * tilesAllocSize);
  if (puzzle->tiles == NULL) {
    return EII_PUZZLE_PARSE_NEXT_FREE(4);
  }

  // Check for a missing top-level puzzle object.
  if (s->numTokens < 1 || s->t[s->offset].type != JSMN_OBJECT) {
    return EII_PUZZLE_PARSE_NEXT_FREE(5);
  }
  const int puzzleObjectSize = s->t[s->offset].size;
  s->offset++;

  int numTilesJson = 0;
  for (int i = 0; i < puzzleObjectSize; ++i) {
    if (eiiJsonEq(s->str, &s->t[s->offset], "name") == 0) {
      s->offset++;
      size_t len = s->t[s->offset].end - s->t[s->offset].start;
      puzzle->name = (char*) malloc(len + 1);
      memset(puzzle->name, 0, len + 1);
      memcpy(puzzle->name, s->str + s->t[s->offset].start, len);
      s->offset++;
    }
    if (eiiJsonEq(s->str, &s->t[s->offset], "rows") == 0) {
      s->offset++;
      puzzle->rows = strtol(s->str + s->t[s->offset].start, NULL, 10);
      s->offset++;
    }
    if (eiiJsonEq(s->str, &s->t[s->offset], "cols") == 0) {
      s->offset++;
      puzzle->cols = strtol(s->str + s->t[s->offset].start, NULL, 10);
      s->offset++;
    }
    if (eiiJsonEq(s->str, &s->t[s->offset], "tiles") == 0) {
      s->offset++;

      if (s->t[s->offset].type != JSMN_ARRAY) {
        return EII_PUZZLE_PARSE_NEXT_FREE(6);
      }
      numTilesJson = s->t[s->offset].size;
      s->offset++;

      for (int iTile = 0; iTile < numTilesJson; ++iTile) {
        if (s->t[s->offset].type != JSMN_ARRAY || s->t[s->offset].size != EII_NUM_DIRS) {
          return EII_PUZZLE_PARSE_NEXT_FREE(7);
        }
        s->offset++;

        for (int iNESW = 0; iNESW < EII_NUM_DIRS; ++iNESW) {
          if (s->t[s->offset].type != JSMN_PRIMITIVE) {
            return EII_PUZZLE_PARSE_NEXT_FREE(8);
          }
          errno = 0;
          puzzle->tiles[iTile].nesw[iNESW] = strtol(s->str + s->t[s->offset].start, NULL, 10);
          s->offset++;
          if (errno != 0) {
            // errno values tend to be large, like 34.
            return EII_PUZZLE_PARSE_NEXT_FREE(errno);
          }
        }
      }
    }
    if (eiiJsonEq(s->str, &s->t[s->offset], "placed") == 0) {
      s->offset++;

      if (s->t[s->offset].type != JSMN_ARRAY) {
        return EII_PUZZLE_PARSE_NEXT_FREE(9);
      }
      puzzle->placedSize = s->t[s->offset].size;
      puzzle->placed = (EiiPlaced*) malloc(sizeof(EiiPlaced) * puzzle->placedSize);
      if (! puzzle->placed) {
          return EII_PUZZLE_PARSE_NEXT_FREE(10);
      }
      s->offset++;

      for (int iPlaced = 0; iPlaced < puzzle->placedSize; ++iPlaced) {
        if (s->numTokens < 1 || s->t[s->offset].type != JSMN_OBJECT) {
          return EII_PUZZLE_PARSE_NEXT_FREE(11);
        }
        const int placedObjectSize = s->t[s->offset].size;
        s->offset++;

        for (int iPlacedObj = 0; iPlacedObj < placedObjectSize; ++iPlacedObj) {
          if (eiiJsonEq(s->str, &s->t[s->offset], "row") == 0) {
            s->offset++;
            puzzle->placed[iPlaced].row = strtol(s->str + s->t[s->offset].start, NULL, 10);
            s->offset++;
          }
          if (eiiJsonEq(s->str, &s->t[s->offset], "col") == 0) {
            s->offset++;
            puzzle->placed[iPlaced].col = strtol(s->str + s->t[s->offset].start, NULL, 10);
            s->offset++;
          }
          if (eiiJsonEq(s->str, &s->t[s->offset], "tile") == 0) {
            s->offset++;

            if (s->t[s->offset].type != JSMN_ARRAY || s->t[s->offset].size != EII_NUM_DIRS) {
              return EII_PUZZLE_PARSE_NEXT_FREE(12);
            }
            s->offset++;

            for (int iNESW = 0; iNESW < EII_NUM_DIRS; ++iNESW) {
              if (s->t[s->offset].type != JSMN_PRIMITIVE) {
                return EII_PUZZLE_PARSE_NEXT_FREE(13);
              }
              errno = 0;
              puzzle->placed[iPlaced].tile.nesw[iNESW] = strtol(s->str + s->t[s->offset].start, NULL, 10);
              s->offset++;
              if (errno != 0) {
                // errno values tend to be large, like 34.
                return EII_PUZZLE_PARSE_NEXT_FREE(errno);
              }
            }
          }
        }
      }
    }
    if (eiiJsonEq(s->str, &s->t[s->offset], "gen") == 0) {
      s->offset++;
      size_t len = s->t[s->offset].end - s->t[s->offset].start;
      puzzle->genJSON = (char*) malloc(len + 1);
      memset(puzzle->genJSON, 0, len + 1);
      memcpy(puzzle->genJSON, s->str + s->t[s->offset].start, len);
      s->offset += s->t[s->offset].size * 2 + 1;
    }
  }

  if (puzzle->rows <= 0 || puzzle->cols <= 0) {
    return EII_PUZZLE_PARSE_NEXT_FREE(14);
  }
  puzzle->size = puzzle->rows * puzzle->cols;

  if (numTilesJson != puzzle->size) {
    return EII_PUZZLE_PARSE_NEXT_FREE(15);
  }

  return 0;
}

static void eiiPuzzleParseFree(eiiPuzzleParseState* s) {
  if (s) {
    s->str = NULL;
    free(s->t);
    s->t = NULL;
    s->numTokens = 0;
    s->offset = 0;
  }
}

/** Convert integer to a character in 0-9a-zA-Z with '<' and '>' representing out of range. */
static char eiiIntToChar(int x) {
  static const char* chars = "0123456789"
                             "abcdefghijklmnopqrstuvwxyz"
                             "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
  if (x < 0) { return '<'; }
  if (x >= 26*2 + 10) { return '>'; }
  return chars[x];
}

static int eiiPuzzlePrint(EiiPuzzle puzzle, FILE* stream) {
  if (puzzle.tiles == NULL || stream == NULL) {
    return 1;
  }

  size_t bufferSize = puzzle.cols * 4 + 3;
  char* buffer = (char*) malloc(bufferSize);
  buffer[bufferSize-2] = '\n';
  buffer[bufferSize-1] = '\0';
  for (int row = 0; row < puzzle.rows; ++row) {
    // Print North Edges
    memset(buffer, ' ', bufferSize-2);
    for (int col = 0; col < puzzle.cols; ++col) {
      buffer[1 + col*4] = eiiIntToChar(puzzle.tiles[row * puzzle.cols + col].nesw[EII_NORT]);
    }
    fputs(buffer, stream);
    // Print East & West Edges
    memset(buffer, ' ', bufferSize-2);
    for (int col = 0; col < puzzle.cols; ++col) {
      EiiTile p = puzzle.tiles[row * puzzle.cols + col];
      buffer[col * 4] = eiiIntToChar(p.nesw[EII_WEST]);
      buffer[col * 4 + 1] = ':';
      buffer[col * 4 + 2] = eiiIntToChar(p.nesw[EII_EAST]);
    }
    fputs(buffer, stream);
    // Print South Edges
    memset(buffer, ' ', bufferSize-2);
    for (int col = 0; col < puzzle.cols; ++col) {
      buffer[1 + col*4] = eiiIntToChar(puzzle.tiles[row * puzzle.cols + col].nesw[EII_SOUT]);
    }
    fputs(buffer, stream);
  }
  free(buffer);

  return 0;
}

static int eiiPuzzlePrintFlat(EiiPuzzle puzzle, FILE* stream) {
  if (puzzle.tiles == NULL || stream == NULL) {
    return 1;
  }

  for (int row = 0; row < puzzle.rows; ++row) {
    for (int col = 0; col < puzzle.cols; ++col) {
      if (col > 0) {
        fputc(' ', stream);
      }
      for (int edge = 0; edge < EII_NUM_DIRS; ++edge) {
        fputc(eiiIntToChar(puzzle.tiles[row*puzzle.cols + col].nesw[edge]), stream);
      }
    }
    fputc('\n', stream);
  }

  return 0;
}

static int eiiPuzzlePrintJSON(EiiPuzzle puzzle, FILE* stream) {
  if (puzzle.tiles == NULL || stream == NULL) {
    return 1;
  }

  fprintf(stream, "{\n");
  if (puzzle.name != NULL) {
    fprintf(stream, "  \"name\": \"%s\",\n", puzzle.name);
  }
  if (puzzle.genJSON != NULL) {
    fprintf(stream, "  \"gen\": %s,\n", puzzle.genJSON);
  }
  fprintf(stream, "  \"rows\": %d,\n", puzzle.rows);
  fprintf(stream, "  \"cols\": %d,\n", puzzle.cols);
  fprintf(stream, "  \"tiles\": [\n");
  for (int i = 0; i < puzzle.size; ++i) {
    EiiTile* tile = &puzzle.tiles[i];
    fprintf(stream, "    [%d, %d, %d, %d]", tile->nesw[EII_NORT], tile->nesw[EII_EAST], tile->nesw[EII_SOUT], tile->nesw[EII_WEST]);
    if (i != puzzle.size - 1) { fprintf(stream, ","); }
    fprintf(stream, "\n");
  }
  fprintf(stream, "  ],\n");
  if (puzzle.placedSize == 0) {
    fprintf(stream, "  \"placed\": []\n");
  }
  else {
    fprintf(stream, "  \"placed\": [\n");
    for (int i = 0; i < puzzle.placedSize; ++i) {
      EiiPlaced* placed = &puzzle.placed[i];
      fprintf(stream, "    {\"row\": %d, \"col\": %d, \"tile\": [%d, %d, %d, %d]}", placed->row, placed->col, placed->tile.nesw[EII_NORT], placed->tile.nesw[EII_EAST], placed->tile.nesw[EII_SOUT], placed->tile.nesw[EII_WEST]);
      if (i != puzzle.placedSize - 1) { fprintf(stream, ","); }
      fprintf(stream, "\n");
    }
    fprintf(stream, "  ]\n");
  }
  fprintf(stream, "}\n");

  return 0;
}

static int eiiNumEdgeConflicts(EiiPuzzle puzzle) {
  int count = 0;
  for (int row = 0; row < puzzle.rows; ++row) {
    for (int col = 0; col < puzzle.cols; ++col) {
      int idx = row * puzzle.cols + col;
      if ((row == 0 ? 0 : puzzle.tiles[idx - puzzle.cols].nesw[EII_SOUT]) != puzzle.tiles[idx].nesw[EII_NORT]) {
        count += 1;
      }
      if ((row == puzzle.rows - 1 ? 0 : puzzle.tiles[idx + puzzle.cols].nesw[EII_NORT]) != puzzle.tiles[idx].nesw[EII_SOUT]) {
        count += 1;
      }
      if ((col == puzzle.cols - 1 ? 0 : puzzle.tiles[idx + 1].nesw[EII_WEST]) != puzzle.tiles[idx].nesw[EII_EAST]) {
        count += 1;
      }
      if ((col == 0 ? 0 : puzzle.tiles[idx - 1].nesw[EII_EAST]) != puzzle.tiles[idx].nesw[EII_WEST]) {
        count += 1;
      }
    }
  }
  return count;
}

#ifdef __cplusplus
}
#endif
