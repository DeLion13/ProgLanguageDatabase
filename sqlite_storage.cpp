#include "sqlite_storage.h"
#include "storage.h"
#include "language.hpp"
#include "login.h"

#include <QDebug>
#include <QtSql>
#include <QErrorMessage>

Q_DECLARE_METATYPE(Language);
Q_DECLARE_METATYPE(Programmer);

Language getLanguageFromQuery(const QSqlQuery & query) {
    int id = query.value("id").toInt();
    string name = query.value("name").toString().toStdString();
    string type = query.value("type").toString().toStdString();
    string author = query.value("author").toString().toStdString();
    int user_id = query.value("user_id").toInt();

    QSqlQuery query2;
    query2.prepare("SELECT image FROM images WHERE id == :id");
    query2.bindValue(":id", id);
    query2.exec();

    QString fileName;
    while (query2.next()) {
        fileName = query2.value("image").toString();
    }

    Language l;
    l.id = id;
    l.name = name;
    l.type = type;
    l.author = author;
    l.user_id = user_id;
    l.photo = fileName.toStdString();

    return l;
}

Programmer getProgrammerFromQuery(const QSqlQuery & query) {
    int id = query.value("id").toInt();
    string name = query.value("name").toString().toStdString();
    string stage = query.value("stage").toString().toStdString();
    string date_of_start = query.value("date_of_start").toString().toStdString();

    Programmer l;
    l.id = id;
    l.name = name;
    l.stage = stage;
    l.date_of_start = date_of_start;

    return l;
}

SqliteStorage::SqliteStorage(const string & dir_name) : dir_name_(dir_name) {
    db_ = QSqlDatabase::addDatabase("QSQLITE");

}

bool SqliteStorage::open() {
    QString path = QString::fromStdString(this -> dir_name_);
    db_.setDatabaseName(path);

    bool connected = db_.open();  // open db connection
    if (!connected) {
        return false;
    }

    this -> lang2_ = this -> getAllProgrammer();

    return  true;
}
bool SqliteStorage::close() {
    db_.close();
    return  true;
}
// Languages
vector<Language> SqliteStorage::getAllLanguages(void) {

    QSqlQuery query("SELECT * FROM lang");
    while (query.next())
    {
       int id = query.value("id").toInt();
       string name = query.value("name").toString().toStdString();
       string type = query.value("type").toString().toStdString();
       string author = query.value("author").toString().toStdString();
       int user_id = query.value("user_id").toInt();

       QSqlQuery query2;
       query2.prepare("SELECT image FROM images WHERE id == :id");
       query2.bindValue(":id", id);
       query2.exec();

       QString fileName;
       while (query2.next()) {
           fileName = query2.value("image").toString();
       }

       Language l;
       l.id = id;
       l.name = name;
       l.type = type;
       l.author = author;
       l.user_id = user_id;
       l.photo = fileName.toStdString();

       this -> lang_.push_back(l);
    }

    return  this -> lang_;
}

optional<Language> SqliteStorage::getLanguageById(int language_id) {
    QSqlQuery query;
    query.prepare("SELECT * FROM lang WHERE id = :id");
    query.bindValue(":id", language_id);
    if (!query.exec()) {  // do exec if query is prepared SELECT query
       qDebug() << "get person error:" << query.lastError();
       return  nullopt;
    }
    if (query.next()) {
       Language l = getLanguageFromQuery(query);
       return  l;
    } else {
       qDebug() << " not found ";
       return nullopt;
    }
    return nullopt;
}
bool SqliteStorage::updateLanguage(const Language & language) {
    QSqlQuery query;
    query.prepare("UPDATE lang SET name = :name, type = :type, author = :author WHERE id = :id");
    query.bindValue(":name", QString::fromStdString(language.name));
    query.bindValue(":type", QString::fromStdString(language.type));
    query.bindValue(":author",  QString::fromStdString(language.author));
    query.bindValue(":id", QString::number(language.id));
    if (!query.exec()){
        qDebug() << "updatePerson error:" << query.lastError();
    }
    qDebug() << "4";
    QSqlQuery query2;
    query2.prepare("UPDATE images SET image = :img WHERE id = :id");
    query2.bindValue(":img", language.photo.c_str());
    query2.bindValue(":id", language.id);
    if(!query2.exec()) {
        qDebug() << "Insert image:" << query2.lastError();
    }
    qDebug() << "5";

    for (Language & l : this -> lang_) {
        if (l.id == language.id) {
            l = language;
        }
    }
}
bool SqliteStorage::removeLanguage(int language_id) {

    QSqlQuery query1;
    query1.prepare("DELETE FROM images WHERE id = :id");
    query1.bindValue(":id", language_id);
    query1.exec();

    QSqlQuery query;
    query.prepare("DELETE FROM lang WHERE id = :id");
    query.bindValue(":id", language_id);

    if (!query.exec()) {
        qDebug() << "deletePerson error:" << query.lastError();
    }

    QSqlQuery query2;
    query2.prepare("DELETE FROM links WHERE FK_b = :id");
    query2.bindValue(":id", language_id);

    if (!query2.exec()) {
        qDebug() << "deletePerson error:" << query2.lastError();
    }

    if (query.numRowsAffected() == 0) {
        return  false;
    }

    int index = -1;
    for (int i = 0; i < this -> lang_.size(); i++) {
        if (this -> lang_[i].id == language_id) {
            index = i;
        }
    }
    if (index >= 0) {
        this -> lang_.erase(this -> lang_.begin() + index);
        return true;
    }

}

int SqliteStorage::getNewId() {
    int max_id = 0;
    for (Language & l : this -> lang_) {
        if (l.id >= max_id) {
            max_id = l.id;
        }
    }
    int new_id = max_id;
    return new_id;
}


int SqliteStorage::insertLanguage(const Language & language, QString photo) {

    QSqlQuery query;
    query.prepare("INSERT INTO lang (name, type, author, user_id) VALUES (:name, :type, :author, :user_id)");
    query.bindValue(":name", QString::fromStdString(language.name));
    query.bindValue(":type", QString::fromStdString(language.type));
    query.bindValue(":author",  QString::fromStdString(language.author));
    query.bindValue(":user_id", language.user_id);
    if (!query.exec()) {
        return 0;
    }

    QVariant var = query.lastInsertId();

    this -> lang_ = this -> getAllLanguages();
    int new_id = this -> getNewId();

    Language copy = language;
    copy.id = new_id;

    this -> lang_.push_back(copy);

    QSqlQuery query2;
    query2.prepare("INSERT INTO images (id, image) VALUES (:id, :img)");
    query2.bindValue(":id", new_id);
    query2.bindValue(":img", photo);
    if(!query2.exec()) {
        qDebug() << "Insert image:" << query2.lastError();
    }
    qDebug() << "inserted:!:";
    return var.toInt();
}
// Programmer
vector<Programmer> SqliteStorage::getAllProgrammer(void) {
    vector<Programmer> langs;
    QSqlQuery query("SELECT * FROM prog");
    while (query.next())
    {
        int id = query.value("id").toInt();
        string name = query.value("name").toString().toStdString();
        string stage = query.value("stage").toString().toStdString();
        string date_of_start = query.value("date_of_start").toString().toStdString();

        Programmer l;
        l.id = id;
        l.name = name;
        l.stage = stage;
        l.date_of_start = date_of_start;

        langs.push_back(l);
    }

    return langs;
}
optional<Programmer> SqliteStorage::getProgrammerById(int lprogrammer_id) {
    QSqlQuery query;
    query.prepare("SELECT * FROM prog WHERE id = :id");
    query.bindValue(":id", lprogrammer_id);
    if (!query.exec()) {  // do exec if query is prepared SELECT query
       qDebug() << "get person error:" << query.lastError();
       return  nullopt;
    }
    if (query.next()) {
       Programmer l = getProgrammerFromQuery(query);
       return  l;
    } else {
       qDebug() << " not found ";
       return nullopt;
    }
    return nullopt;
}
bool SqliteStorage::updateProgrammer(const Programmer & programmer) {
    QSqlQuery query;
    query.prepare("UPDATE prog SET name = :name, stage = :stage, date_of_start = :date_of_start WHERE id = :id");
    query.bindValue(":name", QString::fromStdString(programmer.name));
    query.bindValue(":type", QString::fromStdString(programmer.stage));
    query.bindValue(":author",  QString::fromStdString(programmer.date_of_start));
    query.bindValue(":id", programmer.id);
    if (!query.exec()){
        qDebug() << "updatePerson error:" << query.lastError();
    }
    for (Programmer & l : this -> lang2_) {
        if (l.id == programmer.id) {
            l = programmer;
        }
    }
}
bool SqliteStorage::removeProgrammer(int programmer_id) {
    QSqlQuery query;
    query.prepare("DELETE FROM prog WHERE id = :id");
    query.bindValue(":id", programmer_id);
    if (!query.exec()) {
        qDebug() << "deletePerson error:" << query.lastError();
    }

    if (query.numRowsAffected() == 0) {
        return  false;
    }

    int index = -1;
    for (int i = 0; i < this -> lang2_.size(); i++) {
        if (this -> lang2_[i].id == programmer_id) {
            index = i;
        }
    }
    if (index >= 0) {
        this -> lang2_.erase(this -> lang2_.begin() + index);
        return true;
    }
}

int SqliteStorage::insertProgrammer(const Programmer & programmer) {
    QSqlQuery query1;
    query1.prepare("SELECT * FROM prog WHERE name == :name AND stage == :stage AND date_of_start == :date_of_start");
    query1.bindValue(":name", QString::fromStdString(programmer.name));
    query1.bindValue(":stage", QString::fromStdString(programmer.stage));
    query1.bindValue(":date_of_start",  QString::fromStdString(programmer.date_of_start));
    query1.exec();

    int k = 0;
    while (query1.next()) {
        k++;
    }
    QVariant var;
    if (k <= 0) {
        QSqlQuery query;
        query.prepare("INSERT INTO prog (name, stage, date_of_start) VALUES (:name, :stage, :date_of_start)");
        query.bindValue(":name", QString::fromStdString(programmer.name));
        query.bindValue(":stage", QString::fromStdString(programmer.stage));
        query.bindValue(":date_of_start",  QString::fromStdString(programmer.date_of_start));
        if (!query.exec()){
            qDebug() << "Insert Prog error:" << query.lastError();
        }
        var = query.lastInsertId();
        int new_id = this -> getNewId();
        Programmer copy = programmer;
        copy.id = new_id;
        this -> lang2_.push_back(copy);
    } else {
        qDebug() << "PROBLEM!";
    }

    return var.toInt();
}

bool SqliteStorage::login_pass() {
     bool isTrue = false;
     int k;

     do {
        LogIn login(this);
        login.setWindowTitle("LogIn");
        k = login.exec();
        if (k == 0) {
            break;
        }
        this -> user_id = login.getUserId();
        qDebug() << this -> user_id;
     } while (this -> user_id == -1);
     if (k != 0) {
        isTrue = true;
     }
     return isTrue;
}

vector<Language> SqliteStorage::getYourLangs(int page, int pagin, QString find) {
    int off = (page - 1) * pagin;

    vector <Language> lang;
    QSqlQuery query;
    query.prepare("SELECT * FROM lang WHERE user_id == :us_id AND name LIKE ('%' || :search_text || '%') LIMIT :page_size OFFSET :skipped_items");
    query.bindValue(":page_size", pagin);
    query.bindValue(":skipped_items", off);
    query.bindValue(":search_text", find);
    query.bindValue(":us_id", this -> user_id);
    query.exec();

    while (query.next()) {
        if (this -> user_id == query.value("user_id").toInt()) {
           int id = query.value("id").toInt();
           string name = query.value("name").toString().toStdString();
           string type = query.value("type").toString().toStdString();
           string author = query.value("author").toString().toStdString();
           int user_id = query.value("user_id").toInt();

           QSqlQuery query2;
           query2.prepare("SELECT image FROM images WHERE id == :id");
           query2.bindValue(":id", id);
           query2.exec();

           QString fileName;
           while (query2.next()) {
               fileName = query2.value("image").toString();
           }

           Language l;
           l.id = id;
           l.name = name;
           l.type = type;
           l.author = author;
           l.user_id = user_id;
           l.photo = fileName.toStdString();

           lang.push_back(l);
        }
    }

    return lang;
}

vector<Programmer> SqliteStorage::getProgrammersForLang(const Language & lang) {
    vector <Programmer> progs;
    QSqlQuery query;
    query.prepare("SELECT id, name, stage, date_of_start FROM prog,links WHERE prog.id = links.FK_e AND links.FK_b = :id");
    QString q = QString::number(lang.id);
    query.bindValue(":id", q);
    query.exec();
    while (query.next()) {
           int id = query.value("id").toInt();
           string name = query.value("name").toString().toStdString();
           string stage = query.value("stage").toString().toStdString();
           string date_of_start = query.value("date_of_start").toString().toStdString();

           Programmer p;
           p.id = id;
           p.name = name;
           p.stage = stage;
           p.date_of_start = date_of_start;

           progs.push_back(p);
    }

    return progs;
}

int SqliteStorage::insertProgrammerToLang(const Language & lang, const Programmer & programmer) {
    QSqlQuery query;
    query.prepare("INSERT INTO links (FK_b, FK_e) VALUES (:FK_b, :FK_e)");
    query.bindValue(":FK_b", QString::number(lang.id));
    query.bindValue(":FK_e", QString::number(programmer.id));
    if (!query.exec()){
        return 0;
    }

    QVariant var = query.lastInsertId();

    return var.toInt();
}

bool SqliteStorage::removeProgrammerFromLang(const Language & lang, const Programmer & programmer) {
    QSqlQuery query;

    query.prepare("DELETE FROM links WHERE FK_b = :FK_b AND FK_e = :FK_e");
    query.bindValue(":FK_b", lang.id);
    query.bindValue(":FK_e", programmer.id);
    if (!query.exec()) {
        qDebug() << "DeletePerson error:" << query.lastError();
    }

    return true;
}

int SqliteStorage::returnId() {
    return  this -> user_id;
}

int SqliteStorage::insertNewUser(QString us_name, QString hash) {
    QSqlQuery query1;
    query1.prepare("SELECT username FROM users WHERE username == :u_n");
    query1.bindValue(":u_n", us_name);
    if (!query1.exec()) {
        qDebug() << "SelectUsers error:" << query1.lastError();
    }
    int us_id = -1;
    int i = 0;
    while (query1.next()) {
        i++;
    }

    qDebug() << i;

    if (i <= 0) {
        QSqlQuery query;
        query.prepare("INSERT INTO users (username, password_hash) VALUES (:u, :p)");
        query.bindValue(":u", us_name);
        query.bindValue(":p", hash);
        if (!query.exec()) {
            qDebug() << "Insert User error:" << query.lastError();
        }

        QSqlQuery query2;
        query2.prepare("SELECT id FROM users WHERE username == :u_n");
        query2.bindValue(":u_n", us_name);
        if (!query2.exec()) {
            qDebug() << "SelectUsersId error:" << query2.lastError();
        }

        while (query2.next()) {
            us_id = query2.value("id").toInt();
        }
     }
    return us_id;
}

int SqliteStorage::getSizeOfStor(QString find) {
    QSqlQuery q;
    q.prepare("SELECT * FROM lang WHERE user_id == :u_i AND name LIKE ('%' || :search_text || '%')");
    q.bindValue(":u_i", this -> user_id);
    q.bindValue(":search_text", find);
    q.exec();

    int k = 0;

    while (q.next()) {
        k++;
    }

    return k;
}

int SqliteStorage::insertToSQL(vector<Language> & lang) {
    QSqlQuery q;
    q.prepare("INSERT INTO lang (name, type, author, user_id) VALUES (:name, :type, :author, :user_id)");
    for (int i = 0; i < lang.size(); i++) {
        q.bindValue(":name", lang[i].name.c_str());
        q.bindValue(":type", lang[i].type.c_str());
        q.bindValue(":author", lang[i].author.c_str());
        q.bindValue(":user_id", this -> user_id);

        q.exec();
    }
}
