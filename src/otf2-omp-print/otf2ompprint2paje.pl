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

    # otf2-print is a dependency which comes with scorep
    open(OTF2PRINT,"otf2-print $arg | ") || die "Could not find the application otf2-print: $!\n";

    my $resolution = 1000000;
    my $first_timestamp;

    header($arg);

    # thread zero is created in the beginning
    create ("zero", "6 0.0 zero T 0 zero\n");

    while ($line =  <OTF2PRINT> )
    {
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

	    if ($event =~ /^ENTER/) {
		bufferize("12 $time $thread S \"$region\"\n");
	    }elsif ($event =~ /^LEAVE/) {
		bufferize("14 $time $thread S\n");
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

sub header(){
    my $file = @_[0];
bufferize("#This trace was generated with: otf2ompprint2paje.pl $file
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
%EndEventDef
%EventDef PajePopState 14
%       Time date
%       Container string
%       Type string
%EndEventDef
");

# print type hierarchy

bufferize("0 T 0 T
2 S T S
");
    }
