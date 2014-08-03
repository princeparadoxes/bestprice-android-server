#include "mainwindow.h"
#include "ui_mainwindow.h"

using namespace std;

QSqlQuery* query;
QSqlQuery* query2;
QSqlQuery* query3;
int column=1;
int countmy=0;

void MainWindow::writeLog (QString s)
{
    char buffer[80];
    time_t seconds = time(NULL);
    tm* timeinfo = localtime(&seconds);
    char* format = "%A, %B %d, %Y %I:%M:%S";
    strftime(buffer, 80, format, timeinfo);
    ofstream fout("cppstudio.txt",ios_base::app);
    fout << buffer;
    fout <<"   "+ s.toStdString()<< endl;
    fout.close();
    qDebug() << s;
    ui->textinfo->append(QString(buffer) + "   " + s);

}

bool MainWindow::createConnection(){
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("localhost");
    db.setDatabaseName("bestprice");
    db.setUserName("root");
    db.setPassword("westside");
    query= new QSqlQuery(db);
    query2= new QSqlQuery(db);
    query3= new QSqlQuery(db);
    if (!db.open()) {
        qDebug() << "Database error occurred";
        return false;
    }
    return true;
}
/*void MainWindow::my()
{QString my1,my2;
    QString ID,Subcategory,Tittle;

    query->exec("SELECT Tovar FROM Prices");
    while (query->next())
    {
        my1=query->value(0).toString();
        if(my1!=my2)
        {
            query2->exec("SELECT ID,Subcategory,Tittle FROM Tovar WHERE ID = '"+my1+"'");
            while (query2->next())
            {
                countmy++;
                ID=query2->value(0).toString();
                Subcategory=query2->value(1).toString();
                Tittle=query2->value(2).toString();
                //INSERT INTO `Tovar2`(`ID`, `Subcategory`, `Tittle`) VALUES (1,2,3)
                query3->exec("INSERT INTO `Tovar2` (`ID`,`Subcategory`,`Tittle`) VALUES('"+ID+"','"+Subcategory+"','"+Tittle+"')");
                query3->next();
                writeLog(QString::number(countmy));
            }
            my2=my1;
        }

    }
    writeLog(QString::number(countmy));
}*/

QString MainWindow::intdata(QString messege, QString param)
{
     int len = messege.length();
     int i,j;
     QString result="-1";
     QString s1="<"+param+">";
     QString s2="</"+param+">";
     int s1len=s1.length();
     if (len!=0)
     {

             i=messege.indexOf(s1);
             j=messege.indexOf(s2);
             if (((i!=-1) && (j!=-1)) && (j-i!=1))
             {
                 result = messege.mid(i+s1len,j-i-s1len);
             }
     }
     return result;
 }

QString MainWindow::quarychoise (QString s)
{
    QString mass[] = {"quary:subcategory","quary:tovarsub","quary:category","quary:tovar_prices","quary:nearest_shop","quary:search", "quary:minimalprice"};
    int casei=-1;
    QString parameter= NULL;
    QString parameter2= NULL;
    QString s1="<";
    QString s2 = s;
    //QList<QString> list;
    if (s.indexOf(s1)!=-1)
    {
        s.resize(s.indexOf(s1));
    }
    for (int i=0; i<mass->length(); i++)
    {
        if (s.compare(mass[i])==0)
        {
            casei=i;
            break;
        }
    }
    QString lat;
    QString lon;
    switch(casei)
    {
        case 0:
            if(intdata(s2,"category").compare("-1")!=0)
            {
               parameter=intdata(s2,"category");
            }
            writeLog("Server to base: SELECT Tittle FROM Subcategory WHERE Category="+parameter);
            column=1;
            return "SELECT Tittle FROM Subcategory WHERE Category="+parameter;
            break;

        case 1:
            if(intdata(s2,"subcategory").compare("-1")!=0)
            {
               parameter=intdata(s2,"subcategory");
            }
            if(intdata(s2,"limit").compare("-1")!=0)
            {
               parameter2=intdata(s2,"limit");
            }
            writeLog("Server to base: SELECT ID FROM Subcategory WHERE Tittle = '"+parameter+"'");
            query->exec("SELECT ID FROM Subcategory WHERE Tittle = '"+parameter+"'");
            while (query->next())
            {
                writeLog("Base to Server: " +query->value(0).toString());
                parameter=query->value(0).toString();
            }
            writeLog("Server to base: SELECT Tittle FROM Tovar WHERE Subcategory="+parameter+ " LIMIT "+parameter2+",20");
            column=1;
            return "SELECT Tittle FROM Tovar WHERE Subcategory="+parameter+ " LIMIT "+parameter2+",20";
            break;
        case 2:
            writeLog("Server to base: SELECT Tittle FROM  Category");
            column=1;
            return ("SELECT Tittle FROM  Category");
            break;

        case 3:
            if(intdata(s2,"productname").compare("-1")!=0)
            {
               parameter=intdata(s2,"productname");
            }
            writeLog("Server to base: SELECT ID FROM Tovar WHERE Tittle = '"+parameter+"'");
            query->exec("SELECT ID FROM Tovar WHERE Tittle = '"+parameter+"'");
            query->next();
            writeLog("Base to Server: " +query->value(0).toString());
            parameter=query->value(0).toString();
            column=1;
            return "SELECT Price FROM Prices WHERE Tovar="+parameter;
            break;
        case 4:
            if(intdata(s2,"productname").compare("-1")!=0)
            {
               parameter=intdata(s2,"productname");
            }
            if(intdata(s2,"lat").compare("-1")!=0)
            {
               lat=intdata(s2,"lat");
            }
            if(intdata(s2,"lon").compare("-1")!=0)
            {
               lon=intdata(s2,"lon");
            }
            writeLog("Server to base: SELECT ID FROM Tovar WHERE Tittle = '"+parameter+"'");
            query->exec("SELECT ID FROM Tovar WHERE Tittle = '"+parameter+"'");
            while (query->next())
            {
                writeLog("Base to Server: " +query->value(0).toString());
                parameter=query->value(0).toString();
            }
            writeLog("Server to base: SELECT Shop FROM Prices WHERE Tovar = '"+parameter+"'");
            query->exec("SELECT Shop FROM Prices WHERE Tovar = '"+parameter+"'");
            parameter2=parameter;
            parameter.clear();
            while (query->next())
            {
                writeLog("Base to Server: " +query->value(0).toString());
                parameter=parameter + query->value(0).toString()+ ",";
            }
            parameter.resize(parameter.size()-1);
            writeLog("Server to base: SELECT ID, (6371 * acos(cos(radians("+lat+"))*cos(radians(Shir))*cos(radians(Dolg)-radians("+lon+"))+sin(radians("+lat+"))*sin(radians(Shir)))) AS distance FROM Shop WHERE ID IN ("+parameter+") ORDER BY distance");
            query->exec("SELECT ID, (6371 * acos(cos(radians("+lat+"))*cos(radians(Shir))*cos(radians(Dolg)-radians("+lon+"))+sin(radians("+lat+"))*sin(radians(Shir)))) AS distance FROM Shop WHERE ID IN ("+parameter+") ORDER BY distance");
            query->next();
            writeLog("Base to Server: " +query->value(0).toString());
            parameter=query->value(0).toString();
            column=7;
            return "SELECT Shop.Tittle, Shop.Adress,Shop.Dopinfo, (6371 * acos(cos(radians("+lat+"))*cos(radians(Shop.Shir))*cos(radians(Shop.Dolg)-radians("+lon+"))+sin(radians("+lat+"))*sin(radians(Shop.Shir)))) AS distance, Shop.Shir,Shop.Dolg,Prices.Price FROM Shop,Prices WHERE Shop.ID="+parameter+" AND Prices.Tovar="+parameter2;
            break;
        case 5:
            if(intdata(s2,"search").compare("-1")!=0)
            {
               parameter=intdata(s2,"search");
            }
            if(intdata(s2,"limit").compare("-1")!=0)
            {
               parameter2=intdata(s2,"limit");
            }
            writeLog("Server to base: SELECT * FROM  `Tovar` WHERE Tittle LIKE  '%"+parameter+"%' LIMIT "+parameter2+", 20");
            column=1;
            return "SELECT Tittle FROM  `Tovar` WHERE Tittle LIKE  '%"+parameter+"%' LIMIT "+parameter2+", 20";
            break;
        case 6:
            if(intdata(s2,"productname").compare("-1")!=0)
            {
               parameter=intdata(s2,"productname");
            }
            if(intdata(s2,"lat").compare("-1")!=0)
            {
               lat=intdata(s2,"lat");
            }
            if(intdata(s2,"lon").compare("-1")!=0)
            {
               lon=intdata(s2,"lon");
            }
            writeLog("Server to base: SELECT ID FROM Tovar WHERE Tittle = '"+parameter+"'");
            query->exec("SELECT ID FROM Tovar WHERE Tittle = '"+parameter+"'");
            while (query->next())
            {
                writeLog("Base to Server: " +query->value(0).toString());
                parameter=query->value(0).toString();
            }
            writeLog("Server to base: SELECT Shop FROM Prices WHERE Tovar = '"+parameter+"'");
            query->exec("SELECT Shop FROM Prices WHERE Tovar = '"+parameter+"' ORDER BY Price");
            parameter2=parameter;
            parameter.clear();
            query->next();
            parameter=query->value(0).toString();
            column=7;
            writeLog("Server to base: SELECT Shop.Tittle, Shop.Adress,Shop.Dopinfo, (6371 * acos(cos(radians("+lat+"))*cos(radians(Shop.Shir))*cos(radians(Shop.Dolg)-radians("+lon+"))+sin(radians("+lat+"))*sin(radians(Shop.Shir)))) AS distance, Shop.Shir,Shop.Dolg,Prices.Price FROM Shop,Prices WHERE Shop.ID="+parameter+"AND Prices.Tovar="+parameter2+" ORDER BY Prices.Price");
            return "SELECT Shop.Tittle, Shop.Adress,Shop.Dopinfo, (6371 * acos(cos(radians("+lat+"))*cos(radians(Shop.Shir))*cos(radians(Shop.Dolg)-radians("+lon+"))+sin(radians("+lat+"))*sin(radians(Shop.Shir)))) AS distance, Shop.Shir,Shop.Dolg,Prices.Price FROM Shop,Prices WHERE Shop.ID="+parameter+" AND Prices.Tovar="+parameter2;
            break;
        default:
            break;
    }
}

void MainWindow::base(QString userQuary)
{
    if (!createConnection())
    {
        qDebug() << "Not connected!";
        ui->textinfo->append("Not connected!");
       // writeLog("MySQL Base Not connected!");
    }
    else
    {
        qDebug() << "Connected!";
        ui->textinfo->append("Connected!");

    }
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
    server_status=0;
}

void MainWindow::on_starting_clicked()
{
    tcpServer = new QTcpServer(this);
    connect(tcpServer, SIGNAL(newConnection()), this, SLOT(newuser()));
    if (!tcpServer->listen(QHostAddress::Any, 33333) && server_status==0) {
        qDebug() <<  QObject::tr("Unable to start the server: %1.").arg(tcpServer->errorString());
        ui->textinfo->append(tcpServer->errorString());
    } else {
        server_status=1;
        qDebug() << tcpServer->isListening() << "TCPSocket listen on port";
        writeLog("Сервер запущен");
        if (!createConnection())
        {
             writeLog("Не удалось соедениться с базой MySQL ");
        }
        else
        {
            writeLog("Соединение с базой MySQL установленно");
        }
        //my();
    }
}

void MainWindow::on_stoping_clicked()
{
    if(server_status==1){
        foreach(int i,SClients.keys()){
            QTextStream os(SClients[i]);
            os.setAutoDetectUnicode(true);
            os << QDateTime::currentDateTime().toString() << "\n";
            SClients[i]->close();
            SClients.remove(i);
        }
        tcpServer->close();
        writeLog("Сервер остановлен");
        server_status=0;
    }
}


void MainWindow::newuser()
{
    if(server_status==1){
        writeLog(" Новое соединение");
        QTcpSocket* clientSocket=tcpServer->nextPendingConnection();
        int idusersocs=clientSocket->socketDescriptor();
        SClients[idusersocs]=clientSocket;
        connect(SClients[idusersocs],SIGNAL(readyRead()),this, SLOT(slotReadClient()));
    }
}

void MainWindow::slotReadClient()
{
    QTcpSocket* clientSocket = (QTcpSocket*)sender();
    int idusersocs=clientSocket->socketDescriptor();

    QTextStream os(clientSocket);
    os.setAutoDetectUnicode(true);
    QString b = QString(clientSocket->readAll());
    writeLog("Client: " +b);
        query->exec(quarychoise(b));//"SELECT Tittle FROM  Category");
        while (query->next())
        {
            QString touser;
            for(int i=0;i<column;i++)
            {
                 touser = query->value(i).toString();
                 writeLog("Server to user: "  +touser);
                 os<<touser+"\n";
            }

        }
        //os<<"end\n";
   // }

    // Если нужно закрыть сокет
    //clientSocket->close();
    //SClients.remove(idusersocs);
}
