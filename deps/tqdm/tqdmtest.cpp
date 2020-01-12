#include "include/tqdm/tqdm.h"

int main()
{
   int N = 2000;
   tqdm bar;
   bar.set_label("Outside");

   for(int i = 0; i < 100000000; i++) {
      bar.progress(i, 100000000);
   }
   bar.finish();
}