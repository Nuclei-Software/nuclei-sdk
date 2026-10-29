#!/bin/env python3

import os
import sys
import csv
import io
import json
import re
import struct
import argparse
import random
import html
import http.server
import socketserver
import socket
import time
import threading
import urllib.parse
import webbrowser

try:
    import yaml
except ImportError:
    yaml = None

RENDER_EXTS = ('.md', '.rst', '.csv')
RAW_VALUES = ('1', 'true', 'yes', 'on')
RAW_LINK = '?raw=1'

# port registry: one file per user, mapping absolute directory -> remembered port
CONFIG_ENV = 'NSDK_HTTPSERVER_CONFIG'
DEFAULT_CONFIG_FILE = '~/.httpsrv.yaml'
PORT_MIN = 10000
PORT_MAX = 65000
BIND_ALL = '0.0.0.0'

MAX_CSV_ROWS = 5000
# unknown extensions below this size are shown as text instead of downloaded
INLINE_MAX_BYTES = 5 * 1024 * 1024
OCTET = 'application/octet-stream'

MD_MISSING = ("python module 'markdown-it-py' is not installed, *.md is served as "
              "plain text, run 'pip3 install markdown-it-py' to enable rendering")
RST_MISSING = ("python module 'docutils' is not installed, *.rst is served as "
               "plain text, run 'pip3 install docutils' to enable rendering")

# warnings are deduplicated so a broken renderer does not flood the console
_WARNED = set()


def warn(message):
    if message in _WARNED:
        return
    _WARNED.add(message)
    print("WARNING: %s" % message, file=sys.stderr, flush=True)


PAGE_CSS = """
body {
    margin: 0;
    font-family: -apple-system, "Segoe UI", "Helvetica Neue", Arial, "Noto Sans", sans-serif;
    line-height: 1.6;
    color: #24292f;
    background: #ffffff;
}
.nsdk-nav {
    display: flex;
    align-items: center;
    gap: 12px;
    padding: 6px 16px;
    border-bottom: 1px solid #d0d7de;
    background: #f6f8fa;
    font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", system-ui,
                 "Helvetica Neue", Arial, "Noto Sans", sans-serif;
    font-size: 14px;
}
.nsdk-path {
    flex: 1;
    display: flex;
    align-items: center;
    min-width: 0;
    color: #57606a;
    overflow: hidden;
    white-space: nowrap;
    letter-spacing: 0.01em;
}
.nsdk-path > span, .nsdk-path > a {
    flex: 0 0 auto;
    min-width: 0;
    overflow: hidden;
    text-overflow: ellipsis;
}
.nsdk-path a {
    color: #57606a;
    padding: 2px 6px;
    margin: 0 -2px;
    border-radius: 4px;
    text-decoration: none;
    transition: background-color 0.12s, color 0.12s;
}
.nsdk-path a:hover { color: #0969da; background: #eaeef2; }
.nsdk-sep {
    color: #8c959f;
    margin: 0 3px;
    flex: 0 0 auto;
    font-size: 15px;
    line-height: 1;
}
.nsdk-current {
    color: #24292f;
    font-weight: 600;
    padding: 2px 0;
}
.nsdk-links { display: flex; align-items: center; flex: 0 0 auto; }
.nsdk-links a {
    color: #57606a;
    padding: 3px 10px;
    margin-left: 4px;
    border-radius: 6px;
    text-decoration: none;
    transition: background-color 0.12s, color 0.12s;
}
.nsdk-links a:hover { color: #0969da; background: #eaeef2; }
.nsdk-shell {
    display: flex;
    align-items: flex-start;
    gap: 24px;
    max-width: 1400px;
    margin: 0 auto;
    padding: 8px 24px 48px;
}
.nsdk-shell > .nsdk-doc, .nsdk-shell > .nsdk-listpage {
    flex: 1;
    min-width: 0;
    max-width: none;
    margin: 0;
    padding: 0;
}
.nsdk-toc {
    position: sticky;
    top: 8px;
    flex: 0 0 220px;
    max-height: calc(100vh - 24px);
    overflow: auto;
    padding: 8px 0 8px 16px;
    border-left: 1px solid #d0d7de;
    font-size: 13px;
}
.nsdk-toc-title { color: #57606a; font-weight: 600; margin-bottom: 6px; }
.nsdk-toc ul { list-style: none; margin: 0; padding: 0; }
.nsdk-toc li { margin: 2px 0; line-height: 1.35; }
.nsdk-toc a { color: #57606a; text-decoration: none; }
.nsdk-toc a:hover { color: #0969da; text-decoration: underline; }
.nsdk-toc-l2 { padding-left: 10px; }
.nsdk-toc-l3, .nsdk-toc-l4, .nsdk-toc-l5, .nsdk-toc-l6 { padding-left: 22px; }
@media (max-width: 1000px) {
    .nsdk-toc { display: none; }
}
.nsdk-doc { max-width: 1080px; margin: 0 auto; padding: 8px 24px 48px; }
.nsdk-doc main { display: block; }
.nsdk-code-wrap { position: relative; }
.nsdk-code-wrap > pre { margin-top: 0; }
.nsdk-plain { margin: 12px 0; }
.nsdk-plain pre.nsdk-code {
    margin: 0;
    padding: 0;
    font-family: ui-monospace, SFMono-Regular, "SF Mono", Menlo, Consolas, monospace;
    font-size: 13px;
    line-height: 1.5;
    /* each row is its own block, so the newline between rows must not also
       render: 'pre' would add a blank line after every source line, and
       'pre-wrap' would fold long lines away from their number */
    white-space: pre;
    overflow-x: auto;
    counter-reset: line;
}
.nsdk-plain .nsdk-line { display: block; }
.nsdk-plain .nsdk-line.nsdk-hit { background: #fff8c5; }
.nsdk-plain .nsdk-ln {
    display: inline-block;
    width: 4.5em;
    padding-right: 1em;
    margin-right: 1em;
    text-align: right;
    color: #8c959f;
    background: #f6f8fa;
    border-right: 1px solid #d0d7de;
    text-decoration: none;
    user-select: none;
    -webkit-user-select: none;
    position: sticky;
    left: 0;
}
.nsdk-plain .nsdk-line.nsdk-hit .nsdk-ln { background: #fff1a8; color: #7d4e00; }
.nsdk-plain .nsdk-ln:hover { color: #0969da; background: #eaeef2; }
.nsdk-copy {
    position: absolute;
    top: 6px;
    right: 6px;
    padding: 2px 8px;
    font-size: 11px;
    color: #57606a;
    background: #ffffff;
    border: 1px solid #d0d7de;
    border-radius: 4px;
    cursor: pointer;
    opacity: 0;
    transition: opacity 0.15s;
}
.nsdk-code-wrap:hover > .nsdk-copy, .nsdk-copy:focus { opacity: 1; }
.nsdk-copy:hover { color: #0969da; border-color: #0969da; }
.nsdk-todo {
    display: inline-block;
    padding: 0 4px;
    border-radius: 3px;
    font-size: 85%;
    font-weight: 700;
    letter-spacing: 0.02em;
    vertical-align: baseline;
}
.nsdk-todo-todo, .nsdk-todo-xxx { background: #fff8c5; color: #7d4e00; }
.nsdk-todo-fixme, .nsdk-todo-bug { background: #ffebe9; color: #a40e26; }
.nsdk-todo-hack { background: #fbefff; color: #8250df; }
.nsdk-todo-note { background: #ddf4ff; color: #0550ae; }
.nsdk-doc h1, .nsdk-doc h2 { border-bottom: 1px solid #d0d7de; padding-bottom: 4px; }
.nsdk-doc pre {
    background: #f6f8fa;
    border: 1px solid #d0d7de;
    border-radius: 4px;
    padding: 12px;
    overflow: auto;
}
.nsdk-doc code {
    background: #eff1f3;
    border-radius: 3px;
    padding: 1px 4px;
    font-size: 90%;
}
.nsdk-doc pre code { background: none; padding: 0; font-size: 100%; }
.nsdk-doc table { border-collapse: collapse; margin: 12px 0; }
.nsdk-doc table th, .nsdk-doc table td {
    border: 1px solid #d0d7de;
    padding: 6px 10px;
    vertical-align: top;
}
.nsdk-doc table th { background: #f6f8fa; }
.nsdk-doc blockquote {
    margin: 12px 0;
    padding: 0 12px;
    color: #57606a;
    border-left: 4px solid #d0d7de;
}
.nsdk-doc img { max-width: 100%; }
.nsdk-doc .admonition {
    border: 1px solid #d0d7de;
    border-radius: 4px;
    background: #f6f8fa;
    padding: 8px 12px;
    margin: 12px 0;
}
.nsdk-doc .admonition-title { font-weight: bold; margin: 0 0 4px; }
.nsdk-doc dt { font-weight: bold; }
.nsdk-doc table.nsdk-csv { font-size: 13px; }
.nsdk-doc table.nsdk-csv td {
    white-space: pre-wrap;
    word-break: break-word;
    font-variant-numeric: tabular-nums;
}
.nsdk-doc table.nsdk-csv thead th {
    position: sticky;
    top: 0;
    z-index: 1;
    white-space: nowrap;
}
.nsdk-empty { color: #57606a; font-style: italic; }
.nsdk-notice {
    border: 1px solid #d4a72c;
    border-radius: 4px;
    background: #fff8c5;
    padding: 8px 12px;
    margin: 12px 0;
}
.nsdk-listpage { max-width: 1400px; margin: 0 auto; padding: 8px 24px 48px; }
.nsdk-listpage h1 { font-size: 20px; margin: 8px 0 16px; }
.nsdk-listpage h1 .nsdk-sep { color: #8c959f; font-size: 18px; margin: 0 6px; }
.nsdk-listpage h1 .nsdk-current { color: #24292f; }
.nsdk-filter {
    width: 100%;
    box-sizing: border-box;
    margin: 0 0 10px;
    padding: 6px 10px;
    font-size: 14px;
    color: #24292f;
    border: 1px solid #d0d7de;
    border-radius: 4px;
    background: #ffffff;
}
.nsdk-filter:focus { outline: none; border-color: #0969da; box-shadow: 0 0 0 3px #ddf4ff; }
.nsdk-search-status {
    margin: -4px 0 10px;
    font-size: 12px;
    color: #57606a;
}
a.nsdk-search-hit { font-family: ui-monospace, SFMono-Regular, Menlo, monospace; font-size: 13px; }
table.nsdk-files th.nsdk-sortable { cursor: pointer; user-select: none; }
table.nsdk-files th.nsdk-sortable:hover { color: #0969da; }
table.nsdk-files th .nsdk-arrow { color: #afb8c1; font-size: 10px; margin-left: 3px; }
table.nsdk-files th.nsdk-asc .nsdk-arrow, table.nsdk-files th.nsdk-desc .nsdk-arrow {
    color: #0969da;
}
.nsdk-icon { margin-right: 6px; font-size: 13px; }
.nsdk-listpage h1 a { color: #0969da; text-decoration: none; }
.nsdk-listpage h1 a:hover { text-decoration: underline; }
table.nsdk-files { width: 100%; border-collapse: collapse; font-size: 14px; }
table.nsdk-files th {
    text-align: left;
    padding: 6px 10px;
    background: #f6f8fa;
    border-bottom: 2px solid #d0d7de;
    white-space: nowrap;
}
table.nsdk-files td { padding: 6px 10px; border-bottom: 1px solid #eaeef2; }
table.nsdk-files tr:hover td { background: #f6f8fa; }
table.nsdk-files td.nsdk-size, table.nsdk-files td.nsdk-time {
    color: #57606a;
    white-space: nowrap;
    font-variant-numeric: tabular-nums;
    width: 1%;
}
table.nsdk-files a { color: #0969da; text-decoration: none; }
table.nsdk-files a:hover { text-decoration: underline; }
table.nsdk-files a.nsdk-src {
    margin-left: 8px;
    font-size: 11px;
    color: #8c959f;
    border: 1px solid #d0d7de;
    border-radius: 3px;
    padding: 0 4px;
}
table.nsdk-files a.nsdk-src:hover { color: #0969da; border-color: #0969da; text-decoration: none; }
"""

FAVICON_SVG = """<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 32 32">
<path d="M7 2h12l7 7v21a2 2 0 0 1-2 2H7a2 2 0 0 1-2-2V4a2 2 0 0 1 2-2z" fill="#0969da"/>
<path d="M19 2l7 7h-5a2 2 0 0 1-2-2z" fill="#cfe3ff"/>
<g fill="#ffffff">
<rect x="10" y="13" width="12" height="2.4" rx="1.2"/>
<rect x="10" y="18" width="12" height="2.4" rx="1.2"/>
<rect x="10" y="23" width="8" height="2.4" rx="1.2"/>
</g>
</svg>"""

FAVICON_SVG_BYTES = FAVICON_SVG.encode('utf-8')
_FAVICON_ICO = None

PAGE_TEMPLATE = """<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<link rel="icon" type="image/svg+xml" href="/favicon.svg">
<link rel="alternate icon" type="image/x-icon" href="/favicon.ico">
<title>%(title)s</title>
<style>%(css)s</style>
</head>
<body>
<nav class="nsdk-nav">
<span class="nsdk-path" title="%(pathtip)s">%(path)s</span>
<span class="nsdk-links">%(links)s</span>
</nav>
<div class="nsdk-shell">
<main class="%(mainclass)s">
%(heading)s
%(notice)s
%(body)s
</main>
%(toc)s
</div>
</body>
</html>
"""


# ---------------------------------------------------------------------------
# favicon: a small "document" icon, rendered as both svg and a 32x32 ico so
# that every page (including plain text ones) gets the same tab icon
# ---------------------------------------------------------------------------

def _icon_color(x, y):
    """RGB for the pixel at (x, y) of a 32x32 canvas, None for transparent."""
    left, right, top, bottom = 7, 25, 2, 29
    if not (left <= x <= right and top <= y <= bottom):
        return None
    if y < x - 17:                                  # folded corner
        return (207, 227, 255)
    if 10 <= x <= 22 and (13 <= y <= 14 or 18 <= y <= 19):
        return (255, 255, 255)                       # text lines
    if 10 <= x <= 18 and 23 <= y <= 24:
        return (255, 255, 255)
    return (9, 105, 218)                             # document body


def _icon_rows(size=32):
    """Top-down rows of BGRA pixels."""
    rows = []
    for y in range(size):
        row = bytearray(size * 4)
        for x in range(size):
            color = _icon_color(x, y)
            if color is None:
                continue
            offset = x * 4
            row[offset] = color[2]
            row[offset + 1] = color[1]
            row[offset + 2] = color[0]
            row[offset + 3] = 255
        rows.append(row)
    return rows


def build_favicon_ico(size=32):
    """Build a 32bpp BGRA .ico file in memory, no PIL required."""
    rows = _icon_rows(size)
    xor_bitmap = b''.join(bytes(row) for row in reversed(rows))   # bottom-up
    mask_stride = ((size + 31) // 32) * 4
    mask = bytearray()
    for row in reversed(rows):
        bits = bytearray(mask_stride)
        for x in range(size):
            if row[x * 4 + 3] == 0:
                bits[x // 8] |= 0x80 >> (x % 8)
        mask += bits
    dib_header = struct.pack('<IiiHHIIiiII', 40, size, size * 2, 1, 32, 0,
                             len(xor_bitmap) + len(mask), 0, 0, 0, 0)
    image = dib_header + xor_bitmap + bytes(mask)
    icon_dir = struct.pack('<HHH', 0, 1, 1)
    icon_entry = struct.pack('<BBBBHHII', size % 256, size % 256, 0, 0, 1, 32,
                             len(image), len(icon_dir) + 16)
    return icon_dir + icon_entry + image


def favicon_ico():
    global _FAVICON_ICO
    if _FAVICON_ICO is None:
        _FAVICON_ICO = build_favicon_ico()
    return _FAVICON_ICO


# ---------------------------------------------------------------------------
# document renderers
# ---------------------------------------------------------------------------

TEXT_SAMPLE = 8192


def text_encoding(fspath, sample_size=TEXT_SAMPLE):
    """Return the encoding to announce for a text file, or None if it is binary.

    A NUL byte is the usual binary marker; text in gbk/latin-1 still passes
    because every byte decodes under latin-1, so the NUL check plus the control
    character ratio is what actually rejects images, archives and object files.
    """
    try:
        with open(fspath, 'rb') as f:
            sample = f.read(sample_size)
    except OSError:
        return None
    if b'\x00' in sample:
        return None
    if not sample:
        return 'utf-8'
    for encoding in ('utf-8', 'gbk'):
        try:
            sample.decode(encoding)
            return encoding
        except UnicodeDecodeError:
            continue
    controls = sum(1 for byte in sample if byte < 0x09 or 0x0e <= byte < 0x20)
    return 'latin-1' if controls * 100 <= len(sample) else None


def read_text(fspath, force=False):
    """Read text as utf-8 (BOM tolerant), falling back to gbk/latin-1.

    With force=True an undecodable file is still returned, with undecodable
    bytes replaced, so a known text file holding a few garbage bytes shows up
    in the viewer instead of turning into a download.
    """
    with open(fspath, 'rb') as f:
        data = f.read()
    for encoding in ('utf-8-sig', 'gbk', 'latin-1'):
        try:
            return data.decode(encoding)
        except UnicodeDecodeError:
            continue
    if not force:
        return data.decode('utf-8', 'replace')
    return data.decode('utf-8-sig', 'replace')


_MARKDOWN_BACKEND = None


def linkify_available():
    try:
        import linkify_it            # noqa: F401
    except ImportError:
        return False
    return True


PYGMENTS_FORMATTER = None


def pygments_css():
    """Highlight stylesheet, empty when pygments is not installed."""
    try:
        from pygments.formatters import HtmlFormatter
    except ImportError:
        return ""
    # markdown-it emits plain <pre><code> without a .highlight wrapper, so the
    # pygments rules must key off .nsdk-code-wrap instead, otherwise nothing
    # in the stylesheet ever matches and the block stays uncoloured
    return ".nsdk-code-wrap { background: #f6f8fa; }\n" + \
        HtmlFormatter(style='friendly').get_style_defs('.nsdk-code-wrap')


def highlight_code(code, lang, attrs):
    """Pygments callback for markdown-it; '' means 'render the block unchanged'."""
    global PYGMENTS_FORMATTER
    if not lang:
        return ''
    try:
        from pygments import highlight
        from pygments.lexers import get_lexer_by_name
    except ImportError:
        return ''
    if PYGMENTS_FORMATTER is None:
        from pygments.formatters import HtmlFormatter
        PYGMENTS_FORMATTER = HtmlFormatter(nowrap=True, style='friendly')
    try:
        lexer = get_lexer_by_name(lang, stripall=False)
    except Exception:
        return ''                        # unknown language: leave it plain
    return highlight(code, lexer, PYGMENTS_FORMATTER)


def markdown_backend():
    """Return a callable text -> html, or None when no markdown module exists."""
    global _MARKDOWN_BACKEND
    if _MARKDOWN_BACKEND is None:
        try:
            from markdown_it import MarkdownIt
        except ImportError:
            try:
                import markdown as _markdown
            except ImportError:
                _MARKDOWN_BACKEND = False
            else:
                import functools
                _MARKDOWN_BACKEND = functools.partial(
                    _markdown.markdown,
                    extensions=["extra", "sane_lists", "toc"])
        else:
            # "commonmark" preset keeps raw html, "gfm-like" would strip it.
            # linkify needs both the option and linkify-it-py; markdown-it only
            # imports that module at render time, so probe it up front instead
            # of letting every *.md request blow up on <ModuleNotFoundError>.
            options = {"html": True, "highlight": highlight_code}
            if linkify_available():
                options["linkify"] = True
            md = MarkdownIt("commonmark", options)
            if options.get("linkify"):
                md.enable("linkify")
            md.enable(["table", "strikethrough"])
            md = _load_md_plugins(md)
            _MARKDOWN_BACKEND = md.render
    return _MARKDOWN_BACKEND or None


def _load_md_plugins(md):
    """Optional github-ish syntax: task lists, footnotes, definition lists.

    mdit-py-plugins is not in requirements.txt, so every plugin is optional and
    a missing package must leave the renderer fully working, just plainer.
    """
    try:
        from mdit_py_plugins.tasklists import tasklists_plugin
    except ImportError:
        pass
    else:
        md = md.use(tasklists_plugin, enabled=False)
    try:
        from mdit_py_plugins.footnote import footnote_plugin
    except ImportError:
        pass
    else:
        md = md.use(footnote_plugin)
    try:
        from mdit_py_plugins.deflist import deflist_plugin
    except ImportError:
        pass
    else:
        md = md.use(deflist_plugin)
    return md


_DOCUTILS = None


def docutils_core():
    global _DOCUTILS
    if _DOCUTILS is None:
        try:
            import docutils.core
        except ImportError:
            _DOCUTILS = False
        else:
            _DOCUTILS = docutils.core
    return _DOCUTILS or None


def probe_renderers():
    """Warn once at startup about missing optional modules, nothing else."""
    if markdown_backend() is None:
        warn(MD_MISSING)
    if docutils_core() is None:
        warn(RST_MISSING)


def github_slug(text):
    """GitHub/github-slugger style anchor: lower case, drop punctuation, space to '-'."""
    out = []
    for ch in re.sub(r'<[^>]*>', '', text).strip().lower():
        if ch.isspace():
            out.append('-')
        elif ch.isalnum() or ch in '-_':
            out.append(ch)
    return ''.join(out)


def unique_slug(slug, used):
    if not slug:
        slug = 'section'
    if slug not in used:
        used[slug] = 0
        return slug
    while True:
        used[slug] += 1
        candidate = "%s-%d" % (slug, used[slug])
        if candidate not in used:
            used[candidate] = 0
            return candidate


HEADING_RE = re.compile(r'<h([1-6])([^>]*)>(.*?)</h\1>', re.S)


def add_heading_ids(body):
    """Add GitHub compatible anchor ids so '#some-heading' links work."""
    used = {}

    def repl(match):
        level, attrs, inner = match.group(1), match.group(2), match.group(3)
        if re.search(r'\bid\s*=', attrs):
            return match.group(0)
        slug = unique_slug(github_slug(html.unescape(re.sub(r'<[^>]*>', '', inner))), used)
        return '<h%s%s id="%s">%s</h%s>' % (level, attrs, slug, inner, level)

    return HEADING_RE.sub(repl, body)


# only the strict marker forms are highlighted, and only inside code: a plain
# 'TODO' or 'NOTE' in prose is ordinary writing and must stay uncoloured
TODO_RE = re.compile(r'\b(TODO|FIXME|HACK|XXX)\b(?=\s*[:(]|\s*$)')
CODE_BLOCK_RE = re.compile(r'(<pre\b[^>]*>)(.*?)(</pre>)', re.S)


def mark_keywords(body):
    """Highlight TODO/FIXME markers inside code blocks only.

    Pygments output is a soup of spans, so the regex runs on the block as a
    whole and only ever adds a span around a bare word; the marker text itself
    is not split across spans by any lexer.
    """
    def repl(match):
        opening, inner, closing = match.groups()
        inner = TODO_RE.sub(
            lambda m: '<span class="nsdk-todo nsdk-todo-%s">%s</span>'
                      % (m.group(1).lower(), m.group(1)), inner)
        return opening + inner + closing

    return CODE_BLOCK_RE.sub(repl, body)


COPY_BUTTON = ('<button class="nsdk-copy" type="button" '
               'title="copy code to the clipboard">copy</button>')
COPY_SCRIPT = """
<script>
document.querySelectorAll('.nsdk-code-wrap > pre').forEach(function (pre) {
    var wrap = pre.parentNode;
    var button = wrap.querySelector('.nsdk-copy');
    if (!button) { return; }
    button.addEventListener('click', function () {
        // the plain viewer renders one block row per line with no newline
        // between rows, so textContent would glue every line together and
        // pick the line numbers up with it; read data-text instead
        var rows = pre.querySelectorAll('.nsdk-line');
        var text;
        if (rows.length) {
            text = Array.prototype.map.call(rows, function (row) {
                return row.getAttribute('data-text') || '';
            }).join('\n');
            if (rows[rows.length - 1].getAttribute('data-text') !== '') {
                text += '\n';
            }
        } else {
            text = pre.innerText;
        }
        var done = function () {
            button.textContent = 'copied';
            setTimeout(function () { button.textContent = 'copy'; }, 1200);
        };
        if (navigator.clipboard && navigator.clipboard.writeText) {
            navigator.clipboard.writeText(text).then(done, function () {});
        } else {
            var area = document.createElement('textarea');
            area.value = text;
            document.body.appendChild(area);
            area.select();
            try { document.execCommand('copy'); done(); } catch (e) {}
            document.body.removeChild(area);
        }
    });
});
</script>
"""


LINE_ANCHOR_SCRIPT = """
<script>
(function () {
    function mark() {
        var hash = window.location.hash || '';
        var match = /^#L(\\d+)$/.exec(hash);
        document.querySelectorAll('.nsdk-line.nsdk-hit').forEach(function (el) {
            el.classList.remove('nsdk-hit');
        });
        if (!match) { return; }
        var target = document.getElementById('L' + match[1]);
        if (!target) { return; }
        target.classList.add('nsdk-hit');
        target.scrollIntoView({ block: 'center' });
    }
    window.addEventListener('hashchange', mark);
    mark();
    // clicking a line number copies the link instead of only jumping
    document.querySelectorAll('.nsdk-ln').forEach(function (link) {
        link.addEventListener('click', function (event) {
            var url = window.location.href.split('#')[0] + link.getAttribute('href');
            if (navigator.clipboard && navigator.clipboard.writeText) {
                event.preventDefault();
                navigator.clipboard.writeText(url);
                window.history.replaceState(null, '', link.getAttribute('href'));
                mark();
            }
        });
    });
})();
</script>
"""


def add_code_copy_buttons(body):
    """Wrap every rendered code block with a copy button, nothing else."""
    def repl(match):
        opening, inner, closing = match.groups()
        if 'nsdk-code-wrap' in opening:
            return match.group(0)      # already wrapped by the plain viewer
        return ('<div class="nsdk-code-wrap">%s%s%s%s</div>'
                % (COPY_BUTTON, opening, inner, closing))

    body = CODE_BLOCK_RE.sub(repl, body)
    return body + COPY_SCRIPT if 'nsdk-copy"' in body else body


def build_toc(body):
    """Right hand side table of contents, only when the page has enough headings."""
    headings = []
    for match in HEADING_RE.finditer(body):
        level, attrs, inner = match.group(1), match.group(2), match.group(3)
        ident = re.search(r'\bid="([^"]+)"', attrs)
        if not ident:
            continue
        label = html.unescape(re.sub(r'<[^>]*>', '', inner)).strip()
        if label:
            headings.append((int(level), ident.group(1), label))
    if len(headings) < 3:
        return ''
    items = []
    for level, anchor, label in headings:
        items.append('<li class="nsdk-toc-l%d"><a href="#%s">%s</a></li>'
                     % (level, html.escape(anchor, quote=True), html.escape(label)))
    return ('<nav class="nsdk-toc"><div class="nsdk-toc-title">Contents</div>'
            '<ul>%s</ul></nav>' % '\n'.join(items))


DECORATE_ALL = (mark_keywords, add_code_copy_buttons)


def decorate(body):
    """Post process rendered html shared by every format."""
    for step in DECORATE_ALL:
        body = step(body)
    return body


def render_markdown(fspath):
    render = markdown_backend()
    if render is None:
        return None, MD_MISSING
    return decorate(add_heading_ids(render(read_text(fspath)))), None


# extension -> pygments lexer name for the plain text viewer. Only the formats
# that actually show up in this repo are listed; anything else is not coloured
# only names that pygments actually ships are used here; an unknown name would
# silently fall back to no colour, so 'ld' is deliberately mapped to 'gas'
PLAIN_LEXERS = {
    # c / c++
    '.c': 'c', '.h': 'c', '.cc': 'cpp', '.cpp': 'cpp', '.cxx': 'cpp',
    '.hpp': 'cpp', '.hh': 'cpp', '.hxx': 'cpp', '.ino': 'cpp',
    # assembly; linker scripts are their own mini language and gas chokes on
    # them, so they fall back to plain text rather than spewing error tokens
    '.s': 'gas', '.S': 'gas', '.asm': 'nasm', '.inc': 'gas',
    # build systems
    '.mk': 'make', '.mak': 'make', '.cmake': 'cmake', '.bzl': 'python',
    'BUILD': 'python',
    # python
    '.py': 'python', '.pyi': 'python', '.pyw': 'python',
    # other languages
    '.rs': 'rust', '.js': 'javascript', '.mjs': 'javascript', '.cjs': 'javascript',
    '.ts': 'typescript', '.go': 'go', '.java': 'java', '.rb': 'ruby',
    '.pl': 'perl', '.lua': 'lua', '.tcl': 'tcl', '.ps1': 'powershell',
    '.bat': 'batch', '.cmd': 'batch', '.v': 'verilog', '.sv': 'systemverilog',
    '.dts': 'devicetree', '.dtsi': 'devicetree', '.proto': 'protobuf',
    # shells
    '.sh': 'bash', '.bash': 'bash', '.zsh': 'bash',
    # data and config
    '.json': 'json', '.yaml': 'yaml', '.yml': 'yaml', '.xml': 'xml',
    '.toml': 'toml', '.ini': 'ini', '.cfg': 'ini', '.conf': 'ini',
    '.css': 'css', '.html': 'html', '.htm': 'html',
    # patches and plain
    '.diff': 'diff', '.patch': 'diff', '.log': 'text', '.txt': 'text',
}
MAX_TEXT_LINES = 20000
# recognised by file name rather than extension
PLAIN_NAMES = {
    'Makefile': 'make', 'makefile': 'make', 'GNUmakefile': 'make',
    'CMakeLists.txt': 'cmake', 'Kconfig': 'kconfig', 'Kbuild': 'kconfig',
    'Dockerfile': 'docker', 'Doxyfile': 'ini', 'SConstruct': 'python',
    'SConscript': 'python', 'meson.build': 'python',
    'meson_options.txt': 'python',
    '.gitignore': 'text', '.gitattributes': 'text', '.gitmodules': 'ini',
}


def plain_lexer(fspath):
    name = os.path.basename(fspath)
    if name in PLAIN_NAMES:
        return PLAIN_NAMES[name]
    return PLAIN_LEXERS.get(os.path.splitext(name)[1], '')


# extensions that are always text, even when the content sniff disagrees. These
# are the formats a build/test log normally has, and a single binary byte in one
# of them must not turn the page into a download.
KNOWN_TEXT_EXTS = set(PLAIN_LEXERS) | {'.md', '.rst', '.csv', '.text', '.map'}


def is_known_text(fspath):
    return (os.path.basename(fspath) in PLAIN_NAMES
            or os.path.splitext(fspath)[1] in KNOWN_TEXT_EXTS)


# extensions served with their own MIME type (so a site keeps working) but
# still readable through the viewer when '?raw=1' is asked for
VIEWABLE_EXTS = {'.js', '.mjs', '.cjs', '.css', '.json', '.map',
                 '.svg', '.xml', '.wasm'}


def is_viewable_text(fspath):
    if os.path.splitext(fspath)[1].lower() not in VIEWABLE_EXTS:
        return False
    try:
        if os.path.getsize(fspath) > INLINE_MAX_BYTES:
            return False
    except OSError:
        return False
    return text_encoding(fspath) is not None


def clean_line(line):
    """Replace bytes that would not render with a visible marker.

    A stray binary byte (or a lone surrogate from a truncated utf-8 sequence)
    is shown as U+FFFD rather than silently breaking the page or making the
    whole file look binary.
    """
    cleaned = []
    for ch in line:
        if ch == '�' or '\ud800' <= ch <= '\udfff':
            cleaned.append('�')
        elif ord(ch) < 0x20 and ch not in '\t':
            cleaned.append('�')
        else:
            cleaned.append(ch)
    return ''.join(cleaned)


def render_plain(fspath, text=None):
    """Show a text file as a styled page with line numbers and highlighting."""
    if text is None:
        text = read_text(fspath, force=True)
    lines = [clean_line(line) for line in text.splitlines()]
    truncated = len(lines) > MAX_TEXT_LINES
    if truncated:
        lines = lines[:MAX_TEXT_LINES]

    # colour only when a lexer is known; the line number column is markup we
    # add ourselves, so the code is highlighted before it is split apart
    lexer = plain_lexer(fspath)
    code = ''
    if lexer and lexer != 'text' and any(line.replace('�', '') for line in lines):
        try:
            code = highlight_code('\n'.join(lines) + ('\n' if lines else ''), lexer, '')
        except Exception:
            code = ''
    if code:
        code = code[:-1] if code.endswith('\n') else code
        rendered = code.split('\n')
        # pygments drops trailing blank lines, which would leave fewer rows
        # than source lines and upset the per row data-text lookup below
        while len(rendered) < len(lines):
            rendered.append('')
        rendered = rendered[:len(lines)]
    else:
        rendered = [html.escape(line) for line in lines]

    body_lines = []
    for index, line in enumerate(rendered, 1):
        # each row is a linkable anchor, so 'file.c#L23' scrolls to and marks
        # that line; data-text carries the row's plain text so the copy button
        # never has to reconstruct it from the DOM, which cannot tell rows
        # apart once the newline between them is gone
        body_lines.append('<span class="nsdk-line" id="L%d" data-text="%s">'
                          '<a class="nsdk-ln" href="#L%d">%d</a>%s</span>'
                          % (index, html.escape(lines[index - 1], quote=True),
                             index, index, line))
    # nsdk-code-wrap is what the pygments stylesheet keys off, so the viewer
    # gets coloured by the same rules as markdown code blocks
    # rows are joined without a newline: <pre> keeps whitespace, so a '\n'
    # between two block rows would render as an extra blank line per row
    body = ('<div class="nsdk-plain nsdk-code-wrap">%s<pre class="nsdk-code">%s</pre></div>'
            '%s%s' % (COPY_BUTTON, ''.join(body_lines), LINE_ANCHOR_SCRIPT,
                      COPY_SCRIPT))

    notice = None
    if truncated:
        notice = ('file has more than %d lines, only the first %d are shown, '
                  "append '%s' to the url to view the whole file"
                  % (MAX_TEXT_LINES, MAX_TEXT_LINES, RAW_LINK))
    return body, notice


def render_rst(fspath):
    core = docutils_core()
    if core is None:
        return None, RST_MISSING
    parts = core.publish_parts(
        source=read_text(fspath),
        source_path=fspath,
        writer_name="html5",
        settings_overrides={
            "report_level": 5,          # do not emit docutils system messages
            "halt_level": 6,            # never raise on malformed input
            "input_encoding": "unicode",
            "output_encoding": "unicode",
            "embed_stylesheet": False,  # use the page style below instead
            "stylesheet_path": None,
            "syntax_highlight": "short",
            "file_insertion_enabled": False,  # no ".. include::" reading local files
            "raw_enabled": False,             # no ".. raw:: html" script injection
        })
    return decorate(parts["html_body"]), None


def csv_table(rows):
    rows = [row for row in rows if row]
    if not rows:
        return '<p class="nsdk-empty">(empty csv file)</p>'
    ncols = max(len(row) for row in rows)

    def cells(tag, row):
        row = list(row) + [''] * (ncols - len(row))
        return ''.join('<%s>%s</%s>' % (tag, html.escape(cell), tag) for cell in row)

    out = ['<table class="nsdk-csv">',
           '<thead><tr>%s</tr></thead>' % cells('th', rows[0]),
           '<tbody>']
    for row in rows[1:]:
        out.append('<tr>%s</tr>' % cells('td', row))
    out.append('</tbody></table>')
    return '\n'.join(out)


def render_csv(fspath, max_rows=MAX_CSV_ROWS):
    text = read_text(fspath)
    try:
        dialect = csv.Sniffer().sniff(text[:8192], delimiters=',;\t|')
    except csv.Error:
        dialect = csv.excel
    rows = []
    truncated = False
    try:
        reader = csv.reader(io.StringIO(text, newline=''), dialect)
        for index, row in enumerate(reader):
            if max_rows and index > max_rows:
                truncated = True
                break
            rows.append(row)
    except csv.Error as exc:
        return None, "failed to parse csv: %s" % exc
    notice = None
    if truncated:
        notice = ("csv file is larger than %d rows, only the header and the first "
                  "%d rows are rendered, append '%s' to the url to view the whole file"
                  % (max_rows, max_rows, RAW_LINK))
    return csv_table(rows), notice


def render_doc(fspath, max_rows=MAX_CSV_ROWS):
    ext = os.path.splitext(fspath)[1].lower()
    if ext == '.md':
        return render_markdown(fspath)
    if ext == '.rst':
        return render_rst(fspath)
    if ext == '.csv':
        return render_csv(fspath, max_rows)
    return None, None


# ---------------------------------------------------------------------------
# page building
# ---------------------------------------------------------------------------

def breadcrumb(urlpath, relpath):
    """Linked breadcrumb for urlpath, falling back to the plain path on any surprise.

    Root ('/' served from the served directory) is shown as '~' since that is the
    only name the server owns; deeper levels use the real directory names.
    """
    try:
        root_name = os.path.basename(os.path.normpath(NSDKHandler.directory_root))
        segments = [seg for seg in urlpath.split('/') if seg]
        parts = ['<a href="/">%s</a>' % html.escape(root_name or '~')]
        href = ''
        for index, segment in enumerate(segments):
            href += '/' + segment
            name = os.path.basename(urllib.parse.unquote(segment)) or segment
            if index == len(segments) - 1:
                parts.append('<span class="nsdk-current">%s</span>' % html.escape(name))
            else:
                parts.append('<a href="%s/">%s</a>'
                             % (html.escape(href, quote=True), html.escape(name)))
        return '<span class="nsdk-sep">&#8250;</span>'.join(parts)
    except Exception:
        return html.escape(relpath)


def build_page(title, relpath, body, notice=None, links='', main_class='nsdk-doc',
               crumbs=None, heading='', toc=True):
    notice_html = ""
    if notice:
        notice_html = '<div class="nsdk-notice">%s</div>' % html.escape(notice)
    if crumbs is None:
        crumbs = html.escape(relpath)
    return PAGE_TEMPLATE % {
        "title": html.escape(title),
        "css": PAGE_CSS + (pygments_css() if toc else ""),
        "pathtip": html.escape(relpath, quote=True),
        "path": crumbs,
        "links": links,
        "notice": notice_html,
        "mainclass": main_class,
        "heading": heading,
        "body": body,
        "toc": build_toc(body) if toc else "",
    }


def human_size(size):
    if size < 1024:
        return "%d B" % size
    value = float(size)
    for unit in ('K', 'M', 'G', 'T'):
        value /= 1024.0
        if value < 1024 or unit == 'T':
            return "%.1f %sB" % (value, unit)
    return "%d B" % size


ICON_COLORS = {
    'dir': '#54aeff', 'doc': '#0969da', 'code': '#8250df', 'data': '#1a7f37',
    'image': '#bc4c00', 'archive': '#bf8700', 'binary': '#6e7781',
}
# extension -> (kind, glyph). glyphs are plain ascii so no font gymnastics needed
ICON_KINDS = {
    'dir': '▸',
    'doc': '≡',
    'code': '</>',
    'data': '≣',
    'image': '▣',
    'archive': '❐',
    'binary': '■',
}
ICON_EXTS = {
    '.md': 'doc', '.rst': 'doc', '.txt': 'doc', '.log': 'doc', '.pdf': 'doc',
    '.csv': 'data', '.json': 'data', '.xml': 'data', '.yaml': 'data',
    '.yml': 'data', '.toml': 'data', '.ini': 'data', '.cfg': 'data',
    '.c': 'code', '.h': 'code', '.cc': 'code', '.cpp': 'code', '.hpp': 'code',
    '.s': 'code', '.S': 'code', '.py': 'code', '.sh': 'code', '.mk': 'code',
    '.js': 'code', '.css': 'code', '.html': 'code', '.ld': 'code',
    '.png': 'image', '.jpg': 'image', '.jpeg': 'image', '.gif': 'image',
    '.svg': 'image', '.bmp': 'image', '.ico': 'image',
    '.zip': 'archive', '.gz': 'archive', '.xz': 'archive', '.bz2': 'archive',
    '.tar': 'archive', '.7z': 'archive',
    '.o': 'binary', '.a': 'binary', '.so': 'binary', '.elf': 'binary',
    '.bin': 'binary', '.hex': 'binary', '.wasm': 'binary', '.exe': 'binary',
}


def file_icon(name, isdir):
    """Inline svg-ish badge, no external assets and no font dependency."""
    kind = 'dir' if isdir else ICON_EXTS.get(os.path.splitext(name)[1].lower(), 'binary')
    color = ICON_COLORS[kind]
    glyph = ICON_KINDS[kind]
    return ('<span class="nsdk-icon" style="color:%s" aria-hidden="true">%s</span>'
            % (color, html.escape(glyph)))


LIST_SCRIPT = """
<script>
(function () {
    var table = document.querySelector('table.nsdk-files');
    var filter = document.querySelector('.nsdk-filter');
    if (!table || !table.tBodies.length) { return; }
    var body = table.tBodies[0];
    var status = document.querySelector('.nsdk-search-status');
    if (!status) {
        status = document.createElement('div');
        status.className = 'nsdk-search-status';
        status.style.display = 'none';
        table.parentNode.insertBefore(status, table);
    }
    var state = { key: null, asc: true };

    function rows() {
        return Array.prototype.filter.call(body.rows, function (row) {
            return !row.classList.contains('nsdk-up');
        });
    }
    function value(row, key) {
        if (key === 'name') { return row.getAttribute('data-name') || ''; }
        return parseFloat(row.getAttribute('data-' + key) || '0') || 0;
    }
    function sortBy(key) {
        if (state.key === key) { state.asc = !state.asc; }
        else { state.key = key; state.asc = key === 'name'; }
        var sorted = rows().sort(function (a, b) {
            var x = value(a, key), y = value(b, key);
            var cmp = key === 'name' ? x.localeCompare(y) : x - y;
            return state.asc ? cmp : -cmp;
        });
        sorted.forEach(function (row) { body.appendChild(row); });
        Array.prototype.forEach.call(table.tHead.rows[0].cells, function (cell) {
            cell.classList.remove('nsdk-asc', 'nsdk-desc');
            if (cell.getAttribute('data-sort') === key) {
                cell.classList.add(state.asc ? 'nsdk-asc' : 'nsdk-desc');
            }
        });
    }
    Array.prototype.forEach.call(table.tHead.rows[0].cells, function (cell) {
        var key = cell.getAttribute('data-sort');
        if (key) { cell.addEventListener('click', function () { sortBy(key); }); }
    });
    if (filter) {
        // typing searches the whole subtree through the server and swaps the
        // table for the result list; clearing the box restores the directory
        var original = rows().map(function (row) { return row; });
        var timer = null;
        var seq = 0;

        function clearHits() {
            Array.prototype.forEach.call(
                body.querySelectorAll('tr.nsdk-search-row'),
                function (row) { body.removeChild(row); });
        }
        function restore() {
            clearHits();
            original.forEach(function (row) { row.style.display = ''; });
            status.style.display = 'none';
        }
        function showStatus(text) {
            status.textContent = text;
            status.style.display = text ? '' : 'none';
        }
        function render(payload) {
            var caption = payload.truncated
                ? 'showing the first ' + payload.results.length + ' matches'
                : payload.results.length + ' match' + (payload.results.length === 1 ? '' : 'es');
            showStatus(caption);
            // drop the rows of the previous query first, otherwise every new
            // search piles its hits on top of the old ones
            clearHits();
            original.forEach(function (row) { row.style.display = 'none'; });
            payload.results.forEach(function (hit) {
                var tr = document.createElement('tr');
                tr.className = 'nsdk-search-row';
                var td = document.createElement('td');
                var a = document.createElement('a');
                a.className = 'nsdk-search-hit';
                a.href = '/' + hit.path.split('/').map(encodeURIComponent).join('/') +
                         (hit.dir ? '/' : '');
                a.textContent = hit.path + (hit.dir ? '/' : '');
                td.appendChild(a);
                tr.appendChild(td);
                var sizeCell = document.createElement('td');
                sizeCell.className = 'nsdk-size';
                sizeCell.textContent = hit.size;
                tr.appendChild(sizeCell);
                tr.appendChild(document.createElement('td'));
                body.appendChild(tr);
            });
        }
        filter.addEventListener('input', function () {
            var needle = filter.value.trim();
            clearTimeout(timer);
            if (!needle) { restore(); return; }
            timer = setTimeout(function () {
                var mine = ++seq;
                fetch(location.pathname + '?search=' + encodeURIComponent(needle))
                    .then(function (r) { return r.json(); })
                    .then(function (payload) {
                        if (mine !== seq) { return; }
                        render(payload);
                    })
                    .catch(function () {});
            }, 250);
        });
    }
})();
</script>
"""


def list_page_script(body):
    return body + LIST_SCRIPT if 'class="nsdk-filter"' in body else body


# recursive search behind the filter box
SEARCH_MAX_RESULTS = 5000
SEARCH_MAX_DEPTH = 8
SEARCH_SKIP = {'.git', '.hg', '.svn', 'node_modules', '__pycache__', '.cache',
               '.tox', '.venv', 'venv', '.idea', '.vscode'}


def search_entries(root, needle, max_results=SEARCH_MAX_RESULTS,
                   max_depth=SEARCH_MAX_DEPTH):
    """Recursively find files and directories whose name contains needle.

    Returns (results, truncated) where each result is (relpath, isdir, size).
    The walk is breadth first and stops at max_results so a big tree can not
    stall the request, and depth is capped in case of symlink loops.
    """
    needle = needle.lower()
    results = []
    queue = [('', 0)]
    while queue:
        relbase, depth = queue.pop(0)
        if depth > max_depth:
            continue
        try:
            entries = sorted(os.scandir(os.path.join(root, relbase)),
                             key=lambda e: e.name.lower())
        except OSError:
            continue
        for entry in entries:
            name = entry.name
            relpath = name if not relbase else relbase + '/' + name
            try:
                isdir = entry.is_dir()
            except OSError:
                continue
            if isdir and name in SEARCH_SKIP:
                continue
            if needle in name.lower():
                if len(results) >= max_results:
                    return results, True
                try:
                    size = 0 if isdir else entry.stat().st_size
                except OSError:
                    size = 0
                results.append((relpath, isdir, size))
            if isdir:
                queue.append((relpath, depth + 1))
    return results, False


def local_ip():
    """Best effort local address, so the printed url is reachable from the lan."""
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    try:
        sock.connect(('8.8.8.8', 80))
        return sock.getsockname()[0]
    except OSError:
        return None
    finally:
        sock.close()


def host_name():
    """Return (hostname, note) for the network url.

    A hostname that resolves only to a loopback address (a very common
    /etc/hosts '127.0.1.1 <host>' entry) can not be used from another
    machine, so it is reported with a note. The short name is preferred
    over the fqdn, which is what people type anyway.
    """
    fqdn = socket.getfqdn()
    short = socket.gethostname().split('.')[0]
    for candidate in (short, fqdn):
        if not candidate:
            continue
        try:
            addrs = {info[4][0]
                     for info in socket.getaddrinfo(candidate, None, socket.AF_INET)}
        except OSError:
            continue
        physical = [addr for addr in addrs
                    if not addr.startswith('127.') and addr != '0.0.0.0']
        if physical:
            return candidate, None
        return candidate, ("only resolves to %s here, may not work from another machine"
                           % (', '.join(sorted(addrs)) or 'nothing'))
    return None, 'hostname can not be resolved'


def network_urls(port):
    """Every url a remote browser could use, best first, each with a short note.

    The hostname is always listed (it is what people type) even when its local
    resolution is loopback only, because that is a property of this machine's
    /etc/hosts, not of the network: a remote box may resolve it fine via dns.
    The ip is listed as well so there is always a url known to work.
    """
    urls = []
    name, note = host_name()
    if name:
        urls.append(("http://%s:%d/" % (name, port), note))
    ip = local_ip()
    if ip and ip != name:
        urls.append(("http://%s:%d/" % (ip, port), None))
    if not urls:
        urls.append(("http://%s:%d/" % (socket.gethostname(), port),
                     'no network address found'))
    return urls


def terminal_link(url, text):
    """OSC 8 hyperlink, so the url is clickable, but only on a real terminal."""
    if not sys.stdout.isatty():
        return text
    return '\033]8;;%s\033\\%s\033]8;;\033\\' % (url, text)


# ---------------------------------------------------------------------------
# port registry
# ---------------------------------------------------------------------------

def _parse_simple_yaml(raw):
    """Parse the simple 'ports:\\n  <path>: <port>' layout without pyyaml."""
    ports = {}
    in_ports = False
    for line in raw.splitlines():
        line = line.split('#', 1)[0].rstrip()
        if not line.strip():
            continue
        if not line.startswith(' '):
            in_ports = line.strip() == 'ports:'
            continue
        if in_ports and ':' in line:
            key, _, value = line.strip().rpartition(':')
            key = key.strip().strip("'\"")
            try:
                ports[key] = int(value.strip())
            except ValueError:
                continue
    return ports


class PortRegistry(object):
    """Remember which port a directory was served on last time."""

    # a stray temp file normally lives for milliseconds, so anything this old
    # is the leftover of a killed process and safe to delete
    STALE_TMP_SECONDS = 60

    def __init__(self, path):
        self.path = path
        self.ports = {}
        self.error = None
        self.cleanup_stale_tmp()
        self._load()

    @staticmethod
    def key(directory):
        return os.path.realpath(directory)

    def cleanup_stale_tmp(self):
        """Remove '<config>.<pid>.tmp' files left behind by a killed process.

        set() writes through a temp file and renames it into place; a process
        dying between the write and the rename leaves the temp file orphaned.
        """
        directory = os.path.dirname(self.path) or '.'
        pattern = re.compile(r'^%s\.\d+\.tmp$'
                             % re.escape(os.path.basename(self.path)))
        try:
            names = os.listdir(directory)
        except OSError:
            return
        now = time.time()
        for name in names:
            if not pattern.match(name):
                continue
            victim = os.path.join(directory, name)
            try:
                if now - os.path.getmtime(victim) < self.STALE_TMP_SECONDS:
                    continue            # maybe another server is writing it now
                os.unlink(victim)
            except OSError:
                continue

    def _load(self):
        try:
            with open(self.path, 'r', encoding='utf-8') as f:
                raw = f.read()
        except FileNotFoundError:
            return
        except OSError as exc:
            self.error = "can not read %s: %s" % (self.path, exc)
            return
        try:
            if yaml is not None:
                data = yaml.safe_load(raw)
                ports = data.get('ports') if isinstance(data, dict) else None
            elif raw.lstrip().startswith('{'):
                ports = json.loads(raw).get('ports')
            else:
                ports = _parse_simple_yaml(raw)
        except Exception as exc:
            self.error = "can not parse %s: %s" % (self.path, exc)
            return
        if not isinstance(ports, dict):
            return
        for key, value in ports.items():
            try:
                self.ports[str(key)] = int(value)
            except (TypeError, ValueError):
                continue

    def get(self, directory):
        return self.ports.get(self.key(directory))

    def _dump(self):
        data = {'version': 1, 'ports': self.ports}
        if yaml is not None:
            return ("# nsdk httpserver port registry, generated automatically, safe to delete\n"
                    + yaml.safe_dump(data, default_flow_style=False, sort_keys=True))
        return json.dumps(data, indent=2, sort_keys=True) + "\n"

    def set(self, directory, port):
        if os.path.exists(self.path):
            self._load()                    # merge updates from other servers
        self.ports[self.key(directory)] = int(port)
        tmp = "%s.%d.tmp" % (self.path, os.getpid())
        try:
            with open(tmp, 'w', encoding='utf-8') as f:
                f.write(self._dump())
            os.replace(tmp, self.path)
        except OSError as exc:
            self.error = "can not write %s: %s" % (self.path, exc)
            try:
                os.unlink(tmp)
            except OSError:
                pass
            return False
        self.error = None
        return True


# ---------------------------------------------------------------------------
# http handler
# ---------------------------------------------------------------------------

try:
    ServerClass = http.server.ThreadingHTTPServer
except AttributeError:                          # python < 3.7
    class ServerClass(socketserver.ThreadingMixIn, http.server.HTTPServer):
        daemon_threads = True


class NSDKHandler(http.server.SimpleHTTPRequestHandler):
    # https://gist.github.com/scottj/a510d6bd96941901fb99554566ee226d
    extensions_map = {
        '': 'application/octet-stream',
        '.manifest': 'text/cache-manifest',
        '.html': 'text/html; charset=utf-8',
        '.txt': 'text/plain; charset=utf-8',
        '.rst': 'text/plain; charset=utf-8',
        '.md': 'text/plain; charset=utf-8',
        '.map': 'text/plain; charset=utf-8',
        ".mk": 'text/plain; charset=utf-8',
        ".yaml": 'text/plain; charset=utf-8',
        ".yml": 'text/plain; charset=utf-8',
        '.text': 'text/plain; charset=utf-8',
        '.log': 'text/plain; charset=utf-8',
        '.csv': 'text/plain; charset=utf-8',
        '.png': 'image/png',
        '.jpg': 'image/jpg',
        '.svg': 'image/svg+xml',
        '.css': 'text/css',
        '.pdf': 'application/pdf',
        '.js': 'application/x-javascript',
        '.wasm': 'application/wasm',
        '.json': 'application/json',
        '.xml': 'application/xml',
        '.xls': 'application/vnd.openxmlformats-officedocument.spreadsheetml.sheet',
        '.xlsx': 'application/vnd.ms-excel'
    }

    directory_root = None
    max_rows = MAX_CSV_ROWS
    inline_max = INLINE_MAX_BYTES
    quiet = False
    plain = False                       # --plain: old behaviour, no rendering

    def __init__(self, *args, **kwargs):
        super().__init__(*args, directory=self.directory_root, **kwargs)

    def guess_type(self, path):
        """Like the base class, but small unknown files are shown, not downloaded.

        The browser downloads anything served as application/octet-stream, so an
        unrecognised extension is served as text/plain when the file is small and
        visibly textual. 'Content-Disposition' stays absent, so the browser keeps
        it inline and the user can still right-click to save it.
        """
        ctype = super().guess_type(path)
        if self.plain or ctype != OCTET:
            return ctype
        try:
            if self.inline_max and os.path.getsize(path) > self.inline_max:
                return ctype
        except OSError:
            return ctype
        encoding = text_encoding(path)
        if encoding:
            return 'text/plain; charset=%s' % encoding
        return ctype

    def is_plain_text(self, fspath):
        """True when this file should be wrapped in the line numbered viewer.

        Only text/plain files are wrapped. Anything with a real type of its
        own, such as css, javascript, json or an image, keeps that type so the
        browser can still load it: a docs build needs its _static/*.js and
        *.css served as what they are, not as an HTML page. Add '?raw=1' to
        see such a file through the viewer instead.

        The size check comes first on purpose: a large .txt is still served as
        text/plain by guess_type (it is a known extension, not an unknown one
        rescued by the octet-stream path), so without this limit a 100MB log
        would be pushed through the renderer.
        """
        try:
            if self.inline_max and os.path.getsize(fspath) > self.inline_max:
                return False
        except OSError:
            return False
        ext = os.path.splitext(fspath)[1].lower()
        if ext in ('.js', '.mjs', '.cjs', '.css', '.json', '.map'):
            return False
        if is_known_text(fspath):
            # a .log/.txt holding a few binary bytes must still open in the
            # viewer: refusing here is what used to turn it into a download
            return True
        try:
            return self.guess_type(fspath).startswith('text/plain')
        except OSError:
            return False

    def send_error(self, code, message=None, explain=None):
        """Same status behaviour as the base class, but a styled error page."""
        short, long_ = self.responses.get(code, ('???', '???'))
        if message is None:
            message = short
        if explain is None:
            explain = long_
        title = '%d %s' % (code, short)
        body = ('<h1>%s</h1><p>%s</p>'
                '<p class="nsdk-hint"><a href="./">Back to the directory</a></p>'
                % (html.escape(title), html.escape(explain)))
        page = build_page(title, self.path, body, main_class='nsdk-doc', toc=False)
        data = page.encode('utf-8')
        self.send_response(code, message)
        self.send_header('Content-Type', 'text/html; charset=utf-8')
        self.send_header('Content-Length', str(len(data)))
        if code >= 400 and code < 500:
            self.send_header('Connection', 'close')
        self.end_headers()
        if self.command != 'HEAD' and code >= 200 and code not in (204, 205):
            self.wfile.write(data)

    # -- transport ---------------------------------------------------------

    def handle(self):
        # a browser aborting a download must not print a traceback
        try:
            super().handle()
        except (BrokenPipeError, ConnectionResetError):
            self.close_connection = True

    def handle_one_request(self):
        try:
            super().handle_one_request()
        except (BrokenPipeError, ConnectionResetError):
            self.close_connection = True

    def log_message(self, fmt, *args):
        if not self.quiet:
            super().log_message(fmt, *args)

    # -- routing -----------------------------------------------------------

    def do_GET(self):
        if self.serve_builtin():
            return
        if self.serve_search():
            return
        if self.serve_doc():
            return
        super().do_GET()

    def do_HEAD(self):
        if self.serve_builtin(head_only=True):
            return
        if self.serve_search(head_only=True):
            return
        if self.serve_doc(head_only=True):
            return
        super().do_HEAD()

    def serve_search(self, head_only=False):
        """Recursive search endpoint used by the directory filter box.

        Only answers for a directory url, so a stray query on a file never
        turns into a search of the whole tree.
        """
        if self.plain:
            return False
        parsed = urllib.parse.urlsplit(self.path)
        # keep_blank_values so '?search=' is answered with an empty result
        # instead of falling through to the directory listing
        params = urllib.parse.parse_qs(parsed.query, keep_blank_values=True)
        if 'search' not in params:
            return False
        base = self.translate_path(parsed.path)
        if not os.path.isdir(base):
            return False
        needle = params['search'][0].strip()
        if not needle:
            results, truncated = [], False
        else:
            results, truncated = search_entries(base, needle)
        payload = json.dumps({
            'query': needle,
            'truncated': truncated,
            'results': [{'path': relpath, 'dir': isdir,
                         'size': human_size(size) if not isdir else ''}
                        for relpath, isdir, size in results],
        }).encode('utf-8')
        self.send_bytes(payload, 'application/json; charset=utf-8', head_only)
        return True

    def send_bytes(self, data, content_type, head_only=False, cache='no-cache'):
        self.send_response(200)
        self.send_header('Content-Type', content_type)
        self.send_header('Content-Length', str(len(data)))
        self.send_header('Cache-Control', cache)
        self.end_headers()
        if not head_only:
            self.wfile.write(data)

    def serve_builtin(self, head_only=False):
        """Serve the embedded favicon, unless a real file exists in the tree."""
        if self.plain:
            return False
        urlpath = urllib.parse.urlsplit(self.path).path
        if urlpath not in ('/favicon.ico', '/favicon.svg'):
            return False
        if os.path.isfile(self.translate_path(urlpath)):
            return False
        if urlpath.endswith('.svg'):
            self.send_bytes(FAVICON_SVG_BYTES, 'image/svg+xml', head_only,
                            cache='public, max-age=3600')
        else:
            self.send_bytes(favicon_ico(), 'image/x-icon', head_only,
                            cache='public, max-age=3600')
        return True

    def serve_doc(self, head_only=False):
        """Render documents as HTML, unless '?raw=1' asks for the plain file.

        *.md/*.rst/*.csv are converted by their own renderer. Text files that
        would otherwise show up as an unstyleable browser default (any file
        served as text/plain, or an unknown small file that guess_type turned
        into one) go through the line numbered plain viewer instead.

        *.html/*.htm are the exception: they are the output of a documentation
        build, so they render as web pages by default, and '?raw=1' shows the
        source through the same viewer as any other text file.

        With --plain nothing is rendered at all and every request falls through
        to the stock SimpleHTTPRequestHandler behaviour.
        """
        if self.plain:
            return False
        try:
            parsed = urllib.parse.urlsplit(self.path)
            fspath = self.translate_path(parsed.path)
            if not os.path.isfile(fspath):
                return False
            params = urllib.parse.parse_qs(parsed.query)
            raw = params.get('raw', [''])[0].lower() in RAW_VALUES
            ext = os.path.splitext(fspath)[1].lower()
            notice = None
            if ext in RENDER_EXTS:
                if raw:
                    return False           # plain file as it is
                body, notice = render_doc(fspath, self.max_rows)
                if body is None:
                    body = '<pre>%s</pre>' % html.escape(read_text(fspath))
            elif ext in ('.html', '.htm'):
                if not raw:
                    return False           # let the browser render the page
                body, notice = render_plain(fspath)
                if body is None:
                    return False
            elif self.is_plain_text(fspath):
                if raw:
                    return False
                body, notice = render_plain(fspath)
                if body is None:
                    return False           # binary after all, let it download
                if notice is None and text_encoding(fspath) is None:
                    notice = ("this file contains bytes that are not valid text, "
                              "they are shown as �, append '%s' to the url "
                              "to download the original" % RAW_LINK)
            elif raw and is_viewable_text(fspath):
                # '?raw=1' on a css/js/json file: normally served with its own
                # type so a docs build keeps working, this is the way in to
                # read the source instead
                body, notice = render_plain(fspath)
                if body is None:
                    return False
            else:
                return False
            relpath = os.path.relpath(fspath, self.directory).replace(os.sep, '/')
        except Exception as exc:
            warn("render %s failed (%s), served as plain text" % (self.path, exc))
            return False

        rawlink = parsed.path + RAW_LINK
        links = ('<a href="./">Directory</a><a href="%s">Raw</a>'
                 % html.escape(rawlink, quote=True))
        page = build_page(relpath, relpath, body, notice, links,
                          crumbs=breadcrumb(parsed.path, relpath))
        self.send_bytes(page.encode('utf-8'), 'text/html; charset=utf-8', head_only)
        return True

    def list_directory(self, path):
        """Styled directory listing with the same favicon and navigation."""
        if self.plain:
            # stock listing, exactly what the tool served before
            return super().list_directory(path)
        try:
            names = os.listdir(path)
        except OSError:
            self.send_error(404, "No permission to list directory")
            return None
        names.sort(key=lambda name: (not os.path.isdir(os.path.join(path, name)),
                                     name.lower()))

        displaypath = urllib.parse.unquote(urllib.parse.urlsplit(self.path).path)
        crumbs = breadcrumb(displaypath, displaypath)
        rows = []
        if os.path.realpath(path) != os.path.realpath(self.directory):
            rows.append('<tr class="nsdk-up"><td><a href="../">%s../</a></td>'
                        '<td class="nsdk-size"></td><td class="nsdk-time"></td></tr>'
                        % file_icon('', True))
        for name in names:
            full = os.path.join(path, name)
            try:
                stat = os.lstat(full)
            except OSError:
                continue
            isdir = os.path.isdir(full)
            href = urllib.parse.quote(name, errors='surrogatepass') + ('/' if isdir else '')
            label = file_icon(name, isdir) + html.escape(name) + ('/' if isdir else '')
            size = '-' if isdir else human_size(stat.st_size)
            mtime = time.strftime('%Y-%m-%d %H:%M', time.localtime(stat.st_mtime))
            # html renders as a page; 'src' is a shortcut to the source view
            src = ''
            if not isdir and (os.path.splitext(name)[1].lower()
                              in {'.html', '.htm'} | VIEWABLE_EXTS):
                srchref = urllib.parse.quote(name, errors='surrogatepass') + RAW_LINK
                src = ' <a class="nsdk-src" href="%s">src</a>' % html.escape(srchref, quote=True)
            # data-* lets the browser sort and filter without a round trip;
            # size is numeric and time is a sortable epoch, while the visible
            # cells stay human readable
            rows.append('<tr data-name="%s" data-size="%d" data-time="%d">'
                        '<td><a href="%s">%s</a>%s</td>'
                        '<td class="nsdk-size">%s</td>'
                        '<td class="nsdk-time">%s</td></tr>'
                        % (html.escape(name.lower(), quote=True),
                           0 if isdir else stat.st_size, int(stat.st_mtime),
                           html.escape(href, quote=True), label, src, size, mtime))
        if not names:
            rows.append('<tr><td colspan="3" class="nsdk-empty">(empty directory)</td></tr>')

        body = ('<input class="nsdk-filter" type="search" '
                'placeholder="search all files below this directory..." '
                'aria-label="search all files below this directory">\n'
                '<table class="nsdk-files">'
                '<thead><tr><th class="nsdk-sortable" data-sort="name">Name'
                '<span class="nsdk-arrow">&#9650;&#9660;</span></th>'
                '<th class="nsdk-size nsdk-sortable" data-sort="size">Size'
                '<span class="nsdk-arrow">&#9650;&#9660;</span></th>'
                '<th class="nsdk-time nsdk-sortable" data-sort="time">Modified'
                '<span class="nsdk-arrow">&#9650;&#9660;</span></th></tr></thead>'
                '<tbody>%s</tbody></table>' % '\n'.join(rows))

        # page name: the served root's name at '/', '<root>/<sub>/...' below it
        segments = [seg for seg in displaypath.split('/') if seg]
        root_name = os.path.basename(os.path.normpath(self.directory)) or '~'
        name = root_name if not segments else (root_name + '/' +
                                               '/'.join(urllib.parse.unquote(seg)
                                                        for seg in segments))
        heading = '<h1>%s</h1>' % crumbs
        # the crumb trail already links every level, so the right hand side only
        # needs the two jumps that are not levels: the root and the parent dir
        links = ['<a href="/">Home</a>'] if segments else []
        if segments:
            links.append('<a href="../">Up</a>')
        page = build_page(name, '/' + '/'.join(segments), list_page_script(body),
                          main_class='nsdk-listpage', crumbs=crumbs, heading=heading,
                          links=''.join(links), toc=False)
        self.send_bytes(page.encode('utf-8'), 'text/html; charset=utf-8',
                        head_only=self.command == 'HEAD')
        return None


# ---------------------------------------------------------------------------
# startup
# ---------------------------------------------------------------------------

def try_bind(bind, port):
    """Bind and return (server, port), or (None, None) when the port is taken."""
    try:
        server = ServerClass((bind, port), NSDKHandler)
    except OSError:
        return None, None
    return server, server.server_address[1]


def random_ports():
    """Yield every port in the range once, starting from a random offset."""
    span = PORT_MAX - PORT_MIN + 1
    offset = random.SystemRandom().randrange(span)
    for index in range(span):
        yield PORT_MIN + (offset + index) % span


def parse_args(argv=None):
    parser = argparse.ArgumentParser(
        description="Open http server on selected directory",
        epilog="*.md, *.rst and *.csv are rendered as HTML, "
               "append '?raw=1' to the URL to view the plain text")
    parser.add_argument('--directory', '-d', default=os.getcwd(),
                        help='specify alternate directory (default: current directory)')
    parser.add_argument('--port', type=int,
                        help='port to listen to (default: the port remembered for this '
                             'directory, or a random free port when that one is taken)')
    parser.add_argument('--bind', default=BIND_ALL,
                        help='address to bind, use 127.0.0.1 for local access only '
                             '(default: %(default)s)')
    parser.add_argument('--config', default=None,
                        help='port registry file, defaults to %s or the %s environment variable'
                             % (DEFAULT_CONFIG_FILE, CONFIG_ENV))
    parser.add_argument('--max-rows', type=int, default=MAX_CSV_ROWS,
                        help='max csv rows rendered as html, 0 means no limit '
                             '(default: %(default)s)')
    parser.add_argument('--open', action='store_true',
                        help='open the page in the default browser')
    parser.add_argument('--inline-max', type=int, default=INLINE_MAX_BYTES,
                        help='unknown extensions up to this size are shown as text '
                             'instead of downloaded, 0 disables it (default: %(default)s)')
    parser.add_argument('--quiet', action='store_true', help='do not log http requests')
    parser.add_argument('--plain', action='store_true',
                        help='behave like the old version: no rendering and no '
                             'directory listing changes, the remembered port is kept')
    return parser.parse_args(argv)


def config_path(explicit):
    path = explicit or os.environ.get(CONFIG_ENV) or DEFAULT_CONFIG_FILE
    return os.path.abspath(os.path.expanduser(path))


def main(argv=None):
    args = parse_args(argv)

    directory = os.path.abspath(os.path.expanduser(args.directory))
    if not os.path.isdir(directory):
        print("ERROR: Directory %s not exist, please check!" % args.directory, file=sys.stderr)
        return 1

    probe_renderers()

    NSDKHandler.directory_root = directory
    NSDKHandler.max_rows = max(0, args.max_rows)
    NSDKHandler.inline_max = max(0, args.inline_max)
    NSDKHandler.quiet = args.quiet
    NSDKHandler.plain = args.plain

    # the port registry is independent of --plain: a stable port is useful in
    # both modes, only the rendering and the page chrome are switched off
    registry = PortRegistry(config_path(args.config))

    server = None
    lisport = None
    source = None

    if args.port:
        server, lisport = try_bind(args.bind, args.port)
        if server is None:
            print("ERROR: port %d is not available on %s, either drop --port to let the "
                  "server pick a free one, or choose another port" % (args.port, args.bind),
                  file=sys.stderr)
            return 1
        source = 'specified by --port'
    else:
        remembered = registry.get(directory)
        if remembered:
            server, lisport = try_bind(args.bind, remembered)
            if server is not None:
                source = 'remembered for this directory'
            else:
                print("port %d remembered for %s is in use, selecting another one"
                      % (remembered, directory), file=sys.stderr, flush=True)
        if server is None:
            for candidate in random_ports():
                if candidate == remembered:
                    continue
                server, lisport = try_bind(args.bind, candidate)
                if server is not None:
                    source = 'randomly selected'
                    break
        if server is None:
            print("ERROR: no free port found in range %d-%d" % (PORT_MIN, PORT_MAX),
                  file=sys.stderr)
            return 1
        if lisport != remembered or not os.path.exists(registry.path):
            registry.set(directory, lisport)

    url = "http://127.0.0.1:%d/" % lisport
    info = ["HTTP server for %s" % directory,
            "  listen:  %s:%d (%s)" % (args.bind, lisport, source),
            "  local:   %s" % terminal_link(url, url)]
    if args.bind in (BIND_ALL, '', '::'):
        # show every way in and let the user pick: the hostname is what one
        # types, the ip is the one guaranteed to work from another machine
        for index, (neturl, reason) in enumerate(network_urls(lisport)):
            label = "  network: " if index == 0 else "           "
            info.append("%s%s" % (label, terminal_link(neturl, neturl)))
            if reason:
                info.append("           (%s)" % reason)
    info.append("  port registry: %s%s" % (registry.path,
                                           '' if os.path.exists(registry.path)
                                           else ' (will be created)'))
    print('\n'.join(info), flush=True)
    if registry.error:
        print("  notice: port is not remembered, %s" % registry.error,
              file=sys.stderr, flush=True)

    if args.open:
        threading.Timer(0.3, webbrowser.open, [url]).start()

    try:
        server.serve_forever()
    except KeyboardInterrupt:
        print("\nHTTP server stopped")
    finally:
        server.server_close()
    return 0


if __name__ == '__main__':
    sys.exit(main())
