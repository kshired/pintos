# -*- perl -*-
use strict;
use warnings;
use tests::tests;
use tests::cksum;
use tests::lib;

my ($init, @shuffle);
if (1) {
    # Use precalculated values.
    $init = 3115322833;
    @shuffle = (1691062564, 1973575879, 1647619479, 96566261, 3885786467,
		3022003332, 3614934266, 2704001777, 735775156, 1864109763);
} else {
    # Recalculate values.
    my ($buf) = "";
    for my $i (0...128 * 1024 - 1) {
	$buf .= chr (($i * 257) & 0xff);
    }
    $init = cksum ($buf);

    random_init (0);
    for my $i (1...10) {
	$buf = shuffle ($buf, length ($buf), 1);
	push (@shuffle, cksum ($buf));
    }
}

check_expected (IGNORE_EXIT_CODES => 1, [<<EOF]);
(page-shuffle) begin
(page-shuffle) init: cksum=$init
(page-shuffle) shuffle 0: cksum=$shuffle[0]
(page-shuffle) shuffle 1: cksum=$shuffle[1]
(page-shuffle) shuffle 2: cksum=$shuffle[2]
(page-shuffle) shuffle 3: cksum=$shuffle[3]
(page-shuffle) shuffle 4: cksum=$shuffle[4]
(page-shuffle) shuffle 5: cksum=$shuffle[5]
(page-shuffle) shuffle 6: cksum=$shuffle[6]
(page-shuffle) shuffle 7: cksum=$shuffle[7]
(page-shuffle) shuffle 8: cksum=$shuffle[8]
(page-shuffle) shuffle 9: cksum=$shuffle[9]
(page-shuffle) end
EOF
pass;
