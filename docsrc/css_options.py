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
        #signode += addnodes.desc_addname(property_name, 'property ')
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
        field_body = nodes.field_body()
        field_body += nodes.Text('YES')
        field = nodes.field()
        field += nodes.field_name(text='Required')
        field += field_body

        return field

    def _push_default_field(self):
        default_value = self.options.get('default')
        if default_value is None:
            return
        default_value = default_value.strip()
        field_body = nodes.field_body()
        field_body += addnodes.desc_name(text=default_value)
        field = nodes.field()
        field += nodes.field_name(text='Default value')
        field += field_body

        return field

    def transform_content(self, contentnode):
        field_list = nodes.field_list()
        field_list += self._push_required_field()
        field_list += self._push_default_field()

        contentnode.insert(0, field_list)


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
