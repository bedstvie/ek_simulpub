(function ($) {

    /**
     * Find the row the mouse is currently over. This row is then taken and swapped
     * with the one being dragged.
     *
     * @param x
     *   The x coordinate of the mouse on the page (not the screen).
     * @param y
     *   The y coordinate of the mouse on the page (not the screen).
     */
    Drupal.tableDrag.prototype.findDropTargetRow = function (x, y) {
        var rows = $(this.table.tBodies[0].rows).not(':hidden');
        for (var n = 0; n < rows.length; n++) {
            var row = rows[n];
            var indentDiff = 0;
            var rowY = $(row).offset().top;

            var rowX = $(row).offset().left;
            // Because Safari does not report offsetHeight on table rows, but does on
            // table cells, grab the firstChild of the row and use that instead.
            // http://jacob.peargrove.com/blog/2006/technical/table-row-offsettop-bug-in-safari.
            if (row.offsetHeight == 0) {
                var rowHeight = parseInt(row.firstChild.offsetHeight, 10) / 2;
            }
            // Other browsers.
            else {
                var rowHeight = parseInt(row.offsetHeight, 10) / 2;
            }

            if (row.offsetHeight == 0) {
                var rowWidth = parseInt(row.firstChild.offsetWidth, 10) / 2;
            }
            // Other browsers.
            else {
                var rowWidth = parseInt(row.offsetWidth, 10) / 2;
            }

            // Because we always insert before, we need to offset the height a bit.
            if ((y > (rowY - rowHeight)) && (y < (rowY + rowHeight))) {
                if ((x > (rowX - rowWidth)) && (x < (rowX + rowWidth))) {
                    if(!$(row).hasClass('draggable')) {
                        return null;
                    }

                    if (this.indentEnabled) {
                        // Check that this row is not a child of the row being dragged.
                        for (var n in this.rowObject.group) {
                            if (this.rowObject.group[n] == row) {
                                return null;
                            }
                        }
                    }
                    else {
                        // Do not allow a row to be swapped with itself.
                        if (row == this.rowObject.element) {
                            return null;
                        }
                    }

                    // Check that swapping with this row is allowed.
                    if (!this.rowObject.isValidSwap(row)) {
                        return null;
                    }

                    // We may have found the row the mouse just passed over, but it doesn't
                    // take into account hidden rows. Skip backwards until we find a draggable
                    // row.
                    while ($(row).is(':hidden') && $(row).prev('tr').is(':hidden')) {
                        row = $(row).prev('tr').get(0);
                    }
                    return row;
                }
            }
        }
        return null;
    };

})(jQuery);