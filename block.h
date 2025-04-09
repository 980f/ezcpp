#pragma once

template<typename ContentType> struct Block {
  const size_t size;
  ContentType &content;

  bool isVacuous() const {
    return (size == 0) || (&content == nullptr);
  }

  bool operator !() const {
    return isVacuous();
  }

  /** @returns this as anonymous block of bytes */
  Block <uint8_t> raw() {
    return Block <uint8_t>{size *sizeof(ContentType),reinterpret_cast <uint8_t &>(content)};
  }

  /** @returns this as anonymous block of bytes */
  Block <const uint8_t> c_raw() const {
    return Block <const uint8_t>{size *sizeof(ContentType),reinterpret_cast <const uint8_t &>(content)};
  }
};
