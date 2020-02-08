#include "Utils.h"
#include <iostream>
#include <cstring>
#include <cctype>

using namespace std;

char GetCharacter(const char* prompt, const char* error)
{
    char input;
    bool inputFailure;

    do {

        inputFailure = false;
        cout << prompt;

        cin >> input;

        if (cin.fail())
        {
            cin.ignore(256, '\n');
            cin.clear();
            cout << error << endl;
            inputFailure = true;
        }
        else
        {
            cin.ignore(256, '\n');
            if (isalpha(input))
            {
                input = tolower(input);
            }
            else
            {
                cout << error << endl;
                inputFailure = true;
            }
        }

    } while (inputFailure);

    return input;
}


char GetCharacter(const char* prompt, const char* error, const char validInput[], int validInputLength, CharacterCaseType charCase)
{
    const int IGNORE_CHARS = 256;

    char input;
    bool inputFailure = false;

    do {

        cout << prompt;
        cin >> input;

        if (cin.fail())
        {
            cin.ignore(IGNORE_CHARS, '\n');
            cin.clear();
            cout << error << endl;
            inputFailure = true;
        }
        else
        {
            cin.ignore(IGNORE_CHARS, '\n');

            if (isalpha(input))
            {
                if (charCase == CC_UPPER_CASE)
                {
                    input = toupper(input);
                }
                else if (charCase == CC_LOWER_CASE)
                {
                    input = tolower(input);
                }
            }

            for (int i = 0; i < validInputLength; ++i)
            {
                if (input == validInput[i])
                {
                    return input;
                }
            }

            cout << error << endl;
            inputFailure = true;
        }

    } while (inputFailure);

    return ' ';
}

int GetInteger(const char* prompt, const char* error, const int validInput[], int validInputlength)
{

    const int IGNORE_CHARS = 256;

    int input;
    bool inputFailure;

    do
    {

        inputFailure = false;
        cout << prompt;
        cin >> input;

        if (cin.fail())
        {
            cin.clear();
            cin.ignore(IGNORE_CHARS, '\n');
            cout << error << endl;
            inputFailure = true;
        }
        else
        {
            for (int i = 0; i < validInputlength; i++)
            {
                if (input == validInput[i])
                {
                    return input;
                }
            }

            cout << error << endl;
            inputFailure = true;
        }

    } while (inputFailure);

    return -1;

}

void ClearScreen()
{
    system("cls"); //for windows only!
    //system("clear");
}

void WaitForKeyPress()
{
    system("pause"); //Windows only!

    //system("read -n 1 -s -p \"Press any key to continue...\";echo");
}
