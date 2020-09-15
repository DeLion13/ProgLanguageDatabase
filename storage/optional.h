#pragma once
#if __cplusplus == 201703L
   #include <optional>  // C++17
   using std::optional;
   using std::nullopt;
#else
   #include <experimental/optional>  // C++11
   using std::experimental::optional;
   using std::experimental::nullopt;
#endif