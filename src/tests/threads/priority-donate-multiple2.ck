# -*- perl -*-
use strict;
use warnings;
use tests::tests;
check_expected ([<<'EOF']);
(priority-donate-multiple2) begin
(priority-donate-multiple2) Main thread should have priority 34.  Actual priority: 34.
(priority-donate-multiple2) Main thread should have priority 36.  Actual priority: 36.
(priority-donate-multiple2) Main thread should have priority 36.  Actual priority: 36.
(priority-donate-multiple2) Thread b acquired lock b.
(priority-donate-multiple2) Thread b finished.
(priority-donate-multiple2) Thread a acquired lock a.
(priority-donate-multiple2) Thread a finished.
(priority-donate-multiple2) Thread c finished.
(priority-donate-multiple2) Threads b, a, c should have just finished, in that order.
(priority-donate-multiple2) Main thread should have priority 31.  Actual priority: 31.
(priority-donate-multiple2) end
EOF
pass;
