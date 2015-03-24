<?php
$file  = $variables['node']->file; ?>

<div class="tripal_file-teaser tripal-teaser"> 
  <div class="tripal-file-teaser-title tripal-teaser-title"><?php 
    print l("<i>$file->uniquename", "node/$node->nid", array('html' => TRUE));?>
  </div>
  <div class="tripal-file-teaser-text tripal-teaser-text"> <?php
    print substr($file->description, 0, 650);
    if (strlen($file->description) > 650) {
      print "... " . l("[more]", "node/$node->nid");
    } ?>
  </div>
</div>