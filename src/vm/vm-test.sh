#!/bin/bash
cd build
pintos -v -k -T 60 --qemu  --filesys-size=2 -p tests/vm/pt-grow-stack -a pt-grow-stack --swap-size=4 -- -q  -f run pt-grow-stack < /dev/null 2> tests/vm/pt-grow-stack.errors > tests/vm/pt-grow-stack.output
perl -I../.. ../../tests/vm/pt-grow-stack.ck tests/vm/pt-grow-stack tests/vm/pt-grow-stack.result
pintos -v -k -T 60 --qemu  --filesys-size=2 -p tests/vm/pt-grow-pusha -a pt-grow-pusha --swap-size=4 -- -q  -f run pt-grow-pusha < /dev/null 2> tests/vm/pt-grow-pusha.errors > tests/vm/pt-grow-pusha.output
perl -I../.. ../../tests/vm/pt-grow-pusha.ck tests/vm/pt-grow-pusha tests/vm/pt-grow-pusha.result
pintos -v -k -T 60 --qemu  --filesys-size=2 -p tests/vm/pt-grow-bad -a pt-grow-bad --swap-size=4 -- -q  -f run pt-grow-bad < /dev/null 2> tests/vm/pt-grow-bad.errors > tests/vm/pt-grow-bad.output
perl -I../.. ../../tests/vm/pt-grow-bad.ck tests/vm/pt-grow-bad tests/vm/pt-grow-bad.result
pintos -v -k -T 60 --qemu  --filesys-size=2 -p tests/vm/pt-big-stk-obj -a pt-big-stk-obj --swap-size=4 -- -q  -f run pt-big-stk-obj < /dev/null 2> tests/vm/pt-big-stk-obj.errors > tests/vm/pt-big-stk-obj.output
perl -I../.. ../../tests/vm/pt-big-stk-obj.ck tests/vm/pt-big-stk-obj tests/vm/pt-big-stk-obj.result
pintos -v -k -T 60 --qemu  --filesys-size=2 -p tests/vm/pt-bad-addr -a pt-bad-addr --swap-size=4 -- -q  -f run pt-bad-addr < /dev/null 2> tests/vm/pt-bad-addr.errors > tests/vm/pt-bad-addr.output
perl -I../.. ../../tests/vm/pt-bad-addr.ck tests/vm/pt-bad-addr tests/vm/pt-bad-addr.result
pintos -v -k -T 60 --qemu  --filesys-size=2 -p tests/vm/pt-bad-read -a pt-bad-read -p ../../tests/vm/sample.txt -a sample.txt --swap-size=4 -- -q  -f run pt-bad-read < /dev/null 2> tests/vm/pt-bad-read.errors > tests/vm/pt-bad-read.output
perl -I../.. ../../tests/vm/pt-bad-read.ck tests/vm/pt-bad-read tests/vm/pt-bad-read.result
pintos -v -k -T 60 --qemu  --filesys-size=2 -p tests/vm/pt-write-code -a pt-write-code --swap-size=4 -- -q  -f run pt-write-code < /dev/null 2> tests/vm/pt-write-code.errors > tests/vm/pt-write-code.output
perl -I../.. ../../tests/vm/pt-write-code.ck tests/vm/pt-write-code tests/vm/pt-write-code.result
pintos -v -k -T 60 --qemu  --filesys-size=2 -p tests/vm/pt-write-code2 -a pt-write-code2 -p ../../tests/vm/sample.txt -a sample.txt --swap-size=4 -- -q  -f run pt-write-code2 < /dev/null 2> tests/vm/pt-write-code2.errors > tests/vm/pt-write-code2.output
perl -I../.. ../../tests/vm/pt-write-code2.ck tests/vm/pt-write-code2 tests/vm/pt-write-code2.result
pintos -v -k -T 60 --qemu  --filesys-size=2 -p tests/vm/pt-grow-stk-sc -a pt-grow-stk-sc --swap-size=4 -- -q  -f run pt-grow-stk-sc < /dev/null 2> tests/vm/pt-grow-stk-sc.errors > tests/vm/pt-grow-stk-sc.output
perl -I../.. ../../tests/vm/pt-grow-stk-sc.ck tests/vm/pt-grow-stk-sc tests/vm/pt-grow-stk-sc.result
pintos -v -k -T 300 --qemu  --filesys-size=2 -p tests/vm/page-linear -a page-linear --swap-size=4 -- -q  -f run page-linear < /dev/null 2> tests/vm/page-linear.errors > tests/vm/page-linear.output
perl -I../.. ../../tests/vm/page-linear.ck tests/vm/page-linear tests/vm/page-linear.result
pintos -v -k -T 60 --qemu  --filesys-size=2 -p tests/vm/page-parallel -a page-parallel -p tests/vm/child-linear -a child-linear --swap-size=4 -- -q  -f run page-parallel < /dev/null 2> tests/vm/page-parallel.errors > tests/vm/page-parallel.output
perl -I../.. ../../tests/vm/page-parallel.ck tests/vm/page-parallel tests/vm/page-parallel.result
pintos -v -k -T 600 --qemu  --filesys-size=2 -p tests/vm/page-merge-seq -a page-merge-seq -p tests/vm/child-sort -a child-sort --swap-size=4 -- -q  -f run page-merge-seq < /dev/null 2> tests/vm/page-merge-seq.errors > tests/vm/page-merge-seq.output
perl -I../.. ../../tests/vm/page-merge-seq.ck tests/vm/page-merge-seq tests/vm/page-merge-seq.result
pintos -v -k -T 600 --qemu  --filesys-size=2 -p tests/vm/page-merge-par -a page-merge-par -p tests/vm/child-sort -a child-sort --swap-size=4 -- -q  -f run page-merge-par < /dev/null 2> tests/vm/page-merge-par.errors > tests/vm/page-merge-par.output
perl -I../.. ../../tests/vm/page-merge-par.ck tests/vm/page-merge-par tests/vm/page-merge-par.result
pintos -v -k -T 60 --qemu  --filesys-size=2 -p tests/vm/page-merge-stk -a page-merge-stk -p tests/vm/child-qsort -a child-qsort --swap-size=4 -- -q  -f run page-merge-stk < /dev/null 2> tests/vm/page-merge-stk.errors > tests/vm/page-merge-stk.output
perl -I../.. ../../tests/vm/page-merge-stk.ck tests/vm/page-merge-stk tests/vm/page-merge-stk.result
pintos -v -k -T 60 --qemu  --filesys-size=2 -p tests/vm/page-merge-mm -a page-merge-mm -p tests/vm/child-qsort-mm -a child-qsort-mm --swap-size=4 -- -q  -f run page-merge-mm < /dev/null 2> tests/vm/page-merge-mm.errors > tests/vm/page-merge-mm.output
perl -I../.. ../../tests/vm/page-merge-mm.ck tests/vm/page-merge-mm tests/vm/page-merge-mm.result
pintos -v -k -T 600 --qemu  --filesys-size=2 -p tests/vm/page-shuffle -a page-shuffle --swap-size=4 -- -q  -f run page-shuffle < /dev/null 2> tests/vm/page-shuffle.errors > tests/vm/page-shuffle.output
perl -I../.. ../../tests/vm/page-shuffle.ck tests/vm/page-shuffle tests/vm/page-shuffle.result