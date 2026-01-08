#! /usr/bin/env nix-shell
#! nix-shell -i python -p python3

import argparse
import ast
import json
import sys
from eii import *

placedDefault = [
    {'row': 8, 'col': 7, 'tile': ["yell-pblu-star", "purp-yell-arro", "purp-yell-arro", "gree-oran-snow"]},  # tile 139 (1-based)
    {'row': 2, 'col': 2, 'tile': ["dblu-pink-arro", "maro-gree-arro", "gree-oran-snow", "maro-yell-star"]},  # tile 208 (1-based)
    {'row': 2, 'col': 13, 'tile': ["dblu-pink-arro", "oran-purp-star", "dblu-pink-arro", "dblu-pblu-squa"]}, # tile 255 (1-based)
    {'row': 13, 'col': 2, 'tile': ["dblu-pblu-squa", "purp-yell-arro", "pink-yell-towe", "gree-oran-snow"]}, # tile 181 (1-based)
    {'row': 13, 'col': 13, 'tile': ["yell-dblu-towe", "pblu-pink-snow", "pink-yell-towe", "yell-gree-squa"]} # tile 249 (1-based)
]

parser = argparse.ArgumentParser(
                    prog='eii-color-edges',
                    description='Generate numerical colors for all tile edges in an eternity ii transcription.')
parser.add_argument('-t', '--transcription', type=str, help="filename of transcription to map from")
parser.add_argument('--namespace', action='store_true', help="Treat border edges and inside edges as being in a separate namespace so they can share color values.")
parser.add_argument('--rotate', type=int, default=2, help="Rotation to apply when calculating which edge comes first")
parser.add_argument('--placed', type=str, default=json.dumps(placedDefault), help="Any placed hint tiles")

def eprint(*args, **kwargs):
    print(*args, file=sys.stderr, **kwargs)

def orderFor(tiles):
    """
    >>> sorted(orderFor([[0,0,1,2]]))
    [(0, 1), (0, 2)]
    >>> sorted(orderFor([[0,1,2,3],[0,1,4,5]]))
    [(0, 1), (0, 2), (0, 3), (0, 4), (0, 5), (2, 4)]
    >>> sorted(orderFor([[1,1,1,1],[2,2,2,2],[3,3,3,3]]))
    [(1, 2), (1, 3), (2, 3)]
    """
    for tile in tiles:
        for dir in range(1, 4):
            if tile[0] != tile[dir]:
                yield (tile[0], tile[dir])
    for tileX, i in zip(tiles, range(len(tiles))):
        for tileY in tiles[i+1:]:
            for dir in range(4):
                if tileX[dir] != tileY[dir]:
                    yield (tileX[dir], tileY[dir])
                    break

def orderInsidesAfter(tiles, rows, cols):
    numCornerBorder = 2*rows + 2*cols - 4
    for tileC in tiles[0:4]:
        for tileI in tiles[numCornerBorder:]:
            for dirC in [SOUT, WEST]:
                for dirI in range(4):
                    yield (tileC[dirC], tileI[dirI])
    for tileB in tiles[4:numCornerBorder]:
        for tileI in tiles[numCornerBorder:]:
            for dirB in [EAST, WEST]:
                for dirI in range(4):
                    yield (tileB[dirB], tileI[dirI])

def lessThan(order, color):
    for (x,y) in order:
        if y == color:
            yield x

def removed(order, color):
    for (x,y) in order:
        if x != color and y != color:
            yield (x,y)

def mappingFromOrder(order):
    colors = set([p[i] for i in [0, 1] for p in order])
    mapping = {}
    while len(mapping) != len(colors):
        leastColors = []
        for color in colors - set(mapping.keys()):
            if len(list(lessThan(order, color))) == 0:
                leastColors.append(color)
        if len(leastColors) == 0:
            eprint(f"ERROR: no least color found. Mapping so far: {mapping}.")
            sys.exit(1)
        newColor = 0 if len(mapping) == 0 else max(mapping.values()) + 1
        for color in leastColors:
            mapping[color] = newColor
            order = set(removed(order, color))
    return mapping

if __name__ == "__main__":
    args = parser.parse_args()

    transcription = None
    with open(args.transcription) as f:
        transcription = json.loads(f.read())
    tiles = [tileRotate(args.rotate, tile) for tile in transcription['tiles'].values()]
    rows = transcription['rows']
    cols = transcription['cols']

    order = set(orderFor(tiles))
    if not args.namespace:
        order |= set(orderInsidesAfter(tiles, rows, cols))
    mapping = mappingFromOrder(order)

    puzzle = {
        'name': "real",
        'gen': mapping,
        'rows': rows,
        'cols': cols,
        'tiles': [mapTile(mapping, tile) for tile in tiles],
        'placed': [mapPlaced(mapping, placed) for placed in json.loads(args.placed)],
    }
    
    print(json.dumps(puzzle))
