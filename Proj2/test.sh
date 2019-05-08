#!/bin/bash
# chmod +x test.sh
# cd student-tests
# ./test.sh ../program.out

cd ./tests
for in in `ls *.in`
do
    echo "Running $in"
    name=`basename $in .in`
    out="$name.out"
    myout="$name.my.out"
    mydiff="$name.diff"
    program="../p2"
    $program < $in > $myout
    diff $out $myout > $mydiff
    if [[ -s ${mydiff} ]]; then
        echo "FAIL: $in "
    else
        echo "Passed :) "
    fi
done
