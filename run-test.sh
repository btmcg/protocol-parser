#!/bin/sh

data_file="tmp/itch-data/01302020.NASDAQ_ITCH50"
output="./runtimes"

bin/parser -v >> "${output}"
for run in 0 1 2; do
    bin/parser "${data_file}" >> "${output}"
done
