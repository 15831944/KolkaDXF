#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QDir>
#include <QTextStream>
#include <QMessageBox>
#include <QDesktopServices>
#include <QUrl>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::doLog(QString str){
    ui->textEditLog->appendPlainText(str);
}

void MainWindow::on_buttonUruchom_clicked()
{
    for(int i=0; i<=8; ++i)
        wekRekordy[i].clear();
    if( !baza.IsOpen() )
        baza.OpenFile( sciezkaDoBazy );
    baza.LoadFileToMemory();

    //petla po wszystkich rekordach
    for( unsigned int iRekord=0; iRekord<baza.GetRecordCount(); iRekord++ ){
        //dla kazdego rekordu sprawdzam jaka ma energie i dodaje do odpowiedniej kategorii
        baza.GetAtRecord(iRekord+1);
        double ener = *( static_cast<double*>( baza.GetFieldValue(6) ) );
        double wspX = *( static_cast<double*>( baza.GetFieldValue(7) ) );
        double wspY = *( static_cast<double*>( baza.GetFieldValue(8) ) );
        rekord tmp( wspX, wspY, ener);

        if( ui->checkBoxMniejE2->isChecked() && (ener >= 0.0 && ener < 100.0) ) {
            wekRekordy[0].push_back(tmp);
        } else if( ui->checkBoxE2->isChecked() && (ener >= 100.0 && ener < 1000.0) ) {
            wekRekordy[1].push_back(tmp);
        } else if( ui->checkBoxE3->isChecked() && (ener >= 1000.0 && ener < 10000.0) ) {
            wekRekordy[2].push_back(tmp);
        } else if( ui->checkBoxE4->isChecked() && (ener >= 10000.0 && ener < 100000.0) ) {
            wekRekordy[3].push_back(tmp);
        } else if( ui->checkBoxE5->isChecked() && (ener >= 100000.0 && ener < 1000000.0) ) {
            wekRekordy[4].push_back(tmp);
        } else if( ui->checkBoxE6->isChecked() && (ener >= 1000000.0 && ener < 10000000.0) ) {
            wekRekordy[5].push_back(tmp);
        } else if( ui->checkBoxE7->isChecked() && (ener >= 10000000.0 && ener < 100000000.0) ) {
            wekRekordy[6].push_back(tmp);
        } else if( ui->checkBoxE8->isChecked() && (ener >= 100000000.0 && ener < 1000000000.0) ) {
            wekRekordy[7].push_back(tmp);
        } else if( ui->checkBoxE9Wiecej->isChecked() && (ener >= 1000000000.0)  ) {
            wekRekordy[8].push_back(tmp);
        }

    }
    baza.CloseFile();

    //przygotuj srednice kolek
    double srednice[9];
    double skala = ui->spinBoxSkala->value();
    skala/=1000.0;
    srednice[0]=(1.0*skala);
    srednice[1]=(2.0*skala);
    srednice[2]=(3.0*skala);
    srednice[3]=(5.0*skala);
    srednice[4]=(7.5*skala);
    srednice[5]=(10.0*skala);
    srednice[6]=(15.0*skala);
    srednice[7]=(20.0*skala);
    srednice[8]=(30.0*skala);

    //zapisz do pliku
    QFile file( sciezkaDXF );
    if ( file.open(QIODevice::WriteOnly) ) {
        // file opened successfully
        QTextStream strim( &file );
        //poczatek pliku
        strim << 0 << endl << "SECTION" << endl << 2 << endl << "ENTITIES" << endl << 0 << endl;
        for( int i=0; i<=8; ++i )
            for( int j=0; j<wekRekordy[i].size(); ++j ){
                strim << "CIRCLE" << endl << "8" << endl << "KOLKA E" << i+1 << endl << "10" << endl;
                strim << wekRekordy[i].at(j).wspY*1000.0 << endl;
                strim << "20" << endl;
                strim << wekRekordy[i].at(j).wspX*1000.0 << endl;
                strim << "40" << endl;
                strim << srednice[i] << endl;
                strim << "0" << endl;

                //dla energii wiekszej lub rownej od 3
                if( i>=2 ){
                    strim << "text" << endl << "8" << endl << "OPIS E" << i+1 << endl << "10" << endl;
                    strim << (wekRekordy[i].at(j).wspY*1000.0-(srednice[i]/1.5)) << endl;
                    strim << "20" << endl;
                    strim << (wekRekordy[i].at(j).wspX*1000.0-1.5) << endl;
                    strim << "40" << endl;
                    strim << skala*(srednice[i]/srednice[1]) << endl;
                    strim << "1" << endl;
                    QString strEner = QString::number( (wekRekordy[i].at(j).energia), 'E', 0 );
                    strEner.remove("+0");
                    //QString tmpEner( QString( strEner.at(0) ) + "e" + QString::number(strEner.size()-1) );
                    strim << strEner  << endl;
                    strim << "72" << endl << "4.0" << endl << "11" << endl;
                    strim << wekRekordy[i].at(j).wspY*1000.0 << endl;
                    strim << "21" << endl;
                    strim << wekRekordy[i].at(j).wspX*1000.0 << endl;
                    strim << "0" << endl;
                }
            }
        //koniec pliku
        strim << "ENDSEC" << endl << 0 << endl << "EOF" <<endl;
        file.close();

        //wysweitl podsumowanie
        doLog("---------------------------------------------------------");
        doLog("Zapisujê kó³ka z energiami:");
        if( wekRekordy[0].size() )
            doLog( "-> mniejsza od e2: " +QString::number(wekRekordy[0].size()) + " rekordów.");
        for( int kat=1; kat<8; ++kat)
            if( wekRekordy[kat].size() )
                doLog( "-> e" +QString::number(kat+1) +": " +QString::number(wekRekordy[kat].size()) +
                            " rekordów.");
        if( wekRekordy[8].size() )
            doLog( "-> wiêksza lub równa e9: " +QString::number(wekRekordy[8].size()) + " rekordów.");

        //odblokuj przycisk otwierania kolek
        ui->buttonOtworzKolka->setEnabled(true);
        ui->buttonOtworzFolder->setEnabled(true);

        doLog("Kó³ka zosta³y poprawnie zapisane w pliku "+file.fileName() + "\n");
    } else {
        doLog("Nie uda³o siê zapisaæ kó³ek :(");
    }

}

void MainWindow::on_buttonOtworz_clicked()
{
    QString plik = QFileDialog::getOpenFileName(this, "Otwórz bazê danych",QDir::currentPath(),
                                "Pliki dBase III (*.DBF)");
    //jesli wybrano jakis plik
    if( !plik.isEmpty() ){
        sciezkaDoBazy = new char[ plik.size() ];
        for(int i=0; i<plik.size(); i++)
            sciezkaDoBazy[i] = plik.at(i).toAscii();

        //otwieram baze
        if ( baza.OpenFile( sciezkaDoBazy ) ){
            ui->lineEditInputDbPath->setText( plik );
            ui->buttonUruchom->setEnabled(true);
            plik.truncate(plik.size()-4);

            if(sciezkaDXF.isEmpty()){
                sciezkaDXF = QString( plik + "_KOLKA.DXF" );
                ui->lineEditDXFpath->setText( sciezkaDXF );
            }
            doLog( "Baza pomyœlnie otwarta, zawiera "+ QString::number(baza.GetRecordCount()) +" rekordów." );

            //wylaczam przyciski bo nowa sciezka
            ui->buttonOtworzKolka->setDisabled(true);
            ui->buttonOtworzFolder->setDisabled(true);
        } else {
            doLog("Nie uda³o siê otworzyæ bazy danych :(");
        }
    }

}

void MainWindow::on_buttonWybierzDXF_clicked()
{
    sciezkaDXF = QFileDialog::getSaveFileName(this, "Wybierz miejsce zapisu DXF",QDir::currentPath(),
                                "Plik wynikowy z kó³kami (*.DXF)");
    ui->lineEditDXFpath->setText( sciezkaDXF );
}

void MainWindow::on_actionWyj_cie_triggered()
{
    qApp->exit(0);
}

void MainWindow::on_actionO_programie_triggered()
{
    QMessageBox::information(this, "O programie","Twórcy programu KolkaDXF: \nMateusz i Krzysztof Przenzak");
}


void MainWindow::on_buttonOtworzKolka_clicked()
{
    QDesktopServices::openUrl( QUrl( sciezkaDXF ) );
}

void MainWindow::on_buttonOtworzFolder_clicked()
{
    QStringList strListTmp = sciezkaDXF.split("/");
    QString strTmpSciezka = sciezkaDXF;
    strTmpSciezka.truncate( strTmpSciezka.size()-strListTmp.last().size() );
    QDesktopServices::openUrl( QUrl( "file:///" + strTmpSciezka ) );
}
