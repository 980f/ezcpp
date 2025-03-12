#pragma once

template <typename ContentType> struct Block {
  const size_t size;
  ContentType &content;
};
