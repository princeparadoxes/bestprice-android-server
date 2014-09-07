#include "mainwindow.h"
#include "ui_mainwindow.h"

using namespace std;

QSqlQuery* query;
int column=1;
int countmy=0;

//Просто ведение лога, для удобства. s-сообщение которое нужно записать в лог
//запись идет в: файл(чтоб можно было просматривать сообщения любой давности)
//консоль дебага (чтоб если вылетело не лезть в файл, а быстро посмотреть),
//окно вывода (чтоб смотреть все ли идет нормально)
void MainWindow::writeLog (QString s)
{
    //время, чтоб удобнее было искать сообщения
    char buffer[80];
    time_t seconds = time(NULL);
    tm* timeinfo = localtime(&seconds);
    char* format = "%A, %B %d, %Y %I:%M:%S";
    strftime(buffer, 80, format, timeinfo);
    //открытик файла в который писать лог
    ofstream fout("cppstudio.txt",ios_base::app);
    //запись в файл
    fout << buffer;
    fout <<"   "+ s.toStdString()<< endl;
    fout.close();
    //запись в консоль дебага
    qDebug() << s;
    //вывод в окошко
    ui->textinfo->append(QString(buffer) + "   " + s);

}

//создание подключения к базе данных
bool MainWindow::createConnection(){
    //параметры бызы данных
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("localhost");
    db.setDatabaseName("bestprice");
    db.setUserName("root");
    db.setPassword("westside");
    //инициализация запросов
    query= new QSqlQuery(db);
    if (!db.open()) {
        writeLog("Database error occurred");
        return false;
    }
    return true;
}
//не помню для чего мне нужна была эта функция, но пусть лежит
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

//парсинг запроса,вычленяет то что между <параметр> и </параметр>, тоесть значение параметра
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

//обработка того, что пришло от приложения и на основе этого составление запроса к БД
QString MainWindow::quarychoise (QString s)
{
    //перечень параметров которые могут придти от приложения, не очень удобно сделано, но времени искать другой способ не было
    QString mass[] = {"quary:subcategory","quary:tovarsub","quary:category","quary:tovar_prices","quary:nearest_shop","quary:search", "quary:minimalprice"};

    int casei=-1;
    //значения двух параметров
    QString parameter= NULL;
    QString parameter2= NULL;

    QString s1="<";
    QString s2 = s;

    //вычленяем параметр из запроса который пришел от приложения
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
    //переменные долготы и широты
    QString lat;
    QString lon;
    switch(casei)
    {
        //quary:subcategory - запрос на получие подкатерий на основе номера категории
        case 0:
            //здесь и далее подобные конструкции нужны для того, чтоб вытащить параметр из запроса от приложения
            //для этого используеться функция написанная выше
            if(intdata(s2,"category").compare("-1")!=0)
            {
               parameter=intdata(s2,"category");
            }
            writeLog("Server to base: SELECT Tittle FROM Subcategory WHERE Category="+parameter);
            column=1;
            return "SELECT Tittle FROM Subcategory WHERE Category="+parameter;
            break;
        //quary:tovarsub - получение таваров которые присутсвуют в данной категории
        case 1:
            if(intdata(s2,"subcategory").compare("-1")!=0)
            {
               parameter=intdata(s2,"subcategory");
            }
            if(intdata(s2,"limit").compare("-1")!=0)
            {
               parameter2=intdata(s2,"limit");
            }
            //сначала получаем id категори по его названию
            //(приложение-клиент не знает id, только имя поэтому нам нужен этот запрос)
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
        //quary:category  - получение списка категорий
        case 2:
            writeLog("Server to base: SELECT Tittle FROM  Category");
            column=1;
            return ("SELECT Tittle FROM  Category");
            break;
        //quary:tovar_prices - получение цен на этот товар
        case 3:
            if(intdata(s2,"productname").compare("-1")!=0)
            {
               parameter=intdata(s2,"productname");
            }
            //сначала нужно опять узнать id по названию
            writeLog("Server to base: SELECT ID FROM Tovar WHERE Tittle = '"+parameter+"'");
            query->exec("SELECT ID FROM Tovar WHERE Tittle = '"+parameter+"'");
            query->next();
            writeLog("Base to Server: " +query->value(0).toString());
            parameter=query->value(0).toString();
            column=1;
            return "SELECT Price FROM Prices WHERE Tovar="+parameter;
            break;

        //quary:nearest_shop - нахождение ближайшего магазина
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
            //получаем id товара по названию
            writeLog("Server to base: SELECT ID FROM Tovar WHERE Tittle = '"+parameter+"'");
            query->exec("SELECT ID FROM Tovar WHERE Tittle = '"+parameter+"'");
            while (query->next())
            {
                writeLog("Base to Server: " +query->value(0).toString());
                parameter=query->value(0).toString();
            }
            //получаей id магазина(ов) где есть данный товар
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

            //а вот это сложно, это и есть формула нахождения ближайшего магазина
            writeLog("Server to base: SELECT ID, (6371 * acos(cos(radians("+lat+"))*cos(radians(Shir))*cos(radians(Dolg)-radians("+lon+"))+sin(radians("+lat+"))*sin(radians(Shir)))) AS distance FROM Shop WHERE ID IN ("+parameter+") ORDER BY distance");
            query->exec("SELECT ID, (6371 * acos(cos(radians("+lat+"))*cos(radians(Shir))*cos(radians(Dolg)-radians("+lon+"))+sin(radians("+lat+"))*sin(radians(Shir)))) AS distance FROM Shop WHERE ID IN ("+parameter+") ORDER BY distance");
            query->next();
            writeLog("Base to Server: " +query->value(0).toString());
            parameter=query->value(0).toString();
            column=7;
            return "SELECT Shop.Tittle, Shop.Adress,Shop.Dopinfo, (6371 * acos(cos(radians("+lat+"))*cos(radians(Shop.Shir))*cos(radians(Shop.Dolg)-radians("+lon+"))+sin(radians("+lat+"))*sin(radians(Shop.Shir)))) AS distance, Shop.Shir,Shop.Dolg,Prices.Price FROM Shop,Prices WHERE Shop.ID="+parameter+" AND Prices.Tovar="+parameter2;
            break;

        //quary:search - поиск товара
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
        //"quary:minimalprice" - нахождение магазина где цена на данный товар минимальна
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
            //получаем id товара по названию
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

//честно говоря эта функция не нужна, не знаю для чего я ее добавил, это можно было сделать и в основном коде
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

//конструктор класса
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    //запуск формы
    ui->setupUi(this);
}

//десструктор класса
MainWindow::~MainWindow()
{
    //удаление формы
    delete ui;
    server_status=0;
}

//событие при нажатии на start
void MainWindow::on_starting_clicked()
{
    //создаем новый сервер
    tcpServer = new QTcpServer(this);
    //если произошло подключение запуск функции newuser
    connect(tcpServer, SIGNAL(newConnection()), this, SLOT(newuser()));
    //если порт занят или сервер по каким либо иным причинам не запустился то выводим ошибку
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
//событие на кнопку close
void MainWindow::on_stoping_clicked()
{
    //завершаем все что связвно с сервером, если он запущен
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
        //создаем новое соединение и бла бла бла
        writeLog(" Новое соединение");
        QTcpSocket* clientSocket=tcpServer->nextPendingConnection();
        int idusersocs=clientSocket->socketDescriptor();
        SClients[idusersocs]=clientSocket;
        //если все хорошо считыаем данные от клиента и отвечаем ему с помощбю функции slotReadClient
        connect(SClients[idusersocs],SIGNAL(readyRead()),this, SLOT(slotReadClient()));
    }
}

void MainWindow::slotReadClient()
{
    QTcpSocket* clientSocket = (QTcpSocket*)sender();
    int idusersocs=clientSocket->socketDescriptor();

    QTextStream os(clientSocket);
    os.setAutoDetectUnicode(true);
    //считываем и обрабатываем то что нам сказал клиент и на основе этого отправляем ему данные

    QString b = QString(clientSocket->readAll());
    writeLog("Client: " +b);
        query->exec(quarychoise(b));
        while (query->next())
        {
            QString touser;
            for(int i=0;i<column;i++)
            {
                 touser = query->value(i).toString();
//                 writeLog(query->size()+"");
                 os<<touser+"\n";
            }

        }
}
