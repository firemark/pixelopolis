Cube
========

Properties
-----------

.. css:property:: display
    :type: cube
    :required:


.. css:property:: wall
    :type: SELECTOR-TO-WALL | null


.. css:property:: roof
    :type: SELECTOR-TO-WALL | null


Example
--------

.. code-block:: scss

    house {
        display: cube;
        width: 100;
        depth: 100;
        height: 100;

        roof: wall.roof;
        wall: wall.brick;
    }
