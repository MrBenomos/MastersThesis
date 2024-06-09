#include "genetic_algorithm.h"

CGeneticAlgorithm::CGeneticAlgorithm() : m_rand(0, 99)
{
   m_rand.UseNewNumbers();
}

bool CGeneticAlgorithm::AddVariable(const SPropVar& Variable_)
{
   if (!m_vVariables.empty())
      throw std::string("���������� �������� ����������, ����� ���������� �������(��).");

   return m_mapVariables.insert(std::make_pair(Variable_, static_cast<size_t>(0))).second;
}

size_t CGeneticAlgorithm::AddCondition()
{
   if (m_vVariables.size() != m_mapVariables.size())
      if (m_vVariables.empty())
         InitVectVar();
      else
         throw std::string("���������� ����� ����������, ��� ��� �������������� ������� ����������.");

   TPartCond part(m_vVariables.size(), false);
   m_vSpecified.push_back(std::make_pair(part, part));
   return m_vSpecified.size() - 1;
}

EAddingError CGeneticAlgorithm::AddVariableLeftSideCondition(size_t IndexCondition_, std::string Name_)
{
   auto itVar = m_mapVariables.find(SPropVar(Name_, false));
   if (itVar == m_mapVariables.end())
      return eUnknownVariable;

   if (IndexCondition_ >= m_vSpecified.size())
      return eInvalidIndex;

   TCond& cond = m_vSpecified[IndexCondition_];
   TPartCond& leftPart = cond.first;
   const TPartCond& rightPart = cond.second;

   size_t idx = itVar->second;

   if (idx >= leftPart.size() || idx >= rightPart.size())
      return eInvalidIndex;

   if (leftPart[idx] == true)
      return VariablePresent;

   if (rightPart[idx] == true)
      return OppositeCondition;

   leftPart[idx] = true;

   return eSuccessfully;
}

EAddingError CGeneticAlgorithm::AddVariableRightSideCondition(size_t IndexCondition_, std::string Name_)
{
   auto itVar = m_mapVariables.find(SPropVar(Name_, false));
   if (itVar == m_mapVariables.end())
      return eUnknownVariable;

   if (IndexCondition_ >= m_vSpecified.size())
      return eInvalidIndex;

   TCond& cond = m_vSpecified[IndexCondition_];
   const TPartCond& leftPart = cond.first;
   TPartCond& rightPart = cond.second;

   size_t idx = itVar->second;

   if (idx >= leftPart.size() || idx >= rightPart.size())
      return eInvalidIndex;

   if (rightPart[idx] == true)
      return VariablePresent;

   if (leftPart[idx] == true)
      return OppositeCondition;

   rightPart[idx] = true;

   return eSuccessfully;
}

bool CGeneticAlgorithm::GetVariablesFromString(const std::string& Str_, std::string& StrError_)
{
   std::vector<SPropVar> vVar;
   size_t length = Str_.length();
   for (size_t i = 0; i < length;)
   {
      // ����������� ��� �������
      while (i < length && isspace(Str_.at(i)))
         ++i;

      if (i >= length || Str_.at(i) == '#')
         break;

      // ��������� ��� ����������
      if (!std::isalpha(Str_.at(i)))
      {
         StrError_ = ErrorMessage("��� ���������� ����� ���������� ������ � ��������� �����.", Str_, i);
         return false;
      }

      std::string name;
      for (; i < length; ++i)
      {
         char symb = Str_.at(i);
         if (IllegalSymbol(symb) || isspace(symb))
            break;

         name += symb;
      }

      // ����������� ��� �������
      while (i < length && isspace(Str_.at(i)))
         ++i;

      if (i >= length || Str_.at(i) == '#')
      {
         StrError_ = ErrorMessage("������������� ������, ��������� \'=\'.", Str_);
         return false;
      }

      // �������� ������ '='
      if (Str_.at(i) != '=')
      {
         StrError_ = ErrorMessage("��������� \'=\'.", Str_, i);
         return false;
      }

      ++i;

      // ����������� ��� �������
      while (i < length && isspace(Str_.at(i)))
         ++i;

      if (i >= length || Str_.at(i) == '#')
      {
         StrError_ = ErrorMessage("������������� ������, ��������� �������� ����������.", Str_);
         return false;
      }

      // ��������� ��������
      char symb = Str_.at(i);
      if (symb < '0' || symb > '9')
      {
         StrError_ = ErrorMessage("��������� \'0\' ��� \'1\'.", Str_, i);
         return false;
      }

      // ��������� ����������
      vVar.push_back(SPropVar(name, symb == '0' ? false : true));

      ++i;

      // ����������� ��� ������� �� ','
      while (i < length && isspace(Str_.at(i)))
         ++i;

      // ����������� ','
      if (i < length && Str_.at(i) == ',')
         ++i;

   }

   std::string existing;
   size_t count = 0;
   for (const auto& var : vVar)
   {
      if (!AddVariable(var))
      {
         if (count > 0)
            existing += ", \'" + var.name + '\'';
         else
            existing = '\'' + var.name + '\'';

         ++count;
      }
   }

   if (count > 0)
   {
      if (count == 1)
         StrError_ = ErrorMessage("���������� " + existing + " ��� ����������", Str_);
      else
         StrError_ = ErrorMessage("���������� " + existing + " ��� ����������", Str_);
   }

   return true;
}

bool CGeneticAlgorithm::GetConditionFromString(const std::string& Str_, std::string& StrError_)
{
   bool isLeftPart = true;
   bool isFirstVar = true;
   size_t idx = std::string::npos;
   size_t length = Str_.length();
   for (size_t i = 0; i < length;)
   {
      // ����������� ��� �������
      while (i < length && (isspace(Str_.at(i)) || Str_.at(i) == '$'))
         ++i;

      if (i >= length || Str_.at(i) == '#')
         if ((isLeftPart && !isFirstVar) || (!isLeftPart && isFirstVar))
         {
            StrError_ = ErrorMessage("����������� ������ ����� �������.", Str_);
            if (idx != std::string::npos)
               m_vSpecified.erase(m_vSpecified.begin() + idx);
            return false;
         }
         else
            break;

      // ��������� ���
      if (!std::isalpha(Str_.at(i)))
      {
         StrError_ = ErrorMessage("��� ���������� ����� ���������� ������ � ��������� �����.", Str_, i);
         if (idx != std::string::npos)
            m_vSpecified.erase(m_vSpecified.begin() + idx);
         return false;
      }

      std::string name;
      for (; i < length; ++i)
      {
         char symb = Str_.at(i);
         if (IllegalSymbol(symb) || isspace(symb))
            break;

         name += symb;
      }

      // ��������� ���������� � �������
      EAddingError errorIdx;
      if (isLeftPart)
      {
         if (isFirstVar)
         {
            idx = AddCondition();
            isFirstVar = false;
         }

         errorIdx = AddVariableLeftSideCondition(idx, name);
      }
      else
      {
         if (isFirstVar)
            isFirstVar = false;

         errorIdx = AddVariableRightSideCondition(idx, name);
      }

      switch (errorIdx)
      {
      case eSuccessfully:
         break;
      case eUnknownVariable:
         StrError_ = ErrorMessage("�������������������� ���������� \'" + name + "\'.", Str_);
         if (idx != std::string::npos)
            m_vSpecified.erase(m_vSpecified.begin() + idx);
         return false;
      case VariablePresent:
         break;
      case OppositeCondition:
         StrError_ = ErrorMessage("���������� \'" + name + "\' ��� �������������� � ������ ����� ����� �������.", Str_);
         if (idx != std::string::npos)
            m_vSpecified.erase(m_vSpecified.begin() + idx);
         return false;
      case eInvalidIndex:
         StrError_ = ErrorMessage("���������� � ������������ (������ ����������, ��� �������� �������).", Str_);
         if (idx != std::string::npos)
            m_vSpecified.erase(m_vSpecified.begin() + idx);
         return false;
      default:
         StrError_ = ErrorMessage("����������� ������.", Str_);
         if (idx != std::string::npos)
            m_vSpecified.erase(m_vSpecified.begin() + idx);
         return false;
      }


      // ����������� ��� �������
      while (i < length && isspace(Str_.at(i)))
         ++i;

      if (i >= length || Str_.at(i) == '#')
         if (isLeftPart)
         {
            StrError_ = ErrorMessage("����������� ������ ����� �������.", Str_);
            if (idx != std::string::npos)
               m_vSpecified.erase(m_vSpecified.begin() + idx);
            return false;
         }
         else
            break;

      if (Str_.at(i) == ',')
      {
         ++i;
         continue;
      }

      if (Str_.at(i) == '-')
      {
         if (!isLeftPart)
         {
            StrError_ = ErrorMessage("� ������ ����� ���� ������ ���� �������", Str_, i);
            if (idx != std::string::npos)
               m_vSpecified.erase(m_vSpecified.begin() + idx);
            return false;
         }

         ++i;
         if (i < length)
            if (Str_.at(i) == '>')
            {
               ++i;
               isLeftPart = false;
               isFirstVar = true;
               continue;
            }
            else
            {
               StrError_ = ErrorMessage("��������� \'>\'.", Str_, i);
               if (idx != std::string::npos)
                  m_vSpecified.erase(m_vSpecified.begin() + idx);
               return false;
            }
         else
         {
            StrError_ = ErrorMessage("����������� ������ ����� �������.", Str_);
            if (idx != std::string::npos)
               m_vSpecified.erase(m_vSpecified.begin() + idx);
            return false;
         }
      }

   }

   if ((isLeftPart && !isFirstVar) || (!isLeftPart && isFirstVar))
   {
      StrError_ = ErrorMessage("����������� ������ ����� �������.", Str_);
      if (idx != std::string::npos)
         m_vSpecified.erase(m_vSpecified.begin() + idx);
      return false;
   }

   return true;
}

bool CGeneticAlgorithm::FillDataInFile(const std::string& FileName_, std::string& StrError_)
{
   std::ifstream fileStream(FileName_);

   if (!fileStream.is_open())
   {
      StrError_ = "�� ������� ������� ����: " + FileName_;
      return false;
   }

   Clear();

   bool isVariables = true;
   while (!fileStream.eof()) // ���� ������ ����� �� ������
   {
      std::string line;
      if (!std::getline(fileStream, line))
         continue;

      for (int i = 0; i < line.size(); ++i)
         if (!isspace(line.at(i)))
         {
            if (line.at(i) == '$')
               isVariables = false;

            break;
         }

      if (isVariables)
      {
         if (!GetVariablesFromString(line, StrError_))
         {
            Clear();
            fileStream.close();
            return false;
         }
      }
      else
      {
         if (!GetConditionFromString(line, StrError_))
         {
            Clear();
            fileStream.close();
            return false;
         }
      }

   }

   if (isVariables)
   {
      StrError_ = ErrorMessage("����������� ����������� �����������");
      Clear();
      fileStream.close();
      return false;
   }

   return true;
}

bool CGeneticAlgorithm::Write�onditionIntegrityInFile(const std::string& FileName_, std::string& StrError_) const
{
   std::ofstream fileStream(FileName_);

   if (!fileStream.is_open())
   {
      StrError_ = "�� ������� ������� ����: " + FileName_;
      return false;
   }

   if (!WriteVarsInStream(fileStream, StrError_))
      return false;

   fileStream << "\n$\n";

   return Write�ondsInStream(fileStream, StrError_, m_vSpecified);
}

bool CGeneticAlgorithm::WriteGenerationsInFile(const std::string& FileName_, std::string& StrError_) const
{
   std::ofstream fileStream(FileName_);

   if (!fileStream.is_open())
   {
      StrError_ = "�� ������� ������� ����: " + FileName_;
      return false;
   }

   // ����������
   if (!WriteVarsInStream(fileStream, StrError_))
      return false;

   fileStream << std::endl << '$';

   const size_t sizeGen = m_vGenerations.size();
   for (int i = 0; i < sizeGen; ++i)
   {
      fileStream << std::endl << "# " << std::to_string(i) << std::endl;
      if (!Write�ondsInStream(fileStream, StrError_, m_vGenerations[i]))
      {
         StrError_ += "\n������ " + std::to_string(i) + " �� " + std::to_string(sizeGen);
         return false;
      }
   }

   return true;
}

void CGeneticAlgorithm::Clear()
{
   m_mapVariables.clear();
   m_vVariables.clear();
   m_vSpecified.clear();
}

void CGeneticAlgorithm::InitVectVar()
{
   m_vVariables.clear();

   size_t pos = 0;
   for (auto& var : m_mapVariables)
   {
      m_vVariables.push_back(var.first);
      var.second = pos++;
   }
}

bool CGeneticAlgorithm::IllegalSymbol(char Symbol_) const
{
   const char illegalSymbols[] = { '=', ',', '-','>', '$', '#' };

   for (char illegal : illegalSymbols)
      if (Symbol_ == illegal)
         return true;

   return false;
}

std::string CGeneticAlgorithm::ErrorMessage(const std::string& Message_, const std::string& Line_, size_t Position_) const
{
   std::string strError("������.");

   if (!Message_.empty())
   {
      strError += ' ';
      strError += Message_;
   }

   if (Position_ != std::string::npos && Position_ < Line_.size())
   {
      strError += "\n�������� ������ \'";
      strError += Line_.at(Position_);
      strError += "\' � �������: ";
      strError += std::to_string(Position_ + 1) + '.';
   }

   if (!Line_.empty())
   {
      strError += "\n������:";
      strError += Line_;
   }

   return strError;
}

bool CGeneticAlgorithm::WriteVarsInStream(std::ostream& Stream_, std::string& StrError_) const
{
   if (!m_vVariables.empty())
   {
      Stream_ << m_vVariables[0].name << " = " << m_vVariables[0].value ? '1' : '0';

      for (int i = 1; i < m_mapVariables.size(); ++i)
         Stream_ << ", " << m_vVariables[i].name << " = " << m_vVariables[i].value ? '1' : '0';
   }
   else
   {
      StrError_ = ErrorMessage("��� ����������.");
      return false;
   }

   return true;
}

bool CGeneticAlgorithm::Write�ondsInStream(std::ostream& Stream_, std::string& StrError_, const T�ondIntegrity& Conds_) const
{
   if (!m_vVariables.empty())
   {
      if (!Conds_.empty())
      {
         bool isFirst = true;

         for (const auto& cond : Conds_)
         {
            if (isFirst)
               isFirst = false;
            else
               Stream_ << std::endl;

            const TPartCond& left = cond.first;
            bool isFirst = true;
            for (int i = 0; i < left.size(); ++i)
            {
               if (left[i])
                  if (isFirst)
                  {
                     Stream_ << m_vVariables[i].name;
                     isFirst = false;
                  }
                  else
                     Stream_ << ", " << m_vVariables[i].name;
            }

            Stream_ << " -> ";

            const TPartCond& right = cond.second;
            isFirst = true;
            for (int i = 0; i < right.size(); ++i)
            {
               if (right[i])
                  if (isFirst)
                  {
                     Stream_ << m_vVariables[i].name;
                     isFirst = false;
                  }
                  else
                     Stream_ << ", " << m_vVariables[i].name;
            }
         }
      }
      else
      {
         StrError_ = ErrorMessage("��� �������.");
         return false;
      }
   }
   else
   {
      StrError_ = ErrorMessage("��� ����������.");
      return false;
   }

   return true;
}

void CGeneticAlgorithm::CreateFirstGenerationRandom(size_t Count_)
{
   if (Count_ < 2)
      throw ErrorMessage("���������� ������ ������ ���� ������ 1");

   const size_t sizeCond = m_vSpecified.size();
   const size_t sizeVar = m_vVariables.size();

   m_vGenerations.clear();

   for (size_t iGen = 0; iGen < Count_; ++iGen)
   {
      T�ondIntegrity conds(sizeCond);

      for (size_t iCond = 0; iCond < sizeCond; ++iCond)
      {
         TPartCond left(sizeVar);
         TPartCond right(sizeVar);

         for (size_t iVar = 0; iVar < sizeVar; ++iVar)
         {
            left[iVar] = static_cast<bool>(m_rand.Generate() / 50);
            right[iVar] = static_cast<bool>(m_rand.Generate() / 50);
         }

         conds[iCond] = std::make_pair(left, right);
      }

      m_vGenerations.push_back(conds);
   }
}
