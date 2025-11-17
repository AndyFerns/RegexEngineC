#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "nfa.h"
#include "simulator.h"
#include "dfa.h"

int main(int argc, char* argv[]) {
    if (argc < 3 || argc > 4) {
        fprintf(stderr, "Usage: %s [--dfa] <regex_pattern> <string_to_test>\n", argv[0]);
        return 1;
    }

    int use_dfa = 0; // toggle for dfa or nfa
    const char* infix_regex;
    const char* test_string;

    if (argc == 4) {
        if (strcmp(argv[1], "--dfa") != 0) {
            fprintf(stderr, "Invalid flag. Usage: %s [--dfa] <regex...> <string...>\n", argv[0]);
            return 1;
        }
        use_dfa = 1;
        infix_regex = argv[2];
        test_string = argv[3];
    } else {
        infix_regex = argv[1];
        test_string = argv[2];
    }

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

    // relocated here to use for dfa-nfa simulation path
    int is_match = 0;

    // --- Phase 3: Choose Simulation Path ---
    if (use_dfa) {
        // --- NEW DFA PATH ---
        printf("\n--- Phase 3a: NFA->DFA Conversion ---\n");
        Dfa* dfa = nfa_to_dfa(nfa);
        if (dfa == NULL) {
            fprintf(stderr, "Error converting NFA to DFA.\n");
            // (Need to free NFA here in a real app)
            return 1;
        }
        printf("DFA constructed successfully (%d states).\n", dfa->num_states);
        
        // DFA displaying + simulation
        printf("\n--- Phase 4: DFA Simulation ---\n");
        is_match = simulate_dfa(dfa, test_string);

        // Optional: Uncomment to see the full DFA structure
        print_dfa(dfa); 


        // Clean up the DFA
        free_dfa(dfa);
        
    } else {
        // --- ORIGINAL NFA PATH ---
        printf("\n--- Phase 3b: NFA Simulation ---\n");
        is_match = simulate_nfa(nfa, test_string);
    }
    
    // (We still need to free the NFA itself)
    // free_nfa(nfa); // When you implement this

    printf("\nResult: %s\n", is_match ? "Match" : "No Match");

    // Return 0 for match (success) and 1 for no match (failure)
    return is_match ? 0 : 1; 
}