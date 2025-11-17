#ifndef DFA_H
#define DFA_H

#include "nfa.h" // We need this for the 'State' struct

// Arbitrary limits for our simple converter
#define MAX_DFA_STATES 256
#define MAX_NFA_STATES_PER_DFA_STATE 1024 // Should match simulator's limit

/**
 * @struct DfaState
 * @brief Represents a single state in the DFA.
 *
 * A DFA state is uniquely defined by the *set* of NFA states it represents.
 * It has a transition for each possible character (we'll use a 256-entry
 * array for all ASCII chars), which points to the *next* DfaState.
 */
typedef struct DfaState {
    int id;
    int is_accepting; // 1 if this state is an accepting state, 0 otherwise

    // The set of NFA states this DFA state represents
    State* nfa_states[MAX_NFA_STATES_PER_DFA_STATE];
    int num_nfa_states;

    // Transition table: one entry for every possible ASCII character.
    // Index by (int)c. A NULL entry means a transition to a "dead state".
    struct DfaState* transitions[256];
} DfaState;

/**
 * @struct Dfa
 * @brief Represents the entire DFA.
 *
 * It holds the start state and a list of all states
 * (for easy management and cleanup).
 */
typedef struct Dfa {
    DfaState* start_state;
    DfaState* all_states[MAX_DFA_STATES];
    int num_states;
} Dfa;

/**
 * @brief Converts a complete NFA into an equivalent DFA.
 * @param nfa The NFA to convert (uses nfa->start).
 * @return A pointer to the newly created Dfa, or NULL on failure.
 */
Dfa* nfa_to_dfa(Nfa* nfa);

/**
 * @brief Simulates a DFA against a given string.
 * This is much faster than the NFA simulation.
 * @param dfa The DFA to simulate.
 * @param str The input string.
 * @return 1 (true) if the string is accepted, 0 (false) otherwise.
 */
int simulate_dfa(Dfa* dfa, const char* str);

/**
 * @brief Frees all memory associated with a DFA.
 * @param dfa The DFA to free.
 */
void free_dfa(Dfa* dfa);

/**
 * @brief A helper function to print the DFA's structure (for debugging).
 * @param dfa The DFA to print.
 */
void print_dfa(Dfa* dfa);

#endif // DFA_H