#include <iostream>
#include "autocell.h"

class cChessSquare : public cell::cCell
{
public:
    int file, rank;
    cChessSquare()
    {}
    cChessSquare( int f, int r )
    : file( f )
    , rank(r)
    {}
    void text(char c)
    {
        myText = c;
    }
    char text() const
    {
        return myText;
    }

private:
    char myText;
};

typedef const cChessSquare & sq_t;

class cChess
{
public:
    cChess();

    /// @brief read FEN file containing chess board position
    /// @param fname
    ///
    /// https://en.wikipedia.org/wiki/Forsyth%E2%80%93Edwards_Notation

    void readFEN(const std::string &fname);

    /// @brief human readable  position layout
    /// @return 
    std::string text();

    /// @brief human readable rank position
    /// @param r rank
    /// @return human readable string

    std::string rank(int r);

    /// @brief describe a square in the position
    /// @param rank
    /// @param file
    /// @return human readable description

    std::string describe(sq_t sq );

    /// @brief legal moves available from a square
    /// @param moves square 
    /// @return vector of pointers to square where piece can move to

    std::vector<cChessSquare *> moves(sq_t start);

    /// @brief symbol of piece occupying the square
    /// @param sq square
    /// @return symbol
    
    char piece( sq_t sq );
    std::string pieceVerbose(sq_t sq);

    /// @brief algeabraic name of square e.g. e4
    /// @param q pointer to square
    /// @return name

    std::string algebraic(cChessSquare *q);

    bool isWhiteMove() const
    { return myWhiteMove; }

private:
    cell::cAutomaton<cChessSquare> myBoard;
    bool myWhiteMove;

    const char algrank[8]{'8', '7', '6', '5', '4', '3', '2', '1'};
    const char algfile[8]{'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'};

    /// @brief the standard piece value P is 1, ... Q is 9
    /// @param sq square containing piece   
    /// @return value of piece

    int pieceValue( sq_t sq );

    /// @brief the standard piece value P is 1, ... Q is 9
    /// @param p symbol of piece
    /// @return value of piece

    int pieceValue(char p) const;

    double strength(sq_t);

    /// @brief true if square can be moved to
    bool isEmpty(sq_t sq);
    bool isWhite(sq_t sq);
    bool isCapture( sq_t src, sq_t trg );

    std::vector<cChessSquare *> rookMoves(sq_t start);
    std::vector<cChessSquare *> knightMoves(sq_t start);
    std::vector<cChessSquare *> bishopMoves(sq_t start);
    std::vector<cChessSquare *> queenMoves(sq_t start);
    std::vector<cChessSquare *> pawnMoves(sq_t start);
    std::vector<cChessSquare *> kingMoves(sq_t start);

    /// @brief long moves ( rook, bishop, queen )
    /// @param ret  vector of pointers to reachable squares, added to
    /// @param start starting square
    /// @param fileInc file increment
    /// @param rankInc rank increment

    void longMoves(
        std::vector<cChessSquare *>& ret,
        sq_t start,
        int fileInc,
        int rankInc    );
};
