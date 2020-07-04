Fillers
=========

.. image:: images/series.png
    :alt: fillers

Renders many draw-objs in specified direction. Unline series, filler try to fill specified space with draw-objs.

Properties
-----------

.. css:property:: display
    :type: v-filler | d-filler | h-filler
    :required:

    :type v-filler: filler on X (width) axis
    :type d-filler: filler on Y (depth) axis
    :type h-filler: filler on Z (height) axis

    .. list-table::

        * - .. figure:: images/v-direction.png
                :figclass: align-center

                v-series

          - .. figure:: images/d-direction.png
                :figclass: align-center

                d-series

          - .. figure:: images/h-direction.png
                :figclass: align-center

                h-series

.. css:property:: body
    :type: SELECTOR-TO-DRAW-OBJ

    Link to draw-obj to fill the specified space.

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


.. css:property:: (v-filler) justify
    :type: start | end | center | random

.. css:property:: (d-filler) justify
    :type: start | end | center | random

.. css:property:: (h-filler) justify
    :type: JUSTIFY-X, JUSTIFY-Y | center | random
    :default: start

    Align of objects for the rest axis.

    * for v-filler: align of Y (depth) axis
    * for d-filler: align of X (width) axis
    * for h-filler: align of Y and X axis (with two arguments)

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
        display: v-filler;
        width: 300;
        depth: 100;
        align: stretch;

        body: house.red;
    }

You can try on https://pixelopolis.herokuapp.com/examples/Containers/Series
