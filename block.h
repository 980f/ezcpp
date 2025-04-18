#pragma once

/** simplified version of std::array
  for retrofitting old code that took pointer and size as pair */

template<typename ContentType> struct Block {
  //soon: private:
    size_t size {0};
    ContentType *content{nullptr};
  public:
    size_t quantity() const {
      return size;
    }
  
    Block& operator =(Block &&other) {
      size = other.size;
      content = other.content;
      return *this;
    }

    bool isVacuous() const {
      return (size == 0) || (content == nullptr);
    }

    bool operator !() const {
      return isVacuous();
    }

    ContentType& operator[](unsigned index) const {
      static ContentType hiddenbug{0};
      if (index < size) {
        return content[index];
      } else {
        return hiddenbug; //reference outside the index results in (potential) bleeding of one bug's value to another bug's reference.
      }
    }

    /** @returns this as anonymous block of bytes */
    Block <uint8_t> raw() {
      return Block <uint8_t> {size * sizeof(ContentType), reinterpret_cast <uint8_t *>(content)};
    }

    /** @returns this as anonymous block of bytes */
    Block <const uint8_t> c_raw() const {
      return Block <const uint8_t> {size * sizeof(ContentType), reinterpret_cast <const uint8_t *>(content)};
    }
};
