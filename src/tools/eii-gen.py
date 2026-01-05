#! /usr/bin/env nix-shell
#! nix-shell -i python -p python3
#
# Origin of coordinate system is top-left.

import argparse
import json
import pprint
import random
import sys
import itertools
from dataclasses import dataclass, field
from eii import *

def eprint(*args, **kwargs):
    print(*args, file=sys.stderr, **kwargs)

@dataclass
class EdgeType:
    numColors: int
    edgeIdxs: list[(int, int, int)] = field(default_factory=list)

def setEdge(board, row, col, nesw, color):
    board[row][col][nesw] = color
    if nesw == NORT and row > 0:
        board[row-1][col][SOUT] = color
    elif nesw == EAST and col < cols - 1:
        board[row][col+1][WEST] = color
    elif nesw == SOUT and row < rows - 1:
        board[row+1][col][NORT] = color
    elif nesw == WEST and col > 0:
        board[row][col-1][EAST] = color

def colorEdgesUniform(numColors, numEdges):
    edges = []
    for i in range(numEdges):
        edges.append(1 + (i % numColors))
    random.shuffle(edges)
    return edges

def genBoard():
    board = [[[0,0,0,0] for col in range(cols)] for row in range(rows)]

    for edgeType in [cb, bb, bi, ii]:
        colors = colorEdgesUniform(edgeType.numColors, len(edgeType.edgeIdxs))
        for color, (row, col, nesw) in zip(colors, edgeType.edgeIdxs):
            setEdge(board, row, col, nesw, color)

    return board

def tileAt(board, row, col):
    return [board[row][col][NORT], board[row][col][EAST], board[row][col][SOUT], board[row][col][WEST]]

def place(board, rowCol):
    row, col = rowCol
    return '{"row": ' + str(row) + ', "col": ' + str(col) + ', "tile": ' + str(tileAt(board, row, col)) + '}'

if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        prog = 'eii-gen',
        description = 'Generate an Eternity II puzzle. The border is color 0.')

    parser.add_argument('-r', '--rows', type=int, default=16)
    parser.add_argument('-c', '--cols', type=int, default=16)
    parser.add_argument('-cb', '--corner-border', type=int, default=4)
    parser.add_argument('-bb', '--border-border', type=int, default=5)
    parser.add_argument('-bi', '--border-inside', type=int, default=16)
    parser.add_argument('-ii', '--inside-inside', type=int, default=16)
    parser.add_argument('-s', '--seed', type=int, default=0)

    args = parser.parse_args()
    rows = args.rows
    random.seed(args.seed)
    cols = args.cols
    cb = EdgeType(numColors=args.corner_border)
    bb = EdgeType(numColors=args.border_border)
    bi = EdgeType(numColors=args.border_inside)
    ii = EdgeType(numColors=args.inside_inside)

    # Only use SOUT & EAST edges in edgeIdxs definitions. This helps have a single
    # canonical name for each edge. This does not matter here but it's good to be
    # consistant across all code.
    #
    # Note that cb, bb, bi, ii are all disjoint (ie. share no edges in common).
    # This is important if we decide to start swapping as part of the tile gen
    # to prevent rotational symmetry.

    cb.edgeIdxs += [(0, 0, EAST), (0, 0, SOUT)]
    cb.edgeIdxs += [(0, cols-2, EAST), (0, cols-1, SOUT)]
    cb.edgeIdxs += [(rows-1, 0, EAST), (rows-2, 0, SOUT)]
    cb.edgeIdxs += [(rows-1, cols-2, EAST), (rows-2, cols-1, SOUT)]

    bb.edgeIdxs += sum([[(row-1, 0, SOUT), (row-1, cols-1, SOUT)] for row in range(2, rows-1)], [])
    bb.edgeIdxs += sum([[(0, col-1, EAST), (rows-1, col-1, EAST)] for col in range(2, cols-1)], [])

    bi.edgeIdxs += sum([[(row, 0, EAST), (row, cols-2, EAST)] for row in range(1, rows-1)], [])
    bi.edgeIdxs += sum([[(rows-2, col, SOUT), (0, col, SOUT)] for col in range(1, cols-1)], [])

    ii.edgeIdxs += sum([[(row, col, SOUT), (row, col, EAST)] for col in range(1, cols-2) for row in range(1, rows-2)], [])
    ii.edgeIdxs += [(row, cols-2, SOUT) for row in range(1, rows-2)]
    ii.edgeIdxs += [(rows-2, col, EAST) for col in range(1, cols-2)]

    while True:
        board = genBoard()
        tiles = list(itertools.chain.from_iterable(board))
        if not hasDuplicateTiles(tiles) and not hasSymmetricTiles(tiles):
            break
        eprint('Regenerating due to:' + (' duplicates' if hasDuplicateTiles(board) else '') + (' symmetries' if hasSymmetricTiles(board) else ''))

    # Don't use json.dumps because it expands all arrays across multiple lines which is not readable.
    placedLocations = [(rows//2, (cols-1)//2), (2, 2), (2, cols - 3), (rows - 3, 2), (rows - 3, cols -3)]
    algo = "shuffle"
    name = '{}x{}-{}-{}-{}-{}-{}-seed-{}'.format(rows, cols, algo, args.corner_border, args.border_border, args.border_inside, args.inside_inside, args.seed)
    print('{')
    print('  "name": "{}",'.format(name))
    print('  "gen": {{"size": "{}", "algo": "{}", "cb": {}, "bb": {}, "bi": {}, "ii": {}, "seed": {}}},'.format(
        str(rows) + 'x' + str(cols), algo, args.corner_border, args.border_border, args.border_inside, args.inside_inside, args.seed)
    )
    print('  "rows": {},'.format(rows))
    print('  "cols": {},'.format(cols))
    print('  "tiles": [')
    print(',\n'.join(['    {}'.format(str(tile)) for row in board for tile in row]))
    print('  ],')
    print('  "placed": [')
    print(',\n'.join(['    ' + place(board, rowCol) for rowCol in placedLocations]))
    print('  ]')
    print('}')
