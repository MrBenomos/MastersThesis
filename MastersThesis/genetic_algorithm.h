#pragma once
#include <string>
#include <vector>
#include <map>

// ѕропозициональна€ переменна€ (PropositionalVariable) 
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
   eSuccessfully,       // ”спешное добавление переменной
   eUnknownVariable,    // Ќеизвестна€ переменна€
   VariablePresent,     // ѕеременна€ уже присутствует
   OppositeCondition,   // ѕеременна€ используетс€ в другой части услови€
   eInvalidIndex        // Ќеверный индекс
};

class CGeneticAlgorithm
{
public:
   CGeneticAlgorithm() = default;
   ~CGeneticAlgorithm() = default;

   // ƒобавл€ет пропозициональную переменную, возвращает true если успешно добавлено, false иначе
   bool AddVariable(const SPropVar& Variable_);
   // ƒобавл€ет условие, возвращает индекс этого услови€
   size_t AddCondition();
   // ƒобавл€ет переменную в левую часть услови€
   EAddingError AddVariableLeftSideCondition(size_t IndexCondition_, std::string Name_);
   // ƒобавл€ет переменную в правую часть услови€
   EAddingError AddVariableRightSideCondition(size_t IndexCondition_, std::string Name_);
   // ѕолучает переменные из строки, возвращает признак успешности прочтени€ строки
   bool GetVariablesFromString(const std::string& Str_, std::string& StrError_);
   // ѕолучает условие из строки, возвращает признак успешности прочтени€ строки
   bool GetConditionFromString(const std::string& Str_, std::string& StrError_);
   // ѕолучает данные из файла, возвращает признак успешности заполнени€ данных
   bool FillDataInFile(const std::string& FileName_, std::string& StrError_);
   // «аписывает данные в файл
   bool WriteDataInFile(const std::string& FileName_, std::string& StrError_) const;

   void Clear();
private:
   void InitVectVar();
   bool IllegalSymbol(char Symbol_) const;
   std::string ErrorMessage(const std::string& Message_ = "", const std::string& Line_ = "", size_t Position_ = std::string::npos) const;

private:
   std::map<SPropVar, size_t> m_mapVariables;
   std::vector<SPropVar> m_vVariables;

   using TPartCond = std::vector<bool>;
   using TCond = std::pair<TPartCond, TPartCond>;

   /// состо€ни€: firs - лева€ часть (до импликации), second - права€ часть
   std::vector<TCond> m_vConditions;
};