from typing import AbstractSet, Any

from docutils.nodes import emphasis, inline, literal, Text
from docutils.parsers.rst.directives import unchanged, unchanged_required

from re import split

from sphinx.addnodes import desc_name
from sphinx.application import Sphinx
from sphinx.directives import ObjectDescription
from sphinx.domains import Domain
from sphinx.util.docutils import SphinxRole
from sphinx.util.typing import ExtensionMetadata

class OrganicFunction(ObjectDescription):
  has_content = True
  required_arguments = 1

  option_spec = {
    "return": unchanged
  }

  def handle_signature(self, signature, signode):
    signode.append(desc_name(text = signature))

    if "return" in self.options:
      signode.append(Text(" -> "))
      signode.extend(self.parse_inline(self.options["return"])[0])

    return signature

  def add_target_and_index(self, _, signature, signode):
    signode["ids"].append(f"function-{signature}")

class OrganicInput(ObjectDescription):
  has_content = True
  required_arguments = 1

  option_spec = {
    "type": unchanged_required,
    "default": unchanged,
    "fillable": unchanged
  }

  def handle_signature(self, signature, signode):
    node = inline()

    node.append(emphasis(text = signature, classes = [ "mono" ]))
    node.append(inline(text = ":", classes = [ "mono", "separator" ]))
    node.extend(self.parse_inline(self.options["type"])[0])

    if "default" in self.options:
      node.append(Text(" = "))
      node.extend(self.parse_inline(self.options["default"])[0])
    else:
      signode["classes"].append("required")

    if "fillable" in self.options:
      fillable = split("\\s*~\\s*", self.options["fillable"])

      node.append(Text(" (|"))
      node.append(emphasis(text = fillable[0], classes = [ "mono" ]))
      node.append(Text("|: "))
      node.extend(self.parse_inline(fillable[1])[0])
      node.append(Text(")"))

    signode.append(node)

    return signature

  def add_target_and_index(self, _, signature, signode):
    signode["ids"].append(f"input-{signature}")

class OrganicFunctionHighlight(SphinxRole):
  def run(self):
    node = literal(classes = [
      "code",
      "highlight",
      "organic-code",
      "organic",
      "highlight-organic"
    ])

    node.append(inline(text = self.text, classes = [ "nf" ]))

    return [ node ], []

class OrganicMono(SphinxRole):
  def run(self):
    return [ inline(text = self.text, classes = [ "mono" ]) ], []

class OrganicDomain(Domain):
  name = "organic"
  label = "Organic"
  data_version = 0

  directives = {
    "function": OrganicFunction,
    "input": OrganicInput
  }

  roles = {
    "mono": OrganicMono(),
    "code:function": OrganicFunctionHighlight()
  }

  def get_full_qualified_name(self, node):
    return f"organic.{node.arguments[0]}"

  def merge_domaindata(self, docnames, otherdata):
    pass

def setup(app: Sphinx) -> ExtensionMetadata:
  app.add_domain(OrganicDomain)

  return {
    "version": "0.1.0",
    "parallel_read_safe": True,
    "parallel_write_safe": True
  }
