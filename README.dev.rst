###########
Development
###########

Useful tips for maintaining and adding to the project.


Enable git hooks
================

From within the repository, run

    ``git config --local core.hooksPath .githooks``



Installing and maintaining third-party libraries
================================================

catch
-----

**Add new submodule**

.. code-block::

    git submodule add -- https://github.com/catchorg/Catch2.git third_party/catch2/2.13.10
    cd third_party/catch2/2.13.10
    git checkout v2.13.10


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

    vim nrmake/third_party.mk


google benchmark v1.5.2
-----------------------

**Build and install**

.. code-block::

    # from protocol-parser root
    git clone --branch=v1.5.2 --depth=1 https://github.com/google/benchmark.git gb
    cd gb

    cmake . -DBENCHMARK_ENABLE_GTEST_TESTS=OFF -DBENCHMARK_ENABLE_TESTING=OFF -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=/usr/bin/g++ -DCMAKE_INSTALL_PREFIX=../third_party/google-benchmark-gcc/1.5.2
    make install -j

    # now build with clang
    rm -rf CMakeFiles src/generated

    cmake . -DBENCHMARK_ENABLE_GTEST_TESTS=OFF -DBENCHMARK_ENABLE_TESTING=OFF -DBENCHMARK_USE_LIBCXX=ON -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=/usr/bin/clang++ -DCMAKE_INSTALL_PREFIX=../third_party/google-benchmark-clang/1.5.2
    make install -j

    cd ..
    rm -rf gb


fmt
---

**Add new submodule**

.. code-block::

    git submodule add -- https://github.com/fmtlib/fmt.git third_party/fmt/7.0.0
    cd third_party/fmt/7.0.0
    git checkout 7.0.0

**Remove old submodule**

.. code-block::

    vim .gitmodules
    vim .git/config
    git add .gitmodules
    git rm --cached third_party/fmt/6.2.1
    rm -rf .git/modules/third_party/fmt/6.2.1
    rm -rf third_party/fmt/6.2.1

**Point makefile to new version**

.. code-block::

    vim nrmake/third_party.mk
