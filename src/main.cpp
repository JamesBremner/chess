#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <wex.h>
#include "cStarterGUI.h"
#include "autocell.h"

class cChessCell : public cell::cCell
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
    /// @return human reradable string
    std::string rank(int r );

private:
    cell::cAutomaton<cChessCell> myBoard;
};

class cGUI : public cStarterGUI
{
public:
    cGUI();

private:
    wex::panel & plBoard;

    cChess myChess;
};

    cChess::cChess()
        : myBoard(cell::cAutomaton<cChessCell>(8, 8))
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
        for( char c : line )
        {
            if( c == ' ')
                break;
            int count;
            switch( c ) {
                case 'p': case 'P': case 'r': case 'R': case 'n':
                case 'N': case 'b': case 'B': case 'q':
                case 'Q': case 'k': case 'K':
                myBoard.cell( file, rank )->text( c);
                file++;
                break;
                default:
                count = c - '0';
                for( int k = 0; k < count; k++ ) {
                    myBoard.cell( file, rank )->text(' ');
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
std::string cChess::rank(int r )
{
    std::string ret;
    for (int file = 0; file<8; file++ )
    {
        ret.append( 1, myBoard.cell(file,r)->text());
    }
    return ret;
}


cGUI::cGUI()
    : cStarterGUI(
          "Chess",
          {50, 50, 1000, 500}),
      plBoard(wex::maker::make<wex::panel>(fm))
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

    plBoard.move( 20,20,300,300);
    plBoard.fontName("courier");
    plBoard.events().draw(
        [this]( PAINTSTRUCT& ps )
        {
            wex::shapes S(ps);
            S.textFontName("courier");
            for( int rank = 0; rank<8;rank++)
            {
                S.text(
                    myChess.rank(rank),
                    {0,rank*30});
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
