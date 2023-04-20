#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <random>

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
    Win
};

sf::RenderWindow window;
std::vector<std::vector<Cell>> minefield;

int width, height;
int cellSize;


int gameState = GameState::Playing;

Cell invalidCell = Cell();

Cell &getCell(int x, int y);
int ammountOfBombsAround(int x, int y);
void revealCell(int x, int y);
void revealAll();
void drawCell(int x, int y);
void drawBomb(int x, int y);
int main();

Cell &getCell(int x, int y)
{
    if (x < 0)
        return invalidCell;
    if (x >= width)
        return invalidCell;
    if (y < 0)
        return invalidCell;
    if (y >= height)
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
        //revealAll();
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

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            if (getCell(x, y).isBomb)
                continue;

            if (!getCell(x, y).isOpen)
                return;
        }
    }

    gameState = GameState::Win;
}

void revealAll()
{
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            getCell(x, y).isBomb = true;
            getCell(x, y).isFlag = false;
        }
    }
}

void drawCell(int x, int y)
{
    sf::RectangleShape shape(sf::Vector2f(cellSize, cellSize));
//    sf::Texture tileHiddenTexture;
//    tileHiddenTexture.loadFromFile("/Users/mt/Desktop/HelloSFML/files/images/tile_hidden.png");
//    shape.setTexture(&tileHiddenTexture);
//    window.draw(shape);

    sf::Texture tileRevealTexture;
    tileRevealTexture.loadFromFile("/Users/mt/Desktop/HelloSFML/files/images/tile_revealed.png");

    sf::Texture num1Texture;
    num1Texture.loadFromFile("/Users/mt/Desktop/HelloSFML/files/images/number_1.png");

    sf::Texture num2Texture;
    num2Texture.loadFromFile("/Users/mt/Desktop/HelloSFML/files/images/number_2.png");

    sf::Texture num3Texture;
    num3Texture.loadFromFile("/Users/mt/Desktop/HelloSFML/files/images/number_3.png");

    sf::Texture num4Texture;
    num4Texture.loadFromFile("/Users/mt/Desktop/HelloSFML/files/images/number_4.png");

    sf::Texture num5Texture;
    num5Texture.loadFromFile("/Users/mt/Desktop/HelloSFML/files/images/number_5.png");

    sf::Texture num6Texture;
    num6Texture.loadFromFile("/Users/mt/Desktop/HelloSFML/files/images/number_6.png");

    sf::Texture num7Texture;
    num7Texture.loadFromFile("/Users/mt/Desktop/HelloSFML/files/images/number_7.png");

    sf::Texture num8Texture;
    num8Texture.loadFromFile("/Users/mt/Desktop/HelloSFML/files/images/number_8.png");

    sf:: Texture flagTexture;
    flagTexture.loadFromFile("/Users/mt/Desktop/HelloSFML/files/images/flag.png");

    sf::Texture mineTexture;
    mineTexture.loadFromFile("/Users/mt/Desktop/HelloSFML/files/images/mine.png");

    if (getCell(x, y).isFlag)
    {
        shape.setPosition(sf::Vector2f(x*cellSize, y*cellSize));
        //shape.setFillColor(sf::Color(255, 255, 255, 255));
        shape.setTexture(&flagTexture);
        window.draw(shape);
    }


    if (getCell(x, y).isOpen)
    {
            switch (getCell(x, y).bombsAround) {
                case 0:
                    shape.setPosition(sf::Vector2f(x * cellSize, y * cellSize));
                    shape.setTexture(&tileRevealTexture);
                    window.draw(shape);
                    break;
                case 1:
                    shape.setPosition(sf::Vector2f(x * cellSize, y * cellSize));
                    shape.setTexture(&num1Texture);
                    window.draw(shape);
                    break;
                case 2:
                    shape.setPosition(sf::Vector2f(x * cellSize, y * cellSize));
                    shape.setTexture(&num2Texture);
                    window.draw(shape);
                    break;
                case 3:
                    shape.setPosition(sf::Vector2f(x * cellSize, y * cellSize));
                    shape.setTexture(&num3Texture);
                    window.draw(shape);
                    break;
                case 4:
                    shape.setPosition(sf::Vector2f(x * cellSize, y * cellSize));
                    shape.setTexture(&num4Texture);
                    window.draw(shape);
                    break;
                case 5:
                    shape.setPosition(sf::Vector2f(x * cellSize, y * cellSize));
                    shape.setTexture(&num5Texture);
                    window.draw(shape);
                    break;
                case 6:
                    shape.setPosition(sf::Vector2f(x * cellSize, y * cellSize));
                    shape.setTexture(&num6Texture);
                    window.draw(shape);
                    break;
                case 7:
                    shape.setPosition(sf::Vector2f(x * cellSize, y * cellSize));
                    shape.setTexture(&num7Texture);
                    window.draw(shape);
                    break;
                case 8:
                    shape.setPosition(sf::Vector2f(x * cellSize, y * cellSize));
                    shape.setTexture(&num8Texture);
                    window.draw(shape);
                    break;
            }



        //shape.setFillColor(sf::Color(255, 255, 255, 255));

        if (getCell(x, y).isBomb)
            return;
        if (getCell(x, y).isFlag)
            return;
        if (getCell(x, y).bombsAround == 0)
            return;

    }
}

void drawBomb(int x, int y) {
    sf::RectangleShape bomb(sf::Vector2f(cellSize, cellSize));
    sf::Texture mineTexture;
    mineTexture.loadFromFile("/Users/mt/Desktop/HelloSFML/files/images/mine.png");

    if (getCell(x, y).isBomb) {
        bomb.setTexture(&mineTexture);
        bomb.setPosition(x * cellSize, y * cellSize);
        window.draw(bomb);
    }
}

        //shape.setPosition(sf::Vector2f((float) (x * cellSize), (float) (y * cellSize)));

int main()
{
    window.create(sf::VideoMode(800, 600), "Minesweeper");
//
//    font.loadFromFile("Vera.ttf");
//    bombTexture.loadFromFile("bomb.png");
//    flagTexture.loadFromFile("flag.png");

    width = 25;
    height = 16;
    cellSize = 32;
    int bombChance = 10;

    std::random_device device;
    std::mt19937 gen(device());

    minefield = std::vector<std::vector<Cell>>(width, std::vector<Cell>(height));

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            int randomNum = gen() % 100;
            if (randomNum > 100 - bombChance)
            {
                minefield[x][y].isBomb = true;
            }
        }
    }

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            minefield[x][y].bombsAround = ammountOfBombsAround(x, y);
        }
    }

    while (window.isOpen())
    {
        sf::Event e;
        while(window.pollEvent(e))
        {
            sf::Vector2f mousePos;

            switch (e.type)
            {
                case sf::Event::Closed:
                    window.close();
                    break;

                case sf::Event::MouseButtonPressed:
                    if (e.mouseButton.button == sf::Mouse::Left)
                    {
                        mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

                        mousePos.x /= cellSize;
                        mousePos.y /= cellSize;

                        revealCell((int)mousePos.x, (int)mousePos.y);
                    }
                    if (e.mouseButton.button == sf::Mouse::Right)
                    {
                        mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

                        mousePos.x /= cellSize;
                        mousePos.y /= cellSize;

                        if (getCell((int)mousePos.x, (int)mousePos.y).isOpen)
                            continue;

                        getCell((int)mousePos.x, (int)mousePos.y).isFlag = !getCell((int)mousePos.x, (int)mousePos.y).isFlag;
                    }
                    break;

                default:
                    break;
            }

        }

        window.clear(sf::Color(200, 200, 200, 255));

        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                sf::RectangleShape s(sf::Vector2f((float)cellSize,(float)cellSize));
                sf::Texture tileHiddenTexture;
                tileHiddenTexture.loadFromFile("/Users/mt/Desktop/HelloSFML/files/images/tile_hidden.png");
                s.setTexture(&tileHiddenTexture);
                s.setPosition(j * cellSize, i * cellSize);
                window.draw(s);
            }
        }

        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width; x++)
            {
                if (gameState == GameState::Playing)
                {
                    drawCell(x, y);
                }
                if (gameState == GameState::Loose) {
                    drawCell(x, y);
                    drawBomb(x, y);
                    //exit(0);
                    //if ()

                }
            }
        }

        window.display();

    }

    return 0;
}
