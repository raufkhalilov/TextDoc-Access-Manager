#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib> // Для std::system
#include <limits>  // Для std::numeric_limits

bool IsLoggedIn(std::string& username)
{
    std::string /*username,*/ password;

    // Запрашиваем у пользователя имя и пароль
    std::cout << "Enter username: ";
    std::cin >> username;
    std::cout << "Enter password: ";
    std::cin >> password;

    // Формируем имя файла с данными пользователя
    std::string filename = "data/" + username + ".txt";

    // Проверяем, существует ли файл с таким именем
    std::ifstream read(filename);

    if (!read) {
        // Если файл не найден, выводим сообщение и возвращаем false
        std::cout << "User not found!" << std::endl;
        return false;
    }

    // Читаем пароль из файла
    std::string filePassword;
    getline(read, filePassword);
    read.close();

    // Проверяем, совпадает ли введённый пароль с паролем в файле
    return (filePassword == password);
}


void ClearScreen() {
#ifdef _WIN32
    std::system("cls");
#else
    std::system("clear");
#endif
}

void RegisterUser()
{
    std::string username, password;
    std::cout << "Select a username: ";
    std::cin >> username;

    std::ifstream fileCheck("data/" + username + ".txt");
    if (fileCheck)
    {
        std::cout << "Username already exists. Please choose a different username." << std::endl;
        return;
    }
    fileCheck.close();

    std::cout << "Select a password: ";
    std::cin >> password;

    // Создаём новый файл для нового пользователя
    std::ofstream file("data/" + username + ".txt");
    file << password;
    file.close();

    std::cout << "Registration successful! You can log in now." << std::endl;
}

void DisplayContents(const std::string& username)
{
    // Формируем имя файла с данными пользователя
    std::string filename = "data/" + username + ".txt";

    // Открываем файл для чтения
    std::ifstream file(filename);
    if (file) {
        // Файл открыт успешно, читаем и выводим его содержимое
        std::string line;
        std::cout << "Contents of your file:\n";
        while (getline(file, line)) {
            std::cout << line << std::endl;
        }
        file.close();
    }
    else {
        // Если файл не открыт, выводим сообщение об ошибке
        std::cout << "Unable to open file." << std::endl;
    }
}


void ChangeUsername(std::string& oldUsername)
{
    std::string newUsername;
    std::cout << "Enter new username (or '*' to cancel changes): ";
    std::cin >> newUsername;
    if (newUsername == "*")
        return;
    // Проверяем, существует ли уже новый пользователь
    std::ifstream fileCheck("data/" + newUsername + ".txt");
    if (fileCheck)
    {
        std::cout << "Username already exists. Please choose a different username." << std::endl;
        return;
    }
    fileCheck.close();

    // Переименовываем файл
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
        oldUsername = newUsername;
        std::cout << "Username changed successfully." << std::endl;
    }
    else {
        std::cout << "Error renaming file." << std::endl;
    }
}

void ChangePassword(const std::string& username)
{
    std::ifstream read("data/" + username + ".txt");

    std::string currentPasswordInFile;
    std::getline(read, currentPasswordInFile);
    read.close();

    // Запрос текущего пароля у пользователя
    std::string currentPassword;
    std::cout << "Enter current password: ";
    std::cin >> currentPassword;

    // Проверка пароля
    if (currentPassword != currentPasswordInFile) {
        std::cout << "Incorrect password." << std::endl;
        std::cout << "Press Enter to continue...";
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Игнорируем оставшиеся символы до новой строки
        std::cin.get();    // Ждём нажатия клавиши Enter
        return;
    }
    
    std::string newPassword, newPassword2;
    std::cout << "Enter new password: ";
    std::cin >> newPassword;
    std::cout << "Re-enter the password: ";
    std::cin >> newPassword2;
    if (newPassword != newPassword2) {
        std::cout << "Passwords don't match. Try later.\n";
        std::cout << "Press Enter to continue...";
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Игнорируем оставшиеся символы до новой строки
        std::cin.get();    // Ждём нажатия клавиши Enter
        return;
    }  
         
    std::ofstream file("data/" + username + ".txt");
    if (file) {
        file << newPassword;
        file.close();
        std::cout << "Password changed successfully." << std::endl;
        std::cout << "Press Enter to continue...";
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Игнорируем оставшиеся символы до новой строки
        std::cin.get();
    }
    else {
        std::cout << "Error with your account." << std::endl;
    }
}

int main()
{
    char choice;

    while (true)
    {
        std::cout << "1: Register\n2: Login\n3: Exit\nYour choice: ";
        std::cin >> choice;

        switch (choice)
        {
        case '1':
            RegisterUser();
            break;
        case '2':
        {
            std::string username;
            if (IsLoggedIn(username))
            {
                while (true) // Внутренний цикл для работы после входа
                {
                    ClearScreen();
                    std::cout << "Successfully logged in!" << std::endl;
                    std::cout << "1: Display contents\n2: Change username\n3: Change password\n4: Go to the main menu\nYour choice: ";
                    char postLoginChoice;
                    std::cin >> postLoginChoice;

                    switch (postLoginChoice)
                    {
                    case '1':
                        DisplayContents(username);
                        std::cout << "Press Enter to continue...";
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Игнорируем оставшиеся символы до новой строки
                        std::cin.get();    // Ждём нажатия клавиши Enter
                        break;
                    case '2':
                        ChangeUsername(username);
                        break;
                    case '3':
                        ChangePassword(username);
                        break;
                    case '4':
                        std::cout << "Logging out..." << std::endl;
                        ClearScreen();
                        break;
                    default:
                        std::cout << "Invalid choice. Please select again." << std::endl;
                        break;
                    }

                    if (postLoginChoice == '4') {
                        break; // Выход из внутреннего цикла и возвращение в главное меню
                    }
                }
            }
            else
            {
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

        // Добавляем паузу, чтобы пользователь мог увидеть сообщение
        std::cout << "Press Enter to continue...";
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Игнорируем оставшиеся символы до новой строки
        std::cin.get();    // Ждём нажатия клавиши Enter
        ClearScreen();
    }
}
