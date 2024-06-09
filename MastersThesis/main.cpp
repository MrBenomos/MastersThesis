#include <iostream>
#include <string>

#include "windows.h"
#include "genetic_algorithm.h"

inline void GetFileName(std::string& FileName_)
{
   std::cout << "Введте имя файла (путь): ";
   std::cin >> FileName_;
}

// ============================== Main ==============================
int main(int argc, const char* argv[])
{
   SetConsoleOutputCP(1251);
   SetConsoleCP(1251);

   // Получение имени файла
   std::string fileName;

   if (argc < 2)
      GetFileName(fileName);
   else
      fileName = argv[1];

   // Объявление генетического алгоритма
   CGeneticAlgorithm algor;

   // Заполнение данных для алгоритма из файла
   std::string strError;
   while (!algor.FillDataInFile(fileName, strError))
   {
      std::cout << strError << std::endl;
      GetFileName(fileName);
   }

   // Запуск алгоритма



   // Вывод результата в файл
   if (argc < 3)
   {
      std::cout << "Сохранение результата. ";
      GetFileName(fileName);
   }
   else
      fileName = argv[2];

   algor.CreateFirstGenerationRandom(5);

   while (!algor.WriteGenerationsInFile(fileName, strError))
   {
      std::cout << strError << std::endl;
      GetFileName(fileName);
   }
}
// ============================== End main ==============================