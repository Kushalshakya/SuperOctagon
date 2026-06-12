#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include<iostream>
#include<windows.h>
#include<string>
#include<fstream>
#include<QThread>
#include<QTimer>
#include<QRegularExpression>
#include<vector>
#include<QDebug>
using namespace std;

extern int xCoordinate;
extern int yCoordinate;

void getCursorPosition(int xCoord,int yCoord){
    SetCursorPos(xCoord, yCoord);
}
void click(){
    mouse_event(MOUSEEVENTF_LEFTDOWN, 0,0,0,0);
    mouse_event(MOUSEEVENTF_LEFTUP, 0,0,0,0);
}

LRESULT CALLBACK lowLevelHook(int nCode, WPARAM wParam, LPARAM lParam){
    if (nCode == HC_ACTION) {
        if (wParam == WM_LBUTTONDOWN) {
            POINT cursorPos;
            GetCursorPos(&cursorPos);
            xCoordinate = cursorPos.x;
            yCoordinate = cursorPos.y;

            PostQuitMessage(0);
        }
    }
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

class Macro{
public:
    void tap(int flag, int xCoordinates,int yCoordinates){
        getCursorPosition(xCoordinates, yCoordinates);
        for(int i=0; i<flag; i++){
            click();
        }
    }
    void delay(int delayms){
        Sleep(delayms);
    }
};

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("Macro");
    ui->textEd->setReadOnly(true);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_tapBtn_clicked()
{
    ui->commandInterpreter->insertPlainText("tap --click-1 ");
    ui->commandInterpreter->setFocus();

    QTimer::singleShot(0, this, [=]() {
        HHOOK hHook = SetWindowsHookEx(WH_MOUSE_LL, lowLevelHook, NULL, 0);

        MSG msg;
        while (GetMessage(&msg, NULL, 0, 0) > 0) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);

            if (msg.message == WM_QUIT) {
                break;
            }
        }

        UnhookWindowsHookEx(hHook);

        ui->commandInterpreter->insertPlainText(QString::number(xCoordinate));
        ui->commandInterpreter->insertPlainText(" ");
        ui->commandInterpreter->insertPlainText(QString::number(yCoordinate));
    });
}

void MainWindow::on_delayBtn_clicked()
{
    ui->commandInterpreter->insertPlainText("delay 1000\n");
    ui->commandInterpreter->setFocus();
}


void MainWindow::on_executeBtn_clicked()
{
    Macro M;

    QString extractHTML;
    extractHTML = ui->commandInterpreter->toHtml();
    QRegularExpression regex("<body[^>]*>(.*?)</body>", QRegularExpression::DotMatchesEverythingOption);
    QRegularExpressionMatch match = regex.match(extractHTML);
    if(match.hasMatch()){
        QString bodyContent = match.captured(1).trimmed();
        QRegularExpression pTagRegex("<p[^>]*>(.*?)</p>", QRegularExpression::DotMatchesEverythingOption);
        QRegularExpressionMatchIterator i = pTagRegex.globalMatch(bodyContent);

        vector<QString> pTagContents;
        int pTagCount = 0;

        while(i.hasNext()){
            QRegularExpressionMatch pMatch = i.next();
            QString pContent = pMatch.captured(1);
            pTagContents.push_back(pContent);
            pTagCount++;
        }

        for(int i=0; i<pTagCount; i++){
            QString getCommand;
            getCommand = pTagContents[i];
            QString commandPointer = getCommand.section(" ",0,0);

            if(commandPointer == "tap"){
                QString flag = getCommand.section("--click-",1);

                for(const QString& content: pTagContents){
                    ui->textEd->setText(content);
                }
                M.tap(flag.section(" ",0,0).toInt(), getCommand.section(" ",2,2).toInt(), getCommand.section(" ",3,3).toInt());

            }
            if(commandPointer == "delay"){
                for(const QString& content: pTagContents){
                    ui->textEd->setText(content);
                }
                M.delay(getCommand.section(" ",1).toInt());
            }
        }
    } else {
        ui->textEd->insertPlainText("Cannot run the program. No commands found.\n");
    }
}

void MainWindow::on_searchBtn_clicked()
{
    ui->commandInterpreter->insertPlainText("search ");
    ui->textEd->setFocus();
}

void MainWindow::on_typeBtn_clicked()
{
    ui->commandInterpreter->insertPlainText("Type ");
    ui->textEd->setFocus();
}

