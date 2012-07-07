<article class="<?php print $classes; ?> clearfix"<?php print $attributes; ?>>

  <?php if ($links = render($content['links'])): ?>
    <nav class="comment-actions"><?php print $links; ?></nav>
  <?php endif; ?>

  <?php print $picture; ?>

  <header>
    <p class="comment-submitted"><?php print $submitted; ?><p>
  </header>

  <div<?php print $content_attributes; ?>>
    <?php
      hide($content['links']);
      print render($content);
    ?>
  </div>

</article>
