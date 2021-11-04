#include <iostream>
#include <memory>
#include <string>

#include <emscripten.h>

namespace CPP
{

class Animal
{
public:
   explicit Animal(const char* name) : m_name(name), m_weight(0) {}
   void set_weight(int weight) {
      m_weight = weight;
   }

public:
   virtual void dump() = 0;
   virtual ~Animal() = default;

protected:
   std::string m_name;
   int m_weight;
};

class Dog: public Animal
{
public:
   explicit Dog(const char* name) : Animal(name) {}
   void dump() override {
      std::cout << "Dog name: " << m_name << ", weight: " << m_weight << std::endl;
   }
};

class Bird: public Animal
{
public:
   explicit Bird(const char* name) : Animal(name) {}
   void dump() override {
      std::cout << "Bird name: " << m_name << ", weight: " << m_weight << std::endl;
   }
};

} // CPP

EMSCRIPTEN_KEEPALIVE extern "C"
int CppPoly()
{
   std::unique_ptr<CPP::Animal> a_dog = std::make_unique<CPP::Dog>("wang wang");
   std::unique_ptr<CPP::Animal> a_bird = std::make_unique<CPP::Bird>("zha zha");

   a_dog->set_weight(10);
   a_bird->set_weight(2);

   a_dog->dump();
   a_bird->dump();

   return 1;
}

EMSCRIPTEN_KEEPALIVE extern "C"
const char* ConvertString(const char* input)
{
   static std::string s_str;
   s_str.clear();
   s_str.append(input);
   std::cout << "Append CPP to the string: " << s_str << std::endl;
   s_str.append(" CPP");
   std::cout << "Return: " << s_str << std::endl;
   return s_str.c_str();
}

