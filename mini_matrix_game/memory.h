/// Implementation inspired by Mircea Mihail: https://github.com/mircea-mihail
#define lcdBrightnessAddress 900
#define matrixBrightnessAddress lcdBrightnessAddress + sizeof(byte)
#define matrixBrightnessAutoAddress matrixBrightnessAddress + sizeof(byte)

#define soundSettingsAddress matrixBrightnessAutoAddress + sizeof(int)

#define highScoresAddress soundSettingsAddress + sizeof(int)
#define numberOfHighScores 3