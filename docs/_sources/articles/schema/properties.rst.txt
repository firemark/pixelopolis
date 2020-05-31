Properties
==============

Number
-------

Just simple number. Now we support only integers

.. code-block:: scss

    rule {
        number: 100;
    }

We support mathematical operators:

.. code-block:: scss

    rule {
        math: 100 * (2 + 2);
    }

Percents
----------

Used mainly for properties like width, depth and height. Percents inherits from parent sizes.

.. code-block:: css

    rule {
        width: 100%;
        height: 50%;
    }

.. todo::

    Support math operators like for number


Strings
---------

Used mainly for urls to textures.

.. code-block:: scss

    tex.brick {
        texture: "path-to-texture";
    }

.. todo::

    Support string concatenations


Selectors
----------

Used to link to another element like in cube to join walls:

.. code-block:: scss

    cube {
        roof: wall.roof;
        wall: wall.brick.red;
    }

Or to select textures in a wall:

.. code-block:: scss

    wall.brick {
        texture: tex.brick;
    }

Or used in containers as array of selector:

.. code-block:: scss

    v-series {
        body:
            house.modern,
            house.old,
            factory.brick;
    }

.. warning::

     is possible only to send element name and/or classes, is not possible to join with parent operator or another pseudoclasses

.. todo::

    Support class concatenations


Functions
----------

Is possible to use special functions in properties.


random
^^^^^^^^

Randoms integer between A and B

Usage:

.. code-block:: scss

    random(A, B)

Example:

.. code-block:: scss

    /* randoms number between 0 and 50 */
    rule {
        random-number: random(0, 50);
    }

choice
^^^^^^^^^

Randoms one element from arguments list

Usage:

.. code-block:: scss

    choice(arg1, arg2, ..., argN)

Example:

.. code-block:: scss

    /* choice only one element from list of arguments */
    rule {
        random_obj: choice(obj1, obj2, obj3);
    }
