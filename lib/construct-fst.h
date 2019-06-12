#pragma once
#include "fst-structs.h"

Transducer remap(Transducer& t, int lastState);
Transducer unionFST(Transducer first, Transducer second);
Transducer concatFST(Transducer first, Transducer second);
Transducer starFST(Transducer t);
Transducer initFST(std::string regex);
Transducer constructFST(std::string regex);
