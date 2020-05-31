Cylinder
=========

.. image:: images/cylinder.png
    :alt: cylinder


Great for body's tower or ancient area.


Properties
-----------

.. css:property:: display
    :type: cone
    :required:

.. css:property:: roof
    :type: SELECTOR-TO-WALL | null
    :default: null

    wall of top disk.

.. css:property:: wall
    :type: SELECTOR-TO-WALL | null
    :default: null

    wall of side faces stretched vertically.

.. css:property:: sides
    :type: INTEGER
    :default: 8

    number of cylinder sides. If have more sides then is more circular:

    .. list-table::

        * - .. figure:: images/cylinder-6-sides.png
                :alt: cylinder 6 sides

                sides: 6

          - .. figure:: images/cylinder-32-sides.png
                :alt: cylinder 32 sides

                sides: 32


Example
--------

.. code-block:: scss

    ancient-area {
        display: cylinder;
        width: 100;
        depth: 100;
        height: 100;

        roof: wall.roof;
        wall: wall.brick;
        sides: 32;
    }

You can try in on https://pixelopolis.herokuapp.com/examples/Basic-shapes/Cylinder
