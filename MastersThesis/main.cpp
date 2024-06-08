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

   while (!algor.WriteDataInFile(fileName, strError))
   {
      std::cout << strError << std::endl;
      GetFileName(fileName);
   }


   //const size_t N = 1000;
   //int a[N] = { 0 };
   //for (; ; )
   //{
   //   //мутация в случайную сторону каждого элемента:
   //   for (size_t i = 0; i < N; ++i)
   //      a[i] += (rndm.Generate() == 1) ? 1 : -1;

   //   //теперь выбираем лучших, отсортировав по возрастанию
   //   std::sort(a, a + N);
   //   //и тогда лучшие окажутся во второй половине массива.
   //   //скопируем лучших в первую половину, куда они оставили потомство, а первые умерли:
   //   std::copy(a + N / 2, a + N, a);
   //   //теперь посмотрим на среднее состояние популяции. Как видим, оно всё лучше и лучше.
   //   std::cout << std::accumulate(a, a + N, 0) / N << std::endl;
   //}
}
// ============================== End main ==============================