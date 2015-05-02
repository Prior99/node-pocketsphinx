{
  "targets": [
    {
      "target_name": "PocketSphinx",
      "cflags": ["-DMODELDIR=\"<!(pkg-config --variable=modeldir pocketsphinx)\"", "<!(pkg-config --cflags pocketsphinx sphinxbase)"],
      "libraries": ["<!(pkg-config --libs pocketsphinx sphinxbase)"],
      "xcode_settings": {
        "OTHER_CFLAGS": ["<!(pkg-config --cflags pocketsphinx sphinxbase)"],
        "OTHER_LDFLAGS": ["<!(pkg-config --libs pocketsphinx sphinxbase)"],
      },
      "include_dirs": [
        "<!(node -e \"require('nan')\")"
      ],
      "sources": [ "src/Factory.cpp", "src/Recognizer.cpp" ]
    }
  ]
}
