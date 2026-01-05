#! /usr/bin/env nix-shell
#! nix-shell -i python -p python3

import json
import sys
import argparse
import itertools
from eii import *

parser = argparse.ArgumentParser(
                    prog='eiiFindPuzzleMap',
                    description='Map colors in eternity ii puzzle json files.')
parser.add_argument('-p', type=str, help="filename of puzzle to map from")
parser.add_argument('-q', type=str, help="filename of puzzle to map to")

def eprint(*args, **kwargs):
    print(*args, file=sys.stderr, **kwargs)

def blockColor(color, exceptColors):
    if color in exceptColors:
        return color
    else:
        return 999

def blockTiles(tiles, exceptColors):
    return [[blockColor(c, exceptColors) for c in tile] for tile in tiles]

def mapTiles(tiles, xColor, yColor):
    return [[yColor if c == xColor else c for c in tile] for tile in tiles]

def findMapping(xTiles, yTiles):
    xColors = set(itertools.chain.from_iterable(xTiles))
    yColors = set(itertools.chain.from_iterable(yTiles))
    mapping = {}

    for xColor in xColors:
        target = sortedTiles(blockTiles(xTiles, [0, xColor]))
        matches = {}
        for yColor in yColors:
            candidate = sortedTiles(blockTiles(yTiles, [0, yColor]))
            if mapTiles(target, xColor, yColor) == candidate:
                matches[yColor] = candidate
        if len(matches) == 0:
            eprint("No matches for xColor: " + str(xColor))
            return None
        if len(matches) != 1:
            eprint("Multiple (" + str(len(matches)) + ") matches for xColor: " + str(xColor))
            return None
        yColor = list(matches.keys())[0]
        if yColor in list(mapping.values()):
            eprint("Multiple matches for yColor: " + str(yColor))
            return None
        mapping[xColor] = yColor
    return mapping

if __name__ == "__main__":
    args = parser.parse_args()

    puzzleFrom = None
    with open(args.p) as f:
        puzzleFrom = json.loads(f.read())

    puzzleTo = None
    with open(args.q) as f:
        puzzleTo = json.loads(f.read())

    print(findMapping(puzzleFrom['tiles'], puzzleTo['tiles']))
