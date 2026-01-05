#!/usr/bin/env bash
basedir=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )/.." &> /dev/null && pwd )

generate () {
  local rows=$1
  local cols=$2
  local cb=$3
  local bb=$4
  local bi=$5
  local ii=$6
  local seed=$7
  dirPath="${basedir}/puzzles/${rows}x${cols}/${cb}-${bb}-${bi}-${ii}"
  mkdir -p "${dirPath}"
  printf -v seedStr "%02d" $seed
  filePath="${dirPath}/${seedStr}"
  python3 "${basedir}/src/tools/eii-gen.py" -r $rows -c $cols -cb $cb -bb $bb -bi $bi -ii $ii --seed $seed > "${dirPath}/$seedStr"
  >&2 echo $filePath
}

generate 4 4 4 5 4 4 0
generate 5 5 4 5 5 5 0
generate 8 8 4 4 5 5 0
generate 8 8 4 4 6 6 0
generate 8 8 4 4 7 7 0
generate 8 8 4 4 9 9 0
generate 8 8 4 4 10 10 0

cbs=( 4 )
bbs=( 5 )
sizes=( 6 7 8 9 10 )
for size in "${sizes[@]}"; do
  for cb in "${cbs[@]}"; do
    for bb in "${bbs[@]}"; do
      for seed in $(seq 0 9); do
        ii=$size
        bi=$ii
        generate "$size" "$size" "$cb" "$bb" "$bi" "$ii" "$seed"
      done
    done
  done
done

sizes=( 16 )
iis=( 50 45 40 35 30 27 25 20 )
for size in "${sizes[@]}"; do
  for cb in "${cbs[@]}"; do
    for bb in "${bbs[@]}"; do
      for seed in $(seq 0 9); do
        for ii in "${iis[@]}"; do
          bi=$ii
          generate "$size" "$size" "$cb" "$bb" "$bi" "$ii" "$seed"
        done
      done
    done
  done
done
