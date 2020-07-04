Pyramid
========

.. image:: images/pyramid.png
    :alt: pyramid

Just a pyramid shaped object. Excellent for roofs and… pyramids.

Properties
-----------

.. css:property:: display
    :type: pyramid
    :required:


.. css:property:: wall
    :type: SELECTOR-TO-WALL | null
    :default: null

    Wall of side triangles.


Example
--------

.. code-block:: scss

    pyramid-roof {
        display: pyramid;
        width: 100;
        depth: 100;
        height: 100;

        wall: wall.bricik;
    }

You can try in on https://pixelopolis.herokuapp.com/examples/Basic-shapes/Pyramid
