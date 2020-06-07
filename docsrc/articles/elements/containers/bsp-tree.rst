BSP Tree
=========

.. image:: images/bsp.png

**BSP** means *Binary Space partitioning*. It's an advanced container which parts area into smaller areas randomically.

The algorithm of partitioning is here: http://www.roguebasin.com/index.php?title=Basic_BSP_Dungeon_generation


Properties
-----------

.. css:property:: display
    :type: bsp-tree
    :required:

.. css:property:: body
    :type: DRAW-OBJ-SELECTOR
    :default: null

    links subareas with draw-obj

.. css:property:: padding
    :type: INTEGER
    :default: 0

    padding between subareas.

    .. list-table::

        * - .. figure:: images/bsp-p-5.png
             :figclass: align-center

             padding: 5

          - .. figure:: images/bsp-p-20.png
             :figclass: align-center

             padding: 20

.. css:property:: justify
    :type: JUSTIFY-X, JUSTIFY-Y | center | random
    :default: start, start

    align of body element

    .. list-table::

        * - .. figure:: images/bsp-j-start.png
             :figclass: align-center

             justify: start, start

          - .. figure:: images/bsp-j-end.png
             :figclass: align-center

             justify: end, end

        * - .. figure:: images/bsp-j-center.png
             :figclass: align-center

             justify: center

          - .. figure:: images/bsp-j-random.png
             :figclass: align-center

             justify: random

.. css:property:: iterations-count
    :type: INTEGER
    :default: 3

    How many iterations will be executed to parts area into two smaller areas. Each iteration split each area to two smaller areas (vertically or horizontally). Max number of areas is ``(iterations-count - 1) ** 2`` where ``**`` is a power operator

    If area is too small to split (with *min-width* and *min-height*) then splitting will be stopped.

    If iteration is greater than *iterations-count* and width of area is greater than *max-width* (or vice-versa with height), then area should split into two areas again.

    .. list-table::

        * - .. figure:: images/bsp-i-1.png
             :figclass: align-center

             iterations-count: 1

          - .. figure:: images/bsp-i-2.png
             :figclass: align-center

             iterations-count: 2

        * - .. figure:: images/bsp-i-3.png
             :figclass: align-center

             iterations-count: 3

          - .. figure:: images/bsp-i-4.png
             :figclass: align-center

             iterations-count: 4

.. css:property:: min-width
    :type: INTEGER
    :default: 0

    If width of subarea (+ padding) is smaller than min-width then splitting will be stopped

.. css:property:: min-height
    :type: INTEGER
    :default: 0

    If height of subarea (+ padding) is smaller than min-height then splitting will be stopped

.. css:property:: max-width
    :type: INTEGER
    :default: width of subarea

    If iteration is greater than *iterations-count* and width of area if greater than *max-width*, then area should split into two areas again.

.. css:property:: max-height
    :type: INTEGER
    :default: height of subarea

    If iteration is greater than *iterations-count* and height of area if greater than *max-height*, then area should split into two areas again.


Example
----------

.. code-block:: scss

    district {
        display: bsp-tree;
        body: house;
        iterations-count: 4;
        padding: 10;
        justify: center;
        width: 500;
        depth: 500;
        min-width: 100;
        min-height: 100;
    }

You can try on https://pixelopolis.herokuapp.com/examples/Containers/Bsp-tree
