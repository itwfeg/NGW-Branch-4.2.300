#!/usr/bin/perl
use strict ;
use SoftekBarcode ;

my $file = "../images/sample_sheet.jpg" ;
my $bar = new SoftekBarcode ;
# Set some properties.
$bar->{ReadCode39} = 1 ;
$bar->{ReadCode128} = 0 ;
$bar->{MultipleRead} = 1 ;
$bar->{LicenseKey} = "YOUR LICENSE KEY" ;
my $count = $bar->ScanBarCode($file) ;

if ($count < 0)
{
	print "Error opening $file\n" ;
}
elsif ($count)
{
	printf "Found %d barcodes\n", $count ;
	foreach (1..$count)
	{
		my $barcode = $bar->GetBarString($_) ;
		printf "Barcode %d = %s\n", $_, $barcode ;
	}
}
else
{
	printf "No barcodes found in this image\n" ;
}
