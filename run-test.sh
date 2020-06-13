#!/bin/sh

data_file="${HOME}/var/downloads/itch/01302020.NASDAQ_ITCH50"

for run in 0 1 2; do
    bin/parser "${data_file}" >> runtimes && bin/parser -v >> runtimes
done
