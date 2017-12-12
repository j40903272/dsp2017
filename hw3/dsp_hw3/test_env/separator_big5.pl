#!/usr/bin/perl

use strict;
use warnings;
use utf8;
use open ':encoding(big5)';
binmode(STDIN,':encoding(big5)');
binmode(STDOUT,':encoding(big5)');

while(<>){
  for( my $i = 0 ; $i < length( $_ ) ; $i++ ){
    print " ".substr( $_, $i , 1 );
  }
}
