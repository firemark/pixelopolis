Draw Objs
==========

3D Objects rendered on the image. Draw Objs can have textures and another draw objects (children).

.. toctree::
   :maxdepth: 2
   :caption: Contents:

   basic-shapes/index
   containers/index


Primal properties
--------------------

Each Draw Obj has a few primal properties:


.. list-table:: Properties
    :header-rows: 1

    * - Name
      - Type
      - Description
      - Default

    * - display
      - draw-obj type name
      - select type of draw-obj,
        if display is incorrect then draw-obj will not be rendered
      - element name from selector's rule

    * - width
      - INTEGER / PERCENTAGE
      - size on X axis. Percentage borrows from parent draw-obj
      - 0

    * - depth
      - INTEGER / PERCENTAGE
      - size on Y axis. Percentage borrows from parent draw-obj
      - 0

    * - height
      - INTEGER / PERCENTAGE
      - size on Z axis. Percentage borrows from parent draw-obj
      - 0

    * - rotate
      - INTEGER (degrees)
      - rotation values on X-Y axis
      - 0

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
