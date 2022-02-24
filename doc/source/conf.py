# Configuration file for the Sphinx documentation builder.
#
# This file only contains a selection of the most common options. For a full
# list see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

# -- Path setup --------------------------------------------------------------

# If extensions (or modules to document with autodoc) are in another directory,
# add these directories to sys.path here. If the directory is relative to the
# documentation root, use os.path.abspath to make it absolute, like shown here.
#
import os
import sys
# sys.path.insert(0, os.path.abspath('.'))

# -- Project information -----------------------------------------------------

project = 'Nuclei SDK'
copyright = '2019-Present, Nuclei'
author = 'Nuclei'

# The short X.Y version
version = '0.3.6-dev'

# The full version, including alpha/beta/rc tags
release = '0.3.6-dev'

# -- General configuration ---------------------------------------------------

# Add any Sphinx extension module names here, as strings. They can be
# extensions coming with Sphinx (named 'sphinx.ext.*') or your custom
# ones.
extensions = [
    'sphinx_rtd_theme',
    'breathe',
    'sphinx.ext.githubpages',
    'sphinx.ext.mathjax',
    'recommonmark'
]

# Add any paths that contain templates here, relative to this directory.
templates_path = ['_templates']

# List of patterns, relative to source directory, that match files and
# directories to ignore when looking for source files.
# This pattern also affects html_static_path and html_extra_path.
exclude_patterns = []

# -- Options for HTML output -------------------------------------------------
# Show build timestamp
html_last_updated_fmt = ""

# The theme to use for HTML and HTML Help pages.  See the documentation for
# a list of builtin themes.
#
html_theme = 'sphinx_rtd_theme'

html_logo = 'asserts/images/nsdk_logo_small.png'

# Add any paths that contain custom static files (such as style sheets) here,
# relative to this directory. They are copied after the builtin static files,
# so a file named "default.css" will overwrite the builtin "default.css".
html_static_path = ['_static']

html_theme_options = {
    'logo_only': True,
    'navigation_depth': 6
}

# -- Options for Breathe Project ---------------------------------------------
breathe_projects = {
}
breathe_default_project = ""
breathe_show_define_initializer = True

# -- Options for Latex output -------------------------------------------------
latex_logo = 'asserts/images/nsdk_logo_small.png'
latex_show_pagerefs = True
latex_toplevel_sectioning = 'chapter'
latex_show_urls = 'footnote'

rst_prolog = """
.. |nuclei_contact| replace:: email support@nucleisys.com
.. |NMSIS| replace:: `NMSIS`_
.. _NMSIS: https://github.com/Nuclei-Software/NMSIS
.. |nuclei_sdk| replace:: `Nuclei SDK`_
.. _Nuclei SDK: https://github.com/Nuclei-Software/nuclei-sdk
.. |nuclei_download_center| replace:: `Nuclei Download Center`_
.. _Nuclei Download Center: https://nucleisys.com/download.php
.. |github| replace:: `Github`_
.. _Github: https://github.com
.. |gitee| replace:: `Gitee`_
.. _Gitee: https://gitee.com
.. |github_nuclei_sdk| replace:: `Nuclei SDK in Github`_
.. _Nuclei SDK in Github: https://github.com/Nuclei-Software/nuclei-sdk
.. |gitee_nuclei_sdk| replace:: `Nuclei SDK in Gitee`_
.. _Nuclei SDK in Gitee: https://gitee.com/Nuclei-Software/nuclei-sdk
.. |github_nuclei_sdk_release| replace:: `Nuclei SDK Release in Github`_
.. _Nuclei SDK Release in Github: https://github.com/Nuclei-Software/nuclei-sdk/releases
.. |teraterm| replace:: `TeraTerm in Windows`_
.. _TeraTerm in Windows: http://ttssh2.osdn.jp/
.. |minicom| replace:: `Minicom in Linux`_
.. _Minicom in Linux: https://help.ubuntu.com/community/Minicom
"""

rst_epilog = """
"""
# -- Extension configuration -------------------------------------------------

def setup(app):
    app.add_css_file("css/custom.css")
