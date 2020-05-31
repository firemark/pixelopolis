Square Fence
=============

.. image:: images/square-fence.png
    :alt: Square fence

Renders many draw-objs in specified direction. Unline series, filler try to fill specified space with draw-objs.

Properties
-----------

.. css:property:: display
    :type: square-fence
    :required:

.. css:property:: body
    :type: SELECTOR-TO-DRAW-OBJ

    Links to draw-obj which be rendered in the center.

    .. image:: images/square-fence-body.png
        :alt: Square fence

.. css:property:: edge
    :type: SELECTOR-TO-DRAW-OBJ

    Links to draw-obj which be rendered in four edges.

    .. image:: images/square-fence-edge.png
        :alt: Square fence

.. css:property:: corner
    :type: SELECTOR-TO-DRAW-OBJ

    Links to draw-obj which be rendered in four corners.

    .. image:: images/square-fence-corner.png
        :alt: Square fence

.. css:property:: body-justify
    :type: JUSTIFY-X, JUSTIFY-Y | center | random
    :default: start, start

    align of body object.

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

.. css:property:: edge-justify
    :type: start | end | center | random
    :default: start

    align of edge object.

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

.. css:property:: edge-size
    :type: INTEGER
    :default: 0

    depth (to the center of container) size of edge and corner.

Example
--------

.. code-block:: scss

    prison {
        display: square-fence;
        width: 500;
        depth: 500;

        body: building;
        edge: fence;
        corner: tower;

        body-justify: center;
        edge-size: 40;
    }

You can try in on https://pixelopolis.herokuapp.com/examples/Containers/Square-Fence
