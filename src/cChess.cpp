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

        std::cout << line << "\n";
    }
    // getline(iss, line);
    // std::cout << line.substr(0, line.find(' ')) << "\n";
}

std::string cChess::text()
{
    std::string ret;
    for( int r=0; r< 8; r++)
        ret += std::to_string(r) + " "+ rank(r) + "\n";
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

std::string cChess::describe(int file, int rank)
{
    std::stringstream ret;
    char Piece = myBoard.cell(file, rank)->text();
    if (Piece == ' ')
        return ret.str();
    ret << algfile[file];
    ret << algrank[rank];
    ret << ' ';
    ret << Piece;
    ret << " ";
    ret << std::setprecision(4) << entropy(file, rank);
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

char cChess::piece( sq_t sq )
{
    return myBoard.cell(
            sq.file, sq.rank)->text();
}
int cChess::pieceValue( sq_t sq ) 
{
    return pieceValue(piece( sq ));
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
    case 'p': case 'P':
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
    return ret;
}

double cChess::entropy(int file, int rank)
{
    // this is a stub, waiting for the entropy specification
    // meantime, return the piece value times the number of legal moves available

    cChessSquare sq(file,rank);
    return pieceValue(sq) * moves(sq).size();
}
