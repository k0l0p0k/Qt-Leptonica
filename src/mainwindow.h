#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDebug>
#include <QDragEnterEvent>
#include <QFileDialog>
#include <QFileSystemWatcher>
#include <QGraphicsItem>
#include <QMainWindow>
#include <QMessageBox>
#include <QMimeData>
#include <QSettings>

#include <leptonica/allheaders.h>
#include "ui_mainwindow.h"

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QGuiApplication>
#endif

class MainWindow : public QMainWindow, public Ui::MainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    static const char *qString2Char(QString string);
    void openImage(const QString& imageFileName);

public slots:
    QImage PixToQImage(PIX *pixs);

protected:
    QGraphicsScene *imageScene;
    QGraphicsItem *imageItem;
    bool modified;
    void dragEnterEvent(QDragEnterEvent* event);
    void dropEvent(QDropEvent* event);
    void closeEvent(QCloseEvent* event);

private:
    QFileSystemWatcher *fileWatcher;
    void setFileWatcher(const QString & fileName);
    void readSettings(bool init);
    void writeSettings();
    enum { MaxRecentFiles = 8 };

private slots:
    void slotfileChanged(const QString& fileName);
};

#endif // MAINWINDOW_H
