#pragma once
#include <string>
#include <vector>
#include <map>

// ����������������� ���������� (PropositionalVariable) 
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
   eSuccessfully,       // �������� ���������� ����������
   eUnknownVariable,    // ����������� ����������
   VariablePresent,     // ���������� ��� ������������
   OppositeCondition,   // ���������� ������������ � ������ ����� �������
   eInvalidIndex        // �������� ������
};

class CGeneticAlgorithm
{
public:
   CGeneticAlgorithm() = default;
   ~CGeneticAlgorithm() = default;

   // ��������� ����������������� ����������, ���������� true ���� ������� ���������, false �����
   bool AddVariable(const SPropVar& Variable_);
   // ��������� �������, ���������� ������ ����� �������
   size_t AddCondition();
   // ��������� ���������� � ����� ����� �������
   EAddingError AddVariableLeftSideCondition(size_t IndexCondition_, std::string Name_);
   // ��������� ���������� � ������ ����� �������
   EAddingError AddVariableRightSideCondition(size_t IndexCondition_, std::string Name_);
   // �������� ���������� �� ������, ���������� ������� ���������� ��������� ������
   bool GetVariablesFromString(const std::string& Str_, std::string& StrError_);
   // �������� ������� �� ������, ���������� ������� ���������� ��������� ������
   bool GetConditionFromString(const std::string& Str_, std::string& StrError_);
   // �������� ������ �� �����, ���������� ������� ���������� ���������� ������
   bool FillDataInFile(const std::string& FileName_, std::string& StrError_);
   // ���������� ������ � ����
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

   /// ���������: firs - ����� ����� (�� ����������), second - ������ �����
   std::vector<TCond> m_vConditions;
};