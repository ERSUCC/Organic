import sys
from pathlib import Path

sys.path.append(str(Path("ext").resolve()))

project = "Organic"
copyright = "2026, Isaac Brown"
author = "Isaac Brown"
version = "0.1.0"
release = "0.1.0"

nitpicky = True

html_theme = "alabaster"
html_static_path = [ "static" ]
html_logo = "static/images/logo.png"
html_favicon = "static/images/favicon.png"
html_css_files = [ "css/organic.css" ]
html_copy_source = False
html_show_sourcelink = False
html_permalinks = False

extensions = [ "organic" ]
