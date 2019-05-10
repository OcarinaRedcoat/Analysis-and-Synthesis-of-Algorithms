#!/bin/bash
# chmod +x test.sh
# cd student-tests
# ./test.sh ../program.out

cd ./student-tests
for in in `ls | grep T`
do  
    echo "Running $in"
    name=`basename ${in}`
    in="$name/input"
    out="$name/output"
    myout="$name/my.out"
    mydiff="$name/diff"
    program="../p2"
    $program < $in > $myout
    diff $out $myout > $mydiff
    if [[ -s ${mydiff} ]]; then
        echo "FAIL 👎 : $in "
    else
        echo "Passed 👍"
    fi
done
