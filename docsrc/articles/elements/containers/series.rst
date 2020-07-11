Series
========

.. image:: images/series.png
    :alt: series

Renders selected draw-objs in specified direction. The total width of series is a sum of selected draw-objs width and padding between objects.

Properties
-----------

.. css:property:: display
    :type: x-series | y-series | z-series
    :required:

    :type x-series: series on X (width) axis
    :type y-series: series on Y (depth) axis
    :type z-series: series on Z (height) axis

    .. list-table::

        * - .. figure:: images/v-direction.png
                :figclass: align-center

                x-series

          - .. figure:: images/d-direction.png
                :figclass: align-center

                y-series

          - .. figure:: images/h-direction.png
                :figclass: align-center

                z-series

.. css:property:: body
    :type: SELECTOR-TO-DRAW-OBJ, SELECTOR-TO-DRAW-OBJ, …
    :default: null

    Link to draw-objs.

.. css:property:: padding
    :type: INTEGER
    :default: 0

    Minimal padding between objects.

.. css:property:: align
    :type: start | end | center | stretch
    :default: start

    Align of objects in specified axis.

    For this property is neccessary to set width/depth/height (depends on type).

    .. list-table::

        * - .. figure:: images/align-start.png
                :figclass: align-center

                align: start

          - .. figure:: images/align-end.png
                :figclass: align-center

                align: end

        * - .. figure:: images/align-center.png
                :figclass: align-center

                align: center

          - .. figure:: images/align-stretch.png
                :figclass: align-center

                align: stretch


.. css:property:: (x-series) justify
    :type: start | end | center | random

.. css:property:: (y-series) justify
    :type: start | end | center | random

.. css:property:: (z-series) justify
    :type: JUSTIFY-X, JUSTIFY-Y | center | random
    :default: start

    align of objects for the rest axis.

    * for x-series: align of Y (depth) axis
    * for y-series: align of X (width) axis
    * for z-series: align of Y and X axis (with two arguments)

    .. list-table::

        * - .. figure:: images/series-justify-start.png
                :figclass: align-center

                justify: start

          - .. figure:: images/series-justify-end.png
                :figclass: align-center

                justify: end

        * - .. figure:: images/series-justify-center.png
                :figclass: align-center

                justify: center

          - .. figure:: images/series-justify-random.png
                :figclass: align-center

                justify: random

        * - .. figure:: images/series-justify-start-center.png
                :figclass: align-center

                justify: start, center

          - .. figure:: images/series-justify-end-center.png
                :figclass: align-center

                justify: end, center

Example
--------

.. code-block:: scss

    row {
        display: x-series;
        depth: 100;
        align: center;

        body: house.red, house.white, house.red;
    }

You can try on https://pixelopolis.herokuapp.com/examples/Containers/Series
