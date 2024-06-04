#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

void print_matrix(char matrix[3][3])
{
    printf("-------------\n"); // Top border
    for (size_t i = 0; i < 3; i++)
    {
        for (size_t j = 0; j < 3; j++)
        {
            printf("| %c ", matrix[i][j]);
        }
        printf("|\n");
        printf("-------------\n"); // Separator between rows
    }
    fflush(stdout);
}

char check_winner(char matrix[3][3])
{
    // Check rows
    for (int i = 0; i < 3; i++)
    {
        if (matrix[i][0] == matrix[i][1] && matrix[i][1] == matrix[i][2] && matrix[i][0] != '0')
        {
            return matrix[i][0];
        }
    }

    // Check columns
    for (int i = 0; i < 3; i++)
    {
        if (matrix[0][i] == matrix[1][i] && matrix[1][i] == matrix[2][i] && matrix[0][i] != '0')
        {
            return matrix[0][i];
        }
    }

    // Check diagonals
    if (matrix[0][0] == matrix[1][1] && matrix[1][1] == matrix[2][2] && matrix[0][0] != '0')
    {
        return matrix[0][0];
    }
    if (matrix[0][2] == matrix[1][1] && matrix[1][1] == matrix[2][0] && matrix[0][2] != '0')
    {
        return matrix[0][2];
    }

    // No winner
    return '0';
}

void check_invalid_input(char *string_num)
{
    int arr[10] = {0};
    int length = strlen(string_num);
    // check if all number between 1-9 only one represent and not represet 0 in a argv
    if (strlen(string_num) != 9)
    {
        printf("You must enter 9 numbers\n");
        exit(1);
    }

    for (size_t i = 0; i < length; i++)
    {
        if (string_num[i] < '1' || string_num[i] > '9')
        {
            printf("Each number need to be between 1-9\n");
            exit(1);
        }

        arr[string_num[i] - '0']++;
    }

    // if (arr[0] > 0)
    // {
    //     printf("Do not enter the number 0\n");
    //     exit(1);
    // }

    for (size_t i = 1; i < length; i++)
    {
        if (arr[i] != 1)
        {
            printf("Your input needs to include numbers between 1-9, each number once \n");
            exit(1);
        }
    }
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("You must enter in this format: ./ttt <strategy>\n");
        printf("\n");
        exit(1);
    }
    check_invalid_input(argv[1]);                                            // check if the input is valid
    char *strategy = argv[1];                                                // get the strategy
    char matrix[3][3] = {{' ', ' ', ' '}, {' ', ' ', ' '}, {' ', ' ', ' '}}; // Create a matrix 3x3 with spaces
    int count_of_place_empty = 0;                                            // count the number of empty place
    bool my_turn = true;
    char my_choice;

    for (size_t i = 0; i < 9; i++)
    {
        int c_pos = strategy[i] - '1'; // Convert character to index

        my_turn = true;
        if (count_of_place_empty < 8)
        {

            if (matrix[c_pos / 3][c_pos % 3] == ' ')
            {
                matrix[c_pos / 3][c_pos % 3] = 'X'; /// 1-1=0
                printf("Computer chose: (%d,%d)\n", c_pos / 3 + 1, c_pos % 3 + 1);
                print_matrix(matrix);
                if (check_winner(matrix) == 'X')
                {
                    printf("win\n");
                    exit(0);
                }
                else if (check_winner(matrix) == 'O')
                {
                    printf("lost\n");
                    exit(0);
                }
                count_of_place_empty++;
            }
            else
            {
                continue;
            }
        }
        else
        {
            for (size_t i = 8; i >= 0; i--)
            {
                if (matrix[strategy[i] / 3][strategy[i] % 3] == ' ')
                {
                    matrix[strategy[i] / 3][strategy[i] % 3] = 'X'; /// 1-1=0
                    printf("Computer chose: (%d,%d)\n", c_pos / 3 + 1, c_pos % 3 + 1);
                    print_matrix(matrix);
                    if (check_winner(matrix) == '0')
                    {
                        printf("draw\n");
                        exit(0);
                    }
                    if (check_winner(matrix) == 'X')
                    {
                        printf("win\n");
                        exit(0);
                    }
                    if (check_winner(matrix) == 'O')
                    {
                        printf("lost\n");
                        exit(0);
                    }

                    count_of_place_empty++;
                }
            }
        }

        while (my_turn)
        {
            if (scanf("%c", &my_choice) == 1)

            {
                int user_pos = my_choice - '1';

                if ((my_choice > '0' && my_choice <= '9' && matrix[user_pos / 3][user_pos % 3] == ' '))
                {
                    matrix[user_pos / 3][user_pos % 3] = 'O';
                    printf("You chose: (%d,%d)\n", user_pos / 3 + 1, user_pos % 3 + 1);
                    print_matrix(matrix);
                    if (check_winner(matrix) == 'X')
                    {
                        printf("win\n");
                        exit(0);
                    }
                    if (check_winner(matrix) == 'O')
                    {
                        printf("lost\n");
                        exit(0);
                    }
                    count_of_place_empty++;
                    my_turn = false;
                }
                else if (my_choice > '0' && my_choice <= '9' && matrix[user_pos / 3][user_pos % 3] != ' ')
                {
                    printf("You entered a number that is already taken\n");
                        fflush(stdout);

                }
            }
            else
            {
                printf("please enter one number\n");
                    fflush(stdout);

            }
        }
    }
    fflush(stdout);

}
