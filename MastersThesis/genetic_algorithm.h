#pragma once
#include <string>
#include <vector>
#include <map>
#include <fstream>

#include "random.h"

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
   using TCond = std::vector<char>; // 0 - �� �������., 1 - ����, 2 - �����
   using T�ondIntegrity = std::vector<TCond>;

public:
   CGeneticAlgorithm();
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
   // ���������� ����������� ����������� (��������) � ����
   bool Write�onditionIntegrityInFile(const std::string& FileName_, std::string& StrError_) const;
   // ���������� ��� ������� ��������� � ����
   bool WriteGenerationsInFile(const std::string& FileName_, std::string& StrError_) const;

   void Clear();
protected:
   void InitVectVar();
   bool IllegalSymbol(char Symbol_) const;
   std::string ErrorMessage(const std::string& Message_ = "", const std::string& Line_ = "", size_t Position_ = std::string::npos) const;

   bool WriteVarsInStream(std::ostream& Stream_, std::string& StrError_) const;
   bool Write�ondsInStream(std::ostream& Stream_, std::string& StrError_, const T�ondIntegrity& Conds_) const;

public:
   void CreateFirstGenerationRandom(size_t Count_);
   bool IsCorrectCondition(const TCond& Cond_);
   bool IsTrueCondition(const TCond& Cond_);
   double FitnessFunction(const T�ondIntegrity& Conds_);

private:
   std::map<SPropVar, size_t> m_mapVariables;
   std::vector<SPropVar> m_vVariables;

   /// ���������: firs - ����� ����� (�� ����������), second - ������ �����
   T�ondIntegrity m_vSpecified; // ������� ����������� (��� ������ �-���)

   std::vector<T�ondIntegrity> m_vGenerations;

   CRandom m_rand;
};