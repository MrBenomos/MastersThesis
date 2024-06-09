#pragma once
#include <string>
#include <vector>
#include <map>
#include <fstream>

#include "random.h"

// Пропозициональная переменная (PropositionalVariable) 
struct SPropVar
{
   std::string name;
   bool value;

   SPropVar(const std::string& Name_, bool Value_) :
      name(Name_), value(Value_) {};

   bool operator<(const SPropVar& Var_) const
   { return name < Var_.name; }
};

enum EAddingError
{
   eSuccessfully,       // Успешное добавление переменной
   eUnknownVariable,    // Неизвестная переменная
   VariablePresent,     // Переменная уже присутствует
   OppositeCondition,   // Переменная используется в другой части условия
   eInvalidIndex        // Неверный индекс
};

class CGeneticAlgorithm
{
   using TCond = std::vector<char>; // 0 - не использ., 1 - лево, 2 - право
   using TСondIntegrity = std::vector<TCond>;

public:
   CGeneticAlgorithm();
   ~CGeneticAlgorithm() = default;

   // Добавляет пропозициональную переменную, возвращает true если успешно добавлено, false иначе
   bool AddVariable(const SPropVar& Variable_);
   // Добавляет условие, возвращает индекс этого условия
   size_t AddCondition();
   // Добавляет переменную в левую часть условия
   EAddingError AddVariableLeftSideCondition(size_t IndexCondition_, std::string Name_);
   // Добавляет переменную в правую часть условия
   EAddingError AddVariableRightSideCondition(size_t IndexCondition_, std::string Name_);
   // Получает переменные из строки, возвращает признак успешности прочтения строки
   bool GetVariablesFromString(const std::string& Str_, std::string& StrError_);
   // Получает условие из строки, возвращает признак успешности прочтения строки
   bool GetConditionFromString(const std::string& Str_, std::string& StrError_);
   // Получает данные из файла, возвращает признак успешности заполнения данных
   bool FillDataInFile(const std::string& FileName_, std::string& StrError_);
   // Записывает ограничение целостности (исходное) в файл
   bool WriteСonditionIntegrityInFile(const std::string& FileName_, std::string& StrError_) const;
   // Записывает все текущие поколения в файл
   bool WriteGenerationsInFile(const std::string& FileName_, std::string& StrError_) const;

   void Clear();
protected:
   void InitVectVar();
   bool IllegalSymbol(char Symbol_) const;
   std::string ErrorMessage(const std::string& Message_ = "", const std::string& Line_ = "", size_t Position_ = std::string::npos) const;

   bool WriteVarsInStream(std::ostream& Stream_, std::string& StrError_) const;
   bool WriteСondsInStream(std::ostream& Stream_, std::string& StrError_, const TСondIntegrity& Conds_) const;

public:
   void CreateFirstGenerationRandom(size_t Count_);
   bool IsCorrectCondition(const TCond& Cond_);
   bool IsTrueCondition(const TCond& Cond_);
   double FitnessFunction(const TСondIntegrity& Conds_);

private:
   std::map<SPropVar, size_t> m_mapVariables;
   std::vector<SPropVar> m_vVariables;

   /// состояния: firs - левая часть (до импликации), second - правая часть
   TСondIntegrity m_vSpecified; // условие целостности (для финтес ф-ции)

   std::vector<TСondIntegrity> m_vGenerations;

   CRandom m_rand;
};