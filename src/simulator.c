#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "simulator.h"

// Maximum number of states you can track at once
#define MAX_STATES 1024 


/* HELPER FUNCTIONS */


/**
 * @brief Adds a state to a state-set, hence avoiding duplicates
 * 
 * Also recursively adds all states reachable (epsilon transitions)
 * 
 * @param state The state to add
 * @param set The array of state pointers
 * @param count Pointer to teh current number of states in the set
 * 
 * @returns void
 */
static void add_state_to_set(State* state, State** set, int* count) {
    if (state == NULL) {
        return;
    }

    // check if state is already in the set
    for (int i = 0; i < count; i++) {
        if (set[i] == state) {
            return; // return if theres a duplicate already present
        }
    }

    // Add the state if not present in state set
    set[(*count)++] = state;

    // if e-transitions are present, add the targets recursively
    for (int i = 0; i < state->num_transitions; i++) {
        if (state->transitions[i]->trigger_char == '\0') {
            add_state_to_set(state->transitions[i]->target_state, set, count);
        } 
    }
}

// --- Public Facing Simulator Function --- 

/**
 * @brief Simulates an NFA against a given string
 * 
 * @param nfa The NFA (Start State) to emulate
 * 
 * @returns 1 (true) if the string is accepted
 * 
 * @returns 0 (false) if the string is rejected
 */
int simulate_nfa(Nfa* nfa, const char* str) {
    // Set of states we are in right now
    State* current_states[MAX_STATES];
    // Set of states we will be in after consuming the next char
    State* next_states[MAX_STATES];

    int current_count = 0;
    int next_count = 0;

    // current set starts with NFA's start state + epsilon closure.
    add_state_to_set(nfa->start, current_states, &current_count);

    // loop through each character in the string
    for (int i = 0; str[i] != '\0'; i++) {
        char c = str[i];
        next_count = 0; // Reset the next_states set

        // For each state we are currently in...
        for (int j = 0; j < current_count; j++) {
            State* state = current_states[j];

            // ...check all its transitions...
            for (int k = 0; k < state->num_transitions; k++) {
                Transition* t = state->transitions[k];

                // ...to see if one matches the current character.
                if (t->trigger_char == c) {
                    // If it matches, add the target state (and its closure)
                    // to the *next* set of states.
                    add_state_to_set(t->target_state, next_states, &next_count);
                }
            }
        }

        // wap: The next_states become the current_states for the next loop.
        memcpy(current_states, next_states, next_count * sizeof(State*));
        current_count = next_count;

        // Optimization: If at any point our current set is empty,
        // we can never reach an accept state.
        if (current_count == 0) {
            return 0; // No Match (dead end)
        }
    }

    // 4. Final Check: After the string is exhausted, check if any
    //    of the states we are in is an accepting state.
    for (int i = 0; i < current_count; i++) {
        if (current_states[i]->is_accepting) {
            return 1; // Match!
        }
    }

    // If no current state is an accepting state, it's not a match.
    return 0; // No Match
}