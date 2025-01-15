﻿#include "ProiectMCQt.h"
#include <cpr/cpr.h>
#include <QMessageBox>
#include <QGridLayout>
#include <QLabel>
#include <QPalette>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QKeyEvent>
#include <QPixmap>
#include <QDebug>
#include <QBrush>
#include <QGraphicsOpacityEffect>
#include<QLineEdit>
#include<QPushButton>
#include<QTimer>

ProiectMCQt::ProiectMCQt(QWidget* parent)
    : QMainWindow(parent)
{
    setAttribute(Qt::WA_TranslucentBackground); // Permite transparența pentru fereastra principală
    setupUI();
    //fetchData();
}

ProiectMCQt::~ProiectMCQt()
{
    // Curățarea resurselor, dacă este necesar
}

void ProiectMCQt::setupUI()
{
    // Create central widget and layout
    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    QGridLayout* layout = new QGridLayout(centralWidget);
    centralWidget->setLayout(layout);

    // Initialize the UI state
    showLoginRegisterUI();
}

void ProiectMCQt::showLoginRegisterUI()
{
    QGridLayout* layout = qobject_cast<QGridLayout*>(centralWidget->layout());
    // Clear any existing layout items
    QLayoutItem* item;
    while ((item = layout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }

    // Add fields for username and password
    QLabel* usernameLabel = new QLabel("Username:", this);
    QLineEdit* usernameLineEdit = new QLineEdit(this);
    layout->addWidget(usernameLabel, 0, 0);
    layout->addWidget(usernameLineEdit, 0, 1);

    QLabel* passwordLabel = new QLabel("Password:", this);
    QLineEdit* passwordLineEdit = new QLineEdit(this);
    passwordLineEdit->setEchoMode(QLineEdit::Password); // Hide password characters
    layout->addWidget(passwordLabel, 1, 0);
    layout->addWidget(passwordLineEdit, 1, 1);

    // Add buttons for register and login
    QPushButton* registerButton = new QPushButton("Register", this);
    QPushButton* loginButton = new QPushButton("Login", this);
    layout->addWidget(registerButton, 2, 0);
    layout->addWidget(loginButton, 2, 1);

    // Connect buttons to their respective functions
    QObject::connect(registerButton, &QPushButton::clicked, [=]() {
        std::string username = usernameLineEdit->text().toStdString();
        std::string password = passwordLineEdit->text().toStdString();
        registerUser(username, password);
        });

    QObject::connect(loginButton, &QPushButton::clicked, [=]() {
        std::string username = usernameLineEdit->text().toStdString();
        std::string password = passwordLineEdit->text().toStdString();
        loginUser(username, password);
        });
}

void ProiectMCQt::showGameMenuUI()
{
    // Clear any existing layout items
    QGridLayout* layout = qobject_cast<QGridLayout*>(centralWidget->layout());
    QLayoutItem* item;
    while ((item = layout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }

    // Add a label and buttons for game selection
    QLabel* menuLabel = new QLabel("Choose a game:", this);
    layout->addWidget(menuLabel, 0, 0);

    QPushButton* game1Button = new QPushButton("Game 1", this);
    QPushButton* game2Button = new QPushButton("Game 2", this);
    layout->addWidget(game1Button, 1, 0);
    layout->addWidget(game2Button, 1, 1);

    // Connect buttons to functions that handle game selection
    QObject::connect(game1Button, &QPushButton::clicked, this, &ProiectMCQt::joinGame);
    QObject::connect(game2Button, &QPushButton::clicked, this, &ProiectMCQt::joinGame);
}

void ProiectMCQt::loginUser(const std::string& username, const std::string& password) {
    cpr::Response response = cpr::Post(cpr::Url{ "http://localhost:18080/login" },
        cpr::Body{ R"({"username":")" + username + R"(","password":")" + password + R"("})" },
        cpr::Header{ {"Content-Type", "application/json"} });

    if (response.status_code == 200) {
		this->username = username;
        qDebug() << "Login successful!";
        qDebug() << "Saved username:" << QString::fromStdString(this->username);
        // Show game menu after successful login
        showGameMenuUI();
    }
    else {
        qDebug() << "Failed to login: " << QString::fromStdString(response.text);
    }
}


void ProiectMCQt::fetchData()
{
    auto response = cpr::Get(cpr::Url{ "http://localhost:18080/map" });
    if (response.status_code == 200) {
        QJsonDocument jsonDoc = QJsonDocument::fromJson(response.text.c_str());
        QJsonObject jsonObj = jsonDoc.object();

        size_t height = jsonObj["height"].toInt();
        size_t width = jsonObj["width"].toInt();
        QJsonArray mapArray = jsonObj["map"].toArray();

        QGridLayout* layout = qobject_cast<QGridLayout*>(centralWidget->layout());

        QLayoutItem* item;
		while ((item = layout->takeAt(0)) != nullptr) {
			delete item->widget();
			delete item;
		}

        for (size_t i = 0; i < height; ++i) {
            QJsonArray rowArray = mapArray[i].toArray();
            for (size_t j = 0; j < width; ++j) {
                int tileValue = rowArray[j].toInt();
                QLabel* label = new QLabel();
                label->setFixedSize(20, 20); // Setați dimensiunea pătratelor
                colorTile(label, tileValue);
                layout->addWidget(label, i, j);
            }
        }

        QJsonArray wallsArray = jsonObj["walls"].toArray();
        for (const auto& wallJson : wallsArray) {
            QJsonObject wallObj = wallJson.toObject();
            size_t x = wallObj["x"].toInt();
            size_t y = wallObj["y"].toInt();
            int type = wallObj["type"].toInt();
            QLabel* label = new QLabel();
            label->setFixedSize(20, 20); // Setați dimensiunea pătratelor
            colorTile(label, type);
            layout->addWidget(label, x, y);
        }
    }
    else {
        QMessageBox::critical(this, "Error", QString::fromStdString(response.error.message));
    }
}

void ProiectMCQt::colorTile(QLabel* label, int type)
{
    QPixmap pixmap;
    switch (type) {
    case 0: // EmptySpace
        label->setAutoFillBackground(false);
        label->clear();
        break;
    case 1: // DestructibleWall
        pixmap = QPixmap("C:/Users/Cezar/Desktop/pngwing.com.png");
        if (pixmap.isNull()) {
            qDebug() << "Failed to load image!";
        }
        else {
            qDebug() << "Image loaded successfully!";
            label->setPixmap(pixmap.scaled(label->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
        }
        break;
    case 2: // IndestructibleWall
        label->setAutoFillBackground(true);
        label->setPalette(QPalette(Qt::red));
        break;
    case 3: // DestructibleWallWithBomb
        label->setAutoFillBackground(true);
        label->setPalette(QPalette(Qt::green));
        break;
    case 4: // Player
        label->setAutoFillBackground(true);
        label->setPalette(QPalette(Qt::yellow));
        break;
    default:
        label->setAutoFillBackground(true);
        label->setPalette(QPalette(Qt::gray));
        break;
    }
}


void ProiectMCQt::keyPressEvent(QKeyEvent* event)
{
    int x = playerPosition.first;
    int y = playerPosition.second;

    switch (event->key()) {
    case Qt::Key_W:
        x--;
        break;
    case Qt::Key_S:
        x++;
        break;
    case Qt::Key_A:
        y--;
        break;
    case Qt::Key_D:
        y++;
        break;
    default:
        return;
    }
    sendMoveRequest(x, y, playerID);
}

void ProiectMCQt::sendMoveRequest(int x, int y, int playerID)
{
    // Trimite cererea POST către server pentru a muta jucătorul
    cpr::Response response = cpr::Post(cpr::Url{ "http://localhost:18080/move" },
        cpr::Body{ R"({"x":)" + std::to_string(x) + R"(,"y":)" + std::to_string(y) + R"(,"playerID":)" + std::to_string(playerID) + R"(})" },
        cpr::Header{ {"Content-Type", "application/json"} });
    if (response.status_code == 200) {
        // Actualizează harta după mutare
        fetchData();
    }
    else {
        QMessageBox::critical(this, "Error", QString::fromStdString(response.error.message));
    }
}


void ProiectMCQt::registerUser(const std::string& username, const std::string& password) {
	//this->username = username;
    cpr::Response response = cpr::Post(cpr::Url{ "http://localhost:18080/signup" },
        cpr::Body{ R"({"username":")" + username + R"(","password":")" + password + R"("})" },
        cpr::Header{ {"Content-Type", "application/json"} });

    if (response.status_code == 200) {
        qDebug() << "Account created successfully!";
    }
    else {
        qDebug() << "Failed to create account: " << QString::fromStdString(response.text);
    }
}

void ProiectMCQt::joinGame() {
    auto response = cpr::Post(cpr::Url{ "http://localhost:18080/join_game" },
        cpr::Body{ R"({"username":")" + this->username + R"("})" },
        cpr::Header{ {"Content-Type", "application/json"} });

    if (response.status_code == 200) {
        qDebug() << "Joined game successfully!";
        waitForMatch(); // Wait for players to join the match
    }
    else {
        qDebug() << "Failed to join game: " << QString::fromStdString(response.text);
    }
}

void ProiectMCQt::waitForMatch() {
    QTimer* timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [=]() {
        auto response = cpr::Get(cpr::Url{ "http://localhost:18080/check_match" },
            cpr::Parameters{ { "username", this->username } });
        if (response.status_code == 200) {
            QJsonDocument jsonDoc = QJsonDocument::fromJson(response.text.c_str());
            QJsonObject jsonObj = jsonDoc.object();

            bool matchFound = jsonObj["match_found"].toBool();
            if (matchFound) {
                playerID = jsonObj["player_id"].toInt();
                playerPosition = { jsonObj["x"].toInt(), jsonObj["y"].toInt() };
                fetchData(); // Fetch initial game state
                timer->stop(); // Stop the timer
				showGameUI(); // Show the game UI
            }
        }
        else {
            qDebug() << "Error checking match status: " << QString::fromStdString(response.error.message);
        }
        });
    timer->start(5000); // Check every 5 seconds
}

void ProiectMCQt::showGameUI()
{
    // Clear any existing layout items
    QGridLayout* layout = qobject_cast<QGridLayout*>(centralWidget->layout());
    QLayoutItem* item;
    while ((item = layout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }

    // Fetch and display the map data
    fetchData();
}

