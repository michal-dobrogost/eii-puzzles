#! /usr/bin/env nix-shell
#! nix-shell -i bash -p python3
python3 -m doctest -v src/python/*.py
