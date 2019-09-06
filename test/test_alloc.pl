#!/usr/bin/perl

use warnings;

my $qsize = 4;
my $GPD_ROOT = "/smishra2/projects/gen-pcie";
my $addr, $pchead;
my @qheads;

system("$GPD_ROOT/test/dma_alloc", $qsize, "1");

print "Queue head address:\n";

for (my $i=0; $i<$qsize; $i++) {
    $addr = `$GPD_ROOT/test/dma_alloc $i 0`;
    $qheads[i] = hex($addr);
    printf("0x%X\n", $qheads[i]);
}

# Send qsize in qid arg to get pop counter address
print "Pop counter address:\n";
$addr = `$GPD_ROOT/test/dma_alloc $qsize 0`;
$pchead = hex($addr);
printf("0x%X\n", $pchead);

system("$GPD_ROOT/test/dma_alloc", "0", "1");
