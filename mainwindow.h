#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QMainWindow>
#include <QVector>

#include "cdbfile.h"
#include "rekord.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
private slots:
    void on_buttonOpenDb_clicked();
    void on_buttonSelectDxfPath_clicked();
    void on_buttonStartCirclesSave_clicked();
    void on_actionExit_triggered();
    void on_actionAbout_triggered();
    void on_buttonOpenDxfFile_clicked();
    void on_buttonOpenDxfPathFolder_clicked();

private:
    Ui::MainWindow *ui;

    enum DatabaseType{
        DBF_DATABASE,
        XLS_DATABASE
    };

    void configureBeforeRun();
    bool readDbfDatabase();
    bool readXlsDatabase();
    bool saveToDxfFile();
    void addEnergyRecord(rekord newRecord);
    void doLog(QString str);

    //char* mDatabasePath;
    QString mDatabasePath;
    QString mDxfPath;
    CDBFile mDbfDatabase;
    DatabaseType mDbType;

    QVector<rekord> mRecordsVec[9];
    double mDiameterSizes[9];
    double mPlotScale;
    bool mSelectedEnergies[9];
};

#endif // MAINWINDOW_H
