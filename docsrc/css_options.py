from itertools import chain

from docutils.parsers.rst import directives
from docutils import nodes

from sphinx import addnodes
from sphinx.domains import Domain, ObjType
from sphinx.util.docfields import Field, GroupedField
from sphinx.directives import ObjectDescription
from sphinx.roles import XRefRole
from sphinx.locale import _


class CssProperty(ObjectDescription):
    required_arguments = 1

    option_spec = {
        'type': directives.unchanged,
        'default': directives.unchanged,
        'required': directives.flag,
    }

    doc_field_types = [
        Field('type', label=_('Type'),
            rolename='type', names=('type',),
            has_arg=True),
    ]

    def handle_signature(self, sig, signode):
        property_name = sig.strip()
        signode += addnodes.desc_annotation(property_name, 'property ')
        signode += addnodes.desc_name(property_name, property_name)

        value_type = self.options.get('type')
        if value_type:
            value_type = value_type.strip()
            signode += addnodes.desc_name(text=': ')
            signode += addnodes.desc_name(text='{};'.format(value_type))

        return property_name

    def _push_required_field(self):
        if 'required' not in self.options:
            return
        header = nodes.entry()
        header += nodes.Text('Required')

        value = nodes.entry()
        value += nodes.Text('YES')

        row = nodes.row()
        row += header
        row += value

        return row

    def _push_default_field(self):
        default_value = self.options.get('default')
        if default_value is None:
            return
        header = nodes.entry()
        header += nodes.Text('Default')

        value = nodes.entry()
        value += addnodes.desc_name(text=default_value)

        row = nodes.row()
        row += header
        row += value

        return row

    def transform_content(self, contentnode):
        rows = [
            self._push_required_field(),
            self._push_default_field(),
        ]
        rows = [r for r in rows if r is not None]

        if not rows:
            return

        tbody = nodes.tbody()
        tbody.extend(rows)

        tgroup = nodes.tgroup(cols=2)
        for _ in range(2):
            colspec = nodes.colspec(colwidth=1)
            tgroup += colspec
        tgroup += tbody

        table = nodes.table()
        table += tgroup

        if len(contentnode) > 0:
            contentnode.insert(0, table)
        else:
            contentnode += table


class CssDomain(Domain):
    name = 'css'
    label = 'Css'

    object_types = {
        'property': ObjType(_('property'), 'property'),
    }

    directives = {
        'property': CssProperty,
    }

    roles = {
        'property':  XRefRole(),
    }


def setup(app):
    app.add_domain(CssDomain)

    return {
        'version': 'local',
        'env_version': 1,
        'parallel_read_safe': True,
        'parallel_write_safe': True,
    }
