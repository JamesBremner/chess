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
    wex::panel &plBoard;        /// LHS display of layout
    wex::panel &plDescribe;     /// rHS display describing occupied squares

    cChess myChess;
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

main()
{
    cGUI theGUI;
    return 0;
}
