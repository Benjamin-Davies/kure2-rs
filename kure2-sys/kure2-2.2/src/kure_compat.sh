#!/bin/bash
# usage: script <in_file> <var_name> <out_file>

IN="$1"
NAME="$2"
OUT="$3"

echo "#include <stdio.h>" > $$ # size_t
size=$(wc -c $IN | awk '{print $1}')
echo "const size_t $NAME""_size = $size;" >> $$
echo -n "const char $NAME [$size] = {" >> $$
./bin2hex < "$IN" >> $$
echo "};" >> $$

mv $$ "$OUT"
rm -f $$
