Wall
=====

.. image:: images/wall.png

Image put on the side in basic shape. Wall can have floors, background and particles

Description of properties
--------------------------


.. image:: images/wall-description.png

* *Green* - texture (background)
* *Blue* - top floor
* *Red* - middle floor
* *Bottom* - bottom floor


Properties
-----------

.. css:property:: texture
    :type: TEXTURE-SELECTOR
    :default: null

    links to texture as background

.. css:property:: padding
    :type: INTEGER
    :default: 0

    padding between middle floors.

.. css:property:: middle
    :type: FLOOR-SELECTOR
    :default: null

    Links with floor selector.

.. css:property:: top
    :type: FLOOR-SELECTOR
    :default: null

    Links with floor selector and render only on top of wall. Ignores paddings and align

.. css:property:: bottom
    :type: FLOOR-SELECTOR
    :default: null

    Links with floor selector and render only on bottom of wall. Ignores paddings and align

.. css:property:: points-texture
    :type: TEXTURE-SELECTOR
    :default: null

    If is set, then draw randomically particles (with this texture) on the wall. This property required *points-density*.

.. css:property:: points-density
    :type: PERCENTAGE
    :default: 0%

    How many area should be covered relative to value. This property required *points-texture*.

    .. list-table::

        * - .. figure:: images/wall-points-5.png
                :figclass: align-center

                points-density: 5%

          - .. figure:: images/wall-points-25.png
                :figclass: align-center

                points-density: 25%

Examples
---------

* https://pixelopolis.herokuapp.com/examples/Textures/Textures
* https://pixelopolis.herokuapp.com/examples/Textures/Floor-Align
* https://pixelopolis.herokuapp.com/examples/Textures/Points
