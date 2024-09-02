#include <iostream>
#include <fstream>
#include <string>
#include <asio.hpp> // Используем библиотеку Asio для работы с сетью

using asio::ip::tcp;

std::string HandleRegister(const std::string& username, const std::string& password) {
    std::ifstream fileCheck("data/" + username + ".txt");
    if (fileCheck) {
        return "Username already exists. Please choose a different username.";
    }
    fileCheck.close();

    std::ofstream file("data/" + username + ".txt");
    file << password;
    file.close();

    return "Registration successful! You can log in now.";
}

std::string HandleLogin(const std::string& username, const std::string& password) {
    std::ifstream read("data/" + username + ".txt");
    if (!read) {
        return "User not found!";
    }

    std::string filePassword;
    getline(read, filePassword);
    read.close();

    if (filePassword == password) {
        return "success";
    } else {
        return "Incorrect password.";
    }
}

std::string HandleDisplay(const std::string& username) {
    std::string filename = "data/" + username + ".txt";
    std::ifstream file(filename);
    if (file) {
        std::string line, result;
        getline(file, line);
        while (getline(file, line)) {
            result += line + "\n";
        }
        file.close();
        return result;
    } else {
        return "Unable to open file.";
    }
}

std::string HandleChangeUsername(const std::string& oldUsername, const std::string& newUsername) {
    std::ifstream fileCheck("data/" + newUsername + ".txt");
    if (fileCheck) {
        return "Username already exists. Please choose a different username.";
    }
    fileCheck.close();

    std::ifstream oldFile("data/" + oldUsername + ".txt");
    std::ofstream newFile("data/" + newUsername + ".txt");
    if (oldFile && newFile) {
        std::string content;
        while (getline(oldFile, content)) {
            newFile << content << std::endl;
        }
        oldFile.close();
        newFile.close();
        std::remove(("data/" + oldUsername + ".txt").c_str());
        return "success";
    } else {
        return "Error renaming file.";
    }
}

std::string HandleChangePassword(const std::string& username, const std::string& currentPassword, const std::string& newPassword) {
    std::ifstream read("data/" + username + ".txt");

    std::string currentPasswordInFile;
    std::getline(read, currentPasswordInFile);
    read.close();

    if (currentPassword != currentPasswordInFile) {
        return "Incorrect password.";
    }

    std::ofstream file("data/" + username + ".txt");
    if (file) {
        file << newPassword;
        file.close();
        return "Password changed successfully.";
    } else {
        return "Error with your account.";
    }
}

std::string HandleEdit(const std::string& filename, int lineNumber, const std::string& newText) {
    std::ifstream fileIn("data/" + filename);
    if (!fileIn) {
        return "File not found.";
    }

    std::vector<std::string> lines;
    std::string line;
    while (std::getline(fileIn, line)) {
        lines.push_back(line);
    }
    fileIn.close();

    if (lineNumber < 1 || lineNumber > lines.size()) {
        while (lines.size() < lineNumber) {
            lines.push_back("");
        }
    }

    lines[lineNumber - 1] = newText;

    std::ofstream fileOut("data/" + filename);
    if (!fileOut) {
        return "Error writing to file.";
    }

    for (const auto& l : lines) {
        fileOut << l << std::endl;
    }

    fileOut.close();
    return "Line edited successfully.";
}

void Session(tcp::socket socket) {
    try {
        asio::streambuf request;
        asio::read_until(socket, request, "\n");
        std::istream request_stream(&request);
        std::string command;
        request_stream >> command;

        std::string response;

        if (command == "register") {
            std::string username, password;
            request_stream >> username >> password;
            response = HandleRegister(username, password);
        } else if (command == "login") {
            std::string username, password;
            request_stream >> username >> password;
            response = HandleLogin(username, password);
        } else if (command == "display") {
            std::string username;
            request_stream >> username;
            response = HandleDisplay(username);
        } else if (command == "change_username") {
            std::string oldUsername, newUsername;
            request_stream >> oldUsername >> newUsername;
            response = HandleChangeUsername(oldUsername, newUsername);
        } else if (command == "change_password") {
            std::string username, currentPassword, newPassword;
            request_stream >> username >> currentPassword >> newPassword;
            response = HandleChangePassword(username, currentPassword, newPassword);
        } else if (command == "edit") {
            std::string filename, newText;
            int lineNumber;
            request_stream >> filename >> lineNumber;
            std::getline(request_stream, newText);
            response = HandleEdit(filename, lineNumber, newText);
        } else {
            response = "Unknown command";
        }

        asio::write(socket, asio::buffer(response + "\n"));
    } catch (std::exception& e) {
        std::cerr << "Session error: " << e.what() << std::endl;
    }
}

int main() {
    try {
        asio::io_context io_context;
        tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 8080));

        while (true) {
            tcp::socket socket(io_context);
            acceptor.accept(socket);
            std::thread(Session, std::move(socket)).detach();
        }
    } catch (std::exception& e) {
        std::cerr << "Server error: " << e.what() << std::endl;
    }

    return 0;
}