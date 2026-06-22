# Configuration file for the Sphinx documentation builder.
# https://www.sphinx-doc.org/en/master/usage/configuration.html

import os
import subprocess

# -- Project information -----------------------------------------------------

project = 'cisst'
copyright = '2004-2025, Johns Hopkins University (Baltimore, USA)'
author = 'Anton Deguet, Peter Kazanzides'

# The short X.Y version / release tag
release = 'main'

# -- General configuration ---------------------------------------------------

extensions = [
    'sphinx_tabs.tabs',
    'sphinx_copybutton',
    'breathe',
]

# Add any paths that contain templates here, relative to this directory.
templates_path = ['_templates']

# Patterns to ignore when looking for source files.
exclude_patterns = ['_build', 'Thumbs.db', '.DS_Store', 'migrate.py', '.venv']

master_doc = 'index'

# -- Breathe configuration (Doxygen API integration) -------------------------
# Run Doxygen during the Sphinx build so RTD always has up-to-date API docs.

_docs_dir = os.path.dirname(__file__)
_doxygen_xml = os.path.join(_docs_dir, '_build', 'doxygen', 'xml')

if not os.path.isdir(_doxygen_xml):
    os.makedirs(os.path.join(_docs_dir, '_build', 'doxygen'), exist_ok=True)
    # Only run doxygen if the XML output doesn't exist yet.
    # On ReadTheDocs every build starts fresh, so this always runs.
    subprocess.call('doxygen Doxyfile', shell=True, cwd=_docs_dir)

breathe_projects = {
    'cisst': _doxygen_xml,
}
breathe_default_project = 'cisst'
breathe_default_members = ('members', 'undoc-members')

# -- Options for HTML output -------------------------------------------------

html_theme = 'sphinx_rtd_theme'

html_static_path = ['_static']

html_context = {
    'display_github': True,
    'github_user': 'jhu-cisst',
    'github_repo': 'cisst',
    'github_version': 'main/docs/',
}

html_logo = '_static/cisst-logo.png' if os.path.exists('_static/cisst-logo.png') else None

html_theme_options = {
    'logo_only': False,
    'navigation_depth': 4,
    'collapse_navigation': False,
}

html_css_files = ['custom.css']


html_title = 'cisst libraries'

# -- Code highlighting -------------------------------------------------------

# Default language for :: literal blocks (avoids spurious highlighting)
highlight_language = 'none'

# -- Global RST substitutions ------------------------------------------------

rst_prolog = """
.. |cisst| replace:: *cisst*
.. |SAW| replace:: *SAW*
.. |cisstCommon| replace:: *cisstCommon*
.. |cisstVector| replace:: *cisstVector*
.. |cisstNumerical| replace:: *cisstNumerical*
.. |cisstOSAbstraction| replace:: *cisstOSAbstraction*
.. |cisstMultiTask| replace:: *cisstMultiTask*
.. |cisstRobot| replace:: *cisstRobot*
.. |cisstStereoVision| replace:: *cisstStereoVision*
.. |cisstParameterTypes| replace:: *cisstParameterTypes*
"""
