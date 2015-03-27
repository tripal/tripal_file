
<?php
$incomingNode = $variables['node'];
$filesToAdd = $variables['files'];

dpm($filesToAdd);

if(is_array($filesToAdd)) {
	foreach($filesToAdd as $file) {
		print "$file->uri ";
		print "$file->description";
	} 
} 
else { ?>
<div>I have the file, but I have no way of displaying it... </div>
<?php 
dpm($filesToAdd->uri);	
drupal_set_message("weeee");
	 print tripal_set_message( $filesToAdd->uri);
	 print tripal_set_message(  $filesToAdd->description) ;		
	 }
?>
