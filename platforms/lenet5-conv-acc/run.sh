#!/bin/bash

cd ../../
bin/lenet5-conv-acc.exe
cd -

echo ""
echo "shell: diff command log:"
diff output_log.txt gold_reference.txt -s
