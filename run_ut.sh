#!/bin/bash

cd bin
./ellet &
test_files=`ls | grep test`
for test_file in ${test_files}
do
  ./${test_file}
done

