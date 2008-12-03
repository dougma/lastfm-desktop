#!/usr/bin/perl
use strict;
use warnings;
use File::Find;
use Switch;
        
my $ext = shift;

find( \&find_sources, shift || '.' );

my $exclude;
switch ($^O)
{
    case "cygwin"  { $exclude = "mac"; }
    case "MSWin32" { $exclude = "mac"; } # active perl
    case "darwin"  { $exclude = "win"; }
    else           { $exclude = "win"; }
}

sub find_sources 
{
    if (-d) 
    {
        switch ($_)
        {
            case '.svn'   { $File::Find::prune = 1; }
            case '_build' { $File::Find::prune = 1; }
            case 'tests'  { $File::Find::prune = 1; }
            case ($exclude) { $File::Find::prune = 1; }
        }
    }
    elsif (-f and /\.$ext$/)
    {
        $File::Find::name =~ s|^\./||g;
        print "$File::Find::name\n";
    }
}