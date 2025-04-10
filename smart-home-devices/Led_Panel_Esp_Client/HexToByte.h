#ifndef HEX_TO_BYTE
#define HEX_TO_BYTE

#include <exception>
#include <string>

/**
 * @brief Custom exception class for handling invalid hex characters.
 *
 * This exception is thrown when an invalid hex character is encountered during
 * the conversion of hexadecimal strings to byte arrays. It is used to signal
 * errors in the input data format, ensuring that only valid hex characters are
 * processed.
 */
class InvalidHexCharacterException : public std::exception {
public:
  const char* what() const noexcept override {
    return "Invalid hex character encountered.";
  }
};

namespace internal { // hide functions
  /**
  * @brief Converts a single hex character to a byte (8 bits).
  *
  * This function takes a hex character as input and converts it to its corresponding
  * byte value. It throws a InvalidHexCharacterException if the input character is not
  * a valid hex character.
  *
  * @param hexChar The hex character to convert.
  * @return The byte value corresponding to the hex character.
  * @throws InvalidHexCharacterException If the input character is not a valid hex character.
  */
  byte hexCharToByte(char hexChar) {
    hexChar = toupper(hexChar);  // Convert to uppercase to simplify
    if (hexChar >= '0' && hexChar <= '9') {
      return hexChar - '0';
    } else if (hexChar >= 'A' && hexChar <= 'F') {
      return hexChar - 'A' + 10;
    }
    throw InvalidHexCharacterException();  // Throw an exception for invalid hex characters
  }
}

/**
 * @brief Converts a hex string to a byte array.
 *
 * This function takes a hex string and converts it into a byte array. Each pair of
 * hex characters in the string is converted into a single byte in the array.
 *
 * @param hexString The hex string to convert.
 * @param byteArray The byte array to populate with the converted data.
 * @param byteArraySize The size of the byte array.
 */
void hexStringToByteArray(const String& hexString, byte* byteArray, const std::size_t byteArraySize) {
  for (std::size_t i = 0; i < byteArraySize; i++) {
    byte highNibble = internal::hexCharToByte(hexString[i * 2]);
    byte lowNibble = internal::hexCharToByte(hexString[i * 2 + 1]);
    byteArray[i] = (highNibble << 4) | lowNibble;
  }
}

#endif  //HEX_TO_BYTE