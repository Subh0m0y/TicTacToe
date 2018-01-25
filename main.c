#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#define WIDTH 3
#define SIZE WIDTH * WIDTH
#define MOVE_COUNT 8

const char *DIVIDER = "\n---+---+---\n";
const char *NUM_STR = "123456789";

const char EMPTY = ' ';
const char PLAYER1 = 'X';
const char PLAYER2 = 'O';

/**
 * The container of all the positions that must
 * be occupied by a player to score a win.
 */
const size_t WINNING_MOVES[MOVE_COUNT][3] = {
        // Horizontal streaks
        {0, 1, 2},
        {3, 4, 5},
        {6, 7, 8},
        // Vertical streaks
        {0, 3, 6},
        {1, 4, 7},
        {2, 5, 8},
        // Diagonal streaks
        {0, 4, 8},
        {2, 4, 6}

};

typedef struct state_t
{
    char values[SIZE];
    struct state_t *next;
} State;

/**
 * @return A new State with all blanks and navigation cleared.
 */
State *new_state()
{
    State *state = malloc(sizeof(State));
    for (size_t i = 0; i < SIZE; ++i)
    {
        state->values[i] = EMPTY;
    }
    state->next = NULL;
    return state;
}

/**
 * @param state The parent state.
 * @param index The (free) position to mark.
 * @param player The player which sill mark the cell.
 * @return A new state from a parent with the requested
 * position marked by the given player mark.
 */
State *derive(State *state, size_t index, char player)
{
    state->next = new_state();
    // Copy values
    for (size_t i = 0; i < SIZE; ++i)
    {
        state->next->values[i] = state->values[i];
    }
    state = state->next;
    state->values[index] = player;

    return state;
}

/**
 * Frees the space occupied by the given State and its
 * successors.
 * @param state The state to deallocate.
 */
void free_state(State *state)
{
    if (state == NULL)
        return;
    free(state->values);
    free(state->next);
    state->next = NULL;
    free(state);
}

/**
 * Pretty prints the given state with evenly spaced
 * symbols and dividers and indices in place of blanks.
 * @param state The state to be pretty-printed.
 */
void print_state(State *state)
{
    for (size_t row = 0; row < WIDTH; ++row)
    {
        if (row != 0)
        {
            printf(DIVIDER);
        }
        for (size_t col = 0; col < WIDTH; ++col)
        {
            if (col != 0)
            { printf("|"); }
            size_t index = row * WIDTH + col;
            char value = state->values[index];
            // If the space is empty, print the corresponding index to
            // access that space. Otherwise, print X or O.
            printf(" %c ", (value == EMPTY ? NUM_STR[index] : value));
        }
    }
    printf("\n\n");
}

/**
 * @param player The player who's win to check.
 * @param state The state to check for win in.
 * @return true if the player has won.
 */
bool has_won(char player, State *state)
{
    bool done = false;
    for (size_t move = 0; !done && move < MOVE_COUNT; ++move)
    {
        size_t count = 0;
        for (size_t index = 0; index < WIDTH; ++index)
        {
            size_t move_idx = WINNING_MOVES[move][index];
            if (state->values[move_idx] == player)
            {
                count++;
            } else // no point in checking any further for this move
            {
                break;
            }
        }
        if (count == WIDTH)
        {
            done = true;
        }
    }
    if (done)
    {
        print_state(state);
        printf("\n%c wins!!\n", player);
    }
    return done;
}

/**
 * Creates a new state from the given state using input
 * provided by the human player.
 *
 * @param player The character designated to the human player.
 * @param current The current state to move from.
 * @return A new state based on the input given by the human player.
 */
State *take_input(char player, State *current)
{
    printf("%c's turn!\n", player);
    print_state(current);
    printf("\nEnter the position you want to mark : ");
    size_t index;
    scanf("%d", &index); // NOLINT

    while (index < 1 || index > SIZE || current->values[index - 1] != EMPTY) {
        printf("Invalid position! Try again : ");
        scanf("%d", &index); // NOLINT
    }

    return derive(current, index - 1, player);
}

int main()
{
    State *state = new_state();
    if (state == NULL)
    {
        fprintf(stderr, "Critical error. Unable to allocate memory.\n");
        return EXIT_FAILURE;
    }
    State *head = state;
    char player = PLAYER1;

    while (!has_won(PLAYER1, state) && !has_won(PLAYER2, state))
    {
        state = take_input(player, state);

        if (state == NULL)
        {
            fprintf(stderr, "Error occurred while trying to take input. Exiting.\n");
            return EXIT_FAILURE;
        }

        // Change player
        player = player == PLAYER1 ? PLAYER2 : PLAYER1;
    }
    free_state(head);
    return EXIT_SUCCESS;
}