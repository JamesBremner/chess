#include <string>
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
    wex::panel &plBoard;    /// LHS display of layout
    wex::panel &plDescribe; /// rHS display describing occupied squares

    cChess myChess;
    cChessSquare mySquare;

    void boardConst();
};

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
        });
    mbar.append("File", mfile);

    boardConst();

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

void cGUI::boardConst()
{
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

            char piece = myChess.piece(mySquare);
            if (piece != 'x'  && piece != ' ')
            {
                int x = 35 + mySquare.file * 23;
                int y = 40 + mySquare.rank * 30;
                S.circle(
                    x,y,15);
                S.text(
                    std::string(1, piece),
                    {x - 8,
                     y - 10});
            }
        });
    plBoard.events().mouseMove(
        [this](wex::sMouse &m)
        {
            const int xstart = 22;
            const int xinc = 22;
            const int ystart = 30;
            const int yinc = 30;

            int file = (m.x - xstart) / xinc;
            int rank = (m.y - ystart) / yinc;
            // std::cout
            //     << " mouse move "
            //     << m.x << " " << m.y << " ";
            // std::cout << file << " " << rank
            //           << " " << myChess.piece(cChessSquare(file, rank)) << "\n";
            mySquare.file = file;
            mySquare.rank = rank;
            plBoard.update();
        });
}
main()
{
    cGUI theGUI;
    return 0;
}
