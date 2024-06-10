#include "genetic_algorithm.h"
#include <map>

CGeneticAlgorithm::CGeneticAlgorithm()
{
   m_rand.UseNewNumbers();
}

bool CGeneticAlgorithm::AddVariable(const SPropVar& Variable_)
{
   if (!m_vVariables.empty())
      throw std::string("Невозможно добавить переменную, после добавления условия(ий).");

   return m_mapVariables.insert(std::make_pair(Variable_, static_cast<size_t>(0))).second;
}

size_t CGeneticAlgorithm::AddCondition()
{
   if (m_vVariables.size() != m_mapVariables.size())
      if (m_vVariables.empty())
         InitVectVar();
      else
         throw std::string("Обнаружена новая переменная, при уже сформированном векторе переменных.");

   m_vSpecified.push_back(TCond(m_vVariables.size(), 0));
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

   if (itVar->second >= cond.size())
      return eInvalidIndex;

   char& var = cond[itVar->second];

   if (var == 1)
      return VariablePresent;

   if (var == 2)
      return OppositeCondition;

   var = 1;

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

   if (itVar->second >= cond.size())
      return eInvalidIndex;

   char& var = cond[itVar->second];

   if (var == 2)
      return VariablePresent;

   if (var == 1)
      return OppositeCondition;

   var = 2;

   return eSuccessfully;
}

bool CGeneticAlgorithm::GetVariablesFromString(const std::string& Str_, std::string& StrError_)
{
   std::vector<SPropVar> vVar;
   size_t length = Str_.length();
   for (size_t i = 0; i < length;)
   {
      // Отбрасываем все пробелы
      while (i < length && isspace(Str_.at(i)))
         ++i;

      if (i >= length || Str_.at(i) == '#')
         break;

      // Считываем имя переменной
      if (!std::isalpha(Str_.at(i)))
      {
         StrError_ = ErrorMessage("Имя переменной может начинаться только с латинской буквы.", Str_, i);
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

      // Отбрасываем все пробелы
      while (i < length && isspace(Str_.at(i)))
         ++i;

      if (i >= length || Str_.at(i) == '#')
      {
         StrError_ = ErrorMessage("Незаконченная строка, ожидалось \'=\'.", Str_);
         return false;
      }

      // Получаем символ '='
      if (Str_.at(i) != '=')
      {
         StrError_ = ErrorMessage("Ожидалось \'=\'.", Str_, i);
         return false;
      }

      ++i;

      // Отбрасываем все пробелы
      while (i < length && isspace(Str_.at(i)))
         ++i;

      if (i >= length || Str_.at(i) == '#')
      {
         StrError_ = ErrorMessage("Незаконченная строка, ожидалось значение переменной.", Str_);
         return false;
      }

      // Считываем значение
      char symb = Str_.at(i);
      if (symb < '0' || symb > '9')
      {
         StrError_ = ErrorMessage("Ожидалось \'0\' или \'1\'.", Str_, i);
         return false;
      }

      // Добавляем переменную
      vVar.push_back(SPropVar(name, symb == '0' ? false : true));

      ++i;

      // Отбрасываем все пробелы до ','
      while (i < length && isspace(Str_.at(i)))
         ++i;

      // Отбрасываем ','
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
         StrError_ = ErrorMessage("Переменная " + existing + " уже существует", Str_);
      else
         StrError_ = ErrorMessage("Переменные " + existing + " уже существуют", Str_);
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
      // Отбрасываем все пробелы
      while (i < length && (isspace(Str_.at(i)) || Str_.at(i) == '$'))
         ++i;

      if (i >= length || Str_.at(i) == '#')
         if ((isLeftPart && !isFirstVar) || (!isLeftPart && isFirstVar))
         {
            StrError_ = ErrorMessage("Отсутствует правая часть условия.", Str_);
            if (idx != std::string::npos)
               m_vSpecified.erase(m_vSpecified.begin() + idx);
            return false;
         }
         else
            break;

      // Считываем имя
      if (!std::isalpha(Str_.at(i)))
      {
         StrError_ = ErrorMessage("Имя переменной может начинаться только с латинской буквы.", Str_, i);
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

      // Добавляем переменную к условию
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
         StrError_ = ErrorMessage("Неинициализированная переменная \'" + name + "\'.", Str_);
         if (idx != std::string::npos)
            m_vSpecified.erase(m_vSpecified.begin() + idx);
         return false;
      case VariablePresent:
         break;
      case OppositeCondition:
         StrError_ = ErrorMessage("Переменная \'" + name + "\' уже использовалась в другой части этого условия.", Str_);
         if (idx != std::string::npos)
            m_vSpecified.erase(m_vSpecified.begin() + idx);
         return false;
      case eInvalidIndex:
         StrError_ = ErrorMessage("Обратитесь к разработчику (ошибка индексации, при создании условия).", Str_);
         if (idx != std::string::npos)
            m_vSpecified.erase(m_vSpecified.begin() + idx);
         return false;
      default:
         StrError_ = ErrorMessage("Неизвестная ошибка.", Str_);
         if (idx != std::string::npos)
            m_vSpecified.erase(m_vSpecified.begin() + idx);
         return false;
      }


      // Отбрасываем все пробелы
      while (i < length && isspace(Str_.at(i)))
         ++i;

      if (i >= length || Str_.at(i) == '#')
         if (isLeftPart)
         {
            StrError_ = ErrorMessage("Отсутствует правая часть условия.", Str_);
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
            StrError_ = ErrorMessage("В строке может быть только одно условие", Str_, i);
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
               StrError_ = ErrorMessage("Ожидалось \'>\'.", Str_, i);
               if (idx != std::string::npos)
                  m_vSpecified.erase(m_vSpecified.begin() + idx);
               return false;
            }
         else
         {
            StrError_ = ErrorMessage("Отсутствует правая часть условия.", Str_);
            if (idx != std::string::npos)
               m_vSpecified.erase(m_vSpecified.begin() + idx);
            return false;
         }
      }

   }

   if ((isLeftPart && !isFirstVar) || (!isLeftPart && isFirstVar))
   {
      StrError_ = ErrorMessage("Отсутствует правая часть условия.", Str_);
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
      StrError_ = "Не удалось открыть файл: " + FileName_;
      return false;
   }

   Clear();

   bool isVariables = true;
   while (!fileStream.eof()) // Цикл чтения строк из потока
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
      StrError_ = ErrorMessage("Отсутствует ограничение целостности");
      Clear();
      fileStream.close();
      return false;
   }

   return true;
}

bool CGeneticAlgorithm::WriteСonditionIntegrityInFile(const std::string& FileName_, std::string& StrError_) const
{
   std::ofstream fileStream(FileName_);

   if (!fileStream.is_open())
   {
      StrError_ = "Не удалось открыть файл: " + FileName_;
      return false;
   }

   if (!WriteVarsInStream(fileStream, StrError_))
      return false;

   fileStream << "\n$\n";

   return WriteСondsInStream(fileStream, StrError_, m_vSpecified);
}

bool CGeneticAlgorithm::WriteGenerationsInFile(const std::string& FileName_, std::string& StrError_) const
{
   std::ofstream fileStream(FileName_);

   if (!fileStream.is_open())
   {
      StrError_ = "Не удалось открыть файл: " + FileName_;
      return false;
   }

   // Переменные
   if (!WriteVarsInStream(fileStream, StrError_))
      return false;

   fileStream << std::endl << '$';

   const size_t sizeGen = m_vGenerations.size();
   for (int i = 0; i < sizeGen; ++i)
   {
      fileStream << std::endl << "# " << std::to_string(i) << "\t = " << FitnessFunction(m_vGenerations[i]) << std::endl;
      if (!WriteСondsInStream(fileStream, StrError_, m_vGenerations[i]))
      {
         StrError_ += "\nОбъект " + std::to_string(i) + " из " + std::to_string(sizeGen);
         return false;
      }
   }

   return true;
}

void CGeneticAlgorithm::Start(unsigned int CountIndividuals_, size_t CountIterations_, bool UseMutation, unsigned int Percent_)
{
   if (UseMutation && Percent_ == 0)
      UseMutation = false;

   // Создание первого поколения
   CreateFirstGenerationRandom(CountIndividuals_);

   for (size_t iGeneration = 0; iGeneration < CountIterations_; ++iGeneration)
   {

      std::vector<TСondIntegrity> children;
      for (size_t iNewIndiv = 0; iNewIndiv < CountIndividuals_ * 2; ++iNewIndiv)
      {
         m_rand.SetBoundaries(0, CountIndividuals_ - 1);

         // Селекция (выбор родителей) (турнирный отбор)
         size_t idxParent1, idxParent2;

         size_t first = m_rand.Generate();
         size_t second = m_rand.Generate();
         while (first == second)
            second = m_rand.Generate();

         idxParent1 = FitnessFunction(m_vGenerations[first]) > FitnessFunction(m_vGenerations[second]) ? first : second;

         first = m_rand.Generate();
         second = m_rand.Generate();
         while (first == second)
            second = m_rand.Generate();

         idxParent2 = FitnessFunction(m_vGenerations[first]) > FitnessFunction(m_vGenerations[second]) ? first : second;

         // Скрещивание
         children.push_back(CrossingByGenes(m_vGenerations[idxParent1], m_vGenerations[idxParent2]));
      }

      // Мутация (отключена для последних итераций)
      if (UseMutation && iGeneration < CountIterations_ - 1)
         for (auto& individ : children)
            Mutation(individ, 0.01 * Percent_);

      // Селекция (полная замена, родителей "убиваем")
      Selection(children, CountIndividuals_);
   }

   // Сортируем в порядке убывания
   SortDescendingOrder();

}

void CGeneticAlgorithm::Clear()
{
   m_mapVariables.clear();
   m_vVariables.clear();
   m_vSpecified.clear();
   m_vGenerations.clear();
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
   std::string strError("Ошибка.");

   if (!Message_.empty())
   {
      strError += ' ';
      strError += Message_;
   }

   if (Position_ != std::string::npos && Position_ < Line_.size())
   {
      strError += "\nВстречен символ \'";
      strError += Line_.at(Position_);
      strError += "\' в позиции: ";
      strError += std::to_string(Position_ + 1) + '.';
   }

   if (!Line_.empty())
   {
      strError += "\nСтрока:";
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
      StrError_ = ErrorMessage("Нет переменных.");
      return false;
   }

   return true;
}

bool CGeneticAlgorithm::WriteСondsInStream(std::ostream& Stream_, std::string& StrError_, const TСondIntegrity& Conds_) const
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

            bool isFirst = true;
            for (int i = 0; i < cond.size(); ++i)
            {
               if (cond[i] == 1)
                  if (isFirst)
                  {
                     Stream_ << m_vVariables[i].name;
                     isFirst = false;
                  }
                  else
                     Stream_ << ", " << m_vVariables[i].name;
            }

            Stream_ << " -> ";

            isFirst = true;
            for (int i = 0; i < cond.size(); ++i)
            {
               if (cond[i] == 2)
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
         StrError_ = ErrorMessage("Нет условий.");
         return false;
      }
   }
   else
   {
      StrError_ = ErrorMessage("Нет переменных.");
      return false;
   }

   return true;
}

void CGeneticAlgorithm::CreateFirstGenerationRandom(size_t Count_)
{
   if (Count_ < 2)
      throw ErrorMessage("Количество особей должно быть больше 1");

   if (m_vVariables.size() < 2)
      throw ErrorMessage("Количество переменных должно быть больше 1");

   const size_t sizeCond = m_vSpecified.size();
   const size_t sizeVar = m_vVariables.size();

   m_vGenerations.clear();
   m_rand.SetBoundaries(0, 2);

   for (size_t iGen = 0; iGen < Count_; ++iGen)
   {
      TСondIntegrity conds(sizeCond);

      for (size_t iCond = 0; iCond < sizeCond; ++iCond)
      {
         TCond cond(sizeVar);

         do
         {
            for (size_t iVar = 0; iVar < sizeVar; ++iVar)
            {
               cond[iVar] = static_cast<char>(m_rand.Generate());
            }
         } while (!IsCorrectCondition(cond));

         conds[iCond] = cond;
      }

      m_vGenerations.push_back(conds);
   }
}

CGeneticAlgorithm::TСondIntegrity CGeneticAlgorithm::CrossingByGenes(const TСondIntegrity& Parent1_, const TСondIntegrity& Parent2_) const
{
   if (Parent1_.size() != Parent2_.size())
      throw ErrorMessage("Разное количество условий целостности у родителей!");

   m_rand.SetBoundaries(1, 2);

   TСondIntegrity child;

   for (size_t iCond = 0; iCond < Parent1_.size(); ++iCond)
   {
      TCond cond(m_vVariables.size());

      for (size_t iVar = 0; iVar < m_vVariables.size(); ++iVar)
         cond[iVar] = m_rand.Generate() == 1 ? Parent1_[iCond][iVar] : Parent2_[iCond][iVar];

      child.push_back(cond);
   }

   return child;
}

CGeneticAlgorithm::TСondIntegrity CGeneticAlgorithm::CrossingByConds(const TСondIntegrity& Parent1_, const TСondIntegrity& Parent2_) const
{
   if (Parent1_.size() != Parent2_.size())
      throw ErrorMessage("Разное количество условий целостности у родителей!");

   m_rand.SetBoundaries(1, 2);

   TСondIntegrity child;

   for (size_t iCond = 0; iCond < Parent1_.size(); ++iCond)
      child.push_back(m_rand.Generate() == 1 ? Parent1_[iCond] : Parent2_[iCond]);

   return child;
}

void CGeneticAlgorithm::Selection(const std::vector<TСondIntegrity>& Individuals_, size_t CountSurvivors_)
{
   if (Individuals_.size() < CountSurvivors_)
      throw ErrorMessage("Количество выживших не должно быть меньше самих особей");

   std::multimap<double, size_t, std::greater<double>> mapIdx;

   for (size_t iIndiv = 0; iIndiv < Individuals_.size(); ++iIndiv)
      mapIdx.insert(std::make_pair(FitnessFunction(Individuals_[iIndiv]), iIndiv));

   m_vGenerations.clear();

   auto itIdx = mapIdx.begin();

   for (size_t i = 0; i < CountSurvivors_; ++i)
      m_vGenerations.push_back(Individuals_[(itIdx++)->second]);
}

void CGeneticAlgorithm::Mutation(TСondIntegrity& Individual_, double Ratio_) const
{
   if (Ratio_ <= 0.)
      return;

   const size_t countConds = m_vSpecified.size();
   const size_t countVar = m_vVariables.size();
   const size_t countMutations = std::max(static_cast<int>(Ratio_ * countConds * countVar), 1);

   for (int i = 0; i < countMutations; ++i)
   {
      size_t idxCond = m_rand.Generate(0, (int)countConds-1);
      size_t idxVar = m_rand.Generate(0, (int)countVar-1);
      char newVar = m_rand.Generate(0, 2);

      Individual_[idxCond][idxVar] = newVar;
   }
}

bool CGeneticAlgorithm::IsCorrectCondition(const TCond& Cond_) const
{
   bool haveLeft = false;
   bool haveRight = false;
   for (const auto& var : Cond_)
   {
      if (var == 1)
         haveLeft = true;
      else if (var == 2)
         haveRight = true;

      if (haveLeft && haveRight)
         return true;
   }

   return haveLeft && haveRight;
}

bool CGeneticAlgorithm::IsTrueCondition(const TCond& Cond_) const
{
   for (int i = 0; i < Cond_.size(); ++i)
   {
      if (Cond_[i] == 1 && !m_vVariables[i].value)
         return true;
      else if (Cond_[i] == 2 && m_vVariables[i].value)
         return true;
   }

   return false;
}

double CGeneticAlgorithm::FitnessFunction(const TСondIntegrity& Conds_) const
{
   size_t countVar = m_vVariables.size();

   double fitness = 0;
   const double maxCondition = 1. / Conds_.size();

   for (int iCond = 0; iCond < Conds_.size(); ++iCond)
   {
      const TCond& cond = Conds_[iCond];
      if (!IsCorrectCondition(cond))
         continue;

      // Считаем сколько отличий
      size_t n = 0;
      for (int iVar = 0; iVar < cond.size(); ++iVar)
      {
         switch (cond[iVar])
         {
         case 0:
            if (m_vSpecified[iCond][iVar] != 0)
               ++n;
            break;
         case 1:
            if (m_vSpecified[iCond][iVar] == 0)
               ++n;
            else if (m_vSpecified[iCond][iVar] == 2)
               n += 2;
            break;
         case 2:
            if (m_vSpecified[iCond][iVar] == 0)
               ++n;
            else if (m_vSpecified[iCond][iVar] == 1)
               n += 2;
            break;
         default:
            throw;
         }
      }

      if (!IsTrueCondition(cond))
         fitness += maxCondition / (((countVar + 1) * 2) + n);
      else
         fitness += maxCondition / (n + 1);
   }

   return fitness;
}

void CGeneticAlgorithm::SortDescendingOrder()
{
   std::multimap<double, TСondIntegrity, std::greater<double>> mapIdx;

   for (const auto& individ : m_vGenerations)
      mapIdx.insert(std::make_pair(FitnessFunction(individ), individ));

   m_vGenerations.clear();

   for (const auto& individ : mapIdx)
      m_vGenerations.push_back(individ.second);

}
