#!/usr/bin/perl

my $latexPreProcessFile = shift;
if(!$latexPreProcessFile)
{
  print STDERR "!no input file defined\n";
  exit;
}


my $gcc_cmd   = "avr-gcc";
my $latex_cmd = 'F:\ootb\editoren\miktex-portable-2.8.3761\miktex\bin\pdflatex.exe';

my $escapableCharacters = "'´`\"\\\\/";


my @includePaths ;
my $includes;
my @texPaths = ('.');
my $defines;

my $keepIntermediate = 0;

my $latexMainFile = $latexPreProcessFile;

while(my $option = shift)
{
   if($option eq '-I')
   {
     my $p = shift;
     if(!$p)
     {
       print STDERR "!cmd error\n";
       exit;
     }else{
       $includes .= "-I \"$p\" ";
       push(@includePaths, $p);
     }
   }
   if($option eq '-T')
   {
     my $p = shift;
     if(!$p)
     {
       print STDERR "!cmd error\n";
       exit;
     }else{
       push(@texPaths, $p);
     }
   }

   if($option eq '-D')
   {
     my $p = shift;
     if(!$p)
     {
       print STDERR "!cmd error\n";
       exit;
     }else{
       $defines .= "-D $p ";
       push(@defines, $p);
     }
   }
   if($option eq '-m')
   {
     my $p = shift;
     if(!$p)
     {
       print STDERR "!cmd error\n";
       exit;
     }else{
       $latexMainFile = $p;
     }
   }  
   if($option eq '-k'){
     $keepIntermediate = 1;
   }
}


my @mainFile = readFile($latexPreProcessFile, @texPaths);

open(OUTFILE, ">$latexPreProcessFile.c");

for(my $i = 0; $i< @mainFile; ++$i)
{
  #$mainFile[$i] =~ s/\\\\/\%>newline<\%/g;
  my $line = $mainFile[$i];
  chomp($line);
  if($line =~ /^#\s*include\s+[<"](.*)[>"]/)
  {
    print "found #include: '$1'\n";
    splice(@mainFile, $i, 0, '%%%#### include --- BEGIN - '.$line.' - BEGIN --- include ###%%%'."\n");
    splice(@mainFile, $i+2, 0, '%%%#### include ---  END  - '.$line.' - BEGIN --- include ###%%%'."\n");
    $i += 2;
  }
  
  $line =~ s/%.*//g;
  
  if($line =~ /\\input\{(.*)\}/)
  {
    print "including input file: '$1'\n";
    my @subText = readFile($1, @texPaths);
    unshift(@subText, '%======================================== BEGIN - '.$line.'  ========================================%'."\n");
    push(   @subText, '%========================================  END - '.$line.'   ========================================%'."\n");
    splice(@mainFile, $i, 1, @subText);
  }

  $mainFile[$i] =~ s/([$escapableCharacters])/'%>'.ord($1).'<%'/eg;

  
#  
}


print OUTFILE @mainFile;
close(OUTFILE);
my $preprocessCMD = "$gcc_cmd -E $includes $defines \"$latexPreProcessFile.c\" > \"$latexPreProcessFile.i\"";
print $preprocessCMD ."\n";
print `$preprocessCMD`;

my @mainFile = readFile($latexPreProcessFile.'.i', @texPaths);

my $preprocessedFile = $latexPreProcessFile;
$preprocessedFile =~ s/(.*)\.(.*?)/\1_pp.\2/;
open(OUTFILE, ">$preprocessedFile");

my $scriptname = $0;
$scriptname =~ s/.*[\\\/](.*)/\1/;
print OUTFILE "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n";
print OUTFILE "%%% THIS FILE IS AUTOMATICLY GENERATED - DO NOT CHANGE THINGS HERE - THEY WILL BE OVERWRITTEN             \n";
print OUTFILE "%%% Please edit  '".$latexPreProcessFile."' \n";
print OUTFILE "%%% and execute '". $scriptname ."' again.\n";
print OUTFILE "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n";
print OUTFILE "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";
my $marker = '%%%#### include';

while(my $line = shift(@mainFile))
{
  if($line =~ /^#/)
  {  # do nothing
  }elsif( $line =~ /$marker/)
  {
    print "found include marker ";
    my $foundend = 0;
    do
    { 
      print '.';
      $line = shift(@mainFile);
      if($line =~ /$marker/){
        $foundend = 1;
        print " found end\n";
      }
      
    }while( ! $foundend);
  }else{
    #$line =~ s/\%>newline<\%/\\\\/g;
    $line =~ s/\%>(\d+)<\%/chr($1)/eg;

    print OUTFILE $line;
  }  
}


close(OUTFILE);


print `$latex_cmd -interaction=nonstopmode "$latexMainFile"`;


if($keepIntermediate == 0)
{
  unlink($latexPreProcessFile.'.i');
  unlink($latexPreProcessFile.'.c');
}


sub readFile
{
  my $file = shift;
  my @folders = shift;
  my @res;
  my @paths  = map($_.'/'.$file, @folders);
  unshift(@paths, $file);
  
  #print join("\n", @paths);
  
  while ( @paths )
  {
    if( -f($paths[0]) )
    {
      if( open(READFILE, $paths[0]) ){
        @res = <READFILE>;
        close READFILE;
        return @res;
      }else{
        print STDERR "!Error opening File: '".$paths[0]."'\n";
      }
      
    }
    shift(@paths);  
  }
  if(! @folders)   #file found
  {
     print STDERR "!Could not locate File: $file\n";
  }
  return @res;
}


