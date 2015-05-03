#include <node.h>
#include "Recognizer.h"

using namespace v8;

Recognizer::Recognizer() {

}

Recognizer::~Recognizer() {
  ps_free(ps);
}

Persistent<Function> Recognizer::constructor;

void Recognizer::Init(Handle<Object> exports) {
  Local<FunctionTemplate> tpl = NanNew<FunctionTemplate>(New);
  tpl->SetClassName(NanNew("Recognizer"));
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  // Static Methods and Properties
  tpl->Set(NanNew("fromFloat"), NanNew<FunctionTemplate>(FromFloat)->GetFunction());
  tpl->Set(NanNew("modelDirectory"), NanNew(MODELDIR));

  // Prototype Methods and Properies
  tpl->PrototypeTemplate()->Set(NanNew("start"), NanNew<FunctionTemplate>(Start)->GetFunction());
  tpl->PrototypeTemplate()->Set(NanNew("stop"), NanNew<FunctionTemplate>(Stop)->GetFunction());
  tpl->PrototypeTemplate()->Set(NanNew("restart"), NanNew<FunctionTemplate>(Restart)->GetFunction());

  tpl->PrototypeTemplate()->Set(NanNew("addKeyphraseSearch"), NanNew<FunctionTemplate>(AddKeyphraseSearch)->GetFunction());
  tpl->PrototypeTemplate()->Set(NanNew("addKeywordsSearch"), NanNew<FunctionTemplate>(AddKeywordsSearch)->GetFunction());
  tpl->PrototypeTemplate()->Set(NanNew("addGrammarSearch"), NanNew<FunctionTemplate>(AddGrammarSearch)->GetFunction());
  tpl->PrototypeTemplate()->Set(NanNew("addNgramSearch"), NanNew<FunctionTemplate>(AddNgramSearch)->GetFunction());

  tpl->PrototypeTemplate()->SetAccessor(NanNew("search"), GetSearch, SetSearch);

  tpl->PrototypeTemplate()->Set(NanNew("write"), NanNew<FunctionTemplate>(Write)->GetFunction());
  tpl->PrototypeTemplate()->Set(NanNew("writeSync"), NanNew<FunctionTemplate>(WriteSync)->GetFunction());

  //constructor = Persistent<Function>::New(tpl->GetFunction());
  NanAssignPersistent(constructor, tpl->GetFunction());
  exports->Set(NanNew("PocketSphinx"), tpl->GetFunction());
  //module->Set(NanNew("exports"), constructor);
}

NAN_METHOD(Recognizer::New) {
  NanScope();

  if(!args.IsConstructCall()) {
    const int argc = 2;
    Local<Value> argv[argc] = { args[0], args[1] };
    Local<Function> cons = NanNew<Function>(constructor);
    NanReturnValue(cons->NewInstance(argc, argv));
  }

  if(args.Length() < 2) {
    NanThrowTypeError("Incorrect number of arguments, expected options and callback");
    NanReturnUndefined();
  }

  if(!args[0]->IsObject()) {
    NanThrowTypeError("Expected options to be an object");
    NanReturnUndefined();
  }

  if(!args[1]->IsFunction()) {
    NanThrowTypeError("Expected callback to be a function");
    NanReturnUndefined();
  }

  Recognizer* instance = new Recognizer();

  Handle<Object> options = args[0]->ToObject();
  //instance->callback = Persistent<Function>::New(Local<Function>::Cast(args[1]));
  instance->callback = new NanCallback(args[1].As<Function>());
  //NanAssignPersistent(instance->callback, NanNew<Function>(args[1]));

  cmd_ln_t* config = cmd_ln_init(NULL, ps_args(), TRUE,
    "-hmm", options->Get(NanNew("hmm"))->IsUndefined() ? MODELDIR "/en-us/en-us" : *NanAsciiString(options->Get(NanNew("hmm"))),
    "-dict", options->Get(NanNew("dict"))->IsUndefined() ?  MODELDIR "/en-us/cmudict-en-us.dict" : *NanAsciiString(options->Get(NanNew("dict"))),
    "-samprate", options->Get(NanNew("samprate"))->IsUndefined() ?  "44100" : *NanAsciiString(options->Get(NanNew("samprate"))),
    "-nfft", options->Get(NanNew("nfft"))->IsUndefined() ?  "2048" : *NanAsciiString(options->Get(NanNew("nfft"))),
    NULL);

  instance->ps = ps_init(config);

  instance->Wrap(args.This());

  NanReturnValue(args.This());
}

NAN_METHOD(Recognizer::AddKeyphraseSearch) {
  NanScope();
  Recognizer* instance = node::ObjectWrap::Unwrap<Recognizer>(args.This());

  if(args.Length() < 2) {
    NanThrowTypeError("Incorrect number of arguments, expected name and keyphrase");
    NanReturnValue(args.This());
  }

  if(!args[0]->IsString() || !args[1]->IsString()) {
    NanThrowTypeError("Expected both name and keyphrase to be strings");
    NanReturnValue(args.This());
  }

  NanAsciiString name(args[0]);
  NanAsciiString keyphrase(args[1]);

  int result = ps_set_keyphrase(instance->ps, *name, *keyphrase);
  if(result < 0)
    NanThrowError("Failed to add keyphrase search to recognizer");

  NanReturnValue(args.This());
}

NAN_METHOD(Recognizer::AddKeywordsSearch) {
  NanScope();
  Recognizer* instance = node::ObjectWrap::Unwrap<Recognizer>(args.This());

  if(args.Length() < 2) {
    NanThrowTypeError("Incorrect number of arguments, expected name and file");
    NanReturnValue(args.This());
  }

  if(!args[0]->IsString() || !args[1]->IsString()) {
    NanThrowTypeError("Expected both name and file to be strings");
    NanReturnValue(args.This());
  }

  NanAsciiString name(args[0]);
  NanAsciiString file(args[1]);

  int result = ps_set_kws(instance->ps, *name, *file);
  if(result < 0)
    NanThrowTypeError("Failed to add keywords search to recognizer");

  NanReturnValue(args.This());
}

NAN_METHOD(Recognizer::AddGrammarSearch) {
  NanScope();
  Recognizer* instance = node::ObjectWrap::Unwrap<Recognizer>(args.This());

  if(args.Length() < 2) {
    NanThrowTypeError("Incorrect number of arguments, expected name and file");
    NanReturnValue(args.This());
  }

  if(!args[0]->IsString() || !args[1]->IsString()) {
    NanThrowTypeError("Expected both name and file to be strings");
    NanReturnValue(args.This());
  }

  NanAsciiString name(args[0]);
  NanAsciiString file(args[1]);

  int result = ps_set_jsgf_file(instance->ps, *name, *file);
  if(result < 0)
    NanThrowError("Failed to add grammar search to recognizer");

  NanReturnValue(args.This());
}

NAN_METHOD(Recognizer::AddNgramSearch) {
  NanScope();
  Recognizer* instance = node::ObjectWrap::Unwrap<Recognizer>(args.This());

  if(args.Length() < 2) {
    NanThrowTypeError("Incorrect number of arguments, expected name and file");
    NanReturnValue(args.This());
  }

  if(!args[0]->IsString() || !args[1]->IsString()) {
    NanThrowTypeError("Expected both name and file to be strings");
    NanReturnValue(args.This());
  }

  NanAsciiString name(args[0]);
  NanAsciiString file(args[1]);

  int result = ps_set_lm_file(instance->ps, *name, *file);
  if(result < 0)
    NanThrowError("Failed to add Ngram search to recognizer");

  NanReturnValue(args.This());
}

NAN_GETTER(Recognizer::GetSearch) {
  NanScope();
  Recognizer* instance = node::ObjectWrap::Unwrap<Recognizer>(args.This());

  Local<Value> search = NanNew<String>(ps_get_search(instance->ps));

  NanReturnValue(search);
}

NAN_SETTER(Recognizer::SetSearch) {
  NanScope();
  Recognizer* instance = node::ObjectWrap::Unwrap<Recognizer>(args.This());

  NanAsciiString search(value);

  ps_set_search(instance->ps, *search);
}

NAN_METHOD(Recognizer::Start) {
  NanScope();
  Recognizer* instance = node::ObjectWrap::Unwrap<Recognizer>(args.This());

  int result = ps_start_utt(instance->ps);
  if(result)
    NanThrowError("Failed to start PocketSphinx processing");

  NanReturnValue(args.This());
}

NAN_METHOD(Recognizer::Stop) {
  NanScope();
  Recognizer* instance = node::ObjectWrap::Unwrap<Recognizer>(args.This());

  int result = ps_end_utt(instance->ps);
  if(result)
    NanThrowError("Failed to end PocketSphinx processing");

  NanReturnValue(args.This());
}

NAN_METHOD(Recognizer::Restart) {
  NanScope();
  Recognizer* instance = node::ObjectWrap::Unwrap<Recognizer>(args.This());

  int result = ps_start_utt(instance->ps);
  if(result)
    NanThrowTypeError("Failed to start PocketSphinx processing");

  result = ps_end_utt(instance->ps);
  if(result)
    NanThrowTypeError("Failed to restart PocketSphinx processing");

  NanReturnValue(args.This());
}

NAN_METHOD(Recognizer::Write) {
  NanScope();
  Recognizer* instance = node::ObjectWrap::Unwrap<Recognizer>(args.This());

  if(!args.Length()) {
    NanThrowTypeError("Expected a data buffer to be provided");
    NanReturnValue(args.This());
  }

  if(!node::Buffer::HasInstance(args[0])) {
    Local<Value> argv[1] = { Exception::Error(NanNew("Expected data to be a buffer")) };
    instance->callback->Call(1, argv);
    NanReturnValue(args.This());
  }

  AsyncData* data = new AsyncData();
  data->instance = instance;
  data->data = (int16*) node::Buffer::Data(args[0]);
  data->length = node::Buffer::Length(args[0]) / sizeof(int16);

  uv_work_t* req = new uv_work_t();
  req->data = data;

  uv_queue_work(uv_default_loop(), req, AsyncWorker, (uv_after_work_cb)AsyncAfter);

  NanReturnValue(args.This());
}

NAN_METHOD(Recognizer::WriteSync) {
  NanScope();
  Recognizer* instance = node::ObjectWrap::Unwrap<Recognizer>(args.This());

  if(!args.Length()) {
    NanThrowTypeError("Expected a data buffer to be provided");
    NanReturnValue(args.This());
  }

  if(!node::Buffer::HasInstance(args[0])) {
    Local<Value> argv[1] = { Exception::Error(NanNew("Expected data to be a buffer")) };
    instance->callback->Call(1, argv);
    NanReturnValue(args.This());
  }

  int16* data = (int16*) node::Buffer::Data(args[0]);
  size_t length = node::Buffer::Length(args[0]) / sizeof(int16);

  if(ps_process_raw(instance->ps, data, length, FALSE, FALSE) < 0) {
    Handle<Value> argv[1] = { Exception::Error(NanNew("Failed to process audio data")) };
    instance->callback->Call(1, argv);
    NanReturnValue(args.This());
  }

  int32 score;
  const char* hyp = ps_get_hyp(instance->ps, &score);
  Handle<Value> argv[3] = { NanNull(), hyp ? NanNew(hyp) : NanNull().As<v8::String>(), NanNew<Number>(score)};
  instance->callback->Call(3, argv);

  NanReturnValue(args.This());
}

void Recognizer::AsyncWorker(uv_work_t* request) {
  AsyncData* data = reinterpret_cast<AsyncData*>(request->data);

  if(ps_process_raw(data->instance->ps, data->data, data->length, FALSE, FALSE)) {
    data->hasException = TRUE;
    data->exception = Exception::Error(NanNew("Failed to process audio data"));
    return;
  }

  int32 score;
  const char* hyp = ps_get_hyp(data->instance->ps, &score);

  data->score = score;
  data->hyp = hyp;
}

void Recognizer::AsyncAfter(uv_work_t* request) {
  AsyncData* data = reinterpret_cast<AsyncData*>(request->data);

  if(data->hasException) {
    Handle<Value> argv[1] = { data->exception };
    data->instance->callback->Call(1, argv);
  } else {
    Handle<Value> argv[3] = { NanNull(), data->hyp ? NanNew(data->hyp) : NanNull().As<v8::String>(), NanNew<Number>(data->score)};
    data->instance->callback->Call(3, argv);
  }
}

Local<Value> Recognizer::Default(Local<Value> value, Local<Value> fallback) {
  if(value->IsUndefined()) return fallback;
  return value;
}

NAN_METHOD(Recognizer::FromFloat) {
  NanScope();

  if(!args.Length()) {
    NanThrowTypeError("Expected a data buffer to be provided");
    NanReturnValue(args.This());
  }

  if(!node::Buffer::HasInstance(args[0])) {
    NanThrowTypeError("Expected data to be a buffer");
    NanReturnValue(args.This());
  }

  /*float* data = reinterpret_cast<float*>(node::Buffer::Data(args[0]));
  size_t length = node::Buffer::Length(args[0]) / sizeof(float);

  node::Buffer *slowBuffer = node::Buffer::New(length * sizeof(int16));
  int16* slowBufferData = reinterpret_cast<int16*>(node::Buffer::Data(slowBuffer));*/

  NanReturnValue(args[0]);

  /*for(size_t i = 0; i < length; i++)
    slowBufferData[i] = data[i] * 32768;

  // Courtesy of http://sambro.is-super-awesome.com/2011/03/03/creating-a-proper-buffer-in-a-node-c-addon/
  Local<Object> globalObj = Context::GetCurrent()->Global();
  Local<Function> bufferConstructor = Local<Function>::Cast(globalObj->Get(String::New("Buffer")));
  Handle<Value> constructorArgs[3] = { slowBuffer->handle_, Integer::New(length), Integer::New(0) };
  Local<Object> actualBuffer = bufferConstructor->NewInstance(3, constructorArgs);
  NanReturnValue(actualBuffer);*/
}
