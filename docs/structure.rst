Structure
=======================================

.. graphviz::

   digraph {
     world -> container
     container -> "another container"
     container -> "draw-obj"
     container -> "another draw-obj"
     "draw-obj" -> wall
     "draw-obj" -> "another wall"
     wall -> floor
     wall -> "another floor"
     wall -> texture
     floor -> texture
   }

.. image:: images/structure.png
