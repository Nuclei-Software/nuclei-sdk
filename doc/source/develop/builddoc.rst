.. _develop_builddoc:

Build Nuclei SDK Documentation
==============================

In Nuclei SDK, we use Sphinx and restructured text as documentation tool.

Here we only provide steps to build sphinx documentation in Linux environment.

Install Tools
-------------

To build this the documentation, you need to have these tools installed.

* Python3
* Python Pip tool

Then you can use the pip tool to install extra python packages required to
build the documentation.

.. code-block:: shell

   pip install -r doc/requirements.txt

Build The Documentation
-----------------------

Then you can build the documentation using the following command:

.. code-block:: shell

    cd doc
    make html

The documentation will be generated in *doc/build/html* folder.
