#include <iomanip>
#include <fstream>
#include "cChess.h"

int cell::cCell::myLastID = 0;

cChess::cChess()
    : myBoard(cell::cAutomaton<cChessSquare>(8, 8))
{
}

void cChess::readFEN(const std::string &fname)
{
    std::ifstream ifs(fname);
    if (!ifs.is_open())
        throw std::runtime_error(
            "Cannot open FEN file");
    std::string line;
    getline(ifs, line);
    std::istringstream iss(line);
    for (int rank = 0; rank < 8; rank++)
    {
        getline(iss, line, '/');
        int file = 0;
        for (char c : line)
        {
            if (c == ' ')
                break;
            int count;
            switch (c)
            {
            case 'p':
            case 'P':
            case 'r':
            case 'R':
            case 'n':
            case 'N':
            case 'b':
            case 'B':
            case 'q':
            case 'Q':
            case 'k':
            case 'K':
                myBoard.cell(file, rank)->text(c);
                file++;
                break;
            default:
                count = c - '0';
                for (int k = 0; k < count; k++)
                {
                    myBoard.cell(file, rank)->text(' ');
                    file++;
                }
                break;
            }
        }

        // std::cout << line << "\n";

        if (rank == 7)
        {
            std::cout << "extra " << line.substr(line.find(' ')) << "\n";
            myWhiteMove = false;
            if (line[line.find(' ') + 1] == 'w')
                myWhiteMove = true;
        }
    }
}

std::string cChess::text()
{
    std::string ret;
    for (int r = 0; r < 8; r++)
        ret += std::to_string(r) + " " + rank(r) + "\n";
    return ret;
}
std::string cChess::rank(int r)
{
    std::string ret;
    ret.push_back(algrank[r]);
    ret.push_back(' ');
    for (int file = 0; file < 8; file++)
    {
        ret.push_back(myBoard.cell(file, r)->text());
        ret.push_back(' ');
    }
    ret.push_back(' ');
    ret.push_back(algrank[r]);
    return ret;
}

std::string cChess::describe(sq_t sq)
{
    std::stringstream ret;
    char Piece = piece(sq);
    if (Piece == ' ' || Piece == 'x')
        return ret.str();
    ret << algfile[sq.file];
    ret << algrank[sq.rank];
    ret << " " << pieceVerbose(sq);
    ret << " strength ";
    ret << std::setprecision(4) << strength(sq);
    return ret.str();
}

std::string cChess::algebraic(cChessSquare *q)
{
    int w, h;
    std::string s;
    myBoard.coords(w, h, q);
    s.push_back(algfile[w]);
    s.push_back(algrank[h]);
    return s;
}

bool cChess::move(sq_t Square, sq_t SquareMove)
{
    // check that move is legal
    bool fOK = false;
    for (auto legal : moves(Square))
    {
        if (legal->file == SquareMove.file &&
            legal->rank == SquareMove.rank)
        {
            fOK = true;
            break;
        }
    }
    if (!fOK)
        return false;

    // move the piece
    char p = piece( Square );
    myBoard.cell( Square.file, Square.rank )->text(' ');
    myBoard.cell( SquareMove.file, SquareMove.rank )->text(p);

    return true;
}

char cChess::piece(sq_t sq)
{
    try
    {
        return myBoard.cell(
                          sq.file, sq.rank)
            ->text();
    }
    catch (...)
    {
        return 'x';
    }
}
std::string cChess::pieceVerbose(sq_t sq)
{
    std::string ret;
    if (isWhite(sq))
        ret += "White ";
    else
        ret += "Black ";
    switch (piece(sq))
    {
    case 'p':
    case 'P':
        ret += "pawn";
        break;
    case 'r':
    case 'R':
        ret += "rook";
        break;
    case 'n':
    case 'N':
        ret += "knight";
        break;
    case 'b':
    case 'B':
        ret += "bishop";
        break;
    case 'q':
    case 'Q':
        ret += "queen";
        break;
    case 'k':
    case 'K':
        ret += "king";
        break;
    }
    return ret;
}
int cChess::pieceValue(sq_t sq)
{
    return pieceValue(piece(sq));
}
int cChess::pieceValue(char p) const
{
    switch (p)
    {
    case 'p':
    case 'P':
        return 1;
    case 'n':
    case 'N':
    case 'b':
    case 'B':
        return 3;
    case 'r':
    case 'R':
        return 5;
    case 'q':
    case 'Q':
        return 9;
    default:
        return 0;
    }
}

bool cChess::isEmpty(sq_t sq)
{
    try
    {
        return (piece(sq) == ' ');
    }
    catch (...)
    {
        return false;
    }
}
bool cChess::isWhite(sq_t sq)
{
    try
    {
        // white pieces are capital letters A to Z, Z == 90
        return (piece(sq) <= 90);
    }
    catch (...)
    {
        return false;
    }
}

bool cChess::isCapture(
    sq_t src,
    sq_t trg)
{
    try
    {
        myBoard.cell(src.file, src.rank);
        myBoard.cell(trg.file, trg.rank);

        if (isWhite(src))
        {
            if (!isWhite(trg))
                return true;
        }
        else
        {
            if (isWhite(trg))
                return true;
        }
        return false;
    }
    catch (...)
    {
        return false;
    }
}

void cChess::longMoves(
    std::vector<cChessSquare *> &ret,
    sq_t start,
    int fileInc,
    int rankInc)
{
    cChessSquare dst(
        start.file + fileInc,
        start.rank + rankInc);
    while (true)
    {
        if (!isEmpty(dst))
        {
            if (isCapture(start, dst))
                ret.push_back(myBoard.cell(dst.file, dst.rank));
            break; // blocked from going further
        }
        ret.push_back(myBoard.cell(dst.file, dst.rank));
        dst.rank += rankInc;
        dst.file += fileInc;
    }
}

std::vector<cChessSquare *> cChess::rookMoves(sq_t start)
{
    std::vector<cChessSquare *> ret;
    const int ri[4] = {0, 0, 1, -1};
    const int fi[4] = {1, -1, 0, 0};
    for (int d = 0; d < 4; d++)
    {
        longMoves(
            ret,
            start,
            fi[d],
            ri[d]);
    }
    return ret;
}
std::vector<cChessSquare *> cChess::knightMoves(sq_t start)
{
    std::vector<cChessSquare *> ret;
    const int ri[8] = {1, 1, -1, -1, 2, 2, -2, -2};
    const int fi[8] = {2, -2, 2, -2, 1, -1, 1, -1};
    for (int d = 0; d < 8; d++)
    {
        cChessSquare dst(
            start.file + fi[d],
            start.rank + ri[d]);

        if (isEmpty(dst))
            ret.push_back(myBoard.cell(dst.file, dst.rank));
        else if (isCapture(start, dst))
            ret.push_back(myBoard.cell(dst.file, dst.rank));
    }
    return ret;
}
std::vector<cChessSquare *> cChess::bishopMoves(sq_t start)
{
    std::vector<cChessSquare *> ret;
    const int ri[4] = {1, 1, -1, -1};
    const int fi[4] = {1, -1, 1, -1};
    for (int d = 0; d < 4; d++)
    {
        longMoves(
            ret,
            start,
            fi[d],
            ri[d]);
    }
    return ret;
}
std::vector<cChessSquare *> cChess::queenMoves(sq_t start)
{
    std::vector<cChessSquare *> ret;
    ret = rookMoves(start);
    auto vm = bishopMoves(start);
    ret.insert(ret.end(), vm.begin(), vm.end());
    return ret;
}

std::vector<cChessSquare *> cChess::pawnMoves(sq_t start)
{
    std::vector<cChessSquare *> ret;
    int rankdir = 1;
    if (isWhite(start))
        rankdir = -1;
    if (isEmpty(cChessSquare(start.file, start.rank + rankdir)))
    {
        ret.push_back(myBoard.cell(start.file, start.rank + rankdir));
        if (start.rank == 1 || start.rank == 6)
            if (isEmpty(cChessSquare(start.file, start.rank + 2 * rankdir)))
                ret.push_back(myBoard.cell(start.file, start.rank + 2 * rankdir));
    }
    cChessSquare dst(start.file - 1, start.rank + 1);
    if ((!isEmpty(dst)) && isCapture(start, dst))
        ret.push_back(myBoard.cell(start.file - 1, start.rank + 1));
    dst.file = start.file + 1;
    if ((!isEmpty(dst)) && isCapture(start, dst))
        ret.push_back(myBoard.cell(start.file - 1, start.rank + 1));
    return ret;
}
std::vector<cChessSquare *> cChess::kingMoves(sq_t start)
{
    std::vector<cChessSquare *> ret;
    int w, h;
    for (cChessSquare *c : myBoard.neighbours(start.file, start.rank))
    {
        myBoard.coords(w, h, c);
        cChessSquare dst(w, h);
        if (isEmpty(dst))
            ret.push_back(c);
        else if (isCapture(start, dst))
            ret.push_back(c);
    }
    return ret;
}

std::vector<cChessSquare *> cChess::moves(sq_t start)
{
    std::vector<cChessSquare *> ret;

    switch (piece(start))
    {
    case ' ':
    default:
        break;
    case 'p':
    case 'P':
        ret = pawnMoves(start);
        break;
    case 'r':
    case 'R':
        ret = rookMoves(start);
        break;
    case 'b':
    case 'B':
        ret = bishopMoves(start);
        break;
    case 'n':
    case 'N':
        ret = knightMoves(start);
        break;
    case 'q':
    case 'Q':
        ret = queenMoves(start);
        break;
    case 'k':
    case 'K':
        ret = kingMoves(start);
        break;
    }

    // fill in file and rank of legal cells
    for( auto psq : ret )
    {
        myBoard.coords( psq->file, psq->rank, psq );
    }

    return ret;
}

double cChess::strength(sq_t sq)
{
    // return the piece value times the number of legal moves available

    return pieceValue(sq) * moves(sq).size();
}
