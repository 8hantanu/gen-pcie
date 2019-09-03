#!/usr/bin/perl

# use strict;
use warnings;

my $i = 0;
my $qsize = 4;
my $GPD_ROOT = "/smishra2/projects/gen-pcie";

my @qheads;
my $addr = system("$GPD_ROOT/test/dma_alloc", $qsize, "1");

for ($i=0; $i<$qsize; $i++) {
    $addr = `$GPD_ROOT/test/dma_alloc $i 0`;
    $qheads[i] = hex($addr);
    printf("0x%X\n", $qheads[i]);
}
