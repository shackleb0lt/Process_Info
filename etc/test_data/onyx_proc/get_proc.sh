#!/bin/bash
save_to=`pwd`

pushd /proc
for d in [0-9]*; do
    mkdir -p $save_to/$d
    cp "$d/stat" "$save_to/$d/stat"
done
popd
