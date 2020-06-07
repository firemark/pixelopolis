Structure
=======================================

Hierarchy
----------

.. graphviz::

   digraph {
     world -> container
     container[label="draw-obj\n(container)"]
     world -> "draw-obj\n(another container)"
     container -> "draw-obj\n(nested container)"
     container -> draw_obj
     draw_obj[label="draw-obj\n(basic shape)"]
     container -> "another draw-obj"
     draw_obj -> "another wall"
     draw_obj -> wall
     wall -> "another floor"
     wall -> floor
     wall -> texture
     floor -> texture
   }

Real example
--------------

.. image:: images/structure.png


Elements
------------

World
^^^^^^^^^^

First element in file, describes width, height and select first draw-obj as root

Draw-Obj
^^^^^^^^^^

3D object on the image, usually has links to walls and another draw-obj. Each draw-obj has 4 primal properties:

* Width (in pixels/percents, x axis)
* Depth (in pixels/percents, y axis)
* Height (in pixels/percents, z axis)
* Rotate (in degress, rotate only XY axis)

We have a two categories: Container and Basic shape.

Container
""""""""""

Can have a nested and many another draw-obj. Can have a additional properties like padding, align nested objects etc.

Basic Shape
"""""""""""

Simple shape like a Cube or Dome. Basic shape has walls.

Wall
^^^^^
Image put on the side in basic shape. Wall can have floors, background and particles

Floor
^^^^^^

Floor can have background and another textures (like decoration or windows or door etc.)

Texture
^^^^^^^^

Have only filepath to PNG file or one solid color.

TODO
