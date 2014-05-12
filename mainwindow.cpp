#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTimer>
#include "Memory\Memory.h"
#include "CPU\CPUusage.h"
#include "CPU\CPUusagethread.h"
#include "CPU\CPUspeed.h"
#include "CPU\CPUtemp.h"
#include "Graph\Graph.h"
#include "Settings\settings.h"
#include "displaysettings.h"
#include "GPU\GPUtemp.h"
#include "OHM.h"
#include "Keyboard/keyboard.h"
#include "Keyboard/keyboardthread.h"
#include "Macro/Macro.h"
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsItem>
#include <QColorDialog>
#include <QMouseEvent>
#include <QCursor>

//local function prototypes
std::string intToString(int i);
std::string doubleToString(double i);
//prototypes

//variable stubs
std::string cpuUse;
std::string memUsage;
std::string cpuSpeed;
std::string cpuTemp;
std::string gpuTemp;

//classes
Memory mem;
CPUusagethread cpuUthread;
KeyboardThread keyboardThread;
CPUtemp ctemp;
CPUspeed cspeed;
GPUtemp gpu;
Settings set;
OHM ohm;
Graph grapher;
Macro macro;

//strings
QString qcpuUse;
QString qmemUse;
QString qcpuSpeed;
QString qcpuTemp;
QString qgpuTemp;

QList<int> textDisplay;

//graphs
QGraphicsScene* sceneMem;
QGraphicsScene* sceneCPUS;
QGraphicsScene* sceneCPUT;
QGraphicsScene* sceneCPUU;
QGraphicsScene* sceneGPUU;
QGraphicsScene* sceneMacro;

//metric loop counters
int mu;
int cu;
int cs;
int ct;
int gt;

QTimer *timer;
displaysettings *display;
QProcess *OHMpro;
boolean OHMoff = false;
boolean OHMmessage = true;
boolean fromfile = false;

//moving Graphics
int isSelected;
int selected;



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    isSelected = 0;
    selected = 0;

    sceneMem = new QGraphicsScene(0, 0, 371, 271);
    sceneCPUS = new QGraphicsScene(0, 0, 371, 271);
    sceneCPUT = new QGraphicsScene(0, 0, 371, 271);
    sceneCPUU = new QGraphicsScene(0, 0, 371, 271);
    sceneGPUU = new QGraphicsScene(0, 0, 371, 271);
    sceneMacro = new QGraphicsScene(0, 0, 241, 281);

    //if metric is on start thread
    cpuUthread.start();
    keyboardThread.start();
    //end threads
    display = new displaysettings(this);
    ui->setupUi(this);
    //setStyleSheet("background-color: black;");
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateProg()));
    timer->start(500);
    ui->KeyboardTime10->setChecked(1);//check keyboard refresh
    //load display settings//
    QString fileName = "displaysettings.ini";
    QFile mFile(fileName);
    int c = 1;
    if(!mFile.open(QFile::ReadOnly | QFile::Text))
    {
        qDebug() << "Could not read display settings file.\n";
        c = 0;
    }
    else {
    if(c == 1)
        {
            display->fromsettings = 1;
        }
        QTextStream in(&mFile);
        QString mText = in.readLine();
        ui->mainList->setStyleSheet(mText);
        display->style = mText;
        display->button1 = in.readLine();
        display->button2 = in.readLine();
        display->button3 = in.readLine();
        display->fontcolor.setNamedColor(in.readLine());
        display->kHighlight.setNamedColor(in.readLine());
        keyboardThread.setHighlight(display->kHighlight);
        keyboardThread.hfromset = 1;

        mFile.close();
    }

    //load on/off settings
    fileName = "settings.ini";
    QFile mFile2(fileName);
    if(!mFile2.open(QFile::ReadOnly | QFile::Text))
    {
        qDebug() << "Could not read settings file.\n";
    }
    else {

        QTextStream in2(&mFile2);
        set.cpuSpeed = in2.readLine().toULong();
        set.cpuUse = in2.readLine().toULong();
        set.cpuTemp = in2.readLine().toULong();
        set.memUse = in2.readLine().toULong();
        set.gpuTemp = in2.readLine().toULong();
        set.HLcpuUse = in2.readLine().toULong();
        set.HLmemUsage = in2.readLine().toULong();
        set.HLcpuSpeed = in2.readLine().toULong();
        set.HLcpuTemp  = in2.readLine().toULong();
        set.HLgpuTemp = in2.readLine().toULong();
        set.refresh = in2.readLine().toULong();
        set.Keyboard = in2.readLine().toULong();
        fromfile = true;

        mFile.close();
    }

    //open hardware monitor
    //XML editing
    //rewrite entire file emitting any of our settings
    //open config
    QFile OHMconfig("debug/OpenHardwareMonitor/OpenHardwareMonitor.config");
    if(!OHMconfig.open(QFile::ReadWrite | QFile::Text))
    {
       qDebug() << "Could not read OHMconfig file.\n";
    }
    else {
        QTextStream OHMts(&OHMconfig);
        //create temp file
        QFile OHMnewConfig("debug/OpenHardwareMonitor/temp.txt");
        if(!OHMnewConfig.open(QFile::ReadWrite | QFile::Text))
        {
           qDebug() << "Could not read temp file.\n";
        }
        else {
            QTextStream OHMconNew(&OHMnewConfig);
            //put approprite lines into temp
            OHMconNew << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
            OHMconNew << "<configuration>\n";
            OHMconNew << "  <appSettings>\n";
            OHMconNew << "    <add key=\"minTrayMenuItem\" value=\"true\" />\n";
            OHMconNew << "    <add key=\"startMinMenuItem\" value=\"true\" />\n";
            OHMconNew.flush();
            QString line;
            int count = 0;
            while(!OHMts.atEnd()) {
                line = OHMts.readLine() + "\n";
                if ((count > 2) && (line != "    <add key=\"minTrayMenuItem\" value=\"false\" />\n") && (line != "    <add key=\"startMinMenuItem\" value=\"false\" />\n") &&
                        (line != "    <add key=\"minTrayMenuItem\" value=\"true\" />\n") && (line != "    <add key=\"startMinMenuItem\" value=\"true\" />\n")) {
                    OHMconNew << line;
                    OHMconNew.flush();
                }
                count++;
            }
            OHMconfig.remove();
            OHMnewConfig.rename("debug/OpenHardwareMonitor/OpenHardwareMonitor.config");
            OHMnewConfig.close();
            OHMconfig.close();
        }
    }

    OHMpro = new QProcess();
    OHMpro->start("OpenHardwareMonitor/OpenHardwareMonitor.exe");


    QProcess *WMIserver = new QProcess();
    WMIserver->start("WMIserver.exe");

    updateProg();
}

MainWindow::~MainWindow()
{
    OHMpro->close();
    QProcess *cleanup = new QProcess();
    cleanup->start("RefreshNotificationArea.exe");
    delete ui;
}

void MainWindow::updateProg() {
    updateList();

    keyboardThread.setLineColor(display->fontcolor);

    sceneMem->clear();
    sceneCPUS->clear();
    sceneCPUT->clear();
    sceneCPUU->clear();
    sceneGPUU->clear();

    grapher.draw(sceneMem, mem.array,10, mu, display->fontcolor, Qt::red, "Memory Useage", 0, 100);
    ui->MemoryGraphView->setScene(sceneMem);

    grapher.draw(sceneCPUS, cspeed.array,10, cs, display->fontcolor, Qt::red, "CPU Speed", 0, 10);
    ui->CPUSpeedView->setScene(sceneCPUS);


    grapher.draw(sceneCPUT, ctemp.array,10, ct, display->fontcolor, Qt::red, "CPU Temperature", 0, 150);
    ui->CPUTempView->setScene(sceneCPUT);


    grapher.draw(sceneCPUU, cspeed.array,10, cu, display->fontcolor, Qt::red, "CPU Useage", 0, 100);
    ui->CPUUseView->setScene(sceneCPUU);


    grapher.draw(sceneGPUU, gpu.array,10, gt, display->fontcolor, Qt::red, "GPU Temperature", 0, 150);
    ui->GPUTempView->setScene(sceneGPUU);




    sceneMacro->clear();
    macro.draw(sceneMacro, Qt::red, display->fontcolor);
    ui->MacroView->setScene(sceneMacro);



    if(set.Keyboard)
    {
        keyboardThread.draw();
        ui->actionKeyboard_Log->setChecked(1);
        ui->keyboardView->setScene(keyboardThread.scene);
    }
    if(fromfile)
    {
        timer->start(set.refresh);
        switch(set.refresh)
        {
            case 500:
                ui->action1_High->setChecked(1);
                break;
            case 750:
                ui->action2->setChecked(1);
                break;
            case 1000:
                ui->action3_Medium->setChecked(1);
                break;
            case 1250:
                ui->action4->setChecked(1);
                break;
            case 1500:
                ui->action5_Low->setChecked(1);
                break;
        }

        fromfile = false;
    }


        if(set.updated) {
            set.updated = false;
            QString fileName = "settings.ini";

            QFile mFile(fileName);

            if(!mFile.open(QFile::WriteOnly | QFile::Text))
            {
                qDebug() << "Could not write to file.\n";
                return;
            }
            QTextStream out(&mFile);
            out << set.cpuSpeed;
            out << "\n";
            out << set.cpuUse;
            out << "\n";
            out << set.cpuTemp;
            out << "\n";
            out << set.memUse;
            out << "\n";
            out << set.gpuTemp;
            out << "\n";
            out << set.HLcpuUse;
            out << "\n";
            out << set.HLmemUsage;
            out << "\n";
            out << set.HLcpuSpeed;
            out << "\n";
            out << set.HLcpuTemp;
            out << "\n";
            out << set.HLgpuTemp;
            out << "\n";
            out << set.refresh;
            out << "\n";
            out << set.Keyboard;

            mFile.flush();
            mFile.close();
        }

        if (OHMoff) {
            if (OHMmessage) {
                qDebug() << "ohm is off";
                set.cpuSpeed = false;
                set.HLcpuSpeed = false;
                set.HLcpuTemp = false;
                set.cpuCoreTemp = false;
                set.cpuTemp = false;
                QMessageBox::information(
                    this,
                    tr("GiS"),
                    tr("Open Hardware Monitor has closed. This process is needed to provide you with all posible metrics. To get them back you can either:\n1. Restart GiS\n2. Re-open Open Hardware Monitor (Note: Open Hardware Monitor process will not stop when GiS is closed if this is chosen)\nThen, add back the metrics you want.") );
                OHMmessage = false;
            }
        }

        ui->mainList->setStyleSheet(display->style);
}

void MainWindow::updateList() {
    std::string high;
    std::string low;
    ohm.update();
    ui->mainList->clear();
    if (set.collecting) {

        if (set.memUse || set.HLmemUsage) {
            mem.getUsage();
        }
        if (set.gpuTemp || set.HLgpuTemp) {
            gpu.getTemp();
        }
        if (set.cpuSpeed || set.HLcpuSpeed) {
            cspeed.getSpeed(ohm.CPUspeed);
            if (cspeed.cpuSpeed < 1) {
                if (ohm.loaded) {
                    OHMoff = true;
                    ohm.loaded = false;
                    OHMmessage = true;
                }
            }
            else {
                OHMoff = false;
            }
        }
        if (set.cpuTemp || set.HLcpuTemp || set.cpuCoreTemp) {
            ctemp.getTemp(ohm.CPUtemp);
        }
        if (set.cpuTemp || set.HLcpuTemp) {

        }
    }
    /*
     * 0 memory
     * 1 mem highlow
     * 2 cpu use
     * 3 cpu use highlow
     * 4 cpu speed
     * 5 cpu speed highlow
     * 6 cpu temp
     * 7 cpu temp highlow
     * 8 cpu core temps
     * 9 gpu temp
     * 10 gpu temp highlow
     */

    for (int i = 0; i < textDisplay.length(); i++)
    {
        switch (textDisplay.at(i)) {
            case 0:
                mem.array[mu] = mem.memoryUsage;
                if(mu == 9)
                {
                    mu = 0;
                }
                else
                {
                    mu++;
                }
                ui->actionNumeric_Display->setChecked(1);
                memUsage = "Memory Usage: " + intToString(mem.memoryUsage) + "%";
                qmemUse = QString::fromStdString(memUsage);
                ui->mainList->addItem(qmemUse);
                break;
            case 1:
                ui->actionSession_High_Low->setChecked(1);
                high = "Memory Usage High: " + intToString(mem.HmemUsage) + "%";
                low = "Memory Usage Low: " + intToString(mem.LmemUsage) + "%";
                ui->mainList->addItem(QString::fromStdString(high));
                ui->mainList->addItem(QString::fromStdString(low));
                break;
            case 2:
            cpuUthread.array[cu] = cpuUthread.cpuUsage;
            if(cu == 9)
            {
                cu = 0;
            }
            else
            {
                cu++;
            }
                ui->actionNumeric_Display_2->setChecked(1);
                cpuUse = "CPU Usage: " + intToString(cpuUthread.cpuUsage) + "%";
                qcpuUse = QString::fromStdString(cpuUse);
                ui->mainList->addItem(qcpuUse);
                break;
            case 3:
                ui->actionSession_High_Low_2->setChecked(1);
                high = "CPU Usage High: " + intToString(cpuUthread.HcpuUse) + "%";
                low = "CPU Usage Low: " + intToString(cpuUthread.LcpuUse) + "%";
                ui->mainList->addItem(QString::fromStdString(high));
                ui->mainList->addItem(QString::fromStdString(low));
                break;
            case 4:
            cspeed.array[cs] = cspeed.cpuSpeed;
            if(cs == 9)
            {
                cs = 0;
            }
            else
            {
                cs++;
            }
                ui->actionNumeric_Display_3->setChecked(1);
                cpuSpeed = "CPU Speed: " + doubleToString(cspeed.cpuSpeed) + "Ghz";
                qcpuSpeed = QString::fromStdString(cpuSpeed);
                ui->mainList->addItem(qcpuSpeed);
                break;
            case 5:
                ui->actionSession_High_Low_3->setChecked(1);
                high = "CPU Speed High: " + doubleToString(cspeed.HcpuSpeed) + "Ghz";
                low = "CPU Speed Low: " + doubleToString(cspeed.LcpuSpeed) + "Ghz";
                ui->mainList->addItem(QString::fromStdString(high));
                ui->mainList->addItem(QString::fromStdString(low));
                break;
            case 6:
                ctemp.array[ct] = ctemp.cpuHighTemp;
                if(ct == 9)
                {
                    ct = 0;
                }
                else
                {
                    ct++;
                }
                ui->actionNumeric_Display_4->setChecked(1);
                cpuTemp = "CPU Temp: " + intToString(ctemp.cpuHighTemp) + "°C";
                qcpuTemp = QString::fromStdString(cpuTemp);
                ui->mainList->addItem(qcpuTemp);
                break;
            case 7:
                ui->actionSession_High_Low_4->setChecked(1);
                high = "CPU Temp High: " + intToString(ctemp.HcpuTemp) + "°C";
                low = "CPU Temp Low: " + intToString(ctemp.LcpuTemp) + "°C";
                ui->mainList->addItem(QString::fromStdString(high));
                ui->mainList->addItem(QString::fromStdString(low));
                break;
            case 8:
                ui->actionNumeric_Display_6->setChecked(1);
                if (ctemp.cpu0Temp != 0) {
                    cpuTemp = "CPU Core 1 Temp: " + intToString(ctemp.cpu0Temp) + "°C";
                    qcpuTemp = QString::fromStdString(cpuTemp);
                    ui->mainList->addItem(qcpuTemp);
                }
                if (ctemp.cpu1Temp != 0) {
                    cpuTemp = "CPU Core 2 Temp: " + intToString(ctemp.cpu1Temp) + "°C";
                    qcpuTemp = QString::fromStdString(cpuTemp);
                    ui->mainList->addItem(qcpuTemp);
                }
                if (ctemp.cpu2Temp != 0) {
                    cpuTemp = "CPU Core 3 Temp: " + intToString(ctemp.cpu2Temp) + "°C";
                    qcpuTemp = QString::fromStdString(cpuTemp);
                    ui->mainList->addItem(qcpuTemp);
                }
                if (ctemp.cpu3Temp != 0) {
                    cpuTemp = "CPU Core 4 Temp: " + intToString(ctemp.cpu3Temp) + "°C";
                    qcpuTemp = QString::fromStdString(cpuTemp);
                    ui->mainList->addItem(qcpuTemp);
                }
                if (ctemp.cpu4Temp != 0) {
                    cpuTemp = "CPU Core 5 Temp: " + intToString(ctemp.cpu4Temp) + "°C";
                    qcpuTemp = QString::fromStdString(cpuTemp);
                    ui->mainList->addItem(qcpuTemp);
                }
                if (ctemp.cpu5Temp != 0) {
                    cpuTemp = "CPU Core 6 Temp: " + intToString(ctemp.cpu5Temp) + "°C";
                    qcpuTemp = QString::fromStdString(cpuTemp);
                    ui->mainList->addItem(qcpuTemp);
                }
                break;
            case 9:
                gpu.array[gt] = gpu.gputemps[0];
                if(gt == 9)
                {
                    gt = 0;
                }
                else
                {
                    gt++;
                }
                ui->actionNumeric_Display_5->setChecked(1);
                if(gpu.gputemps[0] == 9999)
                {
                    gpuTemp = "GPU not found. Please turn off.";
                    qgpuTemp = QString::fromStdString(gpuTemp);
                    ui->mainList->addItem(qgpuTemp);
                }
                else{
                    int j;
                    for(int i = 0; i < gpu.numgpu; i++)
                    {
                        j = i + 1;
                        gpuTemp = "GPU " + intToString(j) + " Temp: " + intToString(gpu.gputemps[i]) + "°C";
                        qgpuTemp = QString::fromStdString(gpuTemp);
                        ui->mainList->addItem(qgpuTemp);
                    }
                }
                break;
            case 10:
                ui->actionSession_High_Low_5->setChecked(1);
                high = "GPU Temp High: " + intToString(gpu.HgpuTemp) + "°C";
                low = "GPU Temp Low: " + intToString(gpu.LgpuTemp) + "°C";
                ui->mainList->addItem(QString::fromStdString(high));
                ui->mainList->addItem(QString::fromStdString(low));
                break;
        }
    }
}



std::string intToString(int i)
{
    char* temp = new char[20];
    sprintf(temp,"%d",i);
    return temp;
}

std::string doubleToString(double i){
    char* temp = new char[20];
    sprintf(temp,"%.3lf",i);
    return temp;
}

/*memorygraph 0
 * cpuTemp 1
 * cpuUse 2
 * cpuSpeed 3
 * gpuGraph 4
 * keyboard 5
 * macro 6
 * autoclicker 7 */

void MainWindow::mousePressEvent(QMouseEvent *event) {
    if (isSelected == 0) {
        if (event->button() == Qt::LeftButton && ui->MemoryGraphView->geometry().contains(event->pos())) {
            this->setCursor(Qt::PointingHandCursor);
            isSelected = 1;
            selected = 0;
        }
        if (event->button() == Qt::LeftButton && ui->CPUTempView->geometry().contains(event->pos())) {
            this->setCursor(Qt::PointingHandCursor);
            isSelected = 1;
            selected = 1;
        }
        if (event->button() == Qt::LeftButton && ui->CPUUseView->geometry().contains(event->pos())) {
            this->setCursor(Qt::PointingHandCursor);
            isSelected = 1;
            selected = 2;
        }
        if (event->button() == Qt::LeftButton && ui->CPUSpeedView->geometry().contains(event->pos())) {
            this->setCursor(Qt::PointingHandCursor);
            isSelected = 1;
            selected = 3;
        }
        if (event->button() == Qt::LeftButton && ui->GPUTempView->geometry().contains(event->pos())) {
            this->setCursor(Qt::PointingHandCursor);
            isSelected = 1;
            selected = 4;
        }
        if (event->button() == Qt::LeftButton && ui->keyboardView->geometry().contains(event->pos())) {
            this->setCursor(Qt::PointingHandCursor);
            isSelected = 1;
            selected = 5;
        }
        if (event->button() == Qt::LeftButton && ui->MacroView->geometry().contains(event->pos())) {
            this->setCursor(Qt::PointingHandCursor);
            isSelected = 1;
            selected = 6;
        }
    }
    else {
        switch(selected) {
            case 0:
                ui->MemoryGraphView->setGeometry(event->x() - 30,event->y() - 30,371, 271);
                isSelected = 0;
                break;
            case 1:
                ui->CPUTempView->setGeometry(event->x() - 30,event->y() - 30,371, 271);
                isSelected = 0;
                break;
            case 2:
                ui->CPUUseView->setGeometry(event->x() - 30,event->y() - 30,371, 271);
                isSelected = 0;
                break;
            case 3:
                ui->CPUSpeedView->setGeometry(event->x() - 30,event->y() - 30,371, 271);
                isSelected = 0;
                break;
            case 4:
                ui->GPUTempView->setGeometry(event->x() - 30,event->y() - 30,371, 271);
                isSelected = 0;
                break;
            case 5:
                ui->keyboardView->setGeometry(event->x() - 30,event->y() - 30,486, 146);
                isSelected = 0;
                break;
            case 6:
                ui->MacroView->setGeometry(event->x() - 30,event->y() - 30,241, 281);
                isSelected = 0;
                break;
        }
    this->setCursor(Qt::ArrowCursor);
    }
}


void MainWindow::on_actionNumeric_Display_triggered()
{
    if (set.memUse) {
        set.memUse = false;
        textDisplay.removeAt(textDisplay.indexOf(0));
        ui->actionNumeric_Display->setChecked(0);
    }
    else {
        textDisplay.append(0);
        set.memUse = true;
        ui->actionNumeric_Display->setChecked(1);
    }
    set.updated = true;
    updateProg();
}

void MainWindow::on_actionNumeric_Display_2_triggered()
{
    if (set.cpuUse) {
        set.cpuUse = false;
        textDisplay.removeAt(textDisplay.indexOf(2));
        ui->actionNumeric_Display_2->setChecked(0);
    }
    else {
        textDisplay.append(2);
        set.cpuUse = true;
        ui->actionNumeric_Display_2->setChecked(1);
    }
    set.updated = true;
    updateProg();
}

void MainWindow::on_actionNumeric_Display_3_triggered()
{
    if (set.cpuSpeed) {
        set.cpuSpeed = false;
        textDisplay.removeAt(textDisplay.indexOf(4));
        ui->actionNumeric_Display_3->setChecked(0);
    }
    else {
        set.cpuSpeed = true;
        textDisplay.append(4);
        ui->actionNumeric_Display_3->setChecked(1);
    }
    set.updated = true;
    updateProg();
}

void MainWindow::on_actionNumeric_Display_4_triggered()
{
    if (set.cpuTemp) {
        set.cpuTemp = false;
        textDisplay.removeAt(textDisplay.indexOf(6));
        ui->actionNumeric_Display_4->setChecked(0);
    }
    else {
        set.cpuTemp = true;
        textDisplay.append(6);
        ui->actionNumeric_Display_4->setChecked(1);
    }
    set.updated = true;
    updateProg();
}

void MainWindow::on_actionNumeric_Display_6_triggered()  //show indivudal cores
{
    if (set.cpuCoreTemp) {
        set.cpuCoreTemp = false;
        textDisplay.removeAt(textDisplay.indexOf(8));
        ui->actionNumeric_Display_6->setChecked(0);
    }
    else {
        set.cpuCoreTemp = true;
        textDisplay.append(8);
        ui->actionNumeric_Display_6->setChecked(1);
    }
    set.updated = true;
    updateProg();
}

void MainWindow::on_actionNumeric_Display_5_triggered()
{
    if (set.gpuTemp) {
        set.gpuTemp = false;
        textDisplay.removeAt(textDisplay.indexOf(9));
        ui->actionNumeric_Display_5->setChecked(0);
    }
    else {
        set.gpuTemp = true;
        textDisplay.append(9);
        ui->actionNumeric_Display_5->setChecked(1);
    }
    set.updated = true;
    updateProg();
}

void MainWindow::on_actionStart_triggered()
{
    if (!set.collecting) {
        set.collecting = true;
    }
    updateProg();
}

void MainWindow::on_actionStop_triggered()
{
    if (set.collecting) {
        set.collecting = false;
    }
    updateProg();
}

void MainWindow::on_action5_Low_triggered()
{
    set.refresh = 1500;
    set.updated = true;
    ui->action5_Low->setChecked(1);
    ui->action4->setChecked(0);
    ui->action3_Medium->setChecked(0);
    ui->action2->setChecked(0);
    ui->action1_High->setChecked(0);
    timer->start(1500);
}

void MainWindow::on_action4_triggered()
{
    set.refresh = 1250;
    set.updated = true;
    ui->action5_Low->setChecked(0);
    ui->action4->setChecked(1);
    ui->action3_Medium->setChecked(0);
    ui->action2->setChecked(0);
    ui->action1_High->setChecked(0);
    timer->start(1250);
}

void MainWindow::on_action3_Medium_triggered()
{
    set.refresh = 1000;
    set.updated = true;
    ui->action5_Low->setChecked(0);
    ui->action4->setChecked(0);
    ui->action3_Medium->setChecked(1);
    ui->action2->setChecked(0);
    ui->action1_High->setChecked(0);
    timer->start(1000);
}

void MainWindow::on_action2_triggered()
{
    set.refresh = 750;
    set.updated = true;
    ui->action5_Low->setChecked(0);
    ui->action4->setChecked(0);
    ui->action3_Medium->setChecked(0);
    ui->action2->setChecked(1);
    ui->action1_High->setChecked(0);
    timer->start(750);
}

void MainWindow::on_action1_High_triggered()
{
    set.refresh = 500;
    set.updated = true;
    ui->action5_Low->setChecked(0);
    ui->action4->setChecked(0);
    ui->action3_Medium->setChecked(0);
    ui->action2->setChecked(0);
    ui->action1_High->setChecked(1);
    timer->start(500);
}

void MainWindow::on_actionDisplay_Settings_triggered()
{
    display->tempstyle = ui->mainList->styleSheet();
    display->setModal(false);
    display->update();
    display->show();
}

void MainWindow::on_actionSession_High_Low_triggered()
{
    if (set.HLmemUsage) {
        set.HLmemUsage = false;
        textDisplay.removeAt(textDisplay.indexOf(1));
        ui->actionSession_High_Low->setChecked(0);
    }
    else {
        set.HLmemUsage = true;
        textDisplay.append(1);
        ui->actionSession_High_Low->setChecked(1);
    }
    set.updated = true;
    updateProg();
}

void MainWindow::on_actionSession_High_Low_2_triggered()
{
    if (set.HLcpuUse) {
        set.HLcpuUse = false;
        textDisplay.removeAt(textDisplay.indexOf(3));
        ui->actionSession_High_Low_2->setChecked(0);
    }
    else {
        set.HLcpuUse = true;
        textDisplay.append(3);
        ui->actionSession_High_Low_2->setChecked(1);
    }
    set.updated = true;
    updateProg();
}

void MainWindow::on_actionSession_High_Low_3_triggered()
{
    if (set.HLcpuSpeed) {
        set.HLcpuSpeed = false;
        textDisplay.removeAt(textDisplay.indexOf(5));
        ui->actionSession_High_Low_3->setChecked(0);
    }
    else {
        set.HLcpuSpeed = true;
        textDisplay.append(5);
        ui->actionSession_High_Low_3->setChecked(1);
    }
    set.updated = true;
    updateProg();
}

void MainWindow::on_actionSession_High_Low_4_triggered()
{
    if (set.HLcpuTemp) {
        set.HLcpuTemp = false;
        textDisplay.removeAt(textDisplay.indexOf(7));
        ui->actionSession_High_Low_4->setChecked(0);
    }
    else {
        set.HLcpuTemp = true;
        textDisplay.append(7);
        ui->actionSession_High_Low_4->setChecked(1);
    }
    set.updated = true;
    updateProg();
}

void MainWindow::on_actionSession_High_Low_5_triggered()
{
    if (set.HLgpuTemp) {
        set.HLgpuTemp = false;
        textDisplay.removeAt(textDisplay.indexOf(10));
        ui->actionSession_High_Low_5->setChecked(0);
    }
    else {
        set.HLgpuTemp = true;
        textDisplay.append(10);
        ui->actionSession_High_Low_5->setChecked(1);
    }
    set.updated = true;
    updateProg();
}





void MainWindow::on_actionKeyboard_Log_triggered()
{
    if(set.Keyboard){
        ui->actionKeyboard_Log->setChecked(0);
        set.Keyboard = false;
        ui->menuKeyboard->setEnabled(false);
        ui->keyboardView->setVisible(false);
    }
    else {
        ui->actionKeyboard_Log->setChecked(1);
        set.Keyboard = true;
        ui->menuKeyboard->setEnabled(true);
        ui->keyboardView->setVisible(true);
    }
    set.updated = true;
    updateProg();
}

void MainWindow::on_KeyboardTime2_triggered()
{
    keyboardThread.setTime(2000);
    ui->KeyboardTime2->setChecked(1);
    ui->KeyboardTime4->setChecked(0);
    ui->KeyboardTime6->setChecked(0);
    ui->KeyboardTime8->setChecked(0);
    ui->KeyboardTime10->setChecked(0);
}

void MainWindow::on_KeyboardTime4_triggered()
{
    keyboardThread.setTime(4000);
    ui->KeyboardTime2->setChecked(0);
    ui->KeyboardTime4->setChecked(1);
    ui->KeyboardTime6->setChecked(0);
    ui->KeyboardTime8->setChecked(0);
    ui->KeyboardTime10->setChecked(0);
}

void MainWindow::on_KeyboardTime6_triggered()
{
    keyboardThread.setTime(6000);
    ui->KeyboardTime2->setChecked(0);
    ui->KeyboardTime4->setChecked(0);
    ui->KeyboardTime6->setChecked(1);
    ui->KeyboardTime8->setChecked(0);
    ui->KeyboardTime10->setChecked(0);
}

void MainWindow::on_KeyboardTime8_triggered()
{
    keyboardThread.setTime(8000);
    ui->KeyboardTime2->setChecked(0);
    ui->KeyboardTime4->setChecked(0);
    ui->KeyboardTime6->setChecked(0);
    ui->KeyboardTime8->setChecked(1);
    ui->KeyboardTime10->setChecked(0);
}

void MainWindow::on_KeyboardTime10_triggered()
{
    keyboardThread.setTime(10000);
    ui->KeyboardTime2->setChecked(0);
    ui->KeyboardTime4->setChecked(0);
    ui->KeyboardTime6->setChecked(0);
    ui->KeyboardTime8->setChecked(0);
    ui->KeyboardTime10->setChecked(1);
}

void MainWindow::on_actionSet_Highlight_Color_triggered()
{
    display->kHighlight = QColorDialog::getColor(Qt::white, this);
    keyboardThread.setHighlight(display->kHighlight);
    keyboardThread.hfromset = 1;
    display->fromkey = 1;
    display->update();
}
