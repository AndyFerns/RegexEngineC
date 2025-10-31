#include <stdio.h>
#include <stdlib.h>
#include "parser.h"
#include "nfa.h"
#include "simulator.h"

int main(int argc, char* argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <regex_pattern> <string_to_test>\n", argv[0]);
        return 1;
    }

    const char* infix_regex = argv[1];
    const char* test_string = argv[2];

    printf("Starting regex engine...\n\n");
    printf("Input Infix Regex:  %s\n", infix_regex);
    printf("String to test:     %s\n", test_string);
    printf("\n--- Phase 1: Parsing ---\n");

    char preprocessed_regex[1024];
    preprocess_regex(infix_regex, preprocessed_regex, sizeof(preprocessed_regex));
    printf("Preprocessed Regex: %s\n", preprocessed_regex);

    char postfix_regex[1024];
    if (regex_to_postfix(preprocessed_regex, postfix_regex, sizeof(postfix_regex)) != 0) {
        fprintf(stderr, "Error converting to postfix.\n");
        return 1;
    }
    printf("Postfix Notation:   %s\n", postfix_regex);

    printf("\n--- Phase 2: NFA Construction ---\n");
    Nfa* nfa = build_nfa_from_postfix(postfix_regex);

    if (nfa) {
        printf("NFA constructed successfully!\n");
        printf("Start State ID: %d, End State ID: %d\n", nfa->start->id, nfa->end->id);
    } else {
        fprintf(stderr, "NFA construction failed.\n");
        return 1;
    }

    printf("\n--- Phase 3: Simulation (TODO) ---\n");
    // Final step: Simulate the NFA against the test string
    // int is_match = simulate_nfa(nfa, test_string);
    // printf("Result: %s\n", is_match ? "Match" : "No Match");

    // We will implement a proper free function later to avoid memory leaks.
    // free_nfa(nfa);

    return 0;
}