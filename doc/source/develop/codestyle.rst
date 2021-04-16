.. _develop_codestyle:

Code Style
==========

In Nuclei SDK, we use `EditorConfig`_ to maintain our development
coding styles and `astyle`_ tool to format our source code.

- Our `editorconfig file`_ is maintained in the root directory of
  Nuclei SDK, called ``.editorconfig``.
- Our astyle option file is maintained in the root directory of
  Nuclei SDK, called ``.astylerc``.

For example, if you want to format your applicaton code(*.c/*.h)
located in ``application/baremetal/demo_timer``, you can run the
following command:

.. code-block:: shell

    # make sure astyle is present in PATH
    which astyle
    # format code
    astyle --options=.astylerc --recursive application/baremetal/demo_timer/*.c,*.h


You can install editorconfig plugins for your editor, see https://editorconfig.org/#download.

We use `doxygen`_ to comment C/C++ source code.


.. _EditorConfig: https://editorconfig.org/
.. _editorconfig file: https://github.com/Nuclei-Software/nuclei-sdk/tree/master/.editorconfig
.. _doxygen: http://www.doxygen.nl/manual/docblocks.html
.. _astyle: http://astyle.sourceforge.net/
