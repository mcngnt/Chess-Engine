#define SFML_STATIC

#include "utils.h"
#include "engine.h"


inline sf::Sprite create_sprite(sf::Texture* tex,std::string path)
{
    tex->loadFromFile(path);
    sf::Sprite spr;
    spr.setTexture(*tex);
    sf::Vector2f sprSize(spr.getTexture()->getSize().x * spr.getScale().x,spr.getTexture()->getSize().y * spr.getScale().y);
    spr.setOrigin(sprSize/2.f);
    spr.setScale(sf::Vector2f(0.05, 0.05));
    return spr;
}



void centerText(sf::Text* t_)
{
    sf::FloatRect textRect = t_->getLocalBounds();
    t_->setOrigin(textRect.left + textRect.width/2.0f,
    textRect.top  + textRect.height/2.0f);
}



int main()
{
	srand(time(NULL));
    sf::RenderWindow window(sf::VideoMode(SCREEN_W, SCREEN_H, 32), "Main", sf::Style::Default);
    window.setVerticalSyncEnabled(true);

    sf::Color evenPieceCol(155,106,92);
    sf::Color oddPieceCol(231,209,186);
    sf::Color hoverEvenCol(210, 91, 85);
    sf::Color hoverOddCol(244, 137, 127);
    sf::Color greenCol(20, 240, 50);

    sf::Font font;
    font.loadFromFile("res/GoogleSans-Bold.ttf");
    sf::Text text;
    text.setFont(font);



    Engine engine(Random, Random);

    // engine.currentMoves = engine.board.generateMoves();

    sf::Texture piecesTex[23];
    sf::Sprite piecesSpr[23];
    piecesSpr[White | Pawn] = create_sprite(&piecesTex[White | Pawn], "res/w_pawn.png");
    piecesSpr[White | Rook] = create_sprite(&piecesTex[White | Rook], "res/w_rook.png");
    piecesSpr[White | Knight] = create_sprite(&piecesTex[White | Knight], "res/w_knight.png");
    piecesSpr[White | Bishop] = create_sprite(&piecesTex[White | Bishop], "res/w_bishop.png");
    piecesSpr[White | Queen] = create_sprite(&piecesTex[White | Queen], "res/w_queen.png");
    piecesSpr[White | King] = create_sprite(&piecesTex[White | King], "res/w_king.png");
    piecesSpr[Black | Pawn] = create_sprite(&piecesTex[Black | Pawn], "res/b_pawn.png");
    piecesSpr[Black | Rook] = create_sprite(&piecesTex[Black | Rook], "res/b_rook.png");
    piecesSpr[Black | Knight] = create_sprite(&piecesTex[Black | Knight], "res/b_knight.png");
    piecesSpr[Black | Bishop] = create_sprite(&piecesTex[Black | Bishop], "res/b_bishop.png");
    piecesSpr[Black | Queen] = create_sprite(&piecesTex[Black | Queen], "res/b_queen.png");
    piecesSpr[Black | King] = create_sprite(&piecesTex[Black | King], "res/b_king.png");


    for (int i = 0 ; i < 23 ; ++i)
    {
        piecesSpr[i].scale(2.0f, 2.0f);
    }

    // int depth = 6;
    // engine.test(depth);


    // printf("%d\n", engine.board.whiteToMove);
    // printf("%d\n", engine.aux(depth));

    // BitBoard bitboard = engine.board.boardToBitBoard();
    // engine.board.loadBitBoard(bitboard);



    int tick = 0;

    int pieceSize = 95;
    sf::Vector2f startPos(270, 170);


    bool holdPiece = false;

    sf::Vector2i pieceHeld(-1,-1);
    int pieceHeldType = 0;
    bool spaceTriggered = false;




    while(window.isOpen())
    {
    	sf::Vector2f mousePos = sf::Vector2f(sf::Mouse::getPosition(window));

        sf::Vector2i gridPos(int((mousePos.x - startPos.x)/pieceSize), int((mousePos.y - startPos.y)/pieceSize));


        sf::Event event;
        while (window.pollEvent(event))
        {
            if ((event.type == sf::Event::Closed) || ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Escape)))
            {
                window.close();
                break;
            }

            // if(((engine.board.whiteToMove && !engine.isWhiteBot) || (!engine.board.whiteToMove && !engine.isBlackBot)) && !engine.checkmate )
            // {
                if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
                {
                    if (!holdPiece && gridPos.x >= 0 && gridPos.y >= 0 && gridPos.x <= 7 && gridPos.y <= 7 && engine.get(gridPos) > 0)
                    {
                        holdPiece = true;
                        pieceHeld = gridPos;
                        pieceHeldType = engine.get(gridPos);
                    }
                }
                if (event.type == sf::Event::MouseButtonReleased)
                {
                    if (holdPiece && gridPos.x >= 0 && gridPos.y >= 0 && gridPos.x <= 7 && gridPos.y <= 7)
                    {
                        holdPiece = false;
                        engine.tryMove(pieceHeld, gridPos);
                        pieceHeld = sf::Vector2i(-1,-1);
                    }
                }
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && !spaceTriggered)
                {
                    // engine.unmakeMove();
                    spaceTriggered = true;

                    engine.update();

                    std::string currentFen = engine.board.convertFen();
                    printf("%s\n", currentFen.c_str());
                    // engine.board.loadFen(currentFen);

                    // printf("Olala\n");
                }
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z) && !spaceTriggered)
                {
                    engine.unmakeMove();

                }
                // if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
                // {
                //     for (int i = 1; i <= 5;++i)
                //     {
                //         printf("%s\n", standardNotation(engine.whiteBot.pvMoves[i]).c_str());
                //     }
                // }
            // }

        }

        tick++;
        window.clear();

        spaceTriggered = false;
        

        // printf("%s\n", "Ouf 3");

        // engine.update();



        for (int i = 0 ; i < 8; ++i)
        {
            for (int j = 0 ; j < 8; ++j)
            {

                // Draw Squares
                
                int size = 95;
                
                sf::Vector2f pos = startPos + sf::Vector2f(j * pieceSize,i * pieceSize);

                sf::RectangleShape rectangle(sf::Vector2f(size,size));
                if ((i + j) % 2 == 0)
                {
                    rectangle.setFillColor(evenPieceCol);
                }
                else
                {
                    rectangle.setFillColor(oddPieceCol);
                }

                if (holdPiece)
                {
                    if (engine.isLegal(genMove(pieceHeld.x, pieceHeld.y, j,i, 0)))
                    {
                        if ((i + j) % 2 == 0)
                        {
                            rectangle.setFillColor(hoverEvenCol);
                        }
                        else
                        {
                            rectangle.setFillColor(hoverOddCol);
                        }
                    }
                }
                
                // engine.board.whiteToMove = !engine.board.whiteToMove;
                // engine.board.controlledSquares();
                // engine.board.whiteToMove = !engine.board.whiteToMove;
                // if (engine.board.controlled[i][j])
                // {
                //     engine.board.whiteToMove = !engine.board.whiteToMove;
                //     engine.board.controlledSquares();
                //     engine.board.whiteToMove = !engine.board.whiteToMove;
                //     rectangle.setFillColor(greenCol);
                // }

                rectangle.setPosition(pos);
                window.draw(rectangle);

                // Draw Pieces*
                // printf("%s %d %d\n", "Ouf ", i, j);

                int pnum = engine.get(j,i);
                // printf("%d\n", pnum);

                if (pnum > 0 && sf::Vector2i(j,i) != pieceHeld)
                {
                    piecesSpr[pnum].setPosition(pos + sf::Vector2f(size/2, size/2));
                    window.draw(piecesSpr[pnum]);
                }

                // printf("%s %d %d\n", "Ouf2 ", i, j);


            }
        }



        if (holdPiece)
        {
            piecesSpr[pieceHeldType].setPosition(mousePos);
            window.draw(piecesSpr[pieceHeldType]);
        }


        if (engine.checkmate)
        {
            if (engine.board.isChecked())
            {
                text.setString("Echec et mat");
            }
            else
            {
                text.setString("Pat");
            }
            text.setPosition(sf::Vector2f(650, 80));
            text.setCharacterSize(80);
            centerText(&text);
            window.draw(text);
            // engine.checkmate = false;
            // engine.board.loadFen(engine.board.startingFen);
        }

        window.display();

    }

	return 0;
}