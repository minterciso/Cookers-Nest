#include "databasemanager.h"
#include <QStandardPaths>
#include <QDir>
#include <QFileInfo>
#include <QFile>

DatabaseManager& DatabaseManager::instance() {
    static DatabaseManager instance;
    return instance;
}

DatabaseManager::DatabaseManager() {
    // Prepare the connection for a SQLite database (for now, on memory)
    if(this->isOpen()==false){
        qDebug() << "[DatabaseManager] Starting SQLite driver...";
        db = QSqlDatabase::addDatabase("QSQLITE");
        // If there's any, clear the error
        this->error.clear();
    }
}

DatabaseManager::~DatabaseManager() {
    closeDatabase();
}

bool DatabaseManager::openDatabase() {
    qDebug() << "[DatabaseManager] Opening the database...";
    QMutexLocker locker(&mutex); // Make this thread safe
    if(db.isOpen()) return true; // Database is already opened

    // The default database location is on the documents folder
    qDebug() << "[DatabaseManager] Checking the database folder...";
    QString databasePath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    databasePath.append("/Cookers-Nest");
    qDebug() << "[DatabaseManager] Database will be stored in '" << databasePath << "' folder...";
    // Check if the directory exist, if it does not, create it
    if(QDir(databasePath).exists()==false)
        QDir().mkdir(databasePath);
    databasePath.append("/default.db");
    qDebug() << "[DatabaseManager] Database path: " << databasePath;

    /* Since this is a SQLite database, we can just copy the template database to the directory database.
       This will work, if and only if, we don't want to preserve the data, so, in the case of updating the
       database, we need something extra. But we'll deal with this when the need arrise.
       QFile::copy() never overwrite the destination, so it is actually safe.
    */
    QFile::copy(":/artifacts/database/CookersNestTemplate.db", databasePath);

    // Now open the SQLite Database
    qDebug() << "[DatabaseManager] Try to open the database...";
    db.setDatabaseName(databasePath);
    if(db.open() == false){
        // If we were unable to open the database, get the error and return false
        this->error.append("Unable to open database: ");
        this->error.append(db.lastError().text());
        qFatal() << "[DatabaseManager] " << this->error;
        return false;
    }
    return true;
}

void DatabaseManager::closeDatabase() {
    qDebug() << "[DatabaseManager] Closing the database...";
    QMutexLocker locker(&mutex); // Prevent race conditions from other threads
    if(db.isOpen())
        db.close();
}

QSqlQuery DatabaseManager::executeQuery(const QString &queryStr) {
    qDebug() << "[DatabaseManager] Executing query '" << queryStr << "'";
    QMutexLocker locker(&mutex); // Prevent race conditions from other threads
    QSqlQuery query(db);
    if(query.exec(queryStr) == false){
        this->error.append("Query failed:");
        this->error.append(query.lastError().text());
        qCritical() << "[DatabaseManager] " << this->error;
    }
    return query;
}

bool DatabaseManager::isOpen() const {
    return db.isOpen();
}

QString DatabaseManager::getError() {
    QString retError = this->error;
    this->error.clear();
    return retError;
}
