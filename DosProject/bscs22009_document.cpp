#include "utility.h"

using namespace std;

class MeoVim;
class Document : public directory_items {
    friend class MeoVim;
    list<char> content;
    list<char>::iterator cursor;

    list<char>::iterator selectionBegin;
    list<char>::iterator selectionEnd;
    bool selBack = false;
    string findSaved = "";
    bool somethingSelected() {
        return selectionBegin != content.end();
    }
public:
    // have '\0' at end of file
    Document() : content() {
        content.push_back('\0');
        cursor = content.begin();

        resetSelection();
    }

    Document(fstream &rdr) : content()
    {
        if (rdr.peek() == '\0')
        {
            content.push_back('\0');
            cursor = content.begin();
            resetSelection();
            return;
        }

        do
        {
            char tmp;
            rdr >> tmp;
            content.push_back(tmp);
        } while (rdr.peek() != '\0');
        content.push_back('\0');
        rdr.ignore();
        cursor = content.begin();
        resetSelection();
    }

    void dump(fstream &wrt)
    {
        for (auto c : content)
        {
            wrt << c;
        }
        wrt << endl;
    }

    void insertAtCursor(char toInsert) {
        content.insert(cursor, toInsert);
    }

    void removeAtCursor() {
        if (cursor == content.begin())
            return;

        cursor = content.erase(--cursor);
        if (*cursor == '\n')
            cursor++;
    }

    void moveCursorToRight() {
        if (*cursor == '\0')
            return;

        cursor++;
        // if (*cursor == '\n')
        //     cursor++;
    }

    void moveCursorToLeft()
    {
        if (cursor == content.begin())
            return;

        cursor--;
        // if (*cursor == '\n')
        //     cursor--;
    }

    void moveCursorDown() {
        auto curPos = cursor;
        int count = 0;
        while (*(--curPos) != '\n' && curPos != content.begin())
            count++;
        if (curPos == content.begin())
            count++;

        while (*(++cursor) != '\n' && *(cursor) != '\0');
        count++;

        while (count-- > 0 && *(++cursor) != '\0' && *(cursor) != '\n');
    }

    void moveCursorUp() {
        auto curPos = cursor;
        int count = 0;
        while (*(--curPos) != '\n' && curPos != content.begin())
            count++;
        if (curPos == content.begin())
            return;

        while (*(--cursor) != '\n' && cursor != content.begin());
        cursor--;
        while (*(--cursor) != '\n' && cursor != content.begin());
        if (*(cursor) == '\n')
            count++;

        while (count-- > 0 && *(++cursor) != '\0' && *(cursor) != '\n');
    }

    void resetSelection() {
        selectionBegin = content.end();
        selectionEnd = content.end();
        selBack = false;
    }

    void beginSelection() {
        selectionBegin = cursor;
        selectionEnd = cursor;
        selBack = false;
    }

    void moveSelectionToRight() {
        if (selBack) {
            selectionBegin++;
            if (selectionBegin == selectionEnd) selBack = false;
            return;
        }

        if (*selectionEnd == '\0')
            return;

        selectionEnd++;
        // if (*cursor == '\n')
        //     cursor++;
    }

    void moveSelectionToLeft() {
        if (selBack) {
            if (selectionBegin == content.begin()) return;
            selectionBegin--;
            return;
        }
        
        if (selectionBegin == selectionEnd) {
            selBack = true;
            selectionBegin--;
        } else {
            selectionEnd--;
        }

        // if (*cursor == '\n')
        //     cursor--;
    }

    void selectionToUpper() {
        if (!somethingSelected()) return;

        transform(selectionBegin, selectionEnd, selectionBegin, ::toupper);
    }

    void selectionToLower() {
        if (!somethingSelected()) return;

        transform(selectionBegin, selectionEnd, selectionBegin, ::tolower);
    }

    bool find(string substr) {
        if (substr.empty()) return false;
        findSaved = substr;

        auto it = search(content.begin(), content.end(), substr.begin(), substr.end());

        if (it == content.end()) return false;

        selectionBegin = it;
        selectionEnd = it;
        for (size_t i = 0; i < substr.size(); i++, selectionEnd++);
        return true;
    }

    bool findNext() {
        if (findSaved.empty() || !somethingSelected()) return false;

        auto it = search(selectionEnd, content.end(), findSaved.begin(), findSaved.end());

        if (it == content.end()) return false;

        selectionBegin = it;
        selectionEnd = it;
        for (size_t i = 0; i < findSaved.size(); i++, selectionEnd++);
        return true;
    }

    bool findPrev() {
        if (findSaved.empty() || !somethingSelected()) return false;

        auto it = find_end(content.begin(), selectionBegin, findSaved.begin(), findSaved.end());
        if (it == content.end()) return false;

        selectionBegin = it;
        selectionEnd = it;
        for (size_t i = 0; i < findSaved.size(); i++, selectionEnd++);
        return true;
    }

    bool findWord(string str) {
        return find(" " + str + " ");
    }

    bool findSentence(string str) {
        return find(" " + str + ". ");
    }

    void replace(string str) {
        if (!somethingSelected()) return;
        find(str);
        findSaved = "";

        auto it = content.erase(selectionBegin, selectionEnd);
        content.insert(it, str.begin(), str.end());
        resetSelection();
    }

    void insertPrefix(string prefix) {
        if (!somethingSelected()) return;

        content.insert(selectionBegin, prefix.begin(), prefix.end());

        resetSelection();
    }

    void insertPostFix(string postfix) {
        if (!somethingSelected()) return;

        auto it = selectionEnd++;
        content.insert(it, postfix.begin(), postfix.end());

        resetSelection();
    }

    int spCount() {
        int count = 0;
        for (auto c: content) {
            if (!isalnum(c) && c != '\n' && c != '\0') count++;
        }

        return count;
    }

    int paraCount() {
        int count = 0;
        auto it = content.begin();
        string ss = "\n\n";
        do {
            it = search(it, content.end(), ss.begin(), ss.end());
            if (it == content.end()) break;
            count++; it++++;
        }while(true);

        return count;
    }
};