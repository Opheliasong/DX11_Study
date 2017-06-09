#pragma once
#define RELEASE(x) {if(x) {x->Release(); x=0;}}
#define OUT