#!/bin/bash
make clean
make
cd build
clear

# testfunc(){
pintos-mkdisk tmp.dsk --filesys-size=2
pintos -v -k -T 60 --qemu  --disk=tmp.dsk -p tests/filesys/extended/dir-empty-name -a dir-empty-name -p tests/filesys/extended/tar -a tar -- -q  -f run dir-empty-name < /dev/null 2> tests/filesys/extended/dir-empty-name.errors > tests/filesys/extended/dir-empty-name.output
pintos -v -k -T 60  --qemu --disk=tmp.dsk -g fs.tar -a tests/filesys/extended/dir-empty-name.tar -- -q  run 'tar fs.tar /' < /dev/null 2> tests/filesys/extended/dir-empty-name-persistence.errors > tests/filesys/extended/dir-empty-name-persistence.output
rm -f tmp.dsk
perl -I../.. ../../tests/filesys/extended/dir-empty-name.ck tests/filesys/extended/dir-empty-name tests/filesys/extended/dir-empty-name.result
rm -f tmp.dsk
pintos-mkdisk tmp.dsk --filesys-size=2
pintos -v -k -T 60 --qemu  --disk=tmp.dsk -p tests/filesys/extended/dir-mk-tree -a dir-mk-tree -p tests/filesys/extended/tar -a tar -- -q  -f run dir-mk-tree < /dev/null 2> tests/filesys/extended/dir-mk-tree.errors > tests/filesys/extended/dir-mk-tree.output
pintos -v -k -T 60  --qemu --disk=tmp.dsk -g fs.tar -a tests/filesys/extended/dir-mk-tree.tar -- -q  run 'tar fs.tar /' < /dev/null 2> tests/filesys/extended/dir-mk-tree-persistence.errors > tests/filesys/extended/dir-mk-tree-persistence.output
rm -f tmp.dsk
perl -I../.. ../../tests/filesys/extended/dir-mk-tree.ck tests/filesys/extended/dir-mk-tree tests/filesys/extended/dir-mk-tree.result
rm -f tmp.dsk
pintos-mkdisk tmp.dsk --filesys-size=2
pintos -v -k -T 60 --qemu  --disk=tmp.dsk -p tests/filesys/extended/dir-mkdir -a dir-mkdir -p tests/filesys/extended/tar -a tar -- -q  -f run dir-mkdir < /dev/null 2> tests/filesys/extended/dir-mkdir.errors > tests/filesys/extended/dir-mkdir.output
pintos -v -k -T 60  --qemu --disk=tmp.dsk -g fs.tar -a tests/filesys/extended/dir-mkdir.tar -- -q  run 'tar fs.tar /' < /dev/null 2> tests/filesys/extended/dir-mkdir-persistence.errors > tests/filesys/extended/dir-mkdir-persistence.output
rm -f tmp.dsk
perl -I../.. ../../tests/filesys/extended/dir-mkdir.ck tests/filesys/extended/dir-mkdir tests/filesys/extended/dir-mkdir.result
rm -f tmp.dsk
pintos-mkdisk tmp.dsk --filesys-size=2
pintos -v -k -T 60 --qemu  --disk=tmp.dsk -p tests/filesys/extended/dir-open -a dir-open -p tests/filesys/extended/tar -a tar -- -q  -f run dir-open < /dev/null 2> tests/filesys/extended/dir-open.errors > tests/filesys/extended/dir-open.output
pintos -v -k -T 60  --qemu --disk=tmp.dsk -g fs.tar -a tests/filesys/extended/dir-open.tar -- -q  run 'tar fs.tar /' < /dev/null 2> tests/filesys/extended/dir-open-persistence.errors > tests/filesys/extended/dir-open-persistence.output
rm -f tmp.dsk
perl -I../.. ../../tests/filesys/extended/dir-open.ck tests/filesys/extended/dir-open tests/filesys/extended/dir-open.result
rm -f tmp.dsk
pintos-mkdisk tmp.dsk --filesys-size=2
pintos -v -k -T 60 --qemu  --disk=tmp.dsk -p tests/filesys/extended/dir-over-file -a dir-over-file -p tests/filesys/extended/tar -a tar -- -q  -f run dir-over-file < /dev/null 2> tests/filesys/extended/dir-over-file.errors > tests/filesys/extended/dir-over-file.output
pintos -v -k -T 60  --qemu --disk=tmp.dsk -g fs.tar -a tests/filesys/extended/dir-over-file.tar -- -q  run 'tar fs.tar /' < /dev/null 2> tests/filesys/extended/dir-over-file-persistence.errors > tests/filesys/extended/dir-over-file-persistence.output
rm -f tmp.dsk
perl -I../.. ../../tests/filesys/extended/dir-over-file.ck tests/filesys/extended/dir-over-file tests/filesys/extended/dir-over-file.result
rm -f tmp.dsk
pintos-mkdisk tmp.dsk --filesys-size=2
pintos -v -k -T 60 --qemu  --disk=tmp.dsk -p tests/filesys/extended/dir-rm-cwd -a dir-rm-cwd -p tests/filesys/extended/tar -a tar -- -q  -f run dir-rm-cwd < /dev/null 2> tests/filesys/extended/dir-rm-cwd.errors > tests/filesys/extended/dir-rm-cwd.output
pintos -v -k -T 60  --qemu --disk=tmp.dsk -g fs.tar -a tests/filesys/extended/dir-rm-cwd.tar -- -q  run 'tar fs.tar /' < /dev/null 2> tests/filesys/extended/dir-rm-cwd-persistence.errors > tests/filesys/extended/dir-rm-cwd-persistence.output
rm -f tmp.dsk
perl -I../.. ../../tests/filesys/extended/dir-rm-cwd.ck tests/filesys/extended/dir-rm-cwd tests/filesys/extended/dir-rm-cwd.result
rm -f tmp.dsk
pintos-mkdisk tmp.dsk --filesys-size=2
pintos -v -k -T 60 --qemu  --disk=tmp.dsk -p tests/filesys/extended/dir-rm-parent -a dir-rm-parent -p tests/filesys/extended/tar -a tar -- -q  -f run dir-rm-parent < /dev/null 2> tests/filesys/extended/dir-rm-parent.errors > tests/filesys/extended/dir-rm-parent.output
pintos -v -k -T 60  --qemu --disk=tmp.dsk -g fs.tar -a tests/filesys/extended/dir-rm-parent.tar -- -q  run 'tar fs.tar /' < /dev/null 2> tests/filesys/extended/dir-rm-parent-persistence.errors > tests/filesys/extended/dir-rm-parent-persistence.output
rm -f tmp.dsk
perl -I../.. ../../tests/filesys/extended/dir-rm-parent.ck tests/filesys/extended/dir-rm-parent tests/filesys/extended/dir-rm-parent.result
rm -f tmp.dsk
pintos-mkdisk tmp.dsk --filesys-size=2
pintos -v -k -T 60 --qemu  --disk=tmp.dsk -p tests/filesys/extended/dir-rm-root -a dir-rm-root -p tests/filesys/extended/tar -a tar -- -q  -f run dir-rm-root < /dev/null 2> tests/filesys/extended/dir-rm-root.errors > tests/filesys/extended/dir-rm-root.output
pintos -v -k -T 60  --qemu --disk=tmp.dsk -g fs.tar -a tests/filesys/extended/dir-rm-root.tar -- -q  run 'tar fs.tar /' < /dev/null 2> tests/filesys/extended/dir-rm-root-persistence.errors > tests/filesys/extended/dir-rm-root-persistence.output
rm -f tmp.dsk
perl -I../.. ../../tests/filesys/extended/dir-rm-root.ck tests/filesys/extended/dir-rm-root tests/filesys/extended/dir-rm-root.result
rm -f tmp.dsk
pintos-mkdisk tmp.dsk --filesys-size=2
pintos -v -k -T 60 --qemu  --disk=tmp.dsk -p tests/filesys/extended/dir-rm-tree -a dir-rm-tree -p tests/filesys/extended/tar -a tar -- -q  -f run dir-rm-tree < /dev/null 2> tests/filesys/extended/dir-rm-tree.errors > tests/filesys/extended/dir-rm-tree.output
pintos -v -k -T 60  --qemu --disk=tmp.dsk -g fs.tar -a tests/filesys/extended/dir-rm-tree.tar -- -q  run 'tar fs.tar /' < /dev/null 2> tests/filesys/extended/dir-rm-tree-persistence.errors > tests/filesys/extended/dir-rm-tree-persistence.output
rm -f tmp.dsk
perl -I../.. ../../tests/filesys/extended/dir-rm-tree.ck tests/filesys/extended/dir-rm-tree tests/filesys/extended/dir-rm-tree.result
rm -f tmp.dsk
pintos-mkdisk tmp.dsk --filesys-size=2
pintos -v -k -T 60 --qemu  --disk=tmp.dsk -p tests/filesys/extended/dir-rmdir -a dir-rmdir -p tests/filesys/extended/tar -a tar -- -q  -f run dir-rmdir < /dev/null 2> tests/filesys/extended/dir-rmdir.errors > tests/filesys/extended/dir-rmdir.output
pintos -v -k -T 60  --qemu --disk=tmp.dsk -g fs.tar -a tests/filesys/extended/dir-rmdir.tar -- -q  run 'tar fs.tar /' < /dev/null 2> tests/filesys/extended/dir-rmdir-persistence.errors > tests/filesys/extended/dir-rmdir-persistence.output
rm -f tmp.dsk
perl -I../.. ../../tests/filesys/extended/dir-rmdir.ck tests/filesys/extended/dir-rmdir tests/filesys/extended/dir-rmdir.result
rm -f tmp.dsk
pintos-mkdisk tmp.dsk --filesys-size=2
pintos -v -k -T 60 --qemu  --disk=tmp.dsk -p tests/filesys/extended/dir-under-file -a dir-under-file -p tests/filesys/extended/tar -a tar -- -q  -f run dir-under-file < /dev/null 2> tests/filesys/extended/dir-under-file.errors > tests/filesys/extended/dir-under-file.output
pintos -v -k -T 60  --qemu --disk=tmp.dsk -g fs.tar -a tests/filesys/extended/dir-under-file.tar -- -q  run 'tar fs.tar /' < /dev/null 2> tests/filesys/extended/dir-under-file-persistence.errors > tests/filesys/extended/dir-under-file-persistence.output
rm -f tmp.dsk
perl -I../.. ../../tests/filesys/extended/dir-under-file.ck tests/filesys/extended/dir-under-file tests/filesys/extended/dir-under-file.result
rm -f tmp.dsk
pintos-mkdisk tmp.dsk --filesys-size=2
pintos -v -k -T 150 --qemu  --disk=tmp.dsk -p tests/filesys/extended/dir-vine -a dir-vine -p tests/filesys/extended/tar -a tar -- -q  -f run dir-vine < /dev/null 2> tests/filesys/extended/dir-vine.errors > tests/filesys/extended/dir-vine.output
pintos -v -k -T 60  --qemu --disk=tmp.dsk -g fs.tar -a tests/filesys/extended/dir-vine.tar -- -q  run 'tar fs.tar /' < /dev/null 2> tests/filesys/extended/dir-vine-persistence.errors > tests/filesys/extended/dir-vine-persistence.output
rm -f tmp.dsk
perl -I../.. ../../tests/filesys/extended/dir-vine.ck tests/filesys/extended/dir-vine tests/filesys/extended/dir-vine.result
rm -f tmp.dsk
pintos-mkdisk tmp.dsk --filesys-size=2
pintos -v -k -T 60 --qemu  --disk=tmp.dsk -p tests/filesys/extended/grow-create -a grow-create -p tests/filesys/extended/tar -a tar -- -q  -f run grow-create < /dev/null 2> tests/filesys/extended/grow-create.errors > tests/filesys/extended/grow-create.output
pintos -v -k -T 60  --qemu --disk=tmp.dsk -g fs.tar -a tests/filesys/extended/grow-create.tar -- -q  run 'tar fs.tar /' < /dev/null 2> tests/filesys/extended/grow-create-persistence.errors > tests/filesys/extended/grow-create-persistence.output
rm -f tmp.dsk
perl -I../.. ../../tests/filesys/extended/grow-create.ck tests/filesys/extended/grow-create tests/filesys/extended/grow-create.result
rm -f tmp.dsk
pintos-mkdisk tmp.dsk --filesys-size=2
pintos -v -k -T 60 --qemu  --disk=tmp.dsk -p tests/filesys/extended/grow-dir-lg -a grow-dir-lg -p tests/filesys/extended/tar -a tar -- -q  -f run grow-dir-lg < /dev/null 2> tests/filesys/extended/grow-dir-lg.errors > tests/filesys/extended/grow-dir-lg.output
pintos -v -k -T 60  --qemu --disk=tmp.dsk -g fs.tar -a tests/filesys/extended/grow-dir-lg.tar -- -q  run 'tar fs.tar /' < /dev/null 2> tests/filesys/extended/grow-dir-lg-persistence.errors > tests/filesys/extended/grow-dir-lg-persistence.output
rm -f tmp.dsk
perl -I../.. ../../tests/filesys/extended/grow-dir-lg.ck tests/filesys/extended/grow-dir-lg tests/filesys/extended/grow-dir-lg.result
rm -f tmp.dsk
pintos-mkdisk tmp.dsk --filesys-size=2
pintos -v -k -T 60 --qemu  --disk=tmp.dsk -p tests/filesys/extended/grow-file-size -a grow-file-size -p tests/filesys/extended/tar -a tar -- -q  -f run grow-file-size < /dev/null 2> tests/filesys/extended/grow-file-size.errors > tests/filesys/extended/grow-file-size.output
pintos -v -k -T 60  --qemu --disk=tmp.dsk -g fs.tar -a tests/filesys/extended/grow-file-size.tar -- -q  run 'tar fs.tar /' < /dev/null 2> tests/filesys/extended/grow-file-size-persistence.errors > tests/filesys/extended/grow-file-size-persistence.output
rm -f tmp.dsk
perl -I../.. ../../tests/filesys/extended/grow-file-size.ck tests/filesys/extended/grow-file-size tests/filesys/extended/grow-file-size.result
rm -f tmp.dsk
pintos-mkdisk tmp.dsk --filesys-size=2
pintos -v -k -T 60 --qemu  --disk=tmp.dsk -p tests/filesys/extended/grow-root-lg -a grow-root-lg -p tests/filesys/extended/tar -a tar -- -q  -f run grow-root-lg < /dev/null 2> tests/filesys/extended/grow-root-lg.errors > tests/filesys/extended/grow-root-lg.output
pintos -v -k -T 60  --qemu --disk=tmp.dsk -g fs.tar -a tests/filesys/extended/grow-root-lg.tar -- -q  run 'tar fs.tar /' < /dev/null 2> tests/filesys/extended/grow-root-lg-persistence.errors > tests/filesys/extended/grow-root-lg-persistence.output
rm -f tmp.dsk
perl -I../.. ../../tests/filesys/extended/grow-root-lg.ck tests/filesys/extended/grow-root-lg tests/filesys/extended/grow-root-lg.result
rm -f tmp.dsk
pintos-mkdisk tmp.dsk --filesys-size=2
pintos -v -k -T 60 --qemu  --disk=tmp.dsk -p tests/filesys/extended/grow-root-sm -a grow-root-sm -p tests/filesys/extended/tar -a tar -- -q  -f run grow-root-sm < /dev/null 2> tests/filesys/extended/grow-root-sm.errors > tests/filesys/extended/grow-root-sm.output
pintos -v -k -T 60  --qemu --disk=tmp.dsk -g fs.tar -a tests/filesys/extended/grow-root-sm.tar -- -q  run 'tar fs.tar /' < /dev/null 2> tests/filesys/extended/grow-root-sm-persistence.errors > tests/filesys/extended/grow-root-sm-persistence.output
rm -f tmp.dsk
perl -I../.. ../../tests/filesys/extended/grow-root-sm.ck tests/filesys/extended/grow-root-sm tests/filesys/extended/grow-root-sm.result
rm -f tmp.dsk
pintos-mkdisk tmp.dsk --filesys-size=2
pintos -v -k -T 60 --qemu  --disk=tmp.dsk -p tests/filesys/extended/grow-seq-lg -a grow-seq-lg -p tests/filesys/extended/tar -a tar -- -q  -f run grow-seq-lg < /dev/null 2> tests/filesys/extended/grow-seq-lg.errors > tests/filesys/extended/grow-seq-lg.output
pintos -v -k -T 60  --qemu --disk=tmp.dsk -g fs.tar -a tests/filesys/extended/grow-seq-lg.tar -- -q  run 'tar fs.tar /' < /dev/null 2> tests/filesys/extended/grow-seq-lg-persistence.errors > tests/filesys/extended/grow-seq-lg-persistence.output
rm -f tmp.dsk
perl -I../.. ../../tests/filesys/extended/grow-seq-lg.ck tests/filesys/extended/grow-seq-lg tests/filesys/extended/grow-seq-lg.result
rm -f tmp.dsk
pintos-mkdisk tmp.dsk --filesys-size=2
pintos -v -k -T 60 --qemu  --disk=tmp.dsk -p tests/filesys/extended/grow-seq-sm -a grow-seq-sm -p tests/filesys/extended/tar -a tar -- -q  -f run grow-seq-sm < /dev/null 2> tests/filesys/extended/grow-seq-sm.errors > tests/filesys/extended/grow-seq-sm.output
pintos -v -k -T 60  --qemu --disk=tmp.dsk -g fs.tar -a tests/filesys/extended/grow-seq-sm.tar -- -q  run 'tar fs.tar /' < /dev/null 2> tests/filesys/extended/grow-seq-sm-persistence.errors > tests/filesys/extended/grow-seq-sm-persistence.output
rm -f tmp.dsk
perl -I../.. ../../tests/filesys/extended/grow-seq-sm.ck tests/filesys/extended/grow-seq-sm tests/filesys/extended/grow-seq-sm.result
rm -f tmp.dsk
pintos-mkdisk tmp.dsk --filesys-size=2
pintos -v -k -T 60 --qemu  --disk=tmp.dsk -p tests/filesys/extended/grow-sparse -a grow-sparse -p tests/filesys/extended/tar -a tar -- -q  -f run grow-sparse < /dev/null 2> tests/filesys/extended/grow-sparse.errors > tests/filesys/extended/grow-sparse.output
pintos -v -k -T 60  --qemu --disk=tmp.dsk -g fs.tar -a tests/filesys/extended/grow-sparse.tar -- -q  run 'tar fs.tar /' < /dev/null 2> tests/filesys/extended/grow-sparse-persistence.errors > tests/filesys/extended/grow-sparse-persistence.output
rm -f tmp.dsk
perl -I../.. ../../tests/filesys/extended/grow-sparse.ck tests/filesys/extended/grow-sparse tests/filesys/extended/grow-sparse.result
rm -f tmp.dsk
pintos-mkdisk tmp.dsk --filesys-size=2
pintos -v -k -T 60 --qemu  --disk=tmp.dsk -p tests/filesys/extended/grow-tell -a grow-tell -p tests/filesys/extended/tar -a tar -- -q  -f run grow-tell < /dev/null 2> tests/filesys/extended/grow-tell.errors > tests/filesys/extended/grow-tell.output
pintos -v -k -T 60  --qemu --disk=tmp.dsk -g fs.tar -a tests/filesys/extended/grow-tell.tar -- -q  run 'tar fs.tar /' < /dev/null 2> tests/filesys/extended/grow-tell-persistence.errors > tests/filesys/extended/grow-tell-persistence.output
rm -f tmp.dsk
perl -I../.. ../../tests/filesys/extended/grow-tell.ck tests/filesys/extended/grow-tell tests/filesys/extended/grow-tell.result
rm -f tmp.dsk
pintos-mkdisk tmp.dsk --filesys-size=2
pintos -v -k -T 60 --qemu  --disk=tmp.dsk -p tests/filesys/extended/grow-two-files -a grow-two-files -p tests/filesys/extended/tar -a tar -- -q  -f run grow-two-files < /dev/null 2> tests/filesys/extended/grow-two-files.errors > tests/filesys/extended/grow-two-files.output
pintos -v -k -T 60  --qemu --disk=tmp.dsk -g fs.tar -a tests/filesys/extended/grow-two-files.tar -- -q  run 'tar fs.tar /' < /dev/null 2> tests/filesys/extended/grow-two-files-persistence.errors > tests/filesys/extended/grow-two-files-persistence.output
rm -f tmp.dsk
perl -I../.. ../../tests/filesys/extended/grow-two-files.ck tests/filesys/extended/grow-two-files tests/filesys/extended/grow-two-files.result
rm -f tmp.dsk
pintos-mkdisk tmp.dsk --filesys-size=2
pintos -v -k -T 60 --qemu  --disk=tmp.dsk -p tests/filesys/extended/syn-rw -a syn-rw -p tests/filesys/extended/tar -a tar -p tests/filesys/extended/child-syn-rw -a child-syn-rw -- -q  -f run syn-rw < /dev/null 2> tests/filesys/extended/syn-rw.errors > tests/filesys/extended/syn-rw.output
pintos -v -k -T 60  --qemu --disk=tmp.dsk -g fs.tar -a tests/filesys/extended/syn-rw.tar -- -q  run 'tar fs.tar /' < /dev/null 2> tests/filesys/extended/syn-rw-persistence.errors > tests/filesys/extended/syn-rw-persistence.output
rm -f tmp.dsk
perl -I../.. ../../tests/filesys/extended/syn-rw.ck tests/filesys/extended/syn-rw tests/filesys/extended/syn-rw.result
perl -I../.. ../../tests/filesys/extended/dir-empty-name-persistence.ck tests/filesys/extended/dir-empty-name-persistence tests/filesys/extended/dir-empty-name-persistence.result
perl -I../.. ../../tests/filesys/extended/dir-mk-tree-persistence.ck tests/filesys/extended/dir-mk-tree-persistence tests/filesys/extended/dir-mk-tree-persistence.result
perl -I../.. ../../tests/filesys/extended/dir-mkdir-persistence.ck tests/filesys/extended/dir-mkdir-persistence tests/filesys/extended/dir-mkdir-persistence.result
perl -I../.. ../../tests/filesys/extended/dir-open-persistence.ck tests/filesys/extended/dir-open-persistence tests/filesys/extended/dir-open-persistence.result
perl -I../.. ../../tests/filesys/extended/dir-over-file-persistence.ck tests/filesys/extended/dir-over-file-persistence tests/filesys/extended/dir-over-file-persistence.result
perl -I../.. ../../tests/filesys/extended/dir-rm-cwd-persistence.ck tests/filesys/extended/dir-rm-cwd-persistence tests/filesys/extended/dir-rm-cwd-persistence.result
perl -I../.. ../../tests/filesys/extended/dir-rm-parent-persistence.ck tests/filesys/extended/dir-rm-parent-persistence tests/filesys/extended/dir-rm-parent-persistence.result
perl -I../.. ../../tests/filesys/extended/dir-rm-root-persistence.ck tests/filesys/extended/dir-rm-root-persistence tests/filesys/extended/dir-rm-root-persistence.result
perl -I../.. ../../tests/filesys/extended/dir-rm-tree-persistence.ck tests/filesys/extended/dir-rm-tree-persistence tests/filesys/extended/dir-rm-tree-persistence.result
perl -I../.. ../../tests/filesys/extended/dir-rmdir-persistence.ck tests/filesys/extended/dir-rmdir-persistence tests/filesys/extended/dir-rmdir-persistence.result
perl -I../.. ../../tests/filesys/extended/dir-under-file-persistence.ck tests/filesys/extended/dir-under-file-persistence tests/filesys/extended/dir-under-file-persistence.result
perl -I../.. ../../tests/filesys/extended/dir-vine-persistence.ck tests/filesys/extended/dir-vine-persistence tests/filesys/extended/dir-vine-persistence.result
perl -I../.. ../../tests/filesys/extended/grow-create-persistence.ck tests/filesys/extended/grow-create-persistence tests/filesys/extended/grow-create-persistence.result
perl -I../.. ../../tests/filesys/extended/grow-dir-lg-persistence.ck tests/filesys/extended/grow-dir-lg-persistence tests/filesys/extended/grow-dir-lg-persistence.result
perl -I../.. ../../tests/filesys/extended/grow-file-size-persistence.ck tests/filesys/extended/grow-file-size-persistence tests/filesys/extended/grow-file-size-persistence.result
perl -I../.. ../../tests/filesys/extended/grow-root-lg-persistence.ck tests/filesys/extended/grow-root-lg-persistence tests/filesys/extended/grow-root-lg-persistence.result
perl -I../.. ../../tests/filesys/extended/grow-root-sm-persistence.ck tests/filesys/extended/grow-root-sm-persistence tests/filesys/extended/grow-root-sm-persistence.result
perl -I../.. ../../tests/filesys/extended/grow-seq-lg-persistence.ck tests/filesys/extended/grow-seq-lg-persistence tests/filesys/extended/grow-seq-lg-persistence.result
perl -I../.. ../../tests/filesys/extended/grow-seq-sm-persistence.ck tests/filesys/extended/grow-seq-sm-persistence tests/filesys/extended/grow-seq-sm-persistence.result
perl -I../.. ../../tests/filesys/extended/grow-sparse-persistence.ck tests/filesys/extended/grow-sparse-persistence tests/filesys/extended/grow-sparse-persistence.result
perl -I../.. ../../tests/filesys/extended/grow-tell-persistence.ck tests/filesys/extended/grow-tell-persistence tests/filesys/extended/grow-tell-persistence.result
perl -I../.. ../../tests/filesys/extended/grow-two-files-persistence.ck tests/filesys/extended/grow-two-files-persistence tests/filesys/extended/grow-two-files-persistence.result
perl -I../.. ../../tests/filesys/extended/syn-rw-persistence.ck tests/filesys/extended/syn-rw-persistence tests/filesys/extended/syn-rw-persistence.result
# } 

# echo "test start"
# testfunc > test.result
# echo "test done"