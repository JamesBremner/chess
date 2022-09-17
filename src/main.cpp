#include <string>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <wex.h>
#include "cStarterGUI.h"
#include "autocell.h"

class cChessSquare : public cell::cCell
{
public:
    void text(char c)
    {
        myText = c;
    }
    char text()
    {
        return myText;
    }

private:
    char myText;
};

class cChess
{
public:
    cChess();

    /// @brief read FEN file containing chess board position
    /// @param fname
    ///
    /// https://en.wikipedia.org/wiki/Forsyth%E2%80%93Edwards_Notation

    void readFEN(const std::string &fname);

    /// @brief human readable rank position
    /// @param r rank
    /// @return human readable string

    std::string rank(int r);

    /// @brief describe a square in the position
    /// @param rank 
    /// @param file 
    /// @return human readable description

    std::string describe(int rank, int file);

private:
    cell::cAutomaton<cChessSquare> myBoard;

    const char algrank[8]{'8', '7', '6', '5', '4', '3', '2', '1'};
    const char algfile[8]{'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'};

    int pieceValue(char p) const;

    double entropy(int rank, int file);
};

class cGUI : public cStarterGUI
{
public:
    cGUI();

private:
    wex::panel &plBoard;
    wex::panel &plDescribe;

    cChess myChess;
};

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
    getline(iss, line);
    std::cout << line.substr(0, line.find(' ')) << "\n";
}
std::string cChess::rank(int r)
{
    std::string ret;
    ret.push_back(algrank[r]);
    ret.push_back(' ');
    for (int file = 0; file < 8; file++)
    {
        ret.append(1, myBoard.cell(file, r)->text());
    }
    ret.push_back(' ');
    ret.push_back(algrank[r]);
    return ret;
}
double cChess::entropy(int rank, int file)
{
    return pieceValue(myBoard.cell(file, rank)->text());
}
std::string cChess::describe(int rank, int file)
{
    std::stringstream ret;
    char Piece = myBoard.cell(file, rank)->text();
    if (Piece == ' ')
        return ret.str();
    ret<<algfile[file];
    ret<<algrank[rank];
    ret<<' ';
    ret<<Piece;
    ret << " ";
    ret<< std::setprecision(4) << entropy(rank, file);
    return ret.str();
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

cGUI::cGUI()
    : cStarterGUI(
          "Chess",
          {50, 50, 1000, 500}),
      plBoard(wex::maker::make<wex::panel>(fm)),
      plDescribe(wex::maker::make<wex::panel>(fm))
{
    wex::menubar mbar(fm);
    wex::menu mfile(fm);
    mfile.append(
        "Read FEN",
        [&](const std::string &title)
        {
            wex::filebox fb(fm);
            auto fname = fb.open();
            if (fname.empty())
                return;
            myChess.readFEN(fname);
            fm.text("Chess position " + fname);
            fm.update();
        });
    mbar.append("File", mfile);

    plBoard.move(20, 20, 300, 400);
    plBoard.fontName("courier");
    plBoard.events().draw(
        [this](PAINTSTRUCT &ps)
        {
            wex::shapes S(ps);
            S.textFontName("courier");
            S.text("  abcdefgh",
                   {0, 0});
            for (int rank = 0; rank < 8; rank++)
            {
                S.text(
                    myChess.rank(rank),
                    {0, (rank + 1) * 30});
            }
            S.text("  abcdefgh",
                   {0, 270});
        });

    plDescribe.move(400, 20, 300, 700);
    plDescribe.events().draw(
        [this](PAINTSTRUCT &ps)
        {
            wex::shapes S(ps);
            int row = 0;
            int col = 0;
            for (int rank = 0; rank < 8; rank++) {
                if( rank == 4 ) {
                    col = 100;
                    row = 0;
                }
                for (int file = 0; file < 8; file++)
                {
                    auto desc = myChess.describe(rank, file);
                    if (desc.size())
                        S.text(
                            desc,
                            {col, 20 * row++});
                }
            }
        });

    show();
    run();
}

int cell::cCell::myLastID = 0;

main()
{

    cGUI theGUI;
    return 0;
}
