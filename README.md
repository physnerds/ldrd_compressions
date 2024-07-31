# compressiontests

## Repository to test compression of waveform like data with different compression algorithms. Starting with SZ3

## Requirements

## Creat Conda environment and install all dependencies.

1. In LCRC machines you should be able to find conda or anaconda. 

(I would recommend to install cmake before installing anything else so that there are no conflicts down the line between ROOT build and other builds 
```https://anaconda.org/anaconda/cmake```
)
2. ROOT installation via Conda is given here (https://root.cern/install/#install-via-a-package-manager)
It also gives example on how to create a conda environment inside which you can install all the required dependencies. 

3. On the same environment (that you created when you installed ROOT), install HDF5.
https://anaconda.org/anaconda/hdf5

4. On the same environment install protobuff via conda (needed by MGARD)


5. You will need h5py, a python API for HDF5.
```pip3 install h5py```

## Next we install the compression frameworks

Note, for all compression frameworks build, it would be easier down the line if the build files are copied to the respective install directory. 

5. SZ3 (https://github.com/szcompressor/SZ3)
   Build SZ3 with bundled in zstd compression for the sake of simplicity. See the configure file in SZ3 repository for more information on installation.

6. MGARD (https://github.com/CODARcode/MGARD.git) 
   Follow the readme document to install MGARD. 

7. IDEALEM (https://datagrid.lbl.gov/idealem/)
   You need to ask access to IDEALEM by giving your information. The whole process from requesting access to getting the tarfile of the code takes less than 10 minutes.

## Clone the test framework

```git clone -b feature/lcrc https://github.com/physnerds/compressiontests.git
cd compressiontests```

## Assuming you built and installed all the compression frameworks  and related dependencies, use cmake to build the framework.

Assuming you are in ```tests``` directory
```mkdir build; cd build```

```cmake .. -DZSTD_LIBRARIES=/path/to/compressiontests/SZ3_install/lib64/libzstd.so -DCMAKE_PREFIX_PATH=/path/to/compressiontests/SZ3_install/lib64/cmake -Dmgard_DIR=/path/to/compressiontests/MGARD_install/lib64/cmake/mgard -DSZ_INCLUDE_DIRS=/path/to/compressiontests/SZ3_install/include -DSZ_LIBRARIES=/path/to/compressiontests/SZ3_install/lib64/libSZ3c.so -DIDEALEM_INCLUDE_DIRS=/path/to/compressiontests/idealem-package/build/include -DIDEALEM_LIBRARIES=/path/to/compressiontests/idealem-package/build/lib/libidealem_cxx.a```

And compile the code:
```make```

## Test Codes

1. write_hdf5data.py: This generates synthetic waveform like data (which are basically superposition of sine and cosine waves) and writes into HDF5

2. analyze_hdf5.ipynb: This is a jupyter-notebook notebook that shows how to draw waveform like data from the HDF5 file and do some low-level inspections (print content, size of the data-sets etc)

3. mytest.cxx: A standalone test code that uses SZ3 to compress and decompress the synthetic waveform data, calculate KS value, compression ratio and draw original and decompressed waveform data. 
TODO: Use the compression_interface tool SZ3CompressionTools.cxx to compress and decompress the data instead.

4. mytest_mgard.cxx: A test code that uses the APIs of the compression framework (based on MGARD) to do compression and decompression of the synthetic waveform data. TODO: Check out how plots of original and decompressed data are done in example 3 and try to reimplement it here.

5. mytest_idealem_RNTUPLE.cxx: A test code that compressed and decompressed the synthetic data and writes the compressed data and decompressed data in RNTuple. 
TODO: Use the compression_interface IDEALEMCompressionTools.hxx to do compression and decompression of the data instead.











