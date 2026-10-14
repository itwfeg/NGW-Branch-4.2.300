package SoftekBarcode;

use 5.008;
use strict;
use warnings;
use Carp;

require Exporter;
use AutoLoader;

our @ISA = qw(Exporter);

# Items to export into callers namespace by default. Note: do not export
# names by default without a very good reason. Use EXPORT_OK instead.
# Do not simply export all your public functions/methods/constants.

# This allows declaration	use SoftekBarcode ':all';
# If you do not need this, moving things directly into @EXPORT or @EXPORT_OK
# will save memory.
our %EXPORT_TAGS = ( 'all' => [ qw(
	
) ] );

our @EXPORT_OK = ( @{ $EXPORT_TAGS{'all'} } );

our @EXPORT = qw(
	
);

our $VERSION = '7.0.1';

sub AUTOLOAD {
    # This AUTOLOAD is used to 'autoload' constants from the constant()
    # XS function.

    my $constname;
    our $AUTOLOAD;
    ($constname = $AUTOLOAD) =~ s/.*:://;
    croak "&SoftekBarcode::constant not defined" if $constname eq 'constant';
    my ($error, $val) = constant($constname);
    if ($error) { croak $error; }
    {
	no strict 'refs';
	# Fixed between 5.005_53 and 5.005_61
#XXX	if ($] >= 5.00561) {
#XXX	    *$AUTOLOAD = sub () { $val };
#XXX	}
#XXX	else {
	    *$AUTOLOAD = sub { $val };
#XXX	}
    }
    goto &$AUTOLOAD;
}

require XSLoader;
XSLoader::load('SoftekBarcode', $VERSION);

# Preloaded methods go here.

sub new
{
	my $class = shift ;

	my $this = {
		h_bar_codes => 0,
		h_bar_codes_type => 0,
		@_
	} ;

	bless($this, $class);

	$this->{handle} = init() ;

	$this->{prop_map_string} = {
		Pattern => 45, 
		LicenseKey => 999
	} ;

	$this->{prop_map_uint32} = {
		LineJump => 3
	} ;

	$this->{prop_map_uint16} = {
		MinOccurrence => 4,
		MultipleRead => 6,
		PrefOccurrence => 8,
		Despeckle => 9,
		NoiseReduction => 10,
		QuietZoneSize => 11,
		PageNo => 13,
		ReadCode39 => 16,
		ReadEAN13 => 17,
		ReadEAN8 => 18,
		ReadUPCA => 19,
		ReadUPCE => 20,
		ShowCheckDigit => 21,
		Code39NeedStartStop => 22,
		ReadCode128 => 23,
		ReadCode25 => 24,
		ColorThreshold => 25,
		ReadPatchCodes => 27,
		ScanDirection => 28,
		UseOverSampling => 29,
		MinLength => 31,
		Maxlength => 32,
		ReadCodabar => 34,
		ConvertUPCEToEAN13 => 35,
		SkewTolerance => 36,
		ReadCode25ni => 37,
		AllowDuplicateValues => 38,
		MinSeparation => 40,
		ExtendedCode39 => 41,
		Code39Checksum => 42,
		ErrorCorrection => 43,
		ReadNumeric => 44,
		MinSpaceBarWidth => 46,
		ReadPDF417 => 47,
		MedianFilter => 48,
		Code25Checksum => 51,
		Encoding => 52,
		GammaCorrection => 53,
		SkewLineJump => 54,
		ReadDataMatrix => 65,
		ReadQRCode => 83,
		ColorProcessingLevel => 68,
		MedianFilterLevel => 85,
		QuotedPrintableCharSet => 84,
		UseScaledFaxCoords => 82,
		FilePathEncoding => 81,
		ReadCode93 => 76,
		CodabarMaxVariance => 75,
		UseRunCache => 74,
		SkewSpeed => 73,
		ColorChunks => 72,
		DatabarOptions => 71,
		UseOldCode128Algorithm => 70,
		ReadDatabar => 69,
		ShowCodabarStartStop => 66,
		PatchCodeMinOccurrence => 64,
		PDF417Debug => 63,
		ShortCode128MinLength => 62,
		BitmapResolution => 61,
		Photometric => 60,
		ReadMicroPDF417 => 57,
		ReadShortCode128 => 56,
		MaxRectOverlap => 55,
	} ;


	$this->get_props() ;

	return $this ;
}

sub ScanBarCode
{
	my $this = shift ;
	my $file = shift ;

	$this->set_props() ;

	$this->{h_bar_codes} = 0 ;
	$this->{h_bar_codes_type} = 0 ;
	$this->{count} = 0 ;

	$this->{count} = scanbarcode($this->{handle}, $file, $this->{h_bar_codes}, $this->{h_bar_codes_type}) ;
}

sub GetBarString
{
	my $this = shift ;
	my $index = shift || 0 ;

	return if (($index < 1) || ($index > $this->{count})) ;

	getstring_from_array_ptr($this->{h_bar_codes}, $index - 1) ;
}

sub GetBarStringType
{
	my $this = shift ;
	my $index = shift || 0 ;

	return if (($index < 1) || ($index > $this->{count})) ;

	getstring_from_array_ptr($this->{h_bar_codes_type}, $index - 1) ;
}

sub get_props
{
	my $this = shift ;

	foreach my $key (keys %{$this->{prop_map_uint16}})
	{
		$this->{$key} = get_uint16($this->{handle}, $this->{prop_map_uint16}->{$key}) ;
	}

	foreach my $key (keys %{$this->{prop_map_uint32}})
	{
		$this->{$key} = get_uint32($this->{handle}, $this->{prop_map_uint32}->{$key}) ;
	}

	foreach my $key (keys %{$this->{prop_map_string}})
	{
		$this->{$key} = get_string($this->{handle}, $this->{prop_map_string}->{$key}) ;
	}
}

sub set_props
{
	my $this = shift ;

	foreach my $key (keys %{$this->{prop_map_uint16}})
	{
		set_uint16($this->{handle}, $this->{prop_map_uint16}->{$key}, $this->{$key}) ;
	}

	foreach my $key (keys %{$this->{prop_map_uint32}})
	{
		set_uint32($this->{handle}, $this->{prop_map_uint32}->{$key}, $this->{$key}) ;
	}

	foreach my $key (keys %{$this->{prop_map_string}})
	{
		set_string($this->{handle}, $this->{prop_map_string}->{$key}, $this->{$key}) ;
	}
}

sub DESTROY
{
	my $this = shift ;

	destroy($this->{handle}) ;
}


# Autoload methods go after =cut, and are processed by the autosplit program.

1;
__END__
# Below is stub documentation for your module. You'd better edit it!

=head1 NAME

SoftekBarcode - Perl extension for the Softek Barcode Reader Toolkit

=head1 SYNOPSIS

	use SoftekBarcode ;

	# Create object
	my $bar = SoftekBarcode->new() ;

	#Set properties
	$bar->{'ReadCode39'} = 1 ;
	$bar->{'MultipleRead'} = 1 ;

	# Scan for barcode
	my $count = $bar->ScanBarCode("/tmp/image.tif") ;

	printf "Found %d barcodes\n", $count ;

	# Read results\n
	foreach my $i (1..$count)
	{
		my $barcode = $bar->GetBarString($i) ;
		printf "barcode is %s\n", $barcode ;
		my $type = $bar->GetBarStringType($i) ;
		printf "Type is %s\n", $type ;
	}

=head1 DESCRIPTION

The SoftekBarcode module provides a Perl interface to the Softek Barcode Reader
Toolkit and requires the libbardecode.so file to function. The Perl interface 
is free to use, but the Softek Barcode Reader toolkit is a commercial product.

The module contains the following methods:

new

Create a new instance of the barcode reader toolkit

ScanBarCode ($filePath)

Scan the specified file for barcodes and return the number of barcodes found.

GetBarString ($index)

Get the value of a barcode found after calling ScanBarCode. The argument is a 1 based index to the barcode.

GetBarStringType ($index)

Get the type of a barcode found after calling ScanBarCode. The argument is a 1 based index to the barcode.

The following properties are available and a description for each one can be found at http://www.bardecode.com/help/

		AllowDuplicateValues 
		Code25Checksum
		Code39Checksum 
		Code39NeedStartStop 
		ColorThreshold 
		ColorProcessingLevel
		ConvertUPCEToEAN13 
		DataMatrix
		Despeckle 
		Encoding
		ErrorCorrection 
		ExtendedCode39 
		GammaCorrection
		LineJump
		Maxlength 
		MedianFilter
		MinLength 
		MinOccurrence 
		MinSeparation 
		MinSpaceBarWidth 
		MultipleRead 
		NoiseReduction 
		PageNo 
		Pattern
		PrefOccurrence 
		QuietZoneSize 
		ReadCodabar 
		ReadCode128 
		ReadCode25 
		ReadCode25ni 
		ReadCode39 
		ReadEAN13 
		ReadEAN8 
		ReadNumeric 
		ReadPatchCodes 
		ReadPDF417
		ReadUPCA 
		ReadUPCE 
		ScanDirection 
		ShowCheckDigit 
		SkewLineJump
		SkewTolerance 
		UseOverSampling 
		ReadQRCode
		LicenseKey
		MedianFilterLevel
		QuotedPrintableCharSet
		UseScaledFaxCoords
		FilePathEncoding
		ReadCode93
		CodabarMaxVariance
		UseRunCache
		SkewSpeed
		ColorChunks
		DatabarOptions
		UseOldCode128Algorithm
		ReadDatabar
		ShowCodabarStartStop
		PatchCodeMinOccurrence
		PDF417Debug
		ShortCode128MinLength
		BitmapResolution
		Photometric
		ReadMicroPDF417
		ReadShortCode128
		MaxRectOverlap

=head2 EXPORT

None by default.



=head1 SEE ALSO

http://www.bardecode.com

=head1 AUTHOR

Andy Beezer, E<lt>andy.beezer@softeksoftware.co.uk<gt>

=head1 COPYRIGHT AND LICENSE

Copyright (C) 2007 by Andy Beezer, Softek Software Ltd

This library is free software; you can redistribute it and/or modify
it under the same terms as Perl itself, either Perl version 5.8.0 or,
at your option, any later version of Perl 5 you may have available.


=cut
