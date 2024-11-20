#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QMutex>
#include <QMutexLocker>
#include <QDebug>
#include <QString>

/**
 * @brief A Singleton database to handle everything related to Database Management.
 */
class DatabaseManager
{
public:
    /**
     * @brief The instance of the Singleton.
     * @return The database instance.
     */
    static DatabaseManager& instance();

    /**
     * @brief Get the actual database.
     * @note This should be used only as a last resort.
     * @return The database instance.
     */
    QSqlDatabase& get(){ return this->db; }

    /**
     * @brief Open the database and return the status.
     * @return true if the database was opened successfully, false otherwise.
     */
    bool openDatabase();

    /**
     * @brief Close the database, if it's opened.
     */
    void closeDatabase();

    /**
     * @brief Execute a text based query, and return the QSqlQuery object.
     * @param queryStr The text to execute.
     * @note This is a low level query execution model and, as such, should be use only when actually needed.
     * @return A QSqlQuery with the execution data.
     */
    QSqlQuery executeQuery(const QString &queryStr);

    /**
     * @brief Check if the database is opened.
     * @return true if the database is open, false otherwise.
     */
    bool isOpen() const;

    /**
     * @brief Get the last error from the Database, and clear the error from the instance
     * @return A QString with the last error on the database.
     */
    QString getError();

private:
    // Remove some operators, and ensure nobody calls the constructor and destructor, except the Singleton
    DatabaseManager();
    ~DatabaseManager();
    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;

    QSqlDatabase db;
    QMutex mutex;
    QString error;
};

#endif // DATABASEMANAGER_H
