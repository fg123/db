#include <iostream>
#include <readline/readline.h>
#include <readline/history.h>

#include "database.h"

int usage() {
    std::cout << "usage: db [options]" << std::endl;
    exit(1);
}

void repl(Database& db);

int main(int argc, char** argv) {
    fs::path basePath = ".";
    if (argc == 2) {
        basePath = argv[1];
    }
    try {
        Database db(basePath);
        repl(db);
    }
    catch(const char* err) {
        std::cerr << err << std::endl;
    }
    catch(std::string err) {
        std::cerr << err << std::endl;
    }
}

void repl(Database& db) {
    CommandProcessor processor;

    std::cout << "DB Prompt" << std::endl;
	std::cout << "Press Ctrl+D (EOF) to exit REPL." << std::endl;
	char* input_buffer;
    std::string command_buffer;
	for (;;) {
        input_buffer = readline("> ");
        if (input_buffer) {
            command_buffer.assign(input_buffer);
        }
	    if (!input_buffer || command_buffer == "quit" || command_buffer == "exit") {
            std::cout << std::endl;
            return;
        }
		add_history(input_buffer);
        processor.executeCommand(db, input_buffer);
        free(input_buffer);
		// run(source_to_run, vm);		
	}
}