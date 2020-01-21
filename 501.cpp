
// # Problem
// The eight divisors of 24 are 1, 2, 3, 4, 6, 8, 12 and 24.The ten numbers not exceeding 100 having exactly eight divisors are 24, 30, 40, 42, 54, 56, 66, 70, 78 and 88.
// Let f(n) be the count of numbers not exceeding n with exactly eight divisors.You are given f(100) = 10, f(1000) = 180 and f(10^6) = 224427.Find f(10^12).
#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>

bool hasEightDivisors(unsigned long long n)
{
  unsigned int count = 2; // always at least two divisors: 1 and n
  for (unsigned long long i = 2; i*i <= n; i++)
    if (n % i == 0) // found a divisor ?
    {
      if (i*i == n)
        count++;    // perfect square
      else
        count += 2; // i is a divisor and n/i as well

      // abort early
      if (count > 8)
        return false;
    }

  // exactly 8 divisors ?
  return count == 8;
}

// return the number of primes up to n
std::vector<unsigned int> primes; // will contain all primes up to sqrt(n)
unsigned long long countPrimes(unsigned long long n)
{
    // - find quick result for small n (when primes[] contains enough numbers)

  // if primes[] contains enough elements then run a fast binary search
  if (!primes.empty() && primes.back() > n)
  {
    // find smallest number larger than n
    auto i = std::upper_bound(primes.begin(), primes.end(), n);
    return std::distance(primes.begin(), i);
  }

  auto v = (unsigned int)sqrt(n);
  // about sqrt(n) * 12 bytes, for n = 10^12 => 12 MByte plus primes[]
  std::vector<unsigned long long> higher(v+2, 0);
  std::vector<unsigned int>       lower (v+2, 0);
  std::vector<bool>               used  (v+2, false);

  // assume all numbers are prime numbers
  unsigned long long result = n - 1;
  // the remaining lines subtract composites until result contains the number of primes

  // set up lower and upper bound
  for (unsigned int p = 2; p <= v; p++)
  {
    lower [p] = p - 1;
    higher[p] = n / p - 1;
  }

  for (unsigned int p = 2; p <= v; p++)
  {
    // composite ?
    if (lower[p] == lower[p - 1])
      continue;

    // store prime numbers (if not already existing)
    if (primes.empty() || p > primes.back())
      primes.push_back(p);

    auto temp  = lower[p - 1];
    // remove more composites
    result -= higher[p] - temp;

    auto pSquare = (unsigned long long)p * p;
    auto end = std::min<unsigned long long>(v, n / pSquare);
    // alternate between 1 and 2
    auto j = 1 + (p & 1);

    // adjust upper bound
    for (auto i = p + j; i <= end + 1; i += j)
    {
      if (used[i])
        continue;

      auto d = i * p;
      if (d <= v)
        higher[i] -= higher[d]    - temp;
      else
        higher[i] -= lower[n / d] - temp;
    }

    // adjust lower bound
    for (auto i = v; i >= pSquare; i--)
      lower[i] -= lower[i / p] - temp;

    // cross off multiples
    for (auto i = pSquare; i <= end; i += p*j)
      used[i] = true;
  }

  return result;
}

// count all primes with exactly 8 divisors
unsigned long long fast(unsigned long long n)
{
  countPrimes(n); 
  // a * b * c where a < b < c
  unsigned long long countABC = 0;
  for (size_t indexA = 0; indexA < primes.size(); indexA++)
  {
    unsigned long long a = primes[indexA];
    if (a * a * a > n)
      break;

    for (size_t indexB = indexA + 1; indexB < primes.size(); indexB++)
    {
      auto b = primes[indexB];

      // min(c) = next prime after b
      // max(c) = last prime before n / (a*b)
      auto maxC = n / (a * b);
      if (maxC <= b)
        break;

      // count all primes between min(c) and max(c)
      auto high = countPrimes(maxC);
      auto low  = indexB + 1; // same as countPrimes(b);
      countABC += high - low;
    }
  }

  // a^3 * b
  unsigned long long countA3B = 0;
  for (auto a : primes)
  {
    auto maxB = n / ((unsigned long long)a * a * a);
    if (maxB <= 1)
      break;

    // b can be any prime
    auto numB = countPrimes(maxB);
    // but b must differ from a
    if (maxB >= a)
      numB--;

    countA3B += numB;
  }

  // a^7
  unsigned long long countA7 = 0;
  for (auto a : primes)
  {
    if ((unsigned long long) a * a * a * a * a * a * a > n)
      break;
    countA7++;
  }

  return countABC + countA3B + countA7;
}

int main()
{
  auto limit = 1000000000000ULL;
  std::cin >> limit;

  // find all number with naive approach (= extremely slow)
  //unsigned long long count = 0;
  //for (unsigned long long i = 1; i <= limit; i++)
  //  if (hasEightDivisors(i))
  //    count++;
  //std::cout << count << std::endl;

  std::cout << fast(limit) << std::endl;
  return 0;
}