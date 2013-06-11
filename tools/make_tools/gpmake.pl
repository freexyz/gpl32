#!/usr/bin/perl
use Config::IniFiles;  #這是 include ini 要用到的物件) 
# 給 General Plus 的 module make 用
# gmake all 表 make all
# gmake cli 表 build cli 並產生 rom file 在 rom 中

#process set
@build_modules=@ARGV;
$module_nums=scalar(@ARGV);
ConfigIniInit("Config.ini");
&build_module_main;
my $MakeRootPath;
my $ProjectPath;

 
sub build_module_main
{
  my $build_path;
  my $i;
  my $AllValidModules;
  my $AllModuleValid = 0;
  my @AllModulesArray;
  my $ValidPrameter;
  my $FirstParaName = @build_modules[0];
 # my $MakeRootPath  ;
 # my $ProjectPath = GetCurrentPath();
  my $ProcessPath;
  my $CpoyFromPath;
  my $CopyToPath;
  my $ChangePath;
  my $drv_l1_path;
  my $drv_l2_path;
  my $gplib_path;
  my $app_path;
  my $global_path;
  my $MakeGenerFile;
  my $RtMakeGenerFile;
  my $CommandLine;
  my @drv_l1_Module_array;
  my @drv_l2_Module_array;
  my @gplib_Module_array;
  my @app_Module_array;
  my @global_Module_array;
  my $module_name;
  my $all_modules;
  my $project_name;
  my $ini_project_name;
  
  $MakeGenerFile = "makegener.pl";
  $RtMakeGenerFile = "rtmakegen.pl";
  $ValidParameter = "all depend clean install test";
  	
  @AllModulesArray = GetAllModuleNames();	
  
  $MakeRootPath = sprintf("%s",GetCurrentDrive());
  $drv_l1_path = sprintf("driver_l1");
  $drv_l2_path = sprintf("driver_l2");
  $gplib_path = sprintf("gplib");
  $app_path = sprintf("application");
  $global_path = sprintf(".");
  
  $ProjectPath = GetCurrentPath();

  @drv_l1_Module_array = GetDrvL1ModuleNames();
  @drv_l2_Module_array = GetDrvL2ModuleNames();
  @gplib_Module_array = GetGplibModuleNames();
  @app_Module_array = GetAppModuleNames();
  @global_Module_array = GetGlobalModuleNames();
  $project_name = GetCurrentFolderName();
  $ini_project_name = ReadConfigIni("Project","PROJECTNAME");
  
  $all_modules = sprintf ("@drv_l1_Module_array\n");
  $all_modules = sprintf ("$all_modules@drv_l2_Module_array \n");  
  $all_modules = sprintf ("$all_modules@gplib_Module_array \n");  
  $all_modules = sprintf ("$all_modules@global_Module_array \n");
  $all_modules = sprintf ("$all_modules@app_Module_array \n");
    if ($module_nums == 0)
    {
	    printf ("\n=== GeneralPlus GpMake v1.0.0.2 ===\n");
      printf ("Please input the valid parameter and module name after GpMake.\n");
      printf ("Parameters: \nall clean rebuild install\n%s \n",$all_modules);
      #printf ("Author: Dominant Yang  10/30/2008\n");

			if ($project_name ne $ini_project_name)
			{
				goto INSTALL;
		  }
      goto END;
    }
    
    #if (@build_modules[0] ne 'all')

       my $i;
       my $j;
       my @Modules;
       my $MatchFlag=0;
       my $word;

       if ($FirstParaName eq "all")
       {	
	        for ($i=0; $i<scalar(@drv_l1_Module_array); $i++)
	        {
	           module_make_cmdline($drv_l1_path, @drv_l1_Module_array[$i], "make");  	
	        }
	        for ($i=0; $i<scalar(@drv_l2_Module_array); $i++)
	        {
	         	 module_make_cmdline($drv_l2_path, @drv_l2_Module_array[$i], "make");  	 
	        }
	        for ($i=0; $i<scalar(@gplib_Module_array); $i++)
	        {
	           module_make_cmdline($gplib_path, @gplib_Module_array[$i], "make");  
	        }
	      
	        for ($i=0; $i<scalar(@global_Module_array); $i++)
	        {
	         	  module_make_cmdline($global_path, @global_Module_array[$i], "make");  
	        }
	        
	        for ($i=0; $i<scalar(@app_Module_array); $i++)
	        {
	           module_make_cmdline($app_path, @app_Module_array[$i], "make");  
	        }
	        goto END;
       }
       
       if ($FirstParaName eq "clean")
       {	
	        for ($i=0; $i<scalar(@drv_l1_Module_array); $i++)
	        {
	         	module_make_cmdline($drv_l1_path, @drv_l1_Module_array[$i], "make clean");  
	        }
	        for ($i=0; $i<scalar(@drv_l2_Module_array); $i++)
	        {
	         	module_make_cmdline($drv_l2_path, @drv_l2_Module_array[$i], "make clean"); 	
	        }
	        for ($i=0; $i<scalar(@gplib_Module_array); $i++)
	        {
	          module_make_cmdline($gplib_path, @gplib_Module_array[$i], "make clean"); 	
	        }
	        for ($i=0; $i<scalar(@global_Module_array); $i++)
	        {
	         	module_make_cmdline($global_path, @global_Module_array[$i], "make clean");  	
	        }
	        for ($i=0; $i<scalar(@app_Module_array); $i++)
	        {
	         	module_make_cmdline($app_path, @app_Module_array[$i], "make clean");  	
	        }
	        goto END;
       }	
       
       if ($FirstParaName eq "rebuild")
       {	
	        for ($i=0; $i<scalar(@drv_l1_Module_array); $i++)
	        {
	        	module_make_cmdline($drv_l1_path, @drv_l1_Module_array[$i], "make"); 
	         	module_make_cmdline($drv_l1_path, @drv_l1_Module_array[$i], "make clean");  
	        }
	        for ($i=0; $i<scalar(@drv_l2_Module_array); $i++)
	        {
	        	module_make_cmdline($drv_l2_path, @drv_l2_Module_array[$i], "make"); 	
	         	module_make_cmdline($drv_l2_path, @drv_l2_Module_array[$i], "make clean"); 	
	        }
	        for ($i=0; $i<scalar(@gplib_Module_array); $i++)
	        {
	        	module_make_cmdline($gplib_path, @gplib_Module_array[$i], "make"); 
	          module_make_cmdline($gplib_path, @gplib_Module_array[$i], "make clean"); 	
	        }
	        for ($i=0; $i<scalar(@global_Module_array); $i++)
	        {
	        	module_make_cmdline($global_path, @global_Module_array[$i], "make"); 
	         	module_make_cmdline($global_path, @global_Module_array[$i], "make clean");  	
	        }
	        for ($i=0; $i<scalar(@app_Module_array); $i++)
	        {
	        	module_make_cmdline($app_path, @app_Module_array[$i], "make"); 
	         	module_make_cmdline($app_path, @app_Module_array[$i], "make clean");  	
	        }
	        goto END;
       }	

       if ($FirstParaName eq "install")
       {	
INSTALL:      	
					if ($project_name ne ReadConfigIni("Project","PROJECTNAME"))
					{
					   WriteConfigIni("Project","PROJECTNAME",$project_name);
					}
					ProjectUtlGenerator();
					
				  $CopyFromPath = sprintf("./project.utl");
					$CopyToPath = sprintf("%s/tools/make_tools",$MakeRootPath);
					#printf ("\n$CopyToPath\n");
					$CommandLine = sprintf("gpcopy %s %s",$CopyFromPath, $CopyToPath);
					$CommandLine=TransToWinPathFormat($CommandLine);
					#printf ("\n$CommandLine\n");
					#system ($CommandLine); 
					`$CommandLine`;
					$CommandLine = sprintf("del project.utl");
					#system ($CommandLine); 
					`$CommandLine`;
					#printf ("\n===Install OK===\n");
	        goto END;
       }
       for ($i=0; $i<scalar(@drv_l1_Module_array); $i++)
	     {
         if ($FirstParaName eq @drv_l1_Module_array[$i])
         {
         	 module_make_cmdline($drv_l1_path, @drv_l1_Module_array[$i], "make clean");
         	 module_make_cmdline($drv_l1_path, @drv_l1_Module_array[$i], "make"); 
         }
       }
       
       for ($i=0; $i<scalar(@drv_l2_Module_array); $i++)
	     {
         if ($FirstParaName eq @drv_l2_Module_array[$i])
         {
         	 module_make_cmdline($drv_l2_path, @drv_l2_Module_array[$i], "make clean");
         	 module_make_cmdline($drv_l2_path, @drv_l2_Module_array[$i], "make"); 
         }
       }
       
       for ($i=0; $i<scalar(@gplib_Module_array); $i++)
	     {
         if ($FirstParaName eq @gplib_Module_array[$i])
         {
         	 module_make_cmdline($gplib_path, @gplib_Module_array[$i], "make clean");
         	 module_make_cmdline($gplib_path, @gplib_Module_array[$i], "make"); 
         }
       }
             
       for ($i=0; $i<scalar(@global_Module_array); $i++)
	     {
         if ($FirstParaName eq @global_Module_array[$i])
         {
         	 module_make_cmdline($global_path, @global_Module_array[$i], "make clean"); 
         	 module_make_cmdline($global_path, @global_Module_array[$i], "make"); 
         }
       }
       
       for ($i=0; $i<scalar(@app_Module_array); $i++)
	     {
         if ($FirstParaName eq @app_Module_array[$i])
         {
         	 module_make_cmdline($app_path, @app_Module_array[$i], "make clean"); 
         	 module_make_cmdline($app_path, @app_Module_array[$i], "make"); 
         }
       }
       
       goto END;
    
END:
  
}

sub ConfigIniInit # (FileName)  **at the program beging, we must initial the config ini to get file handle first.
{
  $CfgFileName = @_[0];	
   $ConfigIniFile = new Config::IniFiles -file => @_[0];
}

sub ReadConfigIni # (SectionName, MemberName)  **return: value of MemberName
{
	return $ConfigIniFile->val(@_[0],@_[1]);
}
sub WriteConfigIni # (SectionName, MemberName, MemberValue)  
{
	$ConfigIniFile->newval (@_[0], @_[1], @_[2]);
	$ConfigIniFile->WriteConfig ($CfgFileName);
}

sub DelConfigIni # (SectionName, MemberName)
{
	$ConfigIniFile->delval(@_[0], @_[1]);
}

sub GetAllModuleNames # return the module names to array.
{
	# get all names of each module
    my @ModuleNames;
    ConfigIniInit("Config.ini");
    @ModuleNames = split(/\s+/,ReadConfigIni("MakeCfg","MODULES"));
    return @ModuleNames;	 	
}

sub GetDrvL1ModuleNames # return the module names to array.
{
	# get all names of each module
    my @ModuleNames;
    ConfigIniInit("Config.ini");
    @ModuleNames = split(/\s+/,ReadConfigIni("MakeCfg","DRIVER_L1"));
    return @ModuleNames;	 	
}

sub GetDrvL2ModuleNames # return the module names to array.
{
	# get all names of each module
    my @ModuleNames;
    ConfigIniInit("Config.ini");
    @ModuleNames = split(/\s+/,ReadConfigIni("MakeCfg","DRIVER_L2"));
    return @ModuleNames;	 	
}

sub GetGplibModuleNames # return the module names to array.
{
	# get all names of each module
    my @ModuleNames;
    ConfigIniInit("Config.ini");
    @ModuleNames = split(/\s+/,ReadConfigIni("MakeCfg","GPLIB"));
    return @ModuleNames;	 	
}

sub GetAppModuleNames # return the module names to array.
{
	# get all names of each module
    my @ModuleNames;
    ConfigIniInit("Config.ini");
    @ModuleNames = split(/\s+/,ReadConfigIni("MakeCfg","APPLICATION"));
    return @ModuleNames;	 	
}

sub GetGlobalModuleNames # return the module names to array.
{
	# get all names of each module
    my @ModuleNames;
    ConfigIniInit("Config.ini");
    @ModuleNames = split(/\s+/,ReadConfigIni("MakeCfg","GLOBAL"));
    return @ModuleNames;	 	
}

sub GetCurrentPath
{
 my $CurPath = `cd`;
 my @PathSplitArray;
    @PathSplitArray = split(/\\/,$CurPath);
    $CurPath = join ("/",@PathSplitArray);
    #print ($CurPath);
    chomp($CurPath);
    return $CurPath;
}

sub GetCurrentWinPath
{
 my $CurWinPath = `cd`;
 chomp($CurWinPath);
   return $CurWinPath;
}

sub TransToWinPathFormat
{
 my $InputPath = @_[0];
 my @PathSplitArray;
    @PathSplitArray = split(/\//,$InputPath);
    $InputPath = join ("\\",@PathSplitArray);
    #print ($CurPath);
    chomp($InputPath);
    return $InputPath;
}

sub GetCurrentDrive
{
 my $CurDrv = `cd`;
 chomp($CurDrv);
 $CurDrv =~ s/^(\w:).*/$1/;
 #print ($CurDrv);
 return $CurDrv;
}

sub module_make_cmdline  # (LayerName, ModuleName, CmdLine)
{
	my $LayerName = @_[0];
	my $ModuleName = @_[1];
	my $CmdLine = @_[2];
	my $ProcessPath;
	my $CopyFromPath;
	my $CopyToPath;
	my $CommandLine;
	my $md_path;
	my $liba_path;
	my $cmd_response;
 
  
  $liba_path = ReadConfigIni("Project","LIBA_OUT_PATH");
  if ($liba_path eq '')
  {
  	$liba_path = "liba";
  }
  
	$ProcessPath = sprintf("%s/%s/%s/make",$MakeRootPath,$LayerName,$ModuleName);
	printf("\n===>$ProcessPath\n");
	chdir ($ProcessPath);
	system "$CmdLine"; 
	
	if ($CmdLine eq "make")
	{
		$CopyFromPath = sprintf("%s/%s/%s/make/*.a",$MakeRootPath,$LayerName,$ModuleName);
		#$CopyToPath = sprintf("%s/liba",$ProjectPath);
		$CopyToPath = sprintf("%s/%s/",$MakeRootPath,$liba_path);
		#printf ("md $CopyToPath");
		#`md $CopyToPath`;
		
		
		#printf ("\n$CopyToPath\n");
		$CommandLine = sprintf("copy %s %s",$CopyFromPath, $CopyToPath);
		$CommandLine=TransToWinPathFormat($CommandLine);
		#printf ("\n$CommandLine\n");
		`$CommandLine`; 
  } 
	
} 


sub CreateMakeFile #(char* buffer)
{
  open (MAKEFILE, ">makefile");
  print MAKEFILE @_;
  close (MAKEFILE);
}

sub CreateProjectUtl #(char* buffer)
{
  open (MAKEFILE, ">project.utl");
  print MAKEFILE @_;
  close (MAKEFILE);
}

sub ProjectUtlGenerator
{
 my  $Widx=0;
 my  @WriteBuf;
 my  @TargetArray;
 my  @SplitArray;
 my  @TargetO;
 my  $target_o;
 my  $i;
 my  $DepArray;
 my  $DepArrayI;

#initial the config ini file handles
ConfigIniInit("Config.ini");

#my $test = ReadConfigIni("ToolChainCfg","CC");
# Setup the environment variable
  @WriteBuf[$Widx] = sprintf ("PROJECTNAME = %s\n",ReadConfigIni("Project","PROJECTNAME"));
  $Widx++;
  @WriteBuf[$Widx] = sprintf ("OPTIMAL_FLAG_DRV = %s\n",ReadConfigIni("Project","OPTIMAL_FLAG_DRV"));
  $Widx++;
  @WriteBuf[$Widx] = sprintf ("OPTIMAL_FLAG = %s\n",ReadConfigIni("Project","OPTIMAL_FLAG"));  

  CreateProjectUtl (@WriteBuf);

}


sub GetCurrentFolderName
{
 my $CurDrv = `cd`;
 my @tree;
 my $idx;
 my $folder_name;

 @tree = split(/\\/,$CurDrv);
 $idx = (scalar(@tree) - 1);

 $folder_name = @tree[$idx];
 return $folder_name;
}
