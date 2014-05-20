#ifndef MACRO_H
#define MACRO_H

#include <QGraphicsScene>
#include <QtCore>

class Macro : public QThread
{
public:
    Macro();
    struct keys{
        short vk;
        int ascii;
    };

    void draw(QGraphicsScene* scene, QColor background, QColor font);
    void recHit();
    void onHit();
    void offHit();
    void run();
    void record();
    int recording;
    int counter;
    void pressKey(int);
    QString loc;
    void readFile();
    void writeFile(QString);
    void keyPressed(QKeyEvent *e);
    int macro[1000];
    keys keyStorage[104];
};

#endif // MACRO_H
