## Compression Interface
Write an interface that connects to RNTuple API.
It should have the following functionalities:

### Functionalities
It should have the following functionalities:

#### Compressor
Compress the original data
CompressData(orig_data,......)
First variadic template argument -->configuration parameter
Return compressed data
Save Configuration Parameter [json?]

#### Decompressor
Decompress the compressed data
DecompressData(comp_data,...)
Return decompressed data
First variadic template argument --> configuration parameter [json?]



