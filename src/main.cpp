#define SFML_STATIC

#include "utils.h"
#include "engine.h"

#define botBaseTime 50

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

int time_to_alloc(int baseTime, int inc)
{
    if (baseTime == 0)
    {
        return botBaseTime;
    }
    if (baseTime < 5 * inc)
    {
        return inc;
    }
    return (baseTime/30) + inc;
}



int main()
{
    bool useUCI = true;

	// srand(time(NULL));
    // sf::RenderWindow window(sf::VideoMode(SCREEN_W, SCREEN_H, 32), "Main", sf::Style::Default);
    // window.setVerticalSyncEnabled(true);

    // sf::Color evenPieceCol(231,209,186);
    // sf::Color oddPieceCol(155,106,92);
    // sf::Color hoverEvenCol(210, 91, 85);
    // sf::Color hoverOddCol(244, 137, 127);

    // sf::Color fromEvenCol(244,213,79);
    // sf::Color fromOddCol(212,168,39);
    // sf::Color toEvenCol(215,190,124);
    // sf::Color toOddCol(164, 121, 61);

    // sf::Color greenCol(20, 240, 50);

    // sf::Font font;
    // font.loadFromFile("res/GoogleSans-Bold.ttf");
    // sf::Text text;
    // text.setFont(font);

    // sf::Texture screenTexture;
    // screenTexture.create(SCREEN_W, SCREEN_H); 


    Engine engine(NotBot, Good);



    engine.whiteBot.maxTime = botBaseTime;
    engine.blackBot.maxTime = botBaseTime;


    // sf::Texture piecesTex[23];
    // sf::Sprite piecesSpr[23];

    // if (!useUCI)
    // {
    //     piecesSpr[White | Pawn] = create_sprite(&piecesTex[White | Pawn], "res/w_pawn.png");
    //     piecesSpr[White | Rook] = create_sprite(&piecesTex[White | Rook], "res/w_rook.png");
    //     piecesSpr[White | Knight] = create_sprite(&piecesTex[White | Knight], "res/w_knight.png");
    //     piecesSpr[White | Bishop] = create_sprite(&piecesTex[White | Bishop], "res/w_bishop.png");
    //     piecesSpr[White | Queen] = create_sprite(&piecesTex[White | Queen], "res/w_queen.png");
    //     piecesSpr[White | King] = create_sprite(&piecesTex[White | King], "res/w_king.png");
    //     piecesSpr[Black | Pawn] = create_sprite(&piecesTex[Black | Pawn], "res/b_pawn.png");
    //     piecesSpr[Black | Rook] = create_sprite(&piecesTex[Black | Rook], "res/b_rook.png");
    //     piecesSpr[Black | Knight] = create_sprite(&piecesTex[Black | Knight], "res/b_knight.png");
    //     piecesSpr[Black | Bishop] = create_sprite(&piecesTex[Black | Bishop], "res/b_bishop.png");
    //     piecesSpr[Black | Queen] = create_sprite(&piecesTex[Black | Queen], "res/b_queen.png");
    //     piecesSpr[Black | King] = create_sprite(&piecesTex[Black | King], "res/b_king.png");


    //     for (int i = 0 ; i < 23 ; ++i)
    //     {
    //         piecesSpr[i].scale(2.4f, 2.4f);
    //     }
    // }



    // // int depth = 4;
    // // engine.test(depth);

    // // std::chrono::high_resolution_clock::time_point startTime = std::chrono::high_resolution_clock::now();

    // // engine.whiteBot.search(&engine.board, 6, -infinity, infinity);
    // // engine.whiteBot.playWell(&engine.board);

    // // std::chrono::high_resolution_clock::time_point endTime = std::chrono::high_resolution_clock::now();
    // // std::chrono::milliseconds duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);

    // // std::cout << duration.count() << std::endl;


    // int tick = 0;

    // int pieceSize = 138;
    // sf::Vector2f startPosDraw(0,0);


    // bool holdPiece = false;

    // sf::Vector2i pieceHeld(-1,-1);
    // int pieceHeldType = 0;
    // bool spaceTriggered = false;

    // bool doUpdate = false;

    // int countIm = 0;

    // std::string totalCom;

    // if(useUCI)
    // {
    //     window.close();
    // }

    // while(window.isOpen())
    while(true)
    {
        // doUpdate = true;
    	// sf::Vector2f mousePos = sf::Vector2f(sf::Mouse::getPosition(window));

        // sf::Vector2i gridPos(int((mousePos.x - startPosDraw.x)/pieceSize), int((mousePos.y - startPosDraw.y)/pieceSize));


        // sf::Event event;
        // while (window.pollEvent(event) && !useUCI)
        // {
        //     if ((event.type == sf::Event::Closed) || ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Escape)))
        //     {
        //         window.close();
        //         break;
        //     }

        //     // if(((engine.board.whiteToMove && !engine.isWhiteBot) || (!engine.board.whiteToMove && !engine.isBlackBot)) && !engine.checkmate )
        //     // {
        //         if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
        //         {
        //             if (!holdPiece && gridPos.x >= 0 && gridPos.y >= 0 && gridPos.x <= 7 && gridPos.y <= 7 && engine.get(gridPos) > 0)
        //             {
        //                 holdPiece = true;
        //                 pieceHeld = gridPos;
        //                 pieceHeldType = engine.get(gridPos);
        //             }
        //         }
        //         if (event.type == sf::Event::MouseButtonReleased)
        //         {
        //             if (holdPiece && gridPos.x >= 0 && gridPos.y >= 0 && gridPos.x <= 7 && gridPos.y <= 7)
        //             {
        //                 holdPiece = false;
        //                 engine.tryMove(pieceHeld, gridPos, 'q');
        //                 pieceHeld = sf::Vector2i(-1,-1);
        //                 doUpdate = false;
        //             }
        //         }
        //         if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && !spaceTriggered)
        //         {
        //             // engine.unmakeMove();
        //             spaceTriggered = true;

        //             engine.update();

        //             // std::string currentFen = engine.board.convertFen();
        //             // engine.board.loadFen(currentFen);

        //         }
        //         if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z) && !spaceTriggered)
        //         {
        //             engine.unmakeMove();

        //         }
        //         if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
        //         {
        //             screenTexture.update(window);
        //             if (screenTexture.copyToImage().saveToFile("res/" + std::to_string(engine.board.zobristKey + countIm) + ".png"))
        //             {
        //                 std::cout << "screenshot saved to " << "res/" + std::to_string(engine.board.zobristKey + countIm) + ".png" << std::endl;
        //             }
        //             countIm += 1;
        //         }
        //     // }

        // }

        // tick++;
        // window.clear(sf::Color(255, 255, 255));

        // spaceTriggered = false;

        
        // if (doUpdate)
        // {
        //     // printf("%d\n", engine.board.get(21));
        //     engine.update();
        //     doUpdate=false;
        // }



        // if (!useUCI)
        // {
        //     for (int i = 0 ; i < 8; ++i)
        //     {
        //         for (int j = 0 ; j < 8; ++j)
        //         {

        //             // Draw Squares
                                    
        //             sf::Vector2f pos = startPosDraw + sf::Vector2f(j * pieceSize,i * pieceSize);



        //             sf::RectangleShape rectangle(sf::Vector2f(pieceSize, pieceSize));
        //             if ((i + j) % 2 == 0)
        //             {
        //                 rectangle.setFillColor(evenPieceCol);
        //             }
        //             else
        //             {
        //                 rectangle.setFillColor(oddPieceCol);
        //             }


        //             if (!engine.movesHistory.empty())
        //             {
        //                 int move = engine.movesHistory.top();
        //                 sf::Vector2i start = posIntTo2D(startPos(move));
        //                 sf::Vector2i end = posIntTo2D(endPos(move));
        //                 if (j == start.x && i == start.y)
        //                 {
        //                     if ((i + j) % 2 == 0)
        //                     {
        //                         rectangle.setFillColor(fromEvenCol);
        //                     }
        //                     else
        //                     {
        //                         rectangle.setFillColor(fromOddCol);
        //                     }
        //                 }
        //                 if (j == end.x && i == end.y)
        //                 {
        //                     if ((i + j) % 2 == 0)
        //                     {
        //                         rectangle.setFillColor(toEvenCol);
        //                     }
        //                     else
        //                     {
        //                         rectangle.setFillColor(toOddCol);
        //                     }
        //                 }
        //             }


        //             if (holdPiece)
        //             {
        //                 if (engine.isLegal(genMove(pieceHeld.x, pieceHeld.y, j,i, 0)))
        //                 {
        //                     if ((i + j) % 2 == 0)
        //                     {
        //                         rectangle.setFillColor(hoverEvenCol);
        //                     }
        //                     else
        //                     {
        //                         rectangle.setFillColor(hoverOddCol);
        //                     }
        //                 }
        //             }


        //             // for (int k = 0; k < 64; ++k)
        //             // {
        //             //     if (engine.isLegal(genMove(k%8, k/8, j,i, 0)))
        //             //     {
        //             //         if ((i + j) % 2 == 0)
        //             //         {
        //             //             rectangle.setFillColor(hoverEvenCol);
        //             //         }
        //             //         else
        //             //         {
        //             //             rectangle.setFillColor(hoverOddCol);
        //             //         }
        //             //     }
        //             // }
                    
                    
        //             // engine.board.whiteToMove = !engine.board.whiteToMove;
        //             // engine.board.controlledSquares();
        //             // engine.board.whiteToMove = !engine.board.whiteToMove;
        //             // if (engine.board.controlled[i][j])
        //             // {
        //             //     engine.board.whiteToMove = !engine.board.whiteToMove;
        //             //     engine.board.controlledSquares();
        //             //     engine.board.whiteToMove = !engine.board.whiteToMove;
        //             //     rectangle.setFillColor(greenCol);
        //             // }

                   

        //             rectangle.setPosition(pos);
        //             window.draw(rectangle);


        //             int pnum = engine.get(j,i);

        //             if (pnum > 0 && sf::Vector2i(j,i) != pieceHeld)
        //             {
        //                 piecesSpr[pnum].setPosition(pos + sf::Vector2f(pieceSize/2, pieceSize/2));
        //                 window.draw(piecesSpr[pnum]);
        //             }

        //             // text.setPosition(pos + sf::Vector2f(pieceSize/2, pieceSize/2));
        //             // text.setColor(sf::Color(0,0,0));
        //             // centerText(&text);
        //             // text.setString(std::to_string(j + 8 * i));
        //             // text.setCharacterSize(80);
        //             // window.draw(text);

        //         }
        //     }
        // }



        // if (holdPiece)
        // {
        //     piecesSpr[pieceHeldType].setPosition(mousePos);
        //     window.draw(piecesSpr[pieceHeldType]);
        // }


        // if (engine.checkmate)
        // {
        //     if (engine.board.isChecked())
        //     {
        //         text.setString("Echec et mat");
        //     }
        //     else
        //     {
        //         text.setString("Pat");
        //     }
        //     text.setPosition(sf::Vector2f(550, 550));
        //     text.setCharacterSize(80);
        //     text.setOutlineColor(sf::Color(0,0,0));
        //     text.setOutlineThickness(5);
        //     centerText(&text);
        //     window.draw(text);
        //     // engine.checkmate = false;
        //     // engine.board.loadFen(engine.board.startingFen);
        // }


        // window.display();

        if (useUCI)
        {
            std::string command;
            std::getline(std::cin, command);
            std::string zstring = std::to_string(engine.board.whiteToMoveZobrist);
            std::ofstream file(  zstring + ".txt", std::ios::app);
            if (file.is_open()) {
                file << command << std::endl;
                file.close();
            }

            std::vector<std::string> words;
            std::stringstream ss(command);
            std::string str;
            while (std::getline(ss, str, ' ')) 
            {
                words.push_back(str);
            }

            bool readPos = false;

            // Pos mode :
            // 0 : None
            // 1 : Fen
            // 2 : startpos
            int posMode = 0;
            bool readMoves = false;

            bool readBlackTime = false;
            bool readWhiteTime = false;
            bool readBlackInc = false;
            bool readWhiteInc = false;
            bool readMoveTime = false;

            bool needToGo = false;

            int whiteBaseTime = 0;
            int whiteInc = 0;
            int blackBaseTime = 0;
            int blackInc = 0;

            int moveTime = 0;

            std::string word;

            std::string fen;

            for (std::string word : words)
            {
                if (word == "quit")
                {
                    return 0;
                }
                if (word == "uci")
                {
                    std::cout << "uciok" << std::endl;
                    break;
                }
                if (word == "isready")
                {
                    std::cout << "readyok" << std::endl;
                    break;
                }
                if (word == "movetime")
                {
                    readMoveTime = true;
                    continue;
                }
                if (readMoveTime)
                {
                    moveTime = atoi(word.c_str());
                    readMoveTime = false;
                    continue;
                }
                if (word == "go")
                {
                    needToGo = true;
                }
                if (word == "wtime")
                {
                    readWhiteTime = true;
                    continue;
                }
                if (readWhiteTime)
                {
                    whiteBaseTime = atoi(word.c_str());
                    readWhiteTime = false;
                    continue;
                }
                if (word == "btime")
                {
                    readBlackTime = true;
                    continue;
                }
                if (readBlackTime)
                {
                    blackBaseTime = atoi(word.c_str());
                    readBlackTime = false;
                    continue;
                }
                if (word == "winc")
                {
                    readWhiteInc = true;
                    continue;
                }
                if (readWhiteInc)
                {
                    whiteInc = atoi(word.c_str());
                    readWhiteInc = false;
                    continue;
                }
                if (word == "binc")
                {
                    readBlackInc = true;
                    continue;
                }
                if (readBlackInc)
                {
                    blackInc = atoi(word.c_str());
                    readBlackInc = false;
                    continue;
                }
                if (word == "position")
                {
                    readPos = true;
                    continue;
                }
                if (word == "startpos" && readPos)
                {
                    posMode = 2;
                    engine.board.loadFen(engine.board.startingFen);
                    engine.currentMoves = engine.board.generateMoves(false);
                    engine.movesHistory = std::stack<int>();
                    continue;
                }
                if (word == "fen" && readPos)
                {
                    posMode = 1;
                    continue;
                }
                if (readPos && posMode == 1)
                {
                    fen += word + " ";
                    continue;
                }
                if (posMode == 2 && word == "moves")
                {
                    engine.board.whiteToMove = true;
                    readMoves = true;
                    continue;
                }
                if(posMode == 2 && readMoves)
                {
                    // printf("hihi\n");
                    if (word.size() == 4)
                    {
                        engine.tryMove(standNotToMove(word), ' ');
                    }
                    else
                    {
                        char c = word[4];
                        word.pop_back();
                        engine.tryMove(standNotToMove(word), c);
                    }
                    continue;
                }
            }

            if (readPos && posMode == 1)
            {
                engine.board.loadFen(fen);
                engine.currentMoves = engine.board.generateMoves(false);
                engine.movesHistory = std::stack<int>();
            }

            if (needToGo)
            {
                engine.whiteBot.maxTime = time_to_alloc(whiteBaseTime, whiteInc);
                engine.blackBot.maxTime = time_to_alloc(blackBaseTime, blackInc);

                // if (moveTime > 0)
                // {
                //     engine.whiteBot.maxTime = moveTime;
                //     engine.blackBot.maxTime = moveTime;   
                // }

                int bestMove = engine.getBestMove();
                std::string bestMoveString;
                bestMoveString  = standardNotation(bestMove);
                std::cout << "bestmove ";
                std:: cout << bestMoveString << std:: endl;
            }

        }



        // window.display();

    }

	return 0;
}