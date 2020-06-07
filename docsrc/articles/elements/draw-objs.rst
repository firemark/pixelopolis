Draw Objs
==========

3D Objects render on the image. Draw Objs can have textures and another draw objects (children). We have two types of draw objects - containers and basic shapes.

.. toctree::
   :maxdepth: 2
   :caption: Contents:

   basic-shapes/index
   containers/index


Primal properties
--------------------

Each Draw Obj has a few primal properties:

.. css:property:: display
    :type: DISPLAY-TYPE
    :default: element's name

    type of draw-obj to howto display draw-obj (in ex. cube, triangle, dome, etc). Each display's type has another properties and another shape.

    If display is incorrect then draw-obj will not be rendered.


.. css:property:: width
    :type: INTEGER | PERCENTAGE
    :default: 0

    size on X axis.

    :type INTEGER: width in pixels
    :type PERCENTAGE: relative width to the parent object


.. css:property:: depth
    :type: INTEGER | PERCENTAGE
    :default: 0

    size on Y axis.

    :type INTEGER: depth in pixels
    :type PERCENTAGE: relative depth to the parent object

.. css:property:: height
    :type: INTEGER | PERCENTAGE
    :default: 0

    size on Z axis.

    :type INTEGER: height in pixels
    :type PERCENTAGE: relative height to the parent object

.. css::property:: rotate
    :type: INTEGER
    :default: 0

    rotation values on X-Y axis


Examples
-----------

.. code-block:: scss

    draw-obj {
        display: cube;
        width: 200;
        depth: 100%;
        height: 100;
        rotate: 90;
    }
