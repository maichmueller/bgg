
#ifndef TQDM_REGISTER_H
#define TQDM_REGISTER_H

#include <vector>

class tqdm;

struct tqdm_register {
   static size_t levels;

   static size_t add_pbar() {
      auto temp = levels;
      levels += 1;
      return temp;
   }
   static void pop_pbar() {
      levels -= 1;
   }
};

#endif  // TQDM_REGISTER_H
