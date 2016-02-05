<?php

#  <OPTION value="bt36i">Windows installer (includes vendor reader support)</OPTION>
#  <OPTION value="bt36">Windows tar.bz2 (includes vendor reader support)</OPTION>
#  <OPTION value="bt83i">Windows 64-bit installer(no Bruker or T2D support)</OPTION>
#  <OPTION value="bt83">Windows 64-bit tar.bz2 (no Bruker or T2D support)</OPTION>  
#  <OPTION value="bt23">Macintosh (NO vendor reader support)</OPTION>
#  <OPTION value="bt17">Linux 64 bit (NO vendor reader support)</OPTION>
#  <OPTION value="bt81">Source, bjam build (includes vendor reader support)</OPTION>
#  <OPTION value="bt81n">Source, bjam build (NO vendor reader support)</OPTION>
#  <OPTION value="bt211">Source, MSVC build (NO vendor reader support)</OPTION>
#  <OPTION value="bt212">Source, GNU Autotools build (NO vendor reader support)</OPTION>

$debug = 0;

$FileNameBaseArray = array(
    "bt36" => "pwiz-bin-windows-x86-vc120-release-XXX.tar.bz2", #pwiz-bin-windows-x86-vc100-release-3_0_3725.tar.bz2 
    "bt36i" => "pwiz-setup-XXX-x86.msi", #pwiz-setup-3.0.3725-x86.msi 
    "bt83" => "pwiz-bin-windows-x86_64-vc120-release-XXX.tar.bz2", #pwiz-bin-windows-x86_64-vc100-release-3_0_3725.tar.bz2 
    "bt83i" => "pwiz-setup-XXX-x86_64.msi", #pwiz-setup-3.0.3725-x86_64.msi 
	"bt23" => "pwiz-bin-darwin-x86-xgcc40-release-XXX.tar.bz2", #pwiz-bin-darwin-x86-xgcc40-release-3_0_3725.tar.bz2
	"bt17" => "pwiz-bin-linux-x86_64-gcc48-release-XXX.tar.bz2", #pwiz-bin-linux-x86_64-gcc48-release-3_0_3725.tar.bz2 
	"bt81" => "pwiz-src-XXX.tar.bz2", #pwiz-src-3_0_3725.tar.bz2 
	"bt81n" => "pwiz-src-without-v-XXX.tar.bz2", #pwiz-src-without-v-3_0_3725.tar.bz2  
	"bt211" => "libpwiz_msvc_XXX.zip", # libpwiz_msvc_3_0_3923.zip
	"bt212" => "libpwiz_src_XXX.tgz", # libpwiz_src_3_0_3923.tgz
);


#echo "hello<BR>\n";
$count_my_page = ("hitcounter_plus.txt");
#$hits = file($count_my_page);
#$counts = $hits[0];
#$counts+=1;
$fp = fopen($count_my_page,"a") or die ("Error opening file in write mode!<BR>");

$today = date("F j, Y, g:i a");
#echo $today," hello<BR>\n";

######$downloadType = $_GET['downloadtype'];
$downloadType = $_POST['downloadtype'];

$putString =  $today."\t".$_SERVER['REMOTE_HOST']."\t".$_SERVER['REMOTE_ADDR']."\t".$downloadType."\n";

#echo "hello<BR>\n";

#echo $putString,"<BR>\n";

fputs($fp , $putString);
fclose($fp);

#  file_get_contents("http://proteowizard.sourceforge.net/downloadstub.php");

if ($downloadType) { // if page is not submitted to itself echo the form
	#echo "trying to download\n";

	$winInstaller = 0;	

	$downloadTypeString = $downloadType;

	if(($downloadType == 'bt36i') || ($downloadType == 'bt83i')){
		$winInstaller = 'i';
		$downloadTypeString = rtrim($downloadType,"i");
	}
	
	if($downloadType == 'bt81n'){
		$downloadTypeString = 'bt81';
	}

	if($debug == 1){
		echo "DOWNLOAD TYPE STRING: ",$downloadTypeString,"<BR>\n";
	}

    $remoteURL = "http://teamcity.labkey.org:8080/app/rest/buildTypes/id:" . $downloadTypeString . "/builds?status=SUCCESS&count=1&guest=1";

//    $remoteURL = "http://teamcity.labkey.org:8080/app/rest/buildTypes/id:bt36/builds?status=SUCCESS&count=1&guest=1";

	if($debug == 1){
		echo "<BR>REMOTE URL: ".$remoteURL."<BR>\n";
	}

	$teamCityInfoString = file_get_contents("$remoteURL");
		 
	preg_match("/build id=\"(\d+)\"/", $teamCityInfoString, $matches);
	$buildId = $matches[1];
	$versionURL = "http://teamcity.labkey.org:8080/repository/download/" . $downloadTypeString . "/" . $buildId . ":id/VERSION?guest=1";
	
	if($debug == 1){
		echo "VERSION URL: ",$versionURL,"<BR>";
	}
	
	$versionString = file_get_contents($versionURL);
	if(!$winInstaller){
		$versionString = preg_replace('/\./', '_', $versionString);
		$downloadURL = "http://teamcity.labkey.org:8080/repository/download/" . $downloadTypeString . "/". $buildId . ":id/" . $FileNameBaseArray[$downloadType];
		
	}
	else{
		$downloadURL = "http://teamcity.labkey.org:8080/repository/download/" . $downloadTypeString . "/". $buildId . ":id/" . $FileNameBaseArray[$downloadType];
	
	}
	
	$downloadURL = preg_replace("/XXX/", $versionString, $downloadURL);
	$downloadFile = basename($downloadURL);
	$downloadURL = $downloadURL."?guest=1";

	if($debug == 1){
		echo "DOWNLOAD URL: ",$downloadURL,"<BR>\n";
	}
	
#	$artifactURL = "http://teamcity.labkey.org:8080/viewLog.html?buildId=". $buildId ."&tab=artifacts&buildTypeId=". $downloadType . "&guest=1";
#	$artifactString = file_get_contents($artifactURL);
#	$matchString = '/<a href=\'repository\/download\/' . $downloadType . '\/' . $buildId . ':id\/(.*?)\'>/';
#	preg_match($matchString,$artifactString, $matches);
#	$downloadFile = $matches[1];

#	echo "$artifactURL<BR>\n";

#	preg_match_all($matchString,$artifactString, $matches);
#	if($winInstaller == 1){
#	$downloadFile = $matches[1][1];
#	}
#	else{
#		$downloadFile = $matches[1][0];
#	}


#	if($noVendor == 1){
#		$downloadFile = str_replace('pwiz-src-','pwiz-src-without-v-',$downloadFile);
#	}
	
	
	
#	$downloadURL = "http://teamcity.labkey.org:8080/guestAuth/repository/download/" . $downloadType . "/" .  	$buildId . ":id/" . $downloadFile ;
#	echo "DOWNLOAD URL:".$downloadURL."<BR>";
if($debug == 1){
	exit();
}

    $mm_type="application/x-bzip2"; 

	header("Pragma: public");
	header("Expires: 0");
	header("Cache-Control: must-revalidate, post-check=0, pre-check=0");
	header("Cache-Control: public");
	header("Content-Description: File Transfer");
	header("Content-Type: " . $mm_type);
#	header("Content-Length: " .(string)($fs[1]) );  //for whatever reason I can't the size...
	header('Content-Disposition: attachment; filename="'.$downloadFile.'"');
	header("Content-Transfer-Encoding: binary\n");
	readfile($downloadURL); // outputs the content of the file
	exit;
}
else {
	print 'Please go <a href="http://proteowizard.sourceforge.net/downloads.shtml">HERE</a> to download ProteoWizard' ;
}

?>
