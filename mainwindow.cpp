#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtWidgets/QFileDialog>
#include <QDir>
#include <QTextStream>
#include <QtWidgets/QMessageBox>
#include <QDesktopServices>
#include <QUrl>
#include <QtCore/qmath.h>

extern "C" {
    #include <libxls/xls.h>
}

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

void MainWindow::configureBeforeRun()
{
    // Clear old record data
    for(int i=0; i<=8; ++i)
        mRecordsVec[i].clear();

    // Save selected energy groups
    mSelectedEnergies[0] = ui->checkBoxMniejE2->isChecked();
    mSelectedEnergies[1] = ui->checkBoxE2->isChecked();
    mSelectedEnergies[2] = ui->checkBoxE3->isChecked();
    mSelectedEnergies[3] = ui->checkBoxE4->isChecked();
    mSelectedEnergies[4] = ui->checkBoxE5->isChecked();
    mSelectedEnergies[5] = ui->checkBoxE6->isChecked();
    mSelectedEnergies[6] = ui->checkBoxE7->isChecked();
    mSelectedEnergies[7] = ui->checkBoxE8->isChecked();
    mSelectedEnergies[8] = ui->checkBoxE9Wiecej->isChecked();

    // Prepare circles diameter size
    mPlotScale = ui->spinBoxSkala->value();
    mPlotScale/=1000.0;
    mDiameterSizes[0]=(1.0*mPlotScale);
    mDiameterSizes[1]=(2.0*mPlotScale);
    mDiameterSizes[2]=(3.0*mPlotScale);
    mDiameterSizes[3]=(5.0*mPlotScale);
    mDiameterSizes[4]=(7.5*mPlotScale);
    mDiameterSizes[5]=(10.0*mPlotScale);
    mDiameterSizes[6]=(15.0*mPlotScale);
    mDiameterSizes[7]=(20.0*mPlotScale);
    mDiameterSizes[8]=(30.0*mPlotScale);
}

bool MainWindow::saveToDxfFile()
{
    QFile file( mDxfPath );
    if ( file.open(QIODevice::WriteOnly) )
    {
        // File opened successfully
        QTextStream strim( &file );
        // File header
        strim << 0 << endl << "SECTION" << endl << 2 << endl << "ENTITIES" << endl << 0 << endl;

        // Basic info for energy < E2
        for( int i=0; i<=1; ++i )
            for( int j=0; j<mRecordsVec[i].size(); ++j ){
                strim << "CIRCLE" << endl << "8" << endl << "KOLKA E" << i+1 << endl << "10" << endl;
                strim << mRecordsVec[i].at(j).wspY*1000.0 << endl;
                strim << "20" << endl;
                strim << mRecordsVec[i].at(j).wspX*1000.0 << endl;
                strim << "40" << endl;
                strim << mDiameterSizes[i] << endl;
                strim << "0" << endl;
            }

        // More info for energy >= E2
        for( int i=2; i<=8; ++i )
            for( int j=0; j<mRecordsVec[i].size(); ++j )
            {
                strim << "CIRCLE" << endl << "8" << endl << "KOLKA E" << i+1 << endl << "10" << endl;
                strim << mRecordsVec[i].at(j).wspY*1000.0 << endl;
                strim << "20" << endl;
                strim << mRecordsVec[i].at(j).wspX*1000.0 << endl;
                strim << "40" << endl;
                strim << mDiameterSizes[i] << endl;
                strim << "0" << endl;

                strim << "text" << endl << "8" << endl << "OPIS E" << i+1 << endl << "10" << endl;
                strim << (mRecordsVec[i].at(j).wspY*1000.0-(mDiameterSizes[i]/1.5)) << endl;
                strim << "20" << endl;
                strim << (mRecordsVec[i].at(j).wspX*1000.0-1.5) << endl;
                strim << "40" << endl;
                strim << mPlotScale*(mDiameterSizes[i]/mDiameterSizes[1]) << endl;
                strim << "1" << endl;
                QString strEner = QString::number( (mRecordsVec[i].at(j).energia), 'E', 0 );
                strEner.remove("+0");
                strim << strEner  << endl;
                strim << "72" << endl << "4.0" << endl << "11" << endl;
                strim << mRecordsVec[i].at(j).wspY*1000.0 << endl;
                strim << "21" << endl;
                strim << mRecordsVec[i].at(j).wspX*1000.0 << endl;
                strim << "0" << endl;
            }

        // File end
        strim << "ENDSEC" << endl << 0 << endl << "EOF" <<endl;
        file.close();

        return true;
    }
    // Could not read the file
    return false;
}

void MainWindow::addEnergyRecord(rekord newRecord)
{
    double ener = newRecord.energia;

    // Below E2
    if( mSelectedEnergies[0] && (ener >= 0.0 && ener < 100.0) )
    {
        mRecordsVec[0].push_back( newRecord );
        return;
    }
    // E2 and higher
    for( int iEGroup = 1; iEGroup < 9; ++ iEGroup)
        if( mSelectedEnergies[iEGroup] && ( ener >= qPow( 10.0,  iEGroup + 1 ) &&
                                            ener <  qPow( 10.0, (iEGroup + 2) ) ))
        {
            mRecordsVec[iEGroup].push_back( newRecord );
            break;
        }
}

bool MainWindow::readDbfDatabase()
{
    if( !mDbfDatabase.IsOpen() )
        if( !mDbfDatabase.OpenFile( const_cast<char*>( mDatabasePath.toStdString().c_str() ) ) )
        {
            doLog(" Nie udało się otworzyć pliku bazy do odczytu. Przerywam.");
            return false;
        }
    mDbfDatabase.LoadFileToMemory();

    if( mDbfDatabase.GetRecordCount() )
    {
        doLog("Wczytano " + QString::number(mDbfDatabase.GetRecordCount()) + " rekordów do przetworzenia.");
    }
    else
    {
        doLog("Nie udało się wczytać rekordów bazy! Przerywam.");
        return false;
    }
    for( unsigned int iRekord=0; iRekord < mDbfDatabase.GetRecordCount(); iRekord++ )
    {
        mDbfDatabase.GetAtRecord(iRekord+1);
        double ener = *( static_cast<double*>( mDbfDatabase.GetFieldValue(6) ) );
        double wspX = *( static_cast<double*>( mDbfDatabase.GetFieldValue(7) ) );
        double wspY = *( static_cast<double*>( mDbfDatabase.GetFieldValue(8) ) );

        rekord tmpRecord( wspX, wspY, ener );
        addEnergyRecord( tmpRecord );
    }
    mDbfDatabase.CloseFile();
    return true;
}

bool MainWindow::readXlsDatabase()
{
    xlsWorkBook* pWorkBook;
    xlsWorkSheet* pWorkSheet;
    struct st_row::st_row_data* row;

    pWorkBook = xls_open( const_cast<char*>( mDatabasePath.toStdString().c_str() ),
                          const_cast<char*>("UTF-8") );

    if (pWorkBook != NULL)
    {
        pWorkSheet = xls_getWorkSheet(pWorkBook,0);
        xls_parseWorkSheet(pWorkSheet);

        for (int t=1; t <= pWorkSheet->rows.lastrow; t++)
        {
            row = &pWorkSheet->rows.row[t];

            rekord tmpRecord( row->cells.cell[6].d,
                              row->cells.cell[7].d,
                              row->cells.cell[5].d );
            addEnergyRecord( tmpRecord );
        }
    } else {
        doLog("Nie udało się wczytać rekordów bazy! Przerywam.");
        return false;
    }
    return true;
}

void MainWindow::doLog(QString str){
    ui->textEditLog->appendPlainText(str);
}

void MainWindow::on_buttonStartCirclesSave_clicked()
{
    // Prepare selected preferences
    configureBeforeRun();

    // Read database
    if( mDbType == DBF_DATABASE )
    {
        if( !readDbfDatabase() )
            return;
    }
    else if( mDbType == XLS_DATABASE )
    {
        if( !readXlsDatabase() )
            return;
    }

    // Save to DXF file
    if( saveToDxfFile() )
    {
        // Show summary
        doLog("---------------------------------------------------------");
        doLog("Zapisuję kółka z energiami:");
        if( mRecordsVec[0].size() )
            doLog( "-> mniejsza od e2: " +QString::number(mRecordsVec[0].size()) + " rekordów.");
        for( int kat=1; kat<8; ++kat)
            if( mRecordsVec[kat].size() )
                doLog( "-> e" +QString::number(kat+1) + ": " +
                       QString::number(mRecordsVec[kat].size()) + " rekordów.");
        if( mRecordsVec[8].size() )
            doLog( "-> większa lub równa e9: " + QString::number(mRecordsVec[8].size()) + " rekordów.");

        // Change buttons status
        ui->buttonOpenDxfFile->setEnabled(true);
        ui->buttonOpenDxfPathFolder->setEnabled(true);

        doLog("Kółka zostały poprawnie zapisane w pliku "+ mDxfPath + "\n");
    }
    else
    {
        doLog("Nie udało się zapisać kółek do pliku DXF :(");
    }
}

void MainWindow::on_buttonOpenDb_clicked()
{
    QString plik = QFileDialog::getOpenFileName(this, "Otwórz bazę danych",QDir::currentPath(),
                                                "Bazy danych (*.DBF *.XLS)");
    // If file was selected
    if( !plik.isEmpty() ){
        mDatabasePath =  plik;
        // DBF Database
        if( plik.endsWith("DBF", Qt::CaseInsensitive) )
        {
            mDbType = DBF_DATABASE;

            // Read database
            if ( mDbfDatabase.OpenFile( const_cast<char*>( mDatabasePath.toStdString().c_str() ) ) ){
                doLog("Baza pomyślnie otwarta, zawiera "+
                      QString::number(mDbfDatabase.GetRecordCount()) +" rekordów.");
            }
            else
            {
                doLog("Nie udało się otworzyć bazy danych :(");
            }
        }
        // XLS Database
        else if( plik.endsWith("XLS", Qt::CaseInsensitive) )
        {
            mDbType = XLS_DATABASE;

            xlsWorkBook* pWorkBook;
            xlsWorkSheet* pWorkSheet;

            pWorkBook = xls_open( const_cast<char*>( mDatabasePath.toStdString().c_str() ),
                                  const_cast<char*>("UTF-8") );

            if (pWorkBook != NULL)
            {
                pWorkSheet = xls_getWorkSheet(pWorkBook,0);
                xls_parseWorkSheet(pWorkSheet);
                doLog("Arkusz pomyślnie otwarty, zawiera "+
                      QString::number(pWorkSheet->rows.lastrow) +" rekordów.");
            }
            else
            {
                doLog("Nie udało się otworzyć bazy danych :(");
            }
        }
        // Other
        else
        {
            doLog("Zły rodzaj bazy danych! Wybierz .DBF lub .XLS!");
            return;
        }

        // Set input database path
        ui->lineEditInputDbPath->setText( plik );

        // Set default DXF path
        plik.truncate(plik.size()-4);
        if(mDxfPath.isEmpty()){
            mDxfPath = QString( plik + "_KOLKA.DXF" );
            ui->lineEditDXFpath->setText( mDxfPath );
        }

        // Change buttons status
        ui->lineEditDXFpath->setEnabled(true);
        ui->buttonSelectDxfPath->setEnabled(true);
        ui->buttonStartCirclesSave->setEnabled(true);
        ui->buttonOpenDxfFile->setDisabled(true);
        ui->buttonOpenDxfPathFolder->setDisabled(true);
    }
}

void MainWindow::on_buttonSelectDxfPath_clicked()
{
    QString nowaSciezka = QFileDialog::getSaveFileName(this, "Wybierz miejsce zapisu DXF",QDir::currentPath(),
                                                       "Plik wynikowy z kółkami (*.DXF)");
    if( nowaSciezka != "" )
        mDxfPath = nowaSciezka;
    ui->lineEditDXFpath->setText( mDxfPath );
}

void MainWindow::on_actionExit_triggered()
{
    qApp->exit(0);
}

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::information(this, "O programie","Twórcy: Mateusz i Krzysztof Przenzak"
                                                 "\nWersja: 2.0"
                                                 "\nData wydania: 26.02.2015");
}

void MainWindow::on_buttonOpenDxfFile_clicked()
{
    QDesktopServices::openUrl( QUrl( mDxfPath ) );
}

void MainWindow::on_buttonOpenDxfPathFolder_clicked()
{
    QStringList strListTmp = mDxfPath.split("/");
    QString strTmpSciezka = mDxfPath;
    strTmpSciezka.truncate( strTmpSciezka.size()-strListTmp.last().size() );
    QDesktopServices::openUrl( QUrl( "file:///" + strTmpSciezka ) );
}
