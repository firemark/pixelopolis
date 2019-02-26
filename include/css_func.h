#include "css.h"

struct Rule* css_find_rule_by_query(
        struct Program* program, 
        struct RuleSelector query);
struct Prop* css_find_prop(struct Rule* rule, char* name);
struct Obj* css_get_first_obj_from_prop(struct Prop* prop);
