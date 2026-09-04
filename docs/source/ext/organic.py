from docutils.nodes import emphasis, inline, literal, raw, Text
from docutils.parsers.rst.directives import unchanged, unchanged_required

from re import split

from sphinx.addnodes import desc_name
from sphinx.application import Sphinx
from sphinx.directives import ObjectDescription
from sphinx.domains import Domain
from sphinx.util.typing import ExtensionMetadata

class OrganicFunction(ObjectDescription):
  has_content = True
  required_arguments = 1

  option_spec = {
    "required": unchanged,
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

  def parse_content_to_nodes(self, allow_section_headings):
    nodes = []

    if "required" in self.options:
      for desc in split(",\\s*", self.options["required"]):
        param = split("\\s*~\\s*", desc)

        node = inline()

        node.append(emphasis(text = param[0], classes = [ "mono" ]))
        node.append(inline(text = ":", classes = [ "mono", "separator" ]))

        tpe = inline(classes = [ "mono" ])

        tpe.extend(self.parse_inline(param[1])[0])

        node.append(tpe)

        nodes.append(node)
        nodes.append(raw(text = "<br>", format = "html"))

    nodes.extend(super().parse_content_to_nodes(allow_section_headings))

    return nodes

class OrganicInput(ObjectDescription):
  has_content = True
  required_arguments = 1

  option_spec = {
    "type": unchanged_required,
    "default": unchanged_required
  }

  def handle_signature(self, signature, signode):
    node = inline()

    node.append(emphasis(text = signature, classes = [ "mono" ]))
    node.append(Text(": "))
    node.extend(self.parse_inline(self.options["type"])[0])
    node.append(Text(" = "))
    node.append(literal(text = self.options["default"]))

    signode.append(node)

    return signature

  def add_target_and_index(self, _, signature, signode):
    signode["ids"].append(f"input-{signature}")

class OrganicDomain(Domain):
  name = "organic"
  label = "Organic"
  data_version = 0

  directives = {
    "function": OrganicFunction,
    "input": OrganicInput
  }

  def get_full_qualified_name(self, node):
    return f"organic.{node.arguments[0]}"

def setup(app: Sphinx) -> ExtensionMetadata:
  app.add_domain(OrganicDomain)

  return {
    "version": "0.1.0",
    "parallel_read_safe": True,
    "parallel_write_safe": True
  }
