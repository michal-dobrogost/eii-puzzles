#!/usr/bin/env bash
set -ex

eiiPrint='./build/eii-print/eii-print'

# Generate real puzzle from transcription.
./src/tools/eii-color-edges.py -p real/transcription.json | "$eiiPrint" --json > real/eii.json
cat real/eii.json | "$eiiPrint" --sort --json > real/sorted.json

# Generate real puzzle from jwortmann data.
./src/tools/eii-json-from-txt.awk others/jwortmann/e2pieces.txt | "$eiiPrint" --json > others/jwortmann/e2pieces.json
cat others/jwortmann/e2pieces.json | "$eiiPrint" --sort --json > /tmp/e2pieces.json.sorted

# Try to find a mapping to the jwortmann version and apply the mapping.
mapping=$(./src/tools/eii-find-map.py -p real/eii.json -q others/jwortmann/e2pieces.json)
python3 ./src/tools/eii-map.py -p real/eii.json -m "$mapping" | "$eiiPrint" --sort --json > /tmp/eii.json.sorted
cat others/jwortmann/e2pieces.json | "$eiiPrint" --sort --json > /tmp/e2pieces.json.sorted

mapping=$(python3 ./src/tools/eii-find-map.py -p real/sorted.json -q others/jwortmann/e2pieces.json)
python3 ./src/tools/eii-map.py -p real/sorted.json -m "$mapping" | "$eiiPrint" --sort --json > /tmp/sorted.json.sorted

# You should only see a diff in the name, gen and placed fields (not in the tiles!).
diff /tmp/eii.json.sorted /tmp/e2pieces.json.sorted || true
diff /tmp/sorted.json.sorted /tmp/e2pieces.json.sorted || true
