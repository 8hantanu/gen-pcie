#!/usr/bin/perl

use warnings;

my $qsize = 4;
my $GPD_ROOT = "/smishra2/projects/gen-pcie";
my $addr, $pchead;
my @qheads;

system("$GPD_ROOT/test/dma_alloc", $qsize, "1");

print "Queue head pointers:\n";

for (my $i=0; $i<$qsize; $i++) {
    $addr = `$GPD_ROOT/test/dma_alloc $i 0`;
    $qheads[i] = hex($addr);
    printf("0x%X\n", $qheads[i]);
}

# send qsize in qid arg to get pop counter pointer
print "Pop counter pointer:\n";
$addr = `$GPD_ROOT/test/dma_alloc $qsize 0`;
$pchead = hex($addr);
printf("0x%X\n", $pchead);

system("$GPD_ROOT/test/dma_alloc", "0", "1");
