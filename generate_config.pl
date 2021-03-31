#!/usr/bin/perl

#if no arguments, will substitute $VARIABLE in every file in conf_templates/, and copy them to conf/
#otherwise, just does the substitution on the given files
#Note: $VARIABLE has to be in the script's shell environment
#VARIABLE=name ./generate_config.pl to set a custom one

my @files = @ARGV;
my $template_dir = "conf_templates/";
my $conf_dir = "conf/";


if (!@files) {
	opendir (DH, $template_dir);
	@files = readdir(DH);
	closedir(DH);
	foreach $file (@files) {
		$file = $template_dir.$file;
	}
}

foreach $file (@files) {
	my $file_name = substr($file, length($template_dir));
	if (-f $file && !system("perl -p -e 's/\\\$([_A-Z]+)/\$ENV{\$1}/g' < $file > $ENV{PWD}/$conf_dir$file_name")) {
		print "created $conf_dir$file_name\n";
	}
}
