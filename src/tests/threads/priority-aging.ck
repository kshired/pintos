# -*- perl -*-
use strict;
use warnings;
use tests::tests;
check_expected ([<<'EOF']);
(priority-aging) begin
(priority-aging) Creating a default-priority thread 2.
(priority-aging) Main thread change its priority and have just lowered thread 2's priority.
(priority-aging) From now, main thread can't get CPU without aging.
(priority-aging) But main thread exiting.
(priority-aging) Success!
(priority-aging) end
EOF
pass;
