#ifndef NFA_H
#define NFA_H

// A single state in NFA. It can have up to two transitions.
// According to Thompsons constructions
// a state has at most two epsilon-transitions.
// Regular transitions are handled by creating a new state pair.
typedef struct State {
    int id;
    struct Transition* transitions[2];
    int num_transitions;
    int is_accepting;
} State;

// A transition represents a directed edge from one state to another.
typedef struct Transition {
    char trigger_char;      // The character that triggers this transition. '\0' for epsilon.
    State* target_state;
} Transition;

// Represents an NFA fragment with a start and end state.
// This is the fundamental unit used in Thompson's Construction.
typedef struct Nfa {
    State* start;
    State* end;
} Nfa;


/**
 * @brief Builds a complete NFA from a postfix regular expression.
 * @param postfix The postfix regex string.
 * @return A pointer to the final Nfa, or NULL on failure.
 */
Nfa* build_nfa_from_postfix(const char* postfix);

/**
 * @brief Frees all memory associated with an NFA.
 * (Important for preventing memory leaks - we will implement this later)
 * @param nfa The NFA to free.
 */
void free_nfa(Nfa* nfa);

#endif