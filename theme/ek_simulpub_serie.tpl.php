<div class="serie-simulpub">
    <div class="serie-image">
        <div class="picture-holder">
            <div class="picture-frame">
              <?php print render($left_image); ?>
            </div>
        </div>
    </div>
  <?php if ($right_image): ?>
      <div class="series-img">
          <a href="<?php print $serie_url; ?>">
              <div class="picture-holder">
                  <div class="picture-frame">
                    <?php print render($right_image); ?>
                  </div>
              </div>
              <div class="data">
                  <span>découvrir la série</span>
              </div>
          </a>
      </div>
  <?php endif; ?>
    <div class="info">
        <h2><?php print $serie_title; ?></h2>
        <div class="publications"><?php print $chapitre_counts; ?></div>
        <div class="message">
          <?php if ($new_chapitres): ?>
              <a href="#" class="button-ico" photoswipe="<?php print $new_chapitres['id']; ?>"><span><?php print $new_chapitres['number']; ?></span></a>
          <?php endif; ?>
        </div>
        <div class="subscribe">
            <div class="description"><?php print $next_info; ?></div>
            <div class="subscribe-form">
              <?php print $subscribe_form; ?>
            </div>
        </div>
    </div>
  <?php if ($tomes): ?>
      <div class="serie-books-expand">
          <span><?php print $show_chapitres; ?></span>
      </div>
      <div class="serie-books">
        <?php foreach ($tomes as $key_tome => $tome): ?>
            <h4><?php print $section_titles[$key_tome]; ?></h4>
            <div class="serie-books-list">
              <?php foreach ($tome as $chapitre): ?>
                  <div <?php print drupal_attributes($chapitre['attributes']) ?>>
                    <?php if (isset($chapitre['photoswipe'])): ?>
                      <a href="#" photoswipe="<?php print $chapitre['photoswipe']; ?>">
                        <?php endif; ?>
                          <span><?php print $chapitre['data']['num']; ?></span>
                        <?php if (isset($chapitre['photoswipe'])): ?>
                      </a>
                  <?php endif; ?>
                  </div>
              <?php endforeach; ?>
            </div>
        <?php endforeach; ?>
      </div>
  <?php endif; ?>
</div>