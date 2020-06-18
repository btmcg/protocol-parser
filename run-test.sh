#!/bin/sh

data_file="tmp/itch-data/01302020.NASDAQ_ITCH50"
output="./runtimes-pr"

printf "# %s %s %s\n" "$(uname --nodename)" "$(uname --kernel-release)" "${data_file}" >> "${output}"
bin/parser -v >> "${output}"
for run in 0 1 2; do
    printf "run %s\n" "${run}"
    bin/parser "${data_file}" >> "${output}"
    printf "\n" >> "${output}"
done
