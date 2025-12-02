#include <any>
#include <cstdint>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <list>
#include <mutex>
#include <regex>
#include <sstream>
#include <stack>
#include <string>
#include <thread>
#include <vector>

#include "../inc/core.h"
#include "../inc/os.h"
#include "../inc/scheduler.h"

OS::OS (DisplayManager &dm, MemoryManager &mm, Config &config)
    : dm(dm),                                    // Display manager
      mm(mm),                                    // Memory Manager
      config(config),                            // Config values
      scheduler(Scheduler(config.getScheduler(), // Scheduler
                          config.getQuantumCycles())),
      generateDummyProcesses(false)
{
    for (int i = 0; i < config.getNumCPU(); i++) {
        this->cores.push_back(Core(i));
    }
};

// TODO: Unused

void OS::updateConfig (Config &config)
{
    this->config = config;

    this->scheduler =
        Scheduler(config.getScheduler(), config.getQuantumCycles());

    this->cores.clear();
    for (int i = 0; i < config.getNumCPU(); ++i) {
        this->cores.push_back(Core(i));
    }
}

void OS::reset ()
{
    std::lock_guard<std::mutex> lock(this->mutex);
    this->cycle = 0;
    this->activeCycle = 0;
    this->processAutoId = 0;

    this->cores.clear();
    this->finishedProcesses.clear();

    while (!this->threads.empty()) {
        if (this->threads.front().joinable())
            this->threads.front().join();
        this->threads.pop();
    }

    if (this->thread.joinable())
        this->thread.join();

    this->generateDummyProcesses = false;
    this->running = false;

    this->loadedProcess.reset();
}

void OS::incrementCycles () { this->cycle++; }

void OS::resetCycles () { 
    this->cycle = 0;
    this->activeCycle = 0;
}

void OS::ls (bool writeReport)
{
    // Lock the mutex first to avoid segfaults when accessing a pre-empted
    // process (which would be a null pointer)
    std::lock_guard<std::mutex> lock(this->mutex);

    // Set up a string stream to print to
    std::stringstream ss;

    // Number of cores
    int nCores = this->cores.size();

    // Calculate number of running cores
    std::list<int> runningCoreIds;
    for (int i = 0; i < nCores; i++) {
        if (cores[i].isRunning())
            runningCoreIds.push_back(cores[i].getId());
    }
    int nRunningCores = runningCoreIds.size();

    // CPU utilization
    float cpuUtil = nRunningCores * 100.0 / nCores;

    // Print system information to the stream
    ss << "CPU utilization: " << cpuUtil << "\%\n"
       << "Cores used: " << nRunningCores << "\n"
       << "Cores available: " << nCores - nRunningCores << "\n\n";

    // Running processes
    ss << "Running processes:\n";
    for (int i : runningCoreIds) {
        // Process has a name
        ss << std::left << std::setw(12)
           << cores[i].getProcessReference()->getName();

        // Formatted process start time
        std::time_t now = cores[i].getProcessReference()->getStartTime();
        ss << std::put_time(std::localtime(&now), " (%m/%d/%Y %H:%M:%S)");

        // Core ID
        ss << "  Core: " << std::setw(5) << std::to_string(cores[i].getId());

        // Progress
        ss << std::right << std::setw(10)
           << cores[i].getProcessReference()->getProgramCounter() << " / "
           << cores[i].getProcessReference()->getTotalCycles() << "\n";
    }

    // Finished processes
    ss << "\nFinished processes:\n";
    for (std::pair<std::string, int> finishedProcess :
         this->finishedProcesses) {
        // Process name + Creation timestamp
        ss << finishedProcess.first << "  FINISHED";

        // Progress
        // Right-align with width 13 lines it up with running processes
        ss << std::right << std::setw(13) << finishedProcess.second << " / "
           << finishedProcess.second << "\n";
    }

    this->dm.clearOutputWindow();

    // report-util
    if (writeReport) {
        std::ofstream report("csopesy-log.txt");
        report << ss.str();
        report.close();
        this->dm._mvwprintw(0, 0, "Report written to csopesy-log.txt");
    } // screen -ls
    else {
        this->dm.clearOutputWindow();

        std::string line;
        int y = 0;

        // Print each line of string -ls separately
        while (std::getline(ss, line)) {
            this->dm._mvwprintw(y++, 0, "%s", line.c_str());
        }

        // Refresh window to show everything
        this->dm.refreshPad();
    }
}

void OS::screenR (std::string processName)
{
    // Lock mutex
    std::lock_guard<std::mutex> lock(this->mutex);

    // This pointer goes out of scope at the end of this call
    std::shared_ptr<Process> foundProcess =
        this->findOngoingProcessByName(processName);

    this->dm.clearOutputWindow();

    if (foundProcess != nullptr) {
        // Share ownership of pointer with OS
        this->loadedProcess = foundProcess;
        this->showDefaultProcessScreenMessage();
    } else {
        this->dm._mvwprintw(0, 0,
                            "Process '%s' not found. Use command "
                            "\"exit\" to return to main manu.",
                            processName.c_str());
    }
}

void OS::showDefaultProcessScreenMessage ()
{
    this->dm._mvwprintw(
        0, 0,
        // Default message
        "You are on the screen for process %s. Type help to get a list of "
        "valid commands.",
        this->loadedProcess->getName().c_str());
}

std::vector<std::shared_ptr<Instruction>> parseInstructionSet (
    std::string instructionSet)
{
    std::vector<std::shared_ptr<Instruction>> parsed_instructions;

    std::stringstream ssRaw(instructionSet);
    std::stringstream ssOut;
    std::stack<char> tempStack;

    ssRaw >> std::ws;
    char c;
    while (ssRaw.get(c)) {
        if (c == '[' || c == '(') {
            tempStack.push(c);
        } else if (!tempStack.empty() && c == ']' && tempStack.top() == '[') {
            tempStack.pop();
        } else if (!tempStack.empty() && c == ')' && tempStack.top() == '(') {
            tempStack.pop();
        }

        ssOut << c;
        if (c == ';' && tempStack.empty()) {
            ssOut << '\n';
            ssRaw >> std::ws;
        } else if (c == ',' && (tempStack.size() <= 1)) {
            ssOut << '!'; // im out of ideas man
            ssRaw >> std::ws;
        }
    }

    const std::regex pattern("[()[\\]]");

    std::string line;
    while (std::getline(ssOut, line, '\n')) {
        std::string cleaned = std::regex_replace(line, pattern, " ");

        int split = cleaned.find(' ');
        std::string instruction = cleaned.substr(0, split);
        std::string args = cleaned.substr(split);

        std::stringstream argsSS(args);
        std::string arg;

        std::vector<std::any> args_vec;

        argsSS >> std::ws;
        while (std::getline(argsSS, arg, '!')) {
            argsSS >> std::ws;
            arg.erase(arg.find_last_not_of(",; ") + 1);

            if (instruction == "FOR") {
                args_vec.push_back(parseInstructionSet(arg));
                continue;
            }

            // If hex addr
            if (arg.length() > 2 && arg.substr(0, 2) == "0x") {
                try {
                    // Convert to uint32
                    uint32_t val = std::stoul(arg, nullptr, 16);
                    args_vec.push_back(val);
                } catch (...) {
                    args_vec.push_back(arg);
                }
            } else {
                try {
                    // If numeric (uint16_t)
                    // TODO: Range check idk
                    uint16_t val = std::stoul(arg);
                    args_vec.push_back(val);
                } catch (...) {
                    // If varname (string)
                    args_vec.push_back(arg);
                }
            }
        }

        std::shared_ptr<Instruction> instructionObj(new Instruction);

        instructionObj->args = args_vec;

        // yandev time baby
        if (instruction == "PRINT") {
            instructionObj->id = PRINT;
        } else if (instruction == "DECLARE") {
            instructionObj->id = DECLARE;
        } else if (instruction == "ADD") {
            instructionObj->id = ADD;
        } else if (instruction == "SUBTRACT") {
            instructionObj->id = SUBTRACT;
        } else if (instruction == "SLEEP") {
            instructionObj->id = SLEEP;
        } else if (instruction == "FOR") {
            instructionObj->id = FOR;
        } else if (instruction == "READ") {
            instructionObj->id = READ;
        } else if (instruction == "WRITE") {
            instructionObj->id = WRITE;
        }

        parsed_instructions.push_back(instructionObj);
    }

    return parsed_instructions;
}

/* TODO: ScreenC*/
void OS::screenC (std::string processName,
                  uint32_t memsize,
                  std::string instructionSet)
{
    std::unique_lock<std::mutex> lock(this->mutex);

    std::stringstream ss(instructionSet);
    std::string instruction_line;

    if (memsize >= 64 || memsize <= 65536) { // 2^6 to 2^16
        if (!(memsize > 0) && ((memsize & (memsize - 1)) == 0)) {
            this->dm._mvwprintw(
                0, 0,
                "invalid memory allocation: Memory size must be a power of 2.");
            return;
        }
    } else {
        this->dm._mvwprintw(0, 0,
                            "invalid memory allocation: Memory size must be in "
                            "the range of 64 (2^6) to 65536 (2^16)");
        return;
    }

    // Parse
    std::vector<std::shared_ptr<Instruction>> parsed_instructions =
        parseInstructionSet(instructionSet);

    if (parsed_instructions.size() < 1 || parsed_instructions.size() > 50) {
        this->dm._mvwprintw(
            0, 0,
            "invalid command: Instruction set size must be between 1 and 50.");
        return;
    }

    uint32_t instruction_count = parsed_instructions.size();

    std::shared_ptr<Process> process(
        new Process(this->processAutoId, processName, parsed_instructions,
                    memsize, this->config.getMemPerFrame()));

    this->processAutoId++;
    this->loadedProcess = process; // Idk what this does, it used to be
                                   // foundProcess but thats not a thing
    this->showDefaultProcessScreenMessage();
    this->scheduler.addProcess(process);
}

/* TODO: mem size*/
void OS::screenS (std::string processName /*, uint32_t memsize */)
{
    // Lock mutex before creating a new process
    std::unique_lock<std::mutex> lock(this->mutex);

    // Shared pointer goes out of scope immediately LOL
    if (this->findOngoingProcessByName(processName)) {
        this->dm._mvwprintw(0, 0, "Process '%s' already exists.",
                            processName.c_str());
        return;
    }

    // TODO: Should we be able to reuse names for process that have already
    // terminated?
    for (const auto &entry : this->finishedProcesses) {
        const std::string &label = entry.first;
        size_t tabPos = label.find('\t');
        std::string name =
            (tabPos != std::string::npos) ? label.substr(0, tabPos) : label;

        if (name == processName) {
            this->dm._mvwprintw(0, 0, "Process '%s' already exists.",
                                processName.c_str());
            return;
        }
    }

    std::shared_ptr<Process> process(new Process(
        this->processAutoId, processName,
        this->config.getMinIns() +
            rand() % (this->config.getMaxIns() - this->config.getMinIns() + 1),
        this->config.getMinMemPerProc() +
            rand() % (this->config.getMaxMemPerProc() -
                      this->config.getMinMemPerProc() + 1),
        this->config.getMemPerFrame()));

    // TODO: Would be confusing when a process is named "Haachama", with id:
    // 810, and I name a process process810, with id 812
    this->processAutoId++;

    // Copies the shared pointer to loadedProcess
    this->loadedProcess = process;

    // Show default message for this screen
    this->showDefaultProcessScreenMessage();

    // Transfers ownership to scheduler queue
    this->scheduler.addProcess(process);
}

std::shared_ptr<Process> OS::findOngoingProcessByName (
    const std::string &processName)
{
    // WARNING: Must already hold mutex before calling this function

    // Search cores
    for (Core &core : this->cores) {
        if (core.isRunning() &&
            core.getProcessReference()->getName() == processName)
            // Share pointer ownership
            return core.getProcess();
    }

    // Search ready queue
    const auto &readyQueue = this->scheduler.getReadyQueue();
    for (const auto &process : readyQueue) {
        if (process->getName() == processName)
            // Share pointer ownership
            return process;
    }

    // Search sleep queue
    const auto &sleepQueue = this->scheduler.getSleepQueue();
    for (const auto &process : sleepQueue) {
        if (process->getName() == processName)
            // Share pointer ownership
            return process;
    }

    return nullptr; // Not found
}

void OS::processSMI_process ()
{
    std::lock_guard<std::mutex> lock(this->mutex);
    this->dm.clearOutputWindow();

    // Print process state information
    std::stringstream ss(this->loadedProcess->getStateAsString());

    std::string line;
    int y = 0;

    // Print each line of string process-smi separately
    while (std::getline(ss, line)) {
        this->dm._mvwprintw(y++, 0, "%s", line.c_str());
    }

    // Refresh window to show everything
    this->dm.refreshPad();
}

void OS::processSMI_main ()
{

    std::lock_guard<std::mutex> lock(this->mutex);
    this->dm.clearOutputWindow();

    std::stringstream ss;

    std::string line;
    int y = 0;

    // Number of cores
    int nCores = this->cores.size();

    // Calculate number of running cores
    std::list<int> runningCoreIds;
    for (int i = 0; i < nCores; i++) {
        if (cores[i].isRunning())
            runningCoreIds.push_back(cores[i].getId());
    }
    int nRunningCores = runningCoreIds.size();

    int mem_usage = mm.getMemUsage();
    int ram_size = mm.getRAMSize();
    float memUtil = mem_usage * 100.0 / ram_size;
    float cpuUtil = nRunningCores * 100.0 / nCores;
    this->dm._mvwprintw(y++, 0, "CPU Utlization: ", cpuUtil);
    this->dm._mvwprintw(y++, 0, "Memory Usage: %d / %d", mem_usage, ram_size);
    this->dm._mvwprintw(y++, 0, "Memory Utiization: %.2f", memUtil);

    int totalMemUsedForReal = 0;
    ss << "Running processes:\n";
    for (int i : runningCoreIds) {
        // Process has a name
        ss << std::left << std::setw(12)
           << cores[i].getProcessReference()->getName();

        // Formatted process start time
        std::time_t now = cores[i].getProcessReference()->getStartTime();
        ss << std::put_time(std::localtime(&now), " (%m/%d/%Y %H:%M:%S)");

        // Core ID
        ss << "  Core: " << std::setw(5) << std::to_string(cores[i].getId());

        // Progress
        ss << std::right << std::setw(10)
           << cores[i].getProcessReference()->getProgramCounter() << " / "
           << cores[i].getProcessReference()->getTotalCycles() << " ";

        int pid = cores[i].getProcessReference()->getId();
        uint32_t processMem = mm.getMemUsageForPID(pid);

        ss << "Mem Usage: " << processMem << " bytes\n";

        totalMemUsedForReal += processMem;
    }

    // Print each line of string -ls separately
    while (std::getline(ss, line)) {
        this->dm._mvwprintw(y++, 0, "%s", line.c_str());
    }

    this->dm._mvwprintw(y++, 0, "Total Memory Used For Real: %d \n",
                        totalMemUsedForReal);

    // Refresh window to show everything
    this->dm.refreshPad();

    // Refresh window to show everything
    this->dm.refreshPad();
<<<<<<< HEAD
=======
}

void OS::vmstat (){
    std::lock_guard<std::mutex> lock(this->mutex);
    this->dm.clearOutputWindow();

    std::stringstream ss;

    std::string line;
    int y = 0;

    int mem_usage = mm.getMemUsage();
    int ram_size = mm.getRAMSize();
    this->dm._mvwprintw(y++, 0, "Total Memory: %d", ram_size);
    this->dm._mvwprintw(y++, 0, "Used Memory: %d / %d", mem_usage, ram_size);
    this->dm._mvwprintw(y++, 0, "Free Memory: %d / %d", ram_size - mem_usage, ram_size);
    this->dm._mvwprintw(y++, 0, "Idle CPU ticks: %d", this->cycle - this->activeCycle);
    this->dm._mvwprintw(y++, 0, "Active CPU ticks: %d", this->activeCycle);
    this->dm._mvwprintw(y++, 0, "Total CPU ticks: %d", this->cycle);
    this->dm._mvwprintw(y++, 0, "Num paged in: %d", mm.getPageInCount());
    this->dm._mvwprintw(y++, 0, "Num paged out: %d", mm.getPageOutCount());

    this->dm.refreshPad();
>>>>>>> process-smi_for_main
}

void OS::setGenerateDummyProcesses (bool value)
{
    this->generateDummyProcesses = value;
}

bool OS::isRunning () { return this->running; }

bool OS::isGenerating () { return this->generateDummyProcesses; }

void OS::exit ()
{
    this->running = false;

    // Lock the mutex. TODO: I don't think this is actl needed
    // std::lock_guard<std::mutex> lock(this->mutex);

    if (this->thread.joinable()) {
        this->thread.join();
    }
}
