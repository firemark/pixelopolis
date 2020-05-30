Rule
=====

Rules describe elements of scenario like which objects you want in scenario
or which textures with options are you need.

Language is very easy - is based mostly on CSS language (we have a little exceptions like math operators or nested rules). In this language, we can link to another rules (in example, cube can link to wall rule who links to texture rule), and is enough to describe city-like scenarios.

The example of rule is below:

.. code-block:: scss

    SELECTOR {
        property: <VALUE>;
        property: <VALUE>, <VALUE>; /* array of N elements */
        property: <VALUE>;

        /* ... */

        CHILD-SELECTOR {
            property: <VALUE>;
            /* ... */
        }
        /* ... */
    }

We have a few elements:

* **Selector** - it's a query with requirements to affect properties to element. In example we can make rule only for *house* object or we can make rule for *roof* with *brick* class. More details are in :ref:`selectors` section.
* **Property** - name of attribute of rule. With property we can decribe dimension of object, set texture path or links to another elements.
* **Value** - value of property, can be an integer, string or link to another rule. Is possible to use functions.
* **Child** - nested rules for children of overriding rule - very usefull for container where we have a many elements. Of course, child can have a another children.

.. _selectors:

Selectors
---------------

Selectors are powerfull syntax - we can describe elements with specified classes and makes relations with parent ↔ children of element. More details are below:

By Element name
^^^^^^^^^^^^^^^^^

Just element name - can be everything - *cube*, *wall*, *texture*, *house*, *district* or even *fancy-cow*. Rule, like element can only have one element's name.

.. code-block:: scss

    element { /*...properties...*/ }


By Class
^^^^^^^^^^^^^^

Element can have variations - in example, *house* can cave white or yellow wals, so we can write generic rule for *house* and write two rules for classes *.white* and *.yellow* to describe walls of our house.

Class starts with dot (like in CSS syntax).

.. code-block:: scss

    .white { /*...properties...*/ }
    .yellow { /*...properties...*/ }

Rule can have many classes (remember about no-space between classes - it's neccesary!)

.. code-block:: scss

    .class1.class2 { /*...properties...*/ }

We can join with element's name:

.. code-block:: scss

    house.yellow { /*...properties...*/ }
    element.class1.class2 { /*...properties...*/ }


Parent operator
^^^^^^^^^^^^^^^^

Containers can have a children - in example, we can have a district with N buildings, and we can have a *oldtown* district and *modern* district. So we can change shapes and textures of buldings depends of the parent element.

.. code-block:: scss

    parent > element { /*...properties...*/ }
    parent > .class { /*...properties...*/ }
    parent > element.class { /*...properties...*/ }
    parent > el1 > el2 { /*...properties...*/ }

With district example, rule should be as

.. code-block:: scss

    district.oldtown > building { /*...properties...*/ }
    district.modern > building { /*...properties...*/ }


Greedy parent
^^^^^^^^^^^^^^^

Sometimes, we have a too nested structure - then we need a greedy parent selector.

.. code-block:: scss

    parent element { /*...properties...*/ }

In example, imagine if you have a *oldtown* district which buldings have only wooden roof, so we can desribe rule with

.. code-block:: scss

    district.oldtown roof { roof: wall.wooden; }


In another example, if you have these structure:

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

Sometimes, many elements can have a similar configuration, so it's possible to have an one rule for many selectors:

.. code-block:: scss

    selector1, selector2, selector3 {
        /*...properties...*/
    }


Merge Rules
-------------

It's possible to write many rules for one element. In example:

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

.. warning::
    order of rules is neccesary, bottom rule overwrite properites from top rule, in example:

    .. code-block:: scss

        draw-obj { wall: wall.brick.grass; }
        wall.brick { color: red; }
        wall.grass { color: green; }

    It’s equality this same with:

    .. code-block:: scss

        draw-obj { wall: wall.brick.grass; }
        wall.brick.grass { color: green; }

