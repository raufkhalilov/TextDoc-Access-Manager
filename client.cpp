#include <iostream>
#include <string>
#include <cstdlib> // Для std::system
#include <limits>  // Для std::numeric_limits
#include <asio.hpp> // Используем библиотеку Asio для работы с сетью

using asio::ip::tcp;

void ClearScreen() {
#ifdef _WIN32
    std::system("cls");
#else
    std::system("clear");
#endif
}

std::string SendRequest(const std::string& request) {
    try {
        asio::io_context io_context;
        tcp::resolver resolver(io_context);
        tcp::resolver::results_type endpoints = resolver.resolve("127.0.0.1", "8080");
        tcp::socket socket(io_context);
        asio::connect(socket, endpoints);

        // Отправляем запрос на сервер
        asio::write(socket, asio::buffer(request + "\n"));

        // Читаем ответ от сервера
        asio::streambuf response;
        asio::read_until(socket, response, "\n");
        std::istream response_stream(&response);
        std::string response_data;
        std::getline(response_stream, response_data);

        return response_data;
    } catch (std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return "error";
    }
}

void RegisterUser() {
    std::string username, password;
    std::cout << "Select a username: ";
    std::cin >> username;
    std::cout << "Select a password: ";
    std::cin >> password;

    std::string request = "register " + username + " " + password;
    std::string response = SendRequest(request);

    std::cout << response << std::endl;
}

bool LoginUser(std::string& username) {
    std::string password;
    std::cout << "Enter username: ";
    std::cin >> username;
    std::cout << "Enter password: ";
    std::cin >> password;

    std::string request = "login " + username + " " + password;
    std::string response = SendRequest(request);

    if (response == "success") {
        return true;
    } else {
        std::cout << response << std::endl;
        return false;
    }
    
}

void DisplayContents(const std::string& username) {
    std::string request = "display " + username;
    std::string response = SendRequest(request);

    std::cout << response << std::endl;
    std::cin.get();
}

void ChangeUsername(std::string& oldUsername) {
    std::string newUsername;
    std::cout << "Enter new username (or '*' to cancel changes): ";
    std::cin >> newUsername;
    if (newUsername == "*") {
        return;
    }

    std::string request = "change_username " + oldUsername + " " + newUsername;
    std::string response = SendRequest(request);

    if (response == "success") {
        oldUsername = newUsername;
    }
    std::cout << response << std::endl;
}

void ChangePassword(const std::string& username) {
    std::string currentPassword, newPassword, newPassword2;
    std::cout << "Enter current password: ";
    std::cin >> currentPassword;
    std::cout << "Enter new password: ";
    std::cin >> newPassword;
    std::cout << "Re-enter the password: ";
    std::cin >> newPassword2;

    if (newPassword != newPassword2) {
        std::cout << "Passwords don't match. Try later.\n";
        return;
    }

    std::string request = "change_password " + username + " " + currentPassword + " " + newPassword;
    std::string response = SendRequest(request);

    std::cout << response << std::endl;
}

int main() {
    char choice;

    while (true) {
        std::cout << "1: Register\n2: Login\n3: Exit\nYour choice: ";
        std::cin >> choice;

        switch (choice) {
            case '1':
                RegisterUser();
                break;
            case '2': {
                std::string username;
                if (LoginUser(username)) {
                    while (true) {
                        ClearScreen();
                        std::cout << "Successfully logged in!" << std::endl;
                        std::cout << "1: Display contents\n2: Change username\n3: Change password\n4: Go to the main menu\nYour choice: ";
                        char postLoginChoice;
                        std::cin >> postLoginChoice;

                        switch (postLoginChoice) {
                            case '1':
                                DisplayContents(username);
                                std::cout << "Press Enter to continue..." << std::endl;
                                std::cin.get();
                                break;
                            case '2':
                                ChangeUsername(username);
                                std::cout << "Press Enter to continue..." << std::endl;
                                std::cin.get();
                                break;
                            case '3':
                                ChangePassword(username);
                                std::cout << "Press Enter to continue..." << std::endl;
                                std::cin.get();
                                break;
                            case '4':
                                std::cout << "Logging out..." << std::endl;
                                ClearScreen();
                                // std::cout << "Press Enter to continue..." << std::endl;
                                // std::cin.get();
                                break;
                            default:
                                std::cout << "Invalid choice. Please select again." << std::endl;
                                std::cout << "Press Enter to continue..." << std::endl;
                                std::cin.get();
                                break;
                        }

                        if (postLoginChoice == '4') {
                            break;
                        }
                    }
                } else {
                    std::cout << "Login failed!" << std::endl;
                }
                break;
            }
            case '3':
                return 0;
            default:
                std::cout << "Invalid choice. Please select again." << std::endl;
                break;
        }

        std::cout << "Press Enter to continue...";
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cin.get();
        ClearScreen();
    }
}
