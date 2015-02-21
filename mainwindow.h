#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
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
    void doLog(QString str);
    void on_buttonUruchom_clicked();
    void on_buttonOtworz_clicked();
    void on_buttonWybierzDXF_clicked();
    void on_actionWyj_cie_triggered();
    void on_actionO_programie_triggered();
    void on_buttonOtworzKolka_clicked();
    void on_buttonOtworzFolder_clicked();

private:
    char* sciezkaDoBazy;
    QString sciezkaDXF;
    int rozmiaryKol[9];
    int iloscKolKateg[9];
    QVector<rekord> wekRekordy[9];
    CDBFile baza;
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
