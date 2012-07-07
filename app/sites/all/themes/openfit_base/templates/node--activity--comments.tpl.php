<article id="article-<?php print $node->nid; ?>" class="<?php print $classes; ?> clearfix"<?php print $attributes; ?>>

  <?php print $user_picture; ?>

  <div<?php print $content_attributes; ?>>
  <?php
    hide($content['comments']);
    print render($content);
  ?>
  </div>

  <?php print render($content['comments']); ?>
</article>
