Series
========

.. image:: images/series.png
    :alt: series

Renders selected draw-objs in specified direction. The total width of series is a sum of selected draw-objs width and padding between objects.

Properties
-----------

.. css:property:: display
    :type: v-series | d-series | h-series
    :required:

    :type v-series: series on X (width) axis
    :type d-series: series on Y (depth) axis
    :type h-series: series on Z (height) axis

    .. list-table::

        * - .. figure:: images/v-direction.png

                v-series

          - .. figure:: images/d-direction.png

                d-series

          - .. figure:: images/h-direction.png

                h-series

.. css:property:: body
    :type: SELECTOR-TO-DRAW-OBJ, SELECTOR-TO-DRAW-OBJ, …
    :default: null

    Links to draw-objs

.. css:property:: padding
    :type: INTEGER
    :default: 0

    Minimal padding between objects.

.. css:property:: align
    :type: start | end | center | stretch
    :default: start

    Align of objects in specified axis.

    For this property is neccessary to set width/depth/height (depends on type)

    .. list-table::

        * - .. figure:: images/align-start.png

                align: start

          - .. figure:: images/align-end.png

                align: end

        * - .. figure:: images/align-center.png

                align: center

          - .. figure:: images/align-stretch.png

                align: stretch


.. css:property:: (v-series) justify
    :type: start | end | center | random

.. css:property:: (d-series) justify
    :type: start | end | center | random

.. css:property:: (h-series) justify
    :type: JUSTIFY-X, JUSTIFY-Y | center | random
    :default: start

    align of objects for the rest axis.

    * for v-series: align of Y (depth) axis
    * for d-series: align of X (width) axis
    * for h-series: align of Y and X axis (with two arguments)

    .. list-table::

        * - .. figure:: images/series-justify-start.png

                justify: start

          - .. figure:: images/series-justify-end.png

                justify: end

        * - .. figure:: images/series-justify-center.png

                justify: center

          - .. figure:: images/series-justify-random.png

                justify: random

        * - .. figure:: images/series-justify-start-center.png

                justify: start, center

          - .. figure:: images/series-justify-end-center.png

                justify: end, center

Example
--------

.. code-block:: scss

    row {
        display: v-series;
        depth: 100;
        align: center;

        body: house.red, house.white, house.red;
    }

You can try in on https://pixelopolis.herokuapp.com/examples/Containers/Series
