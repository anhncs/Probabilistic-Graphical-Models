#! /usr/local/bin/perl
#
# This file is part of libDAI - http://www.libdai.org/
#
# Copyright (c) 2006-2011, The libDAI authors. All rights reserved.
#
# Use of this source code is governed by a BSD-style license that can be found in the LICENSE file.


use strict;
use warnings;

my $HOME = $ENV{HOME} ;

if ($#ARGV == 0) {
  push (@ARGV,"$ARGV[0].fg") ;
}

my $expectedArgc = 2 ;
if ($#ARGV != $expectedArgc-1) {
  die ("Usage: $0 <inModelFile> <outModelFile>\n") ;
}

my ($inModelFile,$outModelFile) = @ARGV ;

# First we read the model:
my (@variables,@cliques,@measures,%cliq2meas,$truePartition) ;
&readModel ($inModelFile,\@variables,\@cliques,\@measures,\%cliq2meas,\$truePartition) ;


open (OUT, ">$outModelFile") or die ("[ERROR] Cannot open output file $outModelFile\n") ;

print OUT "# Converted $inModelFile from fastInf format into DAI format\n" ;

print OUT scalar(@cliques), "\n\n" ;

for (my $i=0 ; $i<scalar(@cliques) ; $i++) {
  my $cliqLine = $cliques[$i] ;
  my @splitCliq = split (/\t/, $cliqLine) ;
  #print "Num vars = " , $splitCliq[1] , "       vars: " , $splitCliq[2] , "\n" ;
  print OUT $splitCliq[1], "\n" ;
  print OUT $splitCliq[2], "\n" ;

  my $meas = $cliq2meas{$i} ;
  my $measLine = $measures[$meas] ;
  my @splitMeas = split (/\t/, $measLine) ;
  print OUT $splitMeas[2], "\n" ;

  my @params = split (/\s+/, $splitMeas[3]) ;
  print OUT scalar(@params) , "\n" ;
  for (my $j=0 ; $j<scalar(@params) ; $j++) {
    print OUT $j, " ", $params[$j], "\n" ;
  }
  print OUT "\n" ;
}

close OUT ;



#################
## Subroutines ##
#################
sub readModel {
  my ($modelFile,$variables,$cliques,$measures,$cliq2meas,$truePartition) = @_ ;

  open (MOD_IN , "<$modelFile") or die ("[ERROR] Cannot read model from $modelFile\n") ;

  while (defined (my $line=<MOD_IN>)) {

    if ($line =~ m/^\@Variables/) {
      &readLines (*MOD_IN , $variables) ;
    }
    elsif ($line =~ m/^\@Cliques/) {
      &readLines (*MOD_IN , $cliques) ;
    }
    elsif ($line =~ m/^\@Measures/) {
      &readLines (*MOD_IN , $measures) ;
    }
    elsif ($line =~ m/^\@CliqueToMeasure/) {
      &readCliq2Meas (*MOD_IN , $cliq2meas) ;
    }
    elsif ($line =~ m/^# Exact Partitition function:/) {
      my $partLine ;
      if (not defined ($partLine = <MOD_IN>)) { die ("[ERROR] Cannot read exact partition function."); }
      if ($partLine =~ m/^# (\S+)/) {
        $$truePartition = $1 ;
      }
    }
    # else do nothing

  } # while read line from model file

  close MOD_IN ;
}

sub readLines {
  my ($IN,$variables) = @_ ;

  my $endReached = 0 ;
  while (defined (my $line = <$IN>)) {
    if ($line =~ m/^\@End/) {
      $endReached = 1 ;
      last ;
    }
    else {
      push (@$variables,$line) ;
    }
  }

  if (not $endReached) { die ("[ERROR] Could not find \@End statement.\n") ; }
}

sub readCliq2Meas {
  my ($IN,$cliq2meas) = @_ ;

  my $endReached = 0 ;
  while (defined (my $line = <$IN>)) {
    if ($line =~ m/^\@End/) {
      $endReached = 1 ;
      last ;
    }
    else {
      my @splitLine = split(/\t/,$line) ;
      my $cliq = $splitLine[0] ;
      my $meas = $splitLine[1] ;
      $cliq2meas->{$cliq} = $meas ;
    }
  }

  if (not $endReached) { die ("[ERROR] Could not find \@End statement.\n") ; }
}
