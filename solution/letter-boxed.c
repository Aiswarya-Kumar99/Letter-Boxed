#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define noOfLetters 26

//Declaration
char **gameBoard;
int sides = 0;
int inputWords = 0; 

// Linked list for dict
typedef struct dictNode {
    char word[100];
    struct dictNode *next;
} dictNode;

//Count the number of rows in the board file
int sideCount(const char *filePtr) {
    FILE *file = fopen(filePtr, "r");
    if (!file) {
        return -1;
    }

    char row[noOfLetters];
    int count = 0;

    while (fgets(row, sizeof(row), file)) {
        count++;
    }

    fclose(file);
    return count;
}

//Read the board file
int readBoardFile(const char *filePtr) {

    //Get the number of sides present on the board
    sides = sideCount(filePtr);

    //Allocate memory for the board
    gameBoard = malloc(sides * sizeof(char *));
    if (!gameBoard) {
        return 1;
    }

    //Read the file's contents
    FILE *file = fopen(filePtr, "r");
    if (!file) {
        return 1;
    }

    //Check if there are more than 3 sides
    if (sides < 3) {
        printf("Invalid board\n");
        return 1;
    }

    char row[noOfLetters];
    int currentSide = 0;
    while (fgets(row, sizeof(row), file)) {
        row[strcspn(row, "\n")] = '\0';  //Replace the new line character with null terminator

        // Allocate memory
        gameBoard[currentSide] = malloc((strlen(row) + 1) * sizeof(char));  //Add 1 for null terminator
        strcpy(gameBoard[currentSide++], row);  //Insert the row into the board
    }

    fclose(file);
    return 0;
}


//Read the dictionary file using linked list
dictNode *readDictFile(const char *filePtr) {
    FILE *file = fopen(filePtr, "r");

    dictNode *head = NULL;
    dictNode *tail = NULL;
    char dictWord[100];

    while (fgets(dictWord, sizeof(dictWord), file)) {
        dictNode *newNode = malloc(sizeof(dictNode));
        dictWord[strcspn(dictWord, "\n")] = '\0';  //Replace the new line character with null terminator
        strcpy(newNode->word, dictWord);
        newNode->next = NULL;

        if (!head){
            head = newNode;
        } 
        else {
            tail->next = newNode;
        }

        tail = newNode;
    }

    fclose(file);
    return head;
}

//Function to check if there are repeated letters in the board
int checkRepeatedLetters(void) {
    int usedLetter[noOfLetters] = {0};
    for (int i = 0; i < sides; i++) {
        for (int j = 0; gameBoard[i][j] != '\0'; j++) {
            char c = gameBoard[i][j];
            if (usedLetter[c - 'a']) {
                printf("Invalid board\n");
                return 1;
            }
            usedLetter[c - 'a'] = 1;
        }
    }
    return 0;
}

//Function to validate if a word uses valid letters present on the board
int validLetters(const char *word) {
    int usedLetter[noOfLetters] = {0};

    // Count occurrences of each letter in the board
    for (int i = 0; i < sides; i++) {
        for (int j = 0; gameBoard[i][j] != '\0'; j++) {
            usedLetter[gameBoard[i][j] - 'a'] = 1;
        }
    }

    // Check if each letter in the word is present on the board
    for (int i = 0; word[i] != '\0'; i++) {
        if (!usedLetter[word[i] - 'a']) {
            printf("Used a letter not present on the board\n");
            return 1;
        }
    }
    return 0;
}

// Function to check if the word contains consecutive letters from the same side
int consecutiveLetters(const char *word) {
    int prevSide = -1;
    for (int i = 0; word[i] != '\0'; i++) {
        int currSide = -1;

        // Get the side where the letter is present
        for (int side = 0; side < sides; side++) {
            if (strchr(gameBoard[side], word[i])) {
                currSide = side;
                break;
            }
        }

        if (prevSide == currSide) {
            printf("Same-side letter used consecutively\n");
            return 1;
        }
        prevSide = currSide;
    }
    return 0;
}

//Function to validate the input words
int validateWord(char *word, char *lastLetter) {

    if (validLetters(word)) {
        return 1;
    }

    if (*lastLetter && word[0] != *lastLetter) {
        printf("First letter of word does not match last letter of previous word\n");
        return 1;
    }

    if (consecutiveLetters(word)) {
        return 1;
    }
    
    *lastLetter = word[strlen(word) - 1]; //Update the last letter
    return 0;
}


//Function to check if a word is present in the dictionary
int wordInDictionary(dictNode *dict, const char *word) {
    for (dictNode *node = dict; node != NULL; node = node->next) {
        if (strcmp(node->word, word) == 0) {
            return 0;
        }
    }
    return 1;
}

void freeBoardMemory(void){
    for (int i = 0; i < sides; i++) {
        free(gameBoard[i]);
    }
    free(gameBoard);
}

void freeDictionary(dictNode *head) {
    dictNode *curr = head;
    dictNode *next;

    while (curr != NULL) {
        next = curr->next; 
        free(curr); 
        curr = next; 
    }
}

//Main function
int main(int argc, char *argv[]) {
    if (argc != 3) {
        return 1;
    }

    //Read the board and check if it is valid
    if (readBoardFile(argv[1]) != 0 ) {
        return 1;
    }

    //Check for repeated letters
    if (checkRepeatedLetters() !=0) {
        freeBoardMemory();
        return 1;
    }

    //Read Dictionary
    FILE *file = fopen(argv[2], "r");
    if (!file) {
        return 1; // Error opening the file
    }
    
    dictNode *dictionary = readDictFile(argv[2]);

    char lastLetter = '\0';
    char inputWord[100];
    int lettersUsed[noOfLetters] = {0};
    int sucessFlag = 1;

    //Check each word from the input file
    while (fgets(inputWord, sizeof(inputWord), stdin)) {
        inputWord[strcspn(inputWord, "\n")] = '\0'; //Replace the new line character with null terminator

        if(sucessFlag==1){
        //Check if the word is valid
        if (validateWord(inputWord, &lastLetter)) {
            freeBoardMemory();
            freeDictionary(dictionary);
            return 0;
        }

        //Check if the word is present in the dictionary
        if (wordInDictionary(dictionary, inputWord)) {
            printf("Word not found in dictionary\n");
            freeBoardMemory();
            freeDictionary(dictionary);
            return 0;
        }

        //Mark letters that are used
        for (int i = 0; inputWord[i] != '\0'; i++) {
            lettersUsed[inputWord[i] - 'a'] = 1;
        }}

        sucessFlag = 0;
        //Check if all letters on the board are used
        for (int i = 0; i < sides; i++) {
            for (int j = 0; gameBoard[i][j] != '\0'; j++) {
                if (!lettersUsed[gameBoard[i][j] - 'a']) {
                    sucessFlag = 1;
                    break;
                }
            }
        }

        if(sucessFlag==0)
        {
            printf("Correct\n");
            freeBoardMemory();
            freeDictionary(dictionary);
            return 0;
        }

    }

    //Check if all letters on the board are used
    for (int i = 0; i < sides; i++) {
        for (int j = 0; gameBoard[i][j] != '\0'; j++) {
            if (!lettersUsed[gameBoard[i][j] - 'a']) {
                printf("Not all letters used\n");
                freeBoardMemory();
                freeDictionary(dictionary);
                return 0;
            }
        }
    }

    printf("Correct\n");
    freeBoardMemory();
    freeDictionary(dictionary);
    return 0;
}

/*
 * Acknowledgment of External Resources:
 * - Used ChatGPT to help me understand dynamic memory allocation.
 * - I referred to TutorialsPoint (https://www.tutorialspoint.com/cprogramming/index.htm) 
 *   for learning C functions.
 */
