#include <string>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <wex.h>
#include "cStarterGUI.h"

#include "cChess.h"

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
    myChess.readFEN("start.fen");

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

            for( auto q : myChess.moves( 0,4) )
                std::cout << myChess.algebraic(q) << " ";
        });
    mbar.append("File", mfile);

    plBoard.move(20, 20, 300, 400);
    plBoard.fontName("courier");
    plBoard.events().draw(
        [this](PAINTSTRUCT &ps)
        {
            wex::shapes S(ps);
            S.textFontName("courier");
            S.text("  a b c d e f g h",
                   {0, 0});
            for (int rank = 0; rank < 8; rank++)
            {
                S.text(
                    myChess.rank(rank),
                    {0, (rank + 1) * 30});
            }
            S.text("  a b c d e f g h",
                   {0, 270});
        });

    plDescribe.move(400, 20, 300, 700);
    plDescribe.events().draw(
        [this](PAINTSTRUCT &ps)
        {
            wex::shapes S(ps);
            S.textFontName("courier");
            int row = 0;
            int col = 0;
            for (int rank = 0; rank < 8; rank++)
            {
                if (rank == 4)
                {
                    col = 100;
                    row = 0;
                }
                for (int file = 0; file < 8; file++)
                {
                    auto desc = myChess.describe(file, rank);
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
