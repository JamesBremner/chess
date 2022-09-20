# Chess

This is intended to be a chess position entropy calculator.

So far, I have implemented code to parse Forsyth–Edwards Notation ( https://en.wikipedia.org/wiki/Forsyth%E2%80%93Edwards_Notation ) and display the position.

e4.fen
    
    rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1
    
![image](https://user-images.githubusercontent.com/2046227/190929390-1ddabc13-b338-49db-a98b-713bf094331c.png)

On the right, the occupied squares are shown, with the piece on the square and a measure of strength equal to the product of the value of the piece and the number of moves available to it.

The user can move a piece on the boar. Click on the piece and, holding down the left mouse button, drag it to a new square and release the button.  If the move is legal, the board will be updated with the new position,


