#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pixelopolis/css_debug.h"
#include "pixelopolis/css_func.h"
#include "pixelopolis/hash.h"

static int _match(struct RuleSelector* query, struct RuleSelector* iter_query, struct RuleWithParent* parent);

static int _match_klass(struct RuleSelector* query, struct RuleSelector* iter_query) {
    if (!iter_query->klasses) return 1;
    if (!query->klasses) return 0;

    char* iter_klass;
    css_iter (iter_klass, iter_query->klasses) {
        char* klass;
        css_iter (klass, query->klasses) {               // greedy search
            if (!strcmp(iter_klass, klass)) goto found;  // match
        }
        return 0;  // not found = not match
    found:;
    }
    return 1;
}

static int _match_parent(struct RuleWithParent* parent, struct RuleSelector* parent_iter_query) {
    if (!parent_iter_query) return 1;
    if (!parent) return 0;
    return _match(parent->rule->selector, parent_iter_query, parent->parent);
}

static int _match_greedy_parent(struct RuleWithParent* parent, struct RuleSelector* greedy_parent) {
    if (!greedy_parent) return 1;
    while (parent) {
        if (_match(parent->rule->selector, greedy_parent, parent->parent)) return 1;
        parent = parent->parent;
    }
    return 0;
}

static int _match(struct RuleSelector* query, struct RuleSelector* iter_query, struct RuleWithParent* parent) {
    char* element = query->element;
    char* pseudo_klass = query->pseudo_klass;

    if (iter_query->element) {
        if (!element || strcmp(element, iter_query->element) != 0) return 0;
    }

    if (!_match_klass(query, iter_query)) {
        return 0;
    }

    if (iter_query->pseudo_klass) {
        if (!pseudo_klass || strcmp(pseudo_klass, iter_query->pseudo_klass)) return 0;
    }

    if (!_match_parent(parent, iter_query->parent)) {
        return 0;
    }

    if (!_match_greedy_parent(parent, iter_query->greedy_parent)) {
        return 0;
    }

    return 1;
}

struct RuleWithParent* css_make_rule_from_selector(struct Program* program, struct RuleSelector* query, struct RuleWithParent* parent) {
    struct RuleWithParent* rule_with_parent = MEMORY_ALLOCATE(program->memory, struct RuleWithParent);
    struct Rule* rule = MEMORY_ALLOCATE(program->memory, struct Rule);
    rule->selector = query;
    rule->props = hash_make_with_memory(program->memory);
    rule_with_parent->rule = rule;
    rule_with_parent->parent = parent;

    struct Rule* iter_rule;
    css_iter (iter_rule, program->rules) {
        struct RuleSelector* iter_query = iter_rule->selector;
        if (!_match(query, iter_query, parent)) continue;
        // DEBUG
        // css_debug_rule_selector(stderr, query);
        // fprintf(stderr, " VS ");
        // css_debug_rule(stderr, iter_rule);
        // fprintf(stderr, "\n");

        // filtering done - this is time to copy props
        hash_update(rule->props, iter_rule->props);
    }

    return rule_with_parent;
}
