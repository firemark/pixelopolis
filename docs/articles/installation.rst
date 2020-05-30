Installation
==============

Requirements
---------------

* YACC
* Bison
* libpng
* gcc
* probably linux :)

Locally
----------------

Debug version:

.. code-block:: bash

    make all

Prod version:

.. code-block:: bash

    make all VERSION=PROD


Usage
--------------

.. code-block:: bash

    ./pixelopolis [CSS_PATH] [OUT_PNG_PATH]


Docker Version
----------------

.. code-block:: bash

    # bulding image
    docker build . -t pixelopolis

    # running webpanel on localhost:8000
    docker run -it --rm -p 8000:80 pixelopolis

