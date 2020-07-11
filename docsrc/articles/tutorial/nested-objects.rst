Nested objects
================

In previous tutorials we learnt about containers. In this tutorial we will learn about BSP tree and nested objects. Will be cool, I promise you!

Today we will create the city:

.. image:: images/nested.png

Let's start with first element, *city*. City has *bsp-tree* display - this draw-obj try to split area N times.

.. code-block:: css

    world {
        width: 500;
        height: 500;
        body: city;
    }

    city {
        display: bsp-tree;
        iterations: 4;
        width: 400;
        depth: 400;
        min-width: 80;
        min-height: 80;
        padding: 1;
        body: choice(
            building-place.red-roof,
            building-place.blue-roof);
    }

We decided to randomically choose building with red or blue roof in each splitted area.
Now we need to declare *building-place*. It's a *series* with two objects - plane (grass or mud) and building (house or office).

.. code-block:: css

    building-place {
        display: z-series;
        width: 100%;
        depth: 100%;
        justify: center; /* new attribute - try to align in x-z axis */
        body:
            choice(grass, mud),
            choice(house, office);
    }

Ok, the two most neccesary elements are defined (hooray!). Let's declare *grass* and *mud*.

.. code-block:: css

    grass, mud { /* we can declare more rules using comma */
        display: cube;
        width: 100%;
        depth: 100%;
        height: 2;
    }

    grass { roof: wall.grass; } /* one element can have more rules */
    mud { roof: wall.mud; }

Now we will declare the *house* with pyramid-shaped roof:

.. code-block:: css

    house { /* very similiar to tower in previous tutorial. */
        display: z-series;
        body: body, roof;
        width: 60;
        depth: 60;

        /* each rule can have nested rules */
        /* & is a reference to parent rule, */
        /* in here `& > body` is equals to `house > body` */
        & > body {
            display: cube;
            width: 100%;
            depth: 100%;
            height: 50;
            wall: wall.stone;
        }

        & > roof {
            display: pyramid;
            width: 100%;
            depth: 100%;
            height: 20;
            roof: wall.stone;
        }
    }

... and office:

.. code-block:: css

    office { /* very simillar to house, only difference is a roof and dimensions */
        display: z-series;
        body: body, roof;
        width: 50;
        depth: 80;

        & > body {
            display: cube;
            width: 100%;
            depth: 100%;
            height: 80;
            wall: wall.stone;
        }

        & > roof {
            display: cube;
            width: 100%;
            depth: 100%;
            height: 5;
            wall: wall.stone;
            roof: wall.stone;
        }
    }

But we forget about red and blue roof!

.. code-block:: css

    /* here is a greedy parent - this parent is recurrently */
    /* in here, child's element roof try to find building-place */
    /* in parent's element, grandparent, grandgrandparent etc. */
    building-place.red-roof roof { roof: wall.red-roof; }
    building-place.blue-roof roof { roof: wall.blue-roof; }

And finally we will declare wall and textures:

.. code-block:: css

    wall.stone { texture: tex.gray; }
    wall.gold { texture: tex.gold; }

    tex.gray { color: #AAA; }
    tex.gold { color: #FF2; }

That's all! The final code:

.. code-block:: css

    world {
        width: 500;
        height: 500;
        body: city;
    }

    city {
        display: bsp-tree;
        iterations: 4;
        width: 400;
        depth: 400;
        min-width: 80;
        min-height: 80;
        padding: 1;
        body: choice(
            building-place.red-roof,
            building-place.blue-roof);
    }

    building-place {
        display: z-series;
        width: 100%;
        depth: 100%;
        justify: center;
        body:
            choice(grass, mud),
            choice(house, office);
    }

    grass, mud {
        display: cube;
        width: 100%;
        depth: 100%;
        height: 2;
    }

    grass { roof: wall.grass; }
    mud { roof: wall.mud; }

    house {
        display: z-series;
        body: body, roof;
        width: 60;
        depth: 60;

        & > body {
            display: cube;
            width: 100%;
            depth: 100%;
            height: 50;
            wall: wall.stone;
        }

        & > roof {
            display: pyramid;
            width: 100%;
            depth: 100%;
            height: 20;
            roof: wall.stone;
        }
    }

    office {
        display: z-series;
        body: body, roof;
        width: 50;
        depth: 80;

        & > body {
            display: cube;
            width: 100%;
            depth: 100%;
            height: 80;
            wall: wall.stone;
        }

        & > roof {
            display: cube;
            width: 100%;
            depth: 100%;
            height: 5;
            wall: wall.stone;
            roof: wall.stone;
        }
    }

    building-place.red-roof roof { roof: wall.red-roof; }
    building-place.blue-roof roof { roof: wall.blue-roof; }

    wall.grass      { texture: tex.green; }
    wall.mud        { texture: tex.brown; }
    wall.stone      { texture: tex.gray; }
    wall.red-roof   { texture: tex.red; }
    wall.blue-roof  { texture: tex.blue; }

    tex.gray        { color: #AAAAAA; }
    tex.red         { color: #9B111E; }
    tex.green       { color: #567D46; }
    tex.blue        { color: #008491; }
    tex.brown       { color: #70543E; }

You can try it online: https://pixelopolis.herokuapp.com/examples/Tutorial/Nested-Objects
