Rule
=====

.. code-block:: scss

    SELECTOR {
        property: <OBJ>;
        property: <OBJ1>, <OBJ2>; /* array of N elements */
        property: <OBJ>;

        /* ... */

        CHILD-SELECTOR {
            property: <OBJ>;
            /* ... */
        }
        /* ... */
    }


Properties
----------------

.. code-block:: scss

    SELECTOR {
        property1: OBJ;
        property2: OBJ;
    }


Selectors
---------------

By Element name
^^^^^^^^^^^^^^^^^

.. code-block:: scss

    element { /*...properties...*/ }


By Class
^^^^^^^^^^^^^^

.. code-block:: scss

    .class { /*...properties...*/ }

Element can have many classes

.. code-block:: scss

    .class1.class2 { /*...properties...*/ }

We can join with element name:

.. code-block:: scss

    element.class { /*...properties...*/ }
    element.class1.class2 { /*...properties...*/ }


Parent operator
^^^^^^^^^^^^^^^^

.. code-block:: scss

    parent > element { /*...properties...*/ }
    parent > .class { /*...properties...*/ }
    parent > element.class { /*...properties...*/ }
    parent > el1 > el2 { /*...properties...*/ }


Greedy parent
^^^^^^^^^^^^^^^

.. code-block:: scss

    parent element { /*...properties...*/ }


In example, if you have these structure:

.. graphviz::

   digraph {
     parent -> "child.first"
     parent -> "child.second"
     "child.first" -> grandchild
   }

And one rule:

.. code-block:: scss

    parent grandchild { color: red; }

Then grandchild should have red color.


Many Selectors
----------------

.. code-block:: scss

    selector1, selector2, selector3 {
        /*...properties...*/
    }


Merge Rules
-------------

.. code-block:: scss

    draw-obj { wall: wall.brick.red; }
    wall.brick { property-brick: OBJ2; }
    wall.red { property-red: OBJ1; }

It’s equality this same with:

.. code-block:: scss

    draw-obj { wall: wall.brick.red; }
    wall.brick.red {
        property-brick: OBJ2;
        property-red: OBJ1;
    }

Warning, order of rules is neccesary, bottom rule overwrite properites from top rule, in example:

.. code-block:: scss

    draw-obj { wall: wall.brick.grass; }
    wall.brick { color: red; }
    wall.grass { color: green; }

It’s equality this same with:

.. code-block:: scss

    draw-obj { wall: wall.brick.grass; }
    wall.brick.grass { color: green; }

