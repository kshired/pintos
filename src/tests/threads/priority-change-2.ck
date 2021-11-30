# -*- perl -*-
use strict;
use warnings;
use tests::tests;
check_expected ([<<'EOF']);
(priority-change-2) begin
(priority-change-2) Creating a low-priority thread 2.
(priority-change-2) Thread 2 should have just highered its priority.
(priority-change-2) Thread 2 now highering priority.
(priority-change-2) Thread 2 exiting.
(priority-change-2) Thread 2 should have just exited.
(priority-change-2) end
EOF
pass;
