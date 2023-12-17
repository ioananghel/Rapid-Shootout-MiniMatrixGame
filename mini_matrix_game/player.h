const char alphabet[] = "abcdefghijklmnopqrstuvwxyz";
const int alphabetLength = sizeof(alphabet) - 1, inputDebounceTime = 500;
int playerName[3] = {0, 0, 0};
int playerNameSelectIndex = 0, playerNameLength = 3;

struct Player {
    char name[3];
    int score;
};