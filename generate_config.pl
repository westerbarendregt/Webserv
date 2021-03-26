#!/usr/bin/perl

#if no arguments, will substitute $VARIABLE in every file in conf_templates/, and copy them to conf/
#otherwise, just does the substitution on the given files
#Note: $VARIABLE has to be in the script's shell environment
#VARIABLE=name ./generate_config.pl to set a custom one

my @files = @ARGV;

if (!@files) {
	opendir (DH, "conf_templates");
	@files = readdir(DH);
	closedir(DH);
	chdir "conf_templates";
}

foreach $file (@files) {
	if (-f $file && !system("perl -p -e 's/\\\$([_A-Z]+)/\$ENV{\$1}/g' < $file > $ENV{PWD}/conf/$file")) {
		print "created conf/$file\n";
	}
}
