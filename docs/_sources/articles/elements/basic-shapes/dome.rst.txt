Dome
======

.. image:: images/dome.png
    :alt: dome

Great for temples or parlaments


Properties
-----------

.. css:property:: display
    :type: dome
    :required:


.. css:property:: roof
    :type: SELECTOR-TO-WALL | null
    :default: null

    Wall of side faces stretched vertically.


.. css:property:: vertical-sides
    :type: INTEGER
    :default: 8

    Number of vertical sides:

    .. list-table::

        * - .. figure:: images/dome-v-32.png

               vertical-sides: 32

          - .. figure:: images/dome-v-6.png

                vertical-sides: 6

.. css:property:: horizontal-sides
    :type: INTEGER
    :default: 8

    Number of horizontal sides:

    .. list-table::

        * - .. figure:: images/dome-h-6.png

                horizontal-sides: 6

          - .. figure:: images/dome-h-2.png

                horizontal-sides: 2

Example
--------

.. code-block:: scss

    dome-roof {
        display: dome;
        width: 100;
        depth: 100;
        height: 100;

        wall: wall.bricik;
    }

You can try in on https://pixelopolis.herokuapp.com/examples/Basic-shapes/Dome
