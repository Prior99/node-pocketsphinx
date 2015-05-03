#ifndef RECOGNIZER_H
#define RECOGNIZER_H

#include <v8.h>
#include <nan.h>
#include <node.h>
#include <node_buffer.h>
#include <pocketsphinx.h>
#include <sphinxbase/err.h>
#include <sphinxbase/jsgf.h>

class Recognizer : public node::ObjectWrap
{
public:
	static void Init(v8::Handle<v8::Object> exports);

private:
	explicit Recognizer();
	~Recognizer();

	static NAN_METHOD(New);

	static NAN_METHOD(Start);
	static NAN_METHOD(Stop);
	static NAN_METHOD(Restart);

	static NAN_METHOD(Write);
	static NAN_METHOD(WriteSync);

	static NAN_METHOD(AddKeyphraseSearch);
	static NAN_METHOD(AddKeywordsSearch);
	static NAN_METHOD(AddGrammarSearch);
	static NAN_METHOD(AddNgramSearch);

	static NAN_GETTER(GetSearch);
	static NAN_SETTER(SetSearch);

	static NAN_METHOD(FromFloat);

	static v8::Persistent<v8::Function> constructor;
	static void AsyncWorker(uv_work_t* request);
	static void AsyncAfter(uv_work_t* request);

	static v8::Local<v8::Value> Default(v8::Local<v8::Value> value, v8::Local<v8::Value> fallback);

	ps_decoder_t* ps;
	NanCallback *callback;
};

typedef struct AsyncData {
  Recognizer* instance;
  v8::Handle<v8::Value> exception;
  bool hasException;
  int16* data;
  size_t length;
  int32 score;
  const char* hyp;
} AsyncData;

#endif
