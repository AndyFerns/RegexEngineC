#include "dfa.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// --- Static Globals for the Builder ---
// These are used only during the nfa_to_dfa conversion process.
static Dfa* dfa_graph = NULL;
static DfaState* worklist[MAX_DFA_STATES];
static int worklist_head = 0;
static int worklist_tail = 0;

// --- Helper Functions ---

/**
 * @brief (Copied from simulator.c)
 * Recursively adds a state and its epsilon-closure to a set.
 */
static void add_state_to_set(State* state, State** set, int* count) {
    if (state == NULL) return;
    for (int i = 0; i < *count; i++) {
        if (set[i] == state) return; // Already present
    }
    set[(*count)++] = state;

    for (int i = 0; i < state->num_transitions; i++) {
        if (state->transitions[i]->trigger_char == '\0') {
            add_state_to_set(state->transitions[i]->target_state, set, count);
        }
    }
}

/**
 * @brief Comparison function for qsort(), to sort NFA states by their ID.
 */
static int state_ptr_compare(const void* a, const void* b) {
    State* stateA = *(State**)a;
    State* stateB = *(State**)b;
    return stateA->id - stateB->id;
}

/**
 * @brief Sorts a set of NFA state pointers by their state ID.
 * This allows us to easily compare two sets.
 */
static void sort_nfa_state_set(State** set, int count) {
    qsort(set, count, sizeof(State*), state_ptr_compare);
}

/**
 * @brief Checks if two (sorted) NFA state sets are identical.
 */
static int are_sets_equal(State** set1, int count1, State** set2, int count2) {
    if (count1 != count2) return 0;
    for (int i = 0; i < count1; i++) {
        if (set1[i]->id != set2[i]->id) return 0;
    }
    return 1;
}

/**
 * @brief Finds if a DFA state for a given NFA-state-set already exists.
 */
static DfaState* find_dfa_state_by_nfa_set(State** set, int count) {
    // We must sort the input set to compare it against the (already sorted)
    // sets stored in the existing DFA states.
    sort_nfa_state_set(set, count);

    for (int i = 0; i < dfa_graph->num_states; i++) {
        if (are_sets_equal(dfa_graph->all_states[i]->nfa_states,
                           dfa_graph->all_states[i]->num_nfa_states,
                           set, count)) {
            return dfa_graph->all_states[i];
        }
    }
    return NULL; // Not found
}

/**
 * @brief Creates a new DfaState, adds it to the graph, and to the worklist.
 */
static DfaState* create_dfa_state(State** set, int count) {
    if (dfa_graph->num_states >= MAX_DFA_STATES) {
        fprintf(stderr, "Error: Maximum number of DFA states reached.\n");
        return NULL;
    }

    DfaState* dfa_state = (DfaState*)malloc(sizeof(DfaState));
    if (!dfa_state) {
        perror("Failed to allocate DfaState");
        return NULL;
    }

    // Initialize the DfaState
    dfa_state->id = dfa_graph->num_states;
    dfa_state->num_nfa_states = count;
    dfa_state->is_accepting = 0;
    memset(dfa_state->transitions, 0, sizeof(dfa_state->transitions)); // All transitions are NULL (dead)

    // Copy the NFA state set and sort it
    memcpy(dfa_state->nfa_states, set, count * sizeof(State*));
    sort_nfa_state_set(dfa_state->nfa_states, dfa_state->num_nfa_states);

    // Check if this new DFA state is an accepting state
    for (int i = 0; i < count; i++) {
        if (dfa_state->nfa_states[i]->is_accepting) {
            dfa_state->is_accepting = 1;
            break;
        }
    }

    // Add to the main graph and worklist
    dfa_graph->all_states[dfa_graph->num_states++] = dfa_state;
    worklist[worklist_tail++] = dfa_state;

    return dfa_state;
}


// --- Public Functions ---

Dfa* nfa_to_dfa(Nfa* nfa) {
    // 1. Initialize the global DFA graph and worklist
    dfa_graph = (Dfa*)malloc(sizeof(Dfa));
    if (!dfa_graph) {
        perror("Failed to allocate Dfa");
        return NULL;
    }
    dfa_graph->num_states = 0;
    dfa_graph->start_state = NULL;
    memset(dfa_graph->all_states, 0, sizeof(dfa_graph->all_states));
    
    worklist_head = 0;
    worklist_tail = 0;

    // 2. Create the DFA's start state.
    // This is the epsilon-closure of the NFA's start state.
    State* start_set[MAX_NFA_STATES_PER_DFA_STATE];
    int start_count = 0;
    add_state_to_set(nfa->start, start_set, &start_count);
    
    dfa_graph->start_state = create_dfa_state(start_set, start_count);

    // 3. Process the worklist (Subset Construction Algorithm)
    while (worklist_head < worklist_tail) {
        DfaState* current_dfa_state = worklist[worklist_head++];

        // We must check transitions for every possible character.
        // For simplicity, we check all printable ASCII characters.
        for (int c = 32; c < 127; c++) {
            
            // This set will hold the *next* set of NFA states
            State* next_set[MAX_NFA_STATES_PER_DFA_STATE];
            int next_count = 0;

            // For each NFA state 's' in our current DFA state...
            for (int i = 0; i < current_dfa_state->num_nfa_states; i++) {
                State* nfa_s = current_dfa_state->nfa_states[i];

                // ...check all its transitions...
                for (int j = 0; j < nfa_s->num_transitions; j++) {
                    Transition* t = nfa_s->transitions[j];
                    
                    // ...to see if one matches the character 'c'.
                    if (t->trigger_char == (char)c) {
                        // If it matches, add the *epsilon-closure* of the
                        // target state to our 'next_set'.
                        add_state_to_set(t->target_state, next_set, &next_count);
                    }
                }
            }

            // If next_count is 0, there is no transition on this char.
            // We leave current_dfa_state->transitions[c] as NULL (dead state).
            if (next_count == 0) {
                continue;
            }

            // We have a valid next state. See if we've already created
            // a DFA state for this exact set of NFA states.
            DfaState* target_dfa_state = find_dfa_state_by_nfa_set(next_set, next_count);

            if (target_dfa_state == NULL) {
                // This is a new DFA state. Create it.
                target_dfa_state = create_dfa_state(next_set, next_count);
            }

            // Create the transition in the DFA
            current_dfa_state->transitions[c] = target_dfa_state;
        }
    }

    // 4. Conversion is complete. Return the DFA graph.
    return dfa_graph;
}

int simulate_dfa(Dfa* dfa, const char* str) {
    DfaState* current_state = dfa->start_state;

    for (int i = 0; str[i] != '\0'; i++) {
        char c = str[i];
        
        // This is the core of DFA simulation: a simple array lookup.
        current_state = current_state->transitions[(int)c];

        // If the transition is NULL, we've gone to a "dead state".
        if (current_state == NULL) {
            return 0; // No Match
        }
    }

    // After the string is done, are we in an accepting state?
    return current_state->is_accepting;
}

void free_dfa(Dfa* dfa) {
    if (!dfa) return;
    // Free each DfaState
    for (int i = 0; i < dfa->num_states; i++) {
        free(dfa->all_states[i]);
    }
    // Free the container struct
    free(dfa);
}

void print_dfa(Dfa* dfa) {
    printf("DFA Structure (%d states):\n", dfa->num_states);
    printf("  Start State: S%d\n", dfa->start_state->id);
    
    printf("  Accepting States:");
    for (int i = 0; i < dfa->num_states; i++) {
        if (dfa->all_states[i]->is_accepting) {
            printf(" S%d", dfa->all_states[i]->id);
        }
    }
    printf("\n\n  Transitions:\n");

    for (int i = 0; i < dfa->num_states; i++) {
        DfaState* s = dfa->all_states[i];
        printf("    State S%d (NFA states: {", s->id);
        for(int j = 0; j < s->num_nfa_states; j++) {
            printf("%d%s", s->nfa_states[j]->id, (j == s->num_nfa_states - 1) ? "" : ",");
        }
        printf("}) %s\n", s->is_accepting ? "[ACCEPT]" : "");

        // Print transitions for this state
        for (int c = 32; c < 127; c++) {
            if (s->transitions[c] != NULL) {
                printf("      '%c' -> S%d\n", (char)c, s->transitions[c]->id);
            }
        }
    }
}