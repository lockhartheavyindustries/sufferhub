<article id="article-<?php print $node->nid; ?>" class="<?php print $classes; ?> clearfix"<?php print $attributes; ?>>

  <?php print $user_picture; ?>

  <div<?php print $content_attributes; ?>>
  <?php
    print render($content);
  ?>
  </div>

</article>
