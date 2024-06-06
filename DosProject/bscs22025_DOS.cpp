// # pragma
#include "bscs22025_directory.cpp"
#include "bscs22009_vim.cpp"
using namespace std;

struct comparator
{
    bool operator()(pair<string, long int> x1, pair<string, long int> x2)
    {
        if (x1.second < x2.second)
            return true;
        return false;
    }
};

class DOS
{
    bool UseCustomMode;
    directory whole_dir_system;
    directory *current_dir;
    directory *root_dir;           // this is going to be the partition V:\>
    string current_directory_path; //= "V:\\";
    priority_queue<pair<string, long int>, vector<pair<string, long int>>, comparator> print_queue;
    int files_printed;

    string version = "Xeon DOS [version 2.3.7]";
    string custom_dir = " ";

    MeoVim mvim;

    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // +++++++++++ (Utility functions of DOS class)  +++++++++++
    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    void update_current_directory_path(string &current_dir_path, string name_of_new_dir, bool going_forward = false) // going forward decides we are going back or forward
    {
        if (going_forward && name_of_new_dir != "..")
        {
            current_dir_path += "\\" + name_of_new_dir;
        }
        else
        {
            // check for not deleting the root directory path
            if (current_dir_path.size() > 3)
            {
                size_t index = current_dir_path.size() - 1;
                while (current_dir_path[index] != '\\')
                {
                    current_dir_path.pop_back();
                    index--;
                }
                current_dir_path.pop_back(); // also popping backslash
            }
        }
    }

    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    //++++++++++++++++++++  (Command Functions) ++++++++++++++++++++++++++++++++++
    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    void HELP_command()
    {
        cout << "HELP		Provides Help information for all these commands.\n"
                "DIR		Displays a list of files and subdirectories in a directory.\n"
                "COPY	    Copies one file in the current directory to another location (directory).\n"
                "CREATE  	Creates a file with the name provided and allows the user to enter contents of the file.\n"
                "CD.		Prints working directory (i.e. your current directory in your tree structure).\n"
                "CD..		Change directory to previous directory.\n"
                "CD\\		Changes directory to root directory (i.e. V:\\>)\n"
                "DEL		Delete a file whose name is provided in input.\n"
                "EXIT		Quits the program\n"
                "CONVERT	  Asks two types and converts extension of all files of one type to another type.\n"
                "EDIT	    Opens a file (loads into a linked list) and allows the user to edit and save the contents of the file. See bullet 5 below\n"
                "FIND       Searches for a file in your current virtual directory whose name is provided as input.\n"
                "FINDF	    Searches for a text string in the currently open file or the file whose name is provided as input.\n"
                "FINDSTR    Searches for strings in all files of your current virtual directory, prints names of files with the string\n"
                "FIND_DIR   Seraches for a directory with the provided name and returns its name if preet"
                "FORMAT	    Formats the current virtual directory i.e. empties the current directory and all subdirectories.\n"
                "LOADTREE	Load a given tree in your tree structure. Given tree is in a file named tree.txt\n"
                "MD (name)  makes directory with id 'name'\n"
                "MOVE	    Moves one file (whose name is provided as input) from one directory to another directory.\n"
                "PPRINT	    Adds a text file to the priority based print queue, and displays the current priority queue.\n"
                "PROMPT	    Changes the Windows command prompt (for example from V:\\> to V$).\n"
                "PRINT		Adds a text file from the current directory to the print queue, and displays the current queue. \n"
                "PQUEUE	    Shows current state of the priority based print queue, with time left for each element\n"
                "PWD		Prints working directory\n"
                "QUEUE		Shows current state of the print queue, with time left for each element\n"
                "RENAME	    Renames a file whose current and new name is provided as input.\n"
                "RMDIR	    Removes a directory along with its contents. Consider setting the respective sibling links\n"
                "SAVE		Saves the currently open file to disk.\n"
                "TREE		Displays the complete directory structure.\n"
                "VER		Displays the version of your program.\n\n";
    }

    // runs dir command functionality
    void DIR_command()
    {
        if (current_dir->get_dir_list().size() > 1)
        {
            // printing all files and directories names in current directory
            for (auto i = (current_dir->get_dir_list()).begin(); i != (current_dir->get_dir_list()).end(); i++)
            {
                if (i->first == "..")
                {
                    continue;
                }
                directory *dr{dynamic_cast<directory *>(i->second)};
                if (dr != nullptr) // if dynamic casting was a success then its an other directory
                {
                    cout << "  <DIR>   " << dr->get_dir_name() << endl; // Remember: directories have no defined extensions
                }
                else
                {
                    cout << "  <TXT>   " << i->first << endl; //  files have extensions already stored in their names
                }
            }
        }
        else
        {
            cout << "Current directory is empty\n\n";
        }
    }

    // go into a child directory and it becomes our current directory
    void cd_command(const string &name_of_next_dir)
    {
        auto next_dir = current_dir->get_dir_list().find(name_of_next_dir);

        if (next_dir != current_dir->get_dir_list().end())
        {
            directory *dir(dynamic_cast<directory *>(next_dir->second));
            if (dir != nullptr)
            {
                current_dir = dir;
                update_current_directory_path(this->current_directory_path, name_of_next_dir, true);
                return;
            }
        }
        cout << "Required Directory not found..." << endl;
    }

    // cd. command  (prints current diretory path)
    void cd_dot_command()
    {
        cout << "Current Directory path:  " << current_directory_path << endl
             << endl;
    }

    // cd.. command (goes to previous directory)
    void cd_DoubleDot_command()
    {
        if (this->current_dir->get_dir_list()[".."] != nullptr)
        {
            directory *dr(dynamic_cast<directory *>(this->current_dir->get_dir_list()[".."])); // ".." key contains the base pointer to previous directory
            this->current_dir = dr;
            update_current_directory_path(this->current_directory_path, " ", false); // this call is set to go backward to previous directory
        }
    }

    // moves the current directory to the root directory
    void cd_slash_command()
    {
        this->current_dir = this->root_dir;
        current_directory_path = "V:\\";
    }

    //++++++++++++++++++ (helper functions) ++++++++++++++++++++

    vector<string> from_string_to_segments(string s, char splitchar)
    {
        stringstream test(s);
        string segment;
        vector<string> seglist;
        while (getline(test, segment, splitchar))
        {
            if (segment != "")
            {
                seglist.push_back(segment);
            }
        }
        return seglist;
    }

    // makes a empty txt file in current dir
    void mkfile_command(string file_name)
    {
        Document *new_doc = new Document();
        directory_items *base_ptr = new_doc;
        current_dir->get_dir_list().insert(make_pair(file_name, base_ptr));
    }

    // this function gets a path and uses it to get to the required directory
    directory *fetch_directory_using_path(string required_dir_path)
    {
        vector<string> path_in_segments = from_string_to_segments(required_dir_path, '\\');
        directory *dir_looking_for = this->root_dir;
        int index = 0;
        for (auto i = path_in_segments.begin(); index < path_in_segments.size(); i++, index++) // last index string will ba the file name so we will not read it
        {
            if (*i == "v:" || *i == "V:")
            {
                continue;
            }
            auto next_dr = dir_looking_for->get_dir_list().find(*i);
            // checking even if the directory exists or not
            if (next_dr != dir_looking_for->get_dir_list().end())
            {
                directory *d(dynamic_cast<directory *>(next_dr->second));
                // checking wheather the provided name is a file or not
                if (d != nullptr)
                {
                    dir_looking_for = d; // going into the found directory
                }
                else //
                {
                    cout << "Wrong path provided...." << endl
                         << endl;
                    return nullptr;
                }
            }
            else
            {
                cout << "Wrong path provided...." << endl
                     << endl;
                return nullptr;
            }
        }
        return dir_looking_for;
    }

    //+++++++++++++     end of helper functions    +++++++++++++

    // copies a file from current directory to another directory
    void copy_command(string which_file, string where)
    {
        auto place_of_file = this->current_dir->get_dir_list().find(which_file);
        if (place_of_file != this->current_dir->get_dir_list().end())
        {

            Document *doc_to_copy(dynamic_cast<Document *>(place_of_file->second));
            if (doc_to_copy != nullptr)
            {
                directory *dr = fetch_directory_using_path(where);
                Document *deep = new Document(*doc_to_copy);
                dr->get_dir_list().insert(make_pair(which_file, deep));
                return;
            }
        }
        cout << "Required file not found in current directory.....\n\n";
    }
    // Move functions

    void move_command(string where, string which_file)
    {
        auto place_of_file = this->current_dir->get_dir_list().find(which_file);
        if (place_of_file != this->current_dir->get_dir_list().end())
        {

            Document *doc_to_copy(dynamic_cast<Document *>(place_of_file->second));
            if (doc_to_copy != nullptr)
            {
                directory *dr = fetch_directory_using_path(where);
                dr->get_dir_list().insert(make_pair(which_file, place_of_file->second));
                place_of_file->second = nullptr;
                this->current_dir->get_dir_list().erase(place_of_file->first);
                return;
            }
        }
        cout << "Required file not found in current directory.....\n\n";
    }

    // allows you to change the extension of txt files only (directories are prohibited)
    void CONVERT_command(string file_looking_for)
    {
        auto it = this->current_dir->get_dir_list().find(file_looking_for);
        if (it != this->current_dir->get_dir_list().end())
        {
            directory *d1(dynamic_cast<directory *>(it->second));
            if (d1 == nullptr) // if the file_looking_for is not a directory
            {
                string new_ext;
                cout << "what would be the new extension: ";
                cin >> new_ext;
                string name_of_file = it->first;

                // deleting the extension of the file
                size_t temp_index = name_of_file.size() - 1;
                while (name_of_file[temp_index] != '.')
                {
                    temp_index--;
                    name_of_file.pop_back();
                }
                name_of_file += new_ext;                                                   // new extension added
                directory_items *new_dr = it->second;                                      // transferring the pointer
                this->current_dir->get_dir_list().insert(make_pair(name_of_file, new_dr)); // inserting same file with updated key
                this->current_dir->get_dir_list().erase(it->first);                        // deleting that file with old key
                return;
            }

            // Remember : Txt files dont have name in them for the time being there only names are the keys
        }
        cout << "Required text file not found...\n";
    }
    // this command is for customization
    void PROMPT_Command()
    {
        if (this->UseCustomMode == false)
        {
            cout << "What would be the name of your custom directory: ";
            cin >> custom_dir;
            custom_dir += "'s directory:\\";
            this->UseCustomMode = true;
        }
        else
        {
            this->UseCustomMode = false;
        }
    }

    // deletes the file whose name is provided as input (only text files are deleted)
    void delete_command(string file_to_delete)
    {
        auto it = this->current_dir->get_dir_list().find(file_to_delete);
        if (it != this->current_dir->get_dir_list().end())
        {
            if (it != this->current_dir->get_dir_list().end())
            {
                Document *d1(dynamic_cast<Document *>(it->second));
                if (d1 != nullptr) // if the file_looking_for is not a directory
                {
                    mvim.closeFile(d1); // remove file from opened buffer
                    delete[] d1;
                    this->current_dir->get_dir_list().erase(it->first); // deleting the file
                    return;
                }
            }
        }
        cout << "Text file you want to delete not found..." << endl;
    }

    // Opens a file (loads into a linked list) and allows the user to edit and save the contents of the file.
    void EDIT_command()
    {
    }

    //  exists from the compiler
    void EXIT_command()
    {
        //++++++++++++++++   (This command should also save before exiting)   +++++++++++++++++++

        // we can do exit in the function nothing to write here
    }

    // just checks wheather the file exists in the current directory
    void Find_command(string file_to_find)
    {
        auto file_to_find_iterator = this->current_dir->get_dir_list().find(file_to_find);
        // if file is present

        // if the file found is not a directory
        if (file_to_find_iterator != this->current_dir->get_dir_list().end())
        {
            directory *dir(dynamic_cast<directory *>(file_to_find_iterator->second));
            if (dir == nullptr) // if its not a directory then its a text file
            {
                cout << "<txt>    " << file_to_find << "    found in <DIR>   " << this->current_dir->get_dir_name() << endl;
                return;
            }
        }
        cout << "File not found in the current directory..." << endl;
    }

    // it is used to find a child directory in the current directory
    void Find_dir_command(string dir_to_find)
    {
        auto file_to_find_iterator = this->current_dir->get_dir_list().find(dir_to_find);
        // if file is present
        if (file_to_find_iterator != this->current_dir->get_dir_list().end())
        {
            directory *dir(dynamic_cast<directory *>(file_to_find_iterator->second));
            if (dir != nullptr)
            {
                cout << "<DIR>    " << dir_to_find << "    found in <DIR>   " << this->current_dir->get_dir_name() << endl;
                return;
            }
        }
        cout << "File not found in the current directory..." << endl;
    }

    void FINDF_command()
    {
    }

    void FIND_STR_command()
    {
    }

    // Formats the current virtual directory i.e. empties the current directory and all subdirectories.
    // this is going to be recursive function
    void FORMAT_Command(directory *&dir) // we will pass the root directoy here
    {
        while (!dir->get_dir_list().size() != 1) // beacuse the pointer of previous direcory will alo be there
        {
            directory *next_dir(dynamic_cast<directory *>(dir->get_dir_list().begin()->second)); // checking the 1st element of the directory list
            if ((dir->get_dir_list().begin()->first) == "..")
            {
                dir->get_dir_list().begin()->second == nullptr;
            }
            else if (next_dir != nullptr) // if its not nullptr means its a directory
            {
                // then make recursive call on it
                FORMAT_Command(next_dir);
                delete[] next_dir; // using dynamic casted pointer for deletion
            }
            else // if its not a directory its a document
            {
                Document *doc(dynamic_cast<Document *>(dir->get_dir_list().begin()->second));
                delete[] doc;
            }
            dir->get_dir_list().erase(dir->get_dir_list().begin()); // erasing the 1st element wheather it was a file or dir
        }
    }

    // Creates a virtual directory.
    void MD_command(string new_dir_name) // make directiry command
    {

        if (!new_dir_name.empty())
        {

            auto fx = find(new_dir_name.begin(), new_dir_name.end(), '.');
            if (fx == new_dir_name.end())
            {
                if (!(new_dir_name == "mkdir" || new_dir_name == "MKDIR" || new_dir_name == "md" || new_dir_name == "MD" || new_dir_name == ".." || new_dir_name == "."))
                {
                    directory *new_dir = new directory(new_dir_name, this->current_dir);
                    directory_items *base_ptr = new_dir;
                    current_dir->get_dir_list().insert(make_pair(new_dir_name, base_ptr));
                    return;
                }
            }
        }
        cout << "Invalid directory name..." << endl
             << endl;
    }

    // Removes a directory along with its contents. Consider setting the respective sibling links
    void
    RMDIR_command(directory *&dir, bool first_time)
    {
        if (current_dir != root_dir)
        {
            string dr_to_delete;
            directory *previous_dir(dynamic_cast<directory *>(dir->get_dir_list().at("..")));

            if (first_time)
            {
                dr_to_delete = dir->get_dir_name();
            }
            while (!dir->get_dir_list().size() != 1) // beacuse the pointer of previous direcory will alo be there
            {
                directory *next_dir(dynamic_cast<directory *>(dir->get_dir_list().begin()->second)); // checking the 1st element of the directory list
                if ((dir->get_dir_list().begin()->first) == "..")
                {
                    dir->get_dir_list().begin()->second == nullptr;
                }
                else if (next_dir != nullptr) // if its not nullptr means its a directory
                {
                    // then make recursive call on it
                    RMDIR_command(next_dir, false);
                    delete[] next_dir; // using dynamic casted pointer for deletion
                }
                else // if its not a directory its a document
                {
                    Document *doc(dynamic_cast<Document *>(dir->get_dir_list().begin()->second));
                    delete[] doc;
                }
                dir->get_dir_list().erase(dir->get_dir_list().begin()); // erasing the 1st element wheather it was a file or dir
            }
            if (first_time)
            {
                this->current_dir = previous_dir;
                current_dir->get_dir_list().erase(dr_to_delete);
                update_current_directory_path(this->current_directory_path, " ", false); // this call is set to go backward to previous directory
            }
        }
        else
        {
            cout << "You are at root directory cannot remove it...." << endl
                 << endl;
        }
    }

    // shows the version of the software
    void VER_command()
    {
        cout << version << endl
             << endl;
    }

    void print_queue_printer(bool mention_priority = true)
    {
        if (!this->print_queue.empty())
        {
            // 1st we will delete the files whose time stamps expired
            while (abs(time(0) - (this->print_queue.top()).second) > 300)
            {
                this->files_printed++; // keeping track of how many files got printed
                this->print_queue.pop();
                this->files_printed++;
            }
            if (!this->print_queue.empty())
            {
                priority_queue<pair<string, long int>, vector<pair<string, long int>>, comparator> for_printing_only = this->print_queue;
                int count = 1;
                while (!for_printing_only.empty())
                {
                    if (mention_priority)
                    {
                        cout << "Priority: " << count;
                        count++;
                    }
                    cout << "    " << for_printing_only.top().first << endl
                         << endl;
                    for_printing_only.pop();
                }
            }
            else
            {
                cout << "Print Priority Queue is empty..." << endl
                     << endl;
            }
            cout << "Number of files that got printed:  " << this->files_printed << endl;
        }
    }

    // Adds a text file from the current directory to the print queue, and displays the current queue.
    void PRINT_command(const string &file)
    {
        auto x = this->current_dir->get_dir_list().find(file);
        if (x != this->current_dir->get_dir_list().end())
        {
            Document *doc(dynamic_cast<Document *>(x->second));
            if (doc != nullptr)
            {
                print_queue.push(make_pair(x->first, time(0))); // pushing with current time
                print_queue_printer();
                return;
            }
        }
        cout << " Required file not found..." << endl
             << endl;
    }

    // prints print priority queue without telling priority
    void PQUEUE()
    {
        print_queue_printer(false);
    }

    // prints print priority queue with telling priority
    void QUEUE()
    {
        print_queue_printer();
    }

    void PWD()
    {
        cout << "Working directory: "
             << "   <DIR>   " << this->current_dir->get_dir_name();
    }

    void Tree_command(directory *dir, int spaces)
    {

        auto start = dir->get_dir_list().begin();
        auto end = dir->get_dir_list().end();
        for (auto i = start; i != end; i++)
        {
            if (i->first == "..")
            {
                continue;
            }
            else
            {
                directory *dir(dynamic_cast<directory *>(i->second));
                if (dir != nullptr)
                {
                    for (int i = 1; i <= spaces; i++)
                    {
                        cout << " - ";
                    }
                    cout << " <DIR>   " << i->first << endl;
                    Tree_command(dir, spaces + 1);
                }
                else
                {
                    Document *doc(dynamic_cast<Document *>(i->second));
                    if (doc != nullptr)
                    {
                        for (int i = 1; i <= spaces; i++)
                        {
                            cout << " - ";
                        }
                        cout << " <TXT>   " << i->first << endl;
                    }
                }
            }
        }
    }

    void vim_command(string file)
    {
        if (file == "")
        {
            mvim.run();
            return;
        }
        else
        {
            string dot = ".";
            auto it = find_end(file.begin(), file.end(), dot.begin(), dot.end());

            if (it == file.end())
            {
                cout << "What...?\n\n";
                return;
            }
        }

        auto place_of_file = this->current_dir->get_dir_list().find(file);
        if (place_of_file != this->current_dir->get_dir_list().end())
        {
            Document *documentptr(dynamic_cast<Document *>(place_of_file->second));
            if (documentptr != nullptr)
            {
                mvim.openFile(documentptr);
                mvim.run();
                return;
            }
        }
        else
        {
            mkfile_command(file);
            vim_command(file);
        }
    }

    void save_tree_to_file(directory *cur_dir, fstream &wrt, string dir_path_to_save, bool is_first_time)
    {
        update_current_directory_path(dir_path_to_save, cur_dir->get_dir_name(), is_first_time);
        // 1st print all the directories and files in the current directory
        for (auto i = cur_dir->get_dir_list().begin(); i != cur_dir->get_dir_list().end(); i++)
        {
            if (i->first == "..") // skip the previous file we can pass it as a parameter ourself
            {
                continue;
            }
            Document *doc(dynamic_cast<Document *>(i->second));
            if (doc != nullptr)
            {
                wrt << dir_path_to_save << "\\" << i->first << endl;
                doc->dump(wrt);
            }
            else
            {
                directory *dir(dynamic_cast<directory *>(i->second));
                if (dir != nullptr)
                {
                    wrt << dir_path_to_save << "\\" << i->first << endl;
                }
            }
        }
        // now going to the next directry to save its components;
        for (auto i = cur_dir->get_dir_list().begin(); i != cur_dir->get_dir_list().end(); i++)
        {
            if (i->first == "..") // skip the previous file we can pass it as a parameter ourself
            {
                continue;
            }
            directory *dir(dynamic_cast<directory *>(i->second));
            if (dir != nullptr)
            {
                save_tree_to_file(dir, wrt, dir_path_to_save, true);
            }
        }
    }

    void load_tree()
    {
        fstream rdr("DOSsave.txt");
        if (!rdr.is_open())
        {
            cout << "File didnt open";
        }
        string str;
        while (rdr.peek() != EOF)
        {
            // rdr.seekg(0, std::ios::end); // Move to the end of the file
            // std::streampos fileSize = rdr.tellg(); // Get the current position of rdr
            // if (fileSize == -1) // rdr is at end of file
            //{
            //    break;
            //}
            getline(rdr, str);
            vector<string> complete_path = from_string_to_segments(str, '\\');
            // going into the directories as far we can go
            int index = 1; // we are starting from 1 because we are already at V: drive

            for (; index < complete_path.size() - 1; index++)
            {
                auto d = this->current_dir->get_dir_list().find(complete_path[index]);
                if (d != this->current_dir->get_dir_list().end())
                {
                    Document *doc(dynamic_cast<Document *>(d->second));
                    if (doc == nullptr) // if its not a txt file
                    {
                        directory *dr(dynamic_cast<directory *>(d->second));
                        if (dr != nullptr)
                        {
                            this->current_dir = dr; // going into next available directory according to the path provided
                        }
                        else
                        {
                            throw runtime_error("Exception of load tree function triggered. Unknown behaviour. There has to be directory but no directory found");
                        }
                    }
                }
            }

            // now adding the last directory into our directory system
            for (auto i : complete_path[index])
            {
                if (i == '.')
                {
                    Document *dok = new Document(rdr); // saturating the document with the provided text
                    directory_items *di = dok;
                    this->current_dir->get_dir_list().insert(make_pair(complete_path[index], di));
                    continue;
                }
            }

            directory *dr = new directory(complete_path[index], this->current_dir);
            directory_items *di = dr;
            this->current_dir->get_dir_list().insert(make_pair(complete_path[index], di));
            this->current_dir = this->root_dir;
        }

        rdr.close();
    }

public:
    // default constructor
    DOS() : whole_dir_system(), mvim()
    {
        this->current_dir = &whole_dir_system;
        this->root_dir = &whole_dir_system;
        this->current_directory_path = "V:\\";
        this->UseCustomMode = false;
        this->files_printed = 0;
        cout << "Do You want to load the previous saved directory (y/n):   ";
        char ans = ' ';
        cin >> ans;
        while (!(ans == 'y' || ans == 'Y' || ans == 'N' || ans == 'n'))
        {
            cout << "Please give answer in Y (yes) or N (no)";
            cin >> ans;
        }
        system("cls");
        if (ans == 'y' || ans == 'Y')
        {
            load_tree();
        }
    }

    string get_current_dir_path()
    {
        return current_directory_path;
    }
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    //+++++++++++++++++++++ (main execution function thats gonna run in main) +++++++++++++++++++++
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    // gets the full name of the file or directory with spaces included
    void getNameOfFile(stringstream &input, string &file_name)
    {
        input >> file_name;
        string fx;
        input >> fx;
        while (!fx.empty())
        {
            file_name.push_back(' ');
            file_name += fx;
            input >> fx;
        }
    }

    bool command_executor(string whole_command)
    {
        fstream wrt("DOSsave.txt");
        stringstream command_breakdown(whole_command); // using string stream to separate the whole command
        // into space separated words
        int index = 0;
        string command;
        command_breakdown >> command; // 1st taking only the DOS command eg, MD, CLEAR etc
        if (command == "HELP" || command == "help")
        {
            this->HELP_command(); // calling a friends class protected function
        }
        else if (command == "dir" || command == "DIR")
        {
            DIR_command();
        }
        else if (command == "cd" || command == "CD")
        {
            getNameOfFile(command_breakdown, command);
            cd_command(command);
        }
        else if (command == "cd." || command == "CD.")
        {
            cd_dot_command();
        }
        else if (command == "cd.." || command == "CD..")
        {
            cd_DoubleDot_command();
        }
        else if (command == "cd\\" || command == "CD\\")
        {
            cd_slash_command();
        }
        else if (command == "exit" || command == "EXIT")
        {
            wrt.close();
            return true;
        }
        else if (command == "find" || command == "FIND")
        {
            getNameOfFile(command_breakdown, command);
            Find_command(command); // here the command is the name of file to find
        }
        else if (command == "find_dir" || command == "FIND_DIR")
        {
            getNameOfFile(command_breakdown, command);
            Find_dir_command(command);
        }
        else if (command == "md" || command == "MD" || command == "mkdir" || command == "MKDIR")
        {
            // here we are storing the name of the directory about to be created
            // the number of names separeted by space is equal to number of directories that will be made (just like in cmd)
            command_breakdown >> command;
            string previous_name;
            do
            {
                previous_name = command;
                MD_command(command);
                command_breakdown >> command;
            } while (previous_name != command);
        }
        else if (command == "MKFILE" || command == "mkfile")
        {
            command_breakdown >> command;
            string previous_name;
            do
            {
                previous_name = command;
                mkfile_command(command + ".txt");
                command_breakdown >> command;
            } while (previous_name != command);
        }
        else if (command == "COPY" || command == "copy")
        {
            string where;
            command_breakdown >> command; // which file
            command_breakdown >> where;   // where to copy
            copy_command(command, where);
        }
        else if (command == "MOVE" || command == "move")
        {
            string where;
            command_breakdown >> command; // which file
            command_breakdown >> where;   // where to copy
            move_command(where, command);
        }
        else if (command == "FORMAT" || command == "format")
        {
            FORMAT_Command(root_dir);
            this->current_dir = root_dir;
            current_directory_path = "V:\\";
        }
        else if (command == "RMDIR" || command == "rmdir")
        {
            RMDIR_command(current_dir, true);
        }
        else if (command == "VER" || command == "ver")
        {
            VER_command();
        }
        else if (command == "prompt" || command == "PROMPT")
        {
            PROMPT_Command();
        }
        else if (command == "DELETE" || command == "delete")
        {
            getNameOfFile(command_breakdown, command);
            delete_command(command);
        }
        else if (command == "CONVERT" || command == "convert")
        {
            getNameOfFile(command_breakdown, command);
            CONVERT_command(command);
        }
        else if (command == "pwd" || command == "PWD")
        {
            PWD();
        }
        else if (command == "print" || command == "PRINT" || command == "pprint" || command == "PPRINT")
        {
            getNameOfFile(command_breakdown, command);
            PRINT_command(command);
        }
        else if (command == "queue" || command == "QUEUE")
        {
            QUEUE();
        }
        else if (command == "pqueue" || command == "PQUEUE")
        {
            PQUEUE();
        }
        else if (command == "tree" || command == "TREE")
        {
            Tree_command(this->root_dir, 1);
        }
        else if (command == "vim" || command == "vi")
        {
            string fileName;
            command_breakdown >> fileName;
            vim_command(fileName);
        }
        else if (command == "")
            ; // dont error on no command
        else
        {
            cout << "Command not Found....\n\n";
        }
        string pth = this->root_dir->get_dir_name();
        save_tree_to_file(this->root_dir, wrt, pth, false);
        wrt.close();
        return false;
    }

    void PrintLiveCommandONConsole(string s, HANDLE hConsole, COORD cursorPos)
    {
        if (s.size() >= 0)
        {
            SetConsoleCursorPosition(hConsole, cursorPos);
            cout << "                                                                                                                             ";
            SetConsoleCursorPosition(hConsole, cursorPos);
            cout << s;
        }
    }

    COORD GetConsoleCursorPosition(HANDLE hConsoleOutput)
    {
        CONSOLE_SCREEN_BUFFER_INFO cbsi;
        if (GetConsoleScreenBufferInfo(hConsoleOutput, &cbsi))
        {
            return cbsi.dwCursorPosition;
        }
        else
        {
            // The function failed. Call GetLastError() for details.
            COORD invalid = {0, 0};
            return invalid;
        }
    }

    string getCommand()
    {
        string command;
        char x = ' ';
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        COORD cursorPos;
        cursorPos = GetConsoleCursorPosition(hConsole);
        while (true) //  \r  =  enter key
        {
            x = _getch();
            if (x == -32) // if arrow keys are pressed then just ignore them
            {
                x = _getch();
                continue;
            }
            if (x == '\r')
            {
                cout << '\n';
                break;
            }
            else if (x == 8) // backspace pressed
            {
                if (command.size() > 0)
                {
                    command.pop_back();
                }
            }
            else
            {
                command.push_back(x);
            }
            PrintLiveCommandONConsole(command, hConsole, cursorPos);
        }
        return command;
    }

    void start_whole_DOS(DOS &dos_obj)
    {
        string whole_command;
        cout << endl;
        cout << version << endl
             << endl;
        cout << "DOS and VIM made by: Muhammad Ahmad (Bscs22025) and Mubashir Haroon (Bscs22009) \n"
                "Registration ID: Alpha Romia_6_1\n"
                "Course: Data Structures and Algorithms\n\n";
        while (true)
        {
            if (this->UseCustomMode == false)
            {
                cout << dos_obj.current_directory_path << "> ";
            }
            else
            {
                cout << custom_dir << "> ";
            }
            whole_command = getCommand();
            bool exit = command_executor(whole_command);
            if (exit)
            {
                break; // exit command called : terminate the whole code
            }
        }
    }
};
