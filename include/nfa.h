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
 * @brief Creates a new NFA fragment for a single character.
 * @param c The character for the transition.
 * @return A pointer to the newly created Nfa fragment.
 */
Nfa* create_nfa_for_char(char c);

/**
 * @brief Combines two NFA fragments using the concatenation operation.
 * @param nfa1 The first NFA fragment.
 * @param nfa2 The second NFA fragment.
 * @return A new Nfa fragment representing the concatenation.
 */
Nfa* create_nfa_for_concat(Nfa* nfa1, Nfa* nfa2);

/**
 * @brief Combines two NFA fragments using the union (OR) operation.
 * @param nfa1 The first NFA fragment.
 * @param nfa2 The second NFA fragment.
 * @return A new Nfa fragment representing the union.
 */
Nfa* create_nfa_for_union(Nfa* nfa1, Nfa* nfa2);

/**
 * @brief Applies the Kleene star operation to an NFA fragment.
 * @param nfa The NFA fragment to apply the star to.
 * @return A new Nfa fragment representing the star operation.
 */
Nfa* create_nfa_for_star(Nfa* nfa);

#endif