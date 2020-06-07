Texture
==========

.. image:: images/texture.png

Just, texture. To render walls, floors or particles. It can be as PNG file or solid colid.


Properties
-----------

.. css:property:: color
    :type: #RGB | #RRGGBB | rgb(red, green, blue)

    Just solid color. Fill whole wall or floor.

    :type #RRGGBB:
        Color in HEX format. More info: https://www.w3schools.com/css/css_colors_hex.asp

    :type #RGB:
        shortcut HEX format. Transforms ``#123`` into ``#112233``

    :type rgb(red, green, blue):
        Color in decimal format. Each color is a value from 0 to 255. More info: https://www.w3schools.com/css/css_colors_rgb.asp

.. css:property:: texture
    :type: STRING

    Relative filepath to PNG file. Transparent color is not alpha channel (alpha channel is ignored, sorry ``;_;``) but is a ``#FF0000`` color.


Examples
--------

.. code-block:: scss

    tex.color {
        color: #FFF;
    }

    tex.texture {
        texture: "relative/filepath.png";
    }

More examples: https://pixelopolis.herokuapp.com/examples/Textures/Textures
