#!/usr/bin/perl -w
#
# status.pl: ant-colony status
# Raphael Finkel 3/2020

use strict;

# constants
my $dataFileName = 'mmas.out';
my $statusFileName = 'nohup.out';

sub showSolutions {
    open DATA, "/bin/tac $dataFileName | grep -B 20 -m 1 -P '\t' | /bin/tac |";
    my $header = <DATA>;
    $header =~ /(\d+)\s+(\d+)/;
    my ($violations, $solutions) = ($1, $2);
    my @solutions = ();
    for my $index (1 .. $solutions) {
        my $line = <DATA>;
        chomp $line;
        push @solutions, $line;
    }
    close DATA;
    print "There are $solutions solutions that have $violations violations:\n";
    print join("\n", @solutions) . "\n";
} # showSolutions

sub showTiming {
    my @stats = stat $statusFileName;
    my $lastChange = $stats[9]; # 9 is mtime
    my $timeDiff = time - $lastChange;
    printf "Last update was %d seconds ago: %d days, %d hours.\n",
        $timeDiff, $timeDiff / (3600*24), ($timeDiff % (3600*24)) / 3600;
} # showTiming

sub showCPU {
    my $psOutput = `/bin/ps -A -o pid,user,rss,time,args | /bin/grep PAC`;
    $psOutput =~ /ext-dvu\+\s\d+\s(\d+)-(\d+):(\d+):(\d+)/;
    my ($days, $hours, $minutes, $seconds) = ($1, $2, $3, $4);
    if (defined $days) {
        printf "CPU time used since this instance started: %dd, %dh, %dm, %ds\n",
            $days, $hours, $minutes, $seconds;
    } else {
        print "Not currently running.\n";
    }
} # showCPU

showTiming();
showCPU();
showSolutions();

