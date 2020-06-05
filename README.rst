===============
protocol-parser
===============

Maintaining Third-party Libraries
---------------------------------

fmt
___

**Add new submodule**

.. code-block::

    git submodule add -- https://github.com/fmtlib/fmt.git third_party/fmt/6.2.1
    cd third_party/fmt/6.2.1
    git checkout -b 6.2.1

**Remove old submodule**

.. code-block::

    vim .gitmodules
    git add .gitmodules
    vim .git/config
    git rm --cached third_party/fmt/6.2.0
    rm -rf .git/modules/third_party/fmt/6.2.0
    rm -rf third_party/fmt/6.2.0

**Point makefile to new version**

.. code-block::

    vim mk/third_party.mk
