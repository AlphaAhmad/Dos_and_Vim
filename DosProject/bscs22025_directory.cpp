#include "utility.h"
using namespace std;

// ++++++++++ (Directory class) +++++++++++

class directory : public directory_items
{
    // name of the cuurent directory
    string current_dir_name;
    // all the files and directories the current directory contains
    map<string, directory_items *> dir_list;

public:
    // default constructor
    directory()
        : current_dir_name("V:\\")
    {
        directory_items *prv = nullptr;        // using base pointer to hold the derived class
        dir_list.insert(make_pair("..", prv)); // inserting the .. as the previous directory
    }
    // parameterd constructor
    directory(string _name, directory *previous_directory)
        : current_dir_name(_name)
    {
        directory_items *prv = previous_directory; // using base pointer to hold the derived class
        dir_list.insert(make_pair("..", prv));     // inserting the .. as the previous directory
    }
    string &get_dir_name()
    {
        return current_dir_name;
    }
    map<string, directory_items *> &get_dir_list()
    {
        return this->dir_list;
    }
};
