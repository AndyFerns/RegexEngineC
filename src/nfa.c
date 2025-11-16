#include "nfa.h"
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

// A global counter to give each state a unique ID.
static int state_id_counter = 0;

/**
 * @brief Creates a new, isolated state.
 * @return A pointer to the new State.
 */
static State* create_state() {
    State* state = (State*)malloc(sizeof(State));
    if (!state) return NULL;

    state->id = state_id_counter++;
    state->num_transitions = 0;
    state->is_accepting = 0;
    state->transitions[0] = NULL;
    state->transitions[1] = NULL;
    return state;
}

/**
 * @brief Adds a transition from a source state to a target state.
 * @param from_state The source state.
 * @param to_state The target state.
 * @param trigger The character for the transition ('\0' for epsilon).
 */
static void add_transition(State* from_state, State* to_state, char trigger) {
    if (from_state->num_transitions >= 2) {
        // According to Thompson's construction, a state should have at most two transitions.
        fprintf(stderr, "Error: Exceeded max transitions for a state.\n");
        return;
    }
    Transition* t = (Transition*)malloc(sizeof(Transition));
    if (!t) return;

    t->trigger_char = trigger;
    t->target_state = to_state;
    from_state->transitions[from_state->num_transitions++] = t;
}

/**
 * @brief Creates a new NFA fragment for a single character.
 * Visual: (start) --c--> (end)
 * @param c The character for the transition.
 * @return A pointer to the newly created Nfa fragment.
 */
static Nfa* create_nfa_for_char(char c) {
    Nfa* nfa = (Nfa*)malloc(sizeof(Nfa));
    if (!nfa) return NULL;

    nfa->start = create_state();
    nfa->end = create_state();
    add_transition(nfa->start, nfa->end, c);
    return nfa;
}

/**
 * @brief Combines two NFA fragments using the concatenation operation.
 * Links the end of nfa1 to the start of nfa2.
 * Visual: (nfa1.start) --> (nfa1.end/nfa2.start) --> (nfa2.end)
 * @param nfa1 The first NFA fragment.
 * @param nfa2 The second NFA fragment.
 * @return A new Nfa fragment representing the concatenation.
 */
static Nfa* create_nfa_for_concat(Nfa* nfa1, Nfa* nfa2) {
    // Add an epsilon transition from the end of nfa1 to the start of nfa2
    add_transition(nfa1->end, nfa2->start, '\0');
    nfa1->end->is_accepting = 0; // No longer an accepting state

    Nfa* new_nfa = (Nfa*)malloc(sizeof(Nfa));
    if (!new_nfa) return NULL;

    new_nfa->start = nfa1->start;
    new_nfa->end = nfa2->end;

    free(nfa1);
    free(nfa2);
    return new_nfa;
}

/**
 * @brief Combines two NFA fragments using the union (OR) operation.
 * Creates a new start state that branches to both nfa1 and nfa2,
 * and a new end state that both original ends point to.
 * @param nfa1 The first NFA fragment.
 * @param nfa2 The second NFA fragment.
 * @return A new Nfa fragment representing the union.
 */
static Nfa* create_nfa_for_union(Nfa* nfa1, Nfa* nfa2) {
    Nfa* new_nfa = (Nfa*)malloc(sizeof(Nfa));
    if (!new_nfa) return NULL;

    new_nfa->start = create_state();
    new_nfa->end = create_state();

    // New start state points to the start of the original NFAs
    add_transition(new_nfa->start, nfa1->start, '\0');
    add_transition(new_nfa->start, nfa2->start, '\0');

    // Original end states point to the new end state
    add_transition(nfa1->end, new_nfa->end, '\0');
    nfa1->end->is_accepting = 0;
    add_transition(nfa2->end, new_nfa->end, '\0');
    nfa2->end->is_accepting = 0;

    free(nfa1);
    free(nfa2);
    return new_nfa;
}

/**
 * @brief Applies the Kleene star operation to an NFA fragment.
 * Allows for zero or more repetitions of the original NFA.
 * @param nfa The NFA fragment to apply the star to.
 * @return A new Nfa fragment representing the star operation.
 */
static Nfa* create_nfa_for_star(Nfa* nfa) {
    Nfa* new_nfa = (Nfa*)malloc(sizeof(Nfa));
    if (!new_nfa) return NULL;

    new_nfa->start = create_state();
    new_nfa->end = create_state();

    // Epsilon from new start to new end (zero occurrences)
    add_transition(new_nfa->start, new_nfa->end, '\0');
    // Epsilon from new start to old start
    add_transition(new_nfa->start, nfa->start, '\0');

    // Epsilon from old end to new end
    add_transition(nfa->end, new_nfa->end, '\0');
    // Epsilon from old end back to old start (loop for one or more occurrences)
    add_transition(nfa->end, nfa->start, '\0');
    nfa->end->is_accepting = 0;

    free(nfa);
    return new_nfa;
}

/**
 * @brief Main function to build the NFA from a postfix expression.
 * It uses a stack-based approach.
 */
Nfa* build_nfa_from_postfix(const char* postfix) {
    Nfa* nfa_stack[1024]; // A simple stack for NFA fragments
    int stack_top = -1;

    for (int i = 0; postfix[i] != '\0'; i++) {
        char token = postfix[i];

        if (isalnum(token)) {
            // If it's an operand, create a simple NFA for it and push to stack
            nfa_stack[++stack_top] = create_nfa_for_char(token);
        } else if (token == '.') {
            // Concatenation: pop two, combine, push result
            Nfa* nfa2 = nfa_stack[stack_top--];
            Nfa* nfa1 = nfa_stack[stack_top--];
            nfa_stack[++stack_top] = create_nfa_for_concat(nfa1, nfa2);
        } else if (token == '|') {
            // Union: pop two, combine, push result
            Nfa* nfa2 = nfa_stack[stack_top--];
            Nfa* nfa1 = nfa_stack[stack_top--];
            nfa_stack[++stack_top] = create_nfa_for_union(nfa1, nfa2);
        } else if (token == '*') {
            // Star: pop one, apply star, push result
            Nfa* nfa = nfa_stack[stack_top--];
            nfa_stack[++stack_top] = create_nfa_for_star(nfa);
        }
    }

    if (stack_top != 0) {
        fprintf(stderr, "Error: NFA stack should have exactly one item at the end.\n");
        return NULL;
    }

    // The final NFA is the only item left on the stack.
    Nfa* final_nfa = nfa_stack[stack_top--];
    // Mark its end state as the one and only accepting state.
    final_nfa->end->is_accepting = 1;
    return final_nfa;
}

// TODO: Implement free_nfa to prevent memory leaks
void free_nfa(Nfa* nfa) {
    // TODO: This requires a graph traversal (like DFS or BFS) 
    // to visit and free all states and transitions to avoid memory leaks. 
    (void)nfa;

}