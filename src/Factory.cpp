#include <node.h>
#include "Recognizer.h"

using namespace v8;

extern "C" {
	void InitAll(Handle<Object> exports) {
		Recognizer::Init(exports);
	}

	NODE_MODULE(PocketSphinx, InitAll);
}
