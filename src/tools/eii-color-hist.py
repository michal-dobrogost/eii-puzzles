#! /usr/bin/env nix-shell
#! nix-shell -i python -p python3

import argparse
import ast
import json
from collections import defaultdict
import itertools
from eii import *

parser = argparse.ArgumentParser(
                    description='Generate color histograms for all tile edge types (corner-border, border-border, border-inside, inside-inside).')
parser.add_argument('-p', type=str, help="filename of puzzle to map from")

def colorsCornerBorder(tiles):
    return itertools.chain.from_iterable([[t[SOUT], t[WEST]] for t in tiles[0:4]])

def colorsBorderBorder(tiles):
    numBorders = 2 * puzzle['rows'] + 2 * puzzle['cols'] - 4
    return itertools.chain.from_iterable([[t[EAST], t[WEST]] for t in tiles[4:numBorders]])

def colorsBorderInside(tiles):
    numBorders = 2 * puzzle['rows'] + 2 * puzzle['cols'] - 4
    return [t[SOUT] for t in tiles[4:numBorders]]

def colorsInsideInside(tiles):
    numBorders = 2 * puzzle['rows'] + 2 * puzzle['cols'] - 4
    return [t[dir] for t in tiles[numBorders:] for dir in [0,1,2,3]]

def histFromColors(colors):
    hist = defaultdict(int)
    for c in colors:
        hist[c] += 1
    return dict(hist)

def printHist(name, hist):
    print(name + ': ' + str(len(hist)) + ' ' + str(hist))

if __name__ == "__main__":
    args = parser.parse_args()

    puzzle = None
    with open(args.p) as f:
        puzzle = json.loads(f.read())

    tiles = sortedTiles(puzzle['tiles'])
    printHist('corner-border', histFromColors(colorsCornerBorder(tiles)))
    printHist('border-border', histFromColors(colorsBorderBorder(tiles)))
    printHist('border-inside', histFromColors(colorsBorderInside(tiles)))
    printHist('inside-inside', histFromColors(colorsInsideInside(tiles)))
