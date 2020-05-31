#!/bin/bash

echo "" > output_log.txt

cd ../../
bin/lenet5-conv-acc.exe
cd -

echo ""
echo "shell: diff command log:"
diff output_log.txt gold_reference.txt -s
