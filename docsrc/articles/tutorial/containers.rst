Containers
================

In previous tutorials we used only one cube. It's boring, because reality have more cubes.

To add more Cubes (or any shapes) we are using containers. In this tutorial we use two containers: filler and series. So let's start!

Our goal is make a city walls with simple towers:

.. image:: images/containers.png

So we need declare first element with world, let's try to make city walls with 3 towers (left, center, right) and columns between towers.

.. code-block:: css

    world {
        width: 400;
        height: 400;
        body: city-wall;
    }

    city-wall {
        display: x-series;
        depth: 50;
        body:  /* N elements */
          tower,
          columns,
          tower,
          columns,
          tower;
    }

*city-wall* is a ``x-series`` - (x is for *X* axis). Series try to put N objects in one line (X/Y/Z axis).

Ok, so we need to declare first nested element, *tower*.

Notice the new parent in rule: ``>`` - is a parent operator.
Rule ``parent > child`` will be invoked when *child* element will be in *parent* container.

.. code-block:: css

    tower {
        display: y-series; /* series on Y axis */
        body: body, roof; /* two elements */
        width: 50;
        depth: 100%;
    }

    tower > body { /* body element must have tower's parent */
        display: cube;
        width: 100%; /* width inherits from parent's element */
        depth: 100%;
        height: 50;
        wall: wall.stone;
    }

    tower > roof {
        display: pyramid;
        width: 100%;
        depth: 100%;
        height: 20;
        roof: wall.gold;
    }

Ok, that's it! Now we need the columns. We will use `filler` here - this container,
is very simillar to series, but try to fill all *width/depth/height*
(depends on type of filler, *x*, *y* or *z*):

.. code-block:: scss

    columns {
        display: x-filler;
        body: column;
        width: random(50, 100);
        depth: 100%;
        align: center;
        justify: center;
        padding: 5;
    }

    columns > column {
        display: cube;
        depth: 10;
        width: 10;
        height: 50;

        roof: wall.stone;
        wall: wall.stone;
    }

And finally we will declare wall and textures:

.. code-block:: scss

    wall.stone { texture: tex.gray; }
    wall.gold { texture: tex.gold; }

    tex.gray { color: #AAA; }
    tex.gold { color: #FF2; }

That's all! The final code:

.. code-block:: css

    world {
        width: 400;
        height: 400;
        body: city-wall;
    }

    city-wall {
        display: x-series;
        depth: 50;
        body:
          tower,
          columns,
          tower,
          columns,
          tower;
    }

    tower {
        display: y-series;
        body: body, roof;
        width: 50;
        depth: 100%;
    }

    tower > body {
        display: cube;
        width: 100%;
        depth: 100%;
        height: 50;
        wall: wall.stone;
    }

    tower > roof {
        display: pyramid;
        width: 100%;
        depth: 100%;
        height: 20;
        roof: wall.gold;
    }

    columns {
        display: x-filler;
        body: column;
        width: random(50, 100);
        depth: 100%;
        align: center;
        justify: center;
        padding: 5;
    }

    columns > column {
        display: cube;
        depth: 10;
        width: 10;
        height: 50;

        roof: wall.stone;
        wall: wall.stone;
    }

    wall.stone { texture: tex.gray; }
    wall.gold { texture: tex.gold; }

    tex.gray { color: #AAA; }
    tex.gold { color: #FF2; }

You can try it online: https://pixelopolis.herokuapp.com/examples/Tutorial/Containers
