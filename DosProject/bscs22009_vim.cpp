#include "bscs22009_document.cpp"

using namespace std;

class MeoVim {
    vector<Document *> opened;
    vector<Document *>::iterator current;

    Document *File() {
        return *(current);
    }

    enum class modes {
        Command,
        Insert,
        Visual
    };
    modes mode;

    void printMode() {
        int rows, cols;
        ConsoleHandler::getConsoleSize(cols, rows);
        ConsoleHandler::gotoRowCol(rows - 2, 0);
        ConsoleHandler::SetClr(BLACK, CYAN);
        for (size_t i = 0; i < cols; i++) {
            cout << ' ';
        }
        ConsoleHandler::gotoRowCol(rows - 2, 0);
        if (mode == modes::Insert) {
            cout << "[insert]";
        } else if (mode == modes::Visual) {
            cout << "[visual]";
        }
        ConsoleHandler::SetClr(WHITE, BLACK);
    }

    void printCommand(string str) {
        int rows, cols;
        ConsoleHandler::getConsoleSize(cols, rows);
        ConsoleHandler::gotoRowCol(rows - 1, 0);
        for (size_t i = 0; i < cols; i++) {
            cout << ' ';
        }
        ConsoleHandler::gotoRowCol(rows - 1, 0);
        cout << str;
    }

    void print() {

        printMode();
        int currentFG = WHITE;
        int currentBG = BLACK;

        Document *file = File();
        ConsoleHandler::gotoRowCol(0, 0);
        for (auto it = file->content.begin(); it != file->content.end(); it++) {
            if (it == file->selectionBegin && it != file->selectionEnd) {
                ConsoleHandler::SetClr(BLACK, CYAN);
                currentFG = BLACK;
                currentBG = CYAN;
            } else if (it == file->selectionEnd) {
                ConsoleHandler::SetClr(WHITE, BLACK);
                currentFG = WHITE;
                currentBG = BLACK;
            }

            if (it == file->cursor) {
                ConsoleHandler::SetClr(BLACK, WHITE);
                if (*it == '\n')
                    cout << ' ';
                cout << *it;
                ConsoleHandler::SetClr(currentFG, currentBG);
            }
            else {
                if (*it == '\n')
                    cout << ' ';
                cout << *it;
            }
        }
    }

public:
    MeoVim() : opened(), current() {
        mode = modes::Command;
    }

    void openFile(Document *fileAddress) {
        auto alreadyOpened = find(opened.begin(), opened.end(), fileAddress);

        if (alreadyOpened == opened.end()) {
            opened.push_back(fileAddress);
            current = --opened.end();
        }
        else {
            current = alreadyOpened;
        }
    }

    void closeFile(Document* fileAddress) {
        auto alreadyOpened = find(opened.begin(), opened.end(), fileAddress);

        if (alreadyOpened != opened.end()) {
            opened.erase(alreadyOpened);
        }
    }

    void run() {
        if (opened.empty()) {
            cout << "No file opened...\n\n";
            return;
        } 
        system("cls");
        print();

        while (true) {
            print();
            int key = _getch();
            // up/down etc keys
            if (key == 224) {
                key = _getch();
                switch (key)
                {
                case 77: // right
                    File()->moveCursorToRight();
                    break;
                case 75: // left
                    File()->moveCursorToLeft();
                    break;
                case 72: // up
                    File()->moveCursorUp();
                    break;
                case 80: // down
                    File()->moveCursorDown();
                    break;
                default:
                    break;
                }
            }
            else if (key == 'i') { // insert mode
                mode = modes::Insert;
                File()->resetSelection();
                print();
                do {
                    key = _getch();
                    if (key == 27) {
                        break; // esc
                    }
                    else if (key == 224) {
                        key = _getch();
                        switch (key)
                        {
                        case 77: // right
                            File()->moveCursorToRight();
                            break;
                        case 75: // left
                            File()->moveCursorToLeft();
                            break;
                        case 72: // up
                            File()->moveCursorUp();
                            break;
                        case 80: // down
                            File()->moveCursorDown();
                            break;
                        default:
                            break;
                        }
                        print();
                        continue;
                    }

                    else if (key == 8) { // backspace
                        File()->removeAtCursor();
                        system("cls");
                        print();
                        continue;
                    }
                    else if (key == 13) { // enter
                        File()->insertAtCursor('\n');
                        system("cls");
                        print();
                    }
                    else {
                        File()->insertAtCursor(key);
                        print();
                    }
                } while (key != 27);
                mode = modes::Command;
            }
            else if (key == 'v') { // visual mode
                mode = modes::Visual;
                File()->beginSelection();
                print();
                do {
                    key = _getch();
                    if (key == 27) {
                        break; // esc
                    }
                    else if (key == 224) {
                        key = _getch();
                        switch (key)
                        {
                        case 77: // right
                            File()->moveCursorToRight();
                            File()->moveSelectionToRight();
                            break;
                        case 75: // left
                            File()->moveCursorToLeft();
                            File()->moveSelectionToLeft();
                            break;
                        default:
                            break;
                        }
                        print();
                        continue;
                    }
                } while (key != 27);
                mode = modes::Command;
                //File()->resetSelection();
            } 
            else if (key == 'u' && File()->somethingSelected()) {
                File()->selectionToUpper();
                File()->resetSelection();
            } else if (key == 'l' && File()->somethingSelected()) {
                File()->selectionToLower();
                File()->resetSelection();
            }
            else if (key == '/') { // find
                File()->resetSelection();
                print();
                printCommand("/");
                string find;
                getline(cin, find);
                if (!File()->find(find)) {
                    printCommand("Not Foud :(");
                }
            } else if (key == 'n' && File()->somethingSelected()) {
                if (!File()->findNext()) {
                    printCommand("Not Foud :(");
                }
            } else if (key == 'p' && File()->somethingSelected()) {
                if (!File()->findPrev()) {
                    printCommand("Not Found :(");
                }
            } else if (key == 'r' && File()->somethingSelected()) {
                printCommand("replace: ");
                string replace;
                getline(cin, replace);
                File()->replace(replace);
                system("cls");
                print();
            } else if (key == ':') {
                printCommand(":");
                string cmd;
                getline(cin, cmd);

                if (cmd == "q" || cmd == "Q") {
                    system("cls");
                    return;
                } else if (cmd == "wq" || cmd == "WQ") {
                    opened.erase(current);
                    system("cls");
                    return;
                } else if (cmd == "next") {
                    if (++current == opened.end()) {
                        current = opened.begin();
                    }
                    system("cls");
                } else if (cmd == "findw") {
                    File()->resetSelection();
                    print();
                    printCommand("/");
                    string find;
                    getline(cin, find);
                    if (!File()->findWord(find)) {
                        printCommand("Not Foud :(");
                    }
                } else if (cmd == "finds") {
                    File()->resetSelection();
                    print();
                    printCommand("/");
                    string find;
                    getline(cin, find);
                    if (!File()->findSentence(find)) {
                        printCommand("Not Foud :(");
                    }
                } else if (cmd == "pre" && File()->somethingSelected()) {
                    print();
                    printCommand("prefix: ");
                    string pre;
                    getline(cin, pre);
                    File()->insertPrefix(pre);
                } else if (cmd == "post" && File()->somethingSelected()) {
                    print();
                    printCommand("postfix: ");
                    string post;
                    getline(cin, post);
                    File()->insertPostFix(post);
                } else if (cmd == "spcount") {
                    print();
                    stringstream ss;
                    ss << "Special Count: " << File()->spCount();
                    printCommand(ss.str());
                } else if (cmd == "paracount") {
                    print();
                    stringstream ss;
                    ss << "Para count: " << File()->paraCount();
                    printCommand(ss.str());
                }
            }
        }
    }
};
