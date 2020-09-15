#pragma once

#include <vector>
#include <string>
// #include <experimental/optional>

#include "optional.h"
// #include <optional>

#include "language.hpp"
#include "programmers.hpp"
#include "storage.h"

using std::string;
using std::vector;

class XmlStorage : public Storage {
  const string dir_name_;

  vector<Language> lang_;
  vector<Programmer> lang2_;

  int getNewId();
  int getNewProgId();

public:
  XmlStorage(const string & dir_name) : dir_name_(dir_name) { }

  bool open();
  bool close();
  // Languages
  vector<Language> getAllLanguages(void);
  optional<Language> getLanguageById(int language_id);
  bool updateLanguage(const Language & language);
  bool removeLanguage(int language_id);
  int insertLanguage(const Language & language, QString photo);
  // Programmer
  vector<Programmer> getAllProgrammer(void);
  optional<Programmer> getProgrammerById(int lprogrammer_id);
  bool updateProgrammer(const Programmer & programmer);
  bool removeProgrammer(int programmer_id);
  int insertProgrammer(const Programmer & programmer);

  bool login_pass();
  vector<Language> getYourLangs(int page, int pagin, QString find);
  vector<Programmer> getProgrammersForLang(const Language & lang);
  int insertProgrammerToLang(const Language & lang, const Programmer & programmer);
  bool removeProgrammerFromLang(const Language & lang, const Programmer & programmer);
  int returnId();
  int insertNewUser(QString us_name, QString hash);

  int getSizeOfStor(QString find);
  int insertToSQL(vector<Language> & lang);
};
