#!/usr/bin/perl -s

##############################################################################
#
# Project:  NitroSDK - header generator - 
# File:     header_generator_ioreg.pl
#
# Copyright 2003-2004 Nintendo. All rights reserved.
#
# These coded instructions, statements, and computer programs contain
# proprietary information of Nintendo of America Inc. and/or Nintendo
# Company Ltd., and are protected by Federal copyright law. They may
# not be disclosed to third parties or copied or duplicated in any form,
# in whole or in part, without the prior written consent of Nintendo.
#
# $Log: header_generator_ioreg_sp.pl,v $
# Revision 1.6  2004/05/18 01:48:02  takano_makoto
# Included optimized routine by conditional logic
#
# Revision 1.4  2004/05/14 09:56:57  takano_makoto
# Add Condition Field.
#
# Revision 1.3  2004/02/12 10:56:04  yasu
# new location of include files ARM9/ARM7
#
# Revision 1.2  2004/02/05 07:09:01  yasu
# change SDK prefix iris -> nitro
#
# Revision 1.1  2004/01/22 08:36:31  nishida_kenji
# output not #include <nitro/hw/mmap_global.h>
# but #include <nitro_sp/hw/mmap_global.h>
#
# Revision 1.5  2003/12/16 09:45:31  yasu
# Support for nitro_sp
#
# Revision 1.4  2003/12/08 12:25:55  yada
# Made so that when SDK_ASM not defined, will not include types.h
#
# Revision 1.3  2003/12/08 12:19:12  yada
# Changed the include file from MemoryMap.h to mmap_global.h
#
# Revision 1.2  2003/12/08 08:05:51  nishida_kenji
# generate only English
#
# Revision 1.1  2003/11/17 01:54:50  yasu
# Moved ioreg generating script to below ioreg/
#
# Revision 1.17  2003/11/14 06:16:08  nishida_kenji
# Abolished format of REG at the beginning of register variable names.
#
# Revision 1.16  2003/11/14 06:11:31  yasu
# included the addition of <nitro/hw/memoryMap.h> 
# Added a warning that it is an auto generating file.
#
# Revision 1.15  2003/11/13 23:57:31  nishida_kenji
# Made so that register variable outputs reg_*
#
# $NoKeywords: $
##############################################################################

#
# Header file macro generation script
# 
# How to use:
# conv.pl [-v] [-dup] filename1.csv [filename2.csv ...]
#
# Output the header file filename1.h (filename2.h...) where the 
# macro definition from the CSV file is written.
#
# See (SDKRoot)/docs/private/how-to-make-headers.txt
# for CSV file format.
#

use Text::ParseWords;

#
# Global variable
#
$line_no = 0;
%name_hash = ();
%index_name_hash = ();
@hash_array = ();




#
# verbose output
#
sub verbose {
  if ($verbose_mode == 1) {
    print STDERR @_;
  }
}




#
# Remove spaces at beginning and end of field.
#
sub trim {
  my @out = @_;
  for (@out) {
    s/^\s+//;
    s/\s+$//;
  }
  return @out;
}




#
# CSV parsing routine
#
sub parse_csv {
  my @fields = quotewords(",", 0, shift @_);
  @fields = trim(@fields);
  
  my $tmp;
  # Remove ,,,, from end of line
  while(defined($tmp = pop @fields) && $tmp =~ /^$/ ) {
       ;
  }
  push @fields, $tmp;

  return @fields;
}




#
# Pre processor
#
sub preprocess {
  return parse_csv(shift @_);
}




#
# Check for duplicate macro names
#

sub check_macro_duplicate {
  my $name = shift @_;
  my $condition = shift @_;
  my $no   = shift @_;
  
  if ($condition ne "") {
    return 0;
  }
  
  if (exists $name_hash{$name}) {
    if ($duplicate_ok == 1) {
      print STDERR  "WARNING: generating duplicate macro \'$name\' in line $no\n";
    } else {
      die "ERROR: duplicate macro \'$name\' in line $no";
    }
    return 1;
  } else  {
    $name_hash{$name} = 1;
    return 0;
  }
}



#
# Analyze
#
sub analyze {
  my @fields = @_;

  my $fields;
  my $tmphash;
  my $address, $condition, $name, $bitwidth, $rw, $category, $volatile;
  $tmphash = {};
  
  
  $address = shift @fields;
  die "ERROR: Illegal address \'$address\' in line $line_no\n" unless ($address =~ /^0x[0-9A-Fa-f]+$/);
  
  
  $condition = shift @fields;
  die "ERROR: Illegal condition name \'$condition\' in line $line_no\n" unless ($address =~ /^[A-Za-z0-9_()&|!]+$/);
  
  
  $name = shift @fields;
  die "ERROR: Illegal macro name \'$name\' in line $line_no\n" unless ($name =~ /^[A-Za-z0-9_]+$/);
  check_macro_duplicate($name, $condition, $line_no);
  
  
  $bitwidth = shift @fields;
  if ($bitwidth eq "8") {
    $mask_format = "0x%02x";
  } elsif ($bitwidth eq "16") {
    $mask_format = "0x%04x";
  } elsif ($bitwidth eq "32") {
    $mask_format = "0x%08x";
  } elsif ($bitwidth eq "64") {
    $mask_format = "0x%016x";
  } else {
    die "ERROR: Illegal bitwidth \'$bitwidth\' in line $line_no\n";
  }
  
  $rw = shift @fields;
  $category = shift @fields;
  $volatile = shift @fields;
  
  $tmphash->{"condition"} = $condition;
  $tmphash->{"name"} = $name;
  $tmphash->{"offset"} = $address;
  $tmphash->{"bitwidth"} = $bitwidth;
  $tmphash->{"rw"} = $rw;
  if ($rw eq "r") {
    $tmphash->{"const"} = "const";
  } else {
    $tmphash->{"const"} = "";
  }
  
  $tmphash->{"category"} = $category;

  if ($volatile eq "volatile") {
    $tmphash->{"volatile"} = "v";
  } elsif ($volatile eq "permanent") {
    $tmphash->{"volatile"} = "";
  } else {
    die "ERROR: specify volatile/permanent in line $line_no\n";
  }
    
  @{$tmphash->{"option"}} = splice @fields, 0;

  $tmphash->{"mask_format"} = $mask_format;
  $tmphash->{"line_no"} = $line_no;
  return $tmphash;
}


#
# Add elements to array
# If there are duplicates, link them as linear list
#
sub push_fields {
    my $hash = shift @_;
    my $array_num;
    my $tmp_hash;
    
    # If the same name is already registered, add it to hash table as linear list.
    if ( exists($index_name_hash{ $hash->{"name"} }) ) {
        $array_num = $index_name_hash{ $hash->{"name"} };  # Get index of corresponding array
        # Add so that (condition eq "") comes to the end of linear list.
        $tmp_hash = $hash_array[ $array_num ];
        if ( $tmp_hash->{"condition"} eq "") {
            $hash->{"next"} = $tmp_hash;
            $hash_array[ $array_num] = $hash;
            return;
        }
        
        while ( exists($tmp_hash->{"next"}) ) {
            if ($tmp_hash->{"next"}->{"condition"} eq "") {
                $hash->{"next"} = $tmp_hash->{"next"};
                $tmp_hash->{"next"} = $hash;
                return;
            }
            $tmp_hash = $tmp_hash->{"next"};
        }
        $tmp_hash->{"next"} = $hash;
    } else {
        $array_num = @hash_array;
        $index_name_hash{ $hash->{"name"} } = $array_num;
        push @hash_array, $hash;
    }
}


#
# Category information collection
#
sub collect_category {
  my %cat_hash;
  my $hash;
  foreach $hash (@hash_array) {
    $cat_hash{$hash->{"category"}} = 1;
  }
  return (keys %cat_hash);
}

#
# Merge condition
#
sub merge_condition {
    my $cond1 = shift @_;
    my $cond2 = shift @_;
    my $name = shift @_;
    
    my $merged_cond;
    
    if ( $cond1 eq "" || $cond2 eq "") {
        $merged_cond = "";
        return $merged_cond;
    }
    
    if ( "$cond1" eq "!$cond2" || "!$cond1" eq "$cond2" ) {
        $merge_cond = "";
    } else {
        $merged_cond = "$cond1 || $cond2";
    }
    
    return $merged_cond;
}



#
# Get offset definition of register
#
sub cull_register_offset_def {
    my $hash = shift @_;
    
    my $offset_name;
    my @tmp_hash;
    my @def_array = ();
    my $i, $k;
    
    $i = 0;
    while ( 1 ) {
        
        {
            $offset_name = "REG_$hash->{'name'}_OFFSET";
            $hash->{"offset_def"} = $offset_name;
            $hash->{"address_def"} = "REG_$hash->{'name'}_ADDR";
            # Leave the definition of this variable because it will be used later.
            $hash->{"valname_def"} = "REG_$hash->{'category'}_$hash->{'name'}";
            $hash->{"valname_def_new"} = "reg_$hash->{'category'}_$hash->{'name'}";
        }
        
        $tmp_hash[$i]->{"condition"}        = $hash->{"condition"};
        $tmp_hash[$i]->{"name"}             = $hash->{"name"};
        $tmp_hash[$i]->{"offset_def"}       = $hash->{"offset_def"};
        $tmp_hash[$i]->{"offset"}           = $hash->{"offset"};
        $tmp_hash[$i]->{"address_def"}      = $hash->{"address_def"};
        $tmp_hash[$i]->{"valname_def"}      = $hash->{"valname_def"};
        $tmp_hash[$i]->{"valname_def_new"}  = $hash->{"valname_def_new"};
        
        my $val = "(*($hash->{'const'} REGType$hash->{'bitwidth'}$hash->{'volatile'} *) $hash->{'address_def'})";
        
        $tmp_hash[$i]->{"reg_type"} = $val;
        
        my $find_flg = 0;
        
        #If reg_type and offset are same, merge condition.
        foreach $k (@def_array) {
            
            if ( $k->{'condition'} eq "" ) {
                # Field without conditions should come last, so if it exists ahead already, condition setting is wrong.
                print STDERR "WARNING: illegal condition in \'$k->{'name'}\'\n";
            }
            
            if ( ($k->{"reg_type"} eq $tmp_hash[$i]->{"reg_type"}) && ($k->{"offset"} eq $tmp_hash[$i]->{"offset"}) ) 
            {
                $k->{'condition'} = merge_condition( $k->{'condition'}, $tmp_hash[$i]->{'condition'}, $k->{'name'} );
                $find_flg = 1;
                last;
            }
        }
        if ($find_flg == 0) {
            push @def_array, $tmp_hash[$i];
            $i++;
        }
        
        # If still in the list, process the next.
        if (exists($hash->{"next"})) {
            $hash = $hash->{"next"};
        } else {
            last;
        }
    }
    
    return @def_array;
}


#
# Output
#
sub output {
    my $output_filename = shift @_;
    my $category = shift @_;
    
    # Open output file
    
    open OUT, ">$output_filename" or die "ERROR: Cannot create file \'$output_filename\'\n";
    my $handle = OUT;
#   my $handle = STDOUT;

    my $include_guard = $output_filename;
    $include_guard =~ s/[.\/]/_/g;
    $include_guard = uc($include_guard). "_";
    
    #
    # Output
    #
    print $handle <<ENDDOC;
/*---------------------------------------------------------------------------*
  Project:  NitroSDK - IO Register List - 
  File:     $output_filename

  Copyright 2003-2004 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

 *---------------------------------------------------------------------------*/
//
//  I was generated automatically, don't edit me directly!!!
//
#ifndef $include_guard
#define $include_guard

#ifndef SDK_ASM
#include <nitro/types.h>
#include <nitro/hw/ARM7/mmap_global.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Definition of Register offsets, addresses and variables.
 */


ENDDOC
    my $hash, $tmp_hash;
    
    foreach $hash (@hash_array) {
        next if ($hash->{"category"} ne $category);
        
        print $handle "/* $hash->{'name'} */\n\n";
        
        my @reg_array = cull_register_offset_def($hash);  # Optimize duplicate items
        my $nest_state = 0;
        
        foreach $tmp_hash (@reg_array) 
        {
            if ($tmp_hash->{"condition"} ne "") {
                if ($nest_state == 0) {
                    $nest_state = 1;
                    printf $handle "#if %s\n\n", $tmp_hash->{'condition'};
                } elsif ($nest_state == 1) {
                    printf $handle "\n#elif %s\n\n", $tmp_hash->{'condition'};
                }
            } elsif ($nest_state == 1) {
                printf $handle "\n#else\n\n";
            }
            
            printf $handle "#define %-50s %s\n", $tmp_hash->{'offset_def'}, $tmp_hash->{'offset'};
            printf $handle "#define %-50s %s\n", $tmp_hash->{"address_def"}, "(HW_REG_BASE + $tmp_hash->{'offset_def'})";
            printf $handle "#define %-50s %s\n", $tmp_hash->{"valname_def_new"}, $tmp_hash->{"reg_type"};
            
            if ($nest_state == 1) {
                if ( $tmp_hash->{"condition"} ne "" ) {
                    printf $handle "// endif %s\n", $tmp_hash->{"condition"};
                } else {
                    printf $handle "// endif else\n";
                }
            }
        }
        
        if ($nest_state == 1) {
            printf $handle "#endif\n";
        }
        printf $handle "\n";
        
    }
    
    print $handle <<ENDDOC;

/*
 * Definitions of Register fields
 */

ENDDOC
    my $hash;
    my $nest_state;
    
    foreach $hash (@hash_array) {
      next if ($hash->{"category"} ne $category);
      
      $nest_state = 0;
      
      printf $handle "\n";
      print $handle "/* $hash->{'name'} */\n";
      
      while ( 1 ) {
        if ($hash->{"condition"} ne "") {
            if ($nest_state == 0) {
                $nest_state = 1;
                printf $handle "#if %s\n", $hash->{'condition'};
            } elsif ($nest_state == 1) {
                printf $handle "#elif %s\n", $hash->{'condition'};
            }
        } elsif ($nest_state == 1) {
            printf $handle "#else\n";
        }
        
        my $no = $hash->{"line_no"};
        
        my @array = @{$hash->{"option"}};
        
        my $field_macro_func = "#define $hash->{'valname_def'}_FIELD(";
        my $field_macro_flag = 0;
        my $field_macro_body = "    (u$hash->{'bitwidth'})( \\\n";
        
        my $fieldname, $fieldshift, $fieldsize;
        while($fieldname = shift @array) {
            my $shift_macro;
            die "ERROR: Field option not good in line $no\n" if (!defined($fieldshift = shift @array));
            die "ERROR: Field option not good in line $no\n" if (!defined($fieldsize = shift @array));
            
            print $handle "\n";
            
            # Output REG_XXX_XXXXX_SHIFT
            {
                my $name = "$hash->{'valname_def'}_" . $fieldname . "_SHIFT";
                check_macro_duplicate($name, $hash->{'condition'}, $no);
                $shift_macro = $name;
                printf $handle "#define %-50s %s\n", $name, $fieldshift;
            }
            
            # Output REG_XXX_XXXXX_SIZE
            {
                my $name = "$hash->{'valname_def'}_" . $fieldname . "_SIZE";
                check_macro_duplicate($name, $hash->{'condition'}, $no);
                printf $handle "#define %-50s %s\n", $name, $fieldsize;
            }
            
            # Output REG_XXX_XXXXX_MASK
            {
                my $width = $hash->{"bitwidth"};
                my $mask_val = 1 << $fieldshift;
                my $tmp = $mask_val;
                my $i;
                for ($i = 0; $i < $fieldsize - 1; $i++) {
                    $mask_val = $mask_val << 1;
                    $mask_val = $mask_val + $tmp;
                }
                my $name = "$hash->{'valname_def'}_" . $fieldname . "_MASK";
                check_macro_duplicate($name, $hash->{'condition'}, $no);
                printf $handle "#define %-50s $hash->{'mask_format'}\n", $name, $mask_val;
            }
            
            # Create one line worth of definition of REG_XXX_XXXXX_FIELD
            my $fn = lc($fieldname);
            if ($field_macro_flag == 1) {
                $field_macro_func = $field_macro_func . ", " . $fn;
                $field_macro_body = $field_macro_body . " | \\\n" . "    ((u32)($fn) << $shift_macro)";
            } else {
                $field_macro_func = $field_macro_func . " " . $fn;
                $field_macro_body = $field_macro_body . "    ((u32)($fn) << $shift_macro)";
                $field_macro_flag = 1;
            }
        }
        
        if ($field_macro_flag == 1) {
            # Output REG_XXX_XXXXX_FIELD
            $field_macro_func .= " ) \\\n";
            $field_macro_func .= $field_macro_body;
            print $handle "\n#ifndef SDK_ASM\n";
            print $handle $field_macro_func, ")\n";
            print $handle "#endif\n\n"
            
        }
        
        if ($nest_state == 1) {
            if ( $hash->{"condition"} ne "") {
                printf $handle "// endif %s\n", $hash->{"condition"};
            } else {
                printf $handle "// endif else\n";
            }
            if ( !exists($hash->{"next"})) {
                printf $handle "#endif  \n\n";
            }
        }
        
        # If still in the list, display the next.
        if (exists($hash->{'next'})) {
            $hash = $hash->{'next'};
        } else {
            last;
        }
      }
    }
    
    print $handle <<ENDDOC;

#ifdef __cplusplus
} /* extern "C" */
#endif

/* $include_guard */
#endif
ENDDOC

  verbose("done.\n");
}

#
# Set command line option
#


# If -v or -verbose is specified, verbose mode
if ($v == 1 || $verbose == 1) {
  $verbose_mode = 1;
  verbose("verbose mode on\n");
} else {
  $verbose_mode = 0;
}

# If -nodup is specified, duplicate of macro names is not allowed.
if ($nodup == 1) {
  $duplicate_ok = 0;
} else {
  $duplicate_ok = 1;
}




#
# Main routine
#
foreach $filename (@ARGV) {
  $line_no = 0;
  @hash_array = ();
  
  open INPUTFILE, "$filename" or die  "ERROR: Cannot open file \'$filename\'\n";

  my $line;
  while($line = <INPUTFILE>) {
    $line_no++;
    $line =~ s/\"//g;
    $line =~ s/\#.*//;
    next if ($line =~ /^[\s,]*$/);

    @fields = preprocess($line);
    push_fields( analyze(@fields) );
  }
  verbose("$line_no lines read\n");

  my @categories = collect_category();
  my $category;
  my @headerfile_array = ();
  
  
  foreach $category (@categories) {
    my $output_filename = $filename;
    if (($output_filename =~ s/\.csv/\.h/) == 0) {
      $output_filename .= ".h";
    }
    $output_filename =~ s/\.h/_$category.h/;

    push @headerfile_array, $output_filename;

    verbose("Output filename is $output_filename\n");
    output($output_filename, $category);
  }

  my $master_filename = $filename;
  if (($master_filename =~ s/\.csv/\.h/) == 0) {
    $master_filename .= ".h";
  }
  
  open MASTER, ">$master_filename" or die "ERROR: Cannot create file \'$master_filename$'\n";

  my $include_guard = $master_filename;
  $include_guard =~ s/[.\/]/_/g;
  $include_guard = uc($include_guard). "_";
  
  print MASTER <<ENDDOC;
/*---------------------------------------------------------------------------*
  Project:  NitroSDK - IO Register List - 
  File:     $master_filename

  Copyright 2003-2004 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

 *---------------------------------------------------------------------------*/

#ifndef $include_guard
#define $include_guard

#ifdef __cplusplus
extern "C" {
#endif

ENDDOC

  foreach $header_file (@headerfile_array) {
    print MASTER "#include <$header_file>\n"
  }
  
  print MASTER <<ENDDOC;

#ifdef __cplusplus
} /* extern "C" */
#endif

/* $include_guard */
#endif
ENDDOC


}





