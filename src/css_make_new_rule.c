#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "hash.h"
#include "css_func.h"
#include "css_debug.h"

int _match_klass(struct RuleSelector* query, struct RuleSelector* iter_query) {
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

struct Rule* css_make_rule_from_selector(struct Program* program, struct RuleSelector* query) {
    struct Rule *rule = malloc(sizeof(struct Rule));
    rule->selector = query;
    rule->props = hash_make();

    char* element = query->element;
    char* pseudo_klass = query->pseudo_klass;

    struct Rule *iter_rule;
    css_iter(iter_rule, program->rules) {
        struct RuleSelector *iter_query = iter_rule->selector;

        if (iter_query->element) {
            if (!element || strcmp(element, iter_query->element))
                continue;
        } 

        if (!_match_klass(query, iter_query)) {
            continue;
        }

        if (iter_query->pseudo_klass) {
            if (!pseudo_klass || strcmp(pseudo_klass, iter_query->pseudo_klass))
                continue;
        }

        /*
        fprintf(stderr, "FOUND: ");
        css_debug_rule_selector(stderr, query);
        fprintf(stderr, " => ");
        css_debug_rule_selector(stderr, iter_query);
        fprintf(stderr, "\n");
        */

        // filtering done - this is time to copy props
        hash_update(rule->props, iter_rule->props);
    }

    return rule;
}
