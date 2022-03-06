//
//  main.cpp
//  P3
//
//  Created by Joshua Jacobs on 10/3/21.
//
 
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <assert.h>
 
using namespace std;

/*
 ================
 Reflection for when Future Josh looks back
 ================
 
 I actually really enjoyed this project, and I learned a good number of things during this, much of which I didn't actually end up using but now have
 exposure. This week in particular was busy, I had 2 tests, hard hwk, 2 duty shifts, and 2 projects. Obviously I managed this, but my design process
 was... wonky, at best. I didn't end up using a sort function, thus this O(n) will be very slow with large data. For the tests it doesn't matter. I
 didn't end up implementing a sort because keeping the order that the programs were added to the strideScheduler was important, and I was worried about
 failing to order them correctly. How could have I solved this? I was thinking I could have "PIDs" that tracked order they were put in program, and then later
 when I order mattered, I could then just check PIDs, and I'd have to remember to increment them as they were blocked or something.
 
 Additionally, I have no idea why I didnt do this earlier, but I should have just made a StrideScheduler object. I have no idea why the idea didn't come to me
 until I was halfway done, but at that point I didn't want to risk breaking anything, especially with the limited time I had this week. I got really good
 at visualizing pointers and memory management, so that was sort of a plus. There was an interesting bug doing it the way I was doing it, but I actually enjoyed
 figuring it out relatively quickly once I read documentation.
 
 What did I learn with this assignment code-making wise:
 1) Gained more confidence using bool operators
 2) Pointer notation feels intuitive at this point.
 3) Researched a bit more about data types such as string_type and auto. Spent some time looking into different things with it
    I think I want to look into how to make data more scaleable from small to large and be data efficient
 4) Asserts! Wow! Debugging with these bad boys cut so much time, it was a godsend, and I am upset I didn't use them more before
 5) Shorten Notation, learned a few small tricks such as with what I did in Runner with constructor, and I learned about ternary operators later
 
 
 What would I change?
 1) Sort.
    1b) My program is so inefficient if you had a lot of programs running...
 2) StrideScheduler Object. Why I decided that passing everything by reference was the way to go escapes me.
 3) I would've liked to implement point 3 above, but I had some trouble wording it into google. Requires more research on my part.
 4) Shorten program. I definitely feel like I could chop 50+ lines off of this program. But late night/early morning coding, a tight schedule,
    and a questionable design, and a little laziness makes the file a litle bloated.
 
 
 If this github link is unchanged for whatever reason, here were the specs:
 https://github.com/pkivolowitz/summer_os_headstart/tree/master/p3
 */


/*
If I had more time this week I would do:
 TODO: Implement StrideRunner object
 TODO: FindNextRunner();
 TODO: Implement a sort
 */


 
class Runner{
    string name;
    string::size_type priority;
    int stride = 0;
    double pass = 0;
    static int const STRIDE_CALC = 100000;
    
    public:
    Runner() {};
    Runner(string n, string::size_type p) : name(n), priority(p) {stride = STRIDE_CALC/priority;}
    void run() {pass += stride;}
    
    string getName() {return name;}
    string::size_type getPrio() {return priority;}
    int getStride() {return stride;}
    double getPass() {return pass;}
 
 
    void setPass(double pass) {this->pass = pass;}
 
    
    
    bool operator== (const Runner& rhs) const {
        return (name == rhs.name) && (priority == rhs.priority) && (stride == rhs.stride) && (pass == rhs.pass);
    }
};
 
int GetTestFile(string);
void RunStrideScheduler(ifstream&);
int ParseCommandInput(string);
unsigned long FindSecondComma(string);
 
Runner NewJob(string, int);
 
void AssignNewJob(vector<Runner>&, Runner**, string&, bool&);
void FinishJob(vector<Runner>&, Runner**, bool&);
void Interrupt(vector<Runner>&, Runner**, bool);
void Block(vector<Runner>&, vector<Runner>&, Runner**, bool&);
void Unblock(vector<Runner>&, vector<Runner>&, Runner**, string&, bool&);
void PrintRunnable(vector<Runner>, Runner*);
void PrintRunning(Runner*, bool);
void PrintBlocked(vector<Runner>&);
 
int main(int argc, const char * argv[]) {
    ifstream testFile;
    
    string command;
 
    if(argv[1] != NULL) {
        testFile.open(argv[1]);
        if(testFile.ios_base::fail()) {
            cout << "Error. File Opening Problem.";
            return -1;
        }
        RunStrideScheduler(testFile);
        testFile.close();
    } else {
        cout << "Error. No given file path." << endl;
        return -1;
    }
    
    return 0;
}
 
 
void RunStrideScheduler(ifstream& testFile) {
    vector<Runner> strideRunner;
    vector<Runner> blockedList;
    Runner *currentRunner = nullptr;
    string command;
    bool systemIdle = 1;
    
    while(getline(testFile, command)) {
        switch (ParseCommandInput(command)) {
            case 0: {
                AssignNewJob(strideRunner, &currentRunner, command, systemIdle);
                break;
            }
            case 1:
                FinishJob(strideRunner, &currentRunner, systemIdle);
                break;
            case 2:
                Interrupt(strideRunner, &currentRunner, systemIdle);
                break;
            case 3:
                Block(blockedList, strideRunner, &currentRunner, systemIdle);
                break;
            case 4:
                Unblock(blockedList, strideRunner, &currentRunner, command, systemIdle);
                break;
            case 5:
                PrintRunnable(strideRunner, currentRunner);
                break;
            case 6:
                PrintRunning(currentRunner, systemIdle);
                break;
            case 7:
                PrintBlocked(blockedList);
                break;
            case -1:
                //This is for those empty lines at the end of files...
                //cout << "Line did not contain stride scheduler command." << endl;
                break;
            default:
                break;
        }
    }
}
 
void AssignNewJob(vector<Runner> &strideRunner, Runner **currentRunner, string &command, bool& idle) {
    string progN = "";
    string::size_type progP;
 
    //Find the Name
    int j = 0;
    for (int i = 7; i < FindSecondComma(command); i++) {
        progN += command[i];
        j++;
    }
    
    //Must not have grabbed the comma
    assert(progN.find(",") == string::npos);
    
    //Erase name and comma from command
    command.erase(command.begin(), command.begin()+FindSecondComma(command)+1);
    
    progP = stoi(command);
    Runner runner(progN, progP);
    
    cout << "New job: " << progN << " added with priority: " << progP << endl;
    
    if (idle) {
        cout << "Job: " << progN << " scheduled." << endl;
        
        strideRunner.push_back(runner);
        *currentRunner = &strideRunner.at(0);
        idle = 0;
    } else {
        //Here is a cool unexpected bug I had to deal with. Apparently I can't just set *currentRunner = &strideRunner.at(i)
        //whenever I feel like it, especially when I am ADDING a program to the vector of the strideScheduler.
        //This is because push_back() often reallocates when it needs to resize. At first I thought that as long as I had the pointer to the object
        //saved in current runner, then later that wouldn't be an issue. Well since it was pointing to somewhere in a vector it was freed.
        //Interestingly it just meant my *currentRunner was now an empty Runner and not a nullptr
        runner.setPass((**currentRunner).getPass());
        Runner currRunner = (**currentRunner);
        strideRunner.push_back(runner);
        for (int i = 0; i < strideRunner.size(); i++) {
            if (strideRunner.at(i) == currRunner) {
                *currentRunner = &strideRunner.at(i);
                break;
            }
        }
    }
}
 
void FinishJob(vector<Runner>& strideRunner, Runner **currentRunner, bool& idle) {
    //Must be a current runner or the system is idle
    assert(*currentRunner != nullptr || idle);
    
    int lowPass = (**currentRunner).getPass();
    int lowIndex = -1;
    
    if(!idle) {
        size_t sz = strideRunner.size();
        int eraseIndex = -1;
        cout << "Job: " << (**currentRunner).getName() << " completed." << endl;
 
        for (int i = 0; i < sz; i++) {
            if(strideRunner.at(i) == (**currentRunner)) {
                eraseIndex = i;
                break;
            }
        }
        
        if(eraseIndex != -1)
            strideRunner.erase(strideRunner.begin()+eraseIndex);
        assert(sz-1 == strideRunner.size());         //Something must have been removed
 
        *currentRunner = nullptr;
        
        if (strideRunner.size() == 0) {
            idle = 1;
            cout << "System is idle." << endl;
        } else {
            lowPass = strideRunner.at(0).getPass();
            lowIndex = 0;
            //TODO: Make a function call called FindNextRunner() and use this chunk
            for (int i = 0; i < strideRunner.size(); i++) {
                if(lowPass > strideRunner.at(i).getPass()) {
                    lowPass = strideRunner.at(i).getPass();
                    lowIndex = i;
                } else if (lowPass == strideRunner.at(i).getPass()) {
                    //If first instance of the same low pass, or if not compare names
                    if(lowIndex == -1 || (strideRunner.at(lowIndex).getName() > strideRunner.at(i).getName())) {
                        lowIndex = i;
                    }
                }
            }
            (*currentRunner) = &strideRunner.at(lowIndex);
            
        }
    } else if (idle) {
        cout << "Error. System is idle." << endl;
    }
}
 
void Interrupt(vector<Runner>& strideRunner, Runner **currentRunner, bool idle) {
    double lowPass;
    int index = 0;
    
    //TODO: WRITE A SORT
    if (!idle) {
        assert(*currentRunner != nullptr);
        
        for (int j = 0; j < strideRunner.size(); j++) {
            if ((**currentRunner) == strideRunner.at(j)){
                strideRunner.at(j).run();
                break;
            }
        }
        
        lowPass = (**currentRunner).getPass();
        for (int i = 0; i < strideRunner.size(); i++) {
            if (lowPass > strideRunner.at(i).getPass()) {
                lowPass = strideRunner.at(i).getPass();
                index = i;
            }
        }
        *currentRunner = &strideRunner.at(index);
        cout << "Job: " << strideRunner.at(index).getName() << " scheduled." << endl;
    } else
        cout << "Error. System is idle." << endl;
}
 
void Block(vector<Runner>& blocked, vector<Runner>& strideRunner, Runner **currentRunner, bool& idle) {
    
    if (!idle) {
        assert(*currentRunner != nullptr);
        auto sz = strideRunner.size();
        double lowPass = (**currentRunner).getPass();
        int lowIndex = -1; //currRunner without doing search TODO: change once sort implemented
        int eraseIndex = -1;
        
        for (int i = 0; i < sz; i++) {
            if(strideRunner.at(i) == (**currentRunner)) {
                eraseIndex = i;
                break;
            }
        }
        
        if(eraseIndex != -1) {
            //Another flaw with how I have designed my system. erase does not reallocate the vector. Thus *currentJob always points to say .at(1)
            //Thus I have to push it frst to blocked before I can very if it worked. Granted, assert will catch if it fails to erase for some reason
            cout << "Job: " << (**currentRunner).getName() << " blocked." << endl;
            blocked.push_back((**currentRunner));
            strideRunner.erase(strideRunner.begin()+eraseIndex);
            assert(sz == strideRunner.size()+1);    //Must have removed the element from the vector
        }
        
        if(strideRunner.empty()) {
            idle = 1;
            cout << "System is idle." << endl;
        } else {
            lowPass = strideRunner.at(0).getPass();
            lowIndex = 0;
            for (int i = 1; i < strideRunner.size(); i++) {
                if(lowPass > strideRunner.at(i).getPass()) {
                    lowPass = strideRunner.at(i).getPass();
                    lowIndex = i;
                } else if (lowPass == strideRunner.at(i).getPass()) {
                    //If first instance of the same low pass, or if not compare names
                    if(lowIndex == -1 || (strideRunner.at(lowIndex).getName() > strideRunner.at(i).getName())) {
                        lowIndex = i;
                    }
                }
            }
            *currentRunner = &strideRunner.at(lowIndex);
            cout << "Job: " << (**currentRunner).getName() << " scheduled." << endl;
        }
    }
    else {
        cout << "Error. System is idle." << endl;
    }
}
 
void Unblock(vector<Runner>& blocked, vector<Runner>& strideRunner, Runner **currentRunner, string& command, bool& idle) {
    //Grab string that comes after "unblock,"
    string s = command.substr(8, command.size()-8);
    Runner run = Runner("", -1);
    auto bsz = blocked.size();
    int eraseIndex = -1;
    
    for (int i = 0; i < blocked.size(); i++) {
        if (blocked.at(i).getName() == s) {
            run = blocked.at(i);
            eraseIndex = i;
            break;
        }
    }
    
    //If index is found
    if (eraseIndex != -1)
        blocked.erase(blocked.begin()+eraseIndex);
    
    
    //If runner is found go to if, else it was an error
    if(bsz == blocked.size()+1) {
        if (!idle) {
            Runner currRunner = **currentRunner;
            run.setPass(currRunner.getPass());
            cout << "Job: " << run.getName() << " has unblocked. Pass set to: " << run.getPass() << endl;
            strideRunner.push_back(run);
            //For an explanation why this code is needed, see inside AssignNewJob();
            for (int i = 0; i < strideRunner.size(); i++) {
                if (strideRunner.at(i) == currRunner) {
                    *currentRunner = &strideRunner.at(i);
                    break;
                }
            }
        } else {
            idle = 0;
            run.setPass(0);
            cout << "Job: " << run.getName() << " has unblocked. Pass set to: " << run.getPass() << endl;
            cout << "Job: " << run.getName() << " scheduled.";
            //run.run();
            strideRunner.push_back(run);
            *currentRunner = &strideRunner.at(0);
        }
    } else {
        cout << "Error. Job: " << s << " not blocked." << endl;
    }
}
 
int ParseCommandInput(string s) {
    int retVal = -1;
    
    if(s.find("newjob") != string::npos) {
        retVal = 0;
    }
    else if(s.find("finish") != string::npos) {
        retVal = 1;
    }
    
    else if(s.find("interrupt") != string::npos) {
        retVal = 2;
    }
    
    else if(s.find("block") != string::npos && s.find("blocked") == string::npos && s.find("unblock") == string::npos) {
        retVal = 3;
    }
    
    else if(s.find("unblock") != string::npos) {
        retVal = 4;
    }
       
    else if(s.find("runnable") != string::npos) {
        retVal = 5;
    }
    
    else if(s.find("running") != string::npos) {
        retVal = 6;
    }
    
    else if(s.find("blocked") != string::npos) {
        retVal = 7;
    }
    
    return retVal;
}
 
//Prints the programs in the Stride Scheduler QUEUE, thus the non-running one
//If you want to see the running program, must call Running() details
void PrintRunnable(vector<Runner> runnables, Runner* currentRunner) {
    int runningIndex = -1;
    
    if (runnables.empty()) {
        cout <<"Runnable:\nNone" << endl;
    } else {
        assert(currentRunner != nullptr);
        for (int i = 0; i < runnables.size(); i++) {
            if (runnables.at(i) == (*currentRunner)) {
                runningIndex = i;
                break;
            }
        }
        
        string removeN = (*currentRunner).getName();
        runnables.erase(runnables.begin()+runningIndex);
        cout << "Runnable: " << endl;
        cout << "NAME     STRIDE  PASS  PRI" << endl;
    
        for (int i = 0; i < runnables.size(); i++) {
            cout << runnables.at(i).getName();
            
            //Incase not one letter prog name
            for (string::size_type j = runnables.at(i).getName().length(); j < 9; j++) {
                cout << " ";
            }
            
            cout << runnables.at(i).getStride();
            
            for (string::size_type j = to_string(runnables.at(i).getStride()).length(); j < 8; j++) {
                cout << " ";
            }
            
            cout << runnables.at(i).getPass();
            
            if (runnables.at(i).getPass() > 999) {
                cout << "  ";
            } else if (runnables.at(i).getPass() > 99) {
                cout << "   ";
            } else if (runnables.at(i).getPass() > 9) {
                cout << "    ";
            } else if (runnables.at(i).getPass() > 9999) {
                cout << " ";
            }

            cout << runnables.at(i).getPrio() << endl;
        }
        
    }
    
}

void PrintRunning(Runner* runner, bool idle) {
        
        if (!idle)
        {
            assert(runner != nullptr);
            cout << "Running:" << endl;
            cout << "NAME     STRIDE  PASS  PRI" << endl;
 
            cout << (*runner).getName();
            
            //Incase not one letter prog name
            for (string::size_type j = (*runner).getName().length(); j < 9; j++) {
                cout << " ";
            }
            
            cout << (*runner).getStride();
            
            for (string::size_type j = to_string((*runner).getStride()).length(); j < 8; j++) {
                cout << " ";
            }
            
            cout << (*runner).getPass();
            
            if ((*runner).getPass() > 999) {
                cout << "  ";
            } else if ((*runner).getPass() > 99) {
                cout << "   ";
            } else if ((*runner).getPass() > 9) {
                cout << "    ";
            } else if ((*runner).getPass() > 9999) {
                cout << " ";
            }
            
            cout << (*runner).getPrio() << endl;
        } else {
            cout << "Running:\nNone" << endl;
        }
}
 
void PrintBlocked(vector<Runner>& blocked) {
    if (blocked.empty()) {
        cout << "Blocked:\nNone" << endl;
    } else{
        cout << "Blocked:" << endl;
        cout << "NAME     STRIDE  PASS  PRI" << endl;
    
        for (int i = 0; i < blocked.size(); i++) {
            cout << blocked.at(i).getName();
            
            //Incase not one letter prog name
            for (string::size_type j = blocked.at(i).getName().length(); j < 9; j++) {
                cout << " ";
            }
            
            cout << blocked.at(i).getStride();
            
            for (string::size_type j = to_string(blocked.at(i).getStride()).length(); j < 8; j++) {
                cout << " ";
            }
            
            cout << blocked.at(i).getPass();
            
            if (blocked.at(i).getPass() > 999) {
                cout << "  ";
            } else if (blocked.at(i).getPass() > 99) {
                cout << "   ";
            } else if (blocked.at(i).getPass() > 9) {
                cout << "    ";
            } else if (blocked.at(i).getPass() > 9999) {
                cout << " ";
            }
            
            for (string::size_type j = to_string(blocked.at(i).getPass()).length(); j < 6; j++ ) {
                cout << " ";
            }
            
            cout << blocked.at(i).getPrio() << endl;
        }
        
    }
}
 
unsigned long FindSecondComma(string s) {
    string::size_type i = s.find(',');
    i = s.find(',', i+1); //Find the next occurence starting from where the first comma was found.
    
    //If there isn't a 2nd Comma
    if (i == string::npos)
        i = -1;
    
    return i;
}
