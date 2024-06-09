#pragma once
#include <iostream>
#include <cstdlib>
#include <ctime>

class CRandom
{
public:

   CRandom(int start = 0, int end = RAND_MAX);

   int Generate();
   void SetBoundaries(int start, int end);

   // Позволяет при каждом новом запуске программы использовать новые числа
   static void UseNewNumbers();
   static void SetSeed(unsigned int seed);

private:

   bool IsCorrect();

   int m_start;
   int m_end;
};

// ======================================== Методы ========================================


inline CRandom::CRandom(int start, int end) : m_start(start), m_end(end)
{
   if (!IsCorrect())
      throw;
}


inline int CRandom::Generate()
{
   return rand() % (m_end - m_start + 1) + m_start;
}


inline void CRandom::SetBoundaries(int start, int end)
{
   m_start = start;
   m_end = end;

   if (!IsCorrect())
      throw;
}


inline void CRandom::SetSeed(unsigned int seed)
{
   srand(seed);
}


inline void CRandom::UseNewNumbers()
{
   srand(static_cast<unsigned int>(time(nullptr)));
}


inline bool CRandom::IsCorrect()
{
   if (m_start > m_end)
      std::swap(m_start, m_end);

   if (m_end - m_start > RAND_MAX)
      return false;

   return true;
}