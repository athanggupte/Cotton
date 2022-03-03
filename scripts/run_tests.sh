#!/bin/bash

set -o pipefail
files=$(find bin/ -type f -path "*-linux-*/tests/*/*" -print)

ret=0
for file in $files; do
    echo "Running $file..."
    $file || ret=1;
done

exit $ret