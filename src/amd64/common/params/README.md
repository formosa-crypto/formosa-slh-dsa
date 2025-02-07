# params

To generate the parameter files, run the following commands in the current directory:

```
cp ../../../../submodules/sphincsplus/ref/params/params-sphincs-shake-*.h .
cp ../../../../submodules/sphincsplus/ref/shake_offsets.h . 
sed -i 's|#include "../shake_offsets.h"|#include "shake_offsets.h"|' *.h
make -C auto/ all clean
rm *.h
```