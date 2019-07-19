#!/usr/bin/perl

# This file is part of Akypuera

# Akypuera is free software: you can redistribute it and/or modify
# it under the terms of the GNU Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.

# Akypuera is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Public License for more details.

# You should have received a copy of the GNU Public License
# along with Akypuera. If not, see <http://www.gnu.org/licenses/>.

sub main {
    my($arg);

    $arg = shift(@ARGV);
    if (!defined($arg)){
	die "Provide the traces.otf2 file within a scorep directory.";
    }

    my $resolution = 1000000;
    my $first_timestamp;

    my @metricsdesc = get_metrics_description ($arg);
    header($arg, @metricsdesc);

    # thread zero is created in the beginning
    create ("zero", "6 0.0 zero T 0 zero\n");

    my @metrics = ();
    my %jobid = (); # Empty hash table to keep jobids from task_switch

    # otf2-print is a dependency which comes with scorep
    open(OTF2PRINT,"otf2-print $arg | ") || die "Could not find the application otf2-print: $!\n";
    while ($line =  <OTF2PRINT> )
    {
	my($line2) = $line;

	$line2 =~ s/:([1234567890])/_\1/g;
	$line2 =~ tr/://;
	$line2 =~ tr/"/#/;
	$line2 =~ s/#.*#//;
	$line2 =~ s/ +/:/g;
	my($event,$thread,$time,$kind) = split(/:/, $line2);

#	print STDERR $line;
	
        if(($line =~ /^ENTER/) || ($line =~ /^LEAVE/)) {
	    chomp $line;
	    my($region) = $line;
	    $region =~ s/^[^"]*"//;
	    $region =~ s/"[^"]*$//;

	    $line =~ s/:([1234567890])/_\1/g;
	    $line =~ tr/://;
	    $line =~ tr/"/#/;
	    $line =~ s/#.*#//;
	    $line =~ s/ +/:/g;
	    my($event,$thread,$time,$kind) = split(/:/, $line);

	    if (!defined($first_timestamp)){
		$first_timestamp = $time;
	    }
	    $time -= $first_timestamp;
	    $time /= $resolution;
	    if ($thread == "0") {$thread = "zero";}

	    my $aux = "";
	    if ($event =~ /^ENTER/) {
#		print STDERR "ENTER -> GET ".$jobid{$thread}." of ".$thread."\n";
		if ($jobid{$thread} =~ /^$/) {
		    $aux .= "12 $time $thread S \"$region\" -1";
		}else{
		    $aux .= "12 $time $thread S \"$region\" $jobid{$thread}";
		    $jobid{$thread} = -1;
		}
	    }elsif ($event =~ /^LEAVE/) {
		$aux .= "14 $time $thread S";
	    }
	    if (scalar(@metrics)){
		$aux .= " " . join(" ", @metrics);
	    }
	    $aux .= "\n";
	    bufferize($aux);
	    @metrics = ()
	}elsif(($line =~ /^METRIC/)){
	    chomp $line;
	    $line =~ s/.*Values: //;
	    $line =~ s/ +/ /g;
	    $line =~ s/, /,/g;
	    my @counters = split(',', $line);
	    foreach my $counter (@counters) {
		$counter =~ s/\(".*UINT64; //;
		$counter =~ s/\)//;
		push (@metrics, $counter);
	    }
	}elsif(($line =~ /^THREAD_TEAM_BEGIN/)){ # || ($line =~ /^THREAD_TEAM_END/)){ # Destroy container is disabled for now
	    chomp $line;
	    $line =~ s/ +/:/g;
	    my($event,$thread,$time) = split(/:/, $line);
	    if (!defined($first_timestamp)){
		$first_timestamp = $time;
	    }
	    $time -= $first_timestamp;
	    $time /= $resolution;

	    if (!($thread == "0")) {
		create($thread, "6 $time $thread T 0 $thread\n");
	    }
	}elsif(($line =~ /^THREAD_TASK_SWITCH/)) { # || ($line =~ /^THREAD_TASK_CREATE/)) {
	    # This line contains the task_create / task_switch event
	    chomp $line;
	    my($jid) = $line;

	    $line =~ s/:([1234567890])/_\1/g;
	    $line =~ tr/://;
	    $line =~ tr/"/#/;
	    $line =~ s/#.*#//;
	    $line =~ s/ +/:/g;
	    my($event,$thread,$time,$kind) = split(/:/, $line);
	    if ($thread == "0") {$thread = "zero";}
	    
	    $jid =~ s/^.*Generation Number: //;
#	    print STDERR "THREAD_TASK -> SET ".$jid." of ".$thread."\n";
	    $jobid{$thread} = $jid;
	}
    }
    flush_buffer();
    return;
}

main();

my %created_containers;
sub create {
    my $thread = @_[0];
    my $strevent = @_[1];

    if (!exists $created_containers{$thread}){
	bufferize ($strevent);
	$created_containers{$thread} = 1;
    }
}

my $strbuffer;
sub bufferize {
    my $str = @_[0];
    $strbuffer .= $str;
    if (length($strbuffer) > 3*1024){ # Every 3KBytes
	print $strbuffer;
	$strbuffer = "";
    }
}

sub flush_buffer()
{
    print $strbuffer;
    $strbuffer = "";
}

sub header {
    my $filename = shift;
    my @metricsdesc = @_;
    my $metricsdescstr = "";
    if (scalar(@metricsdesc)){
	$metricsdescstr = "\n% ". join(" string\n% ", @metricsdesc). " string";
    }

bufferize("#This trace was generated with: otf2ompprint2paje.pl $filename
#otf2ompprint2paje.pl is available at https://github.com/schnorr/akypuera/
#The script relies on the availability of otf2-print executable (ScoreP)
%EventDef PajeDefineContainerType 0
%       Alias string
%       Type string
%       Name string
%EndEventDef
%EventDef PajeDefineStateType 2
%       Alias string
%       Type string
%       Name string
%EndEventDef
%EventDef PajeCreateContainer 6
%       Time date
%       Alias string
%       Type string
%       Container string
%       Name string
%EndEventDef
%EventDef PajeDestroyContainer 7
%       Time date
%       Type string
%       Name string
%EndEventDef
%EventDef PajePushState 12
%       Time date
%       Container string
%       Type string
%       Value string
%       JobId string $metricsdescstr
%EndEventDef
%EventDef PajePopState 14
%       Time date
%       Container string
%       Type string $metricsdescstr
%EndEventDef
");

# print type hierarchy

bufferize("0 T 0 T
2 S T S
");
    }

sub get_metrics_description {
    my $filename = @_[0];
    open(OTF2FD,"otf2-print $filename | ") || die "Could not find the application otf2-print: $!\n";
    my $linecount = 0;
    my @metricsdesc = ();
    while ($line =  <OTF2FD> ) {
	if(($line =~ /^METRIC/)){
	    chomp $line;
	    $line =~ s/.*Values: //;
	    $line =~ s/ +/ /g;
	    $line =~ s/, /,/g;
	    my @counters = split(',', $line);
	    foreach my $counter (@counters) {
		$counter =~ s/\("(.*)\".*$/\1/;
		push (@metricsdesc, $counter);
	    }
	    last;
	}
	$linecount++;

	if ($linecount == 1000){
	    #if I reached the line 1000 without encountering a METRIC line
	    #consider that there are not metrics in the trace
	    last;
	}
    }
    close (OTF2FD);
    return @metricsdesc;
}
