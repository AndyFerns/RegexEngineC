# Project milestones

standard project milestones which act as a checklist for the project implementation

## Phase 1: Preprocessing and Parsing

- Convert the regex string from infix to postfix

```code
(a|b)*c -(Infix)
```

```code
ab|*c. -(Postfix)
```

- [ ] Insert an explicit concatenation operator (use .) where it's implied
- [ ] Implement the Shunting-Yard Algorithm (base standard for infix to postfix conversion)

## Phase 2: Building the NFA (Thompson's Construction)

- Take the postfix expression from Phase 1 and build the NFA.
- Thompson's Construction is an elegant algorithm that works perfectly with postfix notation.
- Read the postfix tokens one by one and build up the NFA using a stack

### Data Structures

- State: A node in your graph. It should have a list of transitions leaving it.

- Transition: An edge in the graph. It connects two states and is triggered by a character or is an "epsilon" (ε) transition (a free move).

- NFA: A simple structure that holds just the startState and endState of a machine fragment

### Implement thomsons constructions

1. Create a stack that holds NFA fragments.

2. Iterate through your postfix tokens:

- On seeing char's (like a), create a simple two-state NFA for it and push it onto the stack.

- On seeing an operator (|, ., *), pop the required number of NFA fragments from the stack, combine them according to the operator's rule, and push the new, larger fragment back onto the stack.

After the last token, the single NFA fragment left on the stack is the complete machine.

## Phase 3: Stimulating the NFA

- simulate it against an input string to see if there's a match.
- it's a non-deterministic machine, keep track of all possible states it could be in at any given moment

1. Handle Epsilon (ε) Transitions: A key concept is the ε-closure: the set of all states reachable from a given state using only free ε-transitions. You'll need a helper function to compute this.

2. Implement the Simulation Algorithm:

- Initialize a set of currentStates to be the ε-closure of the NFA's main start state.

- For each character in the input string:

- Create an empty set for nextStates.

- For each state in currentStates, find where the character takes you and add those destinations to nextStates.

- Update currentStates to be the ε-closure of all states in nextStates.

After the loop, if any state in your final currentStates set is the NFA's accepting state, you have a match!

---
End of Engine Logic

---

## Phase 4: CLI wrapper

Create the user-facing program

1. Set Up main.c as the entry point

2. Parse Command-Line Arguments: Your program should accept two arguments: the regex pattern and the string to test.

```bash
./my-regex-engine "(a|b)*c" "abbc"
```

3. Connect the Pieces: Your main function will call the components you built in order:

4. Pass the pattern to your parser to get the postfix expression.

5. Pass the postfix expression to your NFA constructor.

6. Pass the NFA and the input string to your simulator.

7. Print "Match" or "No Match" based on the result.