#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <random>
#include <chrono>
#include <iomanip>

using namespace std;

int numCols;
int numRows;
int numMines;

int numFlags;
int width;
int height;

string userName;
int userMin;
int userSec;

bool leaderboardOpen = false;


struct Player {
    string name;
    int minutes;
    int seconds;
};

bool operator<(const Player& lhs, const Player& rhs) {
    if (lhs.minutes == rhs.minutes) {
        return lhs.seconds < rhs.seconds;
    } else {
        return lhs.minutes < rhs.minutes;
    }
}

class Leaderboard {
    vector<Player> players;
    bool updated;

public:
    Leaderboard() {
        updated = false;
    }

    void loadPlayer() {
        ifstream infile("/Users/mt/Desktop/Minesweeper/files/leaderboard.txt");

        string line;

        while (getline(infile, line)) {
            istringstream iss(line);
            string name;
            string time;

            if (!(getline(iss, time, ',') && getline(iss, name))) {
                continue;
            }

            // Convert time string to minutes and seconds integers
            int pos = time.find(':');
            int min = stoi(time.substr(0, pos));
            int sec = stoi(time.substr(pos + 1));

            players.push_back({name, min, sec});
        }

    }

    void sortPlayers() {
        sort(players.begin(), players.end());
    }

    void addEntry(int minutes, int seconds, string &name) {
        players.push_back({name, minutes, seconds});
        sortPlayers();
        updated = true;

        ofstream outfile("/Users/mt/Desktop/Minesweeper/files/leaderboard.txt");
        for (int i = 0; i < players.size(); i++) {
            Player player = players[i];
            outfile << player.minutes << ":" << player.seconds << "," << player.name;

            if (i == 0 && updated) {
                outfile << "*";
            }
            outfile << endl;

        }
        updated = false;
    }

    const vector<Player>& getPlayers() const {
        return players;
    }
};

class Timer {
    int min;
    int seconds;
    bool isRunning;
    chrono::high_resolution_clock::time_point startTime;
    chrono::high_resolution_clock::duration elapsedTime;

public:
    Timer() {
        min = 0;
        seconds = 0;
        isRunning = true;
        startTime = chrono::high_resolution_clock::now();
        elapsedTime = chrono::high_resolution_clock::duration::zero();
    }

    void start() {
        if (!isRunning) {
            isRunning = true;
            startTime = chrono::high_resolution_clock::now() - elapsedTime;        }
    }

    void stop() {
        if (isRunning) {
            isRunning = false;
            elapsedTime = chrono::high_resolution_clock::now() - startTime;
        }
        userMin = min;
        userSec = seconds;
    }

    void restart() {
        min = 0;
        seconds = 0;
        isRunning = true;
        startTime = chrono::high_resolution_clock::now();
        elapsedTime = chrono::high_resolution_clock::duration::zero();
    }

    void update() {
        if (isRunning) {
            auto currentTime = chrono::high_resolution_clock::now();
            auto elapsed = chrono::duration_cast<chrono::seconds>(currentTime - startTime).count();
            seconds = elapsed % 60;
            min = elapsed / 60;
        }
    }


    void draw(sf::RenderWindow& gameWindow) {
        sf::Texture digitTexture;
        digitTexture.loadFromFile("/Users/mt/Desktop/Minesweeper/files/images/digits.png");

        sf::IntRect  digitRect(0, 0, 21, 32);

        int minute1Index = min / 10;
        sf::Sprite min1(digitTexture, sf::IntRect(digitRect.left + minute1Index * digitRect.width, digitRect.top, digitRect.width, digitRect.height));
        min1.setPosition((numCols * 32) - 97, 32 * (numRows + 0.5f) + 16);

        int minute2Index = min % 10;
        sf::Sprite min2(digitTexture, sf::IntRect(digitRect.left + minute2Index * digitRect.width, digitRect.top, digitRect.width, digitRect.height));
        min2.setPosition((numCols * 32) - 76, 32 * (numRows + 0.5f) + 16);

        int second1Index = seconds / 10;
        sf::Sprite second1(digitTexture, sf::IntRect(digitRect.left + second1Index * digitRect.width, digitRect.top, digitRect.width, digitRect.height));
        second1.setPosition((numCols * 32) - 54, 32 * (numRows + 0.5f) + 16);

        int second2Index = seconds % 10;
        sf::Sprite second2(digitTexture, sf::IntRect(digitRect.left + second2Index * digitRect.width, digitRect.top, digitRect.width, digitRect.height));
        second2.setPosition((numCols * 32) - 33, 32 * (numRows + 0.5f) + 16);

        gameWindow.draw(min1);
        gameWindow.draw(min2);
        gameWindow.draw(second1);
        gameWindow.draw(second2);
    }
};

struct Cell
{
    bool isBomb = false;
    bool isFlag = false;
    bool isOpen = false;
    int bombsAround = 0;
};

enum GameState
{
    Playing,
    Loose,
    Win,
};


vector<std::vector<Cell>> minefield;


int gameState = GameState::Playing;

Cell invalidCell = Cell();

Cell &getCell(int x, int y);
int ammountOfBombsAround(int x, int y);
void revealCell(int x, int y);
void revealAll();
void drawCell(sf::RenderWindow& gameWindow, int x, int y);
void drawBomb(sf::RenderWindow& gameWindow, int x, int y);
void randomBomb();
void readFile();


Cell &getCell(int x, int y)
{
    if (x < 0)
        return invalidCell;
    if (x >= numCols)
        return invalidCell;
    if (y < 0)
        return invalidCell;
    if (y >= numRows)
        return invalidCell;

    return minefield[x][y];
}

int ammountOfBombsAround(int x, int y)
{
    int ammount = 0;

    if (getCell(x + 1, y).isBomb)
        ammount++;
    if (getCell(x - 1, y).isBomb)
        ammount++;
    if (getCell(x, y + 1).isBomb)
        ammount++;
    if (getCell(x, y - 1).isBomb)
        ammount++;
    if (getCell(x + 1, y + 1).isBomb)
        ammount++;
    if (getCell(x + 1, y - 1).isBomb)
        ammount++;
    if (getCell(x - 1, y + 1).isBomb)
        ammount++;
    if (getCell(x - 1, y - 1).isBomb)
        ammount++;

    return ammount;
}

void revealCell(int x, int y)
{
    Cell &cell = getCell(x, y);

    if (cell.isFlag)
        return;
    if (cell.isOpen)
        return;

    if (cell.isBomb)
    {
        gameState = GameState::Loose;
        revealAll();
        return;
    }

    if (cell.bombsAround == 0)
    {
        cell.isOpen = true;
        revealCell(x + 1, y);
        revealCell(x - 1, y);
        revealCell(x, y + 1);
        revealCell(x, y - 1);
        revealCell(x + 1, y + 1);
        revealCell(x + 1, y - 1);
        revealCell(x - 1, y - 1);
        revealCell(x - 1, y + 1);
    }

    cell.isOpen = true;

    int numClosedTiles = 0;

    for (int y = 0; y < numRows; y++)
    {
        for (int x = 0; x < numCols; x++)
        {
            Cell &cell = getCell(x, y);
            if (cell.isBomb && !cell.isFlag)
            {
                //cell.isFlag = true;
                numClosedTiles++;
            }

            if (!cell.isBomb && !cell.isOpen)
            {
                numClosedTiles++;
                return;
            }
        }
    }

    gameState = GameState::Win;

    if (numClosedTiles > 0)
    {
        for (int y = 0; y < numRows; y++)
        {
            for (int x = 0; x < numCols; x++)
            {
                Cell &cell = getCell(x, y);

                if (cell.isBomb && !cell.isFlag)
                {
                    cell.isFlag = true;
                    numFlags = 0;
                }
            }
        }
    }
}

void revealAll()
{
    for (int y = 0; y < numRows; y++)
    {
        for (int x = 0; x < numCols; x++)
        {
            getCell(x, y).isOpen = true;
            getCell(x, y).isFlag = false;
        }
    }
}

void drawCell(sf::RenderWindow& gameWindow, int x, int y)
{

    sf::RectangleShape shape(sf::Vector2f(32, 32));

    sf::Texture tileRevealTexture;
    tileRevealTexture.loadFromFile("/Users/mt/Desktop/Minesweeper/files/images/tile_revealed.png");

    sf::Texture num1Texture;
    num1Texture.loadFromFile("/Users/mt/Desktop/Minesweeper/files/images/number_1.png");

    sf::Texture num2Texture;
    num2Texture.loadFromFile("/Users/mt/Desktop/Minesweeper/files/images/number_2.png");

    sf::Texture num3Texture;
    num3Texture.loadFromFile("/Users/mt/Desktop/Minesweeper/files/images/number_3.png");

    sf::Texture num4Texture;
    num4Texture.loadFromFile("/Users/mt/Desktop/Minesweeper/files/images/number_4.png");

    sf::Texture num5Texture;
    num5Texture.loadFromFile("/Users/mt/Desktop/Minesweeper/files/images/number_5.png");

    sf::Texture num6Texture;
    num6Texture.loadFromFile("/Users/mt/Desktop/Minesweeper/files/images/number_6.png");

    sf::Texture num7Texture;
    num7Texture.loadFromFile("/Users/mt/Desktop/Minesweeper/files/images/number_7.png");

    sf::Texture num8Texture;
    num8Texture.loadFromFile("/Users/mt/Desktop/Minesweeper/files/images/number_8.png");

    sf:: Texture flagTexture;
    flagTexture.loadFromFile("/Users/mt/Desktop/Minesweeper/files/images/flag.png");

    sf::Texture mineTexture;
    mineTexture.loadFromFile("/Users/mt/Desktop/Minesweeper/files/images/mine.png");

    if (getCell(x, y).isFlag)
    {
        shape.setPosition(sf::Vector2f(x *32, y*32));
        shape.setTexture(&flagTexture);
        gameWindow.draw(shape);
    }


    if (getCell(x, y).isOpen)
    {
        if (getCell(x, y).isBomb) {
            shape.setPosition(sf::Vector2f(x * 32, y * 32));
            shape.setTexture(&tileRevealTexture);
            gameWindow.draw(shape);
            sf::Texture bombTexture;
            bombTexture.loadFromFile("/Users/mt/Desktop/Minesweeper/files/images/mine.png");
            sf::Sprite bomb(bombTexture);
            bomb.setPosition(sf::Vector2f(x * 32, y * 32));
            gameWindow.draw(bomb);
        } else {
            switch (getCell(x, y).bombsAround) {
                case 0:
                    shape.setPosition(sf::Vector2f(x * 32, y * 32));
                    shape.setTexture(&tileRevealTexture);
                    gameWindow.draw(shape);
                    break;
                case 1:
                    shape.setPosition(sf::Vector2f(x * 32, y * 32));
                    shape.setTexture(&num1Texture);
                    gameWindow.draw(shape);
                    break;
                case 2:
                    shape.setPosition(sf::Vector2f(x * 32, y * 32));
                    shape.setTexture(&num2Texture);
                    gameWindow.draw(shape);
                    break;
                case 3:
                    shape.setPosition(sf::Vector2f(x * 32, y * 32));
                    shape.setTexture(&num3Texture);
                    gameWindow.draw(shape);
                    break;
                case 4:
                    shape.setPosition(sf::Vector2f(x * 32, y * 32));
                    shape.setTexture(&num4Texture);
                    gameWindow.draw(shape);
                    break;
                case 5:
                    shape.setPosition(sf::Vector2f(x * 32, y * 32));
                    shape.setTexture(&num5Texture);
                    gameWindow.draw(shape);
                    break;
                case 6:
                    shape.setPosition(sf::Vector2f(x * 32, y * 32));
                    shape.setTexture(&num6Texture);
                    gameWindow.draw(shape);
                    break;
                case 7:
                    shape.setPosition(sf::Vector2f(x * 32, y * 32));
                    shape.setTexture(&num7Texture);
                    gameWindow.draw(shape);
                    break;
                case 8:
                    shape.setPosition(sf::Vector2f(x * 32, y * 32));
                    shape.setTexture(&num8Texture);
                    gameWindow.draw(shape);
                    break;
            }
        }

        if (getCell(x, y).isBomb)
            return;
        if (getCell(x, y).isFlag)
            return;
        if (getCell(x, y).bombsAround == 0)
            return;
    }
}

void drawBomb(sf::RenderWindow& gameWindow, int x, int y) {

    sf::Texture bombTexture;
    bombTexture.loadFromFile("/Users/mt/Desktop/Minesweeper/files/images/mine.png");
    sf::Sprite bomb(bombTexture);

    if (getCell(x, y).isBomb) {
        bomb.setPosition(x * 32, y * 32);

        gameWindow.draw(bomb);
    }
}

void randomBomb() {
    std::random_device device;
    std::mt19937 gen(device());
    int bombsPlaced = 0;

    minefield = std::vector<std::vector<Cell>>(numCols, std::vector<Cell>(numRows));

    while (bombsPlaced < numMines) {
        int x = gen() % numCols;
        int y = gen() % numRows;
        if (!minefield[x][y].isBomb) {
            minefield[x][y].isBomb = true;
            bombsPlaced++;
        }
    }

    for (int y = 0; y < numRows; y++) {
        for (int x = 0; x < numCols; x++) {
            minefield[x][y].bombsAround = ammountOfBombsAround(x, y);
        }
    }
}

void readFile() {
    string line;
    ifstream configFile("/Users/mt/Desktop/Minesweeper/files/board_config.cfg");
    if (configFile.is_open()) {
        getline(configFile, line);
        istringstream(line) >> numCols;
        getline(configFile, line);
        istringstream(line) >> numRows;
        getline(configFile, line);
        istringstream(line) >> numMines;

        configFile.close();
    } else {
        throw runtime_error("Failed to open file");
    }

    // Information of the game
    width = numCols * 32;
    height = (numRows * 32) + 100;
    numFlags = numMines;
}

//void resetTilesToPreviousState() {
//    for (int i = 0; i < numRows; i++) {
//        for (int j = 0; j < numCols; j++) {
//            if (minefield[j][i].isOpen != getCell(j, i).isOpen) {
//                // restore the previous open/closed state of the cell
//                getCell(j, i).isOpen = minefield[j][i].isOpen;
//            }
//
//            if (minefield[j][i].isFlag != getCell(j, i).isFlag) {
//                // restore the previous flag/no-flag state of the cell
//                getCell(j, i).isFlag = minefield[j][i].isFlag;
//            }
//
//            if (minefield[j][i].isBomb != getCell(j, i).isBomb) {
//                // restore the previous bomb/no-bomb state of the cell
//                getCell(j, i).isBomb = minefield[j][i].isBomb;
//
//                // if the cell is now a bomb, but was not before, reveal it
//                if (getCell(j, i).isBomb) {
//                    getCell(j, i).isOpen = true;
//                }
//            }
//
//            drawCell(j, i);
//            }
//        }
//    }

void generate_welcome_window () {

    sf::Font font;
    font.loadFromFile("/Users/mt/Desktop/Minesweeper/files/font.ttf");

    // The Welcome Text
    sf::Text welcomeText("WELCOME TO MINESWEEPER!", font, 24);
    welcomeText.setStyle(sf::Text::Bold);
    welcomeText.setStyle(sf::Text::Underlined);
    welcomeText.setFillColor(sf::Color::White);
    sf::FloatRect textRect = welcomeText.getLocalBounds();
    float welcomeTextWidth = width / 2 - textRect.width / 2;
    float welcomeTextHeight = height / 2 - textRect.height / 2 - 150;
    welcomeText.setPosition(welcomeTextWidth, welcomeTextHeight);

    // The Input Prompt Text
    sf::Text inputPromptText("Enter your name:", font, 20);
    inputPromptText.setStyle(sf::Text::Bold);
    inputPromptText.setFillColor(sf::Color::White);
    sf::FloatRect inputRect = inputPromptText.getGlobalBounds();
    float inputRectWidth = width /2 - inputRect.width /2;
    float inputRectHeight = height / 2 - inputRect.height /2 - 75;
    inputPromptText.setPosition(inputRectWidth, inputRectHeight);

    // The Input Text
    sf::Text inputText("", font, 18);
    inputText.setStyle(sf::Text::Bold);
    inputText.setFillColor(sf::Color::Yellow);
    inputText.setPosition(width/2.0f, height/2.0f - 45);

    // The Cursor
    sf::Text cursor("|", font, 18);
    cursor.setFillColor(sf::Color::Yellow);

    sf::RenderWindow window;
    window.create(sf::VideoMode(width, height), "Minesweeper");
    while (window.isOpen()) {

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            if (event.type == sf::Event::TextEntered) {
                // Read the alphabets only
                if (event.text.unicode >= 'a' && event.text.unicode <= 'z' ||
                    event.text.unicode >= 'A' && event.text.unicode <= 'Z') {
                    // The user cannot enter more than 10 characters
                    if (inputText.getString().getSize() < 10) {
                        inputText.setString(inputText.getString() + static_cast<char>(event.text.unicode));
                    }
                }
            }
                // Pressing backspace to remove the last character
            else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Backspace) {
                string tempStr = inputText.getString();
                if (tempStr.size() > 0) {
                    tempStr.erase(tempStr.size() - 1, 1);
                    inputText.setString(tempStr);
                }
            }
                // Pressing the enter key, close the welcome window ad open the game window
            else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter) {
                string userStr = inputText.getString();
                if (userStr.size() > 0) {
                    for (int i = 0; i < userStr.size(); i++) {
                        userStr[0] = toupper(userStr[0]);
                        userStr[i] = tolower(userStr[i]);
                    }
                    userName = userStr;
                } else {
                    break;
                }
                window.close();

            }
        }
        window.clear(sf::Color::Blue);
        window.draw(welcomeText);
        window.draw(inputPromptText);
        window.draw(inputText);
        if (inputText.getString().getSize() < 11) {
            cursor.setPosition(inputText.getPosition().x + inputText.getGlobalBounds().width,
                               inputText.getPosition().y);
            window.draw(cursor);
        }
        window.display();
    }
}

void generate_leaderboardwindow() {

    Leaderboard leaderboard;



    if (gameState == GameState::Win) {
        leaderboard.addEntry(userMin, userSec, userName);
        leaderboardOpen = true;
    }
    leaderboard.loadPlayer();
    vector<Player> players = leaderboard.getPlayers();

    sf::Font font;
    font.loadFromFile("/Users/mt/Desktop/Minesweeper/files/font.ttf");

    sf::Text title("LEADERBOARD", font, 20);
    title.setStyle(sf::Text::Bold);
    title.setStyle(sf::Text::Underlined);
    title.setFillColor(sf::Color::White);
    sf::FloatRect textRect = title.getLocalBounds();
    float titleWidth = ((16 * numCols) / 3.0f) / 2 - textRect.width / 2;
    float titleHeight = ((numRows * 16) + 50) / 2 - textRect.height / 2 - 120;
    title.setPosition(titleWidth, titleHeight);
    title.setPosition(((16 * numCols) / 3.0f), ((numRows * 16) + 50) / 2 - 120);
    //leaderboardWindow.draw(title);

    sf::Text nameText("", font, 18);
    nameText.setStyle(sf::Text::Bold);
    nameText.setFillColor(sf::Color::White);

    sf::RenderWindow leaderboardWindow;
    leaderboardWindow.create(sf::VideoMode(numCols * 16, ((numRows * 16) + 50)), "Minesweeper");

    while (leaderboardWindow.isOpen()) {

        sf::Event event;
        while (leaderboardWindow.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                leaderboardWindow.close();
            }
            if (leaderboardOpen) {
                if (event.type == sf::Event::Closed) {
                    leaderboardWindow.close();
                    leaderboardOpen = false;
                }
            }
        }

        leaderboardWindow.clear(sf::Color::Blue);

        leaderboardWindow.draw(title);
        for (int i = 0; i < 5; i++) {
            std::ostringstream oss;
            oss << std::setw(2) << std::setfill('0') << players[i].minutes << ":"
                << std::setw(2) << std::setfill('0') << players[i].seconds;
            string time = oss.str();

            sf::Text rankText(to_string(i + 1) + ".", font, 18);
            rankText.setStyle(sf::Text::Bold);
            rankText.setFillColor(sf::Color::White);
            rankText.setPosition(((16 * numCols) / 4.5f) , ((numRows * 16) + 50) / 2 - 70 + (i*35));
            leaderboardWindow.draw(rankText);

            sf::Text timeText(time, font, 18);
            timeText.setStyle(sf::Text::Bold);
            timeText.setFillColor(sf::Color::White);
            timeText.setPosition(((16 * numCols) / 2.5f), ((numRows * 16) + 50) / 2 - 70 + (i*35));
            leaderboardWindow.draw(timeText);


            sf::Text nameText(players[i].name, font, 18);
            nameText.setStyle(sf::Text::Bold);
            nameText.setFillColor(sf::Color::White);
            nameText.setPosition(((16 * numCols) / 1.55f), ((numRows * 16) + 50) / 2 - 70 + (i*35));
            leaderboardWindow.draw(nameText);
        }
        leaderboardWindow.display();
    }
}

//void drawGrid(sf::RenderWindow& gameWindow) {
//    for (int i = 0; i < numRows; i++) {
//        for (int j = 0; j < numCols; j++) {
//            sf::RectangleShape s(sf::Vector2f((float)cellSize,(float)cellSize));
//            sf::Texture tileHiddenTexture;
//            tileHiddenTexture.loadFromFile("/fileesfiles/images/tile_hidden.png");
//            s.setTexture(&tileHiddenTexture);
//            s.setPosition(j * cellSize, i * cellSize);
//            gameWindow.draw(s);
//        }
//    }
//}

int main()
{
    bool debugMode = false;
    bool paused = false;
    bool leadOpen = false;

    readFile();
    generate_welcome_window();


    randomBomb();

    sf::Texture happyTexture;
    happyTexture.loadFromFile("/Users/mt/Desktop/Minesweeper/files/images/face_happy.png");
    sf::Sprite happy(happyTexture);
    happy.setPosition(sf::Vector2f(((numCols / 2.0) * 32) - 32, 32 * (numRows + 0.5f)));

    sf::Texture faceWinTexture;
    faceWinTexture.loadFromFile("/Users/mt/Desktop/Minesweeper/files/images/face_win.png");
    sf::Sprite faceWin(faceWinTexture);
    faceWin.setPosition(sf::Vector2f(((numCols / 2.0) * 32) - 32, 32 * (numRows + 0.5f)));

    sf::Texture faceLoseTexture;
    faceLoseTexture.loadFromFile("/Users/mt/Desktop/Minesweeper/files/images/face_lose.png");
    sf::Sprite faceLose(faceLoseTexture);
    faceLose.setPosition(((numCols / 2.0) * 32) - 32, 32 * (numRows + 0.5f));

    sf::Texture debugTexture;
    debugTexture.loadFromFile("/Users/mt/Desktop/Minesweeper/files/images/debug.png");
    sf::Sprite debugButton(debugTexture);
    debugButton.setPosition((numCols * 32) - 304, 32 * (numRows + 0.5f));

    sf::Texture pauseTexture;
    pauseTexture.loadFromFile("/Users/mt/Desktop/Minesweeper/files/images/pause.png");
    sf::Sprite pauseButton(pauseTexture);
    pauseButton.setPosition((numCols * 32) - 240, 32 * (numRows + 0.5f));

    sf::Texture playTexture;
    playTexture.loadFromFile("/Users/mt/Desktop/Minesweeper/files/images/play.png");
    sf::Sprite playButton(playTexture);
    playButton.setPosition((numCols * 32) - 240, 32 * (numRows + 0.5f));

    sf::Texture leaderboardTexture;
    leaderboardTexture.loadFromFile("/Users/mt/Desktop/Minesweeper/files/images/leaderboard.png");
    sf::Sprite leaderboardButton(leaderboardTexture);
    leaderboardButton.setPosition((numCols * 32) - 176, 32 * (numRows + 0.5f));

    sf::Texture digitTexture;
    digitTexture.loadFromFile("/Users/mt/Desktop/Minesweeper/files/images/digits.png");

    Timer timer;

    sf::RenderWindow gameWindow(sf::VideoMode(width, height), "Minesweeper");

    while (gameWindow.isOpen()) {

        if (gameState == GameState::Loose) {
            timer.stop();
            gameWindow.draw(faceLose);
            debugMode = false;
            paused = false;

        }
        if (gameState == GameState::Win && !leaderboardOpen)  {
            timer.stop();
            gameWindow.draw(faceWin);
            generate_leaderboardwindow();
            debugMode = false;
            paused = false;
            leaderboardOpen = true;
        }

        sf::Event e;
        while(gameWindow.pollEvent(e)) {
            sf::Vector2f mousePos;

            switch (e.type) {
                case sf::Event::Closed:
                    gameWindow.close();
                    break;

                case sf::Event::MouseButtonPressed:
                    if (e.mouseButton.button == sf::Mouse::Left) {
                        mousePos = gameWindow.mapPixelToCoords(sf::Mouse::getPosition(gameWindow));

                        if (happy.getGlobalBounds().contains(mousePos)) {
                            numFlags = numMines;
                            randomBomb();
                            timer.restart();
                            gameState = GameState::Playing;

                        } else if (debugButton.getGlobalBounds().contains(mousePos)) {
                            debugMode = !debugMode;

                        } else if (pauseButton.getGlobalBounds().contains(mousePos)) {
                            debugMode = false;
                            paused = !paused;

                        } else if (leaderboardButton.getGlobalBounds().contains(mousePos)) {

                            leadOpen = !leadOpen;
                            timer.stop();
                            generate_leaderboardwindow();

                            timer.start();

                        } else {

                            mousePos.x /= 32;
                            mousePos.y /= 32;


                            if (mousePos.x >= 0 && mousePos.x < numCols && mousePos.y >= 0 && mousePos.y < numRows) {

                                revealCell((int) mousePos.x, (int) mousePos.y);
                            }

                        }
                    }
                    if (e.mouseButton.button == sf::Mouse::Right) {
                        mousePos = gameWindow.mapPixelToCoords(sf::Mouse::getPosition(gameWindow));

                        mousePos.x /= 32;
                        mousePos.y /= 32;

                        if (getCell((int)mousePos.x, (int)mousePos.y).isOpen)
                            continue;

                        getCell((int)mousePos.x, (int)mousePos.y).isFlag = !getCell((int)mousePos.x, (int)mousePos.y).isFlag;

                        if (getCell((int)mousePos.x, (int)mousePos.y).isFlag) {
                            numFlags--;

                        } else {
                            numFlags++;

                        }
                    }
                    break;

                default:
                    break;
            }

        }

        timer.update();

        gameWindow.clear(sf::Color::White);

        gameWindow.draw(debugButton);
        gameWindow.draw(pauseButton);
        gameWindow.draw(leaderboardButton);

        timer.draw(gameWindow);

        vector<sf::Sprite> digitSprites;
        for (int i = 0; i <= 11; i++) {
            sf::IntRect rect(i * 21, 0, 21, 31);
            sf::Sprite sprite(digitTexture, rect);
            digitSprites.push_back(sprite);
        }

        if (numFlags >= 0) {
            int digit1 = numFlags / 10;
            int digit2 = numFlags % 10;

            sf::Sprite &d0 = digitSprites[int(0)];
            d0.setPosition(33, 32 * (numRows + 0.5f) + 16);
            gameWindow.draw(d0);
            sf::Sprite &d1 = digitSprites[digit1];
            d1.setPosition(54, 32 * (numRows + 0.5f) + 16);
            gameWindow.draw(d1);
            sf::Sprite &d2 = digitSprites[digit2];
            d2.setPosition(75, 32 * (numRows + 0.5f) + 16);
            gameWindow.draw(d2);

        } else if (numFlags < 0) {
            sf::Sprite &negativeSign = digitSprites[int(10)];
            negativeSign.setPosition(12, 32 * (numRows + 0.5f) + 16);
            gameWindow.draw(negativeSign);
        }



        for (int i = 0; i < numRows; i++) {
            for (int j = 0; j < numCols; j++) {
                sf::RectangleShape s(sf::Vector2f((float)32,(float)32));
                sf::Texture tileHiddenTexture;
                tileHiddenTexture.loadFromFile("/Users/mt/Desktop/Minesweeper/files/images/tile_hidden.png");
                s.setTexture(&tileHiddenTexture);
                s.setPosition(j * 32, i * 32);
                gameWindow.draw(s);
            }
        }
        gameWindow.draw(happy);

        for (int y = 0; y < numRows; y++) {
            for (int x = 0; x < numCols; x++) {

                if (gameState == GameState::Playing) {
                    drawCell(gameWindow, x, y);
                }
                if (gameState == GameState::Loose) {
                    drawCell(gameWindow, x, y);
                    gameWindow.draw(faceLose);
                }
                if (gameState == GameState::Win) {
                    gameWindow.draw(faceWin);
                    drawCell(gameWindow, x, y);
                    timer.stop();
                }
                if (debugMode) {
                    drawBomb(gameWindow, x, y);
                }
            }
        }

        if (paused) {
            gameWindow.draw(playButton);
            for (int i = 0; i < numRows; i++) {
                for (int j = 0; j < numCols; j++) {
                    sf::RectangleShape s(sf::Vector2f((float)32,(float)32));
                    sf::Texture tileTexture;
                    tileTexture.loadFromFile("/Users/mt/Desktop/Minesweeper/files/images/tile_revealed.png");
                    s.setTexture(&tileTexture);
                    s.setPosition(j * 32, i * 32);
                    gameWindow.draw(s);

                }
            }

            timer.stop();
            debugMode = false;
        } else {
            //drawGrid(gameWindow);
            gameWindow.draw(pauseButton);
            timer.start();
        }

        gameWindow.display();


    }

    return 0;
}
