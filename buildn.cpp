/*
    eBuildNinja 2024.7.3 - eLite (c) 2024
    Plz don't copy this code
*/
#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <chrono>
#include <vector>
#include <iomanip>
#include <sstream>

auto start = std::chrono::high_resolution_clock::now();

using namespace std;
typedef string String;

// Define ASCII escape sequences for text modifications
std::string ASCII_BOLD;        // Bold text
const std::string ASCII_UNDERLINE = "\x1b[4m";   // Underlined text
const std::string ASCII_REVERSE = "\x1b[7m";     // Reverse (swap foreground and background colors)

// Define colors (optional)
std::string ASCII_BLACK;
std::string ASCII_RED;
std::string ASCII_GREEN;
std::string ASCII_YELLOW;
std::string ASCII_BLUE;
std::string ASCII_MAGENTA;
std::string ASCII_CYAN;
std::string ASCII_WHITE;

// Additional text modifications (optional)
const std::string ASCII_DIM = "\x1b[2m";         // Dim text
const std::string ASCII_BLINK = "\x1b[5m";       // Blinking text
const std::string ASCII_HIDDEN = "\x1b[8m";      // Hidden text
 
// Background colors (optional)
const std::string ASCII_BG_BLACK = "\x1b[40m";
const std::string ASCII_BG_RED = "\x1b[41m";
const std::string ASCII_BG_GREEN = "\x1b[42m";
const std::string ASCII_BG_YELLOW = "\x1b[43m";
const std::string ASCII_BG_BLUE = "\x1b[44m";
const std::string ASCII_BG_MAGENTA = "\x1b[45m";
const std::string ASCII_BG_CYAN = "\x1b[46m";
const std::string ASCII_BG_WHITE = "\x1b[47m";
std::string ASCII_RESET;       // Reset text to default

// Define bright color variables
const std::string BRIGHT_RED = "\033[91m";
const std::string BRIGHT_GREEN = "\033[92m";
const std::string BRIGHT_YELLOW = "\033[93m";
const std::string BRIGHT_BLUE = "\033[94m";
const std::string BRIGHT_MAGENTA = "\033[95m";
const std::string BRIGHT_CYAN = "\033[96m";
const std::string BRIGHT_WHITE = "\033[97m";
std::string RESET_COLOR = "\033[0m";

enum LOG_TYPES {INFO = 0, ERROR = 1, WARNING = 2, MESSAGE = 3, SUCCESS = 4};

void updateColors() {
    ASCII_BLACK = "\x1b[30m";
    ASCII_RED = "\x1b[31m";
    ASCII_GREEN = "\x1b[32m";
    ASCII_YELLOW = "\x1b[33m";
    ASCII_BLUE = "\x1b[34m";
    ASCII_MAGENTA = "\x1b[35m";
    ASCII_CYAN = "\x1b[36m";
    ASCII_WHITE = "\x1b[37m";
    ASCII_RESET = "\x1b[0m" + ASCII_WHITE;
    ASCII_BOLD = "\x1b[1m";
}
std::string getCurrentDateTime() {
    // Get current time
    auto now = std::chrono::system_clock::now();

    // Convert to time_t for extracting calendar time
    std::time_t now_time_t = std::chrono::system_clock::to_time_t(now);
    
    // Convert to milliseconds
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

    // Convert to tm struct for formatting
    std::tm now_tm = *std::localtime(&now_time_t);

    // Use stringstream to format the date and time
    std::stringstream ss;
    ss << std::put_time(&now_tm, "%d/%m/%Y %H:%M:%S");
    ss << '.' << std::setfill('0') << std::setw(3) << ms.count();

    return ss.str();
}
void log(LOG_TYPES type, String message) {
    if(type == INFO) {
        cout << ASCII_BOLD << ASCII_BLUE << "[INFO]" << ASCII_RESET << " " << /*(" << getCurrentDateTime() << ") " <<*/ message << "\n";
    } else if(type == ERROR) {
        cout << ASCII_BOLD << ASCII_RED << "[ERROR]" << ASCII_RESET << " " << /*(" << getCurrentDateTime() << ") " <<*/ message << "\n";
    } else if(type == WARNING) {
        cout << ASCII_BOLD << ASCII_YELLOW << "[WARNING]" << ASCII_RESET << " " << /*(" << getCurrentDateTime() << ") " <<*/ message << "\n";
    } else if(type == MESSAGE) {
        cout << ASCII_BOLD << ASCII_WHITE << "" << ASCII_RESET << "" << message << "\n";
    } else if(type == SUCCESS) {
        cout << ASCII_BOLD << ASCII_GREEN << "[SUCCESS]" << ASCII_RESET << " " << /*(" << getCurrentDateTime() << ") " <<*/ message << "\n";
    }
}

inline bool exists(const std::string& name) {
    ifstream f(name.c_str());
    return f.good();
}

std::vector<std::string> splitOutsideQuotes(const std::string& str) {
    std::vector<std::string> result;
    std::string temp;
    bool inQuotes = false;

    for (size_t i = 0; i < str.size(); ++i) {
        if (str[i] == '\"') {
            inQuotes = !inQuotes;
        } else if (str[i] == ' ' && !inQuotes) {
            if (!temp.empty()) {
                result.push_back(temp);
                temp.clear();
            }
        } else {
            temp += str[i];
        }
    }

    if (!temp.empty()) {
        result.push_back(temp);
    }

    return result;
}

int countCharsOutsideQuotes(const std::string& str, char ch) {
    int count = 0;
    bool insideQuotes = false;
    
    for (size_t i = 0; i < str.size(); ++i) {
        if (str[i] == '"') {
            insideQuotes = !insideQuotes; // Toggle the flag when encountering double quotes
        } else if (!insideQuotes && str[i] == ch) {
            count++;
        }
    }
    
    return count;
}

int runCommandAndCaptureOutput(const std::string& com) {
    std::string commandToRunFinal = com;
    
    FILE* pipe = popen(commandToRunFinal.c_str(), "r"); // Execute the command and capture its output
    if (!pipe) {
        std::cerr << "Error: Couldn't open the command pipe." << std::endl;
        return -1; // Return -1 indicating failure
    }

    const int bufferSize = 128;
    char buffer[bufferSize];
    std::string result = "" ;
    while (fgets(buffer, bufferSize, pipe) != nullptr) {
        result += buffer;
    }
    
    int status = pclose(pipe); // Get the command's exit status
    if (status == 0) {
        return 0; // Return 0 indicating success
    } else {
        std::cerr << "Command failed with error code: " << status << std::endl;
        return -1; // Return -1 indicating failure
    }
}

std::string durationToString(std::chrono::duration<double> duration) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << duration.count(); // Set precision as needed
    oss << " seconds"; // Append unit or any additional text
    
    return oss.str();
}


// Example of build.ninja.inf file:
/*
# Define executables (comment btw)
ex("main.exe" "main.cpp" "-static")
ex("a.exe", "a.cpp")
*/

int compile() {
    log(INFO, "Checking for build.ninja.inf file. . .");
    if(!exists("build.ninja.inf")) {
        log(ERROR, "build.ninja.inf file not found. Uneable to compile current path.");
        return -1;
    }
    log(INFO, "Reading build.ninja.inf file. . .");
    fstream f("build.ninja.inf");
    String line;
    String projectName, projectVersion;
    while(getline(f, line)) {
        if(line.substr(0,1) == "#") {
            // Comment, ignore
        } else if(line.substr(0,3) == "ex(") {
            // Definition of an executable file.
            String args = line.substr(3, line.length() - 2 /* Remove last ) */);
            String exeFilename = splitOutsideQuotes(args)[0];
            String srcFilename = splitOutsideQuotes(args)[1];
            String argsC = "" ;
            if(countCharsOutsideQuotes(args, ',') == 2) {
                argsC = splitOutsideQuotes(args)[2];
                srcFilename = srcFilename.substr(0, srcFilename.length() - 1);
            }

            log(INFO, "Compiling file src/" + srcFilename.erase(srcFilename.length() - 1) + " to executable target/" + exeFilename);

            // Start compiling
            String command = "g++ \"src/" + srcFilename + "\" -o \"target/" + exeFilename + "\" " + argsC;
            int re = system(command.c_str());

            if(re != 0) {
                log(ERROR, "File src/" + srcFilename + " gave an error.");
            } else {
                log(INFO, "File src/" + srcFilename + " was compiled succesfully.");
            }
        } else if(line.substr(0,2) == "v.") {
            // Definition of a variable
            String variableName = line.substr(2, line.substr(2).find_first_of('('));
            if(variableName == "projectName") {
                projectName = line.substr(line.substr(2).find_first_of('(') + 3, line.substr(line.substr(2).find_first_of('(') + 3).find_last_of(')'));
            } else if(variableName == "projectVersion") {
                projectVersion = line.substr(line.substr(2).find_first_of('(') + 3, line.substr(line.substr(2).find_first_of('(') + 3).find_last_of(')'));
            } else {
                log(WARNING, "Variable type \"" + variableName + "\" is not on variable list, ignoring.");
            }
        }
    }

    // End measuring time
    auto end = std::chrono::high_resolution_clock::now();

    // Calculate duration
    std::chrono::duration<double> duration = end - start;

    if(!projectName.empty() && !projectVersion.empty()) {
        log(SUCCESS, "----------------------------------------------------------------------------------------------------------------------");
        log(SUCCESS, "Project " + projectName + ":" + projectVersion + " build completed in " + durationToString(duration) + " at " + getCurrentDateTime());
        log(SUCCESS, "----------------------------------------------------------------------------------------------------------------------");
    } else if(projectName.empty() && !projectVersion.empty()) {
        log(SUCCESS, "----------------------------------------------------------------------------------------------------------------------");
        log(SUCCESS, "Project version " + projectVersion + " build completed in " + durationToString(duration) + " at " + getCurrentDateTime());
        log(SUCCESS, "----------------------------------------------------------------------------------------------------------------------");
    } else if(!projectName.empty() && projectVersion.empty()) {
        log(SUCCESS, "----------------------------------------------------------------------------------------------------------------------");
        log(SUCCESS, "Project  " + projectName + " build completed in " + durationToString(duration) + " at " + getCurrentDateTime());
        log(SUCCESS, "----------------------------------------------------------------------------------------------------------------------");
    } else if(projectName.empty() && projectVersion.empty()) {
        log(SUCCESS, "----------------------------------------------------------------------------------------------------------------------");
        log(SUCCESS, "Build completed in " + durationToString(duration) + " at " + getCurrentDateTime());
        log(SUCCESS, "----------------------------------------------------------------------------------------------------------------------");
    }
    return 0;
}

int test() {
    log(INFO, "Started test for current directory.");
    log(INFO, "Checking for build.ninja.inf file. . .");
    if(!exists("build.ninja.inf")) {
        log(ERROR, "build.ninja.inf file not found. Uneable to compile current path.");
        return -1;
    }
    log(INFO, "Reading build.ninja.inf file. . .");
    fstream f("build.ninja.inf");
    String line;
    String projectName, projectVersion;
    while(getline(f, line)) {
        if(line.substr(0,1) == "#") {
            // Comment, ignore
        } else if(line.substr(0,3) == "ex(") {
            // Definition of an executable file.
            String args = line.substr(3, line.length() - 2 /* Remove last ) */);
            String srcFilename = splitOutsideQuotes(args)[1];
            String argsC = "" ;
            if(countCharsOutsideQuotes(args, ',') == 2) {
                argsC = splitOutsideQuotes(args)[2];
                srcFilename = srcFilename.substr(0, srcFilename.length() - 1);
            }

            log(INFO, "Testing file src/" + srcFilename.erase(srcFilename.length() - 1) + ".");

            // Start compiling
            String command = "g++ \"src/" + srcFilename + "\" " + argsC;
            int re = system(command.c_str());

            if(re != 0) {
                log(ERROR, "File src/" + srcFilename + " gave an error.");
            } else {
                log(INFO, "File src/" + srcFilename + " was tested succesfully.");
            }
        } else if(line.substr(0,2) == "v.") {
            // Definition of a variable
            String variableName = line.substr(2, line.substr(2).find_first_of('('));
            if(variableName == "projectName") {
                projectName = line.substr(line.substr(2).find_first_of('(') + 3, line.substr(line.substr(2).find_first_of('(') + 3).find_last_of(')'));
            } else if(variableName == "projectVersion") {
                projectVersion = line.substr(line.substr(2).find_first_of('(') + 3, line.substr(line.substr(2).find_first_of('(') + 3).find_last_of(')'));
            } else {
                log(WARNING, "Variable type \"" + variableName + "\" is not on variable list, ignoring.");
            }
        }
    }

    // End measuring time
    auto end = std::chrono::high_resolution_clock::now();

    // Calculate duration
    std::chrono::duration<double> duration = end - start;

    if(!projectName.empty() && !projectVersion.empty()) {
        log(SUCCESS, "----------------------------------------");
        log(SUCCESS, "Project " + projectName + ":" + projectVersion + " build completed in " + durationToString(duration) + " at " + getCurrentDateTime());
        log(SUCCESS, "----------------------------------------");
    } else if(projectName.empty() && !projectVersion.empty()) {
        log(SUCCESS, "----------------------------------------");
        log(SUCCESS, "Project version " + projectVersion + " build completed in " + durationToString(duration) + " at " + getCurrentDateTime());
        log(SUCCESS, "----------------------------------------");
    } else if(!projectName.empty() && projectVersion.empty()) {
        log(SUCCESS, "----------------------------------------");
        log(SUCCESS, "Project  " + projectName + " build completed in " + durationToString(duration) + " at " + getCurrentDateTime());
        log(SUCCESS, "----------------------------------------");
    } else if(projectName.empty() && projectVersion.empty()) {
        log(SUCCESS, "----------------------------------------");
        log(SUCCESS, "Build completed in " + durationToString(duration) + " at " + getCurrentDateTime());
        log(SUCCESS, "----------------------------------------");
    }
    return 0;
}

bool isStrOnCharArr(const std::string& str, char** arr, int argc) {
    for (int i = 0; i < argc; ++i) {
        if (str == arr[i]) {
            return true;
        }
    }
    return false;
}


int main(int argc, char** argv) {
    bool noascii = false;
    bool test2 = false;
    bool version = false;

    if(!isStrOnCharArr("--no-ascii", argv, argc)) {
        updateColors();
    }
    if(isStrOnCharArr("--version", argv, argc)) {
        log(MESSAGE, ASCII_BOLD + "eBuildNinja 2024.7.3 - eLite (c) 2024" + ASCII_RESET);
        return 0;
    }
    if(isStrOnCharArr("test", argv, argc)) {
        test2 = true;
    }

    log(INFO, "Starting eBuildNinja. . .");
    log(INFO, "Checking if C++ is installed. . .");
    if(runCommandAndCaptureOutput("gcc --version") != 0) {
        log(ERROR, "C++ should be installed and be added to PATH enviroment variables.");
        return -1;
    }
    log(INFO, "As no argument was found, running as default current path.");

    if(test2) {
        return test();
    }
    
    return compile();
}