#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "hash.h"
#include "css_func.h"
#include "css_debug.h"

int _match(struct RuleSelector* query, struct RuleSelector* iter_query);

int _match_klass(struct RuleSelector* query, struct RuleSelector* iter_query) {
    if (!iter_query->klasses) return 1;
    if (!query->klasses) return 0;

    char* iter_klass;
    css_iter(iter_klass, iter_query->klasses) {
        char* klass;
        css_iter(klass, query->klasses) { // greedy search
            if (!strcmp(iter_klass, klass)) goto found; // match
        }
        return 0; // not found = not match
        found:;
    }
    return 1;
}

int _match_parent(struct RuleSelector* parent_query, struct RuleSelector* parent_iter_query) {
    if (!parent_iter_query) return 1;
    if (!parent_query) return 0;
    return _match(parent_query, parent_iter_query);
}

int _match_greedy_parent(struct RuleSelector* parent_query, struct RuleSelector* greedy_parent) {
    if (!greedy_parent) return 1;
    while (parent_query) {
        if (_match(parent_query, greedy_parent)) return 1;
        parent_query = parent_query->parent;
    }
    return 0;
}

int _match(struct RuleSelector* query, struct RuleSelector* iter_query) {
    char* element = query->element;
    char* pseudo_klass = query->pseudo_klass;

    if (iter_query->element) {
        if (!element || strcmp(element, iter_query->element))
            return 0;
    }

    if (!_match_klass(query, iter_query)) {
        return 0;
    }

    if (iter_query->pseudo_klass) {
        if (!pseudo_klass || strcmp(pseudo_klass, iter_query->pseudo_klass))
            return 0;
    }

    if (!_match_parent(query->parent, iter_query->parent)) {
        return 0;
    }

    if (!_match_greedy_parent(query->parent, iter_query->greedy_parent)) {
        return 0;
    }

    return 1;
}

struct Rule* css_make_rule_from_selector(struct Program* program, struct RuleSelector* query) {
    struct Rule *rule = malloc(sizeof(struct Rule));
    rule->selector = query;
    rule->props = hash_make();

    struct Rule *iter_rule;
    css_iter(iter_rule, program->rules) {
        struct RuleSelector* iter_query = iter_rule->selector;
        if (!_match(query, iter_query)) continue;
        // filtering done - this is time to copy props
        // DEBUG
        //css_debug_rule_selector(stderr, query);
        //fprintf(stderr, " VS ");
        //css_debug_rule(stderr, iter_rule);
        //fprintf(stderr, "\n");
        hash_update(rule->props, iter_rule->props);
    }

    return rule;
}
