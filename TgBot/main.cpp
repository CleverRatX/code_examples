#include <tgbot/tgbot.h>
#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <map>

#include "voice_recognition.h"
#include "sorting_algorithms.h"

using namespace std;

string token = "";
TgBot::Bot* botPtr;
map<int64_t, string> selectedAlgorithm;

// Отправка главного меню
void sendMainMenu(int64_t chatId)
{
    TgBot::ReplyKeyboardMarkup::Ptr keyboard(new TgBot::ReplyKeyboardMarkup);
    vector<TgBot::KeyboardButton::Ptr> row;

    // Кнопки
    TgBot::KeyboardButton::Ptr btn1(new TgBot::KeyboardButton);
    btn1->text = "Камень, ножницы, бумага";
    btn1->requestContact = false;
    btn1->requestLocation = false;
    row.push_back(btn1);

    TgBot::KeyboardButton::Ptr btn2(new TgBot::KeyboardButton);
    btn2->text = "Преобразование речи в текст";
    btn2->requestContact = false;
    btn2->requestLocation = false;
    row.push_back(btn2);

    TgBot::KeyboardButton::Ptr btn3(new TgBot::KeyboardButton);
    btn3->text = "Сортировка строки";
    btn3->requestContact = false;
    btn3->requestLocation = false;
    row.push_back(btn3);

    keyboard->keyboard.push_back(row);
    keyboard->resizeKeyboard = true;
    botPtr->getApi().sendMessage(chatId, "Главное меню. Выберите действие:", false, 0, keyboard);
}

// Отправка выбора сортировки
void sendSortMenu(int64_t chatId)
{
    TgBot::ReplyKeyboardMarkup::Ptr keyboard(new TgBot::ReplyKeyboardMarkup);
    vector<TgBot::KeyboardButton::Ptr> row1;
    vector<TgBot::KeyboardButton::Ptr> row2;

    // Первая строка кнопок
    TgBot::KeyboardButton::Ptr b1(new TgBot::KeyboardButton);
    b1->text = "Пузырьковая сортировка";
    b1->requestContact = false;
    b1->requestLocation = false;
    row1.push_back(b1);

    TgBot::KeyboardButton::Ptr b2(new TgBot::KeyboardButton);
    b2->text = "Сортировка Хоара";
    b2->requestContact = false;
    b2->requestLocation = false;
    row1.push_back(b2);

    TgBot::KeyboardButton::Ptr b3(new TgBot::KeyboardButton);
    b3->text = "Сортировка вставками";
    b3->requestContact = false;
    b3->requestLocation = false;
    row1.push_back(b3);

    // Вторая строка кнопок
    TgBot::KeyboardButton::Ptr b4(new TgBot::KeyboardButton);
    b4->text = "Сортировка выбором";
    b4->requestContact = false;
    b4->requestLocation = false;
    row2.push_back(b4);

    TgBot::KeyboardButton::Ptr b5(new TgBot::KeyboardButton);
    b5->text = "Сортировка слиянием";
    b5->requestContact = false;
    b5->requestLocation = false;
    row2.push_back(b5);

    TgBot::KeyboardButton::Ptr b6(new TgBot::KeyboardButton);
    b6->text = "Назад в главное меню";
    b6->requestContact = false;
    b6->requestLocation = false;
    row2.push_back(b6);

    keyboard->keyboard.push_back(row1);
    keyboard->keyboard.push_back(row2);
    keyboard->resizeKeyboard = true;
    botPtr->getApi().sendMessage(chatId, "Выберите алгоритм сортировки:", false, 0, keyboard);
}

// /start
void handleStart(TgBot::Message::Ptr msg)
{
    sendMainMenu(msg->chat->id);
}

// Голосовые сообщения
void handleVoiceMessage(TgBot::Message::Ptr msg)
{
    int64_t chatId = msg->chat->id;
    botPtr->getApi().sendChatAction(chatId, "typing");

    TgBot::Voice::Ptr voice = msg->voice;
    TgBot::File::Ptr file = botPtr->getApi().getFile(voice->fileId);

    // Скачиваем файл
    string baseUrl = "https://api.telegram.org/file/bot";
    string url = baseUrl + token + "/" + file->filePath;
    string localFile = "voice_" + to_string(chatId) + "_" + to_string(rand()) + ".ogg";

    string downloadCmd = "curl -s -o " + localFile + " \"" + url + "\"";
    exec(downloadCmd.c_str());

    string recognized = transcribeAudio(localFile);
    if (recognized.empty() || recognized.find("whisper") != string::npos)
        recognized = "Не удалось распознать речь.";

    botPtr->getApi().sendMessage(chatId, "Распознанный текст:\n" + recognized);
}

// Камень, ножницы, бумага
void handleGame(TgBot::Message::Ptr msg)
{
    int64_t chatId = msg->chat->id;

    TgBot::InlineKeyboardMarkup::Ptr kb(new TgBot::InlineKeyboardMarkup);
    vector<TgBot::InlineKeyboardButton::Ptr> row;

    TgBot::InlineKeyboardButton::Ptr btn1(new TgBot::InlineKeyboardButton);
    btn1->text = "Камень";
    btn1->callbackData = "rock";
    row.push_back(btn1);

    TgBot::InlineKeyboardButton::Ptr btn2(new TgBot::InlineKeyboardButton);
    btn2->text = "Ножницы";
    btn2->callbackData = "scissors";
    row.push_back(btn2);

    TgBot::InlineKeyboardButton::Ptr btn3(new TgBot::InlineKeyboardButton);
    btn3->text = "Бумага";
    btn3->callbackData = "paper";
    row.push_back(btn3);

    kb->inlineKeyboard.push_back(row);
    botPtr->getApi().sendMessage(chatId, "Сыграем", false, 0, kb);
}

// Выбор в игре
void handleCallback(TgBot::CallbackQuery::Ptr query)
{
    string userChoice = query->data;
    int64_t chatId = query->message->chat->id;

    string choices[3] = {"rock", "scissors", "paper"};
    string botChoice = choices[rand() % 3];

    string result;
    if (userChoice == botChoice)
        result = "Ничья!";
    else if ((userChoice == "rock" && botChoice == "scissors") ||
             (userChoice == "scissors" && botChoice == "paper") ||
             (userChoice == "paper" && botChoice == "rock"))
        result = "Вы выиграли!";
    else
        result = "Вы проиграли. ";

    // Названия для вывода
    string userName, botName;
    if (userChoice == "rock") userName = "Камень";
    else if (userChoice == "scissors") userName = "Ножницы";
    else userName = "Бумага";

    if (botChoice == "rock") botName = "Камень";
    else if (botChoice == "scissors") botName = "Ножницы";
    else botName = "Бумага";

    string answer = "Вы: " + userName + "\nБот: " + botName + "\n" + result;
    botPtr->getApi().sendMessage(chatId, answer);
    botPtr->getApi().answerCallbackQuery(query->id);
}

// Сортировка строки
void handleSorting(int64_t chatId, string text)
{
    string algo = selectedAlgorithm[chatId];
    string result;

    if (algo == "Пузырьковая сортировка")
        result = bubbleSort(text);
    else if (algo == "Сортировка Хоара")
        result = quickSort(text);
    else if (algo == "Сортировка вставками")
        result = insertionSort(text);
    else if (algo == "Сортировка выбором")
        result = selectionSort(text);
    else if (algo == "Сортировка слиянием")
        result = mergeSort(text);

    botPtr->getApi().sendMessage(chatId, "Результат:\n" + result);
    sendMainMenu(chatId);
    selectedAlgorithm.erase(chatId);
}

// Обработка сообщений
void handleMessage(TgBot::Message::Ptr msg)
{
    int64_t chatId = msg->chat->id;

    // Проверка на голосовое сообщение
    if (msg->voice != nullptr)
    {
        handleVoiceMessage(msg);
        return;
    }

    // Проверка на пустой текст
    if (msg->text == "")
        return;

    string text = msg->text;

    // Обработка кнопок меню
    if (text == "Камень, ножницы, бумага")
    {
        handleGame(msg);
        return;
    }

    if (text == "Преобразование речи в текст")
    {
        botPtr->getApi().sendMessage(chatId, "Отправьте голосовое сообщение.");
        return;
    }

    if (text == "Сортировка строки")
    {
        sendSortMenu(chatId);
        return;
    }

    if (text == "Назад в главное меню")
    {
        sendMainMenu(chatId);
        selectedAlgorithm.erase(chatId);
        return;
    }

    // Проверка на выбор алгоритма
    if (text == "Пузырьковая сортировка" || text == "Сортировка Хоара" ||
        text == "Сортировка вставками" || text == "Сортировка выбором" ||
        text == "Сортировка слиянием")
    {
        selectedAlgorithm[chatId] = text;
        botPtr->getApi().sendMessage(chatId, "Введите строку для сортировки:");
        return;
    }

    // Сортируем, если выбран алгоритм 
    if (selectedAlgorithm.count(chatId) > 0)
    {
        handleSorting(chatId, text);
        return;
    }

    // Неизвестная команда
    botPtr->getApi().sendMessage(chatId, "Неизвестная команда.");
    sendMainMenu(chatId);
}

int main()
{
    TgBot::Bot bot(token);
    botPtr = &bot;  // Указатель для использования в функциях

    // Обработчики
    bot.getEvents().onCommand("start", handleStart);
    bot.getEvents().onNonCommandMessage(handleMessage);
    bot.getEvents().onCallbackQuery(handleCallback);

    // Запуск бота
    try
    {
        cout << "Бот запущен\n";
        TgBot::TgLongPoll lp(bot);
        while (true)
            lp.start();
    }
    catch (const TgBot::TgException& e)
    {
        cerr << "Ошибка: " << e.what() << endl;
    }

    return 0;
}