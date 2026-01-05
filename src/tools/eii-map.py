#! /usr/bin/env nix-shell
#! nix-shell -i python -p python3

import argparse
import ast
import json
from eii import *

parser = argparse.ArgumentParser(
                    prog='eiiFindMappingBetweenPuzzles',
                    description='Map colors in eternity ii puzzle json files.')
parser.add_argument('-p', type=str, help="filename of puzzle to map from")
parser.add_argument('-m', type=str, help="mapping to use")

if __name__ == "__main__":
    args = parser.parse_args()

    mapping = ast.literal_eval(args.m)
    
    puzzle = None
    with open(args.p) as f:
        puzzle = json.loads(f.read())

    puzzle['tiles'] = [mapTile(mapping, tile) for tile in puzzle['tiles']]
    puzzle['placed'] = [mapPlaced(mapping, placed) for placed in puzzle['placed']]
    
    print(json.dumps(puzzle))
