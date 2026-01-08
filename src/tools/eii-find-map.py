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

def mapTiles(tiles, fromColor, toColor):
    return [[toColor if c == fromColor else c for c in tile] for tile in tiles]

def findMapping(fromTiles, toTiles):
    fromColors = set(itertools.chain.from_iterable(fromTiles))
    toColors = set(itertools.chain.from_iterable(toTiles))
    mapping = {}

    for fromColor in fromColors:
        target = sortedTiles(blockTiles(fromTiles, [0, fromColor]))
        matches = {}
        for toColor in toColors:
            candidate = sortedTiles(blockTiles(toTiles, [0, toColor]))
            if mapTiles(target, fromColor, toColor) == candidate:
                matches[toColor] = candidate
        if len(matches) == 0:
            eprint("No matches for fromColor: " + str(fromColor))
            return None
        if len(matches) != 1:
            eprint("Multiple (" + str(len(matches)) + ") matches for fromColor: " + str(fromColor))
            return None
        toColor = list(matches.keys())[0]
        if toColor in list(mapping.values()):
            eprint("Multiple matches for toColor: " + str(toColor))
            return None
        mapping[fromColor] = toColor
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
