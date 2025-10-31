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

    // Preprocess the regex to add explicit concatenation
    char preprocessed_regex[1024];
    preprocess_regex(infix_regex, preprocessed_regex, sizeof(preprocessed_regex));
    printf("Preprocessed Regex: %s\n", preprocessed_regex);

    // Convert the preprocessed regex to postfix notation
    char postfix_regex[1024];
    if (regex_to_postfix(preprocessed_regex, postfix_regex, sizeof(postfix_regex)) != 0) {
        fprintf(stderr, "Error converting to postfix.\n");
        return 1;
    }
    printf("Postfix Notation:   %s\n", postfix_regex);

    printf("\n--- Phase 2: NFA Construction (TODO) ---\n");
    // Next step: Call a function to build the NFA from postfix_regex
    // Nfa* nfa = build_nfa_from_postfix(postfix_regex);

    printf("\n--- Phase 3: Simulation (TODO) ---\n");
    // Final step: Simulate the NFA against the test string
    // int is_match = simulate_nfa(nfa, test_string);
    // printf("Result: %s\n", is_match ? "Match" : "No Match");

    return 0;
}