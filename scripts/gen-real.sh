#!/usr/bin/env bash
set -ex

eiiPrint='./build/eii-print/eii-print'
tmp=$(mktemp)
tmpJwortmannTiles=$(mktemp)

# Generate puzzle from transcription.
./src/tools/eii-color-edges.py -t real/transcription.json | "$eiiPrint" --json > real/eii.json
./src/tools/eii-color-edges.py -t real/transcription.json --namespace | "$eiiPrint" --json > real/namespace-eii.json

# Check that puzzles are sorted.
for puzzle in "real/eii.json" "real/namespace-eii.json"; do
  cat "$puzzle" | "$eiiPrint" --sort --json > "$tmp"
  diff "$puzzle" "$tmp"
done

# Generate puzzle from jwortmann data.
./src/tools/eii-json-from-txt.awk others/jwortmann/e2pieces.txt | "$eiiPrint" --json > others/jwortmann/e2pieces.json
cat others/jwortmann/e2pieces.json | "$eiiPrint" --sort --json > "$tmp"
diff others/jwortmann/e2pieces.json "$tmp" # others/jwortmann/e2pieces.json should be sorted

# Find a mapping to the jwortmann version, apply the mapping and check for no diffs in tiles.
# Can't map real/namespace-eii.json because it would require separate mappings for border vs internal tiles.
cat others/jwortmann/e2pieces.json | jq '.tiles' > "$tmpJwortmannTiles"
for puzzle in "real/eii.json"; do
  mapping=$(./src/tools/eii-find-map.py -p "$puzzle" -q others/jwortmann/e2pieces.json)
  python3 ./src/tools/eii-map.py -p "$puzzle" -m "$mapping" | "$eiiPrint" --sort --json | jq '.tiles' > "$tmp"
  diff "$tmp" "$tmpJwortmannTiles"
done

rm "$tmp"
rm "$tmpJwortmannTiles"
