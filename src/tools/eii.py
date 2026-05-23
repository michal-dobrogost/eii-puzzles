NORT = 0
EAST = 1
SOUT = 2
WEST = 3

def tileRotate(rotation, tile):
    """ Rotate a tile counter-clockwise
    >>> tileRotate(0, [1,2,3,4])
    [1, 2, 3, 4]
    >>> tileRotate(1, [1,2,3,4])
    [2, 3, 4, 1]
    >>> tileRotate(2, [1,2,3,4])
    [3, 4, 1, 2]
    >>> tileRotate(3, [1,2,3,4])
    [4, 1, 2, 3]
    """
    return tile[rotation:] + tile[:rotation]

def sortedTile(tile):
    return min([tile[i:] + tile[:i] for i in range(4)])

def sortedTiles(tiles):
    return sorted([sortedTile(t) for t in tiles])

def tilerotFromTile(tile, tiles):
    """
    >>> tilerotFromTile([1,2,3,4], [[0,0,0,0]]) is None
    True
    >>> tilerotFromTile([1,2,3,4], [[0,0,0,0],[1,2,3,4]])
    4
    >>> tilerotFromTile([2,3,4,1], [[0,0,0,0],[1,2,3,4]])
    5
    >>> tilerotFromTile([3,4,1,2], [[0,0,0,0],[1,2,3,4]])
    6
    >>> tilerotFromTile([4,1,2,3], [[0,0,0,0],[1,2,3,4]])
    7
    """
    for iOther in range(len(tiles)):
        for rot in range(4):
            if tile == tileRotate(rot, tiles[iOther]):
                return iOther * 4 + rot
    return None


def isTileSymmetric(tile):
    """
    >>> isTileSymmetric([1,2,3,4])
    False
    >>> isTileSymmetric([1,1,1,1])
    True
    >>> isTileSymmetric([1,2,1,2])
    True
    """
    return len(set([str(tile[i:] + tile[:i]) for i in range(4)])) != 4

def hasSymmetricTiles(tiles):
    return any([isTileSymmetric(tile) for tile in tiles])

def hasDuplicateTiles(tiles):
    """
    >>> hasDuplicateTiles([[1,1,1,1], [2,2,2,2], [3,3,3,3], [4,4,4,4]])
    False
    >>> hasDuplicateTiles([[1,2,3,4], [2,3,4,1], [1,1,1,1], [2,2,2,2]])
    True
    >>> hasDuplicateTiles([[1,2,3,4], [1,1,1,1], [2,3,4,1], [2,2,2,2]])
    True
    """
    return len(set([min([str(tile[i:] + tile[:i]) for i in range(4)]) for tile in tiles])) != len(tiles)

def mapTile(mapping, tile):
    return [mapping[e] for e in tile]

def mapPlaced(mapping, placed):
    return {'row': placed['row'], 'col': placed['col'], 'tile': mapTile(mapping, placed['tile'])}
