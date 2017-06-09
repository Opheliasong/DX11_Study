#pragma once
#define RELEASE(x) {if(x) {x->Release(); x=0;}}
#define SHUTDOWN_AND_DELETE(x) {if(x) {x->Shutdown(); delete x; x=0;}}
#define OUT