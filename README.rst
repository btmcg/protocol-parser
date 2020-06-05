protocol-parser
===============

Cloning repo and submodules
---------------------------

.. code-block::

   git clone --recursive https://gitlab.com/btmcg/protocol-parser.git

or

.. code-block::

   git clone https://gitlab.com/btmcg/protocol-parser.git
   git submodule update --init --recursive


Maintaining third-party libraries
---------------------------------

catch
~~~~~

**Add new submodule**

.. code-block::

    git submodule add -- https://github.com/catchorg/Catch2.git third_party/catch2/2.12.2
    cd third_party/catch2/2.12.2
    git checkout -b v2.12.2


**Remove old submodule**

.. code-block::

    vim .gitmodules
    vim .git/config
    git add .gitmodules
    git rm --cached third_party/catch2/2.12.1
    rm -rf .git/modules/third_party/catch2/2.12.1
    rm -rf third_party/catch2/2.12.1

**Point makefile to new version**

.. code-block::

    vim mk/third_party.mk


fmt
~~~

**Add new submodule**

.. code-block::

    git submodule add -- https://github.com/fmtlib/fmt.git third_party/fmt/6.2.1
    cd third_party/fmt/6.2.1
    git checkout -b 6.2.1

**Remove old submodule**

.. code-block::

    vim .gitmodules
    vim .git/config
    git add .gitmodules
    git rm --cached third_party/fmt/6.2.0
    rm -rf .git/modules/third_party/fmt/6.2.0
    rm -rf third_party/fmt/6.2.0

**Point makefile to new version**

.. code-block::

    vim mk/third_party.mk
