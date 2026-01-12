#!/usr/bin/env bash
basedir=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )/.." &> /dev/null && pwd )

# Generated with `rdseed -b 16`, see https://github.com/michal-dobrogost/rdseed.
seeds=('17029' '61244' '47712' '46531' '64930' '14901' '40561' '8212' '25261' '57905' '6759' '60623' '32176' '34206' '59694' '58735' '25168' '25475' '16403' '50511' '55350' '51582' '22549' '40084' '17351' '21841' '4235' '1327' '31080' '21166' '62849' '36026' '11849' '849' '38276' '32997' '15090' '24690' '60378' '22025' '42740' '53021' '3443' '58046' '33538' '50750' '60649' '47422' '552' '32609' '22569' '12841' '41447' '37111' '21765' '44216' '29081' '50239' '58016' '35734' '919' '26228' '15353' '3026' '33589' '19893' '35410' '8448' '41589' '31149' '7906' '33973' '64777' '53660' '1096' '8839' '18632' '29019' '45881' '45915' '59513' '24833' '11273' '19493' '62183' '17059' '54549' '61973' '43384' '10956' '44401' '38545' '17321' '2573' '32791' '53943' '146' '49860' '4417' '20173')


generate () {
  if [ ${#seeds[@]} -eq 0 ]; then
    >&2 echo "ERROR: ran out of seeds"
    exit 1
  fi
  local seed="${seeds[0]}"
  seeds=("${seeds[@]:1}")

  local rows=$1
  local cols=$2
  local cb=$3
  local bb=$4
  local bi=$5
  local ii=$6
  dirPath="${basedir}/puzzles/${rows}x${cols}/${cb}-${bb}-${bi}-${ii}"
  filename="eii-${rows}x${cols}-shuffle-${cb}-${bb}-${bi}-${ii}-${seed}.json"
  echo -n "$filename... "
  mkdir -p "${dirPath}"
  filePath="${dirPath}/${filename}"
  python3 "${basedir}/src/tools/eii-gen.py" -r $rows -c $cols -cb $cb -bb $bb -bi $bi -ii $ii --seed "$seed" > "${filePath}"
  echo "done"
}

cbReal=4
bbReal=5
biReal=17
iiReal=17
for size in $(seq 4 16); do
  for biOffset in 0 1; do
    cb=4
    bb=$(( (${bbReal}*${size} + 8) / 16 ))
    if [ "$bb" -lt 2 ]; then bb=2; fi
    bi=$(( ${size} + $biOffset ))
    ii=$bi
    for i in $(seq 3); do
      generate "$size" "$size" "$cb" "$bb" "$bi" "$ii"
    done
  done
done

echo "${#seeds[@]} seeds left"
