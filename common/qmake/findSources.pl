#!/usr/bin/perl
use strict;
use warnings;
use File::Find;
use Switch;
        
my $ext = shift;

find( \&show_subs, shift || '.' );

sub show_subs 
{
    if (-d) 
    {
        switch ($_)
        {
            case '.svn'   { $File::Find::prune = 1; }
            case '_build' { $File::Find::prune = 1; }
            case 'tests'  { $File::Find::prune = 1; }
        }
    }
    elsif (-f and /\.$ext$/)
    {
        $File::Find::name =~ s|^\./||g;
        print "$File::Find::name\n";
    }
}