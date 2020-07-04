Cone
=====

.. image:: images/cone.png
    :alt: cone


Very fancy for tower's roof.


Properties
-----------

.. css:property:: display
    :type: cone
    :required:

.. css:property:: roof
    :type: SELECTOR-TO-WALL | null
    :default: null

    Wall of side faces stretched vertically.

.. css:property:: sides
    :type: INTEGER
    :default: 8

    Number of cone sides. If have more sides then is more circular:

    .. list-table::

        * - .. figure:: images/cone-6-sides.png

                sides: 6

          - .. figure:: images/cone-12-sides.png

                sides: 12


Example
--------

.. code-block:: scss

    roof {
        display: cone;
        width: 100;
        depth: 100;
        height: 100;

        roof: wall.roof;
        sides: 32;
    }

You can try in on https://pixelopolis.herokuapp.com/examples/Basic-shapes/Cone
