#include <iostream>
#include <sstream>

std::string
reverseOrderOfBytes(std::string hexadecimal) // Because the JavaScript Virtual
                                             // Machine is little-endian
{
  if (hexadecimal.size() % 2 != 0 || hexadecimal.size() < 2 ||
      !std::regex_match(hexadecimal, std::regex("^(\\d|[a-f])+$"))) {
    std::cerr << "Internal compiler error: Some part of the compiler has "
                 "attempted to reverse the bytes of \""
              << hexadecimal << "\", which doesn't make sense." << std::endl;
    std::exit(1);
  }
  std::string result = "\"";
  for (int i = hexadecimal.size() - 2; i >= 0; i -= 2)
    result += "\\" + hexadecimal.substr(i, 2);
  result += '"';
  return result;
}

std::string getCharVectorRepresentationOfCharacter(uint8_t Character) {
  std::stringstream stream;
  stream.flags(std::ios::hex);
  stream << unsigned(Character) // uint8_t can be a "char", and we don't want it
                                // to be treated as such.
         << std::flush;
  std::string fillWithZeros(stream.str());
  while (fillWithZeros.size() < 2)
    fillWithZeros = "0" + fillWithZeros;
  return reverseOrderOfBytes(fillWithZeros);
}

std::string getCharVectorRepresentationOfInteger16(uint16_t Integer16) {
  std::stringstream stream;
  stream.flags(std::ios::hex);
  stream << Integer16 << std::flush;
  std::string fillWithZeros(stream.str());
  while (fillWithZeros.size() < 4)
    fillWithZeros = "0" + fillWithZeros;
  return reverseOrderOfBytes(fillWithZeros);
}

std::string getCharVectorRepresentationOfInteger32(uint32_t Integer32) {
  std::stringstream stream;
  stream.flags(std::ios::hex);
  stream << Integer32 << std::flush;
  std::string fillWithZeros(stream.str());
  while (fillWithZeros.size() < 8)
    fillWithZeros = "0" + fillWithZeros;
  return reverseOrderOfBytes(fillWithZeros);
}

std::string getCharVectorRepresentationOfInteger64(uint64_t Integer64) {
  std::stringstream stream;
  stream.flags(std::ios::hex);
  stream << Integer64 << std::flush;
  std::string fillWithZeros(stream.str());
  while (fillWithZeros.size() < 16)
    fillWithZeros = "0" + fillWithZeros;
  return reverseOrderOfBytes(fillWithZeros);
}

std::string getCharVectorRepresentationOfDecimal32(float Decimal32) {
  std::stringstream stream;
  stream.flags(std::ios::hex);
  uint32_t Integer32 =
      *((uint32_t *)(&Decimal32)); // This is actually invalid in standard C++,
                                   // but all compilers I've tried accept this.
  stream << Integer32 << std::flush;
  std::string fillWithZeros(stream.str());
  while (fillWithZeros.size() < 8)
    fillWithZeros = "0" + fillWithZeros;
  return reverseOrderOfBytes(fillWithZeros);
}

std::string getCharVectorRepresentationOfDecimal64(double Decimal64) {
  std::stringstream stream;
  stream.flags(std::ios::hex);
  uint64_t Integer64 =
      *((uint64_t *)(&Decimal64)); // Have a better idea? Doing this in assembly
                                   // is even less portable.
  stream << Integer64 << std::flush;
  std::string fillWithZeros(stream.str());
  while (fillWithZeros.size() < 16)
    fillWithZeros = "0" + fillWithZeros;
  return reverseOrderOfBytes(fillWithZeros);
}
