Floor
=======

This element is used as part of wall.


Floor Description
--------------------

.. image:: images/floor-description.png

* Green - Texture (background) from wall
* Blue - Texture (background) from floor
* Grey - Left floor object
* Yellow - Right floor object
* Darkred - middle floor object


Properties
-----------

.. css:property:: texture
    :type: TEXTURE-SELECTOR
    :default: null

    Links to texture as background.

.. css:property:: height
    :type: INTEGER
    :default: 0

    Height of floor. If is not set, then try to borrow from background's height.

.. css:property:: padding
    :type: INTEGER
    :default: 0

    Padding between middle floor objects.

.. css:property:: left-padding
    :type: INTEGER
    :default: padding value

    Padding between left and middle floor objects.

.. css:property:: right-padding
    :type: INTEGER
    :default: padding value

    Padding between right and middle floor objects.

.. css:property:: align
    :type: left | middle | right | justify
    :default: left

    Align of floor objects.

    .. list-table::

        * - .. figure:: images/floor-a-left.png
                :figclass: align-center

                align: left

          - .. figure:: images/floor-a-right.png
                :figclass: align-center

                align: right

        * - .. figure:: images/floor-a-middle.png
                :figclass: align-center

                align: middle

          - .. figure:: images/floor-a-justify.png
                :figclass: align-center

                align: justify

.. css:property:: middle
    :type: TEXTURE-SELECTOR
    :default: null

    Links with texture selector.

.. css:property:: top
    :type: TEXTURE-SELECTOR
    :default: null

    Links with texture selector and render only on left of floor. Ignores paddings and align

.. css:property:: bottom
    :type: TEXTURE-SELECTOR
    :default: null

    Links with texture selector and render only on right of right. Ignores paddings and align

Examples
-------------

https://pixelopolis.herokuapp.com/examples/Textures/Floor-Align
