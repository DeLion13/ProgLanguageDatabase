#pragma once

#include <vector>
#include <string>
#include <QString>

#include "optional.h"
#include "language.hpp"
#include "programmers.hpp"
#include "user.h"

using std::string;
using std::vector;

class Storage {
public:
      virtual bool open() = 0;
      virtual bool close() = 0;
      // Languages
      virtual vector<Language> getAllLanguages(void) = 0;
      virtual optional<Language> getLanguageById(int language_id) = 0;
      virtual bool updateLanguage(const Language & language) = 0;
      virtual bool removeLanguage(int language_id) = 0;
      virtual int insertLanguage(const Language & language, QString photo) = 0;
      // Programmer
      virtual vector<Programmer> getAllProgrammer(void) = 0;
      virtual optional<Programmer> getProgrammerById(int lprogrammer_id) = 0;
      virtual bool updateProgrammer(const Programmer & programmer) = 0;
      virtual bool removeProgrammer(int programmer_id) = 0;
      virtual int insertProgrammer(const Programmer & programmer) = 0;

      virtual bool login_pass() = 0;
      virtual vector<Language> getYourLangs(int page, int pagin, QString find) = 0;
      virtual vector<Programmer> getProgrammersForLang(const Language & lang) = 0;
      virtual int insertProgrammerToLang(const Language & lang, const Programmer & programmer) = 0;
      virtual bool removeProgrammerFromLang(const Language & lang, const Programmer & programmer) = 0;
      virtual int returnId() = 0;

      virtual int insertNewUser(QString us_name, QString hash) = 0;
      virtual int getSizeOfStor(QString find) = 0;
      virtual int insertToSQL(vector<Language> & lang) = 0;
};
