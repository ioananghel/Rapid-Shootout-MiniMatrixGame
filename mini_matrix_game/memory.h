/// Implementation inspired by Mircea Mihail: https://github.com/mircea-mihail
#define lcdBrightnessAddress 900
#define matrixBrightnessAddress lcdBrightnessAddress + sizeof(byte)
#define matrixBrightnessAutoAddress matrixBrightnessAddress + sizeof(byte)

#define soundSettingsAddress matrixBrightnessAutoAddress + sizeof(int)

#define highScoresAddress soundSettingsAddress + sizeof(int)
#define numberOfHighScores 3

// visual represantation of the memory:
/*               addr (in EEPROM)
---------------- 1024 (Max size)
...
---------------- 934
Top 3 Players with High Scores
---------------- 910 (highScoresAddress)
Sound Settings (ON/OFF)
---------------- 906 (soundSettingsAddress)
Auto Matrix Brightness (ON/OFF)
---------------- 902 (matrixBrightnessAutoAddress)
Matrix Brightness
---------------- 901 (matrixBrightnessAddress)
LCD Brightness (byte)
---------------- 900 (lcdBrightnessAddress)
*/
