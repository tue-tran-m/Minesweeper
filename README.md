# The Minesweeper
Create a version of the classic game, Minesweeper. Using SMFL (the Simple and Fast Multimedia Library) to do the work of drawing images to the screen and getting mouse input.

# Live Demo
https://youtu.be/1pOwG15iOLU

# Description
In this project, three are different windows: the welcome window, the game window, and the leaderboard window. A window in SFML is a graphical window that displays graphics and allows user interaction in a computer program by using sf::RenderWindow.

  1. The welcome window:
  This will be the first thing the player should see when you run the game. It will prompt the user for their name. The user will type their name using the keyboard as input.
    
     📌 Features:
      - The user cannot enter more than 10 characters while typing their name.
      - The user has to see the characters as they type and pressing backspace should remove the last character.
      - The characters should only contain alphabets. So when a user types a number/special character, you shouldn’t read it.
      - When the user presses the “Enter” key, you will close this window, and open the game window.

        
  2. Game window:
  There exists a board that has a grid of tiles. A tile could contain a mine, or not. The player clicks on a tile, and it gets revealed. The goal of the game is to reveal all the tiles that are not mines while avoiding the tiles that are.

     📌 Features:
     - Happy Face Button: This button lets you restart a new board with everything reset
     - Counter: Helps to track the number of mines that are on the board.
     - Debug Button: Clicking this button will toggle the visibility of the mines on the board.
     - Pause/Play Button: Pauses or Plays the game.
     -  Leaderboard Button: Will pause the game, and all tiles should display and display the leaderboard window.
     -  Timer: Displays the amount of time that has passed since the beginning of the game.

  3. Leaderboard window:
  The Leaderboard window will appear whenever the user presses the "Leaderboard" button, or when the user wins the game.
