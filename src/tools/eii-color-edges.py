#! /usr/bin/env nix-shell
#! nix-shell -i python -p python3

import argparse
import ast
import json
from eii import *

placedDefault = [
    {'row': 8, 'col': 7, 'tile': ["yell-pblu-star", "purp-yell-arro", "purp-yell-arro", "gree-oran-snow"]},  # tile 139 (1-based)
    {'row': 2, 'col': 2, 'tile': ["dblu-pink-arro", "maro-gree-arro", "gree-oran-snow", "maro-yell-star"]},  # tile 208 (1-based)
    {'row': 2, 'col': 13, 'tile': ["dblu-pink-arro", "oran-purp-star", "dblu-pink-arro", "dblu-pblu-squa"]}, # tile 255 (1-based)
    {'row': 13, 'col': 2, 'tile': ["dblu-pblu-squa", "purp-yell-arro", "pink-yell-towe", "gree-oran-snow"]}, # tile 181 (1-based)
    {'row': 13, 'col': 13, 'tile': ["yell-dblu-towe", "pblu-pink-snow", "pink-yell-towe", "yell-gree-squa"]} # tile 249 (1-based)
]

parser = argparse.ArgumentParser(
                    prog='eiiNumberColors',
                    description='Map colors in eternity ii puzzle json files.')
parser.add_argument('-p', type=str, help="filename of puzzle to map from")
parser.add_argument('--rows', type=int, default=16, help="Number of rows in the puzzle")
parser.add_argument('--cols', type=int, default=16, help="Number of cols in the puzzle")
parser.add_argument('--rotate', type=int, default=2, help="Rotation to apply when calculating which edge comes first")
parser.add_argument('--placed', type=str, default=json.dumps(placedDefault), help="Any placed hint tiles")

def getMapping(tiles):
    result = {}
    for tile in tiles:
        for edge in tile:
            if edge not in result:
                result[edge] = len(result)
    return result

def tileFromTranscription(tTile):
    return tTile.values()[0]

if __name__ == "__main__":
    args = parser.parse_args()

    transcription = None
    with open(args.p) as f:
        transcription = json.loads(f.read())
    tiles = transcription['tiles'].values()

    mapping = getMapping([tileRotate(args.rotate, tile) for tile in tiles])

    puzzle = {
        'name': "real",
        'gen': mapping,
        'rows': args.rows,
        'cols': args.cols,
        'tiles': [mapTile(mapping, tile) for tile in tiles],
        'placed': [mapPlaced(mapping, placed) for placed in json.loads(args.placed)],
    }
    
    print(json.dumps(puzzle))
