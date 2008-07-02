#!/usr/bin/perl
use strict;
use warnings;
use File::Find;
use Switch;
        
my $ext = shift;

find( \&show_subs, '.' );

sub show_subs 
{
    if (-d) 
    {
        switch ($_)
        {
            case '.svn'   { next; }
            case '_build' { next; }
            case 'tests'  { $File::Find::prune = 1; }
        }
    }
    elsif (-f and /\.$ext$/)
    {
        $File::Find::name =~ s|^\./||g;
        print "$File::Find::name\n";
    }
}