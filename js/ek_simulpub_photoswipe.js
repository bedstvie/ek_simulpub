(function ($) {

    var openPhotoSwipe = function(photoswipeId) {
        var pswpElement = document.querySelectorAll('.pswp')[0];

        // build items array
        var items = Drupal.settings.ek_simulpub.photoswipe[photoswipeId];

        var check_mobile = $('.media-check-mobile');
        if (!$(check_mobile).length) {
            $('body').append('<div class="media-check-mobile hidden-xs hidden-sm"></div>');
            check_mobile = $('.media-check-mobile');
        }

        if ($(check_mobile).is(':visible')) {
            var items_desktop = [];
            for (i = 1; i <= items.length; i++) {
                var d = (items_desktop.length) ? items_desktop.length : 0;
                if (i % 2 === 0) {
                    //even
                    var c = i - 1;
                    var p = i - 2;
                    items_desktop[d] = {
                        html: '<div class="photoswipe-split"><div class="photoswipe-split-half" style="background-image: url('+items[p].src+');"></div><div class="photoswipe-split-half" style="background-image: url('+items[c].src+');"></div></div>',
                        title: ''
                    };
                } else if (i == (items.length)) {
                    //last odd
                    var c = i - 1;
                    items_desktop[d] = {
                        html: '<div class="photoswipe--split"><div class="photoswipe-split-full" style="background-image: url('+items[c].src+');"></div></div>',
                        title: ''
                    };
                }
            }

            // define options
            var options = {
                closeOnScroll: false,
                closeOnVerticalDrag: false,
                history: false,
                focus: false,
                showAnimationDuration: 0,
                hideAnimationDuration: 0,
                shareEl: false,
                loop: false,
                index: items_desktop.length -1
            };

            var gallery = new PhotoSwipe( pswpElement, PhotoSwipeUI_Simulpub, items_desktop, options);

        } else {
            // define options
            var options = {
                closeOnScroll: false,
                closeOnVerticalDrag: false,
                history: false,
                focus: false,
                showAnimationDuration: 0,
                hideAnimationDuration: 0,
                shareEl: false,
                loop: false,
                index: items.length -1
            };

            var gallery = new PhotoSwipe( pswpElement, PhotoSwipeUI_Simulpub, items, options);
        }

        gallery.prev = function() {
            var _currentItemIndex = gallery.getCurrentIndex();

            if (_currentItemIndex > 0) {
                gallery.goTo( _currentItemIndex - 1);
            }
        },

            gallery.next = function() {
                var totalItems = gallery.options.getNumItemsFn()
                var _currentItemIndex = gallery.getCurrentIndex();

                if (_currentItemIndex < totalItems - 1) {
                    gallery.goTo( _currentItemIndex + 1);
                }
            };

        gallery.listen('beforeChange', function() {
            var totalItems = gallery.options.getNumItemsFn()
            if (totalItems > 1) {
                $('.pswp__button--arrow--left, .pswp__button--arrow--right').show();
            }

            var _currentItemIndex = gallery.getCurrentIndex();

            if (_currentItemIndex == 0) {
                $('.pswp__button--arrow--left').hide();
            }
            if (_currentItemIndex == totalItems - 1) {
                $('.pswp__button--arrow--right').hide();
            }
        });

        gallery.init();

        if ($(check_mobile).is(':visible')) {
            var indexIndicatorDOMElement = document.querySelectorAll('.pswp__counter')[0];
            gallery.ui.updateIndexIndicator = function() {

                if (gallery.options.getNumItemsFn() === 1) {
                    indexIndicatorDOMElement.innerHTML = "1 / 1";
                }
                else {
                    indexIndicatorDOMElement.innerHTML = ((gallery.options.getNumItemsFn()-gallery.getCurrentIndex())*2-1) +
                        " - " +
                        ((gallery.options.getNumItemsFn()-gallery.getCurrentIndex())*2) +
                        gallery.options.indexIndicatorSep +
                        (gallery.options.getNumItemsFn()*2);
                }

            };

            gallery.ui.updateIndexIndicator();

        }

    };

    var setCookie = function(name, value, expires, path, domain, secure) {
        document.cookie = name + "=" + escape(value) +
            ((expires) ? "; expires=" + expires : "") +
            ((path) ? "; path=" + path : "") +
            ((domain) ? "; domain=" + domain : "") +
            ((secure) ? "; secure" : "");
    }

    var getCookie = function(name) {
        var cookie = " " + document.cookie;
        var search = " " + name + "=";
        var setStr = null;
        var offset = 0;
        var end = 0;
        if (cookie.length > 0) {
            offset = cookie.indexOf(search);
            if (offset != -1) {
                offset += search.length;
                end = cookie.indexOf(";", offset)
                if (end == -1) {
                    end = cookie.length;
                }
                setStr = unescape(cookie.substring(offset, end));
            }
        }
        return(setStr);
    }

    var getCookieArray = function(name) {
        var ids = getCookie(name);
        if(ids) {
            ids = ids.split(",");
        }
        return ids;
    }

    var chapitreMarkNew = function(chapitre) {
        var ids = getCookieArray('simulpubChapitres');
        if (ids) {
            photoswipeId = $(chapitre).attr('photoswipe');
            if (ids.indexOf(photoswipeId) == -1) {
                $(chapitre).parent().addClass('new');
            }
        }
        else {
            $(chapitre).parent().addClass('new');
        }
    }

    var isValidEmailAddress = function(emailAddress) {
        var pattern = /^([a-zA-Z0-9_.+-])+\@(([a-zA-Z0-9-])+\.)+([a-zA-Z0-9]{2,4})+$/;
        return pattern.test(emailAddress);
    }

    Drupal.behaviors.ek_simulpub_photoswipe = {
        attach: function (context, settings) {
            $('.serie-simulpub', context).each(function(){
                $(this).find('.serie-book.active a').last().each(function(){
                    chapitreMarkNew(this);
                });
            });

            $('.message .button-ico', context).each(function(){
                chapitreMarkNew(this);
            })

            $('.serie-book.active a, .message .button-ico', context).click(function(e){
                photoswipeId = $(this).attr('photoswipe');

                $('a[photoswipe="' + photoswipeId + '"]').parent().removeClass('new');

                openPhotoSwipe(photoswipeId);

                var ids = getCookieArray('simulpubChapitres');
                if(ids) {
                    if (ids.indexOf(photoswipeId) == -1) {
                        ids.push(photoswipeId);
                    }
                }
                else {
                    ids = [photoswipeId];
                }

                var cookieDate = new Date;
                cookieDate.setFullYear(cookieDate.getFullYear( ) +10);
                setCookie('simulpubChapitres', ids, cookieDate.toGMTString( ), '/');

                e.preventDefault();
            });

            $('.subscribe-form .button', context).click(function(e){
                $('.subscribe-form .error').remove();

                var parent = $(this).parent('#mc_embed_signup_scroll');
                var mail = $('.email', parent).val();
                if (!isValidEmailAddress(mail)) {
                    var error = $('<div/>', {
                        class: 'error'
                    });
                    var errorMsg = $('<div/>', {
                        text: Drupal.t('The format of your email address is not valid.'),
                        class: 'error-msg'
                    });
                    error.append(errorMsg);
                    parent.after(error);
                    e.preventDefault();
                }
            });
        }
    };
})(jQuery);