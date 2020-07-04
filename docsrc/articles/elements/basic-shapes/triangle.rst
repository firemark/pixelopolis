Triangle
==========

.. image:: images/triangle.png
    :alt: triangle

Just a triangle shaped roof.

Properties
-----------

.. css:property:: display
    :type: triangle
    :required:


.. css:property:: wall
    :type: SELECTOR-TO-WALL | null
    :default: null

    Wall of side triangles.


.. css:property:: roof
    :type: SELECTOR-TO-WALL | null
    :default: null

    Wall of top faces.


Example
--------

.. code-block:: scss

    triangle-roof {
        display: triangle;
        width: 100;
        depth: 100;
        height: 100;

        roof: wall.roof;
        wall: wall.brick;
    }

You can try in on https://pixelopolis.herokuapp.com/examples/Basic-shapes/Triangle
