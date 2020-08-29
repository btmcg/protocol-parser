###############
protocol-parser
###############

Simple code that reads raw market data (currently only `ITCH
<ftp://emi.nasdaq.com/ITCH>`_) and builds a real-time book. Various
stats can be recorded and this project essentially acts as a testbed for
experimentation in optimization.


Cloning repo and submodules
===========================

.. code-block::

   git clone --recursive https://gitlab.com/btmcg/protocol-parser.git

or

.. code-block::

   git clone https://gitlab.com/btmcg/protocol-parser.git
   git submodule update --init --recursive


Data source
===========

The raw itch data files can be found `here
<ftp://emi.nasdaq.com/ITCH>`_.


Building and running
====================

First, build the cppgen binary for c++ code generation. Then run make as
specified below.

.. code-block::

   go build golang/cppgen.go


build options
-------------

``make -j``
    Using gcc, build all targets with full optimizations, including
    benchmarks and tests (if available).

``make COMPILER=clang -j``
    Using clang, build all targets with full optimizations, including
    benchmarks and tests (if available).

``make DEBUG=1 -j``
    Using gcc, build all targets with no optimizations and debug
    assertions included.

``make DEBUG=1 COMPILER=clang parser -j``
    Using clang, build only the parser (and dependencies) with no
    optimizations and debug assertions included.


Development
===========

See `README.dev <README.dev.rst>`_.
