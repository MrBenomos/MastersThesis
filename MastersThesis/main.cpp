#include <iostream>
#include <string>

#include "windows.h"
#include "genetic_algorithm.h"

inline void GetFileName(std::string& FileName_)
{
   std::cout << "Введте имя файла (путь): ";
   std::cin >> FileName_;
}

void showMenu()
{
   std::cout << "Меню:" << std::endl;
   std::cout << "1. Считать данные из файла" << std::endl;
   std::cout << "2. Запустить алгоритм" << std::endl;
   std::cout << "3. Записать все полученные поколения в файл" << std::endl;
   std::cout << "4. Записать сохраненное условие целостности в файл" << std::endl;
   std::cout << "5. Задать имя файла (путь) для чтения" << std::endl;
   std::cout << "6. Задать имя файла (путь) для записи" << std::endl;
   std::cout << "7. Задать параметры для алгоритма" << std::endl;
   std::cout << "9. Очистить консоль" << std::endl;
   std::cout << "0. Выход" << std::endl;
   std::cout << "Выберите пункт: ";
}

// ============================== Main ==============================
int main(int argc, const char* argv[])
{
   SetConsoleOutputCP(1251);
   SetConsoleCP(1251);

   // Получение имени файла
   std::string fileNameInput;
   std::string fileNameOutput;

   if (argc > 2)
   {
      fileNameInput = argv[1];
      fileNameOutput = argv[2];
   }
   else if (argc > 1)
      fileNameInput = argv[1];

   // Объявление генетического алгоритма
   CGeneticAlgorithm algor;

   std::string strError;
   int choice;
   bool running = true;

   unsigned int countIndividuals = 100;
   unsigned int percent = 0;
   size_t countIterations = 100;
   bool useMutation = false;

   // Меню
   while (running) {
      showMenu();
      std::cin >> choice;

      switch (choice)
      {
      case 1: // Заполнение данных для алгоритма из файла

         if (fileNameInput.empty())
            GetFileName(fileNameInput);

         if (!algor.FillDataInFile(fileNameInput, strError))
            std::cout << strError << std::endl;
         else
            std::cout << "Успешно." << std::endl;

         break;

      case 2: // Запуск алгоритма
         
         try
         {
            algor.Start(countIndividuals, countIterations, useMutation, percent);
         }
         catch (const std::string& strError)
         {
            std::cout << strError << std::endl;
         }
         break;

      case 3: // Запись поколений в файл

         if (fileNameOutput.empty())
            GetFileName(fileNameOutput);

         if (!algor.WriteGenerationsInFile(fileNameOutput, strError))
            std::cout << strError << std::endl;
         else
            std::cout << "Успешно." << std::endl;

         break;

      case 4: // Запись сохраненных условий целостности в файл

         if (fileNameOutput.empty())
            GetFileName(fileNameOutput);

         if (!algor.WriteСonditionIntegrityInFile(fileNameOutput, strError))
            std::cout << strError << std::endl;
         else
            std::cout << "Успешно." << std::endl;

         break;

      case 5: // Считать имя файла для чтения

         GetFileName(fileNameInput);

         break;

      case 6: // Считать имя файла для записи

         GetFileName(fileNameOutput);

         break;

      case 7: // Получение параметров для алгоритма

         std::cout << "Количество особей: ";
         std::cin >> countIndividuals;
         std::cout << "Количество итераций: ";
         std::cin >> countIterations;
         std::cout << "Использовать мутации (0 - нет, 1 - да): ";
         std::cin >> useMutation;

         if (useMutation)
         {
            std::cout << "Процент мутаций (%): ";
            std::cin >> percent;
         }

         break;

      case 9: // Очистить консоль

         system("cls");

         break;

      case 0: // Выход

         std::cout << "Выход из программы..." << std::endl;
         running = false;
         break;

      default:

         std::cout << "Неверный выбор. Попробуйте снова." << std::endl;
         break;
      }

      std::cout << std::endl;
   }
}
// ============================== End main ==============================